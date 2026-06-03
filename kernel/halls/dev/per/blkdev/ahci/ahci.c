// ahci.c
// AHCI driver - Basic version
// Created by Fred Nora.
// Following the same philosophy as ata.c
// Rebuilt by Claude

#include <kernel.h>

// #test
unsigned long HBA_BASE = 0;
static const char _zhba_base[62*1024] __attribute__((aligned(4096)));

/*
// TEMP DEBUG: use identity-mapped low buffer
// Allocate below 4GB and use PA == VA assumption to isolate the bug
static unsigned char  __test_buf[512]    __attribute__((aligned(512)));
static HBA_CMD_HEADER __test_cmdlist[32] __attribute__((aligned(1024)));
static unsigned char  __test_fis[256]    __attribute__((aligned(256)));
static HBA_CMD_TBL    __test_cmdtbl      __attribute__((aligned(128)));
*/


// Globals
int g_ahci_driver_initialized = FALSE;


// Main structure:
// It has the following components:
// + 0x00 - 0x2B, Generic Host Control
// + 0x2C - 0x9F, Reserved
// + 0xA0 - 0xFF, Vendor specific registers
// + 0x100 - 0x10FF, Port control registers
volatile HBA_MEM *AHCI_HBA_STRUCT = NULL;


struct ahci_port_d  ahci_port[NR_PORTS];
struct ahci_current_port_d  AHCICurrentPort;

// Next device ID
static uint32_t __next_ahci_id = 0;

struct ahci_device_d *ahci_ready_queue_dev = NULL;
struct ahci_device_d *current_ahci_dev     = NULL;


// =======================================================
// Prototypes (internal)
// =======================================================

static inline void ahci_flush_cr3(void);
static void ahci_io_delay(void);
static void ahci_delay(int ms);
static int  __ahci_setup_port(int port_num);
static int  ahci_identify_device(int port_num);
static void ahci_flush_cache(void *va, unsigned long size);
static void ahci_invalidate_cache(void *va, unsigned long size);
static void ahci_enable(void);
static void ahci_reset_hba(void);
static void ahci_probe_ports(void);


// =======================================================
// Low-level helpers
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
    int i = 0;
    for (i = 0; i < ms * 1000; i++) {
        ahci_io_delay();
    }
}


// =======================================================
// Cache helpers
// Flush command structures BEFORE issuing CI.
// Invalidate data buffer AFTER DMA completes.
// =======================================================

// Write back dirty cache lines (use before issuing command to HBA)
static void ahci_flush_cache(void *va, unsigned long size)
{
    asm volatile("wbinvd" ::: "memory");
    (void)va;
    (void)size;
}

// Invalidate stale cache lines (use after HBA DMA fills the buffer)
static void ahci_invalidate_cache(void *va, unsigned long size)
{
    asm volatile("wbinvd" ::: "memory");
    (void)va;
    (void)size;
}


// =======================================================
// ahci_identify_device
// =======================================================

static int ahci_identify_device(int port_num)
{
    if (port_num < 0 || port_num >= NR_PORTS)
        return -1;

    volatile HBA_PORT *port = &AHCI_HBA_STRUCT->ports[port_num];

    // Check if device is present
    if (port->sig == 0 || port->sig == 0xFFFFFFFF)
        return -1;

    printk("AHCI Port %d: Signature 0x%X\n", port_num, port->sig);

    return 0;
}


// =======================================================
// ahci_read_sector
// IN: port, lba, buffer_va, sector_count
// =======================================================

