// storage.c
// Storage support.
// Created by Fred Nora.

#include <kernel.h>

// Main structure for managing the storage information.
struct storage_d  *storage;
struct storage_controller_d  StorageController;

struct boot_disk_d  BootDisk;


// The number of sectors in the boot disk.
// See: storage_set_total_lba_for_boot_disk().
unsigned long gNumberOfSectorsInBootDisk=0;

const char* sda_string = "sda";
const char* sdb_string = "sdb";
const char* sdc_string = "sdc";
const char* sdd_string = "sdd";
// ...
const char* sdfail_string = "sd?";
// ...

// MBR structure for the system disk.
// see: disk.h
struct mbr_d  *mbr; 

// see: disk.h
struct partition_table_d *system_disk_pt0;
struct partition_table_d *system_disk_pt1;
struct partition_table_d *system_disk_pt2;
struct partition_table_d *system_disk_pt3;
//struct partition_table_d *boot_partition; 
//struct partition_table_d *system_partition; 
// ...

// VBR structure for the boot partition.
// See: volume.h
struct vbr_d  *vbr; 

//
// Disks ---------------------------
//

// Disks
// Structure for the system disk.
// See: disk.c
struct disk_d  *____boot____disk;
// ...

// Disk list.
// Essa lista é preenchida pelo driver de IDE.
// See: disk.h
unsigned long diskList[DISK_COUNT_MAX];

//
// Volumes ---------------------------
//

char *current_volume_string;
// volume atual ??
// Tipo de sistema de arquivos, fat16, ext2 ...
int g_currentvolume_filesystem_type=0;   //use this one.
// volume atual do tipo fat???
// Se é fat32, 16, 12.
int g_currentvolume_fatbits=0;

// #importante:
// Esses são os três volumes básicos do sistema 
// mesmo que o disco só tenha um volume, essas 
// estruturas vão existir.
// See: volume.h
struct volume_d  *volume_vfs;             // volume 0
struct volume_d  *volume_bootpartition;   // volume 1
struct volume_d  *volume_systempartition; // volume 2
// ...

// Volume list
// See: storage.c
unsigned long volumeList[VOLUME_COUNT_MAX];

//
// private functions: prototypes ============
//

static int __create_boot_partition(void);
static int __create_system_partition(void);
static int __create_vfs_partition(void);

static int __ShowDiskInfo(int index);
static int __ShowVolumeInfo(int index);

static int __disk_init(void);
static int __volume_init(void);

static int __validate_disksignature_from_bootblock(void);

// Get the number of sectors in the boot disk
// and save it into a global variable, for now.
static int storage_set_total_lba_for_boot_disk(void);
static int disk_initialize_mbr_info(void);

// =================================================

// Show disk information given its descriptor
static int __ShowDiskInfo(int index)
{
// Worker
    struct disk_d  *d;
    register int n = index;

    //#debug
    printk("\n");
    //printk ("__ShowDiskInfo:\n\n");

// Parameter
    if ( n < 0 || n >= DISK_COUNT_MAX )
    {
        printk("n fail\n");
        goto fail;
    }

// Get a pre-created disk structure.
// #bugbug
// When we created these structures?

    d = (struct disk_d *) diskList[n];

    if ((void *) d == NULL){
        printk("d\n");
        goto fail;
    }
    if ( d->used != TRUE || d->magic != 1234 )
    {
        printk("d validation\n");
        goto fail;
    }

// Show data
    printk("Disk (%d): Name {%s}\n", d->id, d->name );
// Basics
    printk("Bootdisk {%d}\n", d->boot_disk_number );
    printk("Type     {%d}\n", d->diskType );
    // ...
// Capacity
    printk("Number of blocks {%d}\n", d->number_of_blocks );
    printk("Byte per sector  {%d}\n", d->bytes_per_sector );
    printk("Size in bytes    {%d}\n", d->size_in_bytes );
    // ...
    printk("Done\n");
    return 0;

fail:
    printk("Fail\n");
    return (int) -1;
}

// local
// Create boot partition.
// Volume 1
static int __create_boot_partition(void)
{
    char name_buffer[32];

    // The main structure
    if ((void *) storage == NULL){
        panic ("__create_boot_partition: storage");
    }

// --------

//
// Volume 1 - Boot partition.
//

// Volume

    volume_bootpartition = (void *) kmalloc( sizeof(struct volume_d) );
    if ((void *) volume_bootpartition == NULL){
        panic("__create_boot_partition: volume_bootpartition\n");
    }
    memset ( volume_bootpartition, 0, sizeof(struct volume_d) );

// #todo:
    //volume_bootpartition->objectType = ?;
    //volume_bootpartition->objectClass = ?;

// Sera usado pelo VFS.
    volume_bootpartition->volumeType = VOLUME_TYPE_DISK_PARTITION;

    volume_bootpartition->id = BOOTPARTITION_VOLUME_ID;

//
// Disk
//

// The disk the volume belongs to.
    volume_bootpartition->disk = NULL;
// The volume belongs to the boot disk?
    if ((void*) ____boot____disk != NULL){
        volume_bootpartition->disk = (void*) ____boot____disk;
    }
    
// The filesystem used by this volue.
    volume_bootpartition->fs = NULL;

// #todo
// Volume limits.

    volume_bootpartition->__first_lba=0;
    volume_bootpartition->__first_lba=0;

    volume_bootpartition->VBR_lba  = VOLUME1_VBR_LBA;
    volume_bootpartition->FAT1_lba = VOLUME1_FAT_LBA;
    volume_bootpartition->FAT2_lba = 0;  //#bugbug
    volume_bootpartition->ROOT_lba = VOLUME1_ROOTDIR_LBA;
    volume_bootpartition->DATA_lba = VOLUME1_DATAAREA_LBA;

// Name

    //volume_bootpartition->name = "VOLUME 1 - BOOT";  
    ksprintf ( (char *) name_buffer, "VOLUME-%d",volume_bootpartition->id );
    volume_bootpartition->name = (char *) strdup((const char *) name_buffer);  

    //#todo
    volume_bootpartition->cmd = "#TODO";

// Current volume
    current_volume = volume_bootpartition->id;

// Finalizing
    volume_bootpartition->used = (int) TRUE;
    volume_bootpartition->magic = (int) 1234;
    volumeList[BOOTPARTITION_VOLUME_ID] = (unsigned long) volume_bootpartition; 
    storage->boot_volume = (struct volume_d *) volume_bootpartition;

    return 0;
}

