// projf.h
// Projection support using Float values.
// Created by Fred Nora.

#ifndef __PROJF_H
#define __PROJF_H    1

// ======================================================
// Projection support using Float values.
struct gr_projectionF_d
{
// Using float

    int initialized;

    float znear;
    float zfar;
    float fov;
    
    float ar;  //aspect ratio
    unsigned long width;
    unsigned long height;
    
    float scale_factor;
    
    //#todo: hotspot

    //struct gr_mat4x4_d *projection_matrix;
};
extern struct gr_projectionF_d  CurrentProjectionF;

//
// == Prototypes ======================================
//

// ======================================================
// Using float.

int 
grInitializeProjection(
    float znear, 
    float zfar, 
    float fov,
    unsigned long width,
    unsigned long height,
    float scalefactor );


#endif   


