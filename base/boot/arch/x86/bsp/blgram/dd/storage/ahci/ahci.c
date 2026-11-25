// ahci.c
// AHCI driver for Bootloader
// Created by Fred Nora

/*
Alignment reminders (critical for AHCI):
 + Command List (clb): 1 KiB aligned
 + FIS receive area (fb): 256 bytes aligned
 + Command Table (ctba): 128 bytes aligned
 + PRDT entries: 128-byte aligned recommended
*/


#include "../../../bl.h"


// The main structure
volatile HBA_MEM *AHCI_HBA = NULL;

// The top level structure for ports
struct ahci_port_d  ahci_port[NR_PORTS];


// == Prototypes ===============================

static void __ahci_io_delay(void);
static void ahci_delay(int ms);

// =====================================================


// Worker
static void __ahci_io_delay(void)
{
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

// Dalay
static void ahci_delay(int ms)
{
    int i=0;
    int Max = (int) (1000 * ms);

    for (i=0; i<Max; i++){
        __ahci_io_delay();
    };
}


// =====================================================
// Basic AHCI Read Sector (for bootloader)
// =====================================================

// #todo
// We need to handle the lba value using two 32bit variables
// because we are in a 32bit environment?

// IN: port, lba. buffer, sector_count
int 
ahci_read_sector(
    int port, 
    uint64_t lba, 
    void *buffer, 
    uint32_t sector_count )
{
    if (port < 0 || port >= NR_PORTS || !AHCI_HBA || !buffer)
        goto fail;

    volatile HBA_PORT *p = &AHCI_HBA->ports[port];

    // Wait for port to be ready
    while (p->tfd & (ATA_SR_BSY | ATA_SR_DRQ)){
        __ahci_io_delay();
    };

    // Use command slot 0 (simple for bootloader)
    HBA_CMD_HEADER *cmd_hdr = (HBA_CMD_HEADER*)((unsigned long)p->clb);
    memset(cmd_hdr, 0, sizeof(HBA_CMD_HEADER));

    // Setup Command Header
    cmd_hdr->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);  // 5 dwords
    cmd_hdr->w   = 0;   // Read
    cmd_hdr->prdtl = 1; // One PRDT entry

    // Command Table
    // #bugbug: We got to allocate memory for cmd_hdr->ctba?
    HBA_CMD_TBL *cmd_tbl = (HBA_CMD_TBL*)((unsigned long)cmd_hdr->ctba);
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL));

    // === Build Command FIS (Register H2D) ===
    FIS_REG_H2D *fis = (FIS_REG_H2D*)&cmd_tbl->cfis;
    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c        = 1;                    // Command
    fis->command  = ATA_CMD_READ_DMA_EXT; // 0x25
    fis->device   = 1 << 6;               // LBA mode


// #todo
// We need to handle the lba value using two 32bit variables
// because we are in a 32bit environment?

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
    while (p->ci & 1)
    {
        if (p->is & HBA_PxIS_TFES)   // Task File Error
        {
            printf("ahci_read_sector: HBA_PxIS_TFES\n");
            goto fail;
        }
    };

    if (p->tfd & ATA_SR_ERR)
    {
        printf("ahci_read_sector: ATA_SR_ERR\n");
        goto fail;
    }

    return 0;  // Success

fail:
    return (int) (-1);
}

// #todo
// We need to handle the lba value using two 32bit variables
// because we are in a 32bit environment?

int 
ahci_write_sector(
    int port, 
	uint64_t lba, 
	void *buffer, 
	uint32_t sector_count )
{

    // #todo
    // Its very similar to the read implementation.
    // we just need to change some few values.

    return 0;
}

void ahci_test_read(void)
{
    // Test read sector 0 (MBR)
    unsigned char *test_buf = (unsigned char*) malloc(512);

    if (test_buf)
    {
        memset(test_buf, 0, 512);

        // OK. signature was found
        if (ahci_read_sector(0, 0, test_buf, 1) == 0)
        {
            printf("ahci_test_read: AHCI Read Sector 0 OK! Signature: %x %x\n", 
                   test_buf[510], test_buf[511] );
        }
        else
        {
            printf("AHCI Read failed!\n");
        }
    }
}

// =====================================================

static void ahci_reset_hba(void)
{
    if (!AHCI_HBA) 
        return;

    // printf("AHCI: Resetting HBA...\n");
    AHCI_HBA->ghc |= (1 << 0);        // HR = 1

    while (AHCI_HBA->ghc & (1 << 0))
        __ahci_io_delay();

    // printf("AHCI: HBA Reset done\n");
}

