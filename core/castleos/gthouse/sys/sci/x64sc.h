// x64sc.h
// Syscall handlers.
// Called by the Assembler.
// see:
// sci.c
// ...

#ifndef __X86_64__X64SC_H
#define __X86_64__X64SC_H    1

void *sc80h ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

void *sc81h ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

void *sc82h ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

void *sc83h ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

#endif   


