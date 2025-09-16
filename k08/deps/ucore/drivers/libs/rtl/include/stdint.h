
// stdint.h
// Definição de tipos de dados inteiros.



/*
typedef __UINT64_TYPE__ uint64_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT8_TYPE__ uint8_t;

typedef __INT64_TYPE__ int64_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT8_TYPE__ int8_t;

typedef __UINT_FAST8_TYPE__ uint_fast8_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;

typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __INT_FAST64_TYPE__ int_fast64_t;

typedef __UINT_LEAST8_TYPE__ uint_least8_t;
typedef __UINT_LEAST16_TYPE__ uint_least16_t;
typedef __UINT_LEAST32_TYPE__ uint_least32_t;
typedef __UINT_LEAST64_TYPE__ uint_least64_t;

typedef __INT_LEAST8_TYPE__ int_least8_t;
typedef __INT_LEAST16_TYPE__ int_least16_t;
typedef __INT_LEAST32_TYPE__ int_least32_t;
typedef __INT_LEAST64_TYPE__ int_least64_t;
*/


/*
#define __int8_t_defined 1
#define __uint8_t_defined 1
#define __int16_t_defined 1
#define __uint16_t_defined 1
#define __int32_t_defined 1
#define __uint32_t_defined 1
#define __int64_t_defined 1
#define __uint64_t_defined 1
*/


/*
typedef __PTRDIFF_TYPE__ uintptr_t;
typedef __PTRDIFF_TYPE__ intptr_t;
*/


//typedef uint64_t uintmax_t
typedef uint32_t uintmax_t;

//#bugbug hack hack
#define uint64_t uint32_t

/* 7.18.1.1 Exact-width integer types (also in sys/types.h) */
/*
#ifndef	_INT8_T_DEFINED_
#define	_INT8_T_DEFINED_
typedef	__int8_t		int8_t;
#endif

#ifndef	_UINT8_T_DEFINED_
#define	_UINT8_T_DEFINED_
typedef	__uint8_t		uint8_t;
#endif

#ifndef	_INT16_T_DEFINED_
#define	_INT16_T_DEFINED_
typedef	__int16_t		int16_t;
#endif

#ifndef	_UINT16_T_DEFINED_
#define	_UINT16_T_DEFINED_
typedef	__uint16_t		uint16_t;
#endif

#ifndef	_INT32_T_DEFINED_
#define	_INT32_T_DEFINED_
typedef	__int32_t		int32_t;
#endif

#ifndef	_UINT32_T_DEFINED_
#define	_UINT32_T_DEFINED_
typedef	__uint32_t		uint32_t;
#endif

#ifndef	_INT64_T_DEFINED_
#define	_INT64_T_DEFINED_
typedef	__int64_t		int64_t;
#endif

#ifndef	_UINT64_T_DEFINED_
#define	_UINT64_T_DEFINED_
typedef	__uint64_t		uint64_t;
#endif
*/


/*
//7.18.1.2 Minimum-width integer types 
typedef	__int_least8_t		int_least8_t;
typedef	__uint_least8_t		uint_least8_t;
typedef	__int_least16_t		int_least16_t;
typedef	__uint_least16_t	uint_least16_t;
typedef	__int_least32_t		int_least32_t;
typedef	__uint_least32_t	uint_least32_t;
typedef	__int_least64_t		int_least64_t;
typedef	__uint_least64_t	uint_least64_t;

//7.18.1.3 Fastest minimum-width integer types 
typedef	__int_fast8_t		int_fast8_t;
typedef	__uint_fast8_t		uint_fast8_t;
typedef	__int_fast16_t		int_fast16_t;
typedef	__uint_fast16_t		uint_fast16_t;
typedef	__int_fast32_t		int_fast32_t;
typedef	__uint_fast32_t		uint_fast32_t;
typedef	__int_fast64_t		int_fast64_t;
typedef	__uint_fast64_t		uint_fast64_t;
*/

/* 7.18.1.4 Integer types capable of holding object pointers */
/*
#ifndef	_INTPTR_T_DEFINED_
#define	_INTPTR_T_DEFINED_
typedef	__intptr_t		intptr_t;
#endif
*/

//typedef	__uintptr_t		uintptr_t;

/* 7.18.1.5 Greatest-width integer types */
//typedef	__intmax_t		intmax_t;
//typedef	__uintmax_t		uintmax_t;






#define INT8_MIN    (-0x7f - 1)
#define INT16_MIN   (-0x7fff - 1)
#define INT32_MIN   (-0x7fffffff - 1)
#define INT64_MIN   INT32_MIN //(-0x7fffffffffffffff - 1)


