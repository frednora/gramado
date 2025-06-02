// cat.c
// Created by Fred Nora.

#include "../../gwsint.h"

// See: demos.h
struct cat_model_d CatModel;

static void __draw_cat(int eye_scale, int cat_z);


// ============================

//worker
static void __draw_cat(int eye_scale, int cat_z)
{
    int model_z = (int) cat_z;

//---
    // head
    // IN: x, y, r, color, z
    plotCircleZ ( 
        0,    //x
        12,   //y
        25,   //r
        GRCOLOR_LIGHTBLACK,  //color 
        model_z );   // z 

    // eyes
    if ( CatModel.eyesVisible == TRUE )
    {
        plotCircleZ ( 
            -10, 
            20, 
            1+eye_scale, 
            GRCOLOR_LIGHTBLACK, 
            model_z );  //z 
        plotCircleZ ( 
            10, 
            20, 
            1+eye_scale, 
            GRCOLOR_LIGHTBLACK, 
            model_z );  //z 
    }

    // whiskers
    if ( CatModel.whiskersVisible == TRUE ){
        // =
        plotLine3d ( 
            -40, 8, model_z, 
            -4,  5, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            -40, 5, model_z, 
            -4,  4, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            -40, 2, model_z, 
            -4,  3, model_z, 
            GRCOLOR_LIGHTBLACK); 

        // =
        plotLine3d ( 
            4,  5, model_z, 
            40, 8, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            4,  4, model_z, 
            40, 5, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            4,  3, model_z, 
            40, 2, model_z, 
            GRCOLOR_LIGHTBLACK); 
    }

    // mouth
    if ( CatModel.mouthVisible == TRUE ){
        plotLine3d ( 
            -10, -2, model_z, 
             10, -2, model_z, 
             GRCOLOR_LIGHTBLACK); 
    }
//---
}

void setupCatModel(int eyes, int whiskers, int mouth )
{
    CatModel.eyesVisible     = eyes;
    CatModel.whiskersVisible = whiskers;
    CatModel.mouthVisible    = mouth;
}

// #todo
// We can use this for 'screen saver'
// and break the loop whe the user hit a key
// or move the mouse.
void demoCat(void)
{
    register int i=0;
    int j=0;
    //int count = 8;
    static int count=4;
    int scale_max = 100;

// Changing the vire for the current projection.
    gr_depth_range(
        CurrentProjection,   // projection
        0,                   // zNear
        80 );                // zFar

// Setup model
// eyes, whiskers, mouth
    setupCatModel(TRUE,TRUE,TRUE);

// Loop
    while (count>0)
    {
        for (i=0; i<scale_max; i++)
        {
            validate_background();                 //begin paint
            demoClearSurface(GRCOLOR_LIGHTCYAN);   // Clear surface
            // IN: scale, z
            //__draw_cat(i,i);                         // IN: eye scale
            __draw_cat(1,i);
            //invalidate_background();               // end paint
            demoFlushSurface();                  // Flush surface

            // good for qemu,
            //for (j=0; j<8; j++){ rtl_yield();}  // Delay
            // good for kvm,
            for (j=0; j<80; j++)
            {
                rtl_yield();  //#todo: Do not use this delay for now.
            }  
        };
        
        count--;
    };
}

