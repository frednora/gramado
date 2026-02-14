// comp.h
// The purpose of these routines is compose the final frame
// into the backbuffer and display it into the frontbuffer.
// Created by Fred Nora.

#ifndef __COMP_H
#define __COMP_H    1

//extern int __compose_lock;


// Structure to describe a clipping region for a buffer
struct spare_buffer_clip_info_d 
{
    int initialized;       // Safety first

    unsigned long width;   // buffer width in pixels
    unsigned long height;  // buffer height in pixels

    unsigned long bpp;     // bytes per pixel (4 or 3)
    unsigned long pitch;   // bytes per row (width * bytes_per_pixel)

    void *base;            // pointer to buffer memory
};
extern struct spare_buffer_clip_info_d  SpareBufferClipInfo;

// Spare buffer
extern char *spare_128kb_buffer_p;

extern struct dccanvas_d *spare_dccanvas;

struct compositor_d
{
    int used;
    int magic;
    int initialized;

    int _locked;
    unsigned long counter;

// >> This flag enables composition for the display server.
// In this case the server will compose a final backbbuffer
// using buffers and the zorder for these buffers. In this case 
// each application window will have it's own buffer.
// >> If this flag is not set, all the windows will be painted in the
// directly in the same backbuffer, and the compositor will just
// copy the backbuffer to the LFB.
    int __enable_composition;

    // ...
};
extern struct compositor_d  Compositor;


//
// ===================================================
//

// Initialize the spare buffer clipping info
void 
setup_spare_buffer_clip(
    unsigned long width,
    unsigned long height,
    unsigned long bpp,
    void *base );

void *comp_create_slab_spare_128kb_buffer(size_t size_in_kb);

struct canvas_information_d *compCreateCanvasUsingSpareBuffer(void);

void 
spare_putpixel0(
    unsigned int color, 
    unsigned long x, 
    unsigned long y, 
    unsigned long rop );
void comp_draw_into_spare_buffer(void);
void comp_test_spare_buffer(void);
// Copy from spare buffer (0,0) into backbuffer at (dst_x, dst_y).
void comp_blit_spare_to_backbuffer(
    int dst_x, int dst_y,
    int width, int height );

void 
comp_blit_canvas_to_canvas_imp(
    struct canvas_information_d *src_canvas,
    struct canvas_information_d *dst_canvas,
    int dst_x, int dst_y,
    int width, int height );

void 
comp_blit_canvas_to_canvas(
    int id_src_canvas,
    int id_dst_canvas,
    int dst_x, int dst_y,
    int width, int height );


// Flush the window's rectangle
int gws_show_window_rect(struct gws_window_d *window);

//
// Flush window
//

int flush_window(struct gws_window_d *window);
int flush_window_by_id(int wid);


//
// Flush backbuffer
//

void gwssrv_show_backbuffer (void);
// Flush the whole backbuffer.
void flush_frame(void);

//
// Compose
//

// A worker for wmCompose().
void reactRefreshDirtyWindows(void);
void wmReactToPaintEvents(void);
// A worker for wmCompose().

void __display_mouse_cursor(void);
void comp_display_desktop_components(void);


void 
wmCompose(
    unsigned long jiffies, 
    unsigned long clocks_per_second );


//
// Mouse support
//

long comp_get_mouse_x_position(void);
long comp_get_mouse_y_position(void);
void comp_set_mouse_position(long x, long y);
void comp_initialize_mouse(void);

struct gws_window_d *mouse_at(void);

// Sinaliza que precisamos apagar o ponteiro do mouse,
// copiando o conteudo do backbuffer no LFB.
void DoWeNeedToEraseMousePointer(int value);

//
// $
// INITIALIZATION
//

int compInitializeCompositor(void);

#endif    


