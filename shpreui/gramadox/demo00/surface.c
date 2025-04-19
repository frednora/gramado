
// surface.c

#include "gram3d.h"


// Start surface
void 
demoClearSurface(
    struct gws_window_d *clipping_window, 
    unsigned int color)
{
    // #todo
    // We can do this for more resolutions. 

// Se a clipping window é válida.
// ONLY ONE TYPE FOR NOW.
    if ( (void*) clipping_window != NULL )
    {
        if(clipping_window->magic==1234)
        {
            if (clipping_window->type == WT_SIMPLE)
            {
                rectBackbufferDrawRectangle ( 
                    clipping_window->absolute_x, 
                    clipping_window->absolute_y, 
                    clipping_window->width, 
                    clipping_window->height,
                    color, 1, 0 );
                return;
            }
        }
    }

// device context
    if ( (void*) gr_dc != NULL )
    {
        if (gr_dc->magic == 1234)
        {
            if( gr_dc->initialized == TRUE )
            {
                rectBackbufferDrawRectangle ( 
                    gr_dc->absolute_x, 
                    gr_dc->absolute_y, 
                    gr_dc->width, 
                    gr_dc->height,
                    color, 1, 0 );
                
                return;
            }
        }
    }

    unsigned int fail_color = COLOR_RED;

// Limpa na resolução 320x200
// IN: l,t,w,h,color,fill,rop
    rectBackbufferDrawRectangle ( 
           0, 0, 320, 200, 
           fail_color, 1, 0 );
}

// #todo
// We can do this for more resolutions. 
// mostra na resolução 320x200
void demoFlushSurface(struct gws_window_d *clipping_window)
{
// Flush the surface.


// Se a clipping window é válida.
// ONLY ONE TYPE FOR NOW.
    if ( (void*) clipping_window != NULL )
    {
        if (clipping_window->magic==1234)
        {
            if (clipping_window->type == WT_SIMPLE){
                gws_show_window_rect(clipping_window);
                return;
            }
        }
    }

// default dc
    if ( (void*) gr_dc != NULL )
    {
        if (gr_dc->magic == 1234)
        {
            if( gr_dc->initialized == TRUE )
            {
                gws_refresh_rectangle(
                    gr_dc->absolute_x, 
                    gr_dc->absolute_y, 
                    gr_dc->width, 
                    gr_dc->height );

                return;
            }
        }
    }

    gws_refresh_rectangle(
        0, 0, 320, 200 );
}


// Start surface.
// only the working area.
void demoClearWA(unsigned int color)
{
// IN: l,t,w,h,color,fill,rop
    rectBackbufferDrawRectangle ( 
           WindowManager.wa_left, 
           WindowManager.wa_top, 
           WindowManager.wa_width, 
           WindowManager.wa_height, 
           color, 
           1,   //fill 
           0 ); //rop
}

// global
// We can use a NULL window.
void gramado_clear_surface(struct gws_window_d *clipping_window, unsigned int color)
{
    demoClearSurface(clipping_window,color);
}

// global
void gramado_flush_surface(struct gws_window_d *clipping_window)
{
    demoFlushSurface(clipping_window);
}





