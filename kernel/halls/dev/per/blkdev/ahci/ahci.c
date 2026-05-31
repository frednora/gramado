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

    if ((void*)pci_ahci == NULL){
        printk("DDINIT_ahci: pci_ahci == NULL\n");
        return -1;
    }

    // Check controller type
    if (controller_type != STORAGE_CONTROLLER_MODE_AHCI){
        printk("DDINIT_ahci: Wrong controller type\n");
        return -1;
    }

    // BAR5 is the AHCI base address
    unsigned long bar5 = pci_ahci->BAR5 & ~0xF;

    AHCI_HBA = (volatile HBA_MEM *) bar5;

    if (!AHCI_HBA){
        printk("DDINIT_ahci: Invalid HBA address\n");
        return -1;
    }

    printk("AHCI: HBA at 0x%X | CAP=0x%X | PI=0x%X | VS=0x%X\n",
           bar5, AHCI_HBA->cap, AHCI_HBA->pi, AHCI_HBA->vs);

    // Reset and enable AHCI
    ahci_reset_hba();
    ahci_enable();

    // Setup ports
    ahci_probe_ports();

    g_ahci_driver_initialized = TRUE;

    printk("AHCI: Driver initialized successfully\n");
    return 0;
}

