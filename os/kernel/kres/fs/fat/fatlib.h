// fatlib.h
// Definitions and structures used by the fs code.
// Created by Fred Nora.

#ifndef  __FAT_LIB_H
#define  __FAT_LIB_H    1


// definições, variáveios, estruturas e protótipos
// específicos para o sistema de arquivos fat16.
// Isolando pra talvez tirarmos isso do kernel.
// fat16 é o sistema da partição de boot apenas.

// fat16 file attributes.
// See:
// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system

#define FAT_FILE_FLAG_READ            0x01
#define FAT_FILE_FLAG_HIDDEN          0x02
#define FAT_FILE_FLAG_SYSTEM          0x04   //HARDWARE.
#define FAT_FILE_FLAG_VOLUME_LABEL    0x08

#define FAT_FILE_FLAG_DIRECTORY       0x10
#define FAT_FILE_FLAG_FILE            0x20
#define FAT_FILE_FLAG_DEVICE          0x40   
#define FAT_FILE_FLAG_RESERVED        0x80


//
// FAT definitions.
//

//#define FAT_SECTOR_SIZE                 512
//#define FAT_FIRST_DATA_CLUSTER          2

//#define FAT16_CLUSTER_MASK              0xFFFF
//#define FAT16_LINK_TERMINATOR           0xFFFF

//#define FAT32_CLUSTER_MASK              0x0FFFFFFF
//#define FAT32_LINK_TERMINATOR           0x0FFFFFFF

#define FAT12_MASK  0x00000fff  /* mask for 12 bit cluster numbers */
#define FAT16_MASK  0x0000ffff  /* mask for 16 bit cluster numbers */
#define FAT32_MASK  0x0fffffff  /* mask for FAT32 cluster numbers */

#define FAT16_CLUSTER_AVAILABLE    0x0000
#define FAT16_CLUSTER_RESERVED     0xfff0
#define FAT16_CLUSTER_BAD          0xfff7
#define FAT16_CLUSTER_LAST         0xffff

#define FAT_DIRECTORY_ENTRY_FREE    0xE5
#define FAT_DIRECTORY_ENTRY_LAST    0x00

/*
#define FAT_DIRECTORY_ENTRY_FREE        0xE5
#define FAT_DIRECTORY_ENTRY_LAST        0x00

#define FAT_ATTRIBUTE_READ_ONLY         0x01
#define FAT_ATTRIBUTE_HIDDEN            0x02
#define FAT_ATTRIBUTE_SYSTEM            0x04
#define FAT_ATTRIBUTE_VOLUME_ID         0x08
#define FAT_ATTRIBUTE_DIRECTORY         0x10
#define FAT_ATTRIBUTE_ARCHIVE           0x20
#define FAT_ATTRIBUTE_LONG_NAME         (FAT_ATTRIBUTE_READ_ONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID)
#define FAT_ATTRIBUTE_MASK              0x3F

#define FAT_LONG_NAME_TERMINATOR        0x40
#define FAT_LONG_NAME_ORDER_MASK        0x3F
*/

//#define FAT_MAX_PATH                    255

// fat16 support.
#define FAT16_ROOT_ENTRIES  512  // 512 entradas no rootdir. 
#define FAT16_DEFAULT_NUMBER_OF_ENTRIES  FAT16_ROOT_ENTRIES

#define FAT16_ENTRY_SIZE    32   // Tamanho em bytes da entrada no root dir. 
#define FAT_ERRO_1    0xffff9 
#define FAT_ERRO_2    0xffff8
#define FAT_ERRO_3    0xffff7
#define FAT_ERRO_4    0xffff6
#define FAT_STATUS_1  0xffff5
#define FAT_STATUS_2  0xffff4
#define FAT_STATUS_3  0xffff3
#define FAT_STATUS_4  0xffff2
#define ENTRY_NEVER_USED       0x00
#define ENTRY_FILE_IS_DELETED  0xE5 
#define ENTRY_DIRECTORY        0x2E 
#define FILE_ATTRIBUTE_READ_ONLY     0x01 
#define FILE_ATTRIBUTE_HIDDEN        0x02
#define FILE_ATTRIBUTE_SYSTEM        0x04
#define FILE_ATTRIBUTE_VOLUME_LABEL  0x08
#define FILE_ATTRIBUTE_DIRECTORY     0x10
#define FILE_ATTRIBUTE_ARCHIVE       0x20
#define FILE_ATTRIBUTE_UNUSED1       0x40
#define FILE_ATTRIBUTE_UNUSED2       0x80

