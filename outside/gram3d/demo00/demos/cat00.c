// cat00.c

#include "../gram3d.h"


struct cat_demo_info_d  CatDemoInfo;

struct gws_window_d *dw;


static void __drawCatScene(void);

// -------------------------------------



// Draw
// Local worker.
static void __drawCatScene(void)
{

    if ((void*) dw == NULL)
        return;
    if (dw->magic != 1234)
        return;

// ---------------------------------
    //register int i=0;
    //int j=0;
    //int count = 8;
    //int scale_max = 100;


// Loop
    //while (count>0)
    //{
        //for (i=0; i<scale_max; i++)
        //{
            //validate_background();                  //begin paint
            demoClearSurface(dw,GRCOLOR_LIGHTCYAN);   // Clear surface
            

            // IN: eye scale, x,y,z
            __draw_cat(
                1, 
                0, 0, CatDemoInfo.z_counter );

            demoFlushSurface(dw);

            //invalidate_background();               // end paint
            //gr_dc_refresh_screen(gr_dc);

            // good for qemu,
            //for (j=0; j<8; j++){ gwssrv_yield();}  // Delay
            // good for kvm,
            //for (j=0; j<80; j++){ gwssrv_yield();}  // Delay
            
            //rtl_yield();
        //};

        //count--;
    //};
}


void demoCat(void)
{
    __drawCatScene();

// Increase the scene coutner.
    CatDemoInfo.z_counter++;

// Round.
    if (CatDemoInfo.z_counter >= CatDemoInfo.z_max)
        CatDemoInfo.z_counter = 0;
}

// Setup
// Called once
void cat00SetupDemo(void)
{
    CatDemoInfo.initialized = FALSE;

    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

// ---------------
// Create a demo window
    unsigned long w_width = (w >> 1);
    unsigned long w_height = (h >> 1);
    // Centralize
    unsigned long w_left = ((w - w_width) >> 1);
    unsigned long w_top =  ((h - w_height) >> 1);

    dw = NULL;
    dw = 
        (struct gws_window_d *) __create_demo_window(
                                    w_left, w_top, w_width, w_height );

    if ((void*) dw != NULL)
    {
       if (dw->magic == 1234){
           __demo_window = dw;
       }
    }
//---------------------

// depth clipping
// IN: projection, znear, zfar.
    gr_depth_range( CurrentProjection, 0, 100 );

// The camera for the cat.
// 'int' values.
// IN: Position vector, upview vector, lookat vector.
    camera ( 
        0,0,0,
        0,0,0,
        0,0,0 );

// Setup model
// IN: eyes, whiskers, mouth
// See: models.c
    __setupCatModel(TRUE,TRUE,TRUE);

    CatDemoInfo.z_counter = 0;
    CatDemoInfo.z_max = 100;
    CatDemoInfo.initialized = TRUE;
}
