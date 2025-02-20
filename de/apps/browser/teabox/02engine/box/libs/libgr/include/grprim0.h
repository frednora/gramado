
// grprim0.h
// Some 3d stuff usint int.

#ifndef __GRPRIM0_H
#define __GRPRIM0_H    1


#define grMIN2(a, b)  (((a) < (b)) ? (a) : (b))
#define grMAX2(a, b)  (((a) > (b)) ? (a) : (b))
#define grMIN3(x,y,z)    (x < y  ? (x < z ? x : z) : (y < z ? y : z))
#define grMAX3(x,y,z)    ( (x>y) ? ((x>z)?x:z)     : ((y>z)?y:z) )

/*
#define MIN2(v0,v1) ((v0<v1)?v0:v1)
#define MAX2(v0,v1) ((v0>v1)?v0:v1)
#define MIN3(v0,v1,v2) ((v0<v1)?((v0<v2)?v0:v2):((v1<v2)?v1:v2))
#define MAX3(v0,v1,v2) ((v0>v1)?((v0>v2)?v0:v2):((v1>v2)?v1:v2))
*/

//#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))




// 2D integer point.
struct gr_vec2D_d
{
    int x;
    int y;
    unsigned int color;
};
// 3D integer point.
struct gr_vec3D_d
{
    int x;
    int y;
    int z;
    unsigned int color;
};
// 3D line int
struct gr_line_d
{
    struct gr_vec3D_d p[2];
};
// 3D triangle
struct gr_triangle_d
{
    int used;
    int magic;
    int initialized;
    struct gr_vec3D_d p[3];
    // mesh support.
    struct gr_triangle_d *last;
    struct gr_triangle_d *next;
};
// 3D rectangle.
struct gr_rectangle_d
{
    struct gr_vec3D_d p[4];
    // mesh support.
    struct gr_rectangle_d *last;
    struct gr_rectangle_d *next;
};
// Cube
struct gr_cube_d
{
    struct gr_vec3D_d p[8];
};
// Polygon
#define POLYGON_POLYPOINT    1
#define POLYGON_POLYLINE     2
struct gr_polygon_d 
{
// type: POLYLINE, POLYPOINT
    int type;
// number of elements.
    int n;
// List of vectors.
    void *list_address;
};

struct gr_mesh_point_d
{
// raw color
    unsigned int color;
// cada número representa um 
// índice num vetor de indices.
    int n[1];
};

struct gr_mesh_line_d
{
// raw color
    unsigned int color;
// cada número representa um 
// índice num vetor de indices.
    int n[2];
};

// tri mesh
struct gr_mesh_triangle_d
{
    int n;
    struct gr_triangle_d *first_triangle;
    struct gr_triangle_d *last_triangle;
};

// quad mesh
struct gr_mesh_rectangle_d
{
    int n;
    struct gr_rectangle_d *first_rectangle;
    struct gr_rectangle_d *last_rectangle;   
};

struct gr_mesh_d
{
// Quantos bytes tem a face list.
    int facelist_size_in_bytes;   // bytes
    int facelist_size_in_dwords;  // ints
// lista de indices.
// cada indice representa um vetor.
    void *facelist;
// Se o tipo for triangulo, então
// na facelist os indices serão agrupados de 3 em 3.
// Se for quad, então serão agrupados de 4 em 4.
// 1=TRIANGLE
// 2=QUAD
    int type;
// contando os triangulos na lista caso o mesh seja de quad.
    int tri_count;
// contando os quads na lista caso o mesh seja de quad.
    int quad_count; 
// quantos pontos no total.
// Cada indice vai usar 4 bytes da lista.
    int vertex_count;

// Navegation.
    //struct gr_mesh_d *last;
    struct gr_mesh_d *next;
};


struct gr_mesh_list_d
{
// number of elements in the linked list.
    int n;
    struct gr_mesh_d *first_mesh;
    struct gr_mesh_d *last_mesh;
};


// #define RAY_TYPE_LIGHT  1
// #define RAY_TYPE_??     2
#define RAY_ORIGIN  0
#define RAY_TARGET  1
// todo: aliases
// a vector with a starting point.
struct gr_ray_d
{
    int used;
    int magic;
// Type of ray.
// primary ray, reflection ray, shadow ray.
    int type;
    //int intensity;
    //int gradation;
    //int interpolation;
    // ...
// direction:
// The starting point is the source of light.
// #define RAY_ORIGIN  0
// #define RAY_TARGET  1
// 0 = origin. starting point
// 1 = direction. vector
    struct gr_vec3D_d p[2];
// Distance between 
// the starting point and the target.
    int distance;
};

// Array of chars.
struct gr_bitmap_d
{
    int type;
    unsigned long width;
    unsigned long height;
// 64 flags
    unsigned long flags;
    unsigned int color;
// Translation
    int x;
    int y;
    int z;
// buffer
    char *body;
};
// extern struct gr_bitmap_d *bitmap_screen;
// ...


#endif    


