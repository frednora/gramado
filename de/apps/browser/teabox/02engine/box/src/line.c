
// line.c 
// We are using the library libgd.o.

#include "gram3d.h"


//#bugbug: use unsigned int for 'color'.
void 
plotLineRect4 ( 
    int x1, int y1, 
    int x2, int y2, 
    int color )
{
    plotLine3d ( NULL, x1,y1,0, x2,y1,0, color );
    plotLine3d ( NULL, x2,y1,0, x2,y2,0, color );
    plotLine3d ( NULL, x2,y2,0, x1,y2,0, color );
    plotLine3d ( NULL, x1,y2,0, x1,y1,0, color );
}

//#bugbug: use unsigned int for 'color'.
void 
plotLineRectZ ( 
    int x1, int y1, int z1, 
    int x2, int y2, int z2, 
    int color )
{
    plotLine3d ( NULL, x1,y1,z1, x2,y1,z2, color );
    plotLine3d ( NULL, x2,y1,z1, x2,y2,z2, color );
    plotLine3d ( NULL, x2,y2,z1, x1,y2,z2, color );
    plotLine3d ( NULL, x1,y2,z1, x1,y1,z2, color );
}

//#bugbug: use unsigned int for 'color'.
//ok triangulo invertido.
void 
testLines ( 
    int x1, int y1, 
    int x2, int y2, 
    int color )
{
    int dx = x2-x1;
    int dy = y2-y1;

    plotLine3d(NULL,   x1,  y1-dy, 0,   x1+dx, y1   , 0, color);
    plotLine3d(NULL,x1+dx,     y1, 0,     x1, y1+dy , 0, color);
    plotLine3d(NULL,   x1,  y1+dy, 0,  x1-dx, y1    , 0, color);
    plotLine3d(NULL,x1-dx,     y1, 0,     x1, y1-dy , 0, color);
}

//#bugbug: use unsigned int for 'color'.
//ok triangulo invertido.
void 
testLinesZ ( 
    int x1, int y1, int z1, 
    int x2, int y2, int z2, 
    int color )
{
    int dx = (x2-x1);
    int dy = (y2-y1);

    plotLine3d (NULL,   x1,  y1-dy, z1,   x1+dx, y1   , z2, color);
    plotLine3d (NULL,x1+dx,     y1, z1,     x1, y1+dy , z2, color);
    plotLine3d (NULL,   x1,  y1+dy, z1,  x1-dx, y1    , z2, color);
    plotLine3d (NULL,x1-dx,     y1, z1,     x1, y1-dy , z2, color);
}


// ...
//dx = x2 − x1
//dy = y2 − y1
//for x from x1 to x2 do
//    y = y1 + dy × (x − x1) / dx
//    plot(x, y)
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

    for ( x = x1; x < x2; x++ )
    {
         // ugly
         y = (y1 + dy * (x - x1) / dx);
         
         // IN: color, x, y, rop
         libdisp_backbuffer_putpixel(color,x,y,0);
    }; 
}


//test1
void test_draw_line(void)
{
    register int g=0;

    for( g=0; g<400; g++ )
    {
            A_naive_line_drawing_algorithm ( 
                8, 750,                 //x1,x2
                8, (50 + (g*4)) ,       //y1,y2
                COLOR_YELLOW );
     };

    gwssrv_show_backbuffer();
}


// test2
// illusion
void test_draw_line2(void)
{
    register int g=0;
    
    unsigned long vertex1[2];
    unsigned long vertex2[2]; 

    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

    // #todo
    // Check the w h validation.

    // horizontal
    for ( g=0; g<h; g=g+10 )
    {
            vertex1[0]=0;  //x1
            vertex1[1]=g;  //y1

            vertex2[0]=g;  //x2
            vertex2[1]=g;  //y2
            
            A_naive_line_drawing_algorithm ( 
                vertex1[0], vertex2[0],   //x1,x2
                vertex1[1], vertex2[1],   //y1,y2
                COLOR_YELLOW );
     };

    // horizontal
    for ( g=0; g<h; g=g+10 )
    {
            vertex1[0]=g;  //x1
            vertex1[1]=g;  //y1

            vertex2[0]=w;  //x2
            vertex2[1]=g;  //y2
            
            A_naive_line_drawing_algorithm ( 
                vertex1[0], vertex2[0],   //x1,x2
                vertex1[1], vertex2[1],   //y1,y2
                COLOR_BLUE );
     };

    gwssrv_show_backbuffer();
}


/* 
 * grBackbufferDrawHorizontalLine:
 *     Draw a horizontal line on backbuffer. 
 */
// #todo: return the number of pixels changed.
// Called by the triangle rasterization routine
// fillTriangle0() in grprim.c.
int
grBackbufferDrawHorizontalLine ( 
    unsigned long x1,  //min
    unsigned long y, 
    unsigned long x2,  //max
    unsigned int color,
    unsigned long rop )
{
// Number of pixels changed.
    int npixels=0;
    
    //unsigned long __x1 = (unsigned long) x1;  //min
    //unsigned long __x2 = (unsigned long) x2;  //max
    //unsigned long __y  = (unsigned long) y;

// The worker uses 'int'.
    int __x1 = (int) (x1 & 0xFFFFFFFF);  // x min
    int __x2 = (int) (x2 & 0xFFFFFFFF);  // x max
    int __y  = (int) ( y & 0xFFFFFFFF);  // y
    unsigned long _rop = rop;

    unsigned long deviceWidth  = gws_get_device_width();
    //unsigned long deviceHeight = gws_get_device_height();
    int w = (int) (deviceWidth & 0xFFFFFFFF);

// Clipping left.
// Temos que começar de 0.
// Se o xmin for menor que 0, então temos que
// considerarmos não pintar a diferença que esta fora da tela.
    //int dx=0;
    if (__x1<0)
    { 
        __x1=0;  //good
    }

// We can draw a horizontal line above the screen.
    if (y<0){
        //debug_print("grBackbufferDrawHorizontalLine: y<0\n");
        return (int) npixels;
    }

// #todo:
// Limit given by the device context.
    //#delete
    //if (__x2 > 800){
    //    debug_print("grBackbufferDrawHorizontalLine: __x2 > 800\n");
    //    return (int) npixels;
    //}

// Clipping right.
// ok. its working
    if (__x2 >= w){
        __x2 = w-1;
    }

// Invalid line values.
    if (__x1 > __x2)
    {
        //debug_print("grBackbufferDrawHorizontalLine: __x1 > __x2\n");
        return (int) npixels;
    }

// Delta
// Pre-calculating the 
// number of pixels in the real painted line.
    npixels = (int) (__x2 - __x1);

// loop:
// Plot a pixel using the top/left coordinates. (0,0).
// IN: color, x, y, rop
    int check_value = 0;
    while (__x1 < __x2)
    {
        // #todo
        // Here, we need to check if we actually
        // are drawing all the pixels.
        check_value += 
            (int) grPlot2D( 
                      (unsigned int) color, 
                      (int) __x1, 
                      (int) __y, 
                      (unsigned long) _rop );
        __x1++;
    };

// We painted less than we wanted.
    if ( check_value < npixels ){
        npixels = check_value; 
    }

// Return the number of changed pixels.
    return (int) npixels;
}


/*
 * backbuffer_draw_horizontal_line:
 *     Draw a horizontal line on backbuffer. 
 */

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
        backbuffer_putpixel ( color, x1, y, rop_flags ); 
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
        frontbuffer_putpixel ( color, x1, y, rop_flags );
        x1++;
    };
}


//
// End
//






