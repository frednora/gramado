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

// Face definition from OBJ file.
// Stores indices into the vertex, texture, and normal arrays.
// Later these indices are resolved into actual vectors/triangles.
struct gr_face_d
{
    //int used;
    //int magic;
    int initialized;

    int vi[3];  // vertex indices (OBJ is 1-based, so subtract 1 when using)
    //int t[3];  // texture indices (optional, can be 0 if unused)
    //int n[3];  // normal indices (optional, can be 0 if unused)
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



struct gr_mat2x2_d {
    float m[2][2];
};
struct gr_mat3x3_d {
    float m[3][3];
};
struct gr_mat4x4_d {
    float m[4][4];
};


/*
struct gr_vecF4D_d {
    float x, y, z, w;
    unsigned int color;
};
*/

#endif    

