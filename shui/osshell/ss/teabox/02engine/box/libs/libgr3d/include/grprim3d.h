// grprim3d.h
// Some 3D stuff using float.
// Created by Fred Nora.

#ifndef __GRPRIM3D_H
#define __GRPRIM3D_H    1

// 2D float point.
// #todo: Maybe in the future.
struct gr_vecF2D_d
{
    float x;
    float y;
    unsigned int color;
};

// 3D float point.
struct gr_vecF3D_d
{
    float x;
    float y;
    float z;
    unsigned int color;
};

// float 3D triangle
struct gr_triangleF3D_d
{
    int used;
    int magic;
    int initialized;
    struct gr_vecF3D_d p[3];
    // mesh support.
    struct gr_triangleF3D_d *last;
    struct gr_triangleF3D_d *next;
};

// 3D rectangle. float falues.
struct gr_rectangleF3D_d
{
    struct gr_vecF3D_d p[4];
    // mesh support.
    struct gr_rectangleF3D_d *last;
    struct gr_rectangleF3D_d *next;
};


// Matrix structure using float.
struct gr_mat4x4_d
{
    // Not initialized.
    float m[4][4];
};

#endif    

