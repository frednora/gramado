
// gr.c
// ring3 3d library.
// #todo
// We gotta create functions to handle
// the surfaces to paint into it.
// For now we are sending requests for each
// single pixel.

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

//#bugbug
//#include <netdb.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

// libgws
//#include "include/connect.h"  
#include "include/gws.h"  

// ==================

// Device hotspot.
static unsigned long HotSpotX=0;
static unsigned long HotSpotY=0;
static unsigned long __savedWidth=0;
static unsigned long __savedHeight=0;

// Window hotspot.
//static unsigned long WindowHotSpotX=0;
//static unsigned long WindowHotSpotY=0;

// ====================

static int 
__transform_from_viewspace_to_screespace(
    int *res_x, int *res_y,
    int _x, int _y, int _z,
    int left_hand,
    int _hotspotx, int _hotspoty );

static void __initializing_hotspot(void);

// ====================

static void __initializing_hotspot(void)
{
    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);
    __savedWidth  = (w & 0xFFFFFFFF);
    __savedHeight = (h & 0xFFFFFFFF);
    HotSpotX = (w>>1);
    HotSpotY = (h>>1);
}

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
        (unsigned int) ( (b2 - b1) * fraction + b1 ) );

    return (unsigned int) final_color;
}

unsigned int invert_color(unsigned int color)
{
    unsigned int Color = (unsigned int) (color ^ 0x00FFFFFF);

    return (unsigned int) Color;
}


void 
gr_MultiplyMatrixVector(
    struct gr_vecF3D_d *i, 
    struct gr_vecF3D_d *o, 
    struct gr_mat4x4_d *m )
{
    o->x = 
        i->x * m->m[0][0] + 
        i->y * m->m[1][0] + 
        i->z * m->m[2][0] + 
        m->m[3][0];

    o->y = 
        i->x * m->m[0][1] + 
        i->y * m->m[1][1] + 
        i->z * m->m[2][1] + 
        m->m[3][1];
    
    o->z = 
        i->x * m->m[0][2] + 
        i->y * m->m[1][2] + 
        i->z * m->m[2][2] + 
        m->m[3][2];

    float w = 
        i->x * m->m[0][3] + 
        i->y * m->m[1][3] + 
        i->z * m->m[2][3] + 
        m->m[3][3];

    if (w != 0.0f)
    {
        o->x /= w; 
        o->y /= w; 
        o->z /= w;
    }
}


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

static int 
__transform_from_viewspace_to_screespace(
    int *res_x, int *res_y,
    int _x, int _y, int _z,
    int left_hand,
    int _hotspotx, int _hotspoty )
{

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
// Posição canônica do eixo z.
// Usado para projeção em 2D depois de feita
// as transformações.

    // LEFT-HAND
    if (left_hand == TRUE)
    {
        // z é positivo para todos os casos 
        // onde z é maior igual a 0.
        if(z >= 0)
        { 
            X = (X + z);  //para direita
            Y = (Y - z);  //para cima
        }
        // z é módulo para todos os casos 
        // em que z é menor que 0.
        if(z < 0){ z = abs(z);
            X = (X - z);   // para esquerda
            Y = (Y + z);   // para baixo
        }
    }

    // RIGHT-HAND
    if (left_hand != TRUE)
    {
        // z é positivo para todos os casos 
        // onde z é maior igual a 0.
        if(z >= 0)
        { 
            X = (X - z);  //para esquerda
            Y = (Y + z);  //para baixo
        }
        // z é módulo para todos os casos 
        // em que z é menor que 0.
        if(z < 0){ z = abs(z);
            X = (X + z);   // para esquerda
            Y = (Y - z);   // para baixo
        }
    }



// ===================================================
// Return values:

    // fail
    if ( (void*) res_x == NULL ){ return (int) -1; }
    if ( (void*) res_y == NULL ){ return (int) -1; }

    *res_x = (int) X;
    *res_y = (int) Y;

    // ok
    return 0;
}





// #todo: We don't have a surface in ring3 yet.
// We can create different functions for ploting pixels,
// 2d, 3d, int, long, float ...
// different flavours of this.
int 
gws_plot0 (
    int fd,
    float x,
    float y,
    float z,
    unsigned int color )
{

// #todo
// We need a surface in ring3.
// The goal is drawing here in ring 3 and not 
// calling the server for each pixel.
// It's just a test for now. :)

// #bugbug
// convert float to long.
    
    return 0;
}


// Plot a 2d point using int.
int 
gws_plot0Int2D (
    int fd,
    int x,
    int y,
    unsigned int color,
    unsigned long rop )
{

// #todo
// We need a surface in ring3.
// The goal is drawing here in ring 3 and not 
// calling the server for each pixel.
// It's just a test for now. :)

    gws_async_command2(
        fd,
        1000,   // request
        0,      // sub-request
        (unsigned long) (x & 0xFFFFFFFF),      // data1
        (unsigned long) (y & 0xFFFFFFFF),      // data2
        (unsigned long) (color & 0xFFFFFFFF),  // data3
        (unsigned long) rop );                 // data4
    
    return 0;
}

// -------

