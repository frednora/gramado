// chicago.h
// Chicago OS: Italo M.


#ifndef __CHICAGO_H__
#define __CHICAGO_H__


/* Base types */

#define Void   void
#define PVoid  void *

#define Char   char
#define PChar  char *

#define Short   short
#define PShort  short *

#define Int   int
#define PInt  int *

#define Long   long
#define PLong  long *

#define Float   float
#define PFloat  float *

#define Double   double
#define PDouble  double *


/* Unsigned types */

typedef unsigned Char     UInt8;
typedef unsigned Char *  PUInt8;

typedef unsigned Short     UInt16;
typedef unsigned Short *  PUInt16;

typedef unsigned Int     UInt32;
typedef unsigned Int *  PUInt32;


typedef unsigned Long     ULong;
typedef unsigned Long *  PULong;


//typedef unsigned Long Long     UInt64;
//typedef unsigned Long Long *  PUInt64;


/* Signed types */

typedef signed Char     Int8;
typedef signed Char *  PInt8;

typedef signed Short     Int16;
typedef signed Short *  PInt16;

typedef signed Int     Int32;
typedef signed Int *  PInt32;

typedef signed Int     WChar;
typedef signed Int *  PWChar;

//typedef signed Long Long     Int64;
//typedef signed Long Long *  PInt64;


/* Define our IntPtr type */


//#define UINTPTR_MAX 0xFFFFFFFF

typedef unsigned Int     UIntPtr;
typedef unsigned Int *  PUIntPtr;

typedef signed Int     IntPtr;
typedef signed Int *  PIntPtr;


/* Inline assembly */

#define Asm __asm__
#define Volatile __volatile__


/* Attributes */

#define Packed        __attribute__((packed))
#define Aligned(x)    __attribute__((aligned(x)))
#define Optional      __attribute__((weak))


/* Boolean and other defines */

typedef Char     Boolean;
typedef Char *  PBoolean;

#define True 1
#define False 0

#define Null ( (PVoid) 0 )

#define Const const



#endif