int ahci_read_sector(int port, uint64_t lba, void *buffer_va, uint32_t sector_count)
{

// Parameters:
    struct ahci_port_d  *pinfo;
    volatile HBA_PORT   *p;
    HBA_CMD_HEADER      *cmd_hdr;
    HBA_CMD_TBL         *cmd_tbl;
    FIS_REG_H2D         *fis;
    unsigned long        buf_pa;
    uint32_t             timeout;

    if (port < 0 || port >= NR_PORTS || !buffer_va || sector_count == 0)
        return -1;

    pinfo = &ahci_port[port];
    if (!pinfo->initialized)
    {
        printk("AHCI: Port %d not initialized\n", port);
        return -1;
    }

    p = &AHCI_HBA_STRUCT->ports[port];

    printk("=== AHCI READ ATTEMPT === Port %d | LBA %u | Sectors %u | VA=0x%x\n",
           port, (uint32_t)lba, sector_count, (unsigned long)buffer_va);

//
// 1. Wait for port to be ready (BSY + DRQ must be clear)
//

    timeout = 2000000;
    while ((p->tfd & (ATA_SR_BSY | ATA_SR_DRQ)) && timeout--)
        ahci_io_delay();

    if (timeout == 0)
    {
        printk("AHCI: Timeout waiting for port ready\n");
        return -1;
    }

//
// 2. Select command slot 0
//

    cmd_hdr = &pinfo->mem->cmd_list[0];
    cmd_tbl = pinfo->cmd_tbl_va[0];

    memset(cmd_hdr, 0, sizeof(HBA_CMD_HEADER));
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL));

    // Command FIS length in DWORDs (FIS_REG_H2D = 20 bytes = 5 DWORDs)
    cmd_hdr->cfl   = 5;
    cmd_hdr->w     = 0;   // Read (D2H)
    cmd_hdr->prdtl = 1;   // One PRDT entry

//
// 3. Build H2D Register FIS
//

    fis = (FIS_REG_H2D *) cmd_tbl->cfis;
    memset(fis, 0, sizeof(FIS_REG_H2D));

    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c        = 1;                    // Command register update
    fis->command  = ATA_CMD_READ_DMA_EXT;
    fis->device   = (1 << 6);            // LBA48 mode

    // LBA 48-bit address
    fis->lba0 = (uint8_t)((lba >>  0) & 0xFF);
    fis->lba1 = (uint8_t)((lba >>  8) & 0xFF);
    fis->lba2 = (uint8_t)((lba >> 16) & 0xFF);
    fis->lba3 = (uint8_t)((lba >> 24) & 0xFF);
    fis->lba4 = (uint8_t)((lba >> 32) & 0xFF);
    fis->lba5 = (uint8_t)((lba >> 40) & 0xFF);

    // Sector count
    fis->countl = (uint8_t)(sector_count & 0x00FF);
    fis->counth = (uint8_t)((sector_count >> 8) & 0x00FF);

//
// 4. Set up PRDT entry (physical address of data buffer)
//

    buf_pa = virtual_to_physical((unsigned long)buffer_va, gKernelPML4Address);
    printk("AHCI: Buffer VA=0x%x  PA=0x%x\n",
           (uint32_t)(unsigned long)buffer_va, (uint32_t)buf_pa);

    cmd_tbl->prdt_entry[0].dba  = (uint32_t)(buf_pa & 0xFFFFFFFF);
    cmd_tbl->prdt_entry[0].dbau = (uint32_t)(buf_pa >> 32);
    cmd_tbl->prdt_entry[0].dbc  = (sector_count * 512) - 1;  // Byte count minus 1
    cmd_tbl->prdt_entry[0].i    = 1;                          // Interrupt on completion

//
// 5. Flush command structures to memory BEFORE issuing the command
//    The HBA reads cmd_hdr + cmd_tbl via DMA — CPU cache must be written back first.
//

    ahci_flush_cache(cmd_hdr, sizeof(HBA_CMD_HEADER));
    ahci_flush_cache(cmd_tbl, sizeof(HBA_CMD_TBL));

//
// 6. Clear interrupt status and issue command
//

    p->is  = 0xFFFFFFFF;   // Clear all pending interrupt bits
    p->serr = 0xFFFFFFFF;  // Clear SATA errors too
    p->ci  = (1 << 0);     // Issue command slot 0

