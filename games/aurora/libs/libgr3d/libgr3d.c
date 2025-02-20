// libgr3d.c
// Support for 3D routines.
// sing float values.
// Created by Fred Nora.

#include "include/libgr3d.h"

int libgr_dummy=0;

// =============================================

void 
gr_MultiplyMatrixVector(
    struct gr_vecF3D_d *i, 
    struct gr_vecF3D_d *o, 
    struct gr_mat4x4_d *m )
{
    o->x = 
        (float) (
        i->x * m->m[0][0] + 
        i->y * m->m[1][0] + 
        i->z * m->m[2][0] + 
        m->m[3][0] );

    o->y = 
        (float) (
        i->x * m->m[0][1] + 
        i->y * m->m[1][1] + 
        i->z * m->m[2][1] + 
        m->m[3][1] );
    
    o->z = 
        (float) (
        i->x * m->m[0][2] + 
        i->y * m->m[1][2] + 
        i->z * m->m[2][2] + 
        m->m[3][2] );

    float w = 
        (float) (
        i->x * m->m[0][3] + 
        i->y * m->m[1][3] + 
        i->z * m->m[2][3] + 
        m->m[3][3] );

// Normalization.
    if (w != 0.0f)
    {
        o->x = (float) (o->x / w); 
        o->y = (float) (o->y / w); 
        o->z = (float) (o->z / w);
    }
}

struct gr_vecF3D_d *grVectorCrossProduct(
    struct gr_vecF3D_d *v1, 
    struct gr_vecF3D_d *v2 )
{
//#todo: Not tested yet.

    struct gr_vecF3D_d vRes;

    vRes.x = (float) (v1->y * v2->z - v1->z * v2->y);
    vRes.y = (float) (v1->z * v2->x - v1->x * v2->z);
    vRes.z = (float) (v1->x * v2->y - v1->y * v2->x);

    return (struct gr_vecF3D_d *) &vRes;
}

float dot_productF( struct gr_vecF3D_d *v1, struct gr_vecF3D_d *v2 )
{
// Dot product.
// The dot product describe the 
// relationship between two vectors.
// Positive: Same direction
// negative: Opposite direction
// 0:        Perpendicular.

// Fake perpendicular.
    if ( (void*) v1 == NULL ){ return (float) 0.0f; }
    if ( (void*) v2 == NULL ){ return (float) 0.0f; }
// (x*x + y*y + z*z)
    return (float) ( v1->x * v2->x + 
                     v1->y * v2->y + 
                     v1->z * v2->z );
}

// Get delta for bhaskara.
// d<0: (negative) "Raiz de número negativo em Baskara"
// d=0: (null)     duas raizes reais iguais.
// d>0: (positive) duas raizes reais diferentes. (Intersection)
float gr_discriminant(float a, float b, float c)
{
// Used to test for intesection in the ray tracing.
// Discriminant: Delta da função em bhaskara.
    float Discriminant = (float) ((b*b) - (4*a*c));
    return (float) Discriminant;
}




