// curve00.c

#include "../gram3d.h"

// Model's properties
static int __curve00_position = 0;

static void __draw_curve00(void);


// ----------------------------------


static void __draw_curve00(void)
{

}

// Curva e string.
void demoCurve(void)
{

    __curve00_position++;
    if (__curve00_position >= 10)
        __curve00_position=0;

    //validate_background();
    //demoClearSurface(NULL,GRCOLOR_LIGHTYELLOW);

    // IN: position, modelz
    __draw_demo_curve1(__curve00_position,0);
    //__draw_demo_curve1(i,i*4);

    //invalidate_background();
    //demoFlushSurface(NULL);      // flush surface
}

// Setup.
// Called once.
void curve00SetupDemo(void)
{
// Model's properties
    __curve00_position = 0;

    // IN: ?, near, far
    gr_depth_range( CurrentProjection, 0, 100 );

}