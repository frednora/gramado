// tri00.c

#include "../gram3d.h"

void demoTriangle(void)
{

// ---------------
// Create a demo window
    struct gws_window_d *dw;
    dw = NULL;
    dw = (struct gws_window_d *) __create_demo_window(8,8,200,140);
    if( (void*) dw != NULL )
    {
       if(dw->magic==1234){
           __demo_window = dw;
       }
    }

//------------------------------------------
    struct gr_triangle_d *triangle;
    int line_size = 40;

// Create the triangle.
    triangle = (void *) malloc( sizeof( struct gr_triangle_d ) );
    if ( (void*) triangle == NULL )
        return;
    triangle->used = TRUE;
    triangle->magic = 1234;
    triangle->initialized = FALSE;
// down
    triangle->p[0].x = 0; 
    triangle->p[0].y = 0;
    triangle->p[0].z = 0;
    triangle->p[0].color = COLOR_RED;
// right
    triangle->p[1].x = (line_size>>1); 
    triangle->p[1].y = (line_size>>1);
    triangle->p[1].z =  0;
    triangle->p[1].color = COLOR_GREEN;
// left
    triangle->p[2].x = -(line_size>>1);
    triangle->p[2].y =  (line_size>>1);
    triangle->p[2].z =   0;
    triangle->p[2].color = COLOR_BLUE;

    triangle->initialized = TRUE;

    int i=0;
    int j=0;
    int max = 150;
    //int T=0;

    for(i=0; i<max; i++)
    {
        // clear
        demoClearSurface(dw,COLOR_BLACK);
        // Draw a lot of triangles.
        //for(j=0; j<max; j++)
        //{
            // translation
            triangle->p[0].x++;
            triangle->p[1].x++;
            triangle->p[2].x++;
            grTriangle3(dw,triangle);
        //};

        // flush surface
        demoFlushSurface(dw);
        rtl_yield();
        //T++;
    };
}

