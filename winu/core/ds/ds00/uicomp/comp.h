// comp.h
// The purpose of these routines is compose the final frame
// into the backbuffer and display it into the frontbuffer.
// Created by Fred Nora.

#ifndef __COMP_H
#define __COMP_H    1

//extern int __compose_lock;


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
void mouse_at(void);

// Sinaliza que precisamos apagar o ponteiro do mouse,
// copiando o conteudo do backbuffer no LFB.
void DoWeNeedToEraseMousePointer(int value);

//
// $
// INITIALIZATION
//

int compInitializeCompositor(void);

#endif    