// local
// Create system partition.
// Volume 2
static int __create_system_partition(void)
{
    char name_buffer[32];

// The main structure
    if ((void *) storage == NULL){
        panic ("__create_system_partition: storage");
    }

//
// Volume 2 - System partition.
//

// #bugbug: Isso esta errado.
// A partiçao do sistema precisa começar 
// logo apos a partiçao de boot, e a partiçao
// de boot tem 32MB.

// Volume

    volume_systempartition = (void *) kmalloc( sizeof(struct volume_d) );
    if ((void *) volume_systempartition == NULL){
        panic("__create_system_partition: volume_systempartition\n");
    }
    memset ( volume_systempartition, 0, sizeof(struct volume_d) );

//#todo:
    //volume_systempartition->objectType = ?;
    //volume_systempartition->objectClass = ?;

// Sera usado pelo VFS.
    volume_systempartition->volumeType = VOLUME_TYPE_DISK_PARTITION;

    volume_systempartition->id = SYSTEMPARTITION_VOLUME_ID;

//
// Disk
//

// The disk that the volume belongs to.
    volume_systempartition->disk = NULL;

// The filesystem used by this volue.
    volume_systempartition->fs = NULL;

// #todo
// Volume limits.

    volume_systempartition->__first_lba=0;
    volume_systempartition->__first_lba=0;

    volume_systempartition->VBR_lba  = VOLUME2_VBR_LBA;
    volume_systempartition->FAT1_lba = VOLUME2_FAT_LBA;
    volume_systempartition->FAT2_lba = 0;  //#bugbug
    volume_systempartition->ROOT_lba = VOLUME2_ROOTDIR_LBA;
    volume_systempartition->DATA_lba = VOLUME2_DATAAREA_LBA;

// Name

    //volume_systempartition->name = "VOLUME 2";  
    ksprintf ( (char *) name_buffer, "VOLUME-%d",volume_systempartition->id);
    volume_systempartition->name = (char *) strdup((const char *) name_buffer);

    //#todo 
    volume_systempartition->cmd = "#TODO";

// Finalizing
    volume_systempartition->used = (int) TRUE;
    volume_systempartition->magic = (int) 1234;
    volumeList[SYSTEMPARTITION_VOLUME_ID] = (unsigned long) volume_systempartition;
    storage->system_volume = (struct volume_d *) volume_systempartition; 

    return 0;
}

// local
// Create vfs partition
// Volume 0.
static int __create_vfs_partition(void)
{
    char name_buffer[32];

// The main structure
    if ((void *) storage == NULL){
        panic ("__create_vfs_partition: storage");
    }

//
// Volume 0 - vfs
//

// Volume

    volume_vfs = (void *) kmalloc( sizeof(struct volume_d) );
    if ((void *) volume_vfs == NULL){
        panic("__create_vfs_partition: volume_vfs\n");
    }
    memset ( volume_vfs, 0, sizeof(struct volume_d) );

    // #todo:
    //volume_vfs->objectType = ?;
    //volume_vfs->objectClass = ?;

    volume_vfs->id = VFS_VOLUME_ID;

// Sera usado pelo VFS.
    volume_vfs->volumeType = VOLUME_TYPE_BUFFER;

//
// Disk
//

// The disk that the volume belongs to.
    volume_vfs->disk = NULL;

// The filesystem used by this volue.
    volume_vfs->fs = NULL;

// #todo
// Volume limits.

    volume_vfs->__first_lba=0;
    volume_vfs->__first_lba=0;

// These fields are not used in a vfs.
    volume_vfs->VBR_lba=0;
    volume_vfs->FAT1_lba=0;
    volume_vfs->FAT2_lba=0;
    volume_vfs->ROOT_lba=0;
    volume_vfs->DATA_lba=0;

// Name

    //volume_vfs->name = "VOLUME 0"; 
    ksprintf ( (char *) name_buffer, "VOLUME-%d",volume_vfs->id);
    volume_vfs->name = (char *) strdup ( (const char *) name_buffer);  

// cmd #todo
    volume_vfs->cmd = "#TODO";

// Finalizing
    volume_vfs->used = (int) TRUE;
    volume_vfs->magic = (int) 1234;
    volumeList[VFS_VOLUME_ID] = (unsigned long) volume_vfs;
    storage->vfs_volume = (struct volume_d *) volume_vfs; 

    return 0;
}

struct partition_table_d *disk_get_partition_table(int index)
{
// #bugbug
// Only if the ata driver is already initialized.

    struct partition_table_d *pt;
    unsigned char *mbr_base = (unsigned char *) MBR_ADDRESS_VA; 

// #todo
// Parameter validation
    if (index < 0)
        return NULL;

    if (g_ata_driver_initialized != TRUE){
        panic("disk_get_partition_table: g_ata_driver_initialized\n");
    }

// Read from disk
    fs_load_mbr(MBR_ADDRESS_VA);
    
    /*
    // #debug
    int i=0;
    for (i=0; i<512; i++){
        printk("%c",mbr_base[i]);
    };
    printk("\n");
    */

// Partition table
    switch (index){
    case 0:
        pt = (struct partition_table_d *) (mbr_base + MBR_PT0_OFFSET);
        break;
    case 1:
        pt = (struct partition_table_d *) (mbr_base + MBR_PT1_OFFSET);
        break;
    case 2:
        pt = (struct partition_table_d *) (mbr_base + MBR_PT2_OFFSET);
        break;
    case 3:
        pt = (struct partition_table_d *) (mbr_base + MBR_PT3_OFFSET);
        break;
    default:
        return NULL;
        break;
    };

    //#debug
    //printk("Partition %d:\n",index);
    //printk("active %x\n", pt->active );
    //printk("start lba %d\n", pt->start_lba );
    //printk("size %x\n", pt->size );
    // ...

