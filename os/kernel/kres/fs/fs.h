
// fs.h

#ifndef __FS_FS_H
#define __FS_FS_H    1

// Main directories in the system.
struct system_directory_d
{
// name
    char name[64];
    size_t name_size; 
// Is it loaded in the main memory.
    int initialized;
// The virtual address of the directory.
    unsigned long address;
// The total number of entries.
    int number_of_entries;
};

// Canonical:
// The list of the main directories in the system.
// These are the first directories searched when
// we're looking for a file.
// see: fs.c
extern struct system_directory_d  sdROOT;      // '/'
extern struct system_directory_d  sdGRAMADO;   // '/GRAMADO/'
extern struct system_directory_d  sdDE;        // '/DE/'

// Buffers to walk on a pathname,
// loading until 8 directories.
// 1 page size.
#define FS_N_BUFFERS    5
extern unsigned long fs_buffers[FS_N_BUFFERS];


// The dev/ directory entry.
struct dev_dir_entry_d
{
    int used;
    int magic;
    int id;
    int initialized;

// ex:
// "/DEV/DEV1"
    char path[64];
    int path_size;
    
    file *fp;
};

struct dev_dir_entry_d  dev_dir[32];

int fs_initialize_dev_dir(void);

// =================


//
// == pwd =============================
//

// See: path.h
// A string do diretório de trabalho.

struct cwd_d 
{
    //char current_workingdiretory_string[WORKINGDIRECTORY_STRING_MAX];
    //int pwd_initialized;

    char path[WORKINGDIRECTORY_STRING_MAX];
    int size;
    int initialized;
};

extern struct cwd_d  CWD;



// -----------------

// #todo
// Talves possamos usar enum.

// #bugbug
// Essas definições provocam problemas de compilação.

/*
// file types
#define BIN_FILE    1000
#define TXT_FILE    1001
#define CPP_FILE    1002
#define ASM_FILE    1003
// ...

// file extensions
#define BIN_FILE    2000
#define TXT_FILE    2001
#define CPP_FILE    2002
#define ASM_FILE    2003
// ...
*/

// -----------------

#ifndef DIRECTORY_SEPARATOR
# define DIRECTORY_SEPARATOR  '/'
#endif

#ifndef ISSLASH
# define ISSLASH(C) ((C) == DIRECTORY_SEPARATOR)
#endif


// Filesystem types.
#define  FS_TYPE_NULL   0
#define  FS_TYPE_GVFS   1000  // Gramado Virtual File System.
#define  FS_TYPE_VFS    1001
#define  FS_TYPE_FAT12  1002 
#define  FS_TYPE_FAT16  1003 
#define  FS_TYPE_FAT32  1004 
#define  FS_TYPE_EXT2   1005
// No more types. 

// short.
//#define FS_I386_IMAGE  0x014C

// pwd support
#define  FS_VFSWORKINGDIRECTORY_ID      0
#define  FS_BOOTWORKINGDIRECTORY_ID     1
#define  FS_SYSTEMWORKINGDIRECTORY_ID   2
#define  FS_UNKNOWNWORKINGDIRECTORY_ID  (-1)


//
//  == Variables =================================
//


//
// == Search path ================================
//

// See:
// https://en.wikipedia.org/wiki/PATH_(variable)
// ...
// #importante
// + Quando uma função para eecutar uma programa é chamada
// o kernel deve tomar suas decisões sobre qual diretório
// procurar. Não é seguro obedecer o search path indicado pelo
// processo que chamou a função.
// Então o kernel pode começar procurando em /BIN/ e /USER/BIN/.
// O kernel só vai procurar no cwd se o pathname desejado começar
// explicitamente com "./". 
// Multics originated the idea of a search path. 
// The early Unix shell only looked for program names in /bin, 
// but by Version 3 Unix the directory was too large and /usr/bin, 
// and a search path, became part of the operating system.


extern unsigned long search_path_dir_address;
extern unsigned long search_path_dir_entries;


