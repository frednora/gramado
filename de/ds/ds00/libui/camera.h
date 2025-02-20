
// camera.h
#ifndef __CAMERA_H
#define __CAMERA_H    1

// Camera
struct gr_camera_d
{
// Do not use float.

    int used;
    int magic;
    int initialized;
// EYE: Position of the camera.
    struct gr_vec3D_d position;
// UP: Orientation.
    struct gr_vec3D_d upview;
// AT: target point.
    struct gr_vec3D_d lookat;
// ?
    struct gr_projection_d *projection;
// Next node in the linked list.
    // int id;
    // struct gr_camera_d *next;
};

extern struct gr_camera_d  *CurrentCamera;

// ====================================================

int camera_initialize(void);
int 
camera ( 
    int x, int y, int z,
    int xUp, int yUp, int zUp,
    int xLookAt, int yLookAt, int zLookAt );

#endif    


