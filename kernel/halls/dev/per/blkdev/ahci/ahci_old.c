// ahci.c
// AHCI driver - Basic version
// Created by Grok + Fred Nora style
// Following the same philosophy as ata.c

#include <kernel.h>

// #test
unsigned long HBA_BASE=0;

static const char _zhba_base[62*1024]__attribute__((aligned(4096)));


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

static inline void ahci_flush_cr3(void);
static void ahci_io_delay(void);
static void ahci_delay(int ms);
static int ahci_setup_port(int port_num);
static int ahci_identify_device(int port_num);
static void ahci_flush_cache(void *va, unsigned long size);
static void ahci_invalidate_cache(void *va, unsigned long size);
static void ahci_enable(void);
static void ahci_reset_hba(void);
static void ahci_probe_ports(void);


// =======================================================

static inline void ahci_flush_cr3(void) 
{
    unsigned long cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    asm volatile("mov %0, %%cr3" : : "r"(cr3));
}

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

// Identify device on a port
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
    //ahci_port[port_num].todo00 = 1234;  // placeholder

    return 0;
}


// Stronger flush - write back dirty lines
static void ahci_flush_cache(void *va, unsigned long size)
{
    // For safety, we use full WBINVD when 
    // clflush is not reliable
    asm volatile("wbinvd" ::: "memory");
}

// Invalidate after DMA read
static void ahci_invalidate_cache(void *va, unsigned long size)
{
    asm volatile("wbinvd" ::: "memory");
}


// =====================================================
// Basic AHCI Read Sector (kernel-side)
// =====================================================

// IN: port, lba. buffer, sector_count
int ahci_read_sector(int port, uint64_t lba, void *buffer_va, uint32_t sector_count)
{
    struct ahci_port_d *pinfo;
    volatile HBA_PORT *p;
    HBA_CMD_HEADER *cmd_hdr;
    HBA_CMD_TBL *cmd_tbl;
    FIS_REG_H2D *fis;
    unsigned long buf_pa;
    uint32_t timeout;

    if (port < 0 || port >= NR_PORTS || !buffer_va || sector_count == 0)
        return -1;

    pinfo = &ahci_port[port];
    if (!pinfo->initialized)
    {
        printk("AHCI: Port %d not initialized\n", port);
        return -1;
    }

    p = &AHCI_HBA->ports[port];

    printk("=== AHCI READ ATTEMPT === Port %d | LBA %u | Sectors %u | VA=0x%x\n", 
           port, (uint32_t)lba, sector_count, (unsigned long)buffer_va);

    // 1. Wait for port ready
    timeout = 2000000;
    while ((p->tfd & (ATA_SR_BSY | ATA_SR_DRQ)) && timeout--)
        ahci_io_delay();

    if (timeout == 0)
    {
        printk("AHCI: Timeout waiting for ready\n");
        return -1;
    }

    // 2. Prepare command slot 0
    cmd_hdr = &pinfo->mem->cmd_list[0];
    cmd_tbl = pinfo->cmd_tbl_va[0];

    memset(cmd_hdr, 0, sizeof(HBA_CMD_HEADER));
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL));

    cmd_hdr->cfl   = 5;
    cmd_hdr->w     = 0;  // Read
    cmd_hdr->prdtl = 1;

    fis = (FIS_REG_H2D*)cmd_tbl->cfis;
    memset(fis, 0, sizeof(FIS_REG_H2D));

    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c        = 1;
    fis->command  = ATA_CMD_READ_DMA_EXT;
    fis->device   = (1 << 6);   /* LBA48 */

    fis->lba0 = (lba >> 0)  & 0xFF;
    fis->lba1 = (lba >> 8)  & 0xFF;
    fis->lba2 = (lba >> 16) & 0xFF;
    fis->lba3 = (lba >> 24) & 0xFF;
    fis->lba4 = (lba >> 32) & 0xFF;
    fis->lba5 = (lba >> 40) & 0xFF;

    fis->countl = sector_count & 0xFF;
    fis->counth = (sector_count >> 8) & 0xFF;

    buf_pa = virtual_to_physical((unsigned long)buffer_va, gKernelPML4Address);
    printk("Buffer PA: 0x%x\n", (uint32_t)buf_pa);

    cmd_tbl->prdt_entry[0].dba  = (uint32_t)buf_pa;
    cmd_tbl->prdt_entry[0].dbau = (uint32_t)(buf_pa >> 32);
    cmd_tbl->prdt_entry[0].dbc  = (sector_count * 512) - 1;
    cmd_tbl->prdt_entry[0].i    = 1;

    p->is = 0xFFFFFFFF;

    // === CACHE ===
    ahci_flush_cache(buffer_va, sector_count * 512);

    p->ci = (1 << 0);  // Fire! (Interrupt on completion)

    // 3. Wait for completion
    timeout = 3000000;
    while ((p->ci & 1) && timeout--)
    {
        if (p->is & (HBA_PxIS_TFES | HBA_PxIS_HBFS | HBA_PxIS_IFS))
        {
            printk("AHCI Error! IS=0x%x | TFD=0x%x\n", p->is, p->tfd);
            return -1;
        }
    }

    if (timeout == 0)
    {
        printk("AHCI: Command timeout!\n");
        return -1;
    }

    // Invalidate cache after DMA
    ahci_invalidate_cache(buffer_va, sector_count * 512);

    if (p->tfd & ATA_SR_ERR)
    {
        printk("TFD Error = 0x%x\n", p->tfd);
        return -1;
    }

    // === SHOW RESULT ===
    {
        unsigned char *b = (unsigned char*)buffer_va;
        int i;

        printk("First 32 bytes: ");
        for (i = 0; i < 32; i++)
            printk("%x ", b[i]);
        printk("\n");

        printk("MBR Signature (510-511): %x %x\n", b[510], b[511]);

        if (b[510] == 0x55 && b[511] == 0xAA)
            printk(">>> MBR SIGNATURE CORRECT! <<<\n");
        else
            printk("Still wrong signature...\n");
    }

    return 0;
}