// ?? - Contagem de diretórios.
//int dirCount;  


// List of clusters. 
// Usado na rotina de carregamento de arquivo.
// #bugbug
// O limite é um arquivo de 512KB.
//#define MAX_CLUSTERS  1024
#define MAX_CLUSTERS  2048
extern unsigned short file_cluster_list[MAX_CLUSTERS]; 


//
// == Structures ====================================
//

/*
 * Obs: Dentro do kernel base somente teremos primitivas
 *      Operações complexas devem ir para servidores utilitários.
 * Descrição:
 *     Header para o File System Manager. (FSM)
 *     File system header for kernel file management.
 *     Inicialmente suporte ao sistema de arquivos FAT16.
 *     Posteriormente suporte aos sistemas FAT12, FAT32, EXT2.
 * History:
 *     2014 - Created by Fred Nora.
 */
//suporte ao diretório alvo que desejamos acessar.
//com base em um pathname passado via argumento.

struct target_dir_d
{
    int used;
    int magic;

    // Buffer where the directory was loaded.
    unsigned long current_dir_address;

//ponteiro para a string do caminho
    //char *pwd_string;  

    //file name 8.3 (11 bytes;)
    char name[32];

    // ??
    // The number of entries ?

    // ...
    
    int initialized;
};

struct target_dir_d current_target_dir;
 
// links para arquivos ou diretórios 
// dentro do mesmo sistema de arquivos. 

struct hardlink_d
{
    int used;
    int magic;
    //..
};


// links para arquivos e diretórios em 
// volumes espalhados por vários discos. 
struct softlink_d
{
    int used;
    int magic;
    //..
};


/*
 * dir_d:
 *     Estrutura para diretório de arquivo.
 *     #importante: 
 *     Entrada de diretório baseada em streams. 
 */

struct dir_d
{
    int used;
    int magic;

    int id;

    file *_file;
    struct inode_d *inode;

    pid_t pid;
    uid_t uid;
    gid_t gid;

// número de bytes em uma entrada.
    int entry_size_in_bytes;
// número total de bytes no diretório.
    int totalentries_size_in_bytes;
    int number_of_entries;
// Númetro máximo de arquivos em um diretório.
    int fileMax;
// Número total de arquivos presentes no diretório.
    int fileTotal;
// Endereço onde o arquivo foi carregado.
    unsigned long address;
// flag, se esta ou nao na memoria.
    int inMemory;

    struct dir_d *next;
};


/*
 * filesystem_d:
 *     Informações sobre um sistema de arquivos.
 */

struct filesystem_d
{
    object_type_t  objectType;
    object_class_t objectClass;

    int used;
    int magic;

//#todo: int id;

    int type;
    char *name;
// Sectors per cluster.
    int spc; 
// Number of entries in the root dir.
    int dir_entries;
// Size of the entry in bytes.
    int entry_size; 

// The whole filesystem size.
// (tables + data)

    unsigned long fs_first_lba;
    unsigned long fs_last_lba;
    unsigned long fs_size_in_sectors;

// #bugbug
// Thats specific for fat16.
// fat16.

// rootdir
    unsigned long rootdir_address;      // endereço do rootdir
    unsigned long rootdir_first_lba;    // first lba
    unsigned long rootdir_last_lba;     // last lba
    unsigned long rootdir_size_in_sectors;

// fat 1
    unsigned long fat_address;       // endereço da fat  
    unsigned long fat_first_lba;     // first lba
    unsigned long fat_last_lba;      // last lba
    unsigned long fat_size_in_sectors;

// fat2
// ...

// data area
    unsigned long dataarea_address;      // endereço do inicio da area de dados.
    unsigned long dataarea_first_lba;    // first lba
    unsigned long dataarea_last_lba;     // last lba
    unsigned long dataarea_size_in_sectors;

    //...

// if this fs is a fat filesystem.
    //struct fat_d *fat;

// #todo
    //struct volume_d *volume;
    //struct disk_d *disk;

