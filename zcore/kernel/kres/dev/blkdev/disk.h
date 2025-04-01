
// disk.h

#ifndef  __DISK_H
#define  __DISK_H    1

/*
LBA MAP:
Map for main LBA addresses in the system disk.
see: vd/fat/main.asm
--------------------------------------
    0 = mbr
--------------------------------------
Partition 0:
   63 = p0_vbr
   67 = p0_fat1
  313 = p0_fat2
  559 = p0_root
  591 = p0_dataarea
--------------------------------------
Partition 1:
  ...
--------------------------------------
Partition 2:
  ...
--------------------------------------
Partition 3:
  ...
--------------------------------------
*/  



#define SECTOR_SIZE    512  
//#define SECTOR_SIZE  4096  

#define MBR_BOOTABLE   0x80
#define MBR_SIGNATURE  0xAA55

#define DISK_COUNT_MAX 1024    //8

#define DISK_BYTES_PER_SECTOR  512 
//#define DISK_BYTES_PER_SECTOR 4096 


//
// == MBR =================================================
//


// MBR support:
// MBR for fat16 first partition.
// jmp, bpb, partition table, signature.

// jmp
#define  BS_JmpBoot       0  /* x86 jump instruction (3-byte) */

// name
#define  BS_OEMName       2  /* OEM name (8-byte) */

// bpb
#define  BPB_BytsPerSec  11  /* Sector size [byte] (WORD) */
#define  BPB_SecPerClus  13  /* Cluster size [sector] (BYTE) */
#define  BPB_RsvdSecCnt  14  /* Size of reserved area [sector] (WORD) */
#define  BPB_NumFATs     16  /* Number of FATs (BYTE) */
#define  BPB_RootEntCnt  17  /* Size of root directory area for FAT [entry] (WORD) */
#define  BPB_TotSec16    19  /* Volume size (16-bit) [sector] (WORD) */
#define  BPB_Media       21  /* Media descriptor byte (BYTE) */
#define  BPB_FATSz16     22  /* FAT size (16-bit) [sector] (WORD) */
#define  BPB_SecPerTrk   24  /* Number of sectors per track for int13h [sector] (WORD) */
#define  BPB_NumHeads    26  /* Number of heads for int13h (WORD) */
#define  BPB_HiddSec     28  /* Volume offset from top of the drive (DWORD) */
#define  BPB_TotSec32    32  /* Volume size (32-bit) [sector] (DWORD) */

// extra.
#define  BS_DrvNum       36  /* Physical drive number for int13h (BYTE) */
#define  BS_NTres        37  /* WindowsNT error flag (BYTE) */
#define  BS_BootSig      38  /* Extended boot signature (BYTE) */
#define  BS_VolID        39  /* Volume serial number (DWORD) */


// #todo: 11 bytes for DOS 4.0
// See:
// https://en.wikipedia.org/wiki/BIOS_parameter_block
#define  BS_VolLab       42  /* Volume label string (8-byte) */

//
// #bugbug: ??? Is this the right offset??
//#define  BS_FilSysType   50  /* Filesystem type string (8-byte) */ 
#define  BS_FilSysType   53  // starts here in dos 4.0

// #todo:
// Where is this restart point in gramado os ??
// boot code.
//#define  BS_BootCode     62  /* Boot code (448-byte) */
#define  BS_BootCode     62  



/* number of partitions per table partition  */
//#define N_PART 4 

//#define PARTOFF 0x1be

// partition table
// mbr partition table offsets.
#define MBR_Table       446  /* MBR: Offset of partition table in the MBR */ 
#define MBR_PT0_OFFSET  0x01BE  // (446) 
#define MBR_PT1_OFFSET  0x01CE  // (462)
#define MBR_PT2_OFFSET  0x01DE  // (478) 
#define MBR_PT3_OFFSET  0x01EE  // (494) 

//signature.
#define  BS_55AA         510  /* Signature word (WORD) */


//
//=============================================
//

//#define PARTITION_ACTIVE_FLAG    0x80
//#define MBR_PT_ACTIVE_FLAG   PARTITION_ACTIVE_FLAG


// Partition type
// See:
// https://en.wikipedia.org/wiki/Partition_type

#define MBR_PT_EMPTY   0x00
#define MBR_PT_FAT16   0x04
#define MBR_PT_FAT16B  0x06
// ...

//
// =========================================
//

//
// =========================================
//


//macro
//#define CYLMAX(c)  ((c) > 1023 ? 1023 : (c))  


/*
 * disk_type_t:
 *     Enumerando os tipos de disk.
 */
typedef enum {
    DISK_TYPE_NULL, 
    DISK_TYPE_PATA,
    DISK_TYPE_PATAPI,
    DISK_TYPE_SATA,
    DISK_TYPE_SATAPI
    //...
}disk_type_t;