    return (struct partition_table_d *) pt;
};

static int disk_initialize_mbr_info(void)
{
// #bugbug
// Only if the ata driver is already initialized.

// + We are already getting the partition tables.
// #todo
// Get all the info in the mbr sector
// And save it in some variable and structures.

    printk ("disk_get_mbr_info:\n");

    if (g_ata_driver_initialized != TRUE){
        panic("disk_get_mbr_info: g_ata_driver_initialized\n");
    }

//
// Partition tables
//

    system_disk_pt0 = (struct partition_table_d *) disk_get_partition_table(0);
    if ((void*) system_disk_pt0 == NULL)
        goto fail;

    system_disk_pt1 = (struct partition_table_d *) disk_get_partition_table(1);
    if ((void*) system_disk_pt1 == NULL)
        goto fail;

    system_disk_pt2 = (struct partition_table_d *) disk_get_partition_table(2);
    if ((void*) system_disk_pt2 == NULL)
        goto fail;

    system_disk_pt3 = (struct partition_table_d *) disk_get_partition_table(3);
    if ((void*) system_disk_pt3 == NULL)
        goto fail;

    printk("done\n");
    //while(1){}

    return 0;

fail:
    return (int) -1;
}

void disk_show_mbr_info(void)
{

// partition table 0
    if ((void*) system_disk_pt0 == NULL)
        return;

    //#debug
    //printk("Partition %d:\n",index);
    printk("Active? %x\n", 
        system_disk_pt0->active );
    printk("Start LBA: %d\n", 
        system_disk_pt0->start_lba );
    printk("Size in sectors: %d\n", 
        system_disk_pt0->size );

    // #test: For sectors with 512 bytes each.
    unsigned int SizeInKB = (system_disk_pt0->size/2);
    printk("Size in KB: %d\n", SizeInKB);

    // ...
}

// #todo:
// What is the disk?
// What is the LBA limits?
// #todo:
// It needs to depends of the disk id.
// When found the disk id we have access to 
// the disk structure and the controller type.
// We're gonna call different functions depending 
// on the controller type.
int
storage_read_sector( 
    int disk_id,
    unsigned long buffer, 
    unsigned long lba )
{
    int res = -1;

// #todo
// Get the disk structure using the disk_id,
// and check the controller type.

    if (buffer == 0)
        goto fail;

    unsigned int L_current_ide_port = 
        (unsigned int) ata_get_current_ide_port_index();

    res = 
        (int) ataReadSector ( 
            L_current_ide_port,
            (unsigned long) buffer, 
            (unsigned long) lba, 
            0, 
            0 );

    return (int) res;
fail:
    return (int) -1;
}

// #todo:
// What is the disk?
// What is the LBA limits?
// #todo:
// It needs to depends of the disk id.
// When found the disk id we have access to 
// the disk structure and the controller type.
// We're gonna call different functions depending 
// on the controller type.
int
storage_write_sector( 
    int disk_id,
    unsigned long buffer, 
    unsigned long lba )
{
    int res = -1;

// #todo
// Get the disk structure using the disk_id,
// and check the controller type.

    if (buffer == 0)
        goto fail;

    unsigned int L_current_ide_port = 
        (unsigned int) ata_get_current_ide_port_index();

    res = 
        (int) ataWriteSector ( 
            L_current_ide_port,
            (unsigned long) buffer, 
            (unsigned long) lba, 
            0, 
            0 );

    return (int) res;

fail:
    return (int) -1;
}

// __disk_init:
// Initialize the disk manager.
// #bugbug
// #fixme
// The boot disk was not include in the diskList[]
// Probably the 'ide module' will discover all the ide disks
// and rebuild the list. This way all the information made here
// was gone. #fixme

static int __disk_init(void)
{
    struct disk_d *d;
    unsigned char BootDiskNumber=0;
    register int i=0;

    // #debug
    // printk ("__disk_init: Initializing..\n");

// Check the main structure
    if ((void *) storage == NULL){
        panic("__disk_init: storage\n");
    }
// Clean the disk list
    for (i=0; i<DISK_COUNT_MAX; i++){
        diskList[i] = 0;
    };

//
//  Disk structure
//

// Create and initialize the structure for the disk

    d = (void *) kmalloc( sizeof(struct disk_d) );

    if ((void *) d == NULL){
        panic("__disk_init: d\n");
    }
    memset ( d, 0, sizeof(struct disk_d) );
    d->used = (int) TRUE;
    d->magic = (int) 1234;

// ID:
// This is the disk number in the list.
// This is the boot disk. So, it will be the number '0'.
// Setup the current disk.
// Put the pointer into the list.

    d->id = 0;
    current_disk = d->id;
    diskList[current_disk] = (unsigned long) d;

// Type
    d->diskType = DISK_TYPE_NULL;

// Get the number of the boot disk.
// This info was provide by BIOS at boot time.
// That is why we call this 'boot_disk_number'.
// See: info.c
// #bugbug #todo
// Nao chamar um metodo fora desse modulo 
// para realizar esse trabalho.

    //#bugbug: This methoc gets info from a non initialized structure.
    // see: BootBlock in info.h
    //d->boot_disk_number = (char) info_get_boot_info(3);
    d->boot_disk_number = -1;  //#fail

    BootDiskNumber = (char) d->boot_disk_number;

// #bugbug: if d->name is a ponter we need to point to
// a const well define string, or create a new one.

    /*
    switch (BootDiskNumber){
    case 0x80:  d->name = sda_string;  break;
    case 0x81:  d->name = sdb_string;  break;
    case 0x82:  d->name = sdc_string;  break;
    case 0x83:  d->name = sdd_string;  break;
    default:
        debug_print("__disk_init: [FAIL] default boot disk number\n");
        d->name = sdfail_string;
        break;
    };
    */

    // Default name
    d->name = sdfail_string;

    d->next = NULL;

// This will become the system disk.
// #bugbug
// Is this structure initialized?
    if ((void*) storage != NULL){
        storage->system_disk = (struct disk_d *) d;
    }

// This will become the boot disk.
// global
    ____boot____disk = (struct disk_d *) d;

// #todo
// Maybe this is the right moment to save the pointer
// into the list.
    //diskList[ current_disk ] = (unsigned long) d;

   //more?

    printk("Done\n");
    return 0;
}

// Show info for all disks in the list.
void disk_show_info(void)
{
// Called by service 251

    struct disk_d *disk;
    register int i=0;

    for (i=0; i<DISK_COUNT_MAX; i++)
    {
        disk = (struct disk_d *) diskList[i];
        if ((void *) disk != NULL){
            __ShowDiskInfo(i);
        }
    };
}

void diskShowCurrentDiskInfo(void)
{
    if (current_disk < 0)
        return;

    printk("The current disk is {%d}.\n", current_disk );
    __ShowDiskInfo(current_disk);
}

// Get the pointer for the disk structure given the index
void *disk_get_disk_handle(int number)
{
    if ( number < 0 || 
         number >= DISK_COUNT_MAX )
    {
        return NULL;
    }

    return (void *) diskList[number];
}

// worker: Initialize volume support.
static int __volume_init(void)
{
    int Status = -1;  //fail
    register int i=0;
    char name_buffer[32];

// The main structure
    if ((void *) storage == NULL){
        panic ("__volume_init: storage");
    }
// Clear the list
    for (i=0; i<VOLUME_COUNT_MAX; i++){
        volumeList[i] = 0;
    };

// --------

// Volume 0 - vfs
    Status = (int) __create_vfs_partition();
    if (Status<0){
        panic("__volume_init: __create_vfs_partition fail\n");
    }

// Volume 1 - Boot partition.
    Status = (int) __create_boot_partition();
    if (Status<0){
        panic("__volume_init: __create_boot_partition fail\n");
    }

// Volume 2 - System partition.
    Status = (int) __create_system_partition();
    if (Status<0){
        panic("__volume_init: __create_system_partition fail\n");
    }

    return 0;  // Done
}

static int __ShowVolumeInfo(int index)
{
    struct volume_d *v;

    // #debug
    printk("\n");
    //printk ("__ShowVolumeInfo:\n");

// Parameter
    if ( index < 0 || 
         index >= VOLUME_COUNT_MAX )
    {
        printk("index fail\n");
        goto fail;
    }

// Structure validation
    v = (struct volume_d *) volumeList[index];
    if ((void *) v == NULL){
        printk ("v\n");
        goto fail;
    }
    if (v->used != TRUE || v->magic != 1234)
    {
        printk("v validation\n");
        goto fail;
    }

// Show data

// Name
    printk ("Volume (%d): Name {%s}\n", v->id, v->name);
// Basics
    printk ("Type     {%d}\n", v->volumeType);
// LBAs
    printk ("VBR_lba  {%d}\n", v->VBR_lba );
    printk ("FAT2_lba {%d}\n", v->FAT1_lba);
    printk ("FAT2_lba {%d}\n", v->FAT2_lba);
    printk ("ROOT_lba {%d}\n", v->ROOT_lba);
    printk ("DATA_lba {%d}\n", v->DATA_lba);
    // ...

    return 0;  // Done

fail:
    printk("Fail\n");
    return (int) -1;
}

int volumeShowVolumeInfo(int descriptor)
{
    if (descriptor < 0)
        return (int) -1;
    return (int) __ShowVolumeInfo(descriptor);
}

void volumeShowCurrentVolumeInfo(void)
{
    if (current_volume < 0)
        return;

    printk ("The current volume is %d\n", current_volume );
    __ShowVolumeInfo (current_volume);
}

// Show info for all volumes in the list
void volume_show_info (void)
{
    struct volume_d *volume;
    register int i=0;

    for (i=0; i<VOLUME_COUNT_MAX; i++)
    {
        volume = (struct volume_d *) volumeList[i];
        if ((void *) volume != NULL){
            volumeShowVolumeInfo(i);
        }
    };
}

// Get pointer for a volume, given its index.
void *volume_get_volume_handle(int number)
{
    if ( number < 0 || 
         number >= VOLUME_COUNT_MAX )
    {
        return NULL;
    }

// Get pointer
    return (void *) volumeList[number];
}

// Get pointer for current volume
void *volume_get_current_volume_info (void)
{
    if ( current_volume < 0 || 
         current_volume > VOLUME_COUNT_MAX )
    {
        return NULL;
    }

// Get pointer
    return (void *) volumeList[VOLUME_COUNT_MAX];
}

/*
 * read_lba:
 *     Load a sector into memory, given the LBA.
 * IN:
 *   disk_id = disk id. (NOT the IDE ports!)
 *   address = physical address of the buffer.
 *   lba     = logical block address.
 */

// #todo:
// Talvez essa rotina tenha que ter algum retorno no caso de falhas. 
// #bugbug
// Essa rotina e' independente do sistema de arquivos.
// Change name to dest_buffer
// #bugbug
// Disk info?
// Qual é o disco?
// Qual é a porta IDE?
// ...
// #todo
// use 'buffer_va'.
// #todo:
// It needs to depends of the disk id.
// When found the disk id we have access to 
// the disk structure and the controller type.
// We're gonna call different functions depending 
// on the controller type.
int 
read_lba ( 
    int disk_id,
    unsigned long address, 
    unsigned long lba )
{

// #todo: 
// Check lba limits


// #todo
// In the case of reading from AHCI disks use this.
// #todo: We gotta see how the boot loader is handling
// these workers.
// ahci_read_sector(0,0,buf,1)

// #todo
// Each disk has information about its type.
// And the only supported type for now is IDE.
// The workers here operate on ATA disks.

    /*
    // disk id. (NOT the IDE ports!)
    if (disk_id < 0){
        debug_print ("read_lba: [FAIL] disk_id\n");
        goto fail;
    }   
    */

    if (address == 0){
        debug_print ("read_lba: [FAIL] Limits\n");
        goto fail;
    }

// #ps:
// All we have for now is IDE support.
// This routine is getting the port for the boot disk.
    unsigned int L_current_ide_port = 
        (unsigned int) ata_get_current_ide_port_index();

// Is it FAT12/FAT16/FA32 boot disk?
// Is it a FAT disk at all?
// See: volume.h

    switch (g_currentvolume_fatbits){

    case 32:
        debug_print ("read_lba: [FAIL] FAT32 not supported\n");
        goto fail;
        break;

    case 16:
        // #todo: return value.
        // #todo: 
        // IN: buffer, lba, reserved1, reserved2.
        // #see: ata.c
        ataReadSector ( 
            L_current_ide_port, 
            address, 
            lba, 
            0, 
            0 );

        return 0;  // ok
        break;

    // Nothing.
    case 12:
        debug_print ("read_lba: [FAIL] FAT12 not supported\n");
        goto fail;
        break;

    default:
        debug_print ("read_lba: [FAIL] g_currentvolume_fatbits not supported\n");
        panic ("read_lba: default\n");
        //goto fail;
        break;
    };

    // Nothing

fail:
    return (int) -1;
}

/*
 * write_lba:
 *     Write a sector from memory to the disk, given the LBA.
 * IN:
 *   disk_id = disk id. (NOT the IDE ports!)
 *   address = physical address of the buffer.
 *   lba     = logical block address.
 */

// #bugbug
// Essa rotina e' independente do sistema de arquivos.
// #todo: use 'int' return.
// #todo:
// It needs to depends of the disk id.
// When found the disk id we have access to 
// the disk structure and the controller type.
// We're gonna call different functions depending 
// on the controller type.
int 
write_lba( 
    int disk_id,
    unsigned long address, 
    unsigned long lba )
{

// #todo: 
// Check lba limits

// #todo
// Each disk has information about its type.
// And the only supported type for now is IDE.
// The workers here operate on ATA disks.

// #todo
// In the case of writing from AHCI disks use this.
// #todo: We gotta see how the boot loader is handling
// these workers.
// ahci_write_sector(0,0,buf,1)


    /*
    // disk id. (NOT the IDE ports!)
    if (disk_id < 0){
        debug_print ("read_lba: [FAIL] disk_id\n");
        goto fail;
    }   
    */

    if (address == 0){
        debug_print ("write_lba: [FAIL] Limits\n");
        goto fail;
    }

// #ps:
// All we have for now is IDE support.
// This routine is getting the port for the boot disk.
    unsigned int L_current_ide_port = 
        (unsigned int) ata_get_current_ide_port_index();

    // See: volume.h
    switch (g_currentvolume_fatbits){

    case 32:
        printk ("write_lba: [ERROR] FAT32 not supported\n");
        goto fail;
        break;

    case 16:
        // We need to have the information 
        // about the controller type in order to decide 
        // which function we gonna use.
        // #see: ata.c
        // IN: buffer, lba, reserved1, reserved2.
        ataWriteSector (
            L_current_ide_port,
            address, 
            lba, 
            0, 
            0 ); 
        return 0; // ok
        break;

    case 12:
        printk ("write_lba: [ERROR] FAT12 not supported\n");
        goto fail;
        break;

    default:
        printk ("write_lba: default\n");
        panic ("read_lba: default\n");
        //goto fail;
        break;
    };

// Nothing

fail:
    return (int) -1;
}

/*
 * storageWriteCluster:
 *     Save cluster into the disk.
 * IN:
 *   setor   ~ Primeiro setor do cluster.
 *   address ~ Endereço do primeiro setor do cluster.
 *   spc     ~ Número de setores por cluster.
 */
// #bugbug: 
// It is not only for fat ... it is hw worker.

int
storageWriteCluster ( 
    unsigned long sector, 
    unsigned long address, 
    unsigned long spc )
{
    unsigned long i=0;

// #ps:
// This is a FAKE disk id
    const int FakeDiskId = 0;

    // #todo
    // We need some limits here
    if (spc == 0){
        debug_print("storageWriteCluster: spc\n");
        goto fail;
    }

    for (i=0; i<spc; i++)
    {
        // IN: disk id, address, LBA.
        write_lba( FakeDiskId, address, (sector + i) );
        // Next
        address = (address +512); 
    };

    // ...

    return 0;  // OK

fail:
    return (int) -1;
}

// Get the number of sectors in the boot disk and 
// save it into a global variable, for now.
static int storage_set_total_lba_for_boot_disk(void)
{
    struct disk_d *disk;
    struct ata_device_d  *ata_device;

// --------------------------------
// Get the boot disk

    disk = (struct disk_d *) ____boot____disk;

    if ((void*) disk == NULL){
        printk("disk\n");
        goto fail;
    }
    if (disk->magic != 1234){
        printk("disk validation\n");
        goto fail;
    }

// --------------------------------
// Get the ata device information

    ata_device = (struct ata_device_d *) disk->ata_device;

    if ((void*) ata_device == NULL){
        printk("ata_device\n");
        goto fail;
    }
    if (ata_device->magic != 1234){
        printk("ata_device validation\n");
        goto fail;
    }

// --------------------------------
// #debug: Show the number of blocks in this device.

    printk("Number of blocks: %d\n",
        ata_device->dev_total_num_sector );

// Set global variable

    gNumberOfSectorsInBootDisk = 
        (unsigned long) ata_device->dev_total_num_sector;

// Save it into the main storage structure

    if ((void*) storage == NULL){
        printk("storage\n");
        goto fail;
    }
    storage->mumber_of_sectors_in_boot_disk = 
        (unsigned long) ata_device->dev_total_num_sector;

    return TRUE;

fail:
    return FALSE;
}

// This signature came from bootblock.
// We're gonna use it to compare what?
// #debug: We need more attention on this routine.
static int __validate_disksignature_from_bootblock(void)
{

//
// Quick test. (#debug)
//

// #test
// Receiving the signature per se, not the address anymore.
// #todo:
// we're gonna probe the 4 ATA ports in order to find
// this signature in one of the disks.

    // >>>>> Not the address <<<<<
    // Print 8 bytes.
    // #bugbug: >>> printk is not able to print 64bit values.
    unsigned char *value64bit = 
        (unsigned char *) &bootblk.disk_signature;

    if (value64bit[0] != 0xEE)
        panic ("Signature 0xEE");
    if (value64bit[1] != 0xAA)
        panic ("Signature 0xAA");
    if (value64bit[2] != 0xD8)
        panic ("Signature 0xD8");

    /*
    printk ("The signature: %x\n", value64bit[0] );
    printk ("The signature: %x\n", value64bit[1] );
    printk ("The signature: %x\n", value64bit[2] );
    printk ("The signature: %x\n", value64bit[3] );
    printk ("The signature: %x\n", value64bit[4] );
    printk ("The signature: %x\n", value64bit[5] );
    printk ("The signature: %x\n", value64bit[6] );
    printk ("The signature: %x\n", value64bit[7] );
    //while(1){}
    */

    return 0;  //ok
}

