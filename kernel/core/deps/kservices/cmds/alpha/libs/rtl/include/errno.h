// errno.h

//#test
//Isso é um test ainda.

/* 
 * errno.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Error numbers and access to error reporting.
 *
 */

// See: 
// https://man7.org/linux/man-pages/man3/errno.3.html


#ifndef _ERRNO_H_
#define  _ERRNO_H_


/* All the headers include this file. */
#include <_mingw.h>

/*
 * Error numbers.
 * TODO: Can't be sure of some of these assignments, I guessed from the
 * names given by strerror and the defines in the Cygnus errno.h. A lot
 * of the names from the Cygnus errno.h are not represented, and a few
 * of the descriptions returned by strerror do not obviously match
 * their error naming.
 */
#define EPERM		1	/* Operation not permitted */
#define	ENOFILE		2	/* No such file or directory */
#define	ENOENT		2
#define	ESRCH		3	/* No such process */
#define	EINTR		4	/* Interrupted function call */
#define	EIO		5	/* Input/output error */
#define	ENXIO		6	/* No such device or address */
#define	E2BIG		7	/* Arg list too long */
#define	ENOEXEC		8	/* Exec format error */
#define	EBADF		9	/* Bad file descriptor */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Resource temporarily unavailable */
#define	ENOMEM		12	/* Not enough space */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
/* 15 - Unknown Error */
#define	EBUSY		16	/* strerror reports "Resource device" */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Improper link (cross-device link?) */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* Too many open files in system */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Inappropriate I/O control operation */
/* 26 - Unknown Error */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Invalid seek (seek on a pipe?) */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Domain error (math functions) */
#define	ERANGE		34	/* Result too large (possibly too small) */
/* 35 - Unknown Error */
#define	EDEADLOCK	36	/* Resource deadlock avoided (non-Cyg) */
#define	EDEADLK		36
/* 37 - Unknown Error */
#define	ENAMETOOLONG	38	/* Filename too long (91 in Cyg?) */
#define	ENOLCK		39	/* No locks available (46 in Cyg?) */
#define	ENOSYS		40	/* Function not implemented (88 in Cyg?) */
#define	ENOTEMPTY	41	/* Directory not empty (90 in Cyg?) */
#define	EILSEQ		42	/* Illegal byte sequence */


// The server can't accept the connection.
//   Connection refused (POSIX.1-2001).
#define ECONNREFUSED  61


// See: unistd.c
extern int errno;
extern int sys_nerr;
extern const char *sys_errlist[];

#endif	/* Not _ERRNO_H_ */
