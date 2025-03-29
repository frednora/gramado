// storage.c
// Created by Fred Nora.

#include <kernel.h>

// The number of sectors in the boot disk.
// See: storage_set_total_lba_for_boot_disk().
unsigned long gNumberOfSectorsInBootDisk=0;

const char* sda_string = "sda";
const char* sdb_string = "sdb";
const char* sdc_string = "sdc";
const char* sdd_string = "sdd";
const char* sdfail_string = "sd?";

// #test
// see: disk.h
struct partition_table_d *system_disk_pt0;
struct partition_table_d *system_disk_pt1;
struct partition_table_d *system_disk_pt2;
struct partition_table_d *system_disk_pt3;
//struct partition_table_d *boot_partition; 
//struct partition_table_d *system_partition; 

// MBR structure for the system disk.
// See: disk.h
struct mbr_d  *mbr; 

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

// =================================================

// Show disk information given its descriptor.
static int __ShowDiskInfo(int index)
{
// Worker
    struct disk_d  *d;
    register int n = index;

    //printk("\n");
    printk("\n");

    //#debug
    //printk ("__ShowDiskInfo:\n\n");

    if ( n<0 || n >= DISK_COUNT_MAX )
    {
        printk("n fail\n");
        goto fail;
    }
    d = (struct disk_d *) diskList[n];
    if ((void *) d == NULL){
        printk("d fail\n");
        goto fail;
    }
    if ( d->used != TRUE || d->magic != 1234 )
    {
        printk("d validation\n");
        goto fail;
    }

// Show data

    printk("Disk (%d): Name {%s}\n", 
        d->id, d->name );
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


    // The main structure.
    if ( (void *) storage == NULL ){
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

// The disk that the volume belongs to.
    volume_bootpartition->disk = NULL;

    if( (void*) ____boot____disk != NULL ){
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
    ksprintf ( (char *) name_buffer, "VOLUME-%d",volume_bootpartition->id);
    volume_bootpartition->name = (char *) strdup ( (const char *) name_buffer);  

    //#todo
    volume_bootpartition->cmd = "#TODO";

//
// Current volume
//

    current_volume = volume_bootpartition->id;

// Finalizing

    volume_bootpartition->used = (int) TRUE;
    volume_bootpartition->magic = (int) 1234;

    volumeList[BOOTPARTITION_VOLUME_ID] = (unsigned long) volume_bootpartition; 

    storage->boot_volume = (struct volume_d *) volume_bootpartition; 

// done:
    return 0;
}


// local
// Create system partition.
// Volume 2
static int __create_system_partition(void)
{
    char name_buffer[32];


    // The main structure.
    if ( (void *) storage == NULL ){
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
    volume_systempartition->name = (char *) strdup ( (const char *) name_buffer);  

    //#todo 
    volume_systempartition->cmd = "#TODO";


// Finalizing

    volume_systempartition->used = (int) TRUE;
    volume_systempartition->magic = (int) 1234;

    volumeList[SYSTEMPARTITION_VOLUME_ID] = (unsigned long) volume_systempartition;

    storage->system_volume = (struct volume_d *) volume_systempartition; 

// done:
    return 0;
}

// local
// Create vfs partition
// Volume 0.
static int __create_vfs_partition(void)
{
    char name_buffer[32];


// The main structure.

    if ( (void *) storage == NULL ){
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

// #todo
// cmd

    volume_vfs->cmd = "#TODO";

// Finalizing

    volume_vfs->used = (int) TRUE;
    volume_vfs->magic = (int) 1234;

    volumeList[VFS_VOLUME_ID] = (unsigned long) volume_vfs;

    storage->vfs_volume = (struct volume_d *) volume_vfs; 

// done:
    return 0;
}


struct partition_table_d *disk_get_partition_table(int index)
{
// #bugbug
// Only if the ata driver is already initialized.

    struct partition_table_d *pt;
    unsigned char *mbr_base = (unsigned char *) MBR_ADDRESS_VA; 

    if (g_ata_driver_initialized != TRUE){
        panic("disk_get_partition_table: g_ata_driver_initialized\n");
    }

// Read from disk.
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
    switch (index)
    {
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


int disk_initialize_mbr_info(void)
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
    if ( (void*) system_disk_pt0 == NULL )
        return -1;

    system_disk_pt1 = (struct partition_table_d *) disk_get_partition_table(1);
    if ( (void*) system_disk_pt1 == NULL )
        return -1;

    system_disk_pt2 = (struct partition_table_d *) disk_get_partition_table(2);
    if ( (void*) system_disk_pt2 == NULL )
        return -1;

    system_disk_pt3 = (struct partition_table_d *) disk_get_partition_table(3);
    if ( (void*) system_disk_pt3 == NULL )
        return -1;

    printk("done\n");
    //while(1){}
    return 0;
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

int
storage_read_sector( 
    unsigned long buffer, 
    unsigned long lba )
{
    int res=-1;

    //if (buffer == 0)
        //return -1;

    res = (int) ataReadSector ( 
                    (unsigned long) buffer, 
                    (unsigned long) lba, 
                    0, 
                    0 );
    return (int) res; 
}

int
storage_write_sector( 
    unsigned long buffer, 
    unsigned long lba )
{
    int res=-1;

    //if (buffer == 0)
        //return -1;

    res = (int) ataWriteSector ( 
                    (unsigned long) buffer, 
                    (unsigned long) lba, 
                    0, 
                    0 );
    return (int) res; 
}

// disk_init:
// Initialize the disk manager.
// #bugbug
// #fixme
// The boot disk was not include in the diskList[]
// Probably the 'ide module' will discover all the ide disks
// and rebuild the list. This way all the information made here
// was gone. #fixme

int disk_init (void)
{
    struct disk_d *d;
    unsigned char BootDisk=0;
    register int i=0;

    // #debug
    // printk ("disk_init: Initializing..\n");

// Check the storage structure.
    if ((void *) storage == NULL){
        panic("disk_init: storage\n");
    }
// Clean the disk list.
    for ( i=0; i<DISK_COUNT_MAX; i++ ){
        diskList[i] = 0;
    };

//
//  Disk structure
//

// #importante
// Essa estrutura eh vital, 
// nao podemos ficar sem ela.

    d = (void *) kmalloc( sizeof(struct disk_d) );
    if ((void *) d == NULL){
        panic("disk_init: d\n");
    }
    memset ( d, 0, sizeof(struct disk_d) );
    d->used = (int) TRUE;
    d->magic = (int) 1234;

// ID
// This is the boot disk.
// So, it will be the number '0'.
    d->id = 0;
    current_disk = d->id;
// Saving
// #bugbug? Is it too early for that?
    diskList[ current_disk ] = (unsigned long) d;

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

    BootDisk = (char) d->boot_disk_number;

// #bugbug: if d->name is a ponter we need to point to
// a const well define string, or create a new one.


    switch (BootDisk){
    case 0x80:  d->name = sda_string;  break;
    case 0x81:  d->name = sdb_string;  break;
    case 0x82:  d->name = sdc_string;  break;
    case 0x83:  d->name = sdd_string;  break;
    default:
        debug_print("disk_init: [FAIL] default boot disk number\n");
        d->name = sdfail_string;
        break;
    };

    d->next = NULL;

// #bugbug
// Is this structure initialized?
    if ((void*) storage != NULL){
        storage->system_disk = (struct disk_d *) d;
    }

// global.
    ____boot____disk =  (struct disk_d *) d;

// #todo
// Maybe this is the right moment to save the pointer
// into the list.
    //diskList[ current_disk ] = (unsigned long) d;

   //more?

//done:
    printk("Done\n");
    return 0;
}

// Show info for all disks in the list.
// Called by service 251
void disk_show_info(void)
{
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
    if (current_disk<0)
        return;
    printk("The current disk is {%d}.\n", current_disk );
    __ShowDiskInfo(current_disk);
}

/*
 * disk_get_disk_handle:
 *     Obtem o ponteiro da estrutura dado o descritor.
 */
 
void *disk_get_disk_handle ( int number )
{
    if ( number < 0 || number >= DISK_COUNT_MAX )
    {
        return NULL;
    }

    return (void *) diskList[number];
}


/*
 * volume_init:
 *     Inicializa o volume manager.
 */

int volume_init (void)
{
    int Status = -1;  //fail
    register int i=0;
    char name_buffer[32];


    // The main structure.
    if ( (void *) storage == NULL ){
        panic ("volume_init: storage");
    }

    // Clear the list
    for ( i=0; i<VOLUME_COUNT_MAX; i++ ){
        volumeList[i] = 0;
    };


// --------


// Volume 0 - vfs
    Status = (int) __create_vfs_partition();
    if(Status<0){
        panic("volume_init: __create_vfs_partition fail\n");
    }

// Volume 1 - Boot partition.
    Status = (int) __create_boot_partition();
    if(Status<0){
        panic("volume_init: __create_boot_partition fail\n");
    }

// Volume 2 - System partition.
    Status = (int) __create_system_partition();
    if(Status<0){
        panic("volume_init: __create_system_partition fail\n");
    }

//done:
    return 0;
}

static int __ShowVolumeInfo(int index)
{
    struct volume_d *v;

    printk("\n");
    printk("\n");

    //printk ("__ShowVolumeInfo:\n");


    if ( index < 0 || 
         index >= VOLUME_COUNT_MAX )
    {
        printk("index fail\n");
        goto fail;
    }

// Structure validation
    v = (struct volume_d *) volumeList[index];
    if ( (void *) v == NULL ){
        printk ("struct fail\n");
        goto fail;
    }
    if ( v->used != 1 || v->magic != 1234 )
    {
        printk("flags fail\n");
        goto fail;
    }

// Show data

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

//done
    return 0;

fail:
    printk("Fail\n");
    return (int) -1;
}

int volumeShowVolumeInfo (int descriptor)
{
    return (int) __ShowVolumeInfo(descriptor);
}

void volumeShowCurrentVolumeInfo(void)
{
    if (current_volume<0)
        return;
    printk ("The current volume is %d\n", current_volume );

// #bugbug
// Valume is not a disk.
    //__ShowDiskInfo (current_volume);
}

// Show info for all volumes in the list.
void volume_show_info (void)
{
    struct volume_d *volume;
    register int i=0;

    for (i=0; i<VOLUME_COUNT_MAX; i++)
    {
        volume = (struct volume_d *) volumeList[i];
        
        if ( (void *) volume != NULL )
        {
            volumeShowVolumeInfo(i);
        }
    };
}


void *volume_get_volume_handle( int number )
{

// check limts
    if ( number < 0 || 
         number >= VOLUME_COUNT_MAX )
    {
        return NULL;
    }

    return (void *) volumeList[number];
}


void *volume_get_current_volume_info (void)
{
    if ( current_volume < 0 || 
         current_volume > VOLUME_COUNT_MAX )
    {
         return NULL;
    }

    return (void *) volumeList[VOLUME_COUNT_MAX];
}


/*
 * read_lba:
 *     Carrega um um setor na memória, dado o LBA.
 *     Obs: 
 *     Talvez essa rotina tenha que ter algum retorno no caso de falhas. 
 */
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

void 
read_lba ( 
    unsigned long address, 
    unsigned long lba )
{
// #todo
// Fazer algum filtro de argumentos?

    // if ( address == 0 ){}

// Is it FAT12/FAT16/FA32 boot disk?
// Is it a FAT disk at all?
// See: volume.h

    switch (g_currentvolume_fatbits){

    case 32:
        debug_print ("read_lba: [FAIL] FAT32 not supported\n");
        return;
        break;

    // atahdd.c
    case 16:
        //#todo: return value.
        //#todo: IN: buffer,lba,?,?,
        ataReadSector ( address, lba, 0, 0 );
        return;
        break;

    // Nothing.
    case 12:
        debug_print ("read_lba: [FAIL] FAT12 not supported\n");
        return;
        break;

    default:
        debug_print ("read_lba: [FAIL] g_currentvolume_fatbits not supported\n");
        panic ("read_lba: default\n");
        break;
    };
}

/*
 * write_lba:
 *     Grava um setor no disco dado o endereço do buffer e o lba. 
 */
// #bugbug
// Essa rotina e' independente do sistema de arquivos.
// #todo: use 'int' return.
void write_lba( unsigned long address, unsigned long lba )
{

// #todo: 
// Check lba limits.

    if (address == 0){
        debug_print ("write_lba: [FAIL] Limits\n");
        goto fail;
    }

    // See: volume.h
    switch (g_currentvolume_fatbits){

    case 32:
        printk ("write_lba: [ERROR] FAT32 not supported\n");
        goto fail;
        break;

    //See: atahdd.c
    case 16:
        
        ataWriteSector ( address, lba, 0, 0 ); 
        return;
        break;

    case 12:
        printk ("write_lba: [ERROR] FAT12 not supported\n");
        goto fail;
        break;

    default:
        printk ("write_lba: default\n");
        panic ("read_lba: default\n");
        break;
    };

// Nothing.

fail:
    refresh_screen();
    return;
}

// Get the number of sectors in the boot disk
// and save it into a global variable, for now.
int storage_set_total_lba_for_boot_disk(void)
{
    struct disk_d *disk;
    struct ata_device_d  *ata_device;

// --------------------------------
// Get the boot disk
    disk = (struct disk_d *) ____boot____disk;
    if ( (void*) disk == NULL ){
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
    if ( (void*) ata_device == NULL ){
        printk("ata_device\n");
        goto fail;
    }
    if (ata_device->magic != 1234){
        printk("ata_device validation\n");
        goto fail;
    }

// --------------------------------
    // Show the number of blocks.
    printk("Number of blocks: %d\n",
        ata_device->dev_total_num_sector );

// Set global variable.
    gNumberOfSectorsInBootDisk = 
        (unsigned long) ata_device->dev_total_num_sector;

// Save it in the main storage structure.
    if ((void*) storage == NULL){
        printk("storage\n");
        goto fail;
    }
    storage->mumber_of_sectors_in_boot_disk = 
        (unsigned long) ata_device->dev_total_num_sector;

//done
    //refresh_screen();
    return TRUE;
fail:
    return FALSE;
}

//
// $
// INITIALIZATION
//

int init_storage_support(void)
{
// Called by I_initKernelComponents in x64init.c
// #bugbug
// When the rest of the structure is initialized?

    storage = (void *) kmalloc( sizeof(struct storage_d) );
    if ((void *) storage == NULL)
    {
       printk("init_storage_support: storage\n");
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

    return TRUE;
}



