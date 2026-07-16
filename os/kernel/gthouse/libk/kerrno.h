// kerro.h
// List of error numbers.
// Created by Fred Nora.

// See: 
// https://man7.org/linux/man-pages/man3/errno.3.html
// https://www.gnu.org/software/libc/manual/html_node/Error-Codes.html
// ...


/*
Grouping Related Errors for Better Organization
Instead of a flat list, grouping similar errors can make it easier to manage:
File System Errors (EIO, EBADF, ENOENT, ENOTDIR, EISDIR, EROFS, EEXIST, ENOSPC, etc.)
Process and Resource Management Errors (EAGAIN, ENOMEM, EFAULT, EINTR, ESRCH, etc.)
Network and Socket Errors (ENOTSOCK, ECONNRESET, ENETUNREACH, ETIMEDOUT, etc.)
Permission Errors (EPERM, EACCES, etc.)
*/

#ifndef __LIBK_ERRNO_H
#define __LIBK_ERRNO_H    1

// List
#define EPERM      1	/* Operation not permitted */
#define ENOFILE    2	/* No such file or directory */
#define ENOENT     2
#define ESRCH      3	/* No such process */
#define EINTR      4	/* Interrupted function call */
#define EIO        5	/* Input/output error */
#define ENXIO      6	/* No such device or address */
#define E2BIG      7	/* Arg list too long */
#define ENOEXEC    8	/* Exec format error */
#define EBADF      9	/* Bad file descriptor */
#define ECHILD     10	/* No child processes */
#define EAGAIN     11	/* Resource temporarily unavailable */
#define ENOMEM     12	/* Not enough space */
#define EACCES     13	/* Permission denied */
#define EFAULT     14	/* Bad address */
/* 15 - Unknown Error */
// 15 - (Unused in POSIX)
// 15 - (Unused in Linux's standard error definitions)
#define EBUSY      16  /* strerror reports "Resource device" */
#define EEXIST     17  /* File exists */
#define EXDEV      18  /* Improper link (cross-device link?) */
#define ENODEV     19  /* No such device */
#define ENOTDIR    20  /* Not a directory */
#define EISDIR     21  /* Is a directory */
#define EINVAL     22  /* Invalid argument */
#define ENFILE     23  /* Too many open files in system */
#define EMFILE     24  /* Too many open files */
#define ENOTTY     25  /* Inappropriate I/O control operation */
/* 26 - Unknown Error */
// 26 - ETXTBSY (Text file busy)
#define EFBIG     27	/* File too large */
#define ENOSPC    28	/* No space left on device */
#define ESPIPE    29	/* Invalid seek (seek on a pipe?) */
#define EROFS     30	/* Read-only file system */
#define EMLINK    31	/* Too many links */
#define EPIPE     32	/* Broken pipe */
#define EDOM      33	/* Domain error (math functions) */
#define ERANGE    34	/* Result too large (possibly too small) */
/* 35 - Unknown Error */
// 35 - (Unused in most modern POSIX systems; sometimes EAGAIN in older BSD variants)
// 35 - EDEADLK (Resource deadlock avoided) (Linux)
#define EDEADLOCK  36	/* Resource deadlock avoided (non-Cyg) */
#define EDEADLK    36
/* 37 - Unknown Error */
// 37 - (Unused in POSIX)
// 37 - (Unused in Linux's standard error definitions)
#define ENAMETOOLONG  38	/* Filename too long (91 in Cyg?) */
#define ENOLCK     39	/* No locks available (46 in Cyg?) */
#define ENOSYS     40	/* Function not implemented (88 in Cyg?) */
#define ENOTEMPTY  41	/* Directory not empty (90 in Cyg?) */
#define EILSEQ     42	/* Illegal byte sequence */

// The server can't accept the connection.
//   Connection refused (POSIX.1-2001).
#define ECONNREFUSED_POSIX  61

#define ENOTSOCK        88  /* Socket operation on non-socket */
#define EDESTADDRREQ    89  /* Destination address required */
#define EMSGSIZE        90  /* Message too long */
#define EPROTOTYPE      91  /* Protocol wrong type for socket */
#define ENOPROTOOPT     92  /* Protocol not available */
#define EPROTONOSUPPORT 93  /* Protocol not supported */
#define ESOCKTNOSUPPORT 94  /* Socket type not supported */
#define EOPNOTSUPP      95  /* Operation not supported */
#define EPFNOSUPPORT    96  /* Protocol family not supported */
#define EAFNOSUPPORT    97  /* Address family not supported by protocol */
#define EADDRINUSE      98  /* Address already in use */
#define EADDRNOTAVAIL   99  /* Cannot assign requested address */
#define ENETDOWN       100  /* Network is down */
#define ENETUNREACH    101  /* Network is unreachable */
#define ENETRESET      102  /* Network dropped connection */
#define ECONNABORTED   103  /* Connection aborted */
#define ECONNRESET     104  /* Connection reset by peer */
#define ENOBUFS        105  /* No buffer space available */
#define EISCONN        106  /* Socket is already connected */
#define ENOTCONN       107  /* Socket is not connected */
#define ESHUTDOWN      108  /* Cannot send after shutdown */
#define ETOOMANYREFS   109  /* Too many references */
#define ETIMEDOUT      110  /* Connection timed out */
#define ECONNREFUSED   111  /* Connection refused */
#define EHOSTDOWN      112  /* Host is down */
#define EHOSTUNREACH   113  /* No route to host */

// extern int errno;

#endif	/* Not __ERRNO_H */    