// storagePCIScanDevice:
// Get the bus/dev/fun for a device given the class.

uint32_t storagePCIScanDevice(int class)
{
    uint32_t data = -1;
    int bus=0; 
    int dev=0; 
    int fun=0;

// =============
// Probe

    for ( bus=0; bus < 256; bus++ )
    {
        for ( dev=0; dev < 32; dev++ )
        {
            for ( fun=0; fun < 8; fun++ )
            {
                out32 ( PCI_PORT_ADDR, __PCI_CONFIG_ADDR( bus, dev, fun, 0x8) );
                
                data = in32 (PCI_PORT_DATA);
                
                // #todo
                // We need a class variable outside the if statement.
                // ex: ClassValue = data >> 24 & 0xff;
                
                if ( ( data >> 24 & 0xff ) == class )
                {
                    // #todo: Save this information.
                    //printk ("[ Detected PCI device: %s ]\n", 
                        //pci_classes[class] );

                    // Done
                    
                    // #todo
                    // Put this into a variable.
                    
                    // XXXValue = ( fun + (dev*8) + (bus*32) );
                    // return (uint32_t) XXXValue;
                    
                    return (uint32_t) ( fun + (dev*8) + (bus*32) );
                }
            };
        };
    };

// Fail
    printk ("[ PCI device NOT detected ]\n");
    //refresh_screen ();
    return (uint32_t) (-1);
}


// storagePCISetupMassStorageController:
// Espaço de configuraçao PCI Mass Storage.
// Nessa rotina:
// + Encontra o tipo de driver, ser é IDE, RAID, AHCI ou Desconhecido.
// It gets all the information for the PCI device structure.
// + Make some initialization if it finds an ATA/IDE device
// + Make some initialization if it finds an AHCI device

int storagePCISetupMassStorageController(struct pci_device_d *D)
{
// Called by __ata_initialize().

    uint32_t data=0;

// A estrutura ainda nao foi configurada.
    //ata_port.used = FALSE;
    //ata_port.magic = 0;

// We still don't know the type of this controller.
// But the caller already knows that 
// it's a mass storage device, and ide.
    StorageController.controller_type = (uint8_t) STORAGE_CONTROLLER_MODE_UNKNOWN;

// Check parameters.
    if ((void *) D == NULL){
        printk("atapciConfigurationSpace: D\n");
        goto fail;
    }
    if ( D->used != TRUE || D->magic != 1234 ){
        printk ("atapciSetupMassStorageController: D validation\n");
        goto fail;
    }

// Indentification Device
    //data = (uint32_t) pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0 );

// Salvando configurações.
    //D->Vendor = data &0xffff;
    //D->Device = data >> 16 &0xffff;

    // #debug
    // printk ("Disk info: [ Vendor ID: %x,Device ID: %x ]\n", 
    //     D->Vendor, D->Device );

/*
    if ( D->Vendor == 0x1106 && D->Device == 0x0591 ){
        printk ("VIA disk found\n");
    } else if (D->Vendor == 0x1106 && D->Device == 0x0591){
        // ...
    };
 */

// Obtendo informações.
// Classe code, programming interface, revision id.

    //data  = (uint32_t) pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );

// Saving info.
// Classe e sub-classe.
// prog if.
// Revision.

    //ata_pci->classCode  = data >> 24 & 0xff;
    //ata_pci->subclass   = data >> 16 & 0xff;
    //ata_pci->progif     = data >> 8  & 0xff;
    //ata_pci->revisionId = data       & 0xff;

// #importante:
// Aqui detectamos o tipo de dispositivo com base 
// nas informações de classe e subclasse.

//
// == #SCSI ========
//

// ====
    // 1:0 = SCSI controller.
    if ( D->classCode == PCI_CLASSCODE_MASS && 
         D->subclass == PCI_SUBCLASS_SCSI ){

        StorageController.controller_type = (uint8_t) PCI_SUBCLASS_SCSI;
        printk ("atapciSetupMassStorageController: SCSI not supported\n");
        goto fail;

//
// == #IDE ========
//

// ====
    // 1:1 = IDE controller.
    } else if ( D->classCode == PCI_CLASSCODE_MASS && 
                D->subclass == PCI_SUBCLASS_IDE ){

        // #type: (ATA).
        StorageController.controller_type = (uint8_t) STORAGE_CONTROLLER_MODE_ATA; 

        // Compatibilidade e nativo, primary.
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x200 ){ 
            pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 8, data | 0x100 ); 
        }

        // Compatibilidade e nativo, secundary.
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x800 ){ 
            pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 8, data | 0x400 ); 
        }

        //#todo: Comment.
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x8000 ){
            // Bus Master Enable
            data = pci_ReadPCIConfigAddr (D->bus, D->dev, D->func, 4);
            pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 4, data | 0x4);
        }

        // Habilitar interrupcao (INTx#)
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 4 );
        pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 4, data & ~0x400);

        // IDE Decode Enable
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x40 );
        pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 0x40, data | 0x80008000 );

        // Synchronous DMA Control Register
        // Enable UDMA
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x48 );
        pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 0x48, data | 0xf);

        // #debug
        // printk ("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
        //    ata_sub_class_code_register_strings[StorageController.controller_type],
        //    ata_pci.progif,
        //    ata_pci.revisionId );

//
// == #RAID ========
//

// #todo
// Devemos falhar, 
// pois não daremos suporte à IDE RAID.

