// ports.c 
// System ports support.
// 2005 - Created by Fred Nora.

#include "../bl.h"


unsigned char in8 (int port)
{
    unsigned char ret=0;
 
    asm volatile ("inb %%dx, %%al" : "=a"(ret): "d"(port) );

    return (unsigned char) ret;
}

unsigned short in16 (int port)
{
    unsigned short ret=0;

    asm volatile ("inw %%dx, %%ax" : "=a" (ret) : "d" (port) );

    return (unsigned short) ret;
}

unsigned long in32 (int port)
{
    unsigned long ret=0;

    asm volatile ("inl %%dx,%%eax" : "=a" (ret) : "d"(port) );

    return (unsigned long) ret;
}

void out8 ( int port, unsigned char data )
{
    asm volatile ("outb %%al, %%dx" :: "a" (data), "d" (port) );
}

void out16 (int port, unsigned short data)
{
    asm volatile ("outw %%ax, %%dx" :: "a" (data), "d" (port) );
}

void out32 ( int port, unsigned long data )
{
    asm volatile ("outl %%eax, %%dx" :: "a" (data), "d" (port) );
}

//
// End.
//

