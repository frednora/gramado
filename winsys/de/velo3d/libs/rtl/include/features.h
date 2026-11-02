
/*
 * File: features.h
 *
 * Semelhante à glibc, aqui será o lugar de coniguração da biblioteca.
 *
 * History:
 *     2019 - Created by Fred Nora.
 */


#ifndef __FEATURES_H
#define __FEATURES_H 1

/*
 Descrições
 (baseado na glibc)
 
 
   These are defined by the user (or the compiler)
   to specify the desired environment: 
   ----------------------------------------------------
   
   __STRICT_ANSI__	ISO Standard C.
   _GRAMADO_SOURCE  Gramado stuff. 
   _POSIX_SOURCE	IEEE Std 1003.1.
   ...
   
   
   These are defined by this file and are used by the
   header files to decide what to declare or define:
   
   __USE_GRAMADO	Define GRAMADO extensions.
   __USE_POSIX		Define IEEE Std 1003.1 things.
   __USE_POSIX2		Define IEEE Std 1003.2 things.
   __USE_BSD		Define 4.3BSD things.
   __USE_SVID		Define SVID things.
   __USE_MISC		Define things common to BSD and System V Unix.
   __USE_GNU		Define GNU extensions.
   __FAVOR_BSD		Favor 4.3BSD things in cases of conflict.
     ...
 
 */


/* Limpando tudo ...*/
/* Undefine everything, so we get a clean slate.  */
#undef	__USE_GRAMADO
#undef	__USE_POSIX
#undef	__USE_POSIX2
#undef	__USE_BSD
#undef	__USE_SVID    //System V Interface Definition
#undef	__USE_MISC
#undef	__USE_GNU
#undef	__FAVOR_BSD
//...



//
//  _GRAMADO_SOURCE
//

/* If nothing is defined, define _GRAMADO_SOURCE.  */
#if ( !defined(_GRAMADO_SOURCE)  &&  \
      !defined(_GNU_SOURCE)      &&  \ 
      !defined(__STRICT_ANSI__)  &&  \
      !defined(_POSIX_SOURCE)    &&  \
      !defined(_POSIX_C_SOURCE)  &&  \
      !defined(_BSD_SOURCE)      &&  \
      !defined(_SVID_SOURCE))
#define  _GRAMADO_SOURCE 1
#endif


/* Always use ANSI things.  */
#define  __USE_ANSI  1


/* 
 * If nothing (other than _GRAMADO_SOURCE) is defined,
 *   define _BSD_SOURCE and _SVID_SOURCE.  
 */

#if ( !defined(_GNU_SOURCE)     &&  \ 
      !defined(__STRICT_ANSI__) &&  \ 
      !defined(_POSIX_SOURCE)   &&  \
      !defined(_POSIX_C_SOURCE) &&  \
      !defined(_BSD_SOURCE)     &&  \
      !defined(_SVID_SOURCE) )
#define  _BSD_SOURCE   1
#define  _SVID_SOURCE  1
#endif


/* 
 * If none of the ANSI/POSIX macros are defined, 
 * use POSIX.1 and POSIX.2.  
 */

#if ( !defined(__STRICT_ANSI__)  &&  \
      !defined(_POSIX_SOURCE)    &&  \
      !defined(_POSIX_C_SOURCE))
#define  _POSIX_SOURCE    1
#define  _POSIX_C_SOURCE  2
#endif




// GRAMADO
#ifdef  _GRAMADO_SOURCE
    #define  __USE_GRAMADO  1
    #define  __USE_BSD      1
    #define  __FAVOR_BSD    1
#endif



// POSIX 1
#if	defined(_POSIX_SOURCE) || _POSIX_C_SOURCE >= 1
#define	__USE_POSIX	1
#endif


// POSIX 2
#if	defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 2
#define	__USE_POSIX2	1
#endif


// BSD and SVID
#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
#define	__USE_MISC	1
#endif


// BSD
#ifdef	_BSD_SOURCE
#define	__USE_BSD	1
#endif

// SVID - System V Interface Definition
#ifdef	_SVID_SOURCE
#define	__USE_SVID	1
#endif


// GNU
#ifdef	_GNU_SOURCE
#define	__USE_GNU	1
#endif


#undef	__GNU_LIBRARY__
#define	__GNU_LIBRARY__	1





/* 
// #todo
// In GCC version 2, (__extension__ EXPR) will not complain
// about GCC extensions used in EXPR under -ansi or -pedantic.  
#if	!defined(__GNUC__) || __GNUC__ < 2
#define	__extension__
#endif
*/



//#todo
/* This is here only because every header file already includes this one.  */
//#include <sys/cdefs.h>



#endif  /*features.h*/