void ahci_test_read(void)
{
    unsigned char *test_buf = (unsigned char*) kmalloc_aligned(4096, 4096);
    if (!test_buf)
    {
        printk("kmalloc failed\n");
        return;
    }

    memset(test_buf, 0xFE, 512);   /* fill with obvious value */

    printk("=== AHCI TEST READ START ===\n");
    if (ahci_read_sector(0, 0, test_buf, 1) == 0)
    {
        printk("Read completed successfully.\n");
    }
    else
    {
        printk("Read failed!\n");
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

static int ahci_setup_port(int port_num)
{

// Parameter:
    if (port_num < 0 || port_num >= NR_PORTS)
        return (int) -1;

// The port structure
    volatile HBA_PORT *port = &AHCI_HBA->ports[port_num];

// The structure for port info
    struct ahci_port_d *pinfo = &ahci_port[port_num];

//
// Stop port cleanly
//

/*
    // Stop port
    port->cmd &= ~((1<<0) | (1<<4));   // Clear ST + FRE
    // Wait for CR + FR to clear
    while (port->cmd & ((1<<15) | (1<<14)))
        ahci_io_delay();
*/

	// Clear FIS Receive Enable (FRE - bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
	// espera o FR (bit14)
	while(port->cmd & HBA_PxCMD_FR)
        ahci_io_delay();
	// Apagar o ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
	// espera o CR (bit15) terminar
	while(port->cmd & HBA_PxCMD_CR)
        ahci_io_delay();


// --------------------
// Allocate port memory (Command List + FIS + Received FIS)
/*
    size_t mem_size = sizeof(AHCI_PORT_MEMORY);
    pinfo->mem = (AHCI_PORT_MEMORY*) kmalloc_aligned(mem_size, 1024);
    if (!pinfo->mem)
        return -2;
    memset(pinfo->mem, 0, mem_size);
    pinfo->mem_pa = virtual_to_physical((unsigned long)pinfo->mem, gKernelPML4Address);
*/

    pinfo->mem = (AHCI_PORT_MEMORY*) HBA_BASE;  // Virtual address of the whole block
    pinfo->mem_pa = virtual_to_physical((unsigned long)pinfo->mem, gKernelPML4Address);

    // Clear using virtual address
    memset( pinfo->mem, 0, 1024*2);


    // Command List Base
    //port->clb  = (uint32_t) pinfo->mem;  // Bellow 4GB, so upper is 0
    port->clb  = (uint32_t) pinfo->mem_pa;  // Bellow 4GB, so upper is 0
    port->clbu = (uint32_t) 0;
    //memset((char *)((unsigned long)port->clb), 0, 1024);

    // FIS Base
    port->fb   = (uint32_t) (port->clb + 1024);  // Bellow 4GB, so upper is 0
    port->fbu  = (uint32_t) 0;
   	//memset((char *)((unsigned long)port->fb), 0, 256);

// ------------------------------------------

// Command Tables (one per slot)

    int i = 0;
    for (i = 0; i < 32; i++) 
    {
        void *tbl_va = kmalloc_aligned(sizeof(HBA_CMD_TBL), 128);
        if (!tbl_va) return -3;
        memset(tbl_va, 0, sizeof(HBA_CMD_TBL));

        unsigned long tbl_pa = virtual_to_physical((unsigned long)tbl_va, gKernelPML4Address);

        pinfo->mem->cmd_list[i].ctba  = (uint32_t)tbl_pa;
        pinfo->mem->cmd_list[i].ctbau = (uint32_t)(tbl_pa >> 32);
        pinfo->mem->cmd_list[i].prdtl = 1;   // One PRDT for now

        pinfo->cmd_tbl_va[i] = (HBA_CMD_TBL*)tbl_va;
    }

    port->is = 0xFFFFFFFF;  // Clear interrupts

//
// Start port
//

    /*
    port->cmd |= (1 << 4);  // FRE
    while (!(port->cmd & (1 << 14))) ahci_io_delay();
    port->cmd |= (1 << 0);  // ST
    while (!(port->cmd & (1 << 15))) ahci_io_delay();
    */

/*
    // Start port (quick and dirty)
    port->cmd |= (1 << 4);   // FRE
    port->cmd |= (1 << 0);   // ST
*/

    // Aguarde ate que o CR (Command List Running (bit15)) ser apagado
    while((port->cmd & HBA_PxCMD_CR))
        ahci_io_delay();
    // Define FIS Receive Enable (bit4) and Sart (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;


// Initialized?
    pinfo->port_num = port_num;
    pinfo->initialized = TRUE;
    printk("AHCI: Port %d fully initialized\n", port_num);
    return 0;
}

// Probe all ports
static void ahci_probe_ports(void)
{
    int i=0;
    uint32_t pi = AHCI_HBA->pi;

	//int total_np = (AHCI_HBA->cap & 0x1f) + 1;

    printk("AHCI: Ports Implemented = 0x%x\n", pi);

    for (i=0; i < NR_PORTS; i++)
    {
        if (pi & (1u << i))
        {
            volatile HBA_PORT *port = &AHCI_HBA->ports[i];

            printk("AHCI Port %d: sig=0x%x  ", i, port->sig);

            if (port->sig == 0x00000101)
                printk("[SATA]\n");
            else if (port->sig == 0xEB140101)
                printk("[SATAPI]\n");
            else
                printk("[Unknown]\n");

            // Setup port
            ahci_setup_port(i);
        }
    }

    // #todo: We need this.
    //if (port->sig == 0x00000101 && BootDisk.boot_port < 0)
    //{
    //    BootDisk.boot_port = i;
    //    printf("AHCI: Boot port candidate = %d\n", i);
    //}
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

// Parameters:
    if ((void*)pci_ahci == NULL){
        printk("DDINIT_ahci: pci_ahci == NULL\n");
        return -1;
    }
    // Check controller type
    if (controller_type != STORAGE_CONTROLLER_MODE_AHCI){
        printk("DDINIT_ahci: Wrong controller type\n");
        return -1;
    }

    // Get locally allocated HBA base address for testing.
    HBA_BASE = (unsigned long) (_zhba_base);

    // BAR5 is the AHCI base address
    unsigned long bar5 = pci_ahci->BAR5 & ~0xF;
    BootDisk.ahci_bar5 = bar5;  // Save for later use in storage.c

    unsigned long va = AHCI_CONTROLLER_VA;

    // IN: pa, va
    if (mm_map_2mb_region_in_pd0(bar5, va) < 0) 
    {
        printk("AHCI: mapping BAR5 failed\n");
        return -1;
    }

    ahci_flush_cr3();  // Ensure new mapping is active

    printk("DDINIT_ahci: bar5 pa %x\n", bar5);  // physical address
    printk("DDINIT_ahci: bar5 va %x\n", va);    // virtual address
    //while(1){}

// ----------------------------
    AHCI_HBA = (volatile HBA_MEM *) va;
    if (!AHCI_HBA){
        printk("DDINIT_ahci: Invalid HBA address\n");
        return -1;
    }
    printk("AHCI: HBA at 0x%x | CAP=0x%x | PI=0x%x | VS=0x%x\n",
           bar5, AHCI_HBA->cap, AHCI_HBA->pi, AHCI_HBA->vs);

    ahci_reset_hba();
    ahci_enable();
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

/*
    // Setup only port 0 for now
    if (ahci_setup_port(0) != 0){
        printk("AHCI: Failed to setup port 0\n");
        return -1;
    }
*/


    BootDisk.initialized = TRUE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_AHCI;

    g_ahci_driver_initialized = TRUE;

    printk("AHCI: Driver initialized successfully\n");

//========================================

    ahci_test_read();   
    while(1){}

    return 0;
}