//
// 7. Wait for completion (poll CI bit)
//

    timeout = 3000000;
    while ((p->ci & (1 << 0)) && timeout--)
    {
        // Check for fatal errors during transfer
        if (p->is & (HBA_PxIS_TFES | HBA_PxIS_HBFS | HBA_PxIS_IFS))
        {
            printk("AHCI: Error during transfer! IS=0x%x | TFD=0x%x | SERR=0x%x\n",
                   p->is, p->tfd, p->serr);
            return -1;
        }
        ahci_io_delay();
    }

    if (timeout == 0)
    {
        printk("AHCI: Command timeout! TFD=0x%x CI=0x%x IS=0x%x\n",
               p->tfd, p->ci, p->is);
        return -1;
    }

//
// 8. Invalidate CPU cache over the data buffer so we read fresh DMA data
//

    ahci_invalidate_cache(buffer_va, sector_count * 512);

//
// 9. Check TFD for ATA errors
//

    if (p->tfd & ATA_SR_ERR)
    {
        printk("AHCI: TFD Error = 0x%x\n", p->tfd);
        return -1;
    }

//
// 10. Dump result for verification
//

    {
        unsigned char *b = (unsigned char *) buffer_va;
        int i = 0;

        printk("AHCI: First 32 bytes: ");
        for (i = 0; i < 32; i++)
            printk("%x ", b[i]);
        printk("\n");

        printk("AHCI: MBR Signature (510-511): %x %x\n", b[510], b[511]);

        if (b[510] == 0x55 && b[511] == 0xAA)
            printk("AHCI: >>> MBR SIGNATURE CORRECT! <<<\n");
        else
            printk("AHCI: Wrong signature — DMA may not have completed correctly.\n");
    }

    return 0;
}


// =======================================================
// ahci_test_read
// =======================================================

void ahci_test_read(void)
{
    unsigned char *test_buf = (unsigned char *) kmalloc_aligned(4096, 4096);
    if (!test_buf)
    {
        printk("AHCI: kmalloc_aligned failed\n");
        return;
    }

    memset(test_buf, 0xFE, 512);   // Fill with known pattern before read

    printk("=== AHCI TEST READ START ===\n");

    if (ahci_read_sector(0, 0, test_buf, 1) == 0)
        printk("AHCI: Read completed successfully.\n");
    else
        printk("AHCI: Read failed!\n");
}


// =======================================================
// ahci_enable
// Set AE (AHCI Enable) bit in GHC
// =======================================================

static void ahci_enable(void)
{
    if (!AHCI_HBA_STRUCT)
        return;

    AHCI_HBA_STRUCT->ghc |= (1 << 31);   // AE bit
}


// =======================================================
// ahci_reset_hba
// Global HBA Reset via HR bit in GHC
// =======================================================

static void ahci_reset_hba(void)
{
    if (!AHCI_HBA_STRUCT)
        return;

    printk("AHCI: Resetting HBA ...\n");

    AHCI_HBA_STRUCT->ghc |= (1 << 0);   // HR bit

    // Wait for reset to complete (HR self-clears)
    while (AHCI_HBA_STRUCT->ghc & (1 << 0))
        ahci_io_delay();

    printk("AHCI: HBA Reset done\n");
}


// =======================================================
// __ahci_setup_port
// Allocate and map command list, FIS area and command tables.
// =======================================================

