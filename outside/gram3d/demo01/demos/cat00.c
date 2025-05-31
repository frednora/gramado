// cat00.c

#include "../gram3d.h"

void demoCat(void)
{
    register int i=0;
    int j=0;
    int count = 8;
    int scale_max = 100;

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

// Loop
    while (count>0)
    {
        for (i=0; i<scale_max; i++)
        {
            validate_background();                 //begin paint
            demoClearSurface(dw,GRCOLOR_LIGHTCYAN);   // Clear surface
            // IN: eye scale, x,y,z
            __draw_cat(1,0,0,i);
            demoFlushSurface(dw);
            //invalidate_background();               // end paint
            //gr_dc_refresh_screen(gr_dc);

            // good for qemu,
            //for (j=0; j<8; j++){ gwssrv_yield();}  // Delay
            // good for kvm,
            //for (j=0; j<80; j++){ gwssrv_yield();}  // Delay
            
            //rtl_yield();
        };

        count--;
    };
}
