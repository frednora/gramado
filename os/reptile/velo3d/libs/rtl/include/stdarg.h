
// stdarg.h

#ifndef __STDARG_H
#define __STDARG_H    1

// Same found in Sirius OS and Serenity OS.
typedef __builtin_va_list    va_list;

// Same found in Sirius OS.
#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)

#endif

