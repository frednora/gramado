// transf.c
// Transformation support.

#include "../gwsint.h"


// Device hotspot.
// 'Principle Point'.
static unsigned long HotSpotX=0;
static unsigned long HotSpotY=0;


void transf_setup_hotspot(unsigned long x, unsigned long y)
{
    HotSpotX = (unsigned long) x;
    HotSpotY = (unsigned long) y;
}


// Purpose:
// Projects a 3D point (x, y, z) in "view space" to a 2D screen-space coordinate (res_x, res_y) using 
// a simple hand-made oblique projection, with optional left-hand or right-hand rules.
// Summary:
// This function implements a Cavalier Oblique projection (simple 3D-to-2D), 
// which is easy to visualize and code. The z component is mapped to both X and Y for the oblique effect, 
// and the handedness parameter (left_hand) controls the projection style.
// worker: Used by grPlot0()
// Projection Transform - Perspective Projection
// Projection Transform - Orthographic Projection
// The x and y coordinates (in the range of -1 to +1) 
// represent its position on the screen, and the z value
// (in the range of 0 to 1) represents its depth, 
// i.e., how far away from the near plane.
// Transforma no 'world space' para o 'view port'.
// The world space, is common to all the objects. 
// But each object can have your own 'local space'.
// #
// Not standard ortographic projection.
void 
__transform_from_modelspace_to_screespace(
    int *res_x,
    int *res_y,
    int _x, int _y, int _z,
    int use_z_translation )
{

// (0,0) represents the top/left corner in a 2d screen.
// The center of the screen in 2d is the hotspot.
// (0,0,0) represents the center of the screen in 3d
// (0,0,0) in 3d is also the hotspot.

// 3d
// save parameters. (++)
    int x  = (int) _x;  //1
    int y  = (int) _y;  //2
    //int x2 = (int) _y;  //3 #crazy
    int z  = (int) _z;  //4
    int hotspotx = (int) (HotSpotX & 0xFFFFFFFF);
    int hotspoty = (int) (HotSpotY & 0xFFFFFFFF);
// 2d:
// final result.
    int X=0;
    int Y=0;
    //int UseZTranslation = TRUE;
    //int UseZTranslation = FALSE;
    int UseZTranslation = (int) use_z_translation;
    // Register z value into the z buffer.
    //int RegisterZValue=FALSE;

// The world space.
// (HotSpotX,HotSpotY,0)
// This is the origin of the 'world space'.
// model space.
// Been the reference for all the 'object spaces'.

// ===================================================
// X::

// --------------------
// z maior ou igual a zero.
//    |
//    ----
//
    if (z >= 0)
    {
        // x positivo, para direita.
        if (x >= 0 ){
            X = (int) ( hotspotx + x );
        }
        // x negativo, para esquerda.
        if (x < 0 ){ x = abs(x);   
            X = (int) ( hotspotx - x );
        }
        goto done;
    }

// --------------------
// z negativo
//  _
//   |
//
    if (z < 0)
    {
        // x positivo, para direita.
        if (x >= 0){
            X = (int) (hotspotx + x);
        }
        // x negativo, para esquerda.
        if (x < 0){  x = abs(x); 
            X = (int) (hotspotx - x);
        }
        goto done;
    }

done:

// ===================================================
// Y::
     // y positivo, para cima.
     if ( y >= 0 ){
         Y = (int) ( hotspoty - y );
     }
     // y negativo, para baixo
     if ( y < 0 ){ y = abs(y);
         Y = (int) ( hotspoty + y );
     }

// ===================================================
// Z::
    if (UseZTranslation == TRUE)
    {
        // z é positivo para todos os casos 
        // onde z é maior igual a 0.
        if(z >= 0){ 
            X = (X + z); 
            Y = (Y - z); 
        }
        // z é módulo para todos os casos 
        // em que z é menor que 0.
        if(z < 0){ z = abs(z);
            X = (X - z); 
            Y = (Y + z); 
        }
    }

// ===================================================
// Return the values
    if ((void*) res_x != NULL){
        *res_x = (int) X;
    }
    if ((void*) res_y != NULL){
        *res_y = (int) Y;
    }

    return;
}


