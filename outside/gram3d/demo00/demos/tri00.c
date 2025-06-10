// tri00.c

#include "../gram3d.h"


// Our demo window
struct gws_window_d *dw_tri00;

struct gr_triangle_d *tri00_triangle;

// Model's properties.
static int line_size = 40;

static void __draw_triangle(void);


// =================================

static void __draw_triangle(void)
{

// window
    if ((void*)dw_tri00 == NULL)
        return;
    if (dw_tri00->magic != 1234)
        return;

// triangle
    if ((void*)tri00_triangle == NULL)
        return;
    if (tri00_triangle->initialized != TRUE)
        return;

        // clear
        demoClearSurface(dw_tri00,COLOR_BLACK);
        // Draw a lot of triangles.
        //for(j=0; j<max; j++)
        //{
            // translation
            tri00_triangle->p[0].x++;
            tri00_triangle->p[1].x++;
            tri00_triangle->p[2].x++;
            grTriangle3(dw_tri00,tri00_triangle);
        //};

        // flush surface
        demoFlushSurface(dw_tri00);

        rtl_yield();
}

void demoTriangle(void)
{
    __draw_triangle();
}

// Setup demo
void tri00SetupDemo(void)
{

// Model's properties
    line_size = 40;
    // ...

    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

// ---------------
// Create a demo window
    unsigned long w_width = (w >> 1);
    unsigned long w_height = (h >> 1);
    // Centralize
    unsigned long w_left = ((w - w_width) >> 1);
    unsigned long w_top =  ((h - w_height) >> 1);

// ---------------
// Create a demo window
    dw_tri00 = NULL;
    dw_tri00 = 
        (struct gws_window_d *) __create_demo_window(w_left, w_top, w_width, w_height);
    if ((void*) dw_tri00 != NULL)
    {
       if (dw_tri00->magic==1234){
           __demo_window = dw_tri00;
       }
    }

//------------------------------------------

// Create the triangle.
    tri00_triangle = (void *) malloc( sizeof( struct gr_triangle_d ) );
    if ( (void*) tri00_triangle == NULL )
        return;

    tri00_triangle->used = TRUE;
    tri00_triangle->magic = 1234;
    tri00_triangle->initialized = FALSE;

// down
    tri00_triangle->p[0].x = 0; 
    tri00_triangle->p[0].y = 0;
    tri00_triangle->p[0].z = 0;
    tri00_triangle->p[0].color = COLOR_RED;
// right
    tri00_triangle->p[1].x = (line_size>>1); 
    tri00_triangle->p[1].y = (line_size>>1);
    tri00_triangle->p[1].z =  0;
    tri00_triangle->p[1].color = COLOR_GREEN;
// left
    tri00_triangle->p[2].x = -(line_size>>1);
    tri00_triangle->p[2].y =  (line_size>>1);
    tri00_triangle->p[2].z =   0;
    tri00_triangle->p[2].color = COLOR_BLUE;

    tri00_triangle->initialized = TRUE;
}

