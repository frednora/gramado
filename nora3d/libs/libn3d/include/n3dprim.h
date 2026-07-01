// n3dprim.h
// Some 3D stuff using float.
// Created by Fred Nora.

#ifndef __N3DPRIM_H
#define __N3DPRIM_H    1


// 3D float point.
// For nora3d library.
struct n3d_vec_d
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
// For nora3d library.
struct n3d_triangle_d
{
    int used;
    int magic;
    int initialized;
    struct n3d_vec_d p[3];
    // mesh support.
    struct n3d_triangle_d *last;
    struct n3d_triangle_d *next;
};

// 3D rectangle. float falues.
struct n3d_rectangle_d
{
    struct n3d_vec_d p[4];
    // mesh support.
    struct n3d_rectangle_d *last;
    struct n3d_rectangle_d *next;
};

//
// Matrices
//

struct n3d_mat2x2_d {
    float m[2][2];
};
struct n3d_mat3x3_d {
    float m[3][3];
};
struct n3d_mat4x4_d {
    float m[4][4];
};


#endif    