// Worker
static int __ahci_setup_port(int port_num)
{

    printk("__ahci_setup_port: Setup port %d\n", port_num);

// Parameters:
    if (port_num < 0 || port_num >= NR_PORTS)
        return (int) -1;

// --------------------------------------------------------
// Get pointer to the port structure
// The port hardware registers

    volatile HBA_PORT *port = &AHCI_HBA_STRUCT->ports[port_num];

    printk("Port %d: sig=0x%x  ", port_num, port->sig);

    if (port->sig == 0x00000101)
        printk("[SATA]\n");
    else if (port->sig == 0xEB140101)
        printk("[SATAPI]\n");
    else
        printk("[Unknown/Not present]\n");


//
// Stop port cleanly before reconfiguring
//

    // Clear FIS Receive Enable (FRE - bit4)
    port->cmd &= ~HBA_PxCMD_FRE;
    // Wait for FR (bit14) to clear
    while (port->cmd & HBA_PxCMD_FR)
        ahci_io_delay();

    // Clear ST (bit0)
    port->cmd &= ~HBA_PxCMD_ST;
    // Wait for CR (bit15) to clear
    while (port->cmd & HBA_PxCMD_CR)
        ahci_io_delay();

//
// Map port memory block (Command List + Received FIS area)
// Using the locally allocated static buffer for now.
//

// Base address for locally allocated port memory (static buffer)
    HBA_BASE = (unsigned long) _zhba_base;

    // The software port descriptor
    struct ahci_port_d *pinfo = &ahci_port[port_num];
    // #important:
    // pinfo->mem is the poiter for the AHCI_PORT_MEMORY structure.
    pinfo->mem    = (AHCI_PORT_MEMORY *) HBA_BASE;
    pinfo->mem_pa = virtual_to_physical((unsigned long)pinfo->mem, gKernelPML4Address);

    // Zero the whole block
    // memset(pinfo->mem, 0, sizeof(AHCI_PORT_MEMORY));

    // Flush to memory so the HBA sees clean state
    ahci_flush_cache(pinfo->mem, sizeof(AHCI_PORT_MEMORY));

    printk("AHCI: Port %d mem VA=0x%x  PA=0x%x\n",
           port_num,
           (uint32_t)(unsigned long)pinfo->mem,
           (uint32_t)pinfo->mem_pa);

//
// Program Command List Base (CLB) — must be 1K-aligned physical address
//

// #ps: This is the HBA_BASE.
//      At some point we will setup the content into that area.
//      There are some structures into this area.

// CLB: It points to a Command List. That points to Command Tables.
    port->clb  = (uint32_t)(pinfo->mem_pa & 0xFFFFFFFF);
    port->clbu = (uint32_t)(pinfo->mem_pa >> 32);

//
// Program FIS Base (FB) — must be 256-byte aligned physical address
// Placed right after the 1K command list (offset +0x400)
//

    unsigned long fb_pa = pinfo->mem_pa + 0x400;   // 1024 bytes after CLB

// FB: It points to a FIS receive struture.
    port->fb  = (uint32_t)(fb_pa & 0xFFFFFFFF);
    port->fbu = (uint32_t)(fb_pa >> 32);


//
// Allocate Command Tables (one per slot, 128-byte aligned)
// Store both physical address in cmd_list and virtual pointer in cmd_tbl_va.
//

    int i = 0;
    for (i = 0; i < 32; i++)
    {
        // Command table
        // #ps: Command list struture, points to command table structure

        // == virtual address ==
        void *tbl_va = kmalloc_aligned(sizeof(HBA_CMD_TBL), 128); // cmd table
        if (!tbl_va)
            return -3;
        memset(tbl_va, 0, sizeof(HBA_CMD_TBL));
        // Keep virtual pointer for CPU-side access
        // Here we build a lot of command tables.
        pinfo->cmd_tbl_va[i] = (HBA_CMD_TBL *) tbl_va; // save

        // == physical address ==
        // This is a list of command table structure? <<<<
        // Let's make the command list structure
        // points to command tables.

        // >>> The list <<<
        unsigned long tbl_pa = virtual_to_physical((unsigned long)tbl_va, gKernelPML4Address);
        // Write physical address into command header so HBA can DMA-fetch it
        pinfo->mem->cmd_list[i].ctba  = (uint32_t)(tbl_pa & 0xFFFFFFFF);
        pinfo->mem->cmd_list[i].ctbau = (uint32_t)(tbl_pa >> 32);

        // Physical region descriptor table length in entries
        pinfo->mem->cmd_list[i].prdtl = 1;
    }

    // Flush command list with updated ctba values
    ahci_flush_cache(pinfo->mem->cmd_list, sizeof(HBA_CMD_HEADER) * 32);

    // Clear all interrupt status bits
    port->is   = 0xFFFFFFFF;
    port->serr = 0xFFFFFFFF;

//
// Start port
//

    // Wait until CR (Command List Running) is cleared
    while (port->cmd & HBA_PxCMD_CR)
        ahci_io_delay();

    // Enable FIS Receive
    port->cmd |= HBA_PxCMD_FRE;

    // Wait for FR (FIS Receive Running) to set — confirms FIS area is active
    while (!(port->cmd & HBA_PxCMD_FR))
        ahci_io_delay();

    // Start command processing
    port->cmd |= HBA_PxCMD_ST;

//
// Mark as initialized
//

    pinfo->port_num    = port_num;
    pinfo->initialized = TRUE;

    printk("AHCI: Port %d fully initialized | CLB PA=0x%x | FB PA=0x%x\n",
           port_num, port->clb, port->fb);

    return 0;
}