    // struct filesystem_d *next;
};

// The root file system.
// See: fs_init_structures in fs.c
// See: fs.h
extern struct filesystem_d  *root;
// ...


//
// == file context ===========================================
//

// file context.
// It is gonna help us to load a file.
// Let's put in here all the data we need to handle the file.

struct file_context_d
{

// security

    pid_t pid;
    uid_t uid;
    gid_t gid;

//
// storage
//

    //#todo
    // Em que disco o arquivo esta.
    //struct disk_d   *disk;
    
    // Em que volume o arquivo esta.
    struct volume_d *volume;
    
    struct filesystem_d *FileSystem;

    struct dir_d *Directory;

    file *File;

    struct inode_d *inode;
    
    // The FAT is alread loaded in this address.
    unsigned long fat_address;

    // The directory is alread loaded in this address.
    unsigned long dir_address;
    // Number of entries.
    int dir_entries;
    
    unsigned char *file_name;
    
    // Buffer to load the file;
    unsigned long file_address;
    
    // The size of the buffer
    unsigned long buffer_limit;
    
    //...

    struct file_context_d *next;
};



//
// == prototypes =====================================
//

int file_read_buffer( file *f, char *buffer, int len );
int file_write_buffer( file *f, char *string, int len );

// Implementations
ssize_t __read_imp(int fd, char *ubuf, size_t count);
ssize_t __write_imp(int fd, char *ubuf, size_t count);

// syscall
ssize_t sys_read(int fd, const char *ubuf, size_t count);
ssize_t sys_write(int fd, const char *ubuf, size_t count);

// Implementations
int 
__open_imp(
    const char *pathname, 
    int flags, 
    mode_t mode );
int __close_imp(int fd);

// syscalls
int 
sys_open(
    const char *pathname, 
    int flags, 
    mode_t mode );
int sys_close(int fd);


int sys_fcntl( int fd, int cmd, unsigned long arg );
int sys_ioctl( int fd, unsigned long request, unsigned long arg );

file *get_file_from_fd(int fd);

int sys_get_global_sync(int sync_id, int request);
void sys_set_global_sync(int sync_id, int request, int data);
int sys_create_new_sync(void);
int get_saved_sync(void);

int sys_get_file_sync(int fd, int request);
void sys_set_file_sync(int fd, int request, int data);

int sys_get_device_number_by_path(const char *path);
int sys_open_device_by_number(int device_index);

unsigned long sys_get_file_size(const char *path);

int sys_sleep_if_socket_is_empty(int fd);

// ========

int get_free_slots_in_the_file_table(void);
int get_free_slots_in_the_inode_table(void);

int get_filesystem_type (void);
void set_filesystem_type (int type);

unsigned long fs_count_path_levels (unsigned char *path);

// manipulando a lista de arquivos do kernel.
void *get_file(int Index);
void set_file( void *file, int Index);

int fsInit (void);
void fs_init_structures (void);

void file_close(file *_file);
size_t file_get_len(file *_file);
struct inode_d *file_inode(file *f);
int file_truncate(file *_file, size_t len);
int fsCheckELFFile(unsigned long address);

int init_directory_facilities(void);
void fsInitializeWorkingDiretoryString(void);

int fsInitTargetDir(unsigned long dir_address, char *dir_name);

int fsList( const char *dir_name);

void 
fsListFiles ( 
    int disk_id, 
    int volume_id, 
    int directory_id );

int
__load_sequential_sectors ( 
    unsigned long address, 
    unsigned long lba, 
    unsigned long sectors );

unsigned long 
fatClustToSect ( 
    unsigned short cluster, 
    unsigned long spc, 
    unsigned long first_data_sector );

void 
fatLoadCluster ( 
    unsigned long sector, 
    unsigned long address, 
    unsigned long spc );

void 
fs_load_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size );

// MBR
void fs_load_mbr(unsigned long mbr_address);