#define INT8_MAX    0x7f 
#define INT16_MAX   0x7fff 
#define INT32_MAX   0x7fffffff 
#define INT64_MAX   INT32_MAX  //   0x7fffffffffffffff 

#define UINT8_MAX   0xff 
#define UINT16_MAX  0xffff 
#define UINT32_MAX  0xffffffff 
#define UINT64_MAX  UINT32_MAX //0xffffffffffffffff 



/* 7.18.2.2 Limits of minimum-width integer types */
#define	INT_LEAST8_MIN		INT8_MIN
#define	INT_LEAST16_MIN		INT16_MIN
#define	INT_LEAST32_MIN		INT32_MIN
#define	INT_LEAST64_MIN		INT64_MIN

#define	INT_LEAST8_MAX		INT8_MAX
#define	INT_LEAST16_MAX		INT16_MAX
#define	INT_LEAST32_MAX		INT32_MAX
#define	INT_LEAST64_MAX		INT64_MAX

#define	UINT_LEAST8_MAX		UINT8_MAX
#define	UINT_LEAST16_MAX	UINT16_MAX
#define	UINT_LEAST32_MAX	UINT32_MAX
#define	UINT_LEAST64_MAX	UINT64_MAX


/* 7.18.2.3 Limits of fastest minimum-width integer types */

/*
#define	INT_FAST8_MIN		__INT_FAST8_MIN
#define	INT_FAST16_MIN		__INT_FAST16_MIN
#define	INT_FAST32_MIN		__INT_FAST32_MIN
#define	INT_FAST64_MIN		__INT_FAST64_MIN

#define	INT_FAST8_MAX		__INT_FAST8_MAX
#define	INT_FAST16_MAX		__INT_FAST16_MAX
#define	INT_FAST32_MAX		__INT_FAST32_MAX
#define	INT_FAST64_MAX		__INT_FAST64_MAX

#define	UINT_FAST8_MAX		__UINT_FAST8_MAX
#define	UINT_FAST16_MAX		__UINT_FAST16_MAX
#define	UINT_FAST32_MAX		__UINT_FAST32_MAX
#define	UINT_FAST64_MAX		__UINT_FAST64_MAX
*/


/* 7.18.2.4 Limits of integer types capable of holding object pointers */
/*
#ifdef __LP64__
#define	INTPTR_MIN		(-0x7fffffffffffffffL - 1)
#define	INTPTR_MAX		0x7fffffffffffffffL
#define	UINTPTR_MAX		0xffffffffffffffffUL
#else
#define	INTPTR_MIN		(-0x7fffffffL - 1)
#define	INTPTR_MAX		0x7fffffffL
#define	UINTPTR_MAX		0xffffffffUL
#endif
*/

/* 7.18.2.5 Limits of greatest-width integer types */
#define	INTMAX_MIN		INT64_MIN
#define	INTMAX_MAX		INT64_MAX
#define	UINTMAX_MAX		UINT64_MAX



/*
 * 7.18.3 Limits of other integer types.
 *
 * The following object-like macros specify the minimum and maximum limits
 * of integer types corresponding to types specified in other standard
 * header files.
 */

/* Limits of ptrdiff_t */
#define	PTRDIFF_MIN		INTPTR_MIN
#define	PTRDIFF_MAX		INTPTR_MAX

/* Limits of sig_atomic_t */
#define	SIG_ATOMIC_MIN		INT32_MIN
#define	SIG_ATOMIC_MAX		INT32_MAX

/* Limit of size_t */
#ifndef	SIZE_MAX
#define	SIZE_MAX		UINTPTR_MAX
#endif

/* Limits of wchar_t */
#ifndef	WCHAR_MIN
#define	WCHAR_MIN		INT32_MIN
#endif
#ifndef	WCHAR_MAX
#define	WCHAR_MAX		INT32_MAX
#endif

/* Limits of wint_t */
#define	WINT_MIN		INT32_MIN
#define	WINT_MAX		INT32_MAX


/* 7.18.4.1 Macros for minimum-width integer constants. */
#define	INT8_C(_c)		(_c)
#define	INT16_C(_c)		(_c)
#define	INT32_C(_c)		(_c)
//#define	INT64_C(_c)		__CONCAT(_c, LL)

#define	UINT8_C(_c)		(_c)
#define	UINT16_C(_c)		(_c)
//#define	UINT32_C(_c)		__CONCAT(_c, U)
//#define	UINT64_C(_c)		__CONCAT(_c, ULL)

/* 7.18.4.2 Macros for greatest-width integer constants. */
//#define	INTMAX_C(_c)		__CONCAT(_c, LL)
//#define	UINTMAX_C(_c)		__CONCAT(_c, ULL)






