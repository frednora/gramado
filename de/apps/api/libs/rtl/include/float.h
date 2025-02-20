
//credits.
//Copyright (C) 2002-2011 Michael Ringgaard.
 
//All rights reserved. 
	
#ifndef FLOAT_H
#define FLOAT_H 1

//
// Limits for float
//

#define FLT_RADIX      2
#define FLT_ROUNDS     1
#define FLT_DIG        6
#define FLT_EPSILON    1.192092896e-07F
#define FLT_MANT_DIG   24
#define FLT_MAX        3.402823466e+38F
#define FLT_MAX_10_EXP 38
#define FLT_MAX_EXP    128
#define FLT_MIN        1.175494351e-38F
#define FLT_MIN_10_EXP (-37)
#define FLT_MIN_EXP    (-125)

//
// Limits for double
//

#define DBL_DIG        15
#define DBL_EPSILON    2.2204460492503131e-016
#define DBL_MANT_DIG   53
#define DBL_MAX        1.7976931348623158e+308
#define DBL_MAX_10_EXP 308
#define DBL_MAX_EXP    1024
#define DBL_MIN        2.2250738585072014e-308
#define DBL_MIN_10_EXP (-307)
#define DBL_MIN_EXP    (-1021)

//
// Floating point classes for _fpclass()
//

#define _FPCLASS_SNAN   0x0001  // Signaling NaN
#define _FPCLASS_QNAN   0x0002  // Quiet NaN
#define _FPCLASS_NINF   0x0004  // Negative infinity
#define _FPCLASS_NN     0x0008  // Negative normal
#define _FPCLASS_ND     0x0010  // Negative denormal
#define _FPCLASS_NZ     0x0020  // -0
#define _FPCLASS_PZ     0x0040  // +0
#define _FPCLASS_PD     0x0080  // Positive denormal
#define _FPCLASS_PN     0x0100  // Positive normal
#define _FPCLASS_PINF 0x0200 // Positive infinity


//glibc
/*
# if __HAVE_FLOAT128
#  define FLT128_MANT_DIG	113
#  define FLT128_DECIMAL_DIG	36
#  define FLT128_DIG		33
#  define FLT128_MIN_EXP	(-16381)
#  define FLT128_MIN_10_EXP	(-4931)
#  define FLT128_MAX_EXP	16384
#  define FLT128_MAX_10_EXP	4932
#  define FLT128_MAX					\
  __f128 (1.18973149535723176508575932662800702e+4932)
#  define FLT128_EPSILON				\
  __f128 (1.92592994438723585305597794258492732e-34)
#  define FLT128_MIN					\
  __f128 (3.36210314311209350626267781732175260e-4932)
#  define FLT128_TRUE_MIN				\
  __f128 (6.47517511943802511092443895822764655e-4966)
# endif
*/



//Types other than _Float128 are typedefs for other types with old
 //compilers.  

//glibc
/*
# if __HAVE_FLOAT32
#  define FLT32_MANT_DIG	FLT_MANT_DIG
#  define FLT32_DECIMAL_DIG	FLT_DECIMAL_DIG
#  define FLT32_DIG		FLT_DIG
#  define FLT32_MIN_EXP		FLT_MIN_EXP
#  define FLT32_MIN_10_EXP	FLT_MIN_10_EXP
#  define FLT32_MAX_EXP		FLT_MAX_EXP
#  define FLT32_MAX_10_EXP	FLT_MAX_10_EXP
#  define FLT32_MAX		FLT_MAX
#  define FLT32_EPSILON		FLT_EPSILON
#  define FLT32_MIN		FLT_MIN
#  define FLT32_TRUE_MIN	FLT_TRUE_MIN
# endif  
 */


//glibc
/*
# if __HAVE_FLOAT64
#  define FLT64_MANT_DIG	DBL_MANT_DIG
#  define FLT64_DECIMAL_DIG	DBL_DECIMAL_DIG
#  define FLT64_DIG		DBL_DIG
#  define FLT64_MIN_EXP		DBL_MIN_EXP
#  define FLT64_MIN_10_EXP	DBL_MIN_10_EXP
#  define FLT64_MAX_EXP		DBL_MAX_EXP
#  define FLT64_MAX_10_EXP	DBL_MAX_10_EXP
#  define FLT64_MAX		DBL_MAX
#  define FLT64_EPSILON		DBL_EPSILON
#  define FLT64_MIN		DBL_MIN
#  define FLT64_TRUE_MIN	DBL_TRUE_MIN
# endif
*/

//glibc
/*
# if __HAVE_FLOAT32X
#  define FLT32X_MANT_DIG	DBL_MANT_DIG
#  define FLT32X_DECIMAL_DIG	DBL_DECIMAL_DIG
#  define FLT32X_DIG		DBL_DIG
#  define FLT32X_MIN_EXP	DBL_MIN_EXP
#  define FLT32X_MIN_10_EXP	DBL_MIN_10_EXP
#  define FLT32X_MAX_EXP	DBL_MAX_EXP
#  define FLT32X_MAX_10_EXP	DBL_MAX_10_EXP
#  define FLT32X_MAX		DBL_MAX
#  define FLT32X_EPSILON	DBL_EPSILON
#  define FLT32X_MIN		DBL_MIN
#  define FLT32X_TRUE_MIN	DBL_TRUE_MIN
# endif
*/


//glibc
/*
# if __HAVE_FLOAT64X
#  if __HAVE_FLOAT64X_LONG_DOUBLE
#   define FLT64X_MANT_DIG	LDBL_MANT_DIG
#   define FLT64X_DECIMAL_DIG	LDBL_DECIMAL_DIG
#   define FLT64X_DIG		LDBL_DIG
#   define FLT64X_MIN_EXP	LDBL_MIN_EXP
#   define FLT64X_MIN_10_EXP	LDBL_MIN_10_EXP
#   define FLT64X_MAX_EXP	LDBL_MAX_EXP
#   define FLT64X_MAX_10_EXP	LDBL_MAX_10_EXP
#   define FLT64X_MAX		LDBL_MAX
#   define FLT64X_EPSILON	LDBL_EPSILON
#   define FLT64X_MIN		LDBL_MIN
#   define FLT64X_TRUE_MIN	LDBL_TRUE_MIN
#  else
#   define FLT64X_MANT_DIG	FLT128_MANT_DIG
#   define FLT64X_DECIMAL_DIG	FLT128_DECIMAL_DIG
#   define FLT64X_DIG		FLT128_DIG
#   define FLT64X_MIN_EXP	FLT128_MIN_EXP
#   define FLT64X_MIN_10_EXP	FLT128_MIN_10_EXP
#   define FLT64X_MAX_EXP	FLT128_MAX_EXP
#   define FLT64X_MAX_10_EXP	FLT128_MAX_10_EXP
#   define FLT64X_MAX		FLT128_MAX
#   define FLT64X_EPSILON	FLT128_EPSILON
#   define FLT64X_MIN		FLT128_MIN
#   define FLT64X_TRUE_MIN	FLT128_TRUE_MIN
#  endif
# endif
*/


#endif  /* float.h*/