// FAT cache
#define FAT_CACHE_LOADED       1
#define FAT_CACHE_NOT_LOADED   0
#define FAT_CACHE_SAVED        1
#define FAT_CACHE_NOT_SAVED    0

// Boot partition.
extern int g_fat_cache_saved;
extern int g_fat_cache_loaded;


//#define CLUSTERS_TO_SAVE_MAX  (8*1024) //#bugbug
#define CLUSTERS_TO_SAVE_MAX  (32*1024)
// see: fat16.c
extern unsigned short fat16ClustersToSave[CLUSTERS_TO_SAVE_MAX];



//
// FAT16 directory entry
//

// Structure of the Directory Entries 

// Offset   Size      Description
// 00h      8 bytes   Filename
// 08h      3 bytes   Filename Extension
// 0Bh      1 bytes   Attribute Byte
// 0Ch      1 bytes   Reserved for Windows NT
// 0Dh      1 bytes   Creation - Millisecond stamp (actual 100th of a second)
// 0Eh      2 bytes   Creation Time
// 10h      2 bytes   Creation Date
// 12h      2 bytes   Last Access Date
// 14h      2 bytes   Reserved for FAT32
// 16h      2 bytes   Last Write Time
// 18h      2 bytes   Last Write Date
// 1Ah      2 bytes   Starting cluster
// 1Ch      4 bytes   File size in bytes

// SFN Entry
// see:
// http://elm-chan.org/docs/fat_e.html
struct fat16_directory_entry_d 
{

//Short file name (SFN). (8.3)
    unsigned char FileName[11];

/*
    File attribute in combination of following flags. 
    Upper 2 bits are reserved and must be zero.
0x01: ATTR_READ_ONLY (Read-only)
0x02: ATTR_HIDDEN (Hidden)
0x04: ATTR_SYSTEM (System)
0x08: ATTR_VOLUME_ID (Volume label)
0x10: ATTR_DIRECTORY (Directory)
0x20: ATTR_ARCHIVE (Archive)
0x0F: ATTR_LONG_FILE_NAME (LFN entry)
*/
    unsigned char Attributes;
/*
Optional flags that indicates case information of the SFN.
0x08: Every alphabet in the body is low-case.
0x10: Every alphabet in the extensiton is low-case.
*/
    unsigned char Reserved1; 

// Creation time (3 byte)
    unsigned char CreationMS;  //Optional sub-second information.
    unsigned short CreationTime; 
// Creation date. (2 byte)
    unsigned short CreationDate; 
// Optional last accesse date. (2 byte)
    unsigned short LastAccessDate; 

// (First Cluster High)
// Upper part of cluster number. 
// Always zero on the FAT12/16 volume.
// For fat32 
    unsigned short Reserved2;
// Modification time.
    unsigned short LastWriteTime;  
// Modification date.
    unsigned short LastWriteDate;  
// (First Cluster Low)
    unsigned short StartingCluster; 
// File size. 
    unsigned int FileSize; 
};  



// Um arquivo pode usar ate 21 entradas.
// 256 ?
//#define MAX_LFN_CHARACTERS  260
//#define MAX_LFN_DIRENTS     20

//#define FAT_LFN_ORDINAL_FIELD    0x40
//#define FAT_LFN_TYPE_FIELD       0x0  

// #todo
// LFN Entry
struct fat16_lfn_directory_entry_d 
{
    unsigned char  ordinal_field;    // offset 0  - ordinal.
    unsigned short name1[5];         // offset 1  - name part1 (unicode)
    // unsigned char name1[10];      // 
    unsigned char  flag_byte;        // offset 11 - Flag
    unsigned char  reserved;         // offset 12 - Reserved
    unsigned char  checksum;         // offset 13 - Checksum.
    unsigned short name2[6];         // offset 14 - name part2  (unicode)
    unsigned short thing;            // offset 26 - always '0'
    unsigned short name3[2];         // offset 28 - name part3   (unicode)
};

#endif    



