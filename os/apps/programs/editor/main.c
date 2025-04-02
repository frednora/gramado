// main.c
// Simple text editor for Gramado OS.
// 2020 - Created by Fred Nora.

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
// The client-side library.
#include <gws.h>
// Internal
#include <packet.h>

#include "globals.h"
#include <editor.h>


int main(int argc, char *argv[])
{
    //if (argc < 0)
        //return 1;
    return (int) editor_initialize(argc,argv);
}