// #Obs:
// Um disco pode ser físico ou virtual.
// Um disco virtual também pode ter muitos volumes virtuais.

typedef enum {
    DISK_CLASS_NULL,
    DISK_CLASS_PHYSICAL,
    DISK_CLASS_VIRTUAL
}disk_class_t;


/*
// bios parameter block
struct bpb_d
{
    int id;
    int used;
    int magic;
    //...
    struct bpb_d  *next;
};
*/

/*
 * partition_table_d:
 *     Structure for partition table.
 */ 
// See: https://wiki.osdev.org/Partition_Table

/*
Element (offset)  Size      Description
0                 byte      Boot indicator bit flag: 0 = no, 0x80 = bootable (or "active")

1                 byte      Starting head
2                 6 bits    Starting sector (Bits 6-7 are the upper two bits for the Starting Cylinder field.)
3                 10 bits   Starting Cylinder

4                 byte      System ID

5                 byte      Ending Head
6                 6 bits    Ending Sector (Bits 6-7 are the upper two bits for the ending cylinder field)
7                 10 bits   Ending Cylinder

8                 uint32_t  Relative Sector (to start of partition -- also equals the partition's starting LBA value)
12                uint32_t  Total Sectors in partition
*/

struct partition_table_d
{
// //0x80=active  0x00=inactive
    unsigned char active;
// #todo
// Talvez isso não importe se estivermos usando LBA.
    unsigned char start_chs[3];  //sizes:  8,6,10
    unsigned char type;
// #todo
// Talvez isso não importe se estivermos usando LBA.
    unsigned char end_chs[3];    //sizes:  8,6,10   
// Sectors between MBR and first sector.
    unsigned int start_lba;
// Sectors in partition.
// O tamanho da partição dado em setores.
// Se estivermos usando isso, então o CHS não importa.
    unsigned int size;
};

// #test
// see: storage.c
extern struct partition_table_d *system_disk_pt0;
extern struct partition_table_d *system_disk_pt1;
extern struct partition_table_d *system_disk_pt2;
extern struct partition_table_d *system_disk_pt3;
//extern struct partition_table_d *boot_partition; 
//extern struct partition_table_d *system_partition; 

// This is a good code.
// It is easy to handle the partition table values.

struct mbr_d
{
    unsigned char boot_code[446];
    struct partition_table_d p[4];
    unsigned short signature;
}; 

// MBR structure for the system disk.
// See: storage.c
extern struct mbr_d  *mbr; 


/*
 * disk_d:
 *     Estrutura para acesso rápido a discos.
 *     Deve ser simples e com poucos elementos.
 */
// disk info
struct disk_d
{ 
// Object header.
    object_type_t  objectType;
    object_class_t objectClass;

    file *fp;

// Structure validation
    int used;
    int magic;

    disk_type_t  diskType;
    disk_class_t diskClass;

    int id;                 // ID na lista de discos.
    char boot_disk_number;  // ID herdado do boot block.

// Ponteiro para o nome do disco,
// Talvez não precise ser um ponteiro, pode ser um array.
    char *name; 

//#todo
// se está funcionando ... se está inicializado ...
    //int status;

//#todo
// que tipo de operação esta sendo realizada. ou nenhuma.
// se ele está ocupoado o escretor terá que esperar.
    //int state;

// Security
    pid_t pid;     // Qual processo está usando.
    gid_t gid;
    // ...

//#todo
    //struct mbr_d mbr;
    //struct bpb_d bpb;
    //struct partition_table_d        partition_table;
    //struct partition_table_chars_d  partition_table_chars;

// #todo
// contador de processos usando o disco

    uint8_t channel;
    uint8_t dev_num;

//
// Capacity
//

// Number of sectors.
    unsigned long number_of_blocks;
// 512 or 4096?
    unsigned long bytes_per_sector;
// How many bytes in the whole disk.
    unsigned long size_in_bytes;


//
// Storage device structure.
//

// hadware info.
// See:
// ata.h
    struct ata_device_d  *ata_device;

//#todo
//volume list.

    // ...

    struct disk_d *next;
};

// Disks
// Structure for the system disk.
// See: storage.c
extern struct disk_d  *____boot____disk;
// ...

// Disk list.
// Essa lista é preenchida pelo driver de IDE.
// See: storage.c
extern unsigned long diskList[DISK_COUNT_MAX];


//
// == Prototypes ==================================
//

int disk_init (void);
void *disk_get_disk_handle ( int number );

struct partition_table_d *disk_get_partition_table(int index);
int disk_initialize_mbr_info(void);
void disk_show_mbr_info(void);

//
// Show info
//

void diskShowCurrentDiskInfo (void);
void disk_show_info (void);

#endif    



