// screen.h
// Created by Fred Nora.

#ifndef ____SCREEN_H
#define ____SCREEN_H    1


// See: globals.c
extern int refresh_device_screen_flag;
extern int refresh_valid_screen_flag;


struct gws_screen_d
{
    int id;
    int used;
    int magic;
    
    unsigned long flags;
    
    unsigned long width;
    unsigned long height;
    unsigned long bpp;    // bits per pixel

    unsigned long pitch;

// The origin at the center of the screen.
    unsigned long hotspot_x;
    unsigned long hotspot_y;

// The new limits when the origin is the center of the screen.
    unsigned long min_x;
    unsigned long min_y;
    unsigned long max_x;
    unsigned long max_y;

    unsigned long font_size;

    unsigned long char_width; 
    unsigned long char_height;
    
    void *backbuffer;
    void *frontbuffer;

    // Belongs to this display.
    struct gws_display_d *display;
    
    struct gws_screen_d *next;

    struct gws_window_d *root;
};


// The device screen.
// The refresh is called by refresh_device_screen();
// It is called explicit by the app.
// see: globals.c
extern struct gws_screen_d  *DeviceScreen;

//
// == prototypes ================================
//


// Refresh the device screen
void refresh_screen(void);

// Refresh the device screen
void refresh_device_screen(void);

// Refresh the valid screen
void refresh_valid_screen(void);


#endif    

