// curve00.c

#include "../gram3d.h"


static int IntroStep=0;

// Model's properties
static int __curve00_position = 0;

static void __draw_curve00(void);


// ----------------------------------


static void __draw_curve00(void)
{

}

// Curve and string
void intro00DrawScene(void)
{

// The intro finished
// Go to hub world
    if (IntroStep > 3)
    {
        current_level = LEVEL_HUB;
        exit_level = TRUE;
        return;
    }

    __curve00_position++;
    if (__curve00_position >= 10)
    {
        __curve00_position = 0;
        
        // One step was concluded
        IntroStep++;
    }

    //validate_background();
    //demoClearSurface(NULL,GRCOLOR_LIGHTYELLOW);

    // IN: position, modelz
    __draw_demo_curve1(__curve00_position, 0);
    //__draw_demo_curve1(i,i*4);

    //invalidate_background();
    //demoFlushSurface(NULL);      // flush surface
}

// Setup.
// Called once.
void intro00SetupDemo(void)
{
// Model's properties
    __curve00_position = 0;

    // IN: ?, near, far
    gr_depth_range( CurrentProjection, 0, 100 );
}
