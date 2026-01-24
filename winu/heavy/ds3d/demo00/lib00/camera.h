// camera.h


#ifndef __LIB00_CAMERA_H
#define __LIB00_CAMERA_H    1


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
// Position (EYE): where the camera sits in world space.
    struct gr_vecF3D_d position;

// UP: Orientation.
// Up vector (UP): defines orientation (which way is “up”).
    struct gr_vecF3D_d upview;

// AT: target point.
// LookAt (AT): the target point the camera is focused on.
// The object distance.
// Consider znear and zfar. 
// This way we know the if the model 
// is becoming bigger or smaller.
    struct gr_vecF3D_d lookat;


// Forward, Right, Up:
// These form the camera’s local coordinate system (basis vectors).
// Ensures the camera orientation is orthonormal.
// Used to build the view matrix.

// Derived basis vectors (orthonormal camera axes) 
    struct gr_vecF3D_d forward;  // normalized (lookat - position) 
    struct gr_vecF3D_d right;    // cross(forward, upview) 
    struct gr_vecF3D_d up;       // recomputed cross(right, forward)

// View Matrix:
// Stores the transformation from world space into camera space.
// Built once per frame using position, lookat, and upview.
// Applied to all vertices before projection.

// View matrix (world → camera space) 
    // struct gr_mat4x4_d  viewMatrix;

// Projection Link:
// Connects the camera to its projection (gr_projectionF_d).
// Lets you chain View → Projection seamlessly.

// Link to projection (perspective/orthographic) 
    struct gr_projectionF_d *projection;
};
extern struct gr_cameraF_d  CurrentCameraF;

/*
Pipeline
 + Model transform: object → world.
 + View transform: world → camera (using cameraF.viewMatrix).
 + Projection transform: camera → clip space (using cameraF.projection->matProj).
 + Viewport transform: clip space → screen coordinates.
 + Rasterizer: fillTriangle0.
*/

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


