
// sys/file.h


#ifndef __SYS_FILE_H
#define __SYS_FILE_H    1

// LOCK_SH
// Place a shared lock.  More than one process may hold
// a shared lock for a given file at a given time.
// LOCK_EX
// Place an exclusive lock.  Only one process may hold
// an exclusive lock for a given file at a given time.
// LOCK_UN
// Remove an existing lock held by this process.

#define LOCK_SH  0
#define LOCK_EX  1
#define LOCK_UN  2
//#define LOCK_NB  (1 << 2)

// flock - apply or remove an advisory lock on an open file.
int flock(int fd, int operation); 

#endif  


//
// End.
//



