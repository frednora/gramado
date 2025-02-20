
// models.c

#include "gram3d.h"

struct cat_model_d  CatModel;

// The terrain is a cube.
struct cube_model_d *terrain;


//---------------------------------------------------------------

void __setupCatModel(int eyes, int whiskers, int mouth )
{
    CatModel.eyesVisible     = eyes;
    CatModel.whiskersVisible = whiskers;
    CatModel.mouthVisible    = mouth;
}

// Worker
void __draw_cat(int eye_scale, int cat_x, int cat_y, int cat_z)
{
    // object window
    struct gws_window_d *ow;
    ow = NULL;
    //#todo
    // use the demo window if it is possible
    if( (void*) __demo_window != NULL )
    {
       if (__demo_window->magic==1234){
           ow = __demo_window;
       }
    }

// model
    int model_x = (int) cat_x;
    int model_y = (int) cat_y;
    int model_z = (int) cat_z;
    int model_radius = 25;

// eyes
    int eye1_x = model_x -10;
    int eye1_y = model_y +20;
    int eye2_x = model_x +10;
    int eye2_y = model_y +20;
    int eye_radius = (int) (1*eye_scale);
//---

// head
// IN: window,x,y,r,color,z
    grCircle3 ( 
        ow,
        model_x + 0,
        model_y + 12,
        model_radius,
        GRCOLOR_LIGHTBLACK,
        model_z );

// eyes
    if ( CatModel.eyesVisible == TRUE )
    {
        grCircle3 ( 
            ow,
            eye1_x, 
            eye1_y, 
            eye_radius, 
            GRCOLOR_LIGHTBLACK, 
            model_z );
        grCircle3 ( 
            ow,
            eye2_x, 
            eye2_y, 
            eye_radius, 
            GRCOLOR_LIGHTBLACK, 
            model_z );
    }

// whiskers
    if ( CatModel.whiskersVisible == TRUE ){
        // =
        plotLine3d ( 
            ow,
            model_x -40, model_y +8, model_z, 
            model_x -4,  model_y +5, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            ow,
            model_x -40, model_y +5, model_z, 
            model_x -4,  model_y +4, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            ow,
            model_x -40, model_y +2, model_z, 
            model_x -4,  model_y +3, model_z, 
            GRCOLOR_LIGHTBLACK); 

        // =
        plotLine3d ( 
            ow,
            model_x + 4,  model_y +5, model_z, 
            model_x + 40, model_y +8, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            ow,
            model_x + 4,  model_y +4, model_z, 
            model_x + 40, model_y +5, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            ow,
            model_x + 4,  model_y +3, model_z, 
            model_x + 40, model_y +2, model_z, 
            GRCOLOR_LIGHTBLACK); 
    }

// mouth
    if ( CatModel.mouthVisible == TRUE )
    {
        plotLine3d ( 
            ow,
            model_x -10, model_y -2, model_z, 
            model_x +10, model_y -2, model_z, 
             GRCOLOR_LIGHTBLACK); 
    
        unveil_camera ( -10, -2, model_z );
    }
//---
}


// IN: ?
void __draw_demo_curve1(int position, int model_z)
{
    int yOffset = (position + position);
    //int modelz = 0;
    int modelz = (int) model_z;

// line
// a variaçao de y2 me pareceu certa.
// IN: ??
    
    plotQuadBezierSeg ( 
        0,   0,  modelz,          //x0, y0, z0, //ponto inicial
        40,  40, modelz,          //x1, y1, z1, //?
       100,  20+yOffset, modelz,  //x2, y2, z2, //ponto final
       GRCOLOR_LIGHTBLACK );

// String! char by char.
// IN: x,y,color,c,z

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




void drawRectangle0(float modelz)
{
    struct gr_rectangleF3D_d r;

    r.p[0].x = (float) -0.02f;  
    r.p[0].y = (float)  0.02f;  
    r.p[0].z = (float) modelz;  
    r.p[0].color = COLOR_WHITE;

    r.p[1].x = (float)  0.02f;  
    r.p[1].y = (float)  0.02f;  
    r.p[1].z = (float) modelz;  
    r.p[1].color = COLOR_WHITE;

    r.p[2].x = (float)  0.02f;  
    r.p[2].y = (float) -0.02f;  
    r.p[2].z = (float) modelz;  
    r.p[2].color = COLOR_WHITE;

    r.p[3].x = (float) -0.02f;  
    r.p[3].y = (float) -0.02f;  
    r.p[3].z = (float) modelz;  
    r.p[3].color = COLOR_WHITE;

    drawRectangleF( (struct gr_rectangleF3D_d *) &r );
}





