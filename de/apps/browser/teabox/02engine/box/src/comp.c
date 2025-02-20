// comp.c
// Compositor
// Created by Fred Nora.

// Display server routines.
#include "gram3d.h"

//global
int comp_config_use_mouse = FALSE;

extern struct gws_window_d *mouse_hover;

// #todo
// Create some configuration globals here
// int gUseSomething = TRUE;
// ...

//old
static long __old_mouse_x=0;
static long __old_mouse_y=0;
//current
static long __new_mouse_x=0;
static long __new_mouse_y=0;
static int __mouse_initialized = FALSE;


// Onde esta o mouse? em que janela?
// simple implementation.
// get the old one in the 32bit version of gramado.

void mouse_at(void)
{
    register int i=0;
    struct gws_window_d *w;
    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        w = (void*) windowList[i];
        if ( (void*) w != NULL )
        {
            if (w->magic == 1234)
            {
                if ( __new_mouse_x > w->absolute_x &&
                     __new_mouse_x < w->right &&
                     __new_mouse_y > w->absolute_y &&
                     __new_mouse_y > w->bottom )
                {
                    if (w != __root_window)
                    {
                        mouse_hover = (void *) w;
                        redraw_window(w,TRUE);
                    }
                }
            }
        }
    };
}

long comp_get_mouse_x_position(void)
{
    return (long) __new_mouse_x;
}

long comp_get_mouse_y_position(void)
{
    return (long) __new_mouse_y;
}

// + Apaga o cursor antigo, copiando o conteudo do backbuffer
// + Pinta o novo cursor diretamente no lfb.
void __display_mouse_cursor(void)
{
    if ((void *) display_server == NULL)
        return;
    if (display_server->initialized != TRUE)
        return;

// #todo Limits
// Precisa inicializar os valores sobre o mouse.
// Precisa criar uma estrura pra eles.

    if ( __old_mouse_x<0 ){ __old_mouse_x=0; }
    if ( __old_mouse_y<0 ){ __old_mouse_y=0; }

    if ( __new_mouse_x<0 ){ __new_mouse_x=0; }
    if ( __new_mouse_y<0 ){ __new_mouse_y=0; }

//------
//#dangerdanger
//#todo: show the backbuffer
// APAGA

    // So apagaremos se houve algum movimento.
    //if( __old_mouse_x != __new_mouse_x ||
    //    __old_mouse_y != __new_mouse_y )
    //{
        gws_refresh_rectangle( 
            __old_mouse_x, __old_mouse_y, 8, 8 );
    //}

// save
    __old_mouse_x = __new_mouse_x;
    __old_mouse_y = __new_mouse_y;


// #todo: 
// print directly into the lfb.
// DRAW
    frontbuffer_draw_rectangle( 
        (unsigned long) __new_mouse_x, 
        (unsigned long) __new_mouse_y, 
        (unsigned long) 8, 
        (unsigned long) 8, 
        COLOR_YELLOW, 
        0 );
//------        
}

// Flush
// The compositor.
// Called by wmCompose
void compose(void)
{
    int Dirty = FALSE;

// fps++
    if (WindowManager.initialized == TRUE){
        WindowManager.frame_counter++;
    }

// Backgroud
// If the background is marked as dirty, 
// we flush it, validate it, show the cursor and return.

    Dirty = (int) is_background_dirty();

    if (Dirty == TRUE){
        gws_show_backbuffer();
        validate_background();  
        return;
    }

// Rectangles
// Refresh the rectangles of the windows.
    wmRefreshDirtyRectangles();

//
// #suspended
//

// mouse
// Show the cursor in the screen.
// remember: we're using 15 fps refresh,
// we can change this in the kernel.

    if(comp_config_use_mouse == TRUE)
        __display_mouse_cursor();

// Validate the frame.
    validate_background();
    validate();
// fps
    //__update_fps();
}


// global
void comp_initialize_mouse(void)
{
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

    int hotspotx=0;
    int hotspoty=0;
    if(w>=0 && w<=800)
        hotspotx = (w >> 1);
    if(h>=0 && h<=800)
        hotspoty = (h >> 1);

    __old_mouse_x = hotspotx;
    __old_mouse_y = hotspoty;
    __new_mouse_x = hotspotx;
    __new_mouse_y = hotspoty;

    __mouse_initialized = TRUE; 

// CONFIG:
    //comp_config_use_mouse = TRUE;
    comp_config_use_mouse = FALSE;
}

// global
// Set new mouse position.
void comp_set_mouse_position(long x, long y)
{
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

    if ( x<0 ){ x=0; }
    if ( y<0 ){ y=0; }

    if ( x>w ){
        x=w;
    }
    if ( y>h){
        y=h;
    }

// Set
    __new_mouse_x = (long) x;
    __new_mouse_y = (long) y;
}

