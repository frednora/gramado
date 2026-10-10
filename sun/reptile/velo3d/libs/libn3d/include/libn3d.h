// libn3d.h
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

#include "n3dprim.h"
#include <stddef.h>
#include <math.h>

extern int libgr_dummy;

//
// == Prototypes ============================================
//


// Add two 3D vectors
void 
gr_AddVector3D(
    struct n3d_vec_d *a, 
    struct n3d_vec_d *b, 
    struct n3d_vec_d *result ) ;

// Subtract two 3D vectors
void 
gr_SubVector3D(
    struct n3d_vec_d *a, 
    struct n3d_vec_d *b, 
    struct n3d_vec_d *result);

// Scale a 3D vector by a scalar
void 
gr_ScaleVector3D(
    struct n3d_vec_d *v, 
    float s, 
    struct n3d_vec_d *result);

// Magnitude (length) of a 3D vector
float gr_VectorMagnitude3D(struct n3d_vec_d *v);

// Normalize a 3D vector (unit length)
void 
gr_NormalizeVector3D(
    struct n3d_vec_d *v, 
    struct n3d_vec_d *result);

// Distance between two 3D vectors (points)
float gr_VectorDistance3D(struct n3d_vec_d *a, struct n3d_vec_d *b);

// ===================

// Multiply two 2x2 matrices
void gr_MultiplyMatrix2x2(
    struct n3d_mat2x2_d *a,
    struct n3d_mat2x2_d *b,
    struct n3d_mat2x2_d *result );

// Multiply two 3x3 matrices
void gr_MultiplyMatrix3x3(
    struct n3d_mat3x3_d *a,
    struct n3d_mat3x3_d *b,
    struct n3d_mat3x3_d *result );

// Multiply two 4x4 matrices
void gr_MultiplyMatrix4x4(
    struct n3d_mat4x4_d *a,
    struct n3d_mat4x4_d *b,
    struct n3d_mat4x4_d *result );


void 
gr_ProjectVector(
    struct n3d_vec_d *i, 
    struct n3d_vec_d *o, 
    struct n3d_mat4x4_d *m );

void 
gr_MultiplyVector (
    struct n3d_vec_d *i, 
    struct n3d_vec_d *o, 
    struct n3d_mat4x4_d *m );

void 
gr_MultiplyAndProjectVector(
    struct n3d_vec_d *i, 
    struct n3d_vec_d *o, 
    struct n3d_mat4x4_d *m );


struct n3d_vec_d *grVectorCrossProduct(
    struct n3d_vec_d *v1, 
    struct n3d_vec_d *v2 );

float dot_productF( struct n3d_vec_d *v1, struct n3d_vec_d *v2 );

float gr_discriminant(float a, float b, float c);

#endif    

