
// proj.c
// Projection support.
// Created by Fred Nora.

#include "../gwsint.h"

struct gr_projection_d  *CurrentProjection;

//===============================

// See:
int projection_initialize(void)
{

// See: gr_projection_d
    CurrentProjection = (void *) malloc( sizeof(struct gr_projection_d) );
    if ((void *) CurrentProjection == NULL){
        printf("projection_initialize fail\n");
        exit(1);
    }
    CurrentProjection->initialized = FALSE;
    // #todo: Perspective or orthogonal
    CurrentProjection->type = 1; 

    // ??
    CurrentProjection->zNear  =  0;
    CurrentProjection->zFar   = 40;
    CurrentProjection->zRange = 
        (CurrentProjection->zFar - CurrentProjection->zNear);

    //CurrentProjection->angle_of_view = ?;
    //CurrentProjection->ar = ?;
    //CurrentProjection->frustrum_apex = ?;
    //CurrentProjection->frustrum_view = ?;

    //...
    
    CurrentProjection->dc = NULL;
    // Default dc.
    if ((void*) gr_dc != NULL){
        CurrentProjection->dc = gr_dc;
    }

    CurrentProjection->used = TRUE;
    CurrentProjection->magic = 1234;
    CurrentProjection->initialized = TRUE;
    return 0;
}

// Changing the view for a given projection.
int view(struct gr_projection_d *projection, int near, int far)
{
    if ((void *) projection == NULL){
        printf("view: projection\n");
        goto fail;
    }
    if (projection->initialized != TRUE){
        printf("view: initialized\n");
        goto fail;
    }
    CurrentProjection->zNear = (int) near;
    CurrentProjection->zFar = (int) far;
    CurrentProjection->zRange = 
         (int) (CurrentProjection->zFar - CurrentProjection->zNear);

    return 0;
fail:
    return (int) -1;
}

int 
gr_depth_range(
    struct gr_projection_d *projection, 
    int near, 
    int far)
{
    return (int) view(projection,near,far);
}

