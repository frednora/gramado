// kstdarg.h
// Created by Fred Nora.

#ifndef __LIBK_STDARG_H
#define __LIBK_STDARG_H    1

// Credits: 
// kinguio, by Nelson Cole.
typedef __builtin_va_list  va_list;
#define va_start(v,l)  __builtin_va_start(v,l)
#define va_arg(v,l)    __builtin_va_arg(v,l)
#define va_end(v)      __builtin_va_end(v)
#define va_copy(d,s)   __builtin_va_copy(d,s)


/*
// #test:
// I dont understand it very well,
// let's use the embedded version for now ;)

// the following extern "C" code is needed to ensure the
// compiler intrinsic IV_VA_START is invoked.
extern "C" void __cdecl __va_start(va_list *, ...);

#define va_dcl          va_list va_alist;
#define va_start(ap,x)   ( __va_start(&ap, x) )
#define va_arg(ap, t)   \
    ( ( sizeof(t) > sizeof(__int64) || ( sizeof(t) & (sizeof(t) - 1) ) != 0 ) \
        ? **(t **)( ( ap += sizeof(__int64) ) - sizeof(__int64) ) \
        :  *(t  *)( ( ap += sizeof(__int64) ) - sizeof(__int64) ) )
#define va_end(ap)      ( ap = (va_list)0 )
*/


#endif    

