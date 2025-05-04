// diskmap.h
// Disk sectors information.
// File system information for the first partition.
// 2015 - Created by Fred Nora.

/*
 * Refer�ncia para localiza��o de VBR, FAT, ROOT, DATAAREA ...
 * parti��o 0 - (8MB)
 * vbr  - ?  ;134  (depende do tanto de setores reservados.)
 * fat1 - 136
 * fat2 - 200
 * root - 264
 * data - 296
 */

//MBR
#define MBR_LBA  0
#define FS_MBR_LBA  MBR_LBA
 
// Obs: 
//     Valores definidos por imdisk. 
//     @todo: 
#define FS_VBR_LBA       63     //134
#define FS_FAT_LBA       67     //136 
#define FS_ROOTDIR_LBA   559    //264
#define FS_DATAAREA_LBA  591    //296 

//FAT32
#define FAT32_VBR_LBA       FS_VBR_LBA 
#define FAT32_FAT_LBA       FS_FAT_LBA 
#define FAT32_ROOTDIR_LBA   FS_ROOTDIR_LBA
#define FAT32_DATAAREA_LBA  FS_DATAAREA_LBA

//FAT16
#define FAT16_VBR_LBA       FS_VBR_LBA 
#define FAT16_FAT_LBA       FS_FAT_LBA 
#define FAT16_ROOTDIR_LBA   FS_ROOTDIR_LBA
#define FAT16_DATAAREA_LBA  FS_DATAAREA_LBA 
 
//FAT12
#define FAT12_VBR_LBA       FS_VBR_LBA 
#define FAT12_FAT_LBA       FS_FAT_LBA 
#define FAT12_ROOTDIR_LBA   FS_ROOTDIR_LBA
#define FAT12_DATAAREA_LBA  FS_DATAAREA_LBA 

//
//End.
//

