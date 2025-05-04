// storage.h 
// Created by Fred Nora.

#ifndef __DD_STORAGE_H
#define __DD_STORAGE_H    1

#define CONTROLLER_TYPE_ATA   1000
#define CONTROLLER_TYPE_AHCI  2000

struct boot_disk_d
{
    int initialized;
    int controller_type;
};
extern struct boot_disk_d  BootDisk;


// Read lba on ide device.
void read_lba( unsigned long address, unsigned long lba );
// White lba on ide device.
void write_lba( unsigned long address, unsigned long lba );


#endif    

