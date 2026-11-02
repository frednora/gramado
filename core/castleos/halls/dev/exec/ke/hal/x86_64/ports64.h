// ports64.h
// i/o ports for x86_64.
// Created by Fred Nora.


#ifndef __KE_PORTS64_H
#define __KE_PORTS64_H    1

// I/O ports.
#define IO_MEMORY_BEGIN  0x0000
#define IO_MEMORY_END    0xFFFF  


// IO Delay
#define io_delay() \
    asm ("out %%al, $0x80"::);

//------------------------------------

unsigned char  in8  (unsigned short port);
unsigned short in16 (unsigned short port);
unsigned int   in32 (unsigned short port);

void out8  ( unsigned short port, unsigned char  data );
void out16 ( unsigned short port, unsigned short data );
void out32 ( unsigned short port, unsigned int   data );

//------------------------------------

//
// Delay stuff
//

void __x86_io_delay(void);
void wait_ns(int count);
void mdelay(int count);
void udelay(int count);

//
// Services to syscalls.
//

// Service 126
unsigned int portsx86_IN( int bits, unsigned short port );
// Service 127
void 
portsx86_OUT( 
    int bits, 
    unsigned short port, 
    unsigned int value );

#endif   


