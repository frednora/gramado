// ahci.c
// AHCI driver - Basic version
// Created by Grok + Fred Nora style
// Following the same philosophy as ata.c

#include <kernel.h>


// Globals (your style)
int g_ahci_driver_initialized = FALSE;

volatile HBA_MEM *AHCI_HBA = NULL;   // Main HBA register base

struct ahci_port_d  ahci_port[NR_PORTS];
struct ahci_current_port_d  AHCICurrentPort;

// Next device ID
static uint32_t __next_ahci_id = 0;

struct ahci_device_d *ahci_ready_queue_dev = NULL;
struct ahci_device_d *current_ahci_dev = NULL;

// =======================================================

static void ahci_io_delay(void)
{
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

static void ahci_delay(int ms)
{
    int i=0;
    for (i = 0; i < ms * 1000; i++) {
        ahci_io_delay();
    }
}

// Enable AHCI mode and interrupts
static void ahci_enable(void)
{
    if (!AHCI_HBA)
        return;

    // Set AE (AHCI Enable) bit
    AHCI_HBA->ghc |= (1 << 31);
}

// Global HBA Reset
static void ahci_reset_hba(void)
{
    if (!AHCI_HBA)
        return;

    printk("AHCI: Resetting HBA...\n");
    AHCI_HBA->ghc |= (1 << 0);  // HR bit

    while (AHCI_HBA->ghc & (1 << 0))
        ahci_io_delay();

    printk("AHCI: HBA Reset done\n");
}


// Kernel-side AHCI port setup
static int ahci_setup_port(int port_num);
/*
static int ahci_setup_port(int port_num)
{
    if (port_num < 0 || port_num >= NR_PORTS)
        return -1;

    volatile HBA_PORT *port = &AHCI_HBA->ports[port_num];

    // Stop port: clear ST + FRE
    port->cmd &= ~((1<<0) | (1<<4));

    // Wait for CR + FR to clear
    while (port->cmd & ((1<<15) | (1<<14))) {
        ahci_io_delay();
    }

    // Allocate aligned memory for CLB and FB
    void *clb_va = kmalloc(1024);
    unsigned long clb_pa = virtual_to_physical(clb_va, gKernelPML4Address);
    memset(clb_va, 0, 1024);

    void *fb_va = kmalloc(256);
    unsigned long fb_pa = virtual_to_physical(fb_va, gKernelPML4Address);
    memset(fb_va, 0, 256);

    // Assign physical addresses to port registers
    port->clb  = (uint32_t) clb_pa;
    port->clbu = (uint32_t)(clb_pa >> 32);
    port->fb   = (uint32_t) fb_pa;
    port->fbu  = (uint32_t)(fb_pa >> 32);

    // Start port: enable FRE + ST
    port->cmd |= (1<<4);   // FRE
    port->cmd |= (1<<0);   // ST

    printk("AHCI: Port %d setup done\n", port_num);
    return 0;
}
*/

static int ahci_setup_port(int port_num)
{
    if (port_num < 0 || port_num >= NR_PORTS)
        return -1;

    volatile HBA_PORT *port = &AHCI_HBA->ports[port_num];
    struct ahci_port_d *pinfo = &ahci_port[port_num];

    // 1. Stop port
    port->cmd &= ~((1u << 0) | (1u << 4));  // Clear ST + FRE
    while (port->cmd & ((1u << 15) | (1u << 14)))   // Wait CR + FR
        ahci_io_delay();

    // 2. Allocate contiguous aligned memory
    //pinfo->mem = (AHCI_PORT_MEMORY*) kmalloc_aligned(sizeof(AHCI_PORT_MEMORY), 1024);

    // =============================================
    // Allocate memory (9472 bytes)
    // =============================================
    size_t mem_size = sizeof(AHCI_PORT_MEMORY);   // should be 9472 bytes


    // Allocate a bit more so we can align it
    void *raw = kmalloc(mem_size + 1024);
    if (!raw)
    {
        printk("AHCI: Out of memory for port %d\n", port_num);
        return -2;
    }
    // Align to 1024 bytes (required for Command List)
    unsigned long va_aligned = ((unsigned long)raw + 1023) & ~0x3FFULL;

    //pinfo->mem = (AHCI_PORT_MEMORY*) kmalloc(sizeof(AHCI_PORT_MEMORY), 1024);
    pinfo->mem = (AHCI_PORT_MEMORY*) va_aligned;
    if (!pinfo->mem)
        return -2;

    memset(pinfo->mem, 0, mem_size);   // use mem_size instead of sizeof


    pinfo->mem_pa = virtual_to_physical((unsigned long)pinfo->mem, gKernelPML4Address);
    pinfo->port_num = port_num;
    pinfo->initialized = TRUE;

    // 3. Setup CLB (Command List Base)
    port->clb  = (uint32_t) pinfo->mem_pa;
    port->clbu = (uint32_t)(pinfo->mem_pa >> 32);

    // 4. Setup FB (FIS Base)
    unsigned long fis_pa = pinfo->mem_pa + offsetof(AHCI_PORT_MEMORY, fis);
    port->fb   = (uint32_t) fis_pa;
    port->fbu  = (uint32_t)(fis_pa >> 32);

    // 5. Link Command Headers to Command Tables
    int i=0;
    for (i=0; i < 32; i++)
    {
        unsigned long tbl_pa = pinfo->mem_pa + offsetof(AHCI_PORT_MEMORY, cmd_tbl[i]);

        pinfo->mem->cmd_list[i].ctba  = (uint32_t) tbl_pa;
        pinfo->mem->cmd_list[i].ctbau = (uint32_t)(tbl_pa >> 32);
        pinfo->mem->cmd_list[i].prdtl = 1;        // We use 1 PRDT for now
    }

    // 6. Clear port interrupts
    port->is = 0xFFFFFFFF;

    // 7. Start port
    port->cmd |= (1u << 4);   // FRE
    while (!(port->cmd & (1u << 14))) ahci_io_delay();  // Wait FR

    port->cmd |= (1u << 0);   // ST
    while (!(port->cmd & (1u << 15))) ahci_io_delay();  // Wait CR

    printk("AHCI: Port %d initialized successfully\n", port_num);
    return 0;
}


// =======================================================
// Port helper functions
// =======================================================

static int ahci_port_start(int port_num)
{
    if (port_num < 0 || port_num >= NR_PORTS)
        return -1;

    volatile HBA_PORT *port = &AHCI_HBA->ports[port_num];

    // Wait until CR (Command List Running) is cleared
    while (port->cmd & (1 << 15))   // CR bit
        ahci_io_delay();

    // Start port
    port->cmd |= (1 << 0);   // ST bit

    return 0;
}

static void ahci_port_stop(int port_num)
{
    if (port_num < 0 || port_num >= NR_PORTS)
        return;

    volatile HBA_PORT *port = &AHCI_HBA->ports[port_num];

    port->cmd &= ~(1 << 0);  // Clear ST

    while (port->cmd & (1 << 15))   // Wait CR
        ahci_io_delay();
}

// =======================================================
// Identify device on a port (very similar to your __ide_identify_device)
// =======================================================

static int ahci_identify_device(int port_num)
{
    if (port_num < 0 || port_num >= NR_PORTS)
        return -1;

    volatile HBA_PORT *port = &AHCI_HBA->ports[port_num];

    // Check if device is present
    if (port->sig == 0 || port->sig == 0xFFFFFFFF)
        return -1;

    printk("AHCI Port %d: Signature 0x%X\n", port_num, port->sig);

    // TODO: Allocate command list, FIS, command table here (next step)

    // For now just mark the port as detected
    ahci_port[port_num].todo00 = 1234;  // placeholder

    return 0;
}

// =======================================================
// Probe all implemented ports
// =======================================================

static void ahci_probe_ports(void)
{
    int i=0;
    uint32_t pi = AHCI_HBA->pi;

    printk("AHCI: Ports Implemented = 0x%X\n", pi);

    for (i = 0; i < NR_PORTS; i++)
    {
        if (pi & (1 << i))
        {
            ahci_identify_device(i);
        }
    }
}

static void ahci_flush_cache(void *va, size_t size)
{
/*
 //#todo
    unsigned long addr = (unsigned long)va;
    unsigned long end = addr + size;

    // CLFLUSH + MFENCE (works on x86_64)
    for (unsigned long p = addr; p < end; p += 64)
    {
        asm volatile("clflush (%0)" : : "r"(p) : "memory");
    }
    asm volatile("mfence" ::: "memory");
*/
}

// =====================================================
// Basic AHCI Read Sector (for kernel)
// =====================================================



// =====================================================
// Basic AHCI Read Sector (Updated for new structures)
// =====================================================
// IN: port, lba. buffer, sector_count
/*
// good
int ahci_read_sector(int port, uint64_t lba, void *buffer_va, uint32_t sector_count)
{
    if (port < 0 || port >= NR_PORTS || !AHCI_HBA || !buffer_va || sector_count == 0)
        return -1;

    struct ahci_port_d *pinfo = &ahci_port[port];
    if (!pinfo->initialized || !pinfo->mem)
    {
        printk("AHCI: Port %d not initialized\n", port);
        return -1;
    }

    volatile HBA_PORT *p = &AHCI_HBA->ports[port];

    printk("ahci_read_sector: Port %d, LBA %llu, sectors %u\n", port, lba, sector_count);

    // Wait for port to be ready (BSY + DRQ)
    while (p->tfd & (ATA_SR_BSY | ATA_SR_DRQ))
        ahci_io_delay();

    // Use command slot 0
    HBA_CMD_HEADER *cmd_hdr = &pinfo->mem->cmd_list[0];
    HBA_CMD_TBL    *cmd_tbl = &pinfo->mem->cmd_tbl[0];

    // Clear structures
    memset(cmd_hdr, 0, sizeof(HBA_CMD_HEADER));
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL));

    // Setup Command Header
    cmd_hdr->cfl   = sizeof(FIS_REG_H2D) / sizeof(uint32_t);  // 5 dwords
    cmd_hdr->w     = 0;      // 0 = Read
    cmd_hdr->prdtl = 1;      // 1 PRDT entry

    // === Build Command FIS ===
    FIS_REG_H2D *fis = (FIS_REG_H2D*)&cmd_tbl->cfis;
    memset(fis, 0, sizeof(FIS_REG_H2D));

    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c        = 1;                          // Command
    fis->command  = ATA_CMD_READ_DMA_EXT;       // 0x25
    fis->device   = (1 << 6);                   // LBA mode

    // LBA48
    fis->lba0 = (uint8_t)(lba >> 0);
    fis->lba1 = (uint8_t)(lba >> 8);
    fis->lba2 = (uint8_t)(lba >> 16);
    fis->lba3 = (uint8_t)(lba >> 24);
    fis->lba4 = (uint8_t)(lba >> 32);
    fis->lba5 = (uint8_t)(lba >> 40);

    fis->countl = (uint8_t)(sector_count);
    fis->counth = (uint8_t)(sector_count >> 8);

    // === PRDT - Physical Address! ===
    unsigned long buf_pa = virtual_to_physical((unsigned long)buffer_va, gKernelPML4Address);

    cmd_tbl->prdt_entry[0].dba  = (uint32_t) buf_pa;
    cmd_tbl->prdt_entry[0].dbau = (uint32_t)(buf_pa >> 32);
    cmd_tbl->prdt_entry[0].dbc  = (sector_count * 512) - 1;
    cmd_tbl->prdt_entry[0].i    = 1;   // Interrupt on completion

    // Clear pending interrupts
    p->is = 0xFFFFFFFF;

    // Issue command on slot 0
    p->ci = (1 << 0);

    // Wait for completion (polling)
    printk("AHCI: Waiting for command completion...\n");
    while (p->ci & 1)
    {
        if (p->is & HBA_PxIS_TFES)
        {
            printk("AHCI Read Error: TFES (Task File Error)\n");
            return -1;
        }
        if (p->is & HBA_PxIS_HBFS)
        {
            printk("AHCI Read Error: HBFS (Host Bus Fatal Error)\n");
            return -1;
        }
    }

    // Check final status
    if (p->tfd & ATA_SR_ERR)
    {
        printk("AHCI Read Error: TFD error (0x%X)\n", p->tfd);
        return -1;
    }

    printk("ahci_read_sector: Success! (LBA %llu)\n", lba);
    return 0;
}
*/


/*
int ahci_read_sector(int port, uint64_t lba, void *buffer_va, uint32_t sector_count)
{
    if (port < 0 || port >= NR_PORTS || !AHCI_HBA || !buffer_va || sector_count == 0)
        return -1;

    struct ahci_port_d *pinfo = &ahci_port[port];
    if (!pinfo->initialized || !pinfo->mem)
    {
        printk("AHCI: Port %d not initialized\n", port);
        return -1;
    }

    volatile HBA_PORT *p = &AHCI_HBA->ports[port];

    printk("=== AHCI READ === Port %d | LBA %d | Sectors %d | Buf VA %x\n", 
           port, lba, sector_count, buffer_va);

    // Wait for port ready
    while (p->tfd & (ATA_SR_BSY | ATA_SR_DRQ))
        ahci_io_delay();

    // Use slot 0
    HBA_CMD_HEADER *cmd_hdr = &pinfo->mem->cmd_list[0];
    HBA_CMD_TBL    *cmd_tbl = &pinfo->mem->cmd_tbl[0];

    memset(cmd_hdr, 0, sizeof(HBA_CMD_HEADER));
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL));

    // Command Header
    cmd_hdr->cfl   = sizeof(FIS_REG_H2D) / sizeof(uint32_t);  // 5
    cmd_hdr->w     = 0;      // Read
    cmd_hdr->prdtl = 1;

    // Command FIS
    FIS_REG_H2D *fis = (FIS_REG_H2D*)cmd_tbl->cfis;
    memset(fis, 0, sizeof(FIS_REG_H2D));

    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c        = 1;
    fis->command  = ATA_CMD_READ_DMA_EXT;
    fis->device   = (1 << 6);   // LBA mode

    fis->lba0 = (lba >> 0)  & 0xFF;
    fis->lba1 = (lba >> 8)  & 0xFF;
    fis->lba2 = (lba >> 16) & 0xFF;
    fis->lba3 = (lba >> 24) & 0xFF;
    fis->lba4 = (lba >> 32) & 0xFF;
    fis->lba5 = (lba >> 40) & 0xFF;

    fis->countl = sector_count & 0xFF;
    fis->counth = (sector_count >> 8) & 0xFF;

    // === CRITICAL: Physical address of buffer ===
    unsigned long buf_pa = virtual_to_physical((unsigned long)buffer_va, gKernelPML4Address);
    printk("Buffer VA: %x  ->  PA: 0x%x\n", buffer_va, buf_pa);

    cmd_tbl->prdt_entry[0].dba  = (uint32_t) buf_pa;
    cmd_tbl->prdt_entry[0].dbau = (uint32_t)(buf_pa >> 32);
    cmd_tbl->prdt_entry[0].dbc  = (sector_count * 512) - 1;
    cmd_tbl->prdt_entry[0].i    = 1;

    // Clear interrupts
    p->is = 0xFFFFFFFF;

    // Issue command
    p->ci = 1 << 0;

    // Wait for completion
    while (p->ci & 1)
    {
        if (p->is & HBA_PxIS_TFES)
        {
            printk("AHCI: TFES Error!\n");
            return -1;
        }
        if (p->is & HBA_PxIS_HBFS)
        {
            printk("AHCI: HBFS Error!\n");
            return -1;
        }
    }

    if (p->tfd & ATA_SR_ERR)
    {
        printk("AHCI: Final TFD Error = 0x%X\n", p->tfd);
        return -1;
    }

    // === DEBUG: Check first bytes and signature ===
    uint8_t *buf = (uint8_t*)buffer_va;
    printk("First 8 bytes : %x %x %x %x %x %x %x %x\n",
           buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    printk("Signature (510-511): %x %x\n", buf[510], buf[511]);

    return 0;
}
*/

int ahci_read_sector(int port, uint64_t lba, void *buffer_va, uint32_t sector_count)
{
    if (port < 0 || port >= NR_PORTS || !AHCI_HBA || !buffer_va || sector_count == 0)
        return -1;

    struct ahci_port_d *pinfo = &ahci_port[port];
    if (!pinfo->initialized || !pinfo->mem)
    {
        printk("AHCI: Port %d not initialized\n", port);
        return -1;
    }

    volatile HBA_PORT *p = &AHCI_HBA->ports[port];

    printk("=== AHCI READ === Port %d | LBA %d | Buf VA %x\n", port, lba, buffer_va);

    // Wait for port ready
    while (p->tfd & (ATA_SR_BSY | ATA_SR_DRQ))
        ahci_io_delay();

    HBA_CMD_HEADER *cmd_hdr = &pinfo->mem->cmd_list[0];
    HBA_CMD_TBL    *cmd_tbl = &pinfo->mem->cmd_tbl[0];

    memset(cmd_hdr, 0, sizeof(HBA_CMD_HEADER));
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL));

    cmd_hdr->cfl   = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
    cmd_hdr->w     = 0;
    cmd_hdr->prdtl = 1;

    FIS_REG_H2D *fis = (FIS_REG_H2D*)cmd_tbl->cfis;
    memset(fis, 0, sizeof(FIS_REG_H2D));

    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c        = 1;
    fis->command  = ATA_CMD_READ_DMA_EXT;
    fis->device   = (1 << 6);

    fis->lba0 = (lba >> 0)  & 0xFF;
    fis->lba1 = (lba >> 8)  & 0xFF;
    fis->lba2 = (lba >> 16) & 0xFF;
    fis->lba3 = (lba >> 24) & 0xFF;
    fis->lba4 = (lba >> 32) & 0xFF;
    fis->lba5 = (lba >> 40) & 0xFF;

    fis->countl = sector_count & 0xFF;
    fis->counth = (sector_count >> 8) & 0xFF;

    unsigned long buf_pa = virtual_to_physical((unsigned long)buffer_va, gKernelPML4Address);
    printk("Buffer PA: 0x%x\n", buf_pa);

    // === PRDT ===
    cmd_tbl->prdt_entry[0].dba  = (uint32_t)buf_pa;
    cmd_tbl->prdt_entry[0].dbau = (uint32_t)(buf_pa >> 32);
    cmd_tbl->prdt_entry[0].dbc  = (sector_count * 512) - 1;
    cmd_tbl->prdt_entry[0].i    = 1;

    p->is = 0xFFFFFFFF;

    // Flush before DMA
    // ahci_flush_cache(buffer_va, sector_count * 512);

    p->ci = 1 << 0;

    // Wait
    while (p->ci & 1)
    {
        if (p->is & (HBA_PxIS_TFES | HBA_PxIS_HBFS))
        {
            printk("AHCI Error! IS=0x%x\n", p->is);
            return -1;
        }
    }

    // Flush after DMA
    //ahci_flush_cache(buffer_va, sector_count * 512);

    if (p->tfd & ATA_SR_ERR)
    {
        printk("TFD Error = 0x%x\n", p->tfd);
        return -1;
    }

    // Show result
    uint8_t *b = (uint8_t*)buffer_va;
    printk("First 16 bytes: ");
    int i=0;
    for(i=0; i<16; i++) printk("%x ", b[i]);
    printk("\nSignature 510-511: %x %x\n", b[510], b[511]);

    return 0;
}


