// pciinfo.c
// Created by Fred Nora.


// 80EE:CAFE is the VirtualBox Guest Additions PCI device. 
// It’s categorized under Class 8, Subclass 0x80 (Other Base System Peripheral), and 
// it exists purely to support VirtualBox’s host–guest integration features.

#include <kernel.h>  


// PCI Class Code Strings
// Indexed by class code [0x00–0xFF]
// Source: PCI Local Bus Specification

static const char* pci_class_strings[] = {
    /* 0x00 */ "Unclassified",
    /* 0x01 */ "Mass Storage Controller",
    /* 0x02 */ "Network Controller",
    /* 0x03 */ "Display Controller",
    /* 0x04 */ "Multimedia Controller",
    /* 0x05 */ "Memory Controller",
    /* 0x06 */ "Bridge Device",
    /* 0x07 */ "Simple Communication Controller",
    /* 0x08 */ "Base System Peripheral",
    /* 0x09 */ "Input Device Controller",
    /* 0x0A */ "Docking Station",
    /* 0x0B */ "Processor",
    /* 0x0C */ "Serial Bus Controller",
    /* 0x0D */ "Wireless Controller",
    /* 0x0E */ "Intelligent I/O Controller",
    /* 0x0F */ "Satellite Communication Controller",
    /* 0x10 */ "Encryption Controller",
    /* 0x11 */ "Signal Processing Controller",
    /* 0x12 */ "Processing Accelerator",
    /* 0x13 */ "Non-Essential Instrumentation",

    // 0x14–0x3F Reserved
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    /* 0x40 */ "Co-Processor",

    // 0x41–0xFE Reserved
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    /* 0xFF */ "Unassigned Class (Vendor Specific)",
};

// PCI Mass Storage Subclass Strings
// Indexed by subclass code [0x00–0xFF]
// Source: PCI Local Bus Specification

static const char* pci_mass_storage_subclass_strings[] = {
    /* 0x00 */ "SCSI Bus Controller",
    /* 0x01 */ "IDE Controller",
    /* 0x02 */ "Floppy Disk Controller",
    /* 0x03 */ "IPI Bus Controller",
    /* 0x04 */ "RAID Controller",
    /* 0x05 */ "ATA Controller (ADMA interface)",
    /* 0x06 */ "Serial ATA Controller",
    /* 0x07 */ "Serial Attached SCSI (SAS) Controller",
    /* 0x08 */ "Non-Volatile Memory Controller",   // e.g. NVMe
    /* 0x09 */ "Universal Flash Storage (UFS) Controller",
    /* 0x0A */ 0, // Reserved
    /* 0x0B */ 0, // Reserved
    /* 0x0C */ 0, // Reserved
    /* 0x0D */ 0, // Reserved
    /* 0x0E */ 0, // Reserved
    /* 0x0F */ 0, // Reserved
    /* 0x80 */ "Other Mass Storage Controller"
};

// PCI Bridge Subclass Strings
// Indexed by subclass code [0x00–0xFF]
// Source: PCI Local Bus Specification

static const char* pci_bridge_subclass_strings[] = {
    /* 0x00 */ "Host/PCI Bridge",
    /* 0x01 */ "PCI/ISA Bridge",
    /* 0x02 */ "PCI/EISA Bridge",
    /* 0x03 */ "PCI/Micro Channel Bridge",
    /* 0x04 */ "PCI/PCI Bridge",
    /* 0x05 */ "PCI/PCMCIA Bridge",
    /* 0x06 */ "PCI/NuBus Bridge",
    /* 0x07 */ "PCI/CardBus Bridge",
    /* 0x08 */ "PCI/RACEway Bridge",
    /* 0x09 */ "PCI/Semi-Transparent PCI-to-PCI Bridge",
    /* 0x0A */ "PCI/InfiniBand-to-PCI Host Bridge",
    /* 0x0B */ 0, // Reserved
    /* 0x0C */ 0, // Reserved
    /* 0x0D */ 0, // Reserved
    /* 0x0E */ 0, // Reserved
    /* 0x0F */ 0, // Reserved
    /* 0x80 */ "Other Bridge Device"
};

/*
 * pciInfo:
 *     Mostra as informações salvas nas estruturas da 
 * lista de dispositivos.
 * 0x2668  82801FB (ICH6) High Definition Audio Controller 0x8086 Intel.
 * 0x2829  Intel(R) ICH8M SATA AHCI Controller 0x8086 Intel.
 * 0x1237  PCI & Memory 0x8086 Intel.
 * ...
 */
// Uma lista com no máximo 32 ponteiros para estrutura 
// de dispositivo pci.

int pciInfo(void)
{
    struct pci_device_d *d;
    register int i=0;
    int Counter=0;
    const int Max = PCI_DEVICE_LIST_SIZE;

    int ClassCode=0;
    int SubclassCode=0;
    char *class_string;
    char *mass_subclass_string;
    char *bridge_subclass_string;

    printk ("pciInfo:\n");

    for (i=0; i<Max; i++)
    {
        d = (void *) pcideviceList[i];
        if ((void *) d != NULL)
        {
            if ( d->used == TRUE && d->magic == 1234 )
            {
                Counter++;
                printk ("\n");

                // #bugbug: What is the limit for this index?
                ClassCode = d->classCode;
                SubclassCode = d->subclass;
                class_string = 
                    (char *) pci_class_strings[ClassCode]; 
                mass_subclass_string = 
                    (char *) pci_mass_storage_subclass_strings[SubclassCode];
                bridge_subclass_string = 
                    (char *) pci_bridge_subclass_strings[SubclassCode];

                printk ("[%d:%d:%d] Vend=%x Dev=%x iLine=%d iPin=%d\n", 
                    d->bus, d->dev, d->func, d->Vendor, d->Device,
                    d->irq_line, d->irq_pin );

                if (ClassCode == PCI_CLASSCODE_MASS){
                    printk("Class={%s} Subclass={%s}\n", 
                        class_string, mass_subclass_string);
                } else if (ClassCode == PCI_CLASSCODE_BRIDGE) {
                    printk("Class={%s} Subclass={%s}\n", 
                        class_string, bridge_subclass_string);
                } else {
                    printk("Class={%s} Subclass={%d}\n", 
                        class_string, d->subclass);
                }
            }
        }
    };

    printk ("Done %d devices found\n",Counter);
    return 0;
fail:
    return (int) -1;
}

/*
 * pciShowDeviceInfo:
 *     Mostra informações sobre um dispositivo PCI da lista.
 *     Apenas um dispositivo.
 */
int pciShowDeviceInfo(int number)
{
    struct pci_device_d *D;

// Limits
    if ( number < 0 || number >= PCI_DEVICE_LIST_SIZE )
    {
        goto fail;
    }

// Get the pointer
    D = (void *) pcideviceList[number];
    if ((void *) D == NULL)
        goto fail;
    if (D->used != TRUE)
        goto fail;
    if (D->magic != 1234){
        goto fail;
    }

// Print
    printk ("Vend={%x} Dev={%x} ClassCode={%x} IntLine={%x}\n",
        D->Vendor, D->Device, D->classCode, D->irq_line );

// done
    return 0;
fail:
    return (int) -1;
}

//
// End
//

