// display.h
// Display device support.
// Created by Fred Nora.

#ifndef ____DISPLAY_H
#define ____DISPLAY_H    1



// Pointers to screens.
#define MAX_SCREENS    4
#define SCREEN_FRONTBUFFER     0
#define SCREEN_BACKBUFFER      1
//#define SCREEN_BACKBUFFER2   2
//#define SCREEN_BACKBUFFER3   3

extern unsigned long screens[MAX_SCREENS];



struct gws_display_d
{
    int id;
    int used;
    int magic;

//"host:display" string used on this connect
    char *display_name;

    int fd;    // Network socket. 
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
// see: globals.c
extern struct gws_display_d  *CurrentDisplay;

//#todo
//#define GWS_DISPLAY_MAX 128;
//unsigned long displayList[GWS_DISPLAY_MAX];

#endif    


