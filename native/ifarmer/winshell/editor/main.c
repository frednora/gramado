// main.c
// EDITOR.BIN
// Simple text editor for Gramado OS.
// 2020 - Created by Fred Nora.

/*
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
*/

#include "globals.h"
#include <editor.h>

static int __open_document(char *file_name);

// ---------------------------------------------

static int __open_document(char *file_name)
{
    int fd = -1;
    char buf[1024];

    if ((void*)file_name == NULL)
        goto fail;

    //doc_fp = (FILE *) fopen(file_name, "r+");        
    //if (doc_fp == NULL){
    //    goto fail;
    //}
    //int fd = (int) fileno(doc_fp);

    fd = (int) open((char *) file_name, 0, "a+");
    if (fd < 0){
        printf ("__open_document: on open()\n");
        goto fail;
    }
    read(fd, buf, 512);
    printf("%s",buf);

    return 0;  // OK

fail:
    printf("fail\n");
    return (int) -1;
}

int main(int argc, char *argv[])
{
    int status = -1;

    //if (argc < 0)
        // return EXIT_FAILURE;

//
// document
//

    /*

    // #test
    // #todo: Save it into a proper buffer
    // We need a structure to handle the document
    // loaded by the editor.

    if (argc >= 2){
        __open_document( (char *) argv[1] );
        while(1){}
    }
    */

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

