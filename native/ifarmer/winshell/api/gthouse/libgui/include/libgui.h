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

//
// UI
//

#include "ui.h"



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
libgui_BackbufferDrawCharBlockStyle(
    unsigned long x,          // top-left in screen space
    unsigned long y,
    unsigned int  fgcolor,
    int           ch,         // character code
    int           scale);      // 1 = classic 8×8, 2 = 16×16 blocks, etc.

void
libgui_BackbufferDrawCharBlockStyle_dc(
    struct dccanvas_d *dc,
    unsigned long x,          // top-left in screen space
    unsigned long y,
    unsigned int fgcolor,
    int ch,         // character code
    int scale );    // 1 = classic 8×8, 2 = 16×16 blocks, etc.

void 
libgui_drawstringblock(
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *str,
    int scale );

void 
libgui_drawstringblock_dc(
    struct dccanvas_d *dc,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *str,
    int scale );


void libgui_set_mouse_pointer(unsigned long x, unsigned long y);

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

//
// #
// INITIALIZATION 
//

// Initialize the libgui library
int libgui_initialize(void);

#endif    

