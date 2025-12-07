// kunistd.h
// Standard unix routines.
// Created by Fred Nora.

#ifndef __LIBK_KUNISTD_H
#define __LIBK_KUNISTD_H    1

#define STDIN_FILENO   0  // Standard input file descriptor
#define STDOUT_FILENO  1  // Standard output file descriptor
#define STDERR_FILENO  2  // Standard error file descriptor

//
// Process
//

// Process IDentifier. (posix)
typedef int  pid_t;

// Parent Process IDentifier 
typedef int  ppid_t;

// Process Group IDentifier.
typedef int  pgid_t;

//
// Thread
//

// Thread IDentifier.
typedef int  tid_t; 

// Thread IDentifier.
typedef int  tgid_t; 

//
// Prototypes ======================
//

// sys_execve:
// This is a work in progress.
int 
sys_execve ( 
    const char *path, 
    char *const argv[], 
    char *const envp[] );

long fpathconf(int fildes, int name);
long pathconf(const char *pathname, int name);

int sys_gethostname(char *ubuff);
int sys_sethostname(const char *new_hostname, size_t len);

off_t sys_lseek(int fd, off_t offset, int whence);

// See: sys.c
unsigned long sys_alarm(unsigned long seconds);

#endif    

