

#ifndef __MMAN_H
#define __MMAN_H    1


#define  MAP_ANONYMOUS   0x20

#define  MAP_ANON   MAP_ANONYMOUS
 
#define  MAP_FAILED   ((void *)-1)
 
#define  MAP_FILE   0
#define  MAP_FIXED   0x10
#define  MAP_PRIVATE   2
#define  MAP_SHARED   1
#define  MAP_TYPE   0xF
#define  PROT_EXEC   4
#define  PROT_NONE   0
#define  PROT_READ   1
#define  PROT_WRITE   2


// POSIX.1-2001, POSIX.1-2008, SVr4, 4.4BSD.
void *mmap ( 
    void *addr, 
    size_t length, 
    int prot, 
    int flags,
    int fd, 
    off_t offset );

// POSIX.1-2001, POSIX.1-2008, SVr4, 4.4BSD.
int munmap (void *addr, size_t length);


//mlock, munlock, mlockall, munlockall - lock and unlock memory
//POSIX.1-2001, SVr4.
int mlock (const void *addr, size_t len);
int munlock (const void *addr, size_t len);
int mlockall (int	flags);
int munlockall (void);


/*
 #todo: https://linux.die.net/man/3/msync
int msync(void *addr, size_t len, int flags); 
int msync(void *addr, size_t len, int flags){}
*/


#endif    


