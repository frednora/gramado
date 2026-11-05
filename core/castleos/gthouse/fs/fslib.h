// fslib.h
// File system support.
// The workers and the real implementation for the services
// called by fs.c.
// Created by Fred Nora.

#ifndef __FS_FSLIB_H
#define __FS_FSLIB_H    1


// Buffers to walk on a pathname,
// loading until 8 directories.
// 1 page size.
#define FS_N_BUFFERS    5
extern unsigned long fs_buffers[FS_N_BUFFERS];


// ===================================================

// Load boot metafile into the memory.
void 
fs_load_boot_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size_in_sectors );

// Save the boot metafile into the disk.
void 
fs_store_boot_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size_in_sectors );


int file_read_buffer(file *f, char *buffer, int len);
int file_write_buffer(file *f, char *string, int len);


// read/write implementations

ssize_t __read_imp(int fd, char *ubuf, size_t count);
ssize_t __write_imp(int fd, char *ubuf, size_t count);

// open/close implementations

int 
__open_imp(
    const char *pathname, 
    int flags, 
    mode_t mode );

int __close_imp(int fd);

file *get_file_from_fd(int fd);

int get_free_slots_in_the_file_table(void);
int get_free_slots_in_the_inode_table(void);

int get_filesystem_type (void);
void set_filesystem_type (int type);

unsigned long fs_count_path_levels (unsigned char *path);

// Manipulando a lista de arquivos do kernel
void *get_file(int Index);
void set_file( void *file, int Index);

void fs_init_structures(void);

int file_close (file *_file);
size_t file_get_len(file *_file);
struct inode_d *file_inode(file *f);
int file_truncate(file *_file, size_t len);


int fsCheckELFFile(unsigned long address);

int fsInitTargetDir(unsigned long dir_address, char *dir_name);

int fsList(const char *dir_name);

void 
fsListFiles ( 
    int disk_id, 
    int volume_id, 
    int directory_id );

void fsUpdateWorkingDiretoryString(char *string);

void fs_fntos(char *name);
void fs_fntos2(char *name);

int fs_duplicate_fd(int oldfd, int start);
int fs_get_free_fd_from_pid (pid_t pid);

int fs_initialize_process_cwd(pid_t pid, const char *string);
void fs_pathname_backup(pid_t pid, int n);
int fs_print_process_cwd(pid_t pid);

void fs_show_file_info(file *f);
void fs_show_file_table(void);
void fs_show_inode_info (struct inode_d *i);
void fs_show_inode_table(void);
void fs_show_root_fs_info(void);


// Worker
// See: fslib.c
// OUT: fp
file *do_read_file_from_disk ( 
    char *file_name, 
    int flags, 
    mode_t mode );


// Worker
int
do_write_file_to_disk ( 
    char *file_name, 
    unsigned long size_in_sectors,
    unsigned long size_in_bytes,
    char *file_address,
    char flag );

void set_global_open_file(void *file, int Index);
void *get_global_open_file(int Index);

int __sys_fcntl_imp(int fd, int cmd, unsigned long arg);

int fs_initialize_dev_dir(void);

void fsInitializeWorkingDiretoryString(void);

// #ps: Global worker for now
int __fs_initialize_imp(void);

#endif   

