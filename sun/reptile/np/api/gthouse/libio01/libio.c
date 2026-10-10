/*
 * File: libio.c
 * Ring 3 i/o library.
 * It's very dangerous.
 * But it helps to implement drivers in ring3 to port it 
 * to kernel in the future.
 * 2019 - Created by Fred Nora.
 */
// [ libio ]
// I just created a ring3 i/o library, 
// given to the ring3 processes full access to the i/o ports. Why not?!
// This way i can build new drivers in ring 3, 
// with no risk for the system. So I will only port to ring0 the well tested drivers.  
// Acesso a portas para processos em user mode.
// Será usado pelos servidores e drivers.
// O privilégio será baseado em 'usuário' e 'desktop'.
//# todo
//+ usar system_call para acessar o kernel
//+ criar lá no kernel as rotinas que atendem essa system call.
// #todo
// Essa bibliorteca poderia ter uma parte relativa
// a autorizações.
// #todo
// Precisamos checar essas chamadas ao kernel.

#include <libio.h>

// Protótipo da system call.
unsigned long
__libio_system_call ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx );

/*
 * __libio_system_call:
 *     system call usada pelo gdeio para acessar as portas.
 */
unsigned long
__libio_system_call ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx )
{
    unsigned long Ret = 0;

// System interrupt.
    asm volatile ( " int %1 \n"
        : "=a"(Ret)
        : "i"(0x80), "a"(ax), "b"(bx), "c"(cx), "d"(dx) );

    return (unsigned long) Ret; 
}

//
// == IN ======================================================
//

// #bugbug: return size
// retorna o valor.
unsigned char libio_inport8 (unsigned short port)
{
    unsigned char _Ret=0;

    _Ret = 
        (unsigned char) __libio_system_call ( 
                            (unsigned long) 126, 
                            (unsigned long) 8, 
                            (unsigned long) (port & 0xFFFF), 
                            (unsigned long) (port & 0xFFFF) );

    return (unsigned char) (_Ret & 0xFF);
}

//retorna o valor.
unsigned short libio_inport16 (unsigned short port)
{
    unsigned short _Ret=0;

    _Ret = 
        (unsigned short) __libio_system_call ( 
                             (unsigned long) 126, 
                             (unsigned long) 16, 
                             (unsigned long) (port & 0xFFFF), 
                             (unsigned long) (port & 0xFFFF) );

    return (unsigned short) (_Ret & 0xFFFF);
}

//retorna o valor.
unsigned int libio_inport32 (unsigned short port)
{
    unsigned int _Ret=0;

    _Ret = 
        (unsigned int) __libio_system_call ( 
                           (unsigned long) 126, 
                           (unsigned long) 32, 
                           (unsigned long) (port & 0xFFFF), 
                           (unsigned long) (port & 0xFFFF) );

    return (unsigned int) (_Ret & 0xFFFFFFFF);
}

//
// == OUT ======================================================
//

void libio_outport8 ( unsigned short port, unsigned char value )
{
    __libio_system_call ( 
        (unsigned long) 127, 
        (unsigned long) 8, 
        (unsigned long) (port & 0xFFFF), 
        (unsigned long) (value & 0xFF) );
}

void libio_outport16 ( unsigned short port, unsigned short value )
{
    __libio_system_call ( 
        (unsigned long) 127, 
        (unsigned long) 16, 
        (unsigned long) (port & 0xFFFF), 
        (unsigned long) (value & 0xFFFF) );
}

void libio_outport32 ( unsigned short port, unsigned int value )
{
    __libio_system_call ( 
        (unsigned long) 127, 
        (unsigned long) 32, 
        (unsigned long) (port & 0xFFFF), 
        (unsigned long) (value & 0xFFFFFFFF) );
}

//
// End
//

