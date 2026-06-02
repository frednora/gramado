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

// =====================================================
// Basic AHCI Read Sector (for kernel)
// =====================================================

// IN: port, lba. buffer, sector_count
int ahci_read_sector(int port, uint64_t lba, void *buffer, uint32_t sector_count)
{

    printk("ahci_read_sector: Testing read from port %d, LBA %d\n", port, lba);

    if (port < 0 || port >= NR_PORTS || !AHCI_HBA || !buffer)
        return -1;

    volatile HBA_PORT *p = &AHCI_HBA->ports[port];

    // Wait for port to be ready
    while (p->tfd & (ATA_SR_BSY | ATA_SR_DRQ))
        ahci_io_delay();

    //#debug
    //printk("ahci_read_sector: Port ready. Issuing command...\n");

    // Use command slot 0 (simple for bootloader)
    HBA_CMD_HEADER *cmd_hdr = (HBA_CMD_HEADER*)((unsigned long)p->clb);
    memset(cmd_hdr, 0, sizeof(HBA_CMD_HEADER));

    // Setup Command Header
    cmd_hdr->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);  // 5 dwords
    cmd_hdr->w   = 0;   // Read
    cmd_hdr->prdtl = 1; // One PRDT entry

    // Command Table
    HBA_CMD_TBL *cmd_tbl = (HBA_CMD_TBL*)((unsigned long)cmd_hdr->ctba);
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL));

    // === Build Command FIS (Register H2D) ===
    FIS_REG_H2D *fis = (FIS_REG_H2D*)&cmd_tbl->cfis;
    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c        = 1;                    // Command
    fis->command  = ATA_CMD_READ_DMA_EXT; // 0x25
    fis->device   = 1 << 6;               // LBA mode

    // LBA48
    fis->lba0 = (uint8_t) (lba);
    fis->lba1 = (uint8_t) (lba >> 8);
    fis->lba2 = (uint8_t) (lba >> 16);
    fis->lba3 = (uint8_t) (lba >> 24);
    fis->lba4 = (uint8_t) (lba >> 32);
    fis->lba5 = (uint8_t) (lba >> 40);

    fis->countl = (uint8_t) sector_count;
    fis->counth = (uint8_t) (sector_count >> 8);

    // === PRDT (Physical Region Descriptor) ===
    cmd_tbl->prdt_entry[0].dba = (uint32_t)((unsigned long)buffer);
    cmd_tbl->prdt_entry[0].dbau = 0;
    cmd_tbl->prdt_entry[0].dbc = (sector_count * 512) - 1;  // byte count
    cmd_tbl->prdt_entry[0].i = 1;   // Interrupt on completion

    // Issue command
    p->ci = 1 << 0;   // Issue slot 0

    // Wait for completion

    //#debug
    printk("ahci_read_sector: wait \n");
    while (p->ci & 1)
    {
        if (p->is & HBA_PxIS_TFES)   // Task File Error
        {
            printk("AHCI Read Error: TFES\n");
            return -1;
        }
    }

    if (p->tfd & ATA_SR_ERR)
    {
        printk("AHCI Read Error: Status Error\n");
        return -1;
    }

    printk("ahci_read_sector: Read done\n");

    return 0;  // Success
}

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
    ahci_setup_port(0);

    BootDisk.initialized = TRUE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_AHCI;

    g_ahci_driver_initialized = TRUE;

    printk("AHCI: Driver initialized successfully\n");
    return 0;
}

