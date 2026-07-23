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
// #include <string.h>   // for memset if needed


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

// Parameters validation:
    if (sockfd < 0)
    {
        errno = EBADF;
        goto fail;
    }
    if ((void*) buf == NULL)
    {
        errno = EFAULT;          // POSIX says EFAULT for invalid buffer
        goto fail;
    }
    if (len == 0) {
        return 0;               //  Common and useful convention
    }

// === Real work happens here ===
// In a real libc you would call the kernel syscall here
// When a valid destination is provided, it needs to use the syscall.

// Case 1: Destination address provided → must use sendto syscall
// This is required for unconnected datagram sockets (UDP).
    if (dest_addr != NULL)
    {
        // #todo: Replace with your actual syscall invocation.
        // sc80(...);

        // If you don't have the syscall yet, you have two options:
        // 1. Return -1 with errno = ENOSYS  ("Function not implemented")
        // 2. Fall through to write() below (only works if socket is connected)
    }

// Case 2: No destination address → behave like send() / write()
// This is correct for connected sockets (TCP, connected UDP, etc.)

    return (ssize_t) write( sockfd, (const void *) buf, len );

fail:
    return (ssize_t) -1;
}

// sendmsg:
// Most powerful of the send family (send, sendto, sendmsg).
// What sendmsg can do that sendto cannot:
// 1. Send multiple buffers at once (scatter/gather via msg_iov)
// 2. Send ancillary/control data (file descriptors, timestamps, pktinfo...)
// 3. Very flexible destination address handling

ssize_t sendmsg (int sockfd, const struct msghdr *msg, int flags)
{
    if (sockfd < 0)
    {
        errno = EBADF;
        goto fail;
    }
    if (msg == NULL)
    {
        errno = EFAULT;
        goto fail;
    }

    if (msg->msg_iov == NULL || msg->msg_iovlen == 0)
    {
        errno = EINVAL;          // No buffers to send
        goto fail;
    }

    // Optional: Basic validation
    if (msg->msg_name != NULL && msg->msg_namelen == 0)
    {
        errno = EINVAL;
        goto fail;
    }

// ================================================
//               REAL IMPLEMENTATION
// ================================================

    printf ("sendmsg: #todo\n");

// In a real libc, sendmsg is usually implemented with a single syscall:
    // sc80(...);

/*
    // Call the kernel
    ssize_t ret = (ssize_t) sc80( ??,                 // <<<< Choose a free syscall number
                                 (unsigned long)sockfd,
                                 (unsigned long)msg,
                                 (unsigned long)flags );

    if (ret < 0)
    {
        errno = (int)(-ret);
        return -1;
    }

    return ret;
*/

    // Temporary stub
    errno = ENOSYS;        // Function not implemented yet
    return (ssize_t)-1;

fail:
    return (ssize_t) -1;
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
        goto fail;
    }
    if ((void*) buf == NULL)
    {
        errno = EFAULT;          // POSIX says EFAULT for invalid buffer
        goto fail;
    }
    if (len == 0) {
        return 0;               //  Common and useful convention
    }

   return (ssize_t) read( sockfd, (const void *) buf, len );

    // #todo: Usar esse.
    //return (ssize_t) recvfrom ( (int) sockfd, 
        //(void *) buf, (size_t) len, (int) flags,
        //(struct sockaddr *) src_addr, (socklen_t *) addrlen );

fail:
    return (ssize_t) (-1);
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
    if (sockfd<0){
        errno = EBADF;
        goto fail;
    }
    if ((void*) buf == NULL)
    {
        errno = EFAULT;          // POSIX says EFAULT for invalid buffer
        goto fail;
    }
    if (len == 0) {
        return 0;               //  Common and useful convention
    }

    return (ssize_t) read( sockfd, (const void *) buf, len );
fail:
    return (ssize_t) (-1);
}


ssize_t recvmsg (int sockfd, struct msghdr *msg, int flags)
{
    if (sockfd < 0)
    {
        errno = EBADF;
        goto fail;
    }
    if (msg == NULL)
    {
        errno = EFAULT;
        goto fail;
    }

    printf ("recvmsg: [TODO]\n");


/*
    ssize_t ret = (ssize_t) sc80(??,                 // <<<< Choose a free syscall number
                                 (unsigned long)sockfd,
                                 (unsigned long)msg,
                                 (unsigned long)flags);

    if (ret < 0)
    {
        errno = (int)(-ret);
        return -1;
    }

    return ret;
*/

    return -1;

fail:
    return (ssize_t) (-1);
}


int 
getpeername ( 
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen )
{
    if (sockfd < 0)
    {
        errno = EBADF;
        goto fail;
    }
    if (addr == NULL || addrlen == NULL)
    {
        errno = EINVAL;
        goto fail;
    }

    printf ("getpeername: [TODO]\n");


/*
    int ret = (int) sc80(??,                         // <<<< Choose a free number
                         (unsigned long)sockfd,
                         (unsigned long)addr,
                         (unsigned long)addrlen);

    if (ret < 0)
    {
        errno = (-ret);
        return -1;
    }

    return 0;
*/

    return -1;

fail:
    return (ssize_t) (-1);
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