// FAT
void 
fs_load_fat(
    unsigned long fat_address, 
    unsigned long fat_lba, 
    size_t size_in_sectors );

// ROOT
void 
fs_load_rootdir(
    unsigned long root_address, 
    unsigned long root_lba, 
    size_t size_in_sectors );

// For boot partition
void fsbp_initialize_fat(void);
// For boot partition
int fsbp_initialize_bp_directories(void);


void 
fatWriteCluster ( 
    unsigned long sector, 
    unsigned long address, 
    unsigned long spc );

int 
fs_save_fat ( 
    unsigned long fat_address, 
    unsigned long fat_lba, 
    size_t fat_size );

int 
fs_save_rootdir ( 
    unsigned long root_address, 
    unsigned long root_lba, 
    size_t root_size );

void fsUpdateWorkingDiretoryString(char *string);

void fs_fntos(char *name);
void fs_fntos2(char *name);

int fs_get_free_fd_from_pid (pid_t pid);
int fs_initialize_process_cwd ( pid_t pid, char *string );
void fs_pathname_backup ( pid_t pid, int n );
int fs_print_process_cwd (pid_t pid);

void fs_show_file_info (file *f);
void fs_show_file_table(void);
void fs_show_inode_info (struct inode_d *i);
void fs_show_inode_table(void);
void fs_show_root_fs_info(void);
void sys_pwd (void);

int 
search_in_dir ( 
    const char *file_name, 
    unsigned long dir_address );

int search_in_root ( const char *file_name );
unsigned short fs_find_n_empty_entries(int n);

int 
findEmptyDirectoryEntry ( 
    unsigned long dir_address, 
    int number_of_entries );

int fsSearchFileInRoot(const char *file_name);
unsigned short fs_find_empty_entry_in_fat(char *fat_address);
int search_path_in_the_inode_table(const char *path);

int
fsSaveFile ( 
    unsigned long fat_address,
    unsigned long dir_address,
    int dir_entries,
    const char *file_name, 
    unsigned long file_size,
    unsigned long size_in_bytes,
    char *file_address,
    char flag );


//
// Read from disk.
//

// Worker
int 
do_read_file_from_disk ( 
    char *file_name, 
    int flags, 
    mode_t mode );
// Wrapper
int 
sys_read_file_from_disk ( 
    const char *file_name, 
    int flags, 
    mode_t mode );

//
// Write to file.
//

// Worker
int
do_write_file_to_disk ( 
    char *file_name, 
    unsigned long file_size,
    unsigned long size_in_bytes,
    char *file_address,
    char flag );
// Wrapper
int
sys_write_file_to_disk ( 
    const char *file_name, 
    unsigned long file_size,
    unsigned long size_in_bytes,
    char *file_address,
    char flag );

int sys_create_empty_file(const char *file_name);
int sys_create_empty_directory(const char *dir_name);
void set_global_open_file ( void *file, int Index );
void *get_global_open_file (int Index);
void sys_cd_command ( const char *string );

// From fsload.c

unsigned long 
fsLoadFile ( 
    unsigned long fat_address,
    unsigned long dir_address,
    int dir_entries,
    const char *file_name, 
    unsigned long buffer,
    unsigned long buffer_size_in_bytes );

unsigned long 
fsLoadFile2 ( 
    struct file_context_d *fc, 
    unsigned char *file_name );


// Load an image from GRAMADO/
unsigned long 
fsLoadProgramFromGRAMADO (
    char *program_name,
    unsigned long buffer,
    unsigned long buffer_size_in_bytes );

// Load an image from DE/
unsigned long 
fsLoadProgramFromDE (
    char *program_name,
    unsigned long buffer,
    unsigned long buffer_size_in_bytes );

int 
fs_load_path ( 
    const char *path, 
    unsigned long address, 
    unsigned long buffer_size );

int fsLoadFileFromCurrentTargetDir (void);

int fs_load_image( const char *filename, unsigned long image_va );

#endif    


