// Header for functions that belongs to the old
// implementation of the library, before the usage of
// dc as parameter they were drawing directly to the 
// backbuffer/frontbuffer.
// #ps: Maybe it will be deleted.

#ifndef __DIRECT00_H
#define __DIRECT00_H    1

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

unsigned int libgui_backbuffer_getpixelcolor( int x, int y );

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


// ---------------------------------------------

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
libgui_refresh_rectangle_via_kernel(
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height );

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


#endif  

