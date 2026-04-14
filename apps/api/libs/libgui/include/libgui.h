// libgui.h
// Graphics device library.
// Created by Fred Nora.

#ifndef __LIBGUI_LIBGUI_H
#define __LIBGUI_LIBGUI_H  1

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
struct dccanvas_d *libgui_create_dc(unsigned char *base,
                               unsigned long width,
                               unsigned long height,
                               unsigned long bpp);

// Getters for default contexts
struct dccanvas_d *libgui_get_backbuffer_dc(void);
struct dccanvas_d *libgui_get_frontbuffer_dc(void);


/*
// #test: 
// Addresses used by the frontbuffer
struct libgui_frontbuffer_info_d 
{
    int initialized;

    unsigned long frontbuffer_begin_va;
    unsigned long frontbuffer_end_va;

    unsigned long frontbuffer_visible_area_begin_va;
    unsigned long frontbuffer_visible_area_end_va;
};
extern struct libgui_frontbuffer_info_d  FrontbufferInfo;
*/

int 
libgui_backbuffer_putpixel3 ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop );

int 
libgui_backbuffer_putpixel2 ( 
    unsigned int color, 
    int x, 
    int y );

// put pixel
// low level.
int 
libgui_fb_backbuffer_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    unsigned long buffer_va );

int
libgui_putpixel0 ( 
    struct dccanvas_d *dc,
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

void 
libgui_backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

void 
libgui_frontbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

int 
libgui_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    int back_or_front );

unsigned int libgui_backbuffer_getpixelcolor( int x, int y );


// libgui_backbuffer_draw_horizontal_line:
// Draw a horizontal line on backbuffer. 
void 
libgui_backbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags );

void 
libgui_frontbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags );


// #todo
// Draw char into a given device context
void 
libgui_drawchar_dc (
    struct dccanvas_d *dc, 
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor,
    unsigned long rop );

void 
libgui_drawchar (
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor,
    unsigned long rop );

void 
libgui_drawstring(
    unsigned long x, 
    unsigned long y, 
    const char *s, 
    unsigned int fg, 
    unsigned int bg, 
    unsigned long rop );

void 
libgui_refresh_rectangle_via_kernel(
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height );

void 
libgui_backbuffer_draw_rectangle0(
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    int fill,
    unsigned long rop_flags,
    int use_kgws );

void 
libgui_frontbuffer_draw_rectangle0 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    int fill,
    unsigned long rop_flags );


void
libgui_BackbufferDrawCharBlockStyle(
    unsigned long x,          // top-left in screen space
    unsigned long y,
    unsigned int  fgcolor,
    int           ch,         // character code
    int           scale);      // 1 = classic 8×8, 2 = 16×16 blocks, etc.

void 
libgui_drawstringblock(
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *str,
    int scale );

void libgui_set_mouse_pointer(unsigned long x, unsigned long y);

//
// #
// INITIALIZATION 
//

// Initialize the libgui library
int libgui_initialize(void);

#endif    



