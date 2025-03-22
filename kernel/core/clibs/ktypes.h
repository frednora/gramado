
// ktypes.h
// Created by Fred Nora.

#ifndef __PUBLIC_KTYPES_H
#define __PUBLIC_KTYPES_H    1 

#define TRUE  1
#define FALSE  0
#define NULL ((void *)0)

// VOID
typedef void  VOID; 
// It will help us to find the IRQ handlers.
typedef void  __VOID_IRQ; 

// Basics
typedef char   CHAR;   //  8bit
typedef short  SHORT;  // 16bit
typedef int    INT;    // 32bit
typedef long   LONG;   // 64bit

// Unsigned Basics
typedef unsigned char   UCHAR;   //  8bit
typedef unsigned short  USHORT;  // 16bit
typedef unsigned int    UINT;    // 32bit
typedef unsigned long   ULONG;   // 64bit

// sysv - unix system v
typedef unsigned char   unchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;
// extra
//typedef unsigned char   uchar;

//bsd
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

// Integers
typedef char   int8_t;
typedef short  int16_t;
typedef int    int32_t;
typedef long   int64_t;

// Unsigned Integers
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned long   uint64_t;

/* Windows CE virtual address */
typedef uint32_t  vaddr_t;
typedef uint32_t  vsize_t;

/* Physical address */
typedef uint32_t  paddr_t;
typedef uint32_t  psize_t;

/* kernel virtual address */
typedef uint32_t  kaddr_t;
typedef uint32_t  ksize_t;

//
// System types.
//
//typedef int tid_t;   //Thread id type.
//typedef int pid_t;   //Process id type.
//typedef int ppid_t;  //Parent process id type.
//typedef int uid_t;   //User id type.
//typedef int gid_t;   //Group id type.
//...


//
// pool
//

//tipo array de strings
typedef char **poll;
typedef char **POLL;

//array de unsigned longs contendo ponteiros 
//para pools de strings;
typedef unsigned long  pool_list_t;
typedef unsigned long  POOLLIST;


/*
 *  HANDLE
 */

typedef void *HANDLE;            // Generic handle.
typedef void *HPROCESS;          // Process.
typedef void *HTHREAD;           // Thread.
typedef void *HUSERSESSION;      // User Session.
typedef void *HWINDOWSTATION;    // Window Station.
typedef void *HDESKTOP;          // Desktop.
//typedef void *HWINDOW;           // Window.
//typedef void *HMENU;             // Menu.
//typedef void *KHANDLE;         // Kernel Handle.
//typedef void *OHANDLE;         // Object Handle. 
//typedef void *RPC_HANDLE;      // Remote Procedure Call Handle.
//typedef void *LPC_HANDLE;      // Local Procedure Call Handle.
//...

//??
typedef  unsigned long  vm_offset_t;
typedef  unsigned long  vm_size_t;

// Parent Process IDentifier 
typedef int  ppid_t;

// Thread IDentifier. linux,  
typedef int  tid_t; 

//
// types used for the ELF support.
//

// See: exec_elf.h

typedef signed char     __int8_t;
typedef unsigned char   __uint8_t;

typedef signed short    __int16_t;
typedef unsigned short  __uint16_t;

typedef signed int      __int32_t;
typedef unsigned int    __uint32_t;

typedef signed long     __int64_t;
typedef unsigned long   __uint64_t;


// BSD-style unsigned bits types.
typedef __uint8_t   u_int8_t;
typedef __uint16_t  u_int16_t;
typedef __uint32_t  u_int32_t;
typedef __uint64_t  u_int64_t;

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

#endif    

//
// End
//

