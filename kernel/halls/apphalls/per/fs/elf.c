// fs/elf.c
// ELF images support.
// Created by Fred Nora.

// See:
// fs/exec_elf.h

#include <kernel.h>

// Worker
int elfCheckSignature(unsigned long address)
{
    unsigned char *buf = (unsigned char *) address;

// Parameter
    if ((void *) buf == NULL){
        printk ("elfCheckSignature: buf\n");
        goto fail;
    }

// Check signature
// ELFMAG0
    if ( buf[0] != 0x7F ||
         buf[1] != 0x45 || buf[2] != 0x4C || buf[3] != 0x46 )
    {
        printk ("elfCheckSignature: Sig\n");
        goto fail;
    }

// ok
    return 0;
fail:
    return (int) -1;
}

