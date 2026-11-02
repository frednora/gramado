// ports64.c
// i/o ports for x86_64.
// Created by Fred Nora.

#include <kernel.h>

unsigned char 
in8 (unsigned short port)
{
    unsigned char ret=0;
 
    asm volatile (
        "inb %%dx, %%al"
        : "=a" ((unsigned char)ret)
        : "d"  ((unsigned short)port) );

    return (unsigned char) (ret & 0xFF);
}

unsigned short 
in16 (unsigned short port)
{
    unsigned short ret=0;

    asm volatile (
        "inw %%dx, %%ax" 
        : "=a" ((unsigned short)ret) 
        : "d" ((unsigned short)port) );

    return (unsigned short) (ret & 0xFFFF);
}

unsigned int 
in32 (unsigned short port)
{
    unsigned int ret=0;

    asm volatile (
        "inl %%dx, %%eax" 
        : "=a" ((unsigned int)ret) 
        : "d"  ((unsigned short)port) );

    return (unsigned int) (ret & 0xFFFFFFFF);
}

//===============================

void 
out8 ( 
    unsigned short port, 
    unsigned char data )
{
    asm volatile (
        "outb %%al, %%dx"
        :  // Nothing
        : "a" ((unsigned char)data), "d" ((unsigned short)port) );
}

void 
out16 (
    unsigned short port, 
    unsigned short data)
{
    asm volatile (
        "outw %%ax, %%dx" 
        :  // Nothing
        : "a" ((unsigned short)data), "d" ((unsigned short)port) );
}

void 
out32 ( 
    unsigned short port, 
    unsigned int data )
{
    asm volatile (
        "outl %%eax, %%dx" 
        :  // Nothing
        : "a" ((unsigned int)data), "d" ((unsigned short)port) );
}


//
// --------------
//



void __x86_io_delay (void)
{
    asm ("xorl %%eax, %%eax" ::);
    asm ("outb %%al, $0x80" ::);
    return;
}

void wait_ns (int count)
{
    count /= 100; 

    while (--count){
        hal_io_delay();
    };
}

// #todo: use unsigned long
void mdelay(int count)
{
    int i=0;
    for (i=0; i<(count*10000); i++)
    {
        hal_io_delay();
    };
}

// #todo: use unsigned long
void udelay(int count)
{
    int i=0;
    for (i=0; i<(count*10000); i++)
    {
        hal_io_delay();
    };
}


// Service 126.
// #todo
// Criar um parâmetro que seja um buffer para retornar o valor
// e retorno da função deve indicar se a operação foi concluida com
// sucesso ou não. Pois existe a possibilidade
// de o aplicativo enviar valores inválidos nos parâmetros.
unsigned int portsx86_IN ( int bits, unsigned short port )
{
    unsigned int Value=0;

// #todo:
// Check validations.
// Who can call this service?

    switch (bits){
    case 8:
        Value = (unsigned int) in8((unsigned short) port);
        return (unsigned int) (Value & 0xFF);
        break;
    case 16:
        Value = (unsigned int) in16((unsigned short) port);
        return (unsigned int) (Value & 0xFFFF);
        break;
    case 32:
        Value = (unsigned int) in32((unsigned short) port);
        return (unsigned int) (Value & 0xFFFFFFFF);
        break;
    };

    return 0;
}

// Service 127
// #todo
// Criar um parâmetro que seja um buffer para retornar o valor
// e retorno da função deve indicar se a operação foi concluida com
// sucesso ou não. Pois existe a possibilidade
// de o aplicativo enviar valores inválidos nos parâmetros.
void 
portsx86_OUT ( 
    int bits, 
    unsigned short port, 
    unsigned int value )
{

// #todo:
// Check validations.
// Who can call this service?

    switch (bits){
    case 8:
        out8 ( (unsigned short) port, (unsigned char) (value & 0xFF) );
        return;
        break;
    case 16:
        out16 ( (unsigned short) port, (unsigned short) (value & 0xFFFF) );
        return;
        break;
    case 32:
        out32 ( (unsigned short) port, (unsigned int) (value & 0xFFFFFFFF) );
        return;
        break;
    };
}








