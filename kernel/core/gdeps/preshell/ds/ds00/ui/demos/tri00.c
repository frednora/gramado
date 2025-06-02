// tri00.c

#include "../../gwsint.h"


void demoTriangle(void)
{
    struct gr_triangle_d *triangle;
    register int i=0;
    int T=0;

    triangle = (void *) malloc( sizeof( struct gr_triangle_d ) );
    if ((void*) triangle == NULL ){
        return;
    }

// down
    triangle->p[0].x = 0; 
    triangle->p[0].y = 0;
    triangle->p[0].z = 0;
    triangle->p[0].color = COLOR_RED;
// right
    triangle->p[1].x = 80; 
    triangle->p[1].y = 80;
    triangle->p[1].z =  0;
    triangle->p[1].color = COLOR_GREEN;
// left
    triangle->p[2].x = -80;
    triangle->p[2].y =  80;
    triangle->p[2].z =   0;
    triangle->p[2].color = COLOR_BLUE;

    for (i=0; i<10; i++)
    {
        // Clear
        demoClearSurface(COLOR_BLACK);
        // Draw
        xxxTriangleZ(triangle);
        // Translation
        triangle->p[0].x++;
        triangle->p[1].x++;
        triangle->p[2].x++;
        // Flush surface
        demoFlushSurface();  
        T++;
    };
}

