/*
 * File: select.h
 * 
 * History:
 *     2020 - Created by Fred Nora.
 */



// See: 
// https://man7.org/linux/man-pages/man2/select.2.html


#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H 1


#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif


// #todo
// Create all these functions.
/*
int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);

       void FD_CLR(int fd, fd_set *set);
       int  FD_ISSET(int fd, fd_set *set);
       void FD_SET(int fd, fd_set *set);
       void FD_ZERO(fd_set *set);

       int pselect(int nfds, fd_set *readfds, fd_set *writefds,
                   fd_set *exceptfds, const struct timespec *timeout,
                   const sigset_t *sigmask);
*/




#endif /* _SYS_SELECT_H */

