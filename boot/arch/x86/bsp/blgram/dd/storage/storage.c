// storage.c
// Storage support.
// Created by Fred Nora.

#include "../../bl.h"

// Main controller structure.
struct storage_controller_d  StorageController;


struct boot_disk_d  BootDisk;


// read_lba: 
// Read a LBA from the disk.
void read_lba( unsigned long address, unsigned long lba )
{
// Called by fsLoadFile in fs.c

    // if ( address == 0 ){}

    //printf("read_lba:\n");

    if (BootDisk.initialized != TRUE){
        printf("read_lba: BootDisk not initialized\n");
        goto fail;
    }

    int ControllerType = BootDisk.controller_type;
    int AHCI_Port = 0;  //BootDisk.boot_port;
    int NumberOfSectors = 1;  // #todo: parameterize this.

    switch (ControllerType)
    {
        case STORAGE_CONTROLLER_MODE_AHCI:
            // IN: port, lba. buffer, sector_count
            ahci_read_sector(AHCI_Port, lba, address, NumberOfSectors);
            break;

        // see: libata.c
        case STORAGE_CONTROLLER_MODE_ATA:
            //printf("STORAGE_CONTROLLER_MODE_ATA\n");
            // IN: address, lba, unused, unused
            ata_read_sector ( address, lba, 0, 0 );
            break;
        
        default:
            printf("read_lba: Invalid controller type\n");
            goto fail;
            break;
    }

//OK:
    return;

fail:
    return;
}

// write_lba: 
// Write a LBA into the disk.
void write_lba ( unsigned long address, unsigned long lba )
{
// Called by fsLoadFile in fs.c

    // if ( address == 0 ){}

    if (BootDisk.initialized != TRUE)
    {
        printf("write_lba: BootDisk not initialized\n");
        goto fail;
    }

    int ControllerType = BootDisk.controller_type;
    switch (ControllerType)
    {
        case STORAGE_CONTROLLER_MODE_AHCI:
            //ahci_write_sector(...)

            // #todo: We need this.
            printf("write_lba: AHCI write not implemented yet\n");
            refresh_screen();
            bl_die();

            break;

        case STORAGE_CONTROLLER_MODE_ATA:
            // IN: address, lba, unused, unused
            ata_write_sector ( address, lba, 0, 0 );
            break;
        
        default:
            printf("write_lba: Invalid controller type\n");
            goto fail;
            break;
    }

//OK:
    return;

fail:
    return;
}

// storagePCIScanDevice:
// Get the bus/dev/fun for a device given the class.

uint32_t storagePCIScanDevice(int class)
{
    uint32_t data = -1;
    int bus=0; 
    int dev=0; 
    int fun=0;

// =============
// Probe

    for ( bus=0; bus < 256; bus++ )
    {
        for ( dev=0; dev < 32; dev++ )
        {
            for ( fun=0; fun < 8; fun++ )
            {
                out32 ( PCI_PORT_ADDR, CONFIG_ADDR( bus, dev, fun, 0x8) );
                
                data = in32 (PCI_PORT_DATA);
                
                // #todo
                // We need a class variable outside the if statement.
                // ex: ClassValue = data >> 24 & 0xff;
                
                if ( ( data >> 24 & 0xff ) == class )
                {
                    // #todo: Save this information.
                    printf ("[ Detected PCI device: %s ]\n", 
                        pci_classes[class] );

                    // Done
                    
                    // #todo
                    // Put this into a variable.
                    
                    // XXXValue = ( fun + (dev*8) + (bus*32) );
                    // return (uint32_t) XXXValue;
                    
                    return (uint32_t) ( fun + (dev*8) + (bus*32) );
                }
            };
        };
    };

// Fail
    printf ("[ PCI device NOT detected ]\n");
    refresh_screen ();
    return (uint32_t) (-1);
}


// Called by main.c
int storage_initialize(void)
{
    int Status = FALSE;
    _u32 data=0;
    _u8 bus=0;
    _u8 dev=0;
    _u8 fun=0;

    // #todo:
    // This is what we need here.
    // We get the type and call the right initialization routine.
    // type = __ataPCIConfigurationSpace(..)

    data = (_u32) storagePCIScanDevice(PCI_CLASS_MASS);
// Error
    if (data == -1)
    {
        printf ("__ata_probe_controller: pci_scan_device fail. ret={%d}\n", 
            (_u32) data );

        // Abort
        Status = (int) (PCI_MSG_ERROR);
        printf("storage: No Mass Storage Controller found\n");
        refresh_screen();
        bl_die();
    }

    // Decode bus/dev/fun from PCI scan result
    bus = ( data >> 8 & 0xff );
    dev = ( data >> 3 & 31 );
    fun = ( data      & 7 );

// Getting device info

    // This is for vendor and device id.
    // data = (uint32_t) __ataReadPCIConfigAddr( bus, dev, fun, 0 );

    // Getting information about the PCI device.
    // class, subclass, prog if and revision id.
    data = (uint32_t) __ataReadPCIConfigAddr( bus, dev, fun, 8 );

    unsigned char __class    = (data >> 24) & 0xff;  // Class
    unsigned char __subclass = (data >> 16) & 0xff;  // Subclass

    if (__class != PCI_CLASS_MASS)
    {
        printf("storage: Not a PCI_CLASS_MASS\n");
        refresh_screen();
        bl_die();
    }

    // Reset BootDisk info
    BootDisk.initialized = FALSE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_UNKNOWN;
    BootDisk.ahci_bar5 = 0;
    BootDisk.boot_port = -1;

// -----------------------

// AHCI:
// Try initialization for the ahci mode.
// AHCI handles only SATA interface standard.
    if (__subclass == STORAGE_CONTROLLER_MODE_AHCI){

        printf("storage: AHCI Controller\n");


        // === CRITICAL: Read BAR5 ===
        BootDisk.ahci_bar5 = __ataReadPCIConfigAddr(bus, dev, fun, 0x24) & ~0xF;

        if (BootDisk.ahci_bar5 == 0)
        {
            printf("AHCI ERROR: BAR5 is zero!\n");
            refresh_screen();
            bl_die();
        }

        // Save PCI location
        BootDisk.bus = bus;
        BootDisk.dev = dev;
        BootDisk.fun = fun;
        BootDisk.controller_type = STORAGE_CONTROLLER_MODE_AHCI;

        // Enable Bus Master (recommended)
        // data = __ataReadPCIConfigAddr(bus, dev, fun, 0x04);
        // __ataWritePCIConfigAddr(bus, dev, fun, 0x04, data | 0x04);

        // #debug
        // refresh_screen();
        // bl_die();

        Status = (int) ahci_initialize();
        if (Status != TRUE){
            // Nothing in case of failure
        }

        //ahci_test_read(); // ok its working 

        // #debug
        // refresh_screen();
        // bl_die();


// ATA:
// Try initialization for the ata mode.
// ATA handles PATA and SATA intarface standards.
    } else if (__subclass == STORAGE_CONTROLLER_MODE_ATA){

        printf("storage: ATA Controller\n");

        // #debug
        // refresh_screen();
        // bl_die();

        Status = (int) ata_initialize();
        if (Status != TRUE){
            return FALSE;        
        }

// #todo
// More types ...
    // }else if (){

// Invalid or unsupported controller type.
    } else {
        printf("storage: Controller type not supported\n");
        refresh_screen();
        bl_die();
    }

    return TRUE;  // Done
}