/*
void ahci_test_read(void)
{
    int port = 0;  // Assuming port 0 has the boot disk for testing
    int lba = 0;   // Read sector 0 (MBR)
    int NumberOfSectors = 1;

    printk("ahci_test_read: Testing read from port %d, LBA %d\n", port, lba);

    // Test read sector 0 (MBR)
    unsigned char *test_buf = (unsigned char*) kmalloc(512);
    if (test_buf)
    {
        memset(test_buf, 0, 512);

        // OK. signature was found
        // IN: port, lba. buffer, sector_count
        if (ahci_read_sector(port, lba, test_buf, NumberOfSectors) == 0)
        {
            printk("ahci_test_read: AHCI Read Sector 0 OK! Signature: %x %x\n", 
                   test_buf[510], test_buf[511]);
        }
        else
        {
            printk("AHCI Read failed!\n");
        }
    }
}
*/

void ahci_test_read(void)
{
    // Use a page that is identity-mapped or marked as uncached if possible
    unsigned char *test_buf = (unsigned char*) kmalloc(512);
    if (!test_buf) return;

    memset(test_buf, 0xFE, 512);   // obvious garbage

    printk("Before: First byte = %x\n", test_buf[0]);

    if (ahci_read_sector(0, 0ULL, test_buf, 1) == 0)
    {
        printk("After read - Signature: %x %x\n", test_buf[510], test_buf[511]);
        
        if (test_buf[510] == 0x55 && test_buf[511] == 0xAA)
            printk("MBR signature correct!\n");
        else
            printk("Still wrong (probably cache issue)\n");
    }
}



