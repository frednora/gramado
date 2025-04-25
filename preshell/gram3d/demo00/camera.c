
#include "gram3d.h"


// Not using float.
struct gr_camera_d  *CurrentCamera;
// Using float.
struct gr_cameraF_d  CurrentCameraF;


// ==============================================
// Not using float.

int camera_initialize(void)
{
    CurrentCamera = (void *) malloc( sizeof( struct gr_camera_d ) );
    if ( (void*) CurrentCamera == NULL ){
        printf("camera_initialize: fail\n");
        exit(1);
    }

    CurrentCamera->initialized = FALSE;
    CurrentCamera->used = TRUE;
    CurrentCamera->magic = 1234;
// Position
    CurrentCamera->position.x = 0;
    CurrentCamera->position.y = 0;
    CurrentCamera->position.z = 0;
// Upview
    CurrentCamera->upview.x = 0;
    CurrentCamera->upview.y = 0;
    CurrentCamera->upview.z = 0;
// Lookat. Target point origin.
    CurrentCamera->lookat.x = 0;
    CurrentCamera->lookat.y = 0;
    CurrentCamera->lookat.z = 0;

    if ( (void*) CurrentProjection != NULL )
    {
        if (CurrentProjection->initialized == TRUE){
            CurrentCamera->projection = 
                (struct gr_projection_d *) CurrentProjection;
        }
    }

    CurrentCamera->initialized = TRUE;

    return 0;
}

int 
camera ( 
    int x, int y, int z,
    int xUp, int yUp, int zUp,
    int xLookAt, int yLookAt, int zLookAt )
{
// Update camera.

    if ( (void*) CurrentCamera == NULL ){
        return -1;
    }
    if (CurrentCamera->magic != 1234){
        return -1;
    }

// EYE:
// Position
    CurrentCamera->position.x = x;
    CurrentCamera->position.y = y;
    CurrentCamera->position.z = z;
// UP:
// Upview
    CurrentCamera->upview.x = xUp;
    CurrentCamera->upview.y = yUp;
    CurrentCamera->upview.z = zUp;
// AT:
// Lookat. target point origin.
    CurrentCamera->lookat.x = xLookAt;
    CurrentCamera->lookat.y = yLookAt;
    CurrentCamera->lookat.z = zLookAt;

    return 0;
}

int 
unveil_camera(
    int model_x, int model_y, int model_z )
{
// object window
    struct gws_window_d *ow;
    ow = NULL;
// #todo
// Use the demo window if it is possible
    if ( (void*) __demo_window != NULL ){
        if (__demo_window->magic==1234)
            ow = __demo_window;
    }

    struct gr_ray_d  r;

    r.used = TRUE;
    r.magic = 1234;

// origin
// reset
    r.p[RAY_ORIGIN].x = 0;
    r.p[RAY_ORIGIN].y = 0;
    r.p[RAY_ORIGIN].z = 0;
    r.p[RAY_ORIGIN].color = COLOR_WHITE;

// Where is our camera?
    if ( (void*) CurrentCamera == NULL ){
        return -1;
    }
    if (CurrentCamera->magic != 1234){
        return -1;
    }
    if (CurrentCamera->initialized != TRUE){
        return -1;
    }

//
// Ray
//

// --------------------------------
//ray: 0.0.0 ---> banco

// --------------------------------
// change the look at vector.
    CurrentCamera->lookat.x = model_x;
    CurrentCamera->lookat.y = model_y;
    CurrentCamera->lookat.z = model_z;

// --------------------------------
// draw the camera (circle)
    // camera circle
    grCircle3 ( 
        ow,
        CurrentCamera->position.x,  //x
        CurrentCamera->position.y,  //y
        8,          //r
        COLOR_RED,  //color 
        0 );        //z 
    //upview. (small circle).
    grCircle3 ( 
        ow,
        CurrentCamera->upview.x,  //x
        CurrentCamera->upview.y,  //y
        4,                        //r
        COLOR_RED,                //color 
        0 );                      //z 

// ---------------
// origin
// The position of the camera.
    r.p[RAY_ORIGIN].x = (int) CurrentCamera->position.x;
    r.p[RAY_ORIGIN].y = (int) CurrentCamera->position.y;
    r.p[RAY_ORIGIN].z = (int) CurrentCamera->position.z;
    r.p[RAY_ORIGIN].color = COLOR_WHITE;

// ==========================
// Draw the 3 axis
// Draw line (canonical in Gramado)
    int off=20;
    // y
    plotLine3d ( 
        ow,
        r.p[0].x,  r.p[0].y,        r.p[0].z, 
        r.p[0].x,  r.p[0].y + off,  r.p[0].z, 
        COLOR_RED ); 
    // x
    plotLine3d ( 
        ow,
        r.p[0].x,       r.p[0].y, r.p[0].z, 
        r.p[0].x + off, r.p[0].y, r.p[0].z, 
        COLOR_GREEN ); 
    // z
    plotLine3d ( 
        ow,
        r.p[0].x,       r.p[0].y,       r.p[0].z, 
        r.p[0].x -off,  r.p[0].y -off,  r.p[0].z, 
        COLOR_BLUE ); 

// ---------------
// look at vector
// target
    r.p[RAY_TARGET].x = (int) CurrentCamera->lookat.x; 
    r.p[RAY_TARGET].y = (int) CurrentCamera->lookat.y; 
    r.p[RAY_TARGET].z = (int) CurrentCamera->lookat.z;
    r.p[RAY_TARGET].color = COLOR_WHITE;
// Draw line (ray)
    plotLine3d ( 
        ow,
        r.p[RAY_ORIGIN].x,  r.p[RAY_ORIGIN].y, r.p[RAY_ORIGIN].z, 
        r.p[RAY_TARGET].x,  r.p[RAY_TARGET].y, r.p[RAY_TARGET].z, 
        r.p[0].color ); 

    return 0;
}

// ==============================================
// Using float.    

int cameraF_initialize(void)
{
// Using float.
    CurrentCameraF.initialized = FALSE;
// Position
    CurrentCameraF.position.x = (float) 0.0f;
    CurrentCameraF.position.y = (float) 0.0f;
    CurrentCameraF.position.z = (float) 0.0f;
// Upview
    CurrentCameraF.upview.x = (float) 0.0f;
    CurrentCameraF.upview.y = (float) 0.5f;   // # Up  :)
    CurrentCameraF.upview.z = (float) 0.0f;
// Lookat
    CurrentCameraF.lookat.x = (float) 0.0f;
    CurrentCameraF.lookat.y = (float) 0.0f;
    CurrentCameraF.lookat.z = (float) 0.0f;
    CurrentCameraF.initialized = TRUE;
    return 0;
}