// =======================================================
// ahci_probe_ports
// Walk all implemented ports and set them up.
// =======================================================

static void ahci_probe_ports(void)
{
    int i = 0;

// 0x0C, Port implemented
    uint32_t pi = AHCI_HBA_STRUCT->pi;

    printk("AHCI: Ports Implemented = 0x%x\n", pi);

    for (i = 0; i < NR_PORTS; i++)
    {
        // This tell is the port is implemented or not.
        if (pi & (1 << i))
        {
            printk("PROBE: Port %d Is implemented\n", i);

            /*
            // Get pointer to the port structure
            volatile HBA_PORT *port = &AHCI_HBA_STRUCT->ports[i];

            printk("Port %d: sig=0x%x  ", i, port->sig);

            if (port->sig == 0x00000101)
                printk("[SATA]\n");
            else if (port->sig == 0xEB140101)
                printk("[SATAPI]\n");
            else
                printk("[Unknown/Not present]\n");
            */

            // Setup port regardless — device may appear after init
            __ahci_setup_port(i);  // Worker
        }
    }
}


// =======================================================
// DDINIT_ahci
// Main driver initialization (mirrors DDINIT_ata style)
// =======================================================

// IN:
// pci_ahci - pointer to PCI device struct for the AHCI controller
// controller_type - expected to be STORAGE_CONTROLLER_MODE_AHCI
int 
DDINIT_ahci(
    struct pci_device_d *pci_ahci,
    uint8_t controller_type )
{
    PROGRESS("DDINIT_ahci:\n");
    printk("DDINIT_ahci:\n");

// Parameters:
    if ((void *) pci_ahci == NULL){
        printk("DDINIT_ahci: pci_ahci == NULL\n");
        return -1;
    }
    if (controller_type != STORAGE_CONTROLLER_MODE_AHCI){
        printk("DDINIT_ahci: Wrong controller type\n");
        return -1;
    }

// BAR5 is the AHCI base address (ABAR)
    unsigned long bar5 = pci_ahci->BAR5 & ~0xF;
    BootDisk.ahci_bar5 = bar5;   // Save for use in storage.c

    unsigned long va = AHCI_CONTROLLER_VA;

    // Map BAR5 into kernel virtual address space
    if (mm_map_2mb_region_in_pd0(bar5, va) < 0)
    {
        printk("AHCI: Mapping BAR5 failed\n");
        return -1;
    }
    ahci_flush_cr3();   // Activate new page table entry

// Do we have a valid address?
    printk("DDINIT_ahci: BAR5 PA=0x%x  VA=0x%x\n", (uint32_t)bar5, (uint32_t)va);

// ----------------------------

//
// Get the pointer for the main structure
//

    AHCI_HBA_STRUCT = (volatile HBA_MEM *) va;

    if (!AHCI_HBA_STRUCT){
        printk("DDINIT_ahci: Invalid AHCI_HBA_STRUCT\n");
        return -1;
    }

    printk("AHCI: HBA at VA=0x%x | CAP=0x%x | PI=0x%x | VS=0x%x\n",
           (uint32_t)va, AHCI_HBA_STRUCT->cap, 
           AHCI_HBA_STRUCT->pi, 
           AHCI_HBA_STRUCT->vs );

// ----------------------------
    ahci_reset_hba();
    ahci_enable();
    ahci_probe_ports();

// ----------------------------
    BootDisk.initialized    = TRUE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_AHCI;

    g_ahci_driver_initialized = TRUE;

    printk("AHCI: Driver initialized successfully\n");

//========================================

    ahci_test_read();
    while(1){}

    return 0;
}
