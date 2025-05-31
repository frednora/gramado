// curve00.c

#include "../gram3d.h"

// Curva e string.
void demoCurve(void)
{
    register int i=0;
    register int j=0;
    int count=8;

    // IN: ?, near, far
    gr_depth_range( CurrentProjection, 0, 100 );
// Loop
    while (count>0){
    count--;
    for (i=0; i<10; i++)
    {
        validate_background();
        demoClearSurface(NULL,GRCOLOR_LIGHTYELLOW);
        // IN: position, modelz
        __draw_demo_curve1(i,0);
        //__draw_demo_curve1(i,i*4);
        //invalidate_background();
        demoFlushSurface(NULL);      // flush surface
        // delay  
        //for (j=0; j<8; j++){ gwssrv_yield();}
    };
    }
}
