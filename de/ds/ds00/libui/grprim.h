
// grprim.h 
// Header for primitives.

#ifndef __GRPRIM_H
#define __GRPRIM_H   1

// The Perspective Calculation
// we can calculate the location of a 3D vertex in a 
// 2D viewing window with a multiplication and a division like this:
// x' = (x*near)/z
// y' = (y*near)/z
// x' = (x*near)/(-z)
// y' = (y*near)/(-z)

//
// ===========================================================
//

int grInit(void);


// See: view()
void 
gwsDepthRange(
    int minZ, 
    int maxZ);

void 
gwsViewport(
    int topleft_x, 
    int topleft_y, 
    int width, 
    int height);

// Point. 
// #todo: 
// Maybe we can use the dc structure.
// Or create another function that can do that.
// maybe grPlot00().
int 
grPlot0 (
    struct gws_window_d *clipping_window,   
    int z, int x, int y, 
    unsigned int color,
    unsigned long rop );

int 
grPlot1 ( 
    struct gws_window_d *clipping_window,   
    int x, int y, int z, 
    unsigned int color,
    unsigned long rop,
    unsigned long flags );

//
// == Line ==============================================
//

void 
plotLine3d (
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color );

void 
plotLine3dEx (
    struct gws_window_d *window,
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color,
    unsigned long rop );

// plot line given two colors.
// interpolation?
void 
plotLine3d2 (
    int x0, int y0, int z0, unsigned long color1,
    int x1, int y1, int z1, unsigned long color2, 
    int flag );

void 
plotQuadBezierSeg ( 
    int x0, int y0, int z0, 
    int x1, int y1, int z1,
    int x2, int y2, int z2, 
    unsigned int color );

//
// == rectangle =======================================
//

void
rectangle_from_two_points (
    int left, int top, 
    int right, int bottom,
    unsigned int color );

void __rectangleZZ( struct gr_rectangle_d *rect );
int grRectangle( struct gr_rectangle_d *rect );

void
rectangle_ras3D (
    int left, int top, int z0,
    int right, int bottom, int z1,
    unsigned int color );


//
// == triangle ==========================
//

int 
fillTriangle0(
    int x1, int y1,
    int x2, int y2,
    int x3, int y3, 
    unsigned int c,
    unsigned long rop );

int xxxTriangleZ ( struct gr_triangle_d *triangle );
int grTriangle( struct gr_triangle_d *triangle);

//
// == polygon ===========================
//

int xxxPolygonZ(struct gr_polygon_d *polygon);

//
// == Cube ============================
//

int xxxDrawCubeZ(struct gr_cube_d *cube);


//
// == Circle ===========================
//

void 
plotCircle(
    int xm, 
    int ym, 
    int r, 
    unsigned int color );

void 
plotCircleZ ( 
    int xm, 
    int ym, 
    int r, 
    unsigned int color, 
    int z );

//
// == Ellipse ====================================
//

void 
plotEllipseRect(
    int x0, 
    int y0, 
    int x1, 
    int y1, 
    unsigned long color );

void 
plotEllipseRectZ (
    int x0, int y0, 
    int x1, int y1, 
    unsigned long color,
    int z );

//
// == Char ========================================
//

void 
plotCharBackbufferDrawcharTransparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    unsigned long c );

void 
plotCharBackbufferDrawcharTransparentZ ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    unsigned long c,
    int z );

#endif    

