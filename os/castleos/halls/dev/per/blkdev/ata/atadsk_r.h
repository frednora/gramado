
// ATA disk read

#ifndef __ATA_ATADSK_R_H
#define __ATA_ATADSK_R_H    1

// Load boot metafile into the memory
void 
atadsk_load_boot_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size_in_sectors );


#endif   

