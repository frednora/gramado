/*
 * File: fcntl.c
 * fcntl - manipulate file descriptor
 * 2020 - Created by Fred Nora.
 */

#include <types.h>
#include <sys/types.h>
#include <errno.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdio.h>
#include <rtl/gramado.h>

// gnu stuff?
int fallocate(int fd, int mode, off_t offset, off_t len)
{
    printf("fallocate: #todo\n");
    //if(fd<0){}
    return -1;
}

int posix_fallocate(int fd, off_t offset, off_t len)
{
    printf("posix_fallocate: #todo\n");
    //if(fd<0){}
    return -1;
}

// fcntl - manipulate file descriptor.
// Duplicating a file descriptor
// File descriptor flags
// File status flags
// Advisory record locking
// Open file description locks (non-POSIX)
// Mandatory locking (Deprecated on linux?)
// Lost locks
// Managing signals
// Leases (Concessões.)
// File and directory change notification (dnotify)
// Changing the capacity of a pipe
// File Sealing (Vedação de arquivo?)
// File seals limit the set of allowed operations on a given file.
// File read/write hints
// See:
// https://man7.org/linux/man-pages/man2/fcntl.2.html

int fcntl( int fd, int cmd, ... )
{
    int value = -1;

    if (fd<0){
        errno = EBADF;
        return (int)(-1);
    }

//++
    va_list ap;
    va_start(ap, cmd);
    unsigned arg = va_arg(ap,int);
    value = 
        (int) sc82 ( 
                  8001,
                  (unsigned long) fd,
                  (unsigned long) cmd,
                  (unsigned long) arg );
    va_end(ap);
//--

    if (value < 0){
        errno = (-value);
        return (int) (-1);
    }

    return (int) (value);
}


// openat - Open a file relative to a directory file descriptor. 
// See: https://linux.die.net/man/2/openat

int openat(int dirfd, const char *pathname, int flags)
{
    int value = -1;

// fd
// The directory fd.
    if (dirfd < 0){ 
        errno = EBADF;
        return (int) (-1);
    }

// path
    if ( (void *) pathname == NULL ){
        errno = EINVAL;
        return (int) (-1);
    }

// #todo
// #bugbug
// Check this syscall ... the argument dirfd.

// Carrega um arquivo dado o nome e um modo.
    value = 
        (int) gramado_system_call ( 
                  246, 
                  (unsigned long) dirfd, 
                  (unsigned long) pathname, 
                  (unsigned long) flags ); 

    if (value < 0){
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}

// open();
// Called by fopen().
int open(const char *pathname, int flags, mode_t mode)
{
    int value = -1;
    char tmp_path[64];
    size_t StringSize=0;

// String validation.
    //if( (void*) pathname == NULL )
    //    return -1;

// String size limits.
    //StringSize = (size_t) strlen(pathname);
    //if(StringSize>64)
        //StringSize=64;

// #importante
// adaptando para fat16.
// #todo: devemos fazer isso em ring0, não aqui.
// isso funcionou.

// #todo: nao fazer isso em ring3.
// pois ja estamos fazendo em ring0.
    //stdio_fntos( (char *) pathname );
    
    sprintf(tmp_path,pathname);

     //printf ("DEBUG-FOPEN: %s\n",tmp_path);

//
// Size
//

// Get file size.
// Limits: 1MB.
    
    /*
    FileSize = (size_t) gramado_system_call ( 178, 
                            (unsigned long) tmp_path, 0, 0 );

    if ( FileSize <= 0 || FileSize >= (1024*1024) )
    {
        printf ("open: [FAIL] FileSize\n");
        return (int) (-1);
    }
    */

// Syscall 16.
// O conteúdo do arquivo é carregado num buffer em ring0.
// O fd é retornado para que possamos ler usando read();
// IN: service, pathname, flags, mode 

    value = 
        (int) gramado_system_call( 
                  16,  
                  (unsigned long) tmp_path, 
                  (unsigned long) flags, 
                  (unsigned long) mode );

    if (value < 0){
        errno = (-value);
        return (int) (-1);
    }

// return fd.
// fopen() needs this. 
    return (int) value;
}


//#test
/*
int __open2(const char* path, int options, ...);
int __open2(const char* path, int options, ...)
{
    if (!path) {
        errno = EFAULT;
        return -1;
    }
    
    va_list ap;
    va_start(ap, options);
    auto mode = (mode_t)va_arg(ap, unsigned);
    va_end(ap);
    
    return openat ( path, options );
}
*/


// creat:
// Linux klibc style.
int creat(const char *pathname, mode_t mode)
{
    if ( (void*) pathname == NULL ){
        errno = EINVAL;
        return (int) (-1);
    }

    return (int) open(pathname, O_CREAT|O_WRONLY|O_TRUNC, mode);
}


/*
// MINIX 3
int flock(int fd, int mode);
int flock(int fd, int mode)
{
  struct flock lck;

  memset((void *) &lck, 0, sizeof(struct flock));
  switch (mode & ~LOCK_NB) {
  case LOCK_SH: lck.l_type = F_RDLCK; break;
  case LOCK_EX: lck.l_type = F_WRLCK; break;
  case LOCK_UN: lck.l_type = F_UNLCK; break;
  default: errno = EINVAL; return -1;
  }
  return fcntl(fd, mode & LOCK_NB ? F_SETLK : F_SETLKW, &lck);
}
*/

/*
 * flock:
 */
// #todo: It's a wrapper.
// Lock operation using fcntl()
// See: sys/file.h
int flock (int fd, int operation)
{
    debug_print("flock: [TODO]\n");

    if(fd<0)
    {
        errno = EBADF;
        return (int) (-1);
    }

// #todo

    return -1;
}

//
// End.
//

