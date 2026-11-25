
// ports64.h

#ifndef __PORTS64_H
#define __PORTS64_H    1

//
// Delay stuff
//

// IO Delay
#define io_delay() \
    asm("out %%al,$0x80"::);

//------------------------------------

void __x86_io_delay(void);
void mdelay(int count);
void udelay(int count);
void wait_ns(int count);

//------------------------------------

unsigned char  in8  (unsigned short port);
unsigned short in16 (unsigned short port);
unsigned int   in32 (unsigned short port);

void out8  ( unsigned short port, unsigned char  data );
void out16 ( unsigned short port, unsigned short data );
void out32 ( unsigned short port, unsigned int   data );

//------------------------------------

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


