// kfcntl.h
// Created by Fred Nora.

#ifndef ____FCNTL_H
#define ____FCNTL_H    1

/* 
 * These values are used for cmd in fcntl(). 
 * POSIX Table 6-1. 
 */
#define F_DUPFD     0	/* duplicate file descriptor */
#define F_GETFD     1	/* get file descriptor flags */
#define F_SETFD     2	/* set file descriptor flags */
#define F_GETFL     3	/* get file status flags */
#define F_SETFL     4	/* set file status flags */
#define F_GETLK     5	/* get record locking information */
#define F_SETLK     6	/* set record locking information */
#define F_SETLKW    7	/* set record locking info; wait if blocked */
#define F_FREESP    8	/* free a section of a regular file */
// ...

/* File descriptor flags used for fcntl().  POSIX Table 6-2. */
#define FD_CLOEXEC         1	/* close on exec flag for third arg of fcntl */

/* L_type values for record locking with fcntl().  POSIX Table 6-3. */
#define F_RDLCK            1	/* shared or read lock */
#define F_WRLCK            2	/* exclusive or write lock */
#define F_UNLCK            3	/* unlock */

//
// O_ flags.
// Flags passed in the function open() and fcntl().
// It came from ring3.
// For the kernel size, see. f->_flags in kstdio.h.

/* File access modes for open() and fcntl(). */
#define O_RDONLY    0x0000  /* open(name, O_RDONLY) opens read only */
#define O_WRONLY    0x0001  /* open(name, O_WRONLY) opens write only */
#define O_RDWR      0x0002  /* open(name, O_RDWR) opens read/write */
/* Mask for use with file access modes.  POSIX Table 6-7. */
#define O_ACCMODE   0x0003  /* mask for file access modes */
#define O_NONBLOCK  0x0004  /* no delay */
#define O_APPEND    0x0008  /* set append mode */
#define O_SHLOCK    0x0010  /* open with shared file lock */
#define O_EXLOCK    0x0020  /* open with exclusive file lock */
#define O_ASYNC     0x0040  /* signal pgrp when data ready */
#define O_SYNC      0x0080  /* synchronous writes */

#define O_NOCTTY    0x0100  /* do not assign a controlling terminal */
#define O_CREAT     0x0200  /* create if nonexistant */
#define O_TRUNC     0x0400  /* truncate to zero length */
#define O_EXCL      0x0800  /* error if already exists */

#define O_REOPEN     0x1000  /* automatically re-open device after driver restart */
#define O_DIRECTORY  0x2000  /* fail if not a directory */
// ...

/* Struct used for locking.  POSIX Table 6-8. */

struct flock 
{
    short l_type;    // type: F_RDLCK, F_WRLCK, or F_UNLCK 
    short l_whence;  // flag for starting offset 
    off_t l_start;   // relative offset in bytes 
    off_t l_len;     // size; if 0, then until EOF 
    pid_t l_pid;     // process id of the locks' owner 
};

#endif  //____FCNTL_H

//
// End
//
