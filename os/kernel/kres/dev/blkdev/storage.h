// storage.h
// Created by Fred Nora.

#ifndef  __STORAGE_H
#define  __STORAGE_H    1

// See storage.c
extern unsigned long gNumberOfSectorsInBootDisk;


struct storage_d
{
    int used;
    int magic;

// Fast access
// The number of sectors in the boot disk.
// See: storage_set_total_lba_for_boot_disk().
    unsigned long mumber_of_sectors_in_boot_disk;

// boot disk?

// Disk.
// System disk
// The disk where the system is installed.

    struct disk_d *system_disk;

// Volume.
// vfs volume.
// Boot volume.
// System volume.

    struct volume_d     *vfs_volume;
    struct volume_d    *boot_volume;
    struct volume_d  *system_volume;

// vfs
// virtual file system
    
    //struct vfs_d *vfs;

//
//  fs ???
//

// #bugbug
// Talvez podemos encontrar issa informação
// na estrutura de volume.

// file system
// Ponteiro para o sistema de arquivos atual.
// Se isso for NULL, então não temos sistema de arquivos.

    struct filesystem_d *fs;

//
// file ??
//

// Stream 
// ponteiro para o arquivo atual,
// que pode ser um arquivo, um diretório, um dispositivo ...
// tudo seguindo definições unix-like para esse tipo de estrutura.
// Na inicialização uma das estruturas deve ser apontada aqui.

// #test
// The rootdir.
// The file pointer represents the boot volume.
// This file is created when the fs/ module is initialized.
// see: fsInit() in fs.c.
    file *bootvolume_fp;
};


// Main structure for managing the storage information.
// See: 
// I_init() in x64init.c

struct storage_d  *storage;


//
// == prototypes ============================================
//



void read_lba( unsigned long address, unsigned long lba );
void write_lba( unsigned long address, unsigned long lba );


int
storage_read_sector( 
    unsigned long buffer, 
    unsigned long lba );

int
storage_write_sector( 
    unsigned long buffer, 
    unsigned long lba );


// Get the number of sectors in the boot disk
// and save it into a global variable, for now.
int storage_set_total_lba_for_boot_disk(void);

//
// $
// INITIALIZATION
//

int init_storage_support(void);

#endif    


