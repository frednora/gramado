// atadsk_w.h
// ATA disk write
// Created by Fred Nora

#ifndef __ATA_ATADSK_W_H
#define __ATA_ATADSK_W_H    1



// Save the boot metafile into the disk.
void 
atadsk_store_boot_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size_in_sectors );


int 
atadsk_save_fat ( 
    unsigned long fat_address, 
    unsigned long fat_lba, 
    size_t fat_size );

int 
atadsk_save_rootdir ( 
    unsigned long root_address, 
    unsigned long root_lba, 
    size_t root_size );

#endif   

