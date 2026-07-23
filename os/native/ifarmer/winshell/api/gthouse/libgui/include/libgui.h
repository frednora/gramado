// libgui.h
// Graphics device library.
// Created by Fred Nora.

#ifndef __LIBGUI_LIBGUI_H
#define __LIBGUI_LIBGUI_H  1

#include <types.h>
#include <stddef.h>

#include "font.h"
#include "vk.h"       // # view input events
#include "lt8x8.h"
#include "rop.h"
#include "dccanvas.h"

#include "bmp.h"  //#test

//
// UI
//

#include "ui.h"


// Header for functions that belongs to the old
// implementation of the library, before the usage of
// dc as parameter they were drawing directly to the 
// backbuffer/frontbuffer.
// #ps: Maybe it will be deleted.
#include "direct00.h"


#define CANVAS_COUNT_MAX    16

// Each window needs to have an index for one of this structurea.
// This way the display server will know the information about 
// the window's canvas.
struct canvas_information_d
{
    int used;
    int magic;
    int initialized;       // Safety first

// If dirty, flush it into the backbuffer.
    int dirty;

// Is it a canvas for frame/chrome.
    int is_frame;

    unsigned long width;   // buffer width in pixels
    unsigned long height;  // buffer height in pixels
    unsigned long bpp;     // bits per pixel
    unsigned long pitch;   // bytes per row (width * bytes_per_pixel)

    void *base;            // pointer to buffer memory

    struct dccanvas_d *dc;  // DC associated with this canvas information

    //struct gws_window_d *owner_window;
    int owner_wid;

    //struct canvas_information_d *clientarea_canvas;
    //struct canvas_information_d *next;
};

// List of canvases
// List of pointer for canvas information structures
extern unsigned long libgui_canvasList[CANVAS_COUNT_MAX];


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
struct dccanvas_d *libgui_create_dc(
    unsigned char *base,
    unsigned long width,
    unsigned long height,
    unsigned long bpp );

// Getters for default contexts
struct dccanvas_d *libgui_get_backbuffer_dc(void);
struct dccanvas_d *libgui_get_frontbuffer_dc(void);

int
libgui_putpixel0 ( 
    struct dccanvas_d *dc,
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );


// #test
// Draw a pixel inside a canvas, given it's device context.
// #todo: No Clipping support yet
void 
libgui_draw_horizontal_line_dc ( 
    struct dccanvas_d *dc,
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

// Draw a string into a device context (dc)
// starting at (x, y), using fg/bg colors and rop.
void 
libgui_drawstring_dc (
    struct dccanvas_d *dc,
    unsigned long x, 
	unsigned long y,
    unsigned int fg_color,
    unsigned int bg_color,
    unsigned long rop,
    const char *string );


// ======================================
// Copy a rectangle
// #todo
// IN:
// w, h, 
// dst l, dst t, dst address, 
// src l, src t, src address.
void 
libgui_refresh_rectangle1 ( 
    unsigned long width,    // common
    unsigned long height,   // common
    unsigned long dst_x,        // dst stuff
    unsigned long dst_y,        // dst stuff
    unsigned long buffer_dest,  // dst stuff
    unsigned long src_x,        // src stuff
    unsigned long src_y,        // src stuff
    unsigned long buffer_src );  // src stuff

// Blit from one canvas into another.
// src_canvas: source canvas (offscreen, window, etc.)
// dst_canvas: destination canvas (backbuffer, frontbuffer, etc.)
void 
libgui_blit_canvas_to_canvas_imp(
    struct canvas_information_d *src_canvas,
    struct canvas_information_d *dst_canvas,
    int dst_x, int dst_y,
    int width, int height );

// Given the indexes
void 
libgui_blit_canvas_to_canvas(
    int id_src_canvas,
    int id_dst_canvas,
    int dst_x, int dst_y,
    int width, int height );

int lingui_initialize_canvas_list(void);

// #test
// Drawing a rectangle inside a given canvas,
// given its device context.
void
lingui_draw_rectangle0_dc(
    struct dccanvas_d *dc,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    unsigned int color,
    unsigned long rop );

void
libgui_BackbufferDrawCharBlockStyle_dc(
    struct dccanvas_d *dc,
    unsigned long x,          // top-left in screen space
    unsigned long y,
    unsigned int fgcolor,
    int ch,         // character code
    int scale );    // 1 = classic 8×8, 2 = 16×16 blocks, etc.

void 
libgui_drawstringblock_dc(
    struct dccanvas_d *dc,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *str,
    int scale );


void 
__draw_button_borders_dc(
    struct dccanvas_d *dc,
    struct ui_component_d *ui_c,
    unsigned int tl_2,  // tl 2 inner (light)
    unsigned int tl_1,  // tl 1 most inner (lighter)
    unsigned int br_2,  // br 2 inner (dark)
    unsigned int br_1,  // br 1 most inner (light) 
    unsigned int outer_color );

struct ui_component_d *libgui_create_ui_component(
    struct dccanvas_d *dc,
    int type,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    const char *label );

// Redraw a ui component
int 
libgui_redraw_ui_component(
	struct ui_component_d *uic,
    struct dccanvas_d *dc );

int 
libgui_set_ui_component_position(
    struct ui_component_d *uic,
	unsigned long left,
	unsigned long top );

int 
libgui_set_ui_component_dimension(
    struct ui_component_d *uic,
	unsigned long width,
	unsigned long height );

void libgui_set_mouse_pointer(unsigned long x, unsigned long y);

//
// #
// INITIALIZATION 
//

// Initialize the libgui library
int libgui_initialize(void);

#endif    

