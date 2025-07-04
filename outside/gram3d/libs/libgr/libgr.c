// libgr.c

/*
This method is a form of “oblique” or “fake” projection 
(like the Cavalier/Cabinet projections in technical drawing), based on 
simply adding or subtracting z, and is not used for true 3D rendering where perspective is needed.
It was, however, very common for early 2.5D engines and certain kinds of “fake 3D” graphics.
*/

/*
What Are 2.5D Engines?
“2.5D” refers to engines that simulate 3D using 2D techniques, often for performance or hardware reasons.
These engines don’t do full 3D perspective math, but instead use tricks to give a sense of depth.
*/

/*
// ------------------------------------
Projection math in these fake 3D engines.

Commonly, for isometric:
screen_x = origin_x + (x - y) * tile_width/2
screen_y = origin_y + (x + y) * tile_height/2

For oblique/cavalier (sometimes used for “fake” 3D):
screen_x = origin_x + x + z * kx
screen_y = origin_y + y + z * ky
(where kx/ky are constants, often 1 or 0.5).
*/

/*
Key observation on libgr_transform_from_viewspace_to_screespace(), by Copilot.

What This Resembles:
Oblique Projection (specifically, something like the Cavalier projection).
This is a centuries-old, hand-calculable way to give a 3D effect on a 2D surface, but without perspective.

Classic 2.5D / Retro Game Engines: <<< --- That is cool
Many old-school engines (pre-OpenGL, pre-Direct3D) used similar math for quick-and-dirty 3D, 
especially for wireframes or simple 3D boxes.

What’s Standard Today <<< --- That is complex and not so cool
Most modern engines use homogeneous coordinates and 
projection matrices (orthographic or perspective) for 3D-to-2D projection.
Even simple engines typically use a matrix for this purpose, 
for flexibility and to support features like field-of-view, aspect ratio, and 
camera transformations.
*/


#include "include/libgr.h"

int coisolibgr=0;

// -------------------------------------------------------

// Purpose:
// Transforms a 2D coordinate from the "view space" to "screen space," considering 
// a screen center (hotspot) as the origin.
// #
// The viewspace is the view considering the camera's point of view.
// Parameter Handling
// res_x, res_y: Output pointers for screen-space coordinates.
// _x, _y: Input coordinates in view space.
// _hotspotx, _hotspoty: The center of the screen in screen coordinates.

int 
libgr_transform_to_screespace(
    int *res_x, int *res_y,
    int _x, int _y, 
    int _hotspotx, int _hotspoty )
{
// Summary:
// This function maps a point (x, y) from a centered coordinate system 
// to the regular screen coordinates, with a chosen origin (hotspot).

// Convert Inputs

// 3d
// save parameters. (++)
    int x  = (int) _x;  //1
    int y  = (int) _y;  //2

// The given hotspot.
// The center os our surface.
    int hotspotx = (int) (_hotspotx & 0xFFFFFFFF);
    int hotspoty = (int) (_hotspoty & 0xFFFFFFFF);

// 2d:
// final result.
    int X=0;
    int Y=0;

    // Register z value into the z buffer.
    //int RegisterZValue=FALSE;

// --------------------
// Transform the X Coordinate
// If x >= 0: Move right from the hotspot.
// If x < 0:  Move left from the hotspot.

    // x positivo, para direita.
    if (x >= 0 ){
        X = (int) ( hotspotx + x );
    }
    // x negativo, para esquerda.
    if (x < 0 ){ x = abs(x);   
        X = (int) ( hotspotx - x );
    }

//    goto done;
// --------------------------------------
//done:

// ===================================================
// Y::
// Transform the Y Coordinate
// If y >= 0: Move up from the hotspot (screen Y axis usually grows downward).
// If y < 0:  Move down from the hotspot.

    // y positivo, para cima.
    if ( y >= 0 ){
        Y = (int) ( hotspoty - y );
    }
    // y negativo, para baixo
    if ( y < 0 ){ y = abs(y);
        Y = (int) ( hotspoty + y );
    }

// ===================================================
// Return values:
// Write Back the Result
// Store calculated values in *res_x and *res_y if output is valid.
// Returns 0 on success, -1 on failure (null pointers).

    // fail
    if ( (void*) res_x == NULL ){ return (int) -1; }
    if ( (void*) res_y == NULL ){ return (int) -1; }

    *res_x = (int) X;
    *res_y = (int) Y;

    // ok
    return 0;

}

// The function libgr_transform_from_viewspace_to_screespace() 
// transforms 3D coordinates (x, y, z) 
// from the view space to 2D screen space coordinates (x, y). 
// This transformation is done without using a perspective matrix, 
// utilizing either a left-hand or right-hand coordinate system.