// Plot a 3d point using int.
int 
gws_plot0Int3D (
    int fd,
    int x,
    int y,
    int z,
    unsigned int color,
    unsigned long rop )
{
    int X=0;
    int Y=0;
    int res_x=0;
    int res_y=0;

    __transform_from_viewspace_to_screespace(
        (int *) &res_x, (int *) &res_y,
        x,y,z,
        TRUE,   //left hand?
        HotSpotX, HotSpotY );

    X = (int) res_x;
    Y = (int) res_y;

// Plot a 2d point using int.
    gws_plot0Int2D( 
        fd,
        X,         //x
        Y,         //y
        (unsigned int) color,  //color
        rop );        //rop

    return 0;
}

/*
 * gr_plotLine3d:  
 */
// Bresenham in 3D
// The algorithm could be extended to three (or more) dimensions.

void 
gr_plotLine3d (
    int fd,
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color,
    unsigned long rop )
{

   int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
   int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
   int dz = abs(z1-z0), sz = z0<z1 ? 1 : -1; 
   
   //#bugbug: This macro is wrong?!
   //int dm = grMAX3 (dx,dy,dz), i = dm; /* maximum difference */
   
   int dm = grMAX3(dx,dy,dz);
   register int i = dm;


    // x1 = y1 = z1 = dm/2; /* error offset */
 
    x1 = (dm >> 1);
    y1 = x1;
    z1 = x1;

    for (;;) 
    {

        //grPlot0 ( NULL, z0, x0, y0, color );

         // Plot a 3d point using int.
         gws_plot0Int3D( 
             fd,
             x0,      // x
             y0,      // y
             z0,      // z
             color,  //color
             rop );  //rop
 
        if (i-- == 0) { break; }
        x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; }
        y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; }
        z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; }
    };
}

// #todo
// plot line given two colors.
// interpolation ?
void 
plotLine3d2 (
    int fd,
    int x0, int y0, int z0, unsigned int color1,
    int x1, int y1, int z1, unsigned int color2, 
    int flag,
    unsigned long rop )
{
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    int dz = abs(z1-z0), sz = z0<z1 ? 1 : -1; 
   
    //#bugbug: This macro is wrong?!
    //int dm = grMAX3 (dx,dy,dz), i = dm; /* maximum difference */
   
    int dm = grMAX3(dx,dy,dz);
    register int i = dm;
   
    // x1 = y1 = z1 = dm/2; /* error offset */
 
    x1 = (dm >> 1);
    y1 = x1;
    z1 = x1;

// nothing for now;
// interpolation flag.

    flag=0;


//
// Loop 
//

    for (;;) {  

         // Plot a 3d point using int.
         gws_plot0Int3D( 
             fd,
             x0,      // x
             y0,      // y
             z0,      // z
             color1,  //color
             rop );  //rop

        // grPlot0 ( NULL, z0, x0, y0, color1 );
        //grPlot0 ( NULL, z0, x0, y0, color2 );
      
        if (i-- == 0) break;
        x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; } 
        y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; } 
        z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; } 
    };
}


void 
plotCircleZ ( 
    int fd,
    int xm, 
    int ym, 
    int r, 
    unsigned int color, 
    int z,
    unsigned long rop )
{

    /* II. Quadrant */ 
   //int x = -r, y = 0, err = 2-2*r; 
   
    //loop
    register int x = -r;
   
    int y = 0;
    int err =  (2-(2*r));

    do {
      
      //setPixel(xm-x, ym+y); /*   I. Quadrant */
      //setPixel(xm-y, ym-x); /*  II. Quadrant */
      //setPixel(xm+x, ym-y); /* III. Quadrant */
      //setPixel(xm+y, ym+x); /*  IV. Quadrant */
      
      //grPlot0 ( NULL, z, xm-x, ym+y, color);
      //grPlot0 ( NULL, z, xm-y, ym-x, color);
      //grPlot0 ( NULL, z, xm+x, ym-y, color);
      //grPlot0 ( NULL, z, xm+y, ym+x, color);

         gws_plot0Int3D( 
             fd,
             xm-x,      // x
             ym+y,      // y
             z,      // z
             color,  //color
             rop );  //rop

         gws_plot0Int3D( 
             fd,
             xm-y,      // x
             ym-x,      // y
             z,      // z
             color,  //color
             rop );  //rop

         gws_plot0Int3D( 
             fd,
             xm+x,      // x
             ym-y,      // y
             z,      // z
             color,  //color
             rop );  //rop

         gws_plot0Int3D( 
             fd,
             xm+y,      // x
             ym+x,      // y
             z,      // z
             color,  //color
             rop );  //rop


      r = err;
      
      // #ugly routine.
      
      /* e_xy+e_y < 0 */
      if (r <= y) 
      { 
           err += ++y * 2 + 1; 
      }           
      
      /* e_xy+e_x > 0 or no 2nd y-step */
      if (r > x || err > y) 
      { 
          err += ++x * 2+1; 
      }
      
    } while (x < 0);
}




// plot rectangle
// #todo
// Incluir nessa rotina se o retãngulo será 'filled'
// ou não. E poderemos chamar mais de uma rotina para isso.

int 
gws_plotrectangle (
    int fd,
    struct gr_rectangle_d *rect )
{
    //#deprecated
    return -1;
}

// plot cube
int 
gws_plotcube (
    int fd,
    struct gr_cube_d *cube )
{
    return -1;
}

int gr_initialize(void)
{
    __initializing_hotspot();
    return 0;
}






