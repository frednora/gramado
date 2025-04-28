// fat.h
// Main structure and function prototypes
// used by the fs subsystem.

#ifndef  __FAT_H
#define  __FAT_H    1

// fat16 structure.
struct fat_d
{
    int used;
    int magic;
    int initialized;
    int type;

    unsigned long fat_address;      // endereço da fat  

    unsigned long fat_first_lba;    // first lba
    unsigned long fat_last_lba;     // last lba

// size in sectors.
    unsigned long fat_size_in_sectors;
// Tamanho da tabela dado em bytes.
    unsigned long size_in_bytes;
// Tamanho da tabela dado em kbytes.
    unsigned long size_in_kb;

    //...
    
    struct volume_d *volume;
};
// See: 
// fs_init_fat in fs.c
// A FAT usada no boot volume.
// see: fat16.c
extern struct fat_d  *bootvolume_fat;


// fat info for the boot partition of the system disk.
struct system_fat_d
{
    int initialized;
    //#todo
    //struct fat_d  *_fat;
    // ...
};
extern struct system_fat_d  bpFAT;


extern struct fat16_directory_entry_d *vol_label_directory_entry;

// 
// == prototypes ==========================================
//

void test_fat16_find_volume_info(void);

int 
fat16_get_entry_info(
    struct fat16_directory_entry_d *entry,
    unsigned long directory_va,
    int entry_number,
    int entry_max );

void from_FAT_name(char *src, char *dst);
void to_FAT_name(char *src, char *dst);

void fs_fat16_cache_not_saved(void);
int fs_save_fat16_cache(void);

unsigned long 
fsGetFileSize ( 
    unsigned char *file_name, 
    unsigned long dir_address );

void 
fsFAT16ListFiles ( 
    const char     *dir_name, 
    unsigned short *dir_address, 
    int            number_of_entries );

void fat16_init_fat_structure(void);
int fat16Init(void);

#endif    


