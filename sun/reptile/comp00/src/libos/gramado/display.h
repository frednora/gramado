// display.h
// Display support.
// The client-side application can handle the display driver functionalities 
// using this structures on the server-side. But actually the server 
// can call more than one display device driver to change some hardware 
// characteristics of the display, just like resolution.
// Our first goal is to connect this server with the 'qemu display driver' 
// or 'bootloader display driver' that probably are gonna me build 
// inside the base kernel.
// Created by Fred Nora.

#ifndef __GRAMADO_DISPLAY_H
#define __GRAMADO_DISPLAY_H    1

//
struct gws_display_d
{
    int id;
    int used;
    int magic;

//"host:display" string used on this connect
    char *display_name;

// Network socket.
    int fd;
// #todo
// Display device fd.
// For resolution configurations.
    int device_fd;

    int lock;  // is someone in critical section?  

// #todo
// The IP of the host where we're gonna display.

    //char ipv4[4];
    //char ipv6[8];

// #todo:
// See: r_render.h

    //int render_mode;
    //rendermode_t render_mode;

// #test

    //char *buffer;		//Output buffer starting address. 
    //char *bufptr;		//Output buffer index pointer. 
    //char *bufmax;		//Output buffer maximum+1 address. 

// The device screen.
// The refresh is called by refresh_device_screen();
// It is called explicit by the app.

    struct gws_screen_d *device_screen;

// The device screen.
// The refresh is called by refresh_valid_screen();
// It is called frequently by the window server.

    struct gws_screen_d *valid_screen;

// #test
// root window for the screen ??
    //struct gws_window_d *root;

    struct gws_window_d *current;
    
// ??
//#test

    //struct gws_gui_d *gui;

    //...

    struct gws_display_d *next;
};

// See: gws.c
extern struct gws_display_d  *CurrentDisplay;

//#todo
//#define GWS_DISPLAY_MAX 128;
//unsigned long displayList[GWS_DISPLAY_MAX];

#endif    


