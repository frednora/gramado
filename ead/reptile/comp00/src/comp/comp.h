// comp.h
// The purpose of these routines is compose the final frame
// into the backbuffer and display it into the frontbuffer.
// Created by Fred Nora.

#ifndef __COMP_H
#define __COMP_H    1

//extern int __compose_lock;

extern int need_rootwindow_redraw;

//
// Compositor responsibilities:
//
// + Buffer management: backbuffer, frontbuffer, per-window canvases.
// + Blitting: copy frame canvas and client canvas into backbuffer.
// + Damage tracking: dirty regions, incremental redraw.
// + Visual effects: transparency, shadows, blur, animations.
// + Presentation: flush backbuffer → frontbuffer, vsync timing.
// + Cursor rendering: mouse pointer visuals, drag-and-drop feedback.
//
struct compositor_d
{
    int used;
    int magic;
    int initialized;

    int _locked;
    unsigned long counter;

// Disable the composition for compatibility with the old method
    int is_composition_disabled;

// Don’t copy the chrome into the backbuffer.
// Global override.
// Valid for all the windows.
// TRUE = compositor blit frame to the desktop, FALSE = compositor skips
    int disable_frame_blit;

    // ...

// Rendering state:
// + Current backbuffer and frontbuffer pointers
// + | Per-window offscreen surfaces (linked list or array of canvases)
//   | Active canvas list head, count of canvases
//   ps: The canvases are associated with the window,
//   so, we need the pointer for the head of the list of windows.
// + Damage regions (rectangles that need redraw)

// Performance metrics:
// + Frame counter and FPS
// + Last frame timestamp
// + Number of blits per frame
// + Dropped frames counter

// Visual effects:
// + Transparency enabled/disabled
// + Shadow rendering flag
// + Blur/animation flags
// + Theme color overrides

    // Effects
    //int use_transparency;
    //int use_shadows;
    //int use_blur;
    //int use_animations;

    // Mouse visuals
    //long mouse_x;
    //long mouse_y;
    //int show_cursor;
    //int cursor_style;

// Mouse and input visuals:
// + Current mouse position (x, y)
// + Cursor style (default, resize, text, drag)
// + Drag-and-drop feedback state

// Debug and configuration:
// + Logging enabled/disabled
// + Memory usage counters for buffers
// + Compatibility mode flags (old vs new compositor pipeline)

    // Debug/performance
    //int blits_per_frame;
    //int dropped_frames;
    //int logging_enabled;

};
extern struct compositor_d  Compositor;


//
// ===================================================
//

// Create a dc for a buffer given its size in KB.
struct dccanvas_d *comp_create_dc_for_a_buffer(
    char *buffer_address, size_t size_in_kb );
struct dccanvas_d *comp_create_dc_and_allocate_buffer(size_t size_in_kb);

void comp_add_to_list(struct canvas_information_d *ci);

// w, h, bits per pixel, dc
struct canvas_information_d *compCreateNewCanvas(struct dccanvas_d *dc);

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

void gwssrv_show_backbuffer(void);
// Flush the whole backbuffer
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
// #test
// Creating a real compositor. Using offscreen buffers.
void compComposeDesktop(void);


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


// It signalise that we need to erease the mouse pointer,
// copying the content from the backbuffer to the front buffer.
// #ps: Only when the server/compositor is drawing the mouse pointer.
void DoWeNeedToEraseMousePointer(int value);

//
// $
// INITIALIZATION
//

int compInitializeCompositor(void);

#endif    
