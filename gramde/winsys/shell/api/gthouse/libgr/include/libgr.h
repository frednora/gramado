
// libgr.h

#ifndef __LIBGR_H
#define __LIBGR_H    1

#include <types.h>
#include <stddef.h>
#include "grprim0.h"

extern int coisolibgr;

// matrix using int
/*
static int projection4x4[4][4] = { 
        {1,0,0,0}, 
        {0,1,0,0},
        {0,0,0,0},
        {1,1,0,0}
        };
*/


// -------------------------------------------------------

// Alternative.
int 
libgr_transform_to_screespace(
    int *res_x, int *res_y,
    int _x, int _y, 
    int _hotspotx, int _hotspoty );

// Alternative.
int 
libgr_transform_from_viewspace_to_screespace(
    int *res_x, int *res_y,
    int _x, int _y, int _z,
    int left_hand,
    int _hotspotx, int _hotspoty );





// Using 'int'.
void 
multiply4(
    int mat1[4][4], 
    int mat2[4][4], 
    int res[4][4] );

// Interpolation
unsigned int 
interpolate_color(
    unsigned int color1, 
    unsigned int color2, 
    unsigned int fraction );

unsigned int invert_color(unsigned int color);

// Fibonacci Series using Recursion 
// #todo: Create fib_int and fib_long
// This thing is very slow.
int fib(int n);

int dot_product( struct gr_vec3D_d *v1, struct gr_vec3D_d *v2 );

int gr_triangle_area_int (int base, int height);

int gr_magic_volume (int x, int y, int z);
int gr_magic_area (int x, int y, int z);

int gr_discriminant_int(int a, int b, int c);


int 
gr_find_obj_height_int ( 
    int *obj_height, int obj_distance,
    int img_height, int img_distance );

int 
gr_find_img_height_int ( 
    int obj_height, int obj_distance,
    int *img_height, int img_distance );

void gr_scale_vec( struct gr_vec3D_d *v, int scale );

int xxxInflateCubeZ( struct gr_cube_d *cube, int value );
int xxxDeflateCubeZ( struct gr_cube_d *cube, int value );


#endif   