// Transforme from the (x,y,z) coordinates of the 'view space'
// to the (x,y) coordinates of the 2d screen space.
// Hand-made. No matrix.
// Using the left-hand style. The same found in Direct3D.
// Not normalized screen.
// Called by grPlot0().
// (This is a not standard method).
// (0,0) represents the top/left corner in a 2D screen.
// The center of the screen in 2D is the hotspot.
// (0,0,0) represents the center of the screen in 3D viewspace
// (0,0,0) in 3D is also the hotspot.
// OUT: 
// Return the 2D screen coordinates in res_x and res_y.

// z in 45 degree.
// Isso é uma projeção quando z esta inclinado em 45 graus.
// #
// Trasformation for Cavalier Oblique Drawings.
// It uses full depth.

/*
Example Usage
This function is used in your 3D demos to convert 3D coordinates of objects, 
such as spinning cubes, into 2D coordinates for rendering on the screen. 
It helps in visualizing the 3D objects on a 2D display without using complex matrix transformations.
*/

// The function libgr_transform_from_viewspace_to_screespace() is used in the grPlot0 function 
// in xgames/aurora/demo00/grprim.c

/*
Parameters
int *res_x, int *res_y: 
    Pointers to store the resulting screen space x and y coordinates.
int _x, _y, _z: 
    The input 3D coordinates in the view space.
int left_hand: 
    A flag indicating whether to use a left-hand coordinate system (TRUE) or 
    right-hand coordinate system (FALSE).
int _hotspotx, _hotspoty: 
    The x and y coordinates of the hotspot, which is the center of the screen in 2D space.
*/

/*
Parameter Handling
res_x, res_y: Output pointers.
_x, _y, _z: Input 3D coordinates (view space).
left_hand: Use left-hand (TRUE) or right-hand (FALSE) coordinate system.
_hotspotx, _hotspoty: The screen center.
*/

// #
// The viewspace is the view considering the camera's point of view.
int 
libgr_transform_from_viewspace_to_screespace(
    int *res_x, int *res_y,
    int _x, int _y, int _z,
    int left_hand,
    int _hotspotx, int _hotspoty )
{
// Purpose:
// Projects a 3D point (x, y, z) in "view space" to a 2D screen-space coordinate (res_x, res_y) using 
// a simple hand-made oblique projection, with optional left-hand or right-hand rules.
// Summary:
// This function implements a Cavalier Oblique projection (simple 3D-to-2D), 
// which is easy to visualize and code. The z component is mapped to both X and Y for the oblique effect, 
// and the handedness parameter (left_hand) controls the projection style.

// Convert Inputs

// 3d
// save parameters. (++)
    int x  = (int) _x;  //1
    int y  = (int) _y;  //2
    //int x2 = (int) _y;  //3 #crazy
    int z  = (int) _z;  //4

// The given hotspot.
// The center os our surface.
    int hotspotx = (int) (_hotspotx & 0xFFFFFFFF);
    int hotspoty = (int) (_hotspoty & 0xFFFFFFFF);

// 2d:
// final result.
    int X=0;
    int Y=0;

    // Register z value into the z buffer.
    //int RegisterZValue=FALSE;

// The world space.
// (HotSpotX,HotSpotY,0)
// This is the origin of the 'world space'.
// model space.
// Been the reference for all the 'object spaces'.

// ===================================================
// X::
// Project X (2D X calculation)


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
// Project Y (2D Y calculation)

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
// Adjust for Z using the chosen coordinate system.
// Posição canônica do eixo z.
// Usado para projeção em 2D depois de feita as transformações.

    // LEFT-HAND: Left-hand system
    if (left_hand == TRUE)
    {
        // z é positivo para todos os casos 
        // onde z é maior igual a 0.
        // Move right and up by z
        if (z >= 0)
        { 
            X = (X + z);  //para direita
            Y = (Y - z);  //para cima
        }
        // z é módulo para todos os casos 
        // em que z é menor que 0.
        //  Move left and down by |z|
        if (z < 0){ z = abs(z);
            X = (X - z);   // para esquerda
            Y = (Y + z);   // para baixo
        }
    }

    // RIGHT-HAND: Right-hand system
    if (left_hand != TRUE)
    {
        // z é positivo para todos os casos 
        // onde z é maior igual a 0.
        // Move left and down by z
        if (z >= 0)
        { 
            X = (X - z);  //para esquerda
            Y = (Y + z);  //para baixo
        }
        // z é módulo para todos os casos 
        // em que z é menor que 0.
        // Move right and up by |z|
        if (z < 0){ z = abs(z);
            X = (X + z);   // para esquerda
            Y = (Y - z);   // para baixo
        }
    }

// ===================================================
// Return values:
// Write Back the Result
// Store calculated values in *res_x and *res_y if output is valid.
// Returns 0 on success, -1 on failure (null pointers).

    // fail
    if ( (void*) res_x == NULL ){ return (int) -1; }
    if ( (void*) res_y == NULL ){ return (int) -1; }

    *res_x = (int) X;
    *res_y = (int) Y;

    // ok
    return 0;
}

