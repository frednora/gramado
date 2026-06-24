// diskmap.h
// Disk sectors information.
// File system information for the first partition.
// 2015 - Created by Fred Nora.

// MBR  - 0
// VBR  - 63
// FAT1 - 67
// FAT2 - ?
// ROOTDIR  - 559
// DATAAREA - 591

// #ps: 
// We got these valoues by formating the disk using imdisk.

#define MBR_LBA  0
#define FS_MBR_LBA  MBR_LBA 
#define FS_VBR_LBA       63
#define FS_FAT_LBA       67 
#define FS_ROOTDIR_LBA   559
#define FS_DATAAREA_LBA  591

// FAT32
#define FAT32_VBR_LBA       FS_VBR_LBA 
#define FAT32_FAT_LBA       FS_FAT_LBA 
#define FAT32_ROOTDIR_LBA   FS_ROOTDIR_LBA
#define FAT32_DATAAREA_LBA  FS_DATAAREA_LBA

// FAT16
#define FAT16_VBR_LBA       FS_VBR_LBA 
#define FAT16_FAT_LBA       FS_FAT_LBA 
#define FAT16_ROOTDIR_LBA   FS_ROOTDIR_LBA
#define FAT16_DATAAREA_LBA  FS_DATAAREA_LBA 
 
// FAT12
#define FAT12_VBR_LBA       FS_VBR_LBA 
#define FAT12_FAT_LBA       FS_FAT_LBA 
#define FAT12_ROOTDIR_LBA   FS_ROOTDIR_LBA
#define FAT12_DATAAREA_LBA  FS_DATAAREA_LBA 

//
// End
//
