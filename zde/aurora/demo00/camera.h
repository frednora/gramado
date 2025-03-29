
// camera.h
#ifndef __CAMERA_H
#define __CAMERA_H    1


// ----------------------------------------------------
// Camera
// Not float.
struct gr_camera_d
{
    int used;
    int magic;
    int initialized;
// EYE: Position of the camera.
    struct gr_vec3D_d position;
// UP: Orientation.
    struct gr_vec3D_d upview;
// AT: target point.
// The object distance.
// Consider znear and zfar. 
// This way we know the if the model 
// is becoming bigger or smaller.
    struct gr_vec3D_d lookat;
// ?
    struct gr_projection_d *projection;
// Next node in the linked list.
    // int id;
    // struct gr_camera_d *next;
};
extern struct gr_camera_d  *CurrentCamera;

// ----------------------------------------------------
// Camera
// float
struct gr_cameraF_d
{
    //int used;
    //int magic;
    int initialized;
// EYE: Position of the camera.
    struct gr_vecF3D_d position;
// UP: Orientation.
    struct gr_vecF3D_d upview;
// AT: target point.
// The object distance.
// Consider znear and zfar. 
// This way we know the if the model 
// is becoming bigger or smaller.
    struct gr_vecF3D_d lookat;
};
extern struct gr_cameraF_d  CurrentCameraF;

// ==============================================
// Not using float.

int camera_initialize(void);
// Update camera.
int 
camera ( 
    int x, int y, int z,
    int xUp, int yUp, int zUp,
    int xLookAt, int yLookAt, int zLookAt );

int 
unveil_camera(
    int model_x, int model_y, int model_z );    
    
// ==============================================
// Using float.    

int cameraF_initialize(void);

#endif    


