
// _types.h

#ifndef ___TYPES_H
#define ___TYPES_H    1


// Basic types upon which most other types are built.
// see: types.h
typedef signed char    __int8_t;
typedef unsigned char  __uint8_t;
typedef signed short    __int16_t;
typedef unsigned short  __uint16_t;
typedef signed int    __int32_t;
typedef unsigned int  __uint32_t;
typedef signed long    __int64_t;
typedef unsigned long  __uint64_t;




// Standard type definitions.

typedef  __uint32_t  __clock_t;    /* clock()... */
typedef  __int32_t   __critical_t;
typedef  double     __double_t;
typedef  float      __float_t;
typedef  __int32_t  __intfptr_t;
typedef  __int64_t  __intmax_t;
typedef  __int32_t  __intptr_t;

typedef  __int32_t  __int_fast8_t;
typedef  __int32_t  __int_fast16_t;
typedef  __int32_t  __int_fast32_t;
typedef  __int64_t  __int_fast64_t;

typedef  __int8_t   __int_least8_t;
typedef  __int16_t  __int_least16_t;
typedef  __int32_t  __int_least32_t;
typedef  __int64_t  __int_least64_t;

typedef  __int32_t	__ptrdiff_t;		/* ptr1 - ptr2 */
typedef  __int32_t	__register_t;
typedef  __int32_t	__segsz_t;		/* segment size (in pages) */
typedef  __uint32_t	__size_t;		/* sizeof() */
typedef  __int32_t	__ssize_t;		/* byte count or error */
typedef  __int64_t	__time_t;		/* time()... */
typedef  __uint32_t	__uintfptr_t;
typedef  __uint64_t	__uintmax_t;
typedef  __uint32_t	__uintptr_t;

typedef  __uint32_t  __uint_fast8_t;
typedef  __uint32_t  __uint_fast16_t;
typedef  __uint32_t  __uint_fast32_t;
typedef  __uint64_t  __uint_fast64_t;

typedef  __uint8_t   __uint_least8_t;
typedef  __uint16_t  __uint_least16_t;
typedef  __uint32_t  __uint_least32_t;
typedef  __uint64_t  __uint_least64_t;

typedef  __uint32_t  __u_register_t;

typedef  __uint32_t  __vm_offset_t;
typedef  __int64_t   __vm_ooffset_t;
typedef  __uint32_t  __vm_paddr_t;
typedef  __uint64_t  __vm_pindex_t;
typedef  __uint32_t  __vm_size_t;

#endif    


