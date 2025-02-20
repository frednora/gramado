// display.h 
// Display structure support.
// Created by Fred Nora.

// This header should be included by including "gws.h".

#ifndef __LIBGWS_DISPLAY_H
#define __LIBGWS_DISPLAY_H    1

// #??
// Podemos ter um buffer de request dentro da estrutura de display.
// Isso é melhor que um buffer global solto,
// Dessa forma cada display pode ter seus próprios buffers.
// #todo
// Handle the access for the display device.
// The kernel needs to gice us one fd, for future configurations.

struct gws_display_d
{
    int id;  // Display id.
    // Structure validation
    int used;
    int magic;

// "host:display" string used on this connect.
    char *display_name;

    //int _device_fd;

// Network socket. 
    int fd;
// Is someone in critical section? 
    int lock;
    int connected;
    int running;

    //#test
    //char *buffer;  //Output buffer starting address. 
    //char *bufptr;  //Output buffer index pointer. 
    //char *bufmax;  //Output buffer maximum+1 address. 

//
// Screen support.
//

    int default_screen;  // Current screen.
    int nscreens;  // Number of screens.
    struct gws_screen_d *screens;  // Screen list.

    // ...

    //char *request_buffer;
    //char *reply_buffer;

    struct gws_display_d *next;
};


//
// The current display
//

//struct gws_display_d *libgwsCurrentDisplay;
extern struct gws_display_d *libgwsCurrentDisplay;

// ================

struct gws_display_d *gws_open_display(const char *display_name);
void gws_close_display(struct gws_display_d *display);

int gws_set_current_display(struct gws_display_d *display);
struct gws_display_d *gws_get_current_display(void);

void gws_display_exit(struct gws_display_d *display);


#endif    

