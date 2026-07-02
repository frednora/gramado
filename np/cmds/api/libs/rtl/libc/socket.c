// socket.c
// See: 
// http://man7.org/linux/man-pages/man2/socket.2.html
// Created by Fred Nora.

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <rtl/gramado.h> 


static int __socket_pipe( int pipefd[2] );


// -----------------


// socket:
// Create an endpoint for communication.
// See: http://man7.org/linux/man-pages/man2/socket.2.html
// OUT: fd.
int socket( int domain, int type, int protocol )
{
    int value = -1;

    value = 
        (int) sc80 ( 
                  7000, 
                  (unsigned long) domain, 
                  (unsigned long) type, 
                  (unsigned long) protocol );
    if (value<0)
    {
        errno = (-value);
        return (int) -1;
    }

    return (int) value;
}

// Local worker.
static int __socket_pipe( int pipefd[2] )
{
    return (int) sc80 ( 
                     247, 
                     (unsigned long) pipefd, 
                     (unsigned long) pipefd, 
                     (unsigned long) pipefd );
}


int socketpair(int domain, int type, int protocol, int sv[2])
{
    int fd = -1;
    int pipefd[2];

// #bugbug
// Only two types of family?

    if ( domain == AF_UNSPEC || domain == AF_UNIX )
    {
        if ( protocol != 0 ){
            return (int) (-1);
        }

        //if ( type != SOCK_STREAM )
            //return (int) (-1);

        // Podemos colocar sv diretamente.
        fd = (int) __socket_pipe (pipefd);

        if ( fd  == -1 ) { 
            printf ("socketpair: fail\n");
            return (int) (-1);
        }else{
            sv[0] = pipefd[1];
            sv[1] = pipefd[1];
            return 0;
        };
    }

    return (int) (-1);
}


// #todo
// not tested.
/*
int gramado_socketpair (int fd[2]);
int gramado_socketpair (int fd[2])
{
    return (int) socketpair ( AF_UNIX, SOCK_STREAM, 0, fd );
}
*/


int 
bind ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen )
{
    int value = -1;

    if (sockfd<0)
    {
        errno=EBADF;
        return (int) -1;
    }

// #todo: 
// Check addr and addrlen.

    value = 
        (int) sc80 ( 
                  7003, 
                  (unsigned long) sockfd, 
                  (unsigned long) addr, 
                  (unsigned long) addrlen );

    if (value<0)
    {
        errno = (-value);
        printf ("bind: [FAIL] Couldn't bind\n");
        return (int) -1;
    }

    return (int) value;
}

// IN:
// + The socket descriptor.
// + The maximum length for the queue of pending connections.
int listen(int sockfd, int backlog)
{
    int value = -1;

// fd limits
    if (sockfd<0){
        errno = EBADF;
        goto fail;
    }
// backlog limits
    if (backlog <= 0 || backlog > SOMAXCONN){
        errno = EBADF;
        goto fail;
    }

    value = 
        (int) sc80 ( 
                  7004, 
                  (unsigned long) sockfd, 
                  (unsigned long) backlog, 
                  (unsigned long) 0 );

// Fail.
    if (value<0)
    {
        errno = (-value);
        goto fail;
    }

// OK
    if (value == 0)
    {
       errno = 0;
       return 0;
    }

// Positive values.
    errno = 0;  //?
    return (int) value;
fail:
    return (int) (-1);
}


// #todo
// See: https://linux.die.net/man/2/accept4
int 
accept4 (
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen, 
    int flags)
{
    errno = -1;
    printf ("accept4: [TODO] Not implemented yet\n");
    return -1;
}


