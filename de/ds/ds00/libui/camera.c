
// camera.c
// Created by Fred Nora.

#include "../gwsint.h"

struct gr_camera_d  *CurrentCamera;


// ======================================

int camera_initialize(void)
{
// Do not use float.

    CurrentCamera = (void *) malloc( sizeof(struct gr_camera_d) );
    if ((void*) CurrentCamera == NULL){
        printf("camera_initialize: fail\n");
        goto fail;
    }

    CurrentCamera->used = TRUE;
    CurrentCamera->magic = 1234;
    CurrentCamera->initialized = FALSE;
// position (0,0,0)
    CurrentCamera->position.x = 0;
    CurrentCamera->position.y = 0;
    CurrentCamera->position.z = 0;
// upview 
    CurrentCamera->upview.x = 0;
    CurrentCamera->upview.y = 20;
    CurrentCamera->upview.z = 0;
// lookat. target point origin.
    CurrentCamera->lookat.x = 0;
    CurrentCamera->lookat.y = 0;
    CurrentCamera->lookat.z = 0;

// Projection
    if ((void*) CurrentProjection != NULL)
    {
        if (CurrentProjection->initialized == TRUE){
            CurrentCamera->projection = 
                (struct gr_projection_d *) CurrentProjection;
        }
    }

    CurrentCamera->initialized = FALSE;
    return 0;

fail:
    // #debug
    exit(1);
    return (int) -1;
}

int 
camera ( 
    int x, int y, int z,
    int xUp, int yUp, int zUp,
    int xLookAt, int yLookAt, int zLookAt )
{
// Do not use float.

    if ((void *) CurrentCamera == NULL){
        printf("camera: fail\n");
        goto fail;
    }
// ----------------
// EYE:
// position
    CurrentCamera->position.x = x;
    CurrentCamera->position.y = y;
    CurrentCamera->position.z = z;
// ----------------
// UP:
// upview
    CurrentCamera->upview.x = xUp;
    CurrentCamera->upview.y = yUp;
    CurrentCamera->upview.z = zUp;
// ----------------
// AT:
// lookat. target point origin.
    CurrentCamera->lookat.x = xLookAt;
    CurrentCamera->lookat.y = yLookAt;
    CurrentCamera->lookat.z = zLookAt;

    return 0;
fail:
    return (int) -1;
}

