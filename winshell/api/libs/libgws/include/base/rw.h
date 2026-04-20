/*
 * File: rw.h
 *     Read and write from socket.
 *     This header should be included by including "gws.h".
 */

#ifndef __LIBGWS_RW_H
#define __LIBGWS_RW_H  1

ssize_t __gws_send( int sockfd, const void *buf, size_t len, int flags );
ssize_t __gws_recv( int sockfd, void *buf, size_t len, int flags );

ssize_t gws_write_to_server(int fd, const void *buf, size_t count);
ssize_t gws_read_from_server(int fd, const void *buf, size_t count);

#endif    



