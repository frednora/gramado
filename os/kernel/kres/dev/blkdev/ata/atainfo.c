
// atainfo.c
// Show information about ata support.
// 2021 - Created by Fred Nora.

#include <kernel.h>


void ata_show_ata_controller_info(void)
{
    printk("\n");

    if (g_ata_driver_initialized != TRUE)
        return;

// Print data.
    switch (AtaController.controller_type)
    {
        case ATA_IDE_CONTROLLER:
            printk("IDE controller\n");
            break;
        case ATA_RAID_CONTROLLER:
            printk("RAID controller\n");
            break;
        case ATA_AHCI_CONTROLLER:
            printk("AHCI controller\n");
            break;
        case ATA_UNKNOWN_CONTROLLER:
        default:
            printk("Unknown controller type\n");
            break;
    };
}

// ata_show_ide_info:
// Mostrar as informações obtidas 
// na inicializações do controlador.
void ata_show_ide_info(void)
{
    int i=0;

    //printk("\n");
    printk("\n");
    printk ("ata_show_ide_info: ide ports\n");

    for ( i=0; i<4; i++ )
    {
        printk("\n");
        printk ("id=%d \n", ide_ports[i].id );
        printk ("channel=%d dev_num=%d \n", 
            ide_ports[i].channel, 
            ide_ports[i].dev_num );
        //printk ("used=%d magic=%d \n", 
        //    ide_ports[i].used, 
        //    ide_ports[i].magic );
        printk ("type=%d      \n", ide_ports[i].type );
        printk ("base_port=%x \n", ide_ports[i].base_port );
        printk ("name=%s      \n", ide_ports[i].name );

// #todo: This is what we need.
        printk ("Size in sectors = %d \n", 
            ide_ports[i].size_in_sectors );
    };

//
// # debug.
//

// primary secondary  ... master slave
    // printk ( " channel=%d dev=%d \n", ata_controller.channel, ata_controller.dev_num );

/*
	// Estrutura 'ata'
	// Qual lista ??
	//pegar a estrutura de uma lista.

	//if( ata != NULL )
	//{
		printk("ata:\n");
 	    printk("type={%d}\n", (int) ata_controller.chip_control_type);
	    printk("channel={%d}\n", (int) ata_controller.channel);
	    printk("devType={%d}\n", (int) ata_controller.dev_type);
	    printk("devNum={%d}\n", (int) ata_controller.dev_num);
	    printk("accessType={%d}\n", (int) ata_controller.access_type);
	    printk("cmdReadMode={%d}\n", (int) ata_controller.cmd_read_modo);
	    printk("cmdBlockBaseAddress={%d}\n", (int) ata_controller.cmd_block_base_address);
	    printk("controlBlockBaseAddress={%d}\n", (int) ata_controller.ctrl_block_base_address);
		printk("busMasterBaseAddress={%d}\n", (int) ata_controller.bus_master_base_address);
		printk("ahciBaseAddress={%d}\n", (int) ata_controller.ahci_base_address);
	//};
*/
}


// Show info of the list of devices.
void ata_show_device_list_info(void)
{
    struct ata_device_d  *sd;
    unsigned long mb28=0;
    unsigned long mb48=0;

    printk("\n");
    //printk("ata_show_device_list_info:\n");
    printk("Printing information about all the\n");
    printk("ata devices in the list\n");

// The head of the list
    sd = (struct ata_device_d *) ready_queue_dev;
    
    while ( (void *) sd != NULL ){

    printk("\n");
    if(sd->boottime_device == TRUE){
        printk("\n");
        printk("The boot device is the port %d\n",sd->dev_nport);
    }

    printk("\n");

    // --------------------
    // The size in bytes.
    printk("Size in bytes: PORT %d: lba28{%d} lba48{%d}\n",
        sd->dev_nport, 
        sd->dev_total_num_sector,
        sd->dev_total_num_sector_lba48 );

    //#debug
    //printk("PORT %d: LBA28 v1{%d} v2{%d} \n",
        //sd->dev_nport, 
        //sd->lba28_value1,
        //sd->lba28_value2 );

    //#debug
    //printk("PORT %d: LBA48 v1{%d} v2{%d} v3{%d} v4{%d}\n",
        //sd->dev_nport, 
        //sd->lba48_value1,
        //sd->lba48_value2,
        //sd->lba48_value3,
        //sd->lba48_value4 );

    mb28 = (unsigned long) (((sd->dev_total_num_sector * 512)/1024)/1024);
    mb48 = (unsigned long) (((sd->dev_total_num_sector_lba48 * 512)/1024)/1024);

    // --------------------
    // The size in mega bytes.
    printk("Size in MB: LBA28 {%d MB} LBA48{%d MB}\n",
        mb28, mb48 );

    sd = (struct ata_device_d *) sd->next;

    };
}

void ata_show_ata_info_for_boot_disk(void)
{
    struct disk_d *disk;
    struct ata_device_d  *ata_device;

// --------------------------------
// Get the boot disk
    disk = (struct disk_d *) ____boot____disk;
    if ( (void*) disk == NULL )
    {
        printk("disk\n");
        return;
    }
    if (disk->magic != 1234)
    {
        printk("disk validation\n");
        return;
    }

// --------------------------------
    // Get the ata device information
    ata_device = (struct ata_device_d *) disk->ata_device;
    if ( (void*) ata_device == NULL )
    {
        printk("ata_device\n");
        return;
    }
    if (ata_device->magic != 1234)
    {
        printk("ata_device validation\n");
        return;
    }

// --------------------------------
    // Show the number of blocks.
    printk("Number of blocks: %d\n",
        ata_device->dev_total_num_sector );

// done
    refresh_screen();
}

