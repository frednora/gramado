/*
 * File: grprim.h 
 *     Header for primitives.
 * History:
 *     2020 - Created by Fred Nora.
 */

// Some 3D stuff using float.

#ifndef __GRPRIM_H
#define __GRPRIM_H   1

//================================================
// The Perspective Calculation
// we can calculate the location of a 3D vertex in a 
// 2D viewing window with a multiplication and a division like this:
// x' = (x*near)/z
// y' = (y*near)/z
// x' = (x*near)/(-z)
// y' = (y*near)/(-z)

//
// ===========================================
//

int 
gr_rotate_x(
    struct gr_triangleF3D_d *in_tri,
    struct gr_triangleF3D_d *out_tri,   // rotated 
    float angle, 
    float fElapsedTime );

int 
gr_rotate_y(
    struct gr_triangleF3D_d *in_tri,
    struct gr_triangleF3D_d *out_tri,   // rotated 
    float angle, 
    float fElapsedTime );

int 
gr_rotate_z(
    struct gr_triangleF3D_d *in_tri,
    struct gr_triangleF3D_d *out_tri,   // rotated 
    float angle, 
    float fElapsedTime );

//
// ===========================================================
//

int grInit(void);

//
// projection
//

//extern struct gr_mat4x4_d matProj;




void 
gwsViewport(
    int topleft_x, 
    int topleft_y, 
    int width, 
    int height);

int 
gr_set_ray_info(
    struct gr_ray_d *r,
    struct gr_vec3D_d *origin,
    struct gr_vec3D_d *direction );

int gr_reset_ray_info(struct gr_ray_d *r);
struct gr_ray_d *ray_object(void);

void gr_dc_extents_init( struct dc_d *dc );
int gr_dc_refresh_screen( struct dc_d *dc );
int gr_dc_fill( struct dc_d *dc, unsigned int bg_color );

int 
gr_dc_plot0(
    struct dc_d *dc,
    int x,
    int y );

int 
grPlot2D ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop );

// Point. 
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

int serviceGrPlot0(void);

//
// == Line ==============================================
//

int 
plotLine3d (
    struct gws_window_d *window,
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color );

void 
plotLine3dEx (
    struct gws_window_d *window,
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color );

// plot line given two colors.
// interpolation ?
void 
plotLine3d2 (
    int x0, int y0, int z0, unsigned long color1,
    int x1, int y1, int z1, unsigned long color2, int flag );

int
plotLine3dLT2 (
    struct gws_window_d *window,
    int x0, int y0, int z0, 
    int x1, int y1, int z1,
    int *x2, int *y2, int *z2, 
    int n,   // track this vector.
    unsigned int color,
    int draw );

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

void drawRectangleF(struct gr_rectangleF3D_d *rectangle);
void __rectangleZZ( struct gr_rectangle_d *rect );
int grRectangle( struct gr_rectangle_d *rect );

void
rectangle_ras3D (
    int left, int top, int z0,
    int right, int bottom, int z1,
    unsigned int color );

//sevice 2042
int serviceGrRectangle(void);

//
// == triangle ==========================
//

int 
grTriangle3(
    struct gws_window_d *window, 
    struct gr_triangle_d *triangle );

int grTriangle(struct gr_triangle_d *triangle);

int 
fillTriangle0(
    int x1, int y1,
    int x2, int y2,
    int x3, int y3, 
    unsigned int c,
    unsigned long rop );

int 
fillTriangle(
    struct gws_window_d *window, 
    struct gr_triangle_d *triangle,
    int hotspotx, int hotspoty,
    unsigned long rop );

int 
plotTriangleF(
    struct gws_window_d *window,
    struct gr_triangleF3D_d *t,
    int fill,
    unsigned long rop );

// Plot a single pixel.
int 
plotPixelF(
    struct gws_window_d *window,
    struct gr_vecF3D_d *vec,
    unsigned long rop );

//
// == polygon ===========================
//

int xxxPolygonZ ( struct gr_polygon_d *polygon );

//
// == Cube ============================
//

int xxxDrawCubeZ( struct gr_cube_d *cube );
//sevice 2041
int serviceGrCubeZ(void);

//
// == Circle ===========================
//

// 2D
void 
plotCircle(
    int xm, 
    int ym, 
    int r, 
    unsigned int color );

// 3D
void 
grCircle3 ( 
    struct gws_window_d *window,
    int xm, 
    int ym, 
    int r, 
    unsigned int color, 
    int z );

//
// == Ellipse ====================================
//

// 2D
void 
grEllipse(
    int x0, 
    int y0, 
    int x1, 
    int y1, 
    unsigned int color );

// 3D
void 
grEllipse3 (
    int x0, int y0, 
    int x1, int y1, 
    unsigned int color,
    int z );

// Fred Nora's algo. 
// Prime stuff.
void noraDrawingStuff(void);
void noraDrawingStuff3(int x, int y, int z);



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

// Pixel
int servicepixelBackBufferPutpixel (void);


#endif    

