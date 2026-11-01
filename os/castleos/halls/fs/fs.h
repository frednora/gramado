// fs.h
// Main header for the fs/ kernel module.
// File system operations.
// Mainly syscalls
// Created by Fred Nora.

#ifndef __FS_FS_H
#define __FS_FS_H    1

struct fs_info_d 
{
    int initialized;
    //unsigned countstuff;
    // ...
};
extern struct fs_info_d FSInfo;


//
// FILES ?
//

// --------------------------------
// Regular files:
// struct file_d
// see: libk/kstdio.h

// --------------------------------
// Sockets:
// struct socket_d
// see: net/socket.h

// --------------------------------
// Pipes:
// struct file_d
// see: fs/pipes.h

// --------------------------------
// TTYs:
// struct tty_d
// see: res/dev/chardev/tty/tty.h
// see: res/dev/chardev/tty/pty.h


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
// == Structures ====================================
//

// Support for the target directory that we want to access.
// Facility to load files from a target directory.
struct target_dir_d
{
    int used;
    int magic;
    int initialized;

    // Buffer where the directory was loaded
    unsigned long current_dir_address;

    // The name of the directory. (format 8.3)
    char name[32];

    // ponteiro para a string do caminho
    //char *pwd_string;  

    // ??
    // The number of entries?

    // ...
};
// See: fs.c
extern struct target_dir_d  current_target_dir;

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

// File system info
struct filesystem_d
{
    struct kobject_d kobj;
    int used;
    int magic;

    //#todo: int id;

    int type;
    char *name;
// Sectors per cluster
    int spc; 
// Number of entries in the root dir
    int dir_entries;
// Size of the entry in bytes
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

// syscall
ssize_t sys_read(int fd, const char *ubuf, size_t count);
ssize_t sys_write(int fd, const char *ubuf, size_t count);

// syscalls
int 
sys_open(
    const char *pathname, 
    int flags, 
    mode_t mode );
int sys_close(int fd);

int sys_fcntl(int fd, int cmd, unsigned long arg);
int sys_ioctl(int fd, unsigned long request, unsigned long arg);


int sys_get_global_sync(int sync_id, int request);
void sys_set_global_sync(int sync_id, int request, int data);
int sys_create_new_sync(void);

int sys_get_saved_sync(void);

int sys_get_file_sync(int fd, int request);
void sys_set_file_sync(int fd, int request, int data);

int sys_get_device_number_by_path(const char *path);
int sys_open_device_by_number(int device_index);

unsigned long sys_get_file_size(const char *path);

int sys_sleep_if_socket_is_empty(int fd);


void sys_pwd(void);


//
// Read from disk
//


// Wrapper
int 
sys_read_file_from_disk ( 
    const char *file_name, 
    int flags, 
    mode_t mode );

//
// Write to file
//


// Wrapper
int
sys_write_file_to_disk ( 
    const char *file_name, 
    unsigned long size_in_sectors,
    unsigned long size_in_bytes,
    char *file_address,
    char flag );

int sys_create_empty_file(const char *file_name);
int sys_create_empty_directory(const char *dir_name);

void sys_cd_command ( const char *string );

int fsInitialize(void);

#endif    

