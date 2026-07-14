// rtl.cpp
// Low level routines and wrappers starting with rtl_.

#include "rtl.hpp"

namespace rtl {

// C syscall implementations
extern "C" {

// System call.
// Interrupt 0x80.
void *sc80 ( 
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
void *sc81 ( 
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
void *sc82 ( 
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
void *sc83 ( 
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

} // extern "C"


//
// ------------------------------
//

// C++ static class wrappers
void* Syscall::cpp_sc80(
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d) 
{
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x80), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// C++ static class wrappers
void* Syscall::cpp_sc81(
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d) 
{
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x81), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// C++ static class wrappers
void* Syscall::cpp_sc82(
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d) 
{
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x82), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// C++ static class wrappers
void* Syscall::cpp_sc83(
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d) 
{
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x83), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

} // namespace rtl

