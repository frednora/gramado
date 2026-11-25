/*
 * File: limits.h 
 * 
 * Descrição:
 *     Descreve alguns limites na linguagem C padrão 32bit.
 *     @todo: Essa versão está imcompleta.
 */
 
 
#ifndef _LIMITS_H
#define _LIMITS_H 

//
// # char support #
// 
 
//Number of bits in a 'char'.  
#define CHAR_BIT  8

//Minimum and maximum values a 'signed char' can hold.  
#define SCHAR_MIN  (-128)
#define SCHAR_MAX  127    
 
//Maximum value an 'unsigned char' can hold.  (Minimum is 0.).  
#define UCHAR_MAX  255

//
// # short support #
// 

//Minimum and maximum values a 'signed short int' can hold.  
#define SHRT_MIN  (-32768)
#define SHRT_MAX  32767
 
//Maximum value an 'unsigned short int' can hold.  (Minimum is 0.).  
#define USHRT_MAX  65535

//
// # int support #
// 

#define INT_MIN  -2147483648
#define INT_MAX  2147483647

#define UINT_MIN    0
#define UINT_MAX    0xffffffff

// #bugbug: Wrong?
#define ULONG_MIN  0
#define ULONG_MAX  0xffffffffffffffff	/* max value for an unsigned long */

// #bugbug: Wrong?
#define LONG_MIN    (-9223372036854775807)	/* min value for a long */
#define LONG_MAX    9223372036854775807	/* max value for a long */


/*
obs:
In canonical input processing mode, terminal input is processed in lines terminated by 
newline ('\n'), EOF, or EOL characters. No input can be read until an entire line has 
been typed by the user, and the read function (see section Input and Output Primitives) 
returns at most a single line of input, no matter how many bytes are requested. 

In noncanonical input processing mode, characters are not grouped into lines, and 
ERASE and KILL processing is not performed. The granularity with which bytes are read 
in noncanonical input mode is controlled by the MIN and TIME settings. See section Noncanonical Input. 

Most programs use canonical input mode, because this gives the user a way to edit input line by line. 
The usual reason to use noncanonical mode is when the program accepts single-character commands
or provides its own editing facilities. 
*/



/* Minimum sizes required by the POSIX P1003.1 standard (Table 2-3). */

#define _POSIX_ARG_MAX    4096	/* exec() may have 4K worth of args */
#define _POSIX_CHILD_MAX     6	/* a process may have 6 children */
#define _POSIX_LINK_MAX      8	/* a file may have 8 links */
#define _POSIX_MAX_CANON   255	/* size of the canonical input queue */
#define _POSIX_MAX_INPUT   255	/* you can type 255 chars ahead */
#define _POSIX_NAME_MAX DIRSIZ	/* max. file name length */
#define _POSIX_NGROUPS_MAX   0	/* supplementary group IDs are optional */
#define _POSIX_OPEN_MAX     16	/* a process may have 16 files open */
#define _POSIX_PATH_MAX    255	/* a pathname may contain 255 chars */
#define _POSIX_PIPE_BUF    512	/* pipes writes of 512 bytes must be atomic */
#define _POSIX_STREAM_MAX    8	/* at least 8 FILEs can be open at once */
#define _POSIX_TZNAME_MAX    3	/* time zone names can be at least 3 chars */
#define _POSIX_SSIZE_MAX 32767	/* read() must support 32767 byte reads */

/* The number of symbolic links that can be
   traversed in the resolution of a pathname
   in the absence of a loop. */
#define _POSIX_SYMLOOP_MAX   8	

/*bsd style*/
/*
#define	_POSIX2_BC_BASE_MAX		99
#define	_POSIX2_BC_DIM_MAX		2048
#define	_POSIX2_BC_SCALE_MAX		99
#define	_POSIX2_BC_STRING_MAX		1000
#define	_POSIX2_COLL_WEIGHTS_MAX	2
#define	_POSIX2_EQUIV_CLASS_MAX		2
#define	_POSIX2_EXPR_NEST_MAX		32
#define	_POSIX2_LINE_MAX		2048
#define	_POSIX2_RE_DUP_MAX		255
*/

/*bsd style*/
/*
#define PASS_MAX	128
#define NL_ARGMAX	9
#define NL_LANGMAX	14
#define NL_MSGMAX	32767
#define NL_NMAX		1
#define NL_SETMAX	255
#define NL_TEXTMAX	255
*/


#define HOST_NAME_MAX 64
//#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX

//#define PTHREAD_STACK_MIN 65536



#define PATH_MAX _POSIX_PATH_MAX

//
// End.
//

#endif /* _LIMITS_H */


