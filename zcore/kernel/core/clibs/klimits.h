// klimits.h 
// It defines some limits in the libc.
// ring 0.
// 2005 - Created by Fred Nora.

#ifndef ____LIMITS_H
#define ____LIMITS_H    1

// Max number of open files in a process.
#define OPEN_MAX  GRAMADO_FOPEN_MAX

#define PIPE_BUF  GRAMADO_PIPE_BUF

// Number of bits in a 'char'.  
#define CHAR_BIT  8
//#todo: Use default lib.
#define HEX_LEN  8

//
// CHAR
//

// Minimum and maximum values a 'signed char' can hold.  
#define SCHAR_MIN  0x80  // (-128)
#define SCHAR_MAX  0x7f  // 127
// Maximum value an 'unsigned char' can hold.  (Minimum is 0.)  
#define UCHAR_MAX  0xff  // 255

//
// SHORT
//

// Minimum and maximum values a 'signed short int' can hold.  
// 0x8000  0x7fff
#define SHRT_MIN  0x8000  //(-32768)
#define SHRT_MAX  0x7fff  //32767
// Maximum value an 'unsigned short int' can hold.  (Minimum is 0.) 
// 0xffff
#define USHRT_MAX  0xffff  //65535

//
//  INT
//

#define SINT_MIN  0x80000000
#define SINT_MAX  0x7fffffff
#define UINT_MAX  0xffffffff

// Alias
// signed int
#define ____INT_MAX  SINT_MAX

//
// long
//

//#warning: ?
#define SLONG_MIN  0x8000000000000000
#define SLONG_MAX  0x7fffffffffffffff 
#define ULONG_MAX  0xffffffffffffffff

#endif    