// Alternative. Not tested.
int accept2 (int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int value = -1;
    
    if(sockfd<0)
    {
        errno = EBADF;
        return (int) (-1);
    }

    value = 
        (int) sc80 ( 
                  7010, 
                  (unsigned long) sockfd, 
                  (unsigned long) addr, 
                  (unsigned long) addrlen );

    if(value<0)
    {
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}


// OUT: fd.
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int value = -1;

    if (sockfd<0){
        errno = EBADF;
        return (int) (-1);
    }
    if ( (void*) addr == NULL ){
        errno = EINVAL;
        return -1;
    }

    value = 
        (int) sc80 ( 
                  7002, 
                  (unsigned long) sockfd, 
                  (unsigned long) addr, 
                  (unsigned long) addrlen );

    if (value<0){
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}


int 
connect ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen )
{
    int value = -1;

    if(sockfd<0){
        errno = EBADF;
        return (int) (-1);
    }

    value = 
        (int) sc80 ( 
                  7001, 
                  (unsigned long) sockfd, 
                  (unsigned long) addr, 
                  (unsigned long) addrlen );

    if (value<0)
    {
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}


// shutdown:
// shut down part of a full-duplex connection.
// See:
// https://linux.die.net/man/3/shutdown

int shutdown(int sockfd, int how)
{
    int value = -1;

    if (sockfd<0)
    {
        errno = EBADF;
        return (int) (-1);
    }

    value = (int) sc80 ( 
              7009, 
              (unsigned long) sockfd, 
              (unsigned long) how, 
              (unsigned long) how );

    if (value<0)
    {
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}


/*
void FD_CLR(int fd, fd_set *set);
void FD_CLR(int fd, fd_set *set)
{}
*/

/*
int  FD_ISSET(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set)
{ return -1; }
*/

/*
void FD_SET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set)
{}
*/

/*
void FD_ZERO(fd_set *set);
void FD_ZERO(fd_set *set)
{}
*/  



/*
select() and pselect() allow a program to monitor multiple file
       descriptors, waiting until one or more of the file descriptors become
       "ready" for some class of I/O operation (e.g., input possible).
*/
//see: sys/select.h
/*
int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);
int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout)
{ return -1; }
*/  
  
/*       
int pselect(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, const struct timespec *timeout,
            const sigset_t *sigmask);                       
int pselect(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, const struct timespec *timeout,
            const sigset_t *sigmask)
{ return -1; }
*/             


// send:

ssize_t 
send ( 
    int sockfd, 
    const void *buf, 
    size_t len, 
    int flags )
{
    if (sockfd < 0)
    {
        errno = EBADF;
        return (ssize_t) -1;
    }

    //#todo: Usar esse.
    //return (ssize_t) sendto ( (int) sockfd, 
        //(const void *) buf, (size_t) len, (int) flags,
        //(const struct sockaddr *) dest_addr, (socklen_t) addrlen );

    return (ssize_t) write( sockfd, (const void *) buf, len );
}


// sendto:
// 4.4BSD, SVr4, POSIX.1-2001.  
// These interfaces first appeared in 4.2BSD.

ssize_t 
sendto ( 
    int sockfd, 
    const void *buf, 
    size_t len, 
    int flags,
    const struct sockaddr *dest_addr, 
    socklen_t addrlen )
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (ssize_t) -1;
    }

    return (ssize_t) write ( sockfd, (const void *) buf, len );
}


ssize_t sendmsg (int sockfd, const struct msghdr *msg, int flags)
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (ssize_t) -1;
    }

    debug_print ("sendmsg: [TODO]\n");
    return -1;
}


// recv:
ssize_t 
recv ( 
    int sockfd, 
    void *buf, 
    size_t len, 
    int flags )
{

    if (sockfd<0){
        errno = EBADF;
        return (ssize_t) (-1);
    }

   return (ssize_t) read( sockfd, (const void *) buf, len );

    // #todo: Usar esse.
    //return (ssize_t) recvfrom ( (int) sockfd, 
        //(void *) buf, (size_t) len, (int) flags,
        //(struct sockaddr *) src_addr, (socklen_t *) addrlen );
}


ssize_t 
recvfrom ( 
    int sockfd, 
    void *buf, 
    size_t len, 
    int flags,
    struct sockaddr *src_addr, 
    socklen_t *addrlen )
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (ssize_t) -1;
    }

    return (ssize_t) read( sockfd, (const void *) buf, len );
}


ssize_t recvmsg (int sockfd, struct msghdr *msg, int flags)
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (ssize_t) -1;
    }

    debug_print ("recvmsg: [TODO]\n");
    return -1;
}


int 
getpeername ( 
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen )
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    debug_print ("getpeername: [TODO]\n");
    return -1;
}

// getsockname:
// POSIX.1-2001, POSIX.1-2008, SVr4, 4.4BSD 
// First appeared in 4.2BSD.
int 
getsockname ( 
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen )
{
    int value = -1;

    if (sockfd<0)
    {
        errno = EBADF;
        return (int) (-1);
    }

    value = 
        (int) sc80 ( 
                  7007, 
                  (unsigned long) sockfd, 
                  (unsigned long) addr, 
                  (unsigned long) addrlen );

    if (value<0)
    {
        printf("getsockname: fail\n");
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}


// ===================================


int 
getsockopt(
    int sockfd, 
    int level, 
    int optname, 
    void *optval, 
    socklen_t *optlen)
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    return -1; 
}


int 
setsockopt (
    int sockfd, 
    int level, 
    int optname, 
    const void *optval, 
    socklen_t optlen )
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    return -1; 
}

int sendfd(int sockfd, int fd)
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (int) -1;
    }
 
    return -1; 
}

int recvfd(int sockfd)
{
    if(sockfd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    return -1; 
}


//
// End
//





