// demos.c
// Commom for all demos.


#include "../gram3d.h"


// Use demos or not
int gUseDemos = TRUE;

// The demos window. This is the canvas for demos.
struct gws_window_d *__demo_window;

// Global counter used by the demos.
unsigned long gBeginTick=0;

// ===================================


struct gws_window_d *__create_demo_window (
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height )
{
    struct gws_window_d *w;

    if ( (void*) __root_window == NULL ){
        return NULL;
    }

// Create window

    w = 
        (struct gws_window_d *) CreateWindow ( 
                                    WT_SIMPLE, 
                                    0, //style
                                    1, //status 
                                    1, //view
                                    "DemoWin",  
                                    left, top, width, height,   
                                    __root_window, 0, 
                                    COLOR_BLACK, 
                                    COLOR_BLACK );

    if ( (void *) w == NULL ){
        return NULL;
    }
    if ( w->used != TRUE ||  w->magic != 1234 ){
        return NULL;
    }

// Register the window.
    int WindowId= -1;
    WindowId = (int) RegisterWindow(w);
    if (WindowId < 0)
    {
        return NULL;
    }

// ok
    return (struct gws_window_d *)  w;
}