// =======================================================
// Main Initialization (mirroring DDINIT_ata)
// =======================================================

// ahci.c
// ... (keep all previous functions)

int DDINIT_ahci(
    struct pci_device_d *pci_ahci,
    uint8_t controller_type )
{
    PROGRESS("DDINIT_ahci:\n");

    printk("DDINIT_ahci:\n");

    if ((void*)pci_ahci == NULL)
    {
        printk("DDINIT_ahci: pci_ahci == NULL\n");
        return -1;
    }

    // Check controller type
    if (controller_type != STORAGE_CONTROLLER_MODE_AHCI)
    {
        printk("DDINIT_ahci: Wrong controller type\n");
        return -1;
    }

    // BAR5 is the AHCI base address
    unsigned long bar5 = pci_ahci->BAR5 & ~0xF;
    BootDisk.ahci_bar5 = bar5;  // Save for later use in storage.c

    unsigned long va   = AHCI_CONTROLLER_VA;

    // IN: pa, va
    if (mm_map_2mb_region_in_pd0(bar5, va) < 0) 
    {
        printk("AHCI: mapping BAR5 failed\n");
        return -1;
    }

    printk("DDINIT_ahci: #breakpoint %x\n", bar5);  // physical address
    printk("DDINIT_ahci: #breakpoint %x\n", va);
    //while(1){}

    AHCI_HBA = (volatile HBA_MEM *) va;  //

    if (!AHCI_HBA)
    {
        printk("DDINIT_ahci: Invalid HBA address\n");
        return -1;
    }


    printk("AHCI: HBA at 0x%x | CAP=0x%x | PI=0x%x | VS=0x%x\n",
           bar5, AHCI_HBA->cap, AHCI_HBA->pi, AHCI_HBA->vs);


    //printk("DDINIT_ahci: #breakpoint\n");
    //while(1){}

    // Reset and enable AHCI
    ahci_reset_hba();
    ahci_enable();

    // Setup ports
    ahci_probe_ports();

/*
 // Probe and setup ports
    uint32_t pi = AHCI_HBA->pi;
    for (int i=0; i<NR_PORTS; i++) {
        if (pi & (1u << i)) {
            printk("AHCI: setting up port %d\n", i);
            ahci_setup_port(i);
        }
    }
*/

// initialize the first port for testing
    //ahci_setup_port(0);

    // Setup only port 0 for now
    if (ahci_setup_port(0) != 0)
    {
        printk("AHCI: Failed to setup port 0\n");
        return -1;
    }


    BootDisk.initialized = TRUE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_AHCI;

    g_ahci_driver_initialized = TRUE;

    printk("AHCI: Driver initialized successfully\n");
    return 0;
}

