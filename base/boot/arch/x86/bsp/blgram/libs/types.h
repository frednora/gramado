// types.h 
// Types descriptions for C99.
// Environment: x86 32bit machines.
// Created by Fred Nora.

#ifndef __TYPES_H
#define __TYPES_H    1

#define TRUE 1
#define FALSE 0

/*
 *  HANDLE
 */
typedef void *HANDLE;

/*
 *  Void
 */
typedef void VOID; 
 
/*
 *  Basics
 */
typedef char   CHAR;
typedef short  SHORT;
typedef int    INT;      //32bit
typedef long   LONG;     //32bit


/*
 *  Unsigned Basics
 */
typedef unsigned char  UCHAR;
typedef unsigned short USHORT;
typedef unsigned int   UINT;      //32bit
typedef unsigned long  ULONG;     //32bit

/*
 *  Ints 
 */ 
typedef char  int8_t;
typedef short int16_t;
typedef int   int32_t;

/*
 *  Unsigned Ints
 */ 
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

//
// Definições de tipos usados pelo Nelson el disk.h e disk.c
//

typedef char _i8;
typedef short _i16;
typedef int _i32;
typedef long long _i64;

typedef unsigned char _u8;
typedef unsigned short _u16;
typedef unsigned long _u32;
typedef unsigned long long _u64;

//typedef unsigned char uint8_t;
//typedef unsigned short uint16_t;
//typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef void _void;

//
// types used for the ELF support.
//

// See: exec_elf.h
typedef  signed char         __int8_t;
typedef  unsigned char       __uint8_t;
typedef  short               __int16_t;
typedef  unsigned short      __uint16_t;
typedef  int                 __int32_t;
typedef  unsigned int        __uint32_t;
//typedef  long long           __int64_t;
//typedef  unsigned long long  __uint64_t;
//...



#endif    


//
// End.
//

