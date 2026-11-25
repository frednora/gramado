
//  fd_set.h
// credits:
// Serenity OS.

#ifndef __FD_SET_H
#define __FD_SET_H    1


// __fd_set:
struct __fd_set {
    unsigned char fds_bits[FD_SETSIZE / 8];
};
typedef struct __fd_set  fd_set;
typedef struct __fd_set  gramado_fd_set;


#define FD_SETSIZE  1024

#define FD_ZERO(set)                         \
    do {                                     \
        memset( (set), 0, sizeof(fd_set) );  \
    } while (0)

#define FD_CLR(fd, set)    ((set)->fds_bits[(fd / 8)] &= ~(1 << (fd) % 8))
#define FD_SET(fd, set)    ((set)->fds_bits[(fd / 8)] |=  (1 << (fd) % 8))
#define FD_ISSET(fd, set)  ((set)->fds_bits[(fd / 8)]  &  (1 << (fd) % 8))

#endif    

