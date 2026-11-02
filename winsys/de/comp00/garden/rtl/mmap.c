/*
 * File: mmap.c 
 *     mmap, munmap - map or unmap files or devices into memory.
 *     Created by Fred Nora.
 */

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/mman.h>


/*
PROT_EXEC
    Pages may be executed.
PROT_READ
    Pages may be read.
PROT_WRITE
    Pages may be written.
PROT_NONE
    Pages may not be accessed.
*/

// #todo
// Explain the input parameters.
// See:
// https://man7.org/linux/man-pages/man2/mmap.2.html
void *mmap ( 
    void *addr, 
    size_t length, 
    int prot, 
    int flags,
    int fd, 
    off_t offset )
{
    debug_print ("mmap: [TODO]\n");
    
    //if (fd<0){}
    
    return NULL; 
}

//#todo
int munmap (void *addr, size_t length)
{
    debug_print ("munmap: [TODO]\n");
    return -1;
}

