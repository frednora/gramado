/*
 * File: gr.h
 *     Client-side 3D graphics support for libgws.
 *     This header should be included by including "gws.h".
 */

#ifndef __LIBGWS_GR_H
#define __LIBGWS_GR_H   1

#define grMIN2(a, b)  (((a) < (b)) ? (a) : (b))
#define grMAX2(a, b)  (((a) > (b)) ? (a) : (b))

#define grMIN3(x,y,z)    (x < y  ? (x < z ? x : z) : (y < z ? y : z))
#define grMAX3(x,y,z)    ( (x>y) ? ((x>z)?x:z)     : ((y>z)?y:z) )

// float
struct libgws_gr_vecF3D_d
{
    float x;
    float y;
    float z;

// For interpolation.
    unsigned int color;
};

// float 3D triangle
struct libgws_gr_triangleF3D_d
{
    int used;
    int magic;
    int initialized;
    struct libgws_gr_vecF3D_d p[3];
    // mesh support.
    struct libgws_gr_triangleF3D_d *last;
    struct libgws_gr_triangleF3D_d *next;
};

// float
struct libgws_gr_mat4x4_d
{
    float m[4][4];  // = { 0 };
};

// int
struct libgws_gr_vec3D_d
{
    int x;
    int y;
    int z;

// For interpolation.
    unsigned int color;
};

struct libgws_gr_line_d
{
    struct libgws_gr_vec3D_d p[2];
};

struct libgws_gr_triandle_d
{
    struct libgws_gr_vec3D_d p[3];
};

struct libgws_gr_rectangle_d
{
    struct libgws_gr_vec3D_d p[4];
};


// Cube
struct libgws_gr_cube_d
{
    struct libgws_gr_vec3D_d p[8];
};


// ??
// graphical projection perspective
// See:
// https://en.wikipedia.org/wiki/Perspective_(graphical)
struct libgws_gr_projection_d
{
    // perspective or orthographic
    int type;

// fovy   Number The angle between the upper and lower 
// sides of the viewing frustum.
// aspect Number The aspect ratio of the viewing window. (width/height).
 
// The frustrum:   
// The rectangle that the can see.
// The limits in z axis. zmin and zmax;
    
    struct libgws_gr_rectangle_d *frustrum_view;   // projection window.
    
    int zNear;   // Distance to the near clipping plane along the -Z axis.
    int zFar;    // Distance to the far clipping plane along the -Z axis.
    int zRange;  // zRange = zNear - zFar;

// fov: field of view.
    int angle_of_view;

// The apex.
    // mid_x = (left + right) * 0.5;
    // mid_y = (bottom + top)  * 0.5;
    struct libgws_gr_vec3D_d *frustrum_apex;


 // aspect ratio
    // ar = screen width / screen height
    // #bugbug: We are using int, not float.
    int ar;

    // talvez precisamos aqui uma matrix de transformaçao.
};



// Camera
struct libgws_gr_camera_d
{

// position
    struct libgws_gr_vec3D_d position;
    struct libgws_gr_vec3D_d upview;
    struct libgws_gr_vec3D_d lookat;
    // ...
    
    struct libgws_gr_projection_d *projection;

    // Next node in the linked list.
    //struct gr_camera_d *next;
};
//struct gr_camera_d *CurrentCamera;



//
// == 3d ==============================================
//

int gr_initialize(void);

unsigned int 
gr_interpolate_color(
    unsigned int color1, 
    unsigned int color2, 
    unsigned int fraction );

unsigned int invert_color(unsigned int color);

void 
libgws_gr_MultiplyMatrixVector(
    struct libgws_gr_vecF3D_d *i, 
    struct libgws_gr_vecF3D_d *o, 
    struct libgws_gr_mat4x4_d *m );

// Plot a point using float.
int 
gws_plot0 (
    int fd,
    float x,
    float y,
    float z,
    unsigned int color );

// Plot a 2d point using int.
int 
gws_plot0Int2D (
    int fd,
    int x,
    int y,
    unsigned int color,
    unsigned long rop );

// Plot a 3d point using int.
int 
gws_plot0Int3D (
    int fd,
    int x,
    int y,
    int z,
    unsigned int color,
    unsigned long rop );

void 
gr_plotLine3d (
    int fd,
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color,
    unsigned long rop );

void 
libgws_plotLine3d2 (
    int fd,
    int x0, int y0, int z0, unsigned int color1,
    int x1, int y1, int z1, unsigned int color2, 
    int flag,
    unsigned long rop );

void 
plotCircleZ ( 
    int fd,
    int xm, 
    int ym, 
    int r, 
    unsigned int color, 
    int z,
    unsigned long rop );

// Plot a rectangle.
int 
gws_plotrectangle (
    int fd,
    struct libgws_gr_rectangle_d *rect );

// Plot a cube.
int 
gws_plotcube (
    int fd,
    struct libgws_gr_cube_d *cube );

#endif    



