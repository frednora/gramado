/*
 * File: stddef.h 
 * About:
 *     Header for standard defines. C99.
 */


/* 
enum {
	false	= 0,
	true	= 1
};
 */


/* 
    @todo:
Macros:
   NULL
   offsetof(,);

Variables:
    typedef ptrdiff_t
    typedef wchar_t
    typedef size_t
*/

// ??
//typedef __PTRDIFF_TYPE__ ptrdiff_t;
//typedef __SIZE_TYPE__ size_t;


// This define is also in types.h.
#ifndef NULL
#define NULL ((void *)0)
#endif

// Signed integral type of the result of 
// subtracting two pointers.
typedef int  ptrdiff_t;
  
/*
 * Integral type whose range of values can represent distinct wide-character 
 * codes for all members of the largest character set specified among the 
 * locales supported by the compilation environment:
 * the null character has the code value 0 and each member of the Portable 
 * Character Set has a code value equal to its value when used as the lone 
 * character in an integer character constant.
 */ 
typedef int  wchar_t;

/* 
 * which is the unsigned integer type of the result of the sizeof operator;   
 */
//typedef unsigned int size_t;
typedef int  size_t;

// offset_of - "Deslocamento dentro de uma estrutura"
#define offset_of(tp, member) (((char*) &((tp*)0)->member) - (char*)0)
//linux like. 
// #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

//#todo: Use this one.
//#define offsetof(type, member)    __builtin_offsetof(type, member)

//
// End.
//

