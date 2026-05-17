
// sys/un.h
// Socket support for unix style. 
// PF_UNIX and PF_LOCAL address family. 
// Used for local communication between programs 
// running on the same computer.
// See:
// https://en.wikipedia.org/wiki/Berkeley_sockets
// 2020 - Created by Fred Nora.

#ifndef ____UN_H
#define ____UN_H    1

// #test
#include <types.h>
#include <sys/ansi.h>
#include <netinet/in.h>

#define UNIX_PATH_MAX  108

// sockaddr_un:
// Socket address, unix style.

struct sockaddr_un
{
    //#bugbug
    //ps: It needs to be 16 bit.

    uint16_t sun_family;
    char sun_path[UNIX_PATH_MAX];
};


/*
// #todo
// socket address gramado style.
struct sockaddr_gramado {

    uint16_t family;
    char sun_path[UNIX_PATH_MAX];    
};
*/


#endif    



