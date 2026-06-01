// ahci.c
// AHCI driver for Bootloader
// Created by Fred Nora

#include "../../../bl.h"


volatile HBA_MEM *AHCI_HBA = NULL;

struct ahci_port_d  ahci_port[NR_PORTS];
struct ahci_current_port_d  AHCICurrentPort;

// =====================================================

static void ahci_io_delay(void)
{
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

static void ahci_delay(int ms)
{
    int i=0;

    for (i=0; i < ms * 1000; i++)
        ahci_io_delay();
}

// =====================================================

static void ahci_reset_hba(void)
{
    if (!AHCI_HBA) 
        return;

    printf("AHCI: Resetting HBA...\n");
    AHCI_HBA->ghc |= (1 << 0);        // HR = 1

    while (AHCI_HBA->ghc & (1 << 0))
        ahci_io_delay();

    printf("AHCI: HBA Reset done\n");
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

    volatile HBA_PORT *port = &AHCI_HBA->ports[port_num];

    // Stop port
    port->cmd &= ~((1<<0) | (1<<4));   // Clear ST + FRE

    // Wait for CR + FR to clear
    while (port->cmd & ((1<<15) | (1<<14)))
        ahci_io_delay();

    // #todo: Allocate aligned memory properly in bootloader
    // For now, using a simple approach (you can improve later)

    // Command List Base (1KB aligned)
    unsigned long clb = (unsigned long) malloc(1024);
    if (!clb) return -1;
    memset((void*)clb, 0, 1024);

    // FIS Base (256B aligned)
    unsigned long fb = (unsigned long) malloc(256);
    if (!fb) return -1;
    memset((void*)fb, 0, 256);

    port->clb  = (uint32_t)clb;
    port->clbu = 0;
    port->fb   = (uint32_t)fb;
    port->fbu  = 0;

    // Start port
    port->cmd |= (1 << 4);   // FRE
    port->cmd |= (1 << 0);   // ST

    printf("AHCI: Port %d setup done\n", port_num);
    return 0;
}

// =====================================================

static void ahci_probe_ports(void)
{
    int i=0;
    uint32_t pi = AHCI_HBA->pi;
    printf("AHCI: Ports Implemented = 0x%x\n", pi);

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

    if (BootDisk.ahci_bar5 == 0)
    {
        printf("AHCI: BAR5 not configured!\n");
        return FALSE;
    }

    AHCI_HBA = (volatile HBA_MEM *) BootDisk.ahci_bar5;

    printf("AHCI HBA at 0x%x\n", BootDisk.ahci_bar5);
    printf("CAP=0x%x | PI=0x%x | VS=0x%x\n", 
           AHCI_HBA->cap, AHCI_HBA->pi, AHCI_HBA->vs);

    ahci_reset_hba();
    ahci_enable();

    ahci_probe_ports();

    BootDisk.initialized = TRUE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_AHCI;

    printf("AHCI: Initialization completed.\n");
    return TRUE;
}


