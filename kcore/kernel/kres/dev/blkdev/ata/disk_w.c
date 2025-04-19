
// disk_w.c
// Created by Fred Nora.

#include <kernel.h>

static int 
__do_save_sequence ( 
    int p,
    unsigned long buffer_va, 
    unsigned long lba, 
    size_t number_of_clusters );
  
// ----------------------------

static int 
__do_save_sequence ( 
    int p,
    unsigned long buffer_va, 
    unsigned long lba, 
    size_t number_of_clusters )
{
    int i=0;

// Arguments
    unsigned long buffer_base = (unsigned long) buffer_va;
    unsigned long buffer_off=0;
    unsigned long lba_base = (unsigned long) lba;
    unsigned long lba_off=0;
    size_t Total = (size_t) (number_of_clusters & 0xFFFFFFFF);
    //size_t Max=0; 

    //if (p<0)
    //    return -1;

// #todo:
// #bugbug:
// How much is the max number of cluster we can save 
// into this disk.

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
    
// ok. No errors.
    return 0;
}

/*
 * fatWriteCluster:
 *     Salva um cluster no disco.
 * Argumentos:
 *   setor   ~ Primeiro setor do cluster.
 *   address ~ Endereço do primeiro setor do cluster.
 *   spc     ~ Número de setores por cluster.
 */
// #todo
// Return 'int'.
// #bugbug: it is not only for fat... it is hw worker.

void 
fatWriteCluster ( 
    unsigned long sector, 
    unsigned long address, 
    unsigned long spc )
{
    unsigned long i=0;

    // #todo
    // We need some limits here for now.

    if (spc == 0){
        debug_print("fatWriteCluster: spc\n");
        return;
    }

    for (i=0; i < spc; i++)
    {
        write_lba( address, (sector + i) );
        address = (address +512); 
    };

    //...

    //return 0;  //#todo
}

// Save fat into the disk.
// Low level. It doesn't check the status of the fat cache.
int 
fs_save_fat ( 
    unsigned long fat_address, 
    unsigned long fat_lba, 
    size_t fat_size )
{

// #bugbug: 
// Debug provisório.
    debug_print ("fs_save_fat:\n");
    printk      ("Saving fat\n");
    refresh_screen();

// Filters

    if (fat_address == 0){
        panic("fs_save_fat: fat_address\n");
    }
    if (fat_lba == 0){
        panic("fs_save_fat: fat_lba\n");
    }
    // VOLUME1_FAT_SIZE
    // Only one size for now
    if (fat_size != VOLUME1_FAT_SIZE){
        panic("fs_save_fat: fat_size\n");
    }

// Do save!
// ata_get_current_ide_port_index()
    __do_save_sequence(
        __IDE_PORT, //port   // #bugbug: hard coded.
        (unsigned long) fat_address,
        (unsigned long) fat_lba,
        (size_t) fat_size );

// #bugbug: 
// Debug provisório.
    debug_print ("fs_save_fat: Done\n");
    printk      ("fs_save_fat: Done\n"); 
    refresh_screen ();

    return 0;
}

/*
 * fs_save_rootdir:
 *     Salva o diretório raiz no disco.
 *     @todo: Identificar parâmetros do sistema de arquivos atual. 
 */
int 
fs_save_rootdir ( 
    unsigned long root_address, 
    unsigned long root_lba, 
    size_t root_size )
{

// #bugbug: 
// Debug
    debug_print ("fs_save_rootdir:\n");
    printk      ("Saving rootdir\n");
    refresh_screen();

// Filters

    if (root_address == 0){
        panic("fs_save_rootdir: root_address\n");
    }
    if (root_lba == 0){
        panic("fs_save_rootdir: root_lba\n");
    }
    // 32 setores
    // 512 entradas de 32 bytes cada.
    // Only one size for now
    if (root_size != 32){
        panic("fs_save_rootdir: root_size\n");
    }

// Do save!
// ata_get_current_ide_port_index()
    __do_save_sequence(
        __IDE_PORT,
        (unsigned long) root_address,
        (unsigned long) root_lba,
        (size_t) root_size );

// #bugbug: 
// Debug provisório.
    debug_print ("fs_save_rootdir: Done\n");
    printk      ("fs_save_rootdir: Done\n"); 
    refresh_screen ();

    return 0;
}

//
// End
//

