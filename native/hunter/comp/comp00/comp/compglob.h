// compglob.h

#ifndef __COMP_COMPGLOB_H
#define __COMP_COMPGLOB_H    1

// ...

// Vamos usar ou nao.
extern int use_vsync;
#define VSYNC_YES  1
#define VSYNC_NO   0

// Indexes to canvases
#define CANVAS_COUNT_MAX    8
#define CANVAS_FRONTBUFFER     0  // VRAM
#define CANVAS_BACKBUFFER      1  // RAM
#define CANVAS_SPAREBUFFER     2  // spare buffer

#define CANVAS_TEST00  6
#define CANVAS_BG00  7

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

    struct gws_window_d *owner_window;

    struct canvas_information_d *clientarea_canvas;
    struct canvas_information_d *next;
};

extern unsigned long canvasList[CANVAS_COUNT_MAX];

extern struct canvas_information_d  *canvas_head;
extern struct canvas_information_d  *canvas_frontbuffer;
extern struct canvas_information_d  *canvas_backbuffer;

#endif   

