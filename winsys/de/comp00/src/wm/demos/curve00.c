// curve00.c
// Created by Fred Nora.

#include "../../ds.h"


static void __draw_demo_curve1(int position, int model_z);

//======================

// IN: ?
static void __draw_demo_curve1(int position, int model_z)
{
    // ??
    int yOffset = (position + position);
    int modelz = (int) model_z;
    //int modelz = 0;

    // line
    //a variaçao de y2 me pareceu certa.
    //IN: ??
    plotQuadBezierSeg ( 
        0,   0,  modelz,          //x0, y0, z0, //ponto inicial
        40,  40, modelz,          //x1, y1, z1, //?
       100,  20+yOffset, modelz,  //x2, y2, z2, //ponto final
       GRCOLOR_LIGHTBLACK );

    //string! char by char
    //IN: x,y,color,c,z
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*0), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'G', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*1), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'R', modelz );   
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*2), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'A', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*3), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'M', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*4), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'A', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*5), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'D', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*6), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'O', modelz );
}


// curva e string.
void demoCurve(void)
{
    register int i=0;
    register int j=0;
    int count=8;

    gr_depth_range(
        CurrentProjection,
        0,      //near
        100 );  //far

    while (count>0){

        count--;

    for (i=0; i<10; i++)
    {
        validate_background();
        demoClearSurface(GRCOLOR_LIGHTYELLOW);

        // IN: position, modelz
        __draw_demo_curve1(i,0);
        //__draw_demo_curve1(i,i*4);

        //invalidate_background();
        demoFlushSurface();          // flush surface
        
        // Delay  
        for (j=0; j<8; j++){ 
            rtl_yield();
        };
    };
    }
}


