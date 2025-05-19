// rect.h
// Created by Fred Nora.

#ifndef __RECT_H
#define __RECT_H    1


// Structure for a rectangle.
// A rectangle belongs to a window.
struct gws_rect_d 
{
    //object_type_t objectType;
    //object_class_t objectClass;
    int used;
    int magic;
// Invalidate rectangle.
// When invalidated it needs to be flushed into the framebuffer.
    int dirty;
    int flag;
//estilo de design
    int style;
// Filled or not.
    int is_empty;
// dimensoes
    unsigned long x;
    unsigned long y;
    unsigned long cx;
    unsigned long cy;
// margins
    unsigned long left;
    unsigned long top;
    unsigned long right;
    unsigned long bottom;
    unsigned long width;
    unsigned long height;
    unsigned int bg_color;
    int is_solid;
    unsigned long rop;


// Essa é  ajanela à qual o retângulo pertence.
    struct gws_window_d *window;   //mudar. #todo

    struct gws_rect_d *next;
};

int rect_validate_size(struct gws_rect_d *rect);
int rect_validate_size2(struct gws_rect_d *rect);

int 
rect_contains_vertically ( 
    struct gws_rect_d *rect,  
    unsigned long y );

int 
rect_contains_horizontally ( 
    struct gws_rect_d *rect,
    unsigned long x );


void 
rect_set_left ( 
    struct gws_rect_d *rect, 
    unsigned long value );
    
void 
rect_set_top ( 
    struct gws_rect_d *rect, 
    unsigned long value );
    
void 
rect_set_right ( 
    struct gws_rect_d *rect, 
    unsigned long value );
    
void 
rect_set_bottom ( 
    struct gws_rect_d *rect, 
    unsigned long value );

int is_rect_null(struct gws_rect_d *rect);
int is_rect_empty(struct gws_rect_d *rect);
int is_rect_dirty(struct gws_rect_d *rect);

int gwssrv_refresh_this_rect(struct gws_rect_d *rect);
int flush_rectangle(struct gws_rect_d *rect);

struct gws_rect_d *clientrect_from_window(struct gws_window_d *window);
struct gws_rect_d *rect_from_window(struct gws_window_d *window);

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


int update_rectangle(struct gws_rect_d *rect);

int
set_rect ( 
    struct gws_rect_d *rect, 
    unsigned long left, 
    unsigned long top,
    unsigned long width,
    unsigned long height );



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



#endif   

