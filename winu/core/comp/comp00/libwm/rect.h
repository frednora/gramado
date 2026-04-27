// rect.h
// Created by Fred Nora.

#ifndef __LIBUI_RECT_H
#define __LIBUI_RECT_H    1


// Structure for a rectangle.
// A rectangle belongs to a window.
struct gws_rect_d 
{
    int used;
    int magic;

    struct gws_window_d *window;

    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    unsigned int bg_color;  // color
    int is_filled;          // filled or not
    unsigned long rop;      // raster operation

    //unsigned int flags;      // Reserved for future use

    int dirty;  // Validation

    struct gws_rect_d *next;
};

//
// =======================================================
//

//
// Core Rectangle Object
//

// Create a rectangle object
struct gws_rect_d *rectangleObject(void);

int
set_rect ( 
    struct gws_rect_d *rect, 
    unsigned long left, 
    unsigned long top,
    unsigned long width,
    unsigned long height );

void rect_set_left   ( struct gws_rect_d *rect, unsigned long value );
void rect_set_top    ( struct gws_rect_d *rect, unsigned long value );
void rect_set_width  ( struct gws_rect_d *rect, unsigned long value );
void rect_set_height ( struct gws_rect_d *rect, unsigned long value );

//
// Geometry Manipulation
//

void 
inflate_rect ( 
    struct gws_rect_d *rect, 
    unsigned long cx, 
    unsigned long cy );
    
void 
copy_inflate_rect ( 
    struct gws_rect_d *rectDest, 
    struct gws_rect_d *rectSrc, 
    unsigned long cx, 
    unsigned long cy );
    
void 
offset_rect ( 
    struct gws_rect_d *rect, 
    unsigned long cx, 
    unsigned long cy );
        
void 
copy_offset_rect ( 
    struct gws_rect_d *rectDest, 
    struct gws_rect_d *rectSrc, 
    unsigned long cx, 
    unsigned long cy ); 

//
// Validation helpers
//

int rect_validate_size(struct gws_rect_d *rect);
int rect_validate_size2(struct gws_rect_d *rect);

int is_rect_null(struct gws_rect_d *rect);
int is_rect_filled(struct gws_rect_d *rect);
int is_rect_dirty(struct gws_rect_d *rect);


//
// Containment and Intersection
//

int 
rect_contains_vertically ( 
    struct gws_rect_d *rect,  
    unsigned long y );

int 
rect_contains_horizontally ( 
    struct gws_rect_d *rect,
    unsigned long x );

// rect_intersect() (commented out)  

//
// Drawing and Refresh
//

int gwssrv_refresh_this_rect(struct gws_rect_d *rect);
int flush_rectangle(struct gws_rect_d *rect);


// Backbuffer support
void 
backbuffer_draw_rectangle( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags );

// Frontbuffer support
void 
frontbuffer_draw_rectangle( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags );

void 
rect_refresh_rectangle_via_kernel(
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height );

void 
gws_refresh_rectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height );
    
void 
gws_refresh_rectangle0 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height,
    unsigned long dst_surface_base,
    unsigned long src_surface_base );

// Copy rectangle
// worker for bitblt in other document.
void 
__refresh_rectangle1 ( 
    unsigned long width,    // common
    unsigned long height,   // common
    unsigned long dst_x,        // dst stuff
    unsigned long dst_y,        // dst stuff
    unsigned long buffer_dest,  // dst stuff
    unsigned long src_x,        // src stuff
    unsigned long src_y,        // src stuff
    unsigned long buffer_src );  // src stuff

// Paint it into the backbuffer.
void 
rectBackbufferDrawRectangle0 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    int fill,
    unsigned long rop_flags,
    int use_kgws );

// Paint it into the backbuffer.
void 
rectBackbufferDrawRectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    int fill,
    unsigned long rop_flags );

void 
rectBackbufferDrawRectangleInsideWindow (
    int wid, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    int fill,
    unsigned long rop );


int update_rectangle(struct gws_rect_d *rect);


//
// Surface support
//

// Atualiza o retângulo da surface da thread.
void 
setup_surface_rectangle ( 
    unsigned long left, 
    unsigned long top, 
    unsigned long width, 
    unsigned long height );

void invalidate_surface_retangle(void);

//
// Client/Window Rect Accessors
//

struct gws_rect_d *clientrect_from_window(struct gws_window_d *window);
struct gws_rect_d *rect_from_window(struct gws_window_d *window);


#endif   

