
// line.c 
// Line support.
// We are using the library libgd.o.
// Created by Fred Nora.

#include "../gwsint.h"

// #todo
// Use 'unsigned int' for 'color'.
void 
plotLineRect4 ( 
    int x1, int y1, 
    int x2, int y2, 
    int color )
{
    plotLine3d( x1,y1,0, x2,y1,0, color );
    plotLine3d( x2,y1,0, x2,y2,0, color );
    plotLine3d( x2,y2,0, x1,y2,0, color );
    plotLine3d( x1,y2,0, x1,y1,0, color );
}

// #todo
// Use 'unsigned int' for 'color'.
void 
plotLineRectZ ( 
    int x1, int y1, int z1, 
    int x2, int y2, int z2, 
    int color )
{
    plotLine3d( x1,y1,z1, x2,y1,z2, color );
    plotLine3d( x2,y1,z1, x2,y2,z2, color );
    plotLine3d( x2,y2,z1, x1,y2,z2, color );
    plotLine3d( x1,y2,z1, x1,y1,z2, color );
}

// triangulo invertido?
// #todo
// Use 'unsigned int' for 'color'.
void 
testLines ( 
    int x1, int y1, 
    int x2, int y2, 
    int color )
{
   int dx = x2-x1;
   int dy = y2-y1;

    plotLine3d(   x1,  y1-dy, 0,   x1+dx, y1   , 0, color);
    plotLine3d(x1+dx,     y1, 0,     x1, y1+dy , 0, color);
    plotLine3d(   x1,  y1+dy, 0,  x1-dx, y1    , 0, color);
    plotLine3d(x1-dx,     y1, 0,     x1, y1-dy , 0, color);
}

// triangulo invertido?
// #todo
// Use 'unsigned int' for 'color'.
void 
testLinesZ ( 
    int x1, int y1, int z1, 
    int x2, int y2, int z2, 
    int color )
{
    int dx = (x2-x1);
    int dy = (y2-y1);

    plotLine3d (   x1,  y1-dy, z1,   x1+dx, y1   , z2, color);
    plotLine3d (x1+dx,     y1, z1,     x1, y1+dy , z2, color);
    plotLine3d (   x1,  y1+dy, z1,  x1-dx, y1    , z2, color);
    plotLine3d (x1-dx,     y1, z1,     x1, y1-dy , z2, color);
}

// ...
// dx = x2 − x1
// dy = y2 − y1
// for x from x1 to x2 do
//     y = y1 + dy × (x − x1) / dx
// plot(x, y)
// See: https://en.wikipedia.org/wiki/Line_drawing_algorithm

void
A_naive_line_drawing_algorithm(
    unsigned long x1,
    unsigned long x2, 
    unsigned long y1, 
    unsigned long y2, 
    unsigned long color)
{
    register unsigned long x=0;
    register unsigned long y=0;
    unsigned long dx =  (x2 - x1);
    unsigned long dy =  (y2 - y1);

    for ( x=x1; x < x2; x++ )
    {
         // ugly
         y = (y1 + dy * (x - x1) / dx);
         // IN: color, x, y, rop
         libdisp_backbuffer_putpixel(color,x,y,0);
    }; 
}

// Test1
void test_draw_line(void)
{
    register int i=0;
    unsigned int Color = COLOR_YELLOW;

// IN: x1,x2,y1,y2,color.
    for (i=0; i<400; i++){
        A_naive_line_drawing_algorithm ( 
            8, 750,
            8, (50 + (i*4)) ,
            Color );
    };
    gwssrv_show_backbuffer();
}

// Test2
// illusion
void test_draw_line2(void)
{
// #bugbug
// Explain it better.
// Something is wrong here. We have 2 loops.

    register int i=0;
    unsigned long vertex1[2];
    unsigned long vertex2[2];
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

// #todo
// Check the w h validation.

    for ( i=0; i<h; i=i+10 )
    {
        vertex1[0]=0;  //x1
        vertex1[1]=i;  //y1
        vertex2[0]=i;  //x2
        vertex2[1]=i;  //y2
        A_naive_line_drawing_algorithm ( 
            vertex1[0], vertex2[0],   //x1,x2
            vertex1[1], vertex2[1],   //y1,y2
            COLOR_YELLOW );
     };

    for ( i=0; i<h; i=i+10 )
    {
            vertex1[0]=i;  //x1
            vertex1[1]=i;  //y1

            vertex2[0]=w;  //x2
            vertex2[1]=i;  //y2
            
            A_naive_line_drawing_algorithm ( 
                vertex1[0], vertex2[0],   //x1,x2
                vertex1[1], vertex2[1],   //y1,y2
                COLOR_BLUE );
     };

// #slow.
    gwssrv_show_backbuffer();
}


/* 
 * grBackbufferDrawHorizontalLine:
 *     Draw a horizontal line on backbuffer. 
 */
// It's using the ring3 routine.
// See libdisp.c for 'put pixel'.
void 
grBackbufferDrawHorizontalLine ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2,  
    unsigned int color )
{
    unsigned int __x1 = (unsigned int) (x1 & 0xFFFFFFFF);
    unsigned int __x2 = (unsigned int) (x2 & 0xFFFFFFFF);
    unsigned int __y  = (unsigned int) (y  & 0xFFFFFFFF);
    unsigned long __rop = 0;

    //debug_print("Line\n");

    unsigned long deviceWidth  = gws_get_device_width();
    //unsigned long deviceHeight = gws_get_device_height();
    int w = (int) (deviceWidth & 0xFFFFFFFF);

// #todo
// Check w limits.


// Limits
// #bugbug
// Checking agains 'unsigned int'.

    if (__x1 < 0){
        return;
    }
    if (__x2 < 0){
        return;
    }
    if (__y < 0){
        return;
    }

    //if (__x2 > 800){
    //    debug_print("grBackbufferDrawHorizontalLine: __x2 > 800\n");
    //    return;
    //}

    if (__x2 >= w){
        __x2 = w-1;
    }

    if (__x1 > __x2){
        debug_print("grBackbufferDrawHorizontalLine: __x1 > __x2\n");
        return;
    }

// Draw the line.
// It's using the ring3 routine.
// IN: color, x, y, rop
// see: libdisp.c

    while (__x1 < __x2)
    {
        libdisp_backbuffer_putpixel( 
            (unsigned int) color, 
            (int) __x1, 
            (int) __y,
            (unsigned long) __rop );
        
        __x1++;  
    };
}

// backbuffer_draw_horizontal_line:
// Draw a horizontal line on backbuffer. 
void 
backbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags )
{
// #todo
// Maybe we need checking some limits here.
    if (x1 > x2){
        return;
    }
// IN: color, x, y, rop flags.
    while (x1 < x2){
        backbuffer_putpixel( color, x1, y, rop_flags ); 
        x1++;
    };
}

void 
frontbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags )
{
// #todo
// Maybe we need checking some limits here.
    if (x1 > x2){
        return;
    }
// IN: color, x, y, rop flags.
    while (x1 < x2){
        frontbuffer_putpixel( color, x1, y, rop_flags );
        x1++;
    };
}

//
// End
//