static void ahci_enable(void)
{
    if (!AHCI_HBA)
        return;

    AHCI_HBA->ghc |= (1 << 31);       // AE = 1 (AHCI Enable)
}

// =====================================================

static int ahci_setup_port(int port_num)
{
    if (port_num < 0 || port_num >= NR_PORTS)
        return -1;

// Get the port structure
    volatile HBA_PORT *port = &AHCI_HBA->ports[port_num];

    // Stop port
    port->cmd &= ~((1<<0) | (1<<4));   // Clear ST + FRE
    // Wait for CR + FR to clear
    while (port->cmd & ((1<<15) | (1<<14)))
        __ahci_io_delay();

    // #todo: Allocate aligned memory properly in bootloader
    // For now, using a simple approach (you can improve later)

    // Command List Base (1KB aligned)
    unsigned long clb = (unsigned long) malloc(1024);
    if (!clb)
        return -1;
    memset((void*)clb, 0, 1024);

    // FIS Base (256B aligned)
    unsigned long fb = (unsigned long) malloc(256);
    if (!fb)
        return -1;
    memset((void*)fb, 0, 256);

    // 3. Command Table - 128 bytes aligned (usually bigger is safer)
    //unsigned long cmd_tbl_mem = (unsigned long) malloc(128);   // 4KB is generous for bootloader
    unsigned long cmd_tbl_mem = (unsigned long) malloc(1024);
    if (!cmd_tbl_mem)
        return -1;
    memset(cmd_tbl_mem, 0, 1024);

    printf("AHCI Port %d: CLB=0x%x | FB=0x%x | CTBA=0x%x\n", 
           port_num, (uint32_t)clb, (uint32_t)fb, (uint32_t)cmd_tbl_mem);

    //refresh_screen();
    //while(1){}

// -------------------------------

// Setup CLB (Command List Base)
    port->clb  = (uint32_t)clb;
    port->clbu = 0;
// 4. Setup FB (FIS Base)
    port->fb   = (uint32_t)fb;
    port->fbu  = 0;

    // #todo: We need a pointer for ctba
    // Link Command Header → Command Table
    HBA_CMD_HEADER *cmd_hdr = (HBA_CMD_HEADER*)clb;
    cmd_hdr->ctba  = (uint32_t)cmd_tbl_mem;
    cmd_hdr->ctbau = 0;

// Start port
    port->cmd |= (1 << 4);   // FRE
    port->cmd |= (1 << 0);   // ST

    // printf("AHCI: Port %d setup done\n", port_num);
    return 0;
}

// =====================================================

static void ahci_probe_ports(void)
{
    int i=0;
    uint32_t pi = AHCI_HBA->pi;

    //printf("AHCI: Ports Implemented = 0x%x\n", pi);
    //printf("AHCI: Ports Implemented = %d\n", pi);

    for (i=0; i < NR_PORTS; i++)
    {
        if (pi & (1u << i))
        {
            volatile HBA_PORT *port = &AHCI_HBA->ports[i];

            printf("AHCI Port %d: sig=0x%x  ", i, port->sig);

            if (port->sig == 0x00000101)
                printf("[SATA]\n");
            else if (port->sig == 0xEB140101)
                printf("[SATAPI]\n");
            else
                printf("[Unknown]\n");

            // Setup port
            ahci_setup_port(i);
        }
    }

/*
// #todo: We need this.
    if (port->sig == 0x00000101 && BootDisk.boot_port < 0)
    {
        BootDisk.boot_port = i;
        printf("AHCI: Boot port candidate = %d\n", i);
    }
*/

}

// =====================================================

int ahci_initialize(void)
{
    printf("ahci_initialize:\n");

    if (BootDisk.ahci_bar5 == 0){
        printf("AHCI: BAR5 not configured!\n");
        return FALSE;
    }

// ----------------------------
// The main structure
    AHCI_HBA = (volatile HBA_MEM *) BootDisk.ahci_bar5;

    printf("AHCI HBA at 0x%x\n", BootDisk.ahci_bar5);
    printf("CAP=0x%x | PI=0x%x | VS=0x%x\n", 
           AHCI_HBA->cap, AHCI_HBA->pi, AHCI_HBA->vs);

    ahci_reset_hba();
    ahci_enable();
    ahci_probe_ports();


    BootDisk.initialized = TRUE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_AHCI;

    printf("AHCI: Initialization completed\n");
    return TRUE;
}