// #ugly
void multiply4 (int mat1[4][4], int mat2[4][4], int res[4][4])
{
    register int i=0; 
    register int j=0; 
    register int k=0;
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            res[i][j] = 0;
            // slow
            for (k = 0; k < 4; k++){
                res[i][j] += mat1[i][k] * mat2[k][j];
            };
        };
    };
}


// Fibonacci Series using Recursion 
// ??: slow.
int fib (int n)
{ 
    register int Copy = n;
    int a=0;
    int b=0;

    if (Copy <= 1){ return Copy; }

    a = fib(Copy - 1); 
    b = fib(Copy - 2);

    return (int) (a+b);
} 

/*
// print fibonacci
void __print_fib(int max);
void __print_fib(int max)
{
    int i=0;
    int a=0;
    int b=1;
    int c;
    
    if (max<0)
        return;
    
    while(a<max)
    {
        printf("%d\n,a");
        c = a+b;
        a = b;
        b = c;
    };
}
*/


//#todo: Explain it better.
unsigned int 
interpolate_color(
    unsigned int color1, 
    unsigned int color2, 
    unsigned int fraction )
{
    unsigned int final_color=0;
    unsigned char r1 = (unsigned char) (color1 >> 16) & 0xff;
    unsigned char r2 = (unsigned char) (color2 >> 16) & 0xff;
    unsigned char g1 = (unsigned char) (color1 >> 8) & 0xff;
    unsigned char g2 = (unsigned char) (color2 >> 8) & 0xff;
    unsigned char b1 = (unsigned char) color1 & 0xff;
    unsigned char b2 = (unsigned char) color2 & 0xff;

    final_color = 
        (unsigned int) (    
        (unsigned int) ( (r2 - r1) * fraction + r1 ) << 16 | 
        (unsigned int) ( (g2 - g1) * fraction + g1 ) <<  8 | 
        (unsigned int) ( (b2 - b1) * fraction + b1 )
    );
    
    return (unsigned int) final_color;
}

//??
//#todo: Explain it better.
unsigned int invert_color(unsigned int color)
{
    unsigned int Color = (unsigned int) (color ^ 0x00FFFFFF);
    return (unsigned int) Color;
}

// dot product
int dot_product( struct gr_vec3D_d *v1, struct gr_vec3D_d *v2 )
{
// Dot product.
// The dot product describe the 
// relationship between two vectors.
// Positive: Same direction
// negative: Opposite direction
// 0:        Perpendicular.

    int scalar=0;

// Fake perpendicular.
    if ( (void*) v1 == NULL ){ return 0; }
    if ( (void*) v2 == NULL ){ return 0; }

// (x*x + y*y + z*z)
    scalar  = (v1->x * v2->x);
    scalar += (v1->y * v2->y);
    scalar += (v1->z * v2->z);

    return (int) (scalar & 0xFFFFFFFF );
}

// inline?
int gr_triangle_area_int (int base, int height)
{
    return (int) ((base*height) >> 1);
}

int gr_magic_volume (int x, int y, int z)
{
    return (int) (x*y*z);
}

// ??
// wrong? Explain it better.
int gr_magic_area (int x, int y, int z)
{
    int area = 
        (int) ( (2*x*y) +
                (2*y*z) +
                (2*x*z) );

    return (int) area;
}

// Get delta for bhaskara.
// d<0: (negative) "Raiz de número negativo em Baskara"
// d=0: (null)     duas raizes reais iguais.
// d>0: (positive) duas raizes reais diferentes. (Intersection)
int gr_discriminant_int(int a, int b, int c)
{
// Discriminant: Delta da função em bhaskara.
    int Discriminant = (int) ((b*b) - (4*a*c));
    return (int) Discriminant;
}

int 
gr_find_obj_height_int ( 
    int *obj_height, int obj_distance,
    int img_height, int img_distance )
{
// #todo: This is a work in progress.
// ih/_oh      = (id/od)
// ih         = _oh*(id/od)
// ih/(id/od) = _oh

// ------------
// Razão entre a distância da imagem e a distância do objeto.
    if (obj_distance == 0){
        return -1;  //fail
    }
    int tmp = (int) (img_distance/obj_distance);

// ------------
// Altura da imagem dividida pela
// razão entre a distância da imagem e a distância do objeto.
    if (tmp==0){
        return -1;  //fail
    }
    int resOH = (int) (img_height / tmp);

// ------------
// done:
// Return the object height
// Check pointer validation

    if( (int*) obj_height == NULL){
        return -1;  //fail
    }
    
    *obj_height = (int) resOH;
    
    return 0; //ok
}

