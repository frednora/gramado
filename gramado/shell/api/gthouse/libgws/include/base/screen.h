// screen.h
// Screen support
// This header should be included by including "gws.h".
// Created by Fred Nora

#ifndef __LIBGWS_SCREEN_H
#define __LIBGWS_SCREEN_H  1

struct gws_screen_d
{
    int id;  // The screen ID.
    // Structure validation
    int used;
    int magic;

// Root window 'wid'.
    int root_wid;

// W and h of the screen.
    unsigned long width;
    unsigned long height;
// Bits per pixel.
    unsigned long bpp;

/*
// width and height of in millimeters
    unsigned long mwidth;
    unsigned long mheight;
*/

    unsigned long font_size;

    unsigned long char_width; 
    unsigned long char_height;
    
    void *backbuffer;
    void *frontbuffer;

// Belongs to this display.
    struct gws_display_d *display;

    struct gws_screen_d *next;
};


#endif    


