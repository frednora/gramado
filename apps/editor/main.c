// main.c
// EDITOR.BIN
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

// The client-side library
#include <gws.h>

// #test
// The client-side library
#include <libgui.h>

// Internal
#include <packet.h>

#include "globals.h"
#include <editor.h>


int main(int argc, char *argv[])
{
    int status = -1;

    //if (argc < 0)
        // return EXIT_FAILURE;

    status = (int) libgui_initialize();
    if (status < 0){
        printf("editor: libgui_initialize fail\n");
        exit(1);
    }

    struct dccanvas_d *dc;
    dc = (struct dccanvas_d *) libgui_get_backbuffer_dc();
    if ((void*)dc == NULL){
        printf("editor: libgui_get_backbuffer_dc fail\n");
        exit(1);
    }

// #test
// Drawing a pixel using the libdisp library.

/*
    // Draw it (backbuffer)
    libgui_putpixel0 ( dc, 0xFF0000, 10, 10, 0 );

    // Draw it (frontbuffer)
    libgui_frontbuffer_putpixel( 0x00FF00, 20, 20, 0 );
*/

    return (int) editor_initialize(argc,argv);
}

