// editor.h


#ifndef __EDITOR_H
#define __EDITOR_H    1

// rtl
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

// The client-side library
#include <gws.h>

// #test
// The client-side library
#include <libgui.h>

// Internal
#include <packet.h>


// ...
#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)


int editor_initialize( int argc, char *argv[] );

#endif    

