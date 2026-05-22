// atadsk_w.c
// ATA disk write
// Created by Fred Nora


#include <kernel.h>

static int 
__do_save_sequence ( 
    int p,
    unsigned long buffer_va, 
    unsigned long lba, 
    size_t number_of_sectors );
  
// ----------------------------

static int 
__do_save_sequence ( 
    int p,
    unsigned long buffer_va, 
    unsigned long lba, 
    size_t number_of_sectors )
{
    int i=0;

// Arguments
    unsigned long buffer_base = (unsigned long) buffer_va;
    unsigned long buffer_off=0;
    unsigned long lba_base = (unsigned long) lba;
    unsigned long lba_off=0;
    size_t Total = (size_t) (number_of_sectors & 0xFFFFFFFF);
    //size_t Max=0; 

    //if (p<0)
    //    return -1;

// #todo:
// #bugbug:
// How much is the max number of cluster we can save into this disk.

    unsigned int L_current_ide_port = 
        (unsigned int) ata_get_current_ide_port_index();


// Esperando antes do próximo.

    for (i=0; i<Total; i++)
    {
        // Waiting before the next.
        // #bugbug: 
        // Maybe this is make this process very slow in the vms.
        
        // #todo:
        // Maybe, do not use this on vms.
        // See: atairq.c
        // if (is_qemu != TRUE)
        disk_ata_wait_irq(p);

        ataWriteSector ( 
            L_current_ide_port,
            (unsigned long) ( buffer_base + buffer_off ), 
            (unsigned long) ( lba_base    + lba_off ), 
            0, 
            0 );

        // Update offsets.
        // Sector size is 512 and the cluster has only one sector for now.
        // #todo
        // We need different sizes of sectors and 
        // different n umber of spc.
        buffer_off += 0x200;  
        lba_off    += 1;
    };
    
// ok. No errors
    return 0;
}

void 
atadsk_store_boot_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size_in_sectors )
{

// Parameters
    if (buffer == 0){
        panic("atadsk_store_boot_metafile: buffer\n");
    }
    if (first_lba == 0){
        panic("atadsk_store_boot_metafile: first_lba\n");
    }
    // VOLUME1_FAT_SIZE
    // Only one size for now
    //if (size_in_sectors != ?){
    //    panic("atadsk_store_boot_metafile: size_in_sectors\n");
    //}

// Do save!
// ata_get_current_ide_port_index()
    __do_save_sequence(
        ATACurrentPort.g_current_ide_port_index,  // port
        (unsigned long) buffer,
        (unsigned long) first_lba,
        (size_t) size_in_sectors );
}

// Save FAT into the disk.
// Low level. It doesn't check the status of the fat cache.
int 
atadsk_save_fat ( 
    unsigned long fat_address, 
    unsigned long fat_lba, 
    size_t fat_size )
{

// #bugbug
    debug_print("atadsk_save_fat:\n");
    printk("Saving FAT\n");

// Parameters:
    if (fat_address == 0){
        panic("atadsk_save_fat: fat_address\n");
    }
    if (fat_lba == 0){
        panic("atadsk_save_fat: fat_lba\n");
    }
    // VOLUME1_FAT_SIZE
    // Only one size for now
    if (fat_size != VOLUME1_FAT_SIZE){
        panic("atadsk_save_fat: fat_size\n");
    }

// Do save! (ATA device)
// ata_get_current_ide_port_index()
    __do_save_sequence(
        ATACurrentPort.g_current_ide_port_index,  // port
        (unsigned long) fat_address,
        (unsigned long) fat_lba,
        (size_t) fat_size );

// #debug
    //debug_print("atadsk_save_fat: Done\n");
    //printk     ("atadsk_save_fat: Done\n"); 

    return 0;
}

// Save root dir into the disk
int 
atadsk_save_rootdir ( 
    unsigned long root_address, 
    unsigned long root_lba, 
    size_t root_size )
{

// #bugbug
    debug_print("atadsk_save_rootdir:\n");
    printk("Saving rootdir\n");

// parameters:
    if (root_address == 0){
        panic("atadsk_save_rootdir: root_address\n");
    }
    if (root_lba == 0){
        panic("atadsk_save_rootdir: root_lba\n");
    }
    // 32 setores
    // 512 entradas de 32 bytes cada.
    // Only one size for now
    if (root_size != 32){
        panic("atadsk_save_rootdir: root_size\n");
    }

// Save
// ata_get_current_ide_port_index()
    __do_save_sequence(
        ATACurrentPort.g_current_ide_port_index,
        (unsigned long) root_address,
        (unsigned long) root_lba,
        (size_t) root_size );    // Size in sectors.

// #debug
    //debug_print("atadsk_save_rootdir: Done\n");
    //printk     ("atadsk_save_rootdir: Done\n"); 

    return 0;
}

//
// End
//