// ====
    // 1:4 = RAID controller
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_RAID ){

        // #type: (ATA RAID).
        StorageController.controller_type = (uint8_t) STORAGE_CONTROLLER_MODE_RAID;
        printk ("atapciSetupMassStorageController: ATA RAID not supported\n");
        goto fail;

//
//  # ACHI
//

// #todo
// Devemos falhar, pois temos outro driver 
// para esse tipo de controlador.

// ====
    // 1:6 = SATA controller.
    } else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_SATA ){

        // #type (ACHI)
        StorageController.controller_type = (uint8_t) STORAGE_CONTROLLER_MODE_AHCI;

        // Compatibilidade e nativo, primary.
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x200 ){ 
            pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 8, data | 0x100 ); 
        }

        // Compatibilidade e nativo, secundary.
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x800 ){ 
            pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 8, data | 0x400 ); 
        }

        // ??#todo: Comment
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x8000 ) 
        {    
            // Bus Master Enable.
            data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 4 );
            pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 4, data | 0x4 );
        } 

        // IDE Decode Enable.
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x40 );
        pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 0x40, data | 0x80008000 );

        // Habilitar interrupcao (INTx#)
        data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 4 );
        pci_WritePCIConfigAddr ( D->bus, D->dev, D->func, 4, data & ~0x400);

        // #debug
        // printk ("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
        //     ata_sub_class_code_register_strings[ata_port.controller_type], 
        //     ata_pci.progif,
        //     ata_pci.revisionId );


    // 1:7
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_SERIALSCSI ){

        StorageController.controller_type = (uint8_t) PCI_SUBCLASS_SCSI;
        printk ("atapciSetupMassStorageController: Serial SCSI not supported\n");
        goto fail;

    // 1:8
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_NVMEMORY ){

        StorageController.controller_type = (uint8_t) PCI_SUBCLASS_SCSI;
        printk ("atapciSetupMassStorageController: NVMe not supported\n");
        goto fail;

    // 1:9
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_SAS ){

        StorageController.controller_type = (uint8_t) PCI_SUBCLASS_SCSI;
        printk ("atapciSetupMassStorageController: SAS not supported\n");
        goto fail;

// ====
// No type
// Fail!
// O tipo de dispositivo de armazenaento de massa 
// não é suportado.

    // Fail
    // ?:? = Class/subclass not supported.
    // #type: Unknown controller.
    }else{
        StorageController.controller_type = (uint8_t) STORAGE_CONTROLLER_MODE_UNKNOWN;
        printk("atapciSetupMassStorageController: Mass Storage Device NOT supported\n");
        goto fail;
    };

// #obs:
// Nesse momento já sabemos se é IDE, RAID, AHCI.
// Vamos pegar mais informações,
// Salvaremos as informações na estrutura.

// PCI cacheline, Latancy, Headr type, end BIST
    data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0xC );
    D->latency_timer = (data >> 8)  & 0xff;
    D->header_type   = (data >> 16) & 0xff;
    D->bist          = (data >> 24) & 0xff;

// BARs
    D->BAR0 = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x10 );
    D->BAR1 = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x14 );
    D->BAR2 = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x18 );
    D->BAR3 = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x1C );
    D->BAR4 = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x20 );
    D->BAR5 = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x24 );

// irqline and irq pin.
    data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x3C );
    D->irq_line = (unsigned char) (data & 0xFF);
    D->irq_pin  = (unsigned char) (data >> 8) & 0xFF;

// #test
// Where are we setting up the PIC controller.
// Is it a good time for it?
// Maybe we're doing it only at the early kernel initialization.

// PCI command and status.
    data = pci_ReadPCIConfigAddr ( D->bus, D->dev, D->func, 4 );
    D->Command = (data & 0xffff); 
    D->Status  = (data >> 16) & 0xffff;

    // #debug
    // printk ("[ Command %x Status %x ]\n", D->Command, D->Status );
    // printk ("[ Interrupt Line %x Interrupt Pin %x ]\n", 
    //     D->irq_line, D->irq_pin );

    /*
    printk("[ IDE PCI: bus=%d dev=%d func=%d line=%d pin=%d ]\n",
       D->bus, D->dev, D->func,
       D->irq_line, D->irq_pin );
    */

/*
Interpret them:
irq_line → the legacy IRQ number (e.g. 14, 15, or something else).
irq_pin → tells you which PCI interrupt pin (INTA#, INTB#, etc.) is in use. 
The chipset maps these pins to IOAPIC inputs.
*/

/*
    refresh_screen();
    while (1)
    {
        asm ("hlt\n");
    }
*/

//
// == DMA ====================
//
    data = pci_ReadPCIConfigAddr( D->bus, D->dev, D->func, 0x48 );
//#debug
    //printk("[ Synchronous DMA Control Register %x ]\n", data );

//done:
    //ata_port.used = TRUE;
    //ata_port.magic = 1234;
    return (int) PCI_MSG_SUCCESSFUL;

fail:
    //ata_port.used = FALSE;
    //ata_port.magic = 0;
    return (int) PCI_MSG_ERROR;
}

/*
uint32_t 
__storageReadPCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset )
{

// #bugbug
// Do not use macros.
// Expand this macro outside the function.
 
    out32 ( 
        PCI_PORT_ADDR, 
        CONFIG_ADDR( bus, dev, fun, offset ) );

    return (uint32_t) in32(PCI_PORT_DATA);
}
*/

//
// $
// INITIALIZATION
//

// Storage manager
// Ordem: storage, disk, volume, file system.
// #importante 
// A estrutura 'storage' vai ser o nível mais baixo.
// É nela que as outras partes devem se basear.
// Create the main 'storage' structure.

