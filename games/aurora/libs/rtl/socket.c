
// socket.c
// See: 
// http://man7.org/linux/man-pages/man2/socket.2.html

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
        (int) gramado_system_call ( 
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
    return (int) gramado_system_call ( 
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
        (int) gramado_system_call ( 
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
        (int) gramado_system_call ( 
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
        (int) gramado_system_call ( 
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
        (int) gramado_system_call ( 
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
        (int) gramado_system_call ( 
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

    value = (int) gramado_system_call ( 
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
    if(sockfd<0)
    {
        errno = EBADF;
        return (ssize_t) -1;
    }

    //#todo: Usar esse.
    //return (ssize_t) sendto ( (int) sockfd, 
        //(const void *) buf, (size_t) len, (int) flags,
        //(const struct sockaddr *) dest_addr, (socklen_t) addrlen );

    return (ssize_t) write ( sockfd, (const void *) buf, len );
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

    if(sockfd<0)
    {
        errno = EBADF;
        return (int) (-1);
    }

    value = 
        (int) gramado_system_call ( 
                  7007, 
                  (unsigned long) sockfd, 
                  (unsigned long) addr, 
                  (unsigned long) addrlen );

    if (value<0)
    {
        printf ("getsockname: fail\n");
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}


// ===================================

// see:
// https://man7.org/linux/man-pages/man3/inet_pton.3.html
int inet_pton(int af, const char *src, void *dst)
{
    printf ("inet_pton: [TODO]\n");
    return -1;
}

int inet_aton (const char *cp, struct in_addr *inp)
{
    printf ("inet_aton: [TODO]\n");
    return -1;
}

// see:
// https://pubs.opengroup.org/onlinepubs/009695399/functions/inet_addr.html
in_addr_t inet_addr(const char *cp)
{
/*
    in_addr_t tmp {};
    int rc = inet_pton(AF_INET, str, &tmp);
    if (rc <= 0)
        return INADDR_NONE;
    return tmp;
*/
    printf ("printf: [TODO]\n");
    return (in_addr_t) 0;
}

in_addr_t inet_network(const char *cp)
{
    printf ("inet_network: [TODO]\n");
    return (in_addr_t) 0;
}


// inet_ntoa:
// Convert network-format internet address
// to base 256 d.d.d.d representation.

char *inet_ntoa (struct in_addr in) 
{
/*
    static char buffer[32];
    inet_ntop(AF_INET, &in.s_addr, buffer, sizeof(buffer));
    return buffer;
 */

    printf ("inet_ntoa: [TODO]\n");
    return NULL;
}

// inet_makeaddr:
// Formulate an Internet address from network + host.  Used in
// building addresses stored in the ifnet structure.

struct in_addr inet_makeaddr ( in_addr_t net, in_addr_t host )
{
    struct in_addr ret;
    in_addr_t addr;

    printf("inet_makeaddr: [TODO] Not implemented!\n");

	/*
	if (net < 128)
		addr = (net << IN_CLASSA_NSHIFT) | (host & IN_CLASSA_HOST);
	else if (net < 65536)
		addr = (net << IN_CLASSB_NSHIFT) | (host & IN_CLASSB_HOST);
	else if (net < 16777216L)
		addr = (net << IN_CLASSC_NSHIFT) | (host & IN_CLASSC_HOST);
	else
		addr = net | host;
	ret.s_addr = htonl(addr);
	*/

    return (struct in_addr) ret;
}


// inet_lnaof:
// Return the local network address portion of an
// internet address; handles class a/b/c network number formats.

in_addr_t inet_lnaof(struct in_addr in)
{
    printf("inet_lnaof: [todo] Not implemented!\n");
    return 0;

   /*
	in_addr_t i = ntohl(in.s_addr);

	if (IN_CLASSA(i))
		return ((i)&IN_CLASSA_HOST);
	else if (IN_CLASSB(i))
		return ((i)&IN_CLASSB_HOST);
	else
		return ((i)&IN_CLASSC_HOST);

    */
}


// inet_netof: 
// Return the network number from an internet
// address; handles class a/b/c network #'s.

in_addr_t inet_netof (struct in_addr in)
{
    printf("inet_netof: [BUGBUG] Not implemented!\n");
    return 0;

   /*
	in_addr_t i = ntohl(in.s_addr);

	if (IN_CLASSA(i))
		return (((i)&IN_CLASSA_NET) >> IN_CLASSA_NSHIFT);
	else if (IN_CLASSB(i))
		return (((i)&IN_CLASSB_NET) >> IN_CLASSB_NSHIFT);
	else
		return (((i)&IN_CLASSC_NET) >> IN_CLASSC_NSHIFT);
   */

}


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





