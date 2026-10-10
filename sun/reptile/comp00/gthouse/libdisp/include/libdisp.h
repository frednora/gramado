// libgd.h
// Graphics device library.
// Created by Fred Nora.

#ifndef __LIBDISP_H
#define __LIBDISP_H  1


#include <types.h>
#include <stddef.h>

#include "vk.h"       // # view input events
#include "lt8x8.h"
#include "rop.h"
#include "dccanvas.h"


// ===================================================


// Create a new device context for a given buffer.
// Parameters:
//   base   - pointer to buffer memory
//   width  - width in pixels
//   height - height in pixels
//   bpp    - bits per pixel (e.g. 32, 24, 16)
//
// Returns:
//   pointer to a new dccanvas_d, or NULL on failure.
struct dccanvas_d *libgd_create_dc(unsigned char *base,
                               unsigned long width,
                               unsigned long height,
                               unsigned long bpp);

// Getters for default contexts
struct dccanvas_d *libgd_get_backbuffer_dc(void);
struct dccanvas_d *libgd_get_frontbuffer_dc(void);


/*
// #test: 
// Addresses used by the frontbuffer
struct libgd_frontbuffer_info_d 
{
    int initialized;

    unsigned long frontbuffer_begin_va;
    unsigned long frontbuffer_end_va;

    unsigned long frontbuffer_visible_area_begin_va;
    unsigned long frontbuffer_visible_area_end_va;
};
extern struct libgd_frontbuffer_info_d  FrontbufferInfo;
*/

int 
libdisp_backbuffer_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop );

int 
grBackBufferPutpixel2 ( 
    unsigned int color, 
    int x, 
    int y );

// put pixel
// low level.
int 
fb_BackBufferPutpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    unsigned long buffer_va );

int
putpixel0 ( 
    struct dccanvas_d *dc,
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

void 
backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

void 
frontbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

int 
libgd_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    int back_or_front );

unsigned int grBackBufferGetPixelColor( int x, int y );

//
// #
// INITIALIZATION 
//

// Initialize the libgd library
int libgd_initialize(void);

#endif    