int storageInitialize(void)
{
// Called by I_initKernelComponents in x64init.c
// #bugbug
// When the rest of the structure is initialized?

    unsigned int data=0;
    unsigned char bus=0;
    unsigned char dev=0;
    unsigned char fun=0;


// check bootdisk signature from bootblock.
    __validate_disksignature_from_bootblock();

// =======================================================
// storage structure

    storage = (void *) kmalloc( sizeof(struct storage_d) );

    if ((void *) storage == NULL){
       printk("storageInitialize: storage\n");
       return FALSE;
    }
    memset( storage, 0, sizeof(struct storage_d) );

    // Set up only the main elements of the structure.
    storage->used = TRUE;
    storage->magic = 1234;
    storage->system_disk = NULL;
    storage->system_volume = NULL;
    storage->bootvolume_fp = NULL;
    // ...

// =====================================
// Initialize disk and volume support
    __disk_init();
    __volume_init();

    int Status = -1;  //error
    int Value = -1;

/*
    // Reset BootDisk info
    BootDisk.initialized = FALSE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_UNKNOWN;
    BootDisk.ahci_bar5 = 0;
    BootDisk.boot_port = -1;
*/

    data = (unsigned int) storagePCIScanDevice(PCI_CLASSCODE_MASS);
// Error
    if (data == -1)
    {
        printk ("storageInitialize: pci_scan_device fail. ret={%d}\n", 
            (unsigned int) data );

        // Abort
        Status = (int) (PCI_MSG_ERROR);
        printk("storage: No Mass Storage Controller found\n");
        //refresh_screen();
        die();
    }

    // Decode bus/dev/fun from PCI scan result
    bus = ( data >> 8 & 0xff );
    dev = ( data >> 3 & 31 );
    fun = ( data      & 7 );


    // Getting information about the PCI device.
    // class, subclass, prog if and revision id.
    data = (uint32_t) pci_ReadPCIConfigAddr( bus, dev, fun, 8 );

    unsigned char __class    = (data >> 24) & 0xff;  // Class
    unsigned char __subclass = (data >> 16) & 0xff;  // Subclass

    if (__class != PCI_CLASSCODE_MASS)
    {
        printk("storage: Not a PCI_CLASSCODE_MASS\n");
        //refresh_screen();
        die();
    }

    // Reset BootDisk info
    BootDisk.initialized = FALSE;
    BootDisk.controller_type = STORAGE_CONTROLLER_MODE_UNKNOWN;
    BootDisk.ahci_bar5 = 0;
    BootDisk.boot_port = -1;

   
    // #debug
    //printk ("Breakpoint\n");
    //while(1){}

// =====================================
// #todo
// Let's know what is the controller we have,
// and than call the right initialization hook for them.

    struct pci_device_d *PCIDeviceStorage;

    if (__subclass == STORAGE_CONTROLLER_MODE_AHCI)
    {
        // AHCI controller only.
        // pci device.
        PCIDeviceStorage = 
            (struct pci_device_d *) scan_pci_device_list2 ( 
                                (unsigned char) PCI_CLASSCODE_MASS, 
                                (unsigned char) PCI_SUBCLASS_SATA );  // ahci

    } else if (__subclass == STORAGE_CONTROLLER_MODE_ATA){

        // ATA controller only.
        // pci device.
        PCIDeviceStorage = 
            (struct pci_device_d *) scan_pci_device_list2 ( 
                                (unsigned char) PCI_CLASSCODE_MASS, 
                                (unsigned char) PCI_SUBCLASS_IDE );

    }

    if ((void *) PCIDeviceStorage == NULL)
    {
        printk("storageInitialize: PCIDeviceStorage\n");
        Status = (int) -1;
        goto fail;
    }
    if ( PCIDeviceStorage->used != TRUE || PCIDeviceStorage->magic != 1234 )
    {
        printk ("storageInitialize: PCIDeviceStorage validation\n");
        Status = (int) -1;
        goto fail;
    }


    printk("Getting PCI into for PCIDeviceStorage\n");
    Value = 
        (unsigned long) storagePCISetupMassStorageController(
            (struct pci_device_d*) PCIDeviceStorage 
        );

    if (Value == PCI_MSG_ERROR){
        printk ("storageInitialize: Error Driver [%x]\n", Value );
        Status = (int) -1;
        goto fail;
    }


    // #ps:
    // 'PCIDeviceStorage' provides us the vendor and device id.
    // The device id will tell us what is the device driver 
    // we need to load or initialize.
    // But for now we have only one generic ata device driver in ata/.

// At this moment we already know the controller type.
// Thanks to storagePCISetupMassStorageController that
// setted up this variable for us.

    uint8_t ControllerType = StorageController.controller_type;

    BootDisk.controller_type = ControllerType;

    switch (ControllerType){
        
        // Controller type: ATA
        case STORAGE_CONTROLLER_MODE_ATA:

            // #debug
            //printk ("Breakpoint ATA\n");
            //while(1){}

           // FORCEPIO - Select PIO mode as standard.
            DDINIT_ata( 
                PCIDeviceStorage, 
                STORAGE_CONTROLLER_MODE_ATA, 
                FORCEPIO 
            );
            break;

        // STORAGE_CONTROLLER_MODE_RAID
        // break

        case STORAGE_CONTROLLER_MODE_AHCI:

            // #debug
            //printk ("Breakpoint AHCI\n");
            //while(1){}

            //panic("storage.c: STORAGE_CONTROLLER_MODE_AHCI\n"); 

            // See: ahci.c
            DDINIT_ahci(
                PCIDeviceStorage,
                STORAGE_CONTROLLER_MODE_AHCI
            );

            //printk("storageInitialize: AHCI controller initialized\n");
            //ahci_test_read();   
            //while(1){}

            break;

        // ...

        //STORAGE_CONTROLLER_MODE_UNKNOWN

        // Invalid
        default:
            panic("storageInitialize: Unsupported controller type #todo");
            break;
    }


// =====================================

// >> Precisa do bootdisk e do ata device.
// Set the number of sectors in the boot disk.
// It depends on the disk and ata initialization.
// So, now we can do this.
    //int Status = FALSE;
    Status = (int) storage_set_total_lba_for_boot_disk();
    if (Status != TRUE){
        printk ("storageInitialize: storage_set_total_lba_for_boot_disk fail\n"); 
        return FALSE;
    }
    //PROGRESS("storageInitialize: storage_set_total_lba_for_boot_disk ok\n"); 

// mbr info:
// It depends on the total lba value for boot disk.
// Its because we're gonna rad the disk to get the partition tables.
    disk_initialize_mbr_info();

    return TRUE;

fail:
    panic("storageInitialize: fail\n");
    return 0;
}

