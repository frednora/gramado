// libgr3d.h
// 3D floating routines.
// Created by Fred Nora.

#ifndef __LIBGR3D_H
#define __LIBGR3D_H   1

#include "grprim3d.h"
#include <stddef.h>

extern int libgr_dummy;

//
// == Prototypes ============================================
//

void 
gr_MultiplyMatrixVector(
    struct gr_vecF3D_d *i, 
    struct gr_vecF3D_d *o, 
    struct gr_mat4x4_d *m );

struct gr_vecF3D_d *grVectorCrossProduct(
    struct gr_vecF3D_d *v1, 
    struct gr_vecF3D_d *v2 );

float dot_productF( struct gr_vecF3D_d *v1, struct gr_vecF3D_d *v2 );

float gr_discriminant(float a, float b, float c);

#endif    

