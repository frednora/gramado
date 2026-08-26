
// rw.c
// Read and write from socket.

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

ssize_t 
__gws_send ( 
    int sockfd, 
    const void *buf, 
    size_t len, 
    int flags )
{
// flags?
    return (ssize_t) write( sockfd, (const void *) buf, len );
}

ssize_t 
__gws_recv ( 
    int sockfd, 
    void *buf, 
    size_t len, 
    int flags )
{
// flags?
    return (ssize_t) read( sockfd, (const void *) buf, len );
}

ssize_t gws_write_to_server (int fd, const void *buf, size_t count)
{
    return (ssize_t) write( fd, (const void *) buf, count );
}

ssize_t gws_read_from_server (int fd, const void *buf, size_t count)
{
    return (ssize_t) read( fd, (const void *) buf, count );
}

