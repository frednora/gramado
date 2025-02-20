// proj.c
// Projection support.
// Integer and float.
// Created by Fred Nora.

#include "gram3d.h"


// ========================================
// Using float

// The projection structure.
// see: projf.h
struct gr_projectionF_d  CurrentProjectionF;

// The projection matrix Using float values.
// See: libs/libgr3d/include/grprim3d.h
struct gr_mat4x4_d matProj;

// ========================================
// Using integer

// The structure for graphical perspective projection.
// Do not use floating point.
// See: projint.h
struct gr_projection_d  *CurrentProjection;


// ===================================================

int projection_initialize(void)
{
// Not using float.

// Projection structure.
// See: projint.h
    struct gr_projection_d *proj;
    proj = (void *) malloc( sizeof(struct gr_projection_d) );
    if ((void*) proj == NULL){
        printf("projection_initialize: proj\n");
        goto FatalError;
    }
    proj->initialized = FALSE;
    // #todo: Perspective or orthogonal
    proj->type = 1; 

// Device context structure.
// gr_dc is the default device context structure.
    if ((void*) gr_dc == NULL){
        printf("projection_initialize: gr_dc\n");
        goto FatalError;
    }
    if (gr_dc->initialized != TRUE){
        printf("projection_initialize: gr_dc->initialized\n");
        goto FatalError;
    }
// Use the default dc.
    proj->dc = gr_dc;

//
// Orthographic projection plane
//

// #importante:
// Podemos alterar as informações do plano de projeção
// mas o plano obtido pelas características do dispositivo
// são dependentes do dispositivos.
// Do mesmo modo que podemos ter uma tela virtual.

    proj->l = gr_dc->absolute_x;
    proj->t = gr_dc->absolute_y;
    proj->w = gr_dc->width;
    proj->h = gr_dc->height;

    proj->r = gr_dc->right;
    proj->b = gr_dc->bottom;

// z range.
    proj->zNear  = gr_dc->znear;
    proj->zFar   = gr_dc->zfar;
    proj->zRange = gr_dc->zrange;

    //proj->angle_of_view = ?;
    //proj->ar = ?;
    //proj->frustrum_apex = ?;
    //proj->frustrum_view = ?;

    //...

    proj->used = TRUE;
    proj->magic = 1234;
    proj->initialized = TRUE;

// Save the current projection.
    CurrentProjection = (struct gr_projection_d *) proj;

    return 0;

FatalError:
// Fatal error.
// We can't survive without this.
    printf("projection_initialize: FAIL\n");
    exit (1);
    return (int) -1;
}

// Projection view.
// Changing the view for a given projection.
int view (struct gr_projection_d *projection, int near, int far)
{
// This structure uses Integer values.

// Parameters:
    if ((void*) projection == NULL){
        printf("view: projection\n");
        goto fail;
    }
    if (projection->initialized != TRUE){
        printf("view: initialized\n");
        goto fail;
    }

    // #ps
    // Far must be bigger than near.

// The projection info structure.
// Using Integer values.
    if ((void*) CurrentProjection == NULL){
        goto fail;
    }
    CurrentProjection->zNear  = (int) near;
    CurrentProjection->zFar   = (int) far;
    CurrentProjection->zRange = (int) (far - near); 
//done
    return 0;
fail:
    return (int) -1;
}

// Wrapper
int 
gr_depth_range(
    struct gr_projection_d *projection, 
    int near, 
    int far)
{
    return (int) view(projection,near,far);
}

// Wrapper
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
// Initializes the CurrentProjectionF structure.
// This structure uses Float values.
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
    float DefaultScaleFactor = (float) 0.5f;
    float fNear = (float) znear;  //0.1f;
    float fFar  = (float) zfar;   //1000.0f;
    float fFov = (float) fov;     //90.0f;

    CurrentProjectionF.initialized = FALSE;

// #ps
// The far must be bigger than near.

    CurrentProjectionF.znear = (float) znear;
    CurrentProjectionF.zfar  = (float) zfar;
    CurrentProjectionF.fov   = (float) fov;

//
// The Scale factor
//

// #todo: 
// We have a formula for this, the caller needs to respect it.
// % da tela.
    
    if ((float) scalefactor <= 0.0f){
        scalefactor = (float) DefaultScaleFactor;
    }
    CurrentProjectionF.scale_factor = (float) scalefactor;

// :::: The fov scaling factor. ::::
// This represents the scale factor but using radians instead of degree.
// fov in radient.
// Quanto menor for o angulo, maior será o objeto.
// 1/tan(fov/2)

    float fFovRad = 
        1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);
    //float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    // #bugbug
    // Actually this is our scale factor for x and y,
    // but this value is not working fine.
    // Se we are using somethig between 0.01f and 2.0f.
    //CurrentProjectionF.scale_factor = fFovRad;

//
// The aspect ratio
//

// Fatal error.
// Division by '0'.
    if (height == 0){
        goto fail;
    }
    float fAspectRatio = (float) width / (float) height;
    //float fAspectRatio = (float) 800 / (float) 600;
    //float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
    CurrentProjectionF.width = (unsigned long) (width & 0xFFFFFFFF);
    CurrentProjectionF.height = (unsigned long) (height & 0xFFFFFFFF);
    CurrentProjectionF.ar = (float) fAspectRatio;

//
// The projection matrix.
//

// This matrix was define on top of proj.c.
// See: 
// libs/libgr3d/include/grprim3d.h

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

// Done:
    CurrentProjectionF.initialized = TRUE;
    return 0;
fail:
    return (int) -1;
}