int 
gr_find_img_height_int ( 
    int obj_height, int obj_distance,
    int *img_height, int img_distance )
{
// #todo: This is a work in progress.
// _ih/oh      = (id/od)
// _ih         = oh*(id/od)

// ------------
// Razão entre a distância da imagem e a distância do objeto.
    if (obj_distance == 0){
        return -1;  //fail
    }
    int tmp = (int) (img_distance/obj_distance);

    int resIH = (int) (obj_height*tmp);

// ------------
// done:
// Return the image height
// Check pointer validation

    if ( (int*) img_height == NULL){
        return -1;  //fail
    }

    *img_height = (int) resIH;

    return 0; //ok
}

void gr_scale_vec( struct gr_vec3D_d *v, int scale )
{
    if ( (void*) v == NULL )
        return;

    v->x *= scale;
    v->y *= scale;
    v->z *= scale;
}

// Scaling: Inflate cube.
int 
xxxInflateCubeZ ( 
    struct gr_cube_d *cube, 
    int value )
{
    if ( (void*) cube == NULL ){
        return (-1);
    }

    //int value = z;
    //int value = z*2;

// South points =====================================
    cube->p[0].x = (cube->p[0].x - value);
    cube->p[0].y = (cube->p[0].y + value);
    cube->p[0].z = (cube->p[0].z - value);

    cube->p[1].x = (cube->p[1].x + value);
    cube->p[1].y = (cube->p[1].y + value);
    cube->p[1].z = (cube->p[1].z - value);

    cube->p[2].x = (cube->p[2].x + value);
    cube->p[2].y = (cube->p[2].y - value);
    cube->p[2].z = (cube->p[2].z - value);

    cube->p[3].x = (cube->p[3].x - value);
    cube->p[3].y = (cube->p[3].y - value);
    cube->p[3].z = (cube->p[3].z - value);

// North points ================================
    cube->p[4].x = (cube->p[4].x - value);
    cube->p[4].y = (cube->p[4].y + value);
    cube->p[4].z = (cube->p[4].z + value);

    cube->p[5].x = (cube->p[5].x + value);
    cube->p[5].y = (cube->p[5].y + value);
    cube->p[5].z = (cube->p[5].z + value);

    cube->p[6].x = (cube->p[6].x + value);
    cube->p[6].y = (cube->p[6].y - value);
    cube->p[6].z = (cube->p[6].z + value);

    cube->p[7].x = (cube->p[7].x - value);
    cube->p[7].y = (cube->p[7].y - value);
    cube->p[7].z = (cube->p[7].z + value);

    return 0;
}

// Scaling: Deflate cube.
int 
xxxDeflateCubeZ ( 
    struct gr_cube_d *cube, 
    int value )
{
    if ( (void*) cube == NULL ){
        return -1;
    }

// South points ==========================
    cube->p[0].x = (cube->p[0].x + value);
    cube->p[0].y = (cube->p[0].y - value);
    cube->p[0].z = (cube->p[0].z + value);

    cube->p[1].x = (cube->p[1].x - value);
    cube->p[1].y = (cube->p[1].y - value);
    cube->p[1].z = (cube->p[1].z + value);

    cube->p[2].x = (cube->p[2].x - value);
    cube->p[2].y = (cube->p[2].y + value);
    cube->p[2].z = (cube->p[2].z + value);

    cube->p[3].x = (cube->p[3].x + value);
    cube->p[3].y = (cube->p[3].y + value);
    cube->p[3].z = (cube->p[3].z + value);

// North points =========================
    cube->p[4].x = (cube->p[4].x + value);
    cube->p[4].y = (cube->p[4].y - value);
    cube->p[4].z = (cube->p[4].z - value);

    cube->p[5].x = (cube->p[5].x - value);
    cube->p[5].y = (cube->p[5].y - value);
    cube->p[5].z = (cube->p[5].z - value);

    cube->p[6].x = (cube->p[6].x - value);
    cube->p[6].y = (cube->p[6].y + value);
    cube->p[6].z = (cube->p[6].z - value);

    cube->p[7].x = (cube->p[7].x + value);
    cube->p[7].y = (cube->p[7].y + value);
    cube->p[7].z = (cube->p[7].z - value);

    return 0;
}












