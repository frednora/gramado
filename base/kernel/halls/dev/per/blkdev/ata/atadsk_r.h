// atadsk_r.h
// ATA disk read
// Created by Fred Nora

#ifndef __ATA_ATADSK_R_H
#define __ATA_ATADSK_R_H    1


unsigned long 
fatClustToSect ( 
    unsigned short cluster, 
    unsigned long spc, 
    unsigned long first_data_sector );

void 
fatLoadCluster ( 
    unsigned long sector, 
    unsigned long address, 
    unsigned long spc );



int
__load_sequential_sectors ( 
    unsigned long address, 
    unsigned long lba, 
    unsigned long sectors );


// Load boot metafile into the memory
void 
atadsk_load_boot_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size_in_sectors );


// MBR
void fs_load_mbr(unsigned long mbr_address);

// FAT
void 
fs_load_fat(
    unsigned long fat_address, 
    unsigned long fat_lba, 
    size_t size_in_sectors );

// ROOT
void 
fs_load_rootdir(
    unsigned long root_address, 
    unsigned long root_lba, 
    size_t size_in_sectors );


#endif   

