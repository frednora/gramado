/*-
 * Copyright (c) 1982, 1986, 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	7.17 (Berkeley) 5/6/91
 */



#ifndef  _TYPES_H_
#define  _TYPES_H_


//#test
typedef int __int32_t;
typedef __int32_t __blkcnt_t;
typedef __int32_t __blksize_t;

/* Standard system types */
typedef __blkcnt_t  blkcnt_t;	/* blocks allocated for file */
typedef __blksize_t blksize_t;	/* optimal blocksize for I/O */

typedef unsigned int id_t;


typedef unsigned char     u_char;
typedef unsigned short    u_short;
typedef unsigned int      u_int;
typedef unsigned long     u_long;

typedef unsigned char   unchar;  /* Sys V compatibility */
typedef unsigned short  ushort;  /* Sys V compatibility */
typedef unsigned int    uint;    /* Sys V compatibility */
typedef unsigned long   ulong;   /* Sys V compatibility */

typedef  char    *caddr_t;    /* core address */
typedef  long    daddr_t;     /* disk address */
typedef  short   dev_t;       /* device number */
typedef  u_long  ino_t;       /* inode number */
//typedef	long	off_t;    /* file offset (should be a quad) */
typedef  u_short  nlink_t;    /* link count */
typedef  long     swblk_t;    /* swap offset */
typedef  long     segsz_t;    /* segment size */


#ifdef _BSD_SOURCE
typedef  u_short    uid_t;    /* user id */
typedef  u_short    gid_t;    /* group id */
typedef  short      pid_t;    /* process id */
#endif


// old ... 
// #bugbug: Como no bsd encontramos valores de 32bit
// vamos mudar isso.
//typedef  u_short  mode_t;   /* permissions */
typedef u_long mode_t;

typedef  u_long   fixpt_t;  /* fixed point number */


#ifndef _POSIX_SOURCE
typedef  struct  _uquad  { u_long val[2]; } u_quad;
typedef  struct  _quad  {   long val[2]; } quad;
typedef  long  *qaddr_t;  /* should be typedef quad * qaddr_t; */

#define  major(x)      ((int)(((u_int)(x) >> 8)&0xff))  /* major number */
#define  minor(x)      ((int)((x)&0xff))                /* minor number */
#define  makedev(x,y)  ((dev_t)(((x)<<8) | (y)))        /* create dev_t */
#endif


/*
#include <machine/ansi.h>
#if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE)
#include <machine/types.h>
#endif
*/

// file offset or size.

typedef  int  __off_t;
typedef  int  __darwin_off_t;
typedef  int  __gramado_off_t;
//typedef	__int64_t	__off_t;
//...



//typedef __WINT_TYPE__ wint_t;


/*
#define __socklen_t_defined
#define __socklen_t uint32_t
typedef __socklen_t socklen_t;
*/


#ifndef  _OFF_T_DEFINED_
#define  _OFF_T_DEFINED_
typedef  __off_t  off_t;
#endif


#ifdef  _CLOCK_T_
typedef  _CLOCK_T_  clock_t;
#undef  _CLOCK_T_
#endif

#ifdef  _SIZE_T_
typedef  _SIZE_T_  size_t;
#undef  _SIZE_T_
#endif

#ifdef  _TIME_T_
typedef  _TIME_T_  time_t;
#undef  _TIME_T_
#endif


//Conflito. 
//#ifndef _SIZE_T
//#define _SIZE_T
//typedef unsigned int size_t;
//#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef int  ssize_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef long  time_t;  /* time in sec since 1 Jan 1970 0000 GMT */
#endif

#ifndef _CLOCK_T
#define _CLOCK_T
typedef long  clock_t;  /* unit for system accounting */
#endif


/* 
 Process IDentifier 
 posix.  
 */

#ifndef __UID_T
#define __UID_T 
typedef int  uid_t;
#endif 

#ifndef __PID_T
#define __PID_T 
typedef int  pid_t;
#endif 


#ifndef __GID_T
#define __GID_T 
typedef int  gid_t;
#endif


#ifndef _POSIX_SOURCE
#define NBBY    8  /* number of bits in a byte */

/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */
#ifndef FD_SETSIZE
#define FD_SETSIZE    256
#endif

typedef  long  fd_mask;
#define  NFDBITS  (sizeof(fd_mask) * NBBY)	/* bits per mask */

#ifndef howmany
#define  howmany(x, y)  (((x)+((y)-1))/(y))
#endif

typedef  struct fd_set {
    fd_mask  fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;


#define  FD_SET(n, p)    ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define  FD_CLR(n, p)    ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define  FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define  FD_ZERO(p)      bzero((char *)(p), sizeof(*(p)))


/*
 * Forward structure declarations for function prototypes.
 * We include the common structures that cross subsystem boundaries here;
 * others are mostly used in the same place that the structure is defined.
 */

/*
#if defined(__STDC__) && defined(KERNEL)
struct	proc;
struct	pgrp;
struct	ucred;
struct	rusage;
struct	file;
struct	buf;
struct	tty;
struct	uio;
#endif
*/




//from: serenity os.
/*
struct stat {
    dev_t st_dev;         // ID of device containing file 
    ino_t st_ino;         // inode number 
    mode_t st_mode;       // protection 
    nlink_t st_nlink;     // number of hard links 
    uid_t st_uid;         // user ID of owner 
    gid_t st_gid;         // group ID of owner 
    dev_t st_rdev;        // device ID (if special file) 
    off_t st_size;        // total size, in bytes 
    blksize_t st_blksize; // blocksize for file system I/O 
    blkcnt_t st_blocks;   // number of 512B blocks allocated 
    time_t st_atime;      // time of last access 
    time_t st_mtime;      // time of last modification 
    time_t st_ctime;      //time of last status change 
};
*/ 

/*
struct utimbuf {
    time_t actime;
    time_t modtime;
};
*/



//
// ptrheads
//

/*
typedef int    pthread_t;
typedef int    pthread_key_t;
typedef void*  pthread_once_t;
*/

/*
typedef struct __pthread_mutex_t {
    uint32_t lock;
    pthread_t owner;
    int level;
    int type;
} pthread_mutex_t;
*/

/*
typedef void* pthread_attr_t;
typedef struct __pthread_mutexattr_t {
    int type;
} pthread_mutexattr_t;
*/

/*
typedef struct __pthread_cond_t {
    int32_t value;
    uint32_t previous;
    int clockid; // clockid_t
} pthread_cond_t;
*/

/*
typedef void* pthread_rwlock_t;
typedef void* pthread_rwlockatrr_t;
typedef void* pthread_spinlock_t;
typedef struct __pthread_condattr_t {
    int clockid; // clockid_t
} pthread_condattr_t;
*/


#endif /* !_POSIX_SOURCE */
#endif /* !_TYPES_H_ */



