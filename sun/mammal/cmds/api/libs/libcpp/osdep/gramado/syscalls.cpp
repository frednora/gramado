
// syscalls.cpp 
// libcpp syscalls for Gramado OS.
// 2024 - Created by Fred Nora.

// =========================

#include <syscalls.hpp>

namespace LibCPP {


// System call.
// Interrupt 0x80.
void* Syscalls::sc80 ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x80), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// System call.
// Interrupt 0x81.
void* Syscalls::sc81 ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x81), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// System call.
// Interrupt 0x82.
void* Syscalls::sc82 ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x82), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// System call.
// Interrupt 0x83.
void* Syscalls::sc83 ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x83), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

}

