
// volume.h

#ifndef  __VOLUME_H
#define  __VOLUME_H    1


#define VOLUME_COUNT_MAX 1024

#define VFS_VOLUME_ID              0
#define BOOTPARTITION_VOLUME_ID    1
#define SYSTEMPARTITION_VOLUME_ID  2
//...

//
// == system disk =================================================
//

// These are the main partitions 
// in the Gramado system.

// mbr
#define MBR_LBA  0 

//================================
// boot partition
// size=32MB.
#define VOLUME1_VBR_LBA       63
#define VOLUME1_FAT_LBA       67    //67  (size=246)
//#define VOLUME1_FAT2_LBA    ??    //313 (size=246)
#define VOLUME1_ROOTDIR_LBA   559
#define VOLUME1_DATAAREA_LBA  591   //( size=FFFF setores)
#define VOLUME1_SPC  1 // sectors per cluster.

#define VOLUME1_FAT_SIZE    246

//=======================================
// system partition
// #bugbug: Isso esta errado.
// A partiçao do sistema precisa começar 
// logo apos a partiçao de boot, e a partiçao
// de boot tem 32MB.
// >>> Podemos colocar em qualquer lugar depois da
// marca de 32MB.
// Cada setor tem 1/2 KB, entao 32mb/1024/2
#define VOLUME2_VBR_LBA       32000  //#bugbug
#define VOLUME2_FAT_LBA       33000  //#bugbug
//#define VOLUME2_FAT2_LBA    ?? 
#define VOLUME2_ROOTDIR_LBA   34000  //#bugbug
#define VOLUME2_DATAAREA_LBA  35000  //#bugbug
#define VOLUME2_SPC  1 // sectors per cluster.
// ==================================================================


extern char *current_volume_string;
// volume atual ??
// Tipo de sistema de arquivos, fat16, ext2 ...
extern int g_currentvolume_filesystem_type;   //use this one.
// volume atual do tipo fat???
// Se é fat32, 16, 12.
extern int g_currentvolume_fatbits;

/*
 * volume_type_t:
 *     Enumerando os tipos de volume.
 */

typedef enum {


    VOLUME_TYPE_NULL, 

    // Partição em disco físico.
    VOLUME_TYPE_DISK_PARTITION,  

    // Partição em disco virtual.
    VOLUME_TYPE_VIRTUAL_DISK_PARTITION,  

    // Arquivo.
    // Um arquivo qualquer. Sem formatação.
    VOLUME_TYPE_RAW,           

    // Buffer.
    // Um arquivo qualquer. Sem formatação.
    // Usado por banco de dados.
    VOLUME_TYPE_BUFFER,

    // Partição de swap.
    VOLUME_TYPE_SWAP

    //...

}volume_type_t;


// #todo
typedef enum {
    VOLUME_CLASS_NULL,
    VOLUME_CLASS_2,
    VOLUME_CLASS_3
}volume_class_t;


/*
 * vbr_d:
 *     Structure for VBR parameters.
 */  
struct vbr_d
{
	//copiar mbr, é parecido;
}; 
// VBR structure for the boot partition.
// See: storage.c
extern struct vbr_d  *vbr; 
 

/*
 * volume_d:
 *     Estrutura para acesso rápido a volumes.
 *     Deve ser simples e com poucos elementos.
 */
struct volume_d
{
    object_type_t  objectType;
    object_class_t objectClass;

    //file *fp;

    volume_type_t   volumeType;
    volume_class_t  volumeClass;
    
    int used;
    int magic;

    int id;
    
    //label
    char *name;

    //pathname
    char *cmd;

    //string usada no path para identificar o disco.
    //isso não existe.
    char path_string[32];  

    
    // Only one thread can call read and write routine at time.
    int blocked;
    struct thread_d *waiting;  //this thread is waiting.

    // This is the process that call the read/write operation on this volume.
    // Qual processo está usando.
    pid_t pid;
    gid_t gid;

    // Areas:
    // Maybe we can find these in the superblock.
    // Well, this is the fast access.
    unsigned long VBR_lba;
    unsigned long FAT1_lba;
    unsigned long FAT2_lba;
    unsigned long ROOT_lba;
    unsigned long DATA_lba;
    
    struct superblock_d super;

    // Ponteiro para um buffer se o tipo permitir.
    void *priv_buffer;
    unsigned long buffer_size_in_sectors;
    unsigned long sector_size;


//
// Capacity
//

// First and last lba.
    unsigned long __first_lba;
    unsigned long __last_lba;
// Number of sectors in the partition.
// last - first.
    unsigned long number_of_blocks;
// 512 or 4096?
    unsigned long bytes_per_sector;
// How many bytes in the whole disk.
    unsigned long size_in_bytes;

    //filesystem_type_t filesystemType;

    //#todo
    // se está funcionando ... se está inicializado ...
    //int status;
    
    //#todo
    // que tipo de operação esta sendo realizada. ou nenhuma.
    // se ele está ocupoado o escretor terá que esperar.
    //int state;


    // Se é um volume virtual e precisa ser salvo
    // pois houve uma modificação.
    int need_to_save;

    // #todo
    // contador de processos usando o volume

// The volume's file system.
    struct filesystem_d *fs;

// The disk that the volume belongs to.
    struct disk_d *disk;

    struct volume_d *next;
};

// #importante:
// Esses são os três volumes básicos do sistema 
// mesmo que o disco só tenha um volume, essas 
// estruturas vão existir.
// See: storage.c
extern struct volume_d  *volume_vfs;             // volume 0
extern struct volume_d  *volume_bootpartition;   // volume 1
extern struct volume_d  *volume_systempartition; // volume 2
// ...

// Volume list
// See: storage.c
extern unsigned long volumeList[VOLUME_COUNT_MAX];


//
// == prototypes ==========================================
//

int volume_init (void);
void *volume_get_volume_handle( int number );
void *volume_get_current_volume_info (void);

//
// Show info
//

int volumeShowVolumeInfo ( int descriptor );
void volumeShowCurrentVolumeInfo (void);
void volume_show_info (void);

#endif    


