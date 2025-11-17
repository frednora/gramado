// libgr3d.h
// 3D floating routines.
// Created by Fred Nora.

#ifndef __LIBGR3D_LIBGR3D_H
#define __LIBGR3D_LIBGR3D_H   1


// rtl
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>

// Helpers
#include "grprim3d.h"
// ...

extern int libgr_dummy;

// Load a file.
char *libgr3dReadFileIntoBuffer(const char *filename);

float scan00_custom_read_float(const char **strPtr);

// It scans a single line given the pointer
const char *scan00_scanline(const char *line_ptr, struct gr_vecF3D_d *return_v);


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


