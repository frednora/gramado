// proj.c
// Projection support.
// Created by Fred Nora.

#include "gram3d.h"

struct gr_projection_d  *CurrentProjection;


struct gr_mat4x4_d matProj;

// ===================================================

int projection_initialize(void)
{
// Not using float.

// See: 
// gr_projection_d

    CurrentProjection = (void *) malloc ( sizeof( struct gr_projection_d ) );
    if ( (void*) CurrentProjection == NULL ){
        printf("projection_initialize: CurrentProjection\n");
        exit(1);
    }

    if ( (void*) gr_dc == NULL){
        printf("projection_initialize: gr_dc\n");
        exit(1);
    }
    if (gr_dc->initialized != TRUE){
        printf("projection_initialize: gr_dc->initialized\n");
        exit(1);
    }

// Use the default dc.
    CurrentProjection->dc = gr_dc;
    CurrentProjection->initialized = FALSE;
    // #todo: Perspective or orthogonal
    CurrentProjection->type = 1; 

//
// Orthographic projection plane
//

// #importante:
// Podemos alterar as informações do plano de projeção
// mas o plano obtido pelas características do dispositivo
// são dependentes do dispositivos.
// Do mesmo modo que podemos ter uma tela virtual.

    CurrentProjection->l = gr_dc->absolute_x;
    CurrentProjection->t = gr_dc->absolute_y;
    CurrentProjection->w = gr_dc->width;
    CurrentProjection->h = gr_dc->height;

    CurrentProjection->r = gr_dc->right;
    CurrentProjection->b = gr_dc->bottom;

// z range.
    CurrentProjection->zNear  = gr_dc->znear;
    CurrentProjection->zFar   = gr_dc->zfar;
    CurrentProjection->zRange = gr_dc->zrange;

    //CurrentProjection->angle_of_view = ?;
    //CurrentProjection->ar = ?;
    //CurrentProjection->frustrum_apex = ?;
    //CurrentProjection->frustrum_view = ?;

    //...

    CurrentProjection->used = TRUE;
    CurrentProjection->magic = 1234;
    CurrentProjection->initialized = TRUE;

    return 0;
}




// Projection view.
// Changing the view for a given projection.
int view (struct gr_projection_d *projection, int near, int far)
{
    if ( (void*) projection == NULL ){
        printf("view: projection\n");
        return -1;
    }
    if (projection->initialized != TRUE){
        printf("view: initialized\n");
        return -1;
    }

    CurrentProjection->zNear  = (int) near;
    CurrentProjection->zFar   = (int) far;
    CurrentProjection->zRange = 
         (int) (CurrentProjection->zFar - CurrentProjection->zNear);

    return 0;
}

int 
gr_depth_range(
    struct gr_projection_d *projection, 
    int near, 
    int far)
{
    return (int) view(projection,near,far);
}

// Limites da profundidade usada pelo raster.
// See: view().
void 
gwsDepthRange(
    int minZ,     // near
    int maxZ)     // far
{
// Changing the view for the current projection
    gr_depth_range(CurrentProjection,minZ,maxZ);
}

// ======================================================
// Using float.

int 
grInitializeProjection(
    float znear, 
    float zfar, 
    float fov,
    unsigned long width,
    unsigned long height,
    float scalefactor )
{
// Projection Matrix
    float fNear = (float) znear;  //0.1f;
    float fFar  = (float) zfar;   //1000.0f;
    float fFov = (float) fov;     //90.0f;

    CurrentProjectionF.initialized = FALSE;

    CurrentProjectionF.znear = (float) znear;
    CurrentProjectionF.zfar  = (float) zfar;
    CurrentProjectionF.fov   = (float) fov;

// % da tela.
    if ( (float) scalefactor <= 0.0f ){
        scalefactor = (float) 0.5f;   // default
    }
    CurrentProjectionF.scale_factor = (float) scalefactor;

// fail
// Division by '0'.
    if (height == 0){
        return -1;
    }

    float fAspectRatio = (float) width / (float) height;
    //float fAspectRatio = (float) 800 / (float) 600;
    //float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();

    CurrentProjectionF.width = (unsigned long) (width & 0xFFFFFFFF);
    CurrentProjectionF.height = (unsigned long) (height & 0xFFFFFFFF);
    CurrentProjectionF.ar = (float) fAspectRatio;

// :::: The fov scaling factor. ::::
// Quanto menor for o angulo, maior será o objeto.
// fov in radient.
// 1/tan(fov/2)

    float fFovRad = 
        1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);
    //float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    // #bugbug
    // Actually this is our scale factor for x and y,
    // but this value is not working fine.
    // Se we are using somethig between 0.01f and 2.0f.
    //CurrentProjectionF.scale_factor = fFovRad;

    // afx
    matProj.m[0][0] = (float) (fAspectRatio * fFovRad);
    // fy
    matProj.m[1][1] = (float) fFovRad;
    // #todo: Here we are normalizing the z values.
    // The purpose is using values in a range of 0~1.
    matProj.m[2][2] = (float) (fFar / (fFar - fNear));
    //#bugbug: Talvez os 2 abaixo estão invertidos.
    matProj.m[2][3] = (float) 1.0f;
    matProj.m[3][2] = (float) ((-fFar * fNear) / (fFar - fNear));
    
    matProj.m[3][3] = (float) 0.0f;

    CurrentProjectionF.initialized = TRUE;

    return 0;
}









