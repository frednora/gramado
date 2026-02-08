// libgr3d.h
// 3D floating routines.
// Created by Fred Nora.

/*
// Functions found here:

gr_MultiplyAndProjectVector:
transforms a 3D vector by a 4x4 matrix, handling translation and perspective division.

grVectorCrossProduct:
gives you a perpendicular vector (useful for normals).

dot_productF:
calculates how much two vectors point in the same direction.

gr_discriminant:
is used for intersection tests (like in ray tracing) to check if a quadratic equation has real solutions.
*/


#ifndef __LIBGR3D_H
#define __LIBGR3D_H   1

#include "grprim3d.h"
#include <stddef.h>

extern int libgr_dummy;

//
// == Prototypes ============================================
//

void 
gr_ProjectVector(
    struct gr_vecF3D_d *i, 
    struct gr_vecF3D_d *o, 
    struct gr_mat4x4_d *m );

void 
gr_MultiplyVector (
    struct gr_vecF3D_d *i, 
    struct gr_vecF3D_d *o, 
    struct gr_mat4x4_d *m );

void 
gr_MultiplyAndProjectVector(
    struct gr_vecF3D_d *i, 
    struct gr_vecF3D_d *o, 
    struct gr_mat4x4_d *m );


struct gr_vecF3D_d *grVectorCrossProduct(
    struct gr_vecF3D_d *v1, 
    struct gr_vecF3D_d *v2 );

float dot_productF( struct gr_vecF3D_d *v1, struct gr_vecF3D_d *v2 );

float gr_discriminant(float a, float b, float c);

#endif    

