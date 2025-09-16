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


/*
Convert FOV from degrees to radians:

fFov * 0.5f / 180.0f * 3.14159f: This part calculates half of the FOV in radians.
Calculate tangent of half FOV:

tanf(fFov * 0.5f / 180.0f * 3.14159f): Calculates the tangent of the half FOV in radians.
Invert the tangent value:

1.0f / tanf(...): Inverts the tangent value to obtain the desired fFovRad.

Half FOV: The tangent function is applied to half the FOV to calculate 
the distance from the projection plane to the viewing point.
*/

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
// Setup the projection matrix
// Called by grInit() in grprim.c.

// Default scale factor if not provided.
    float DefaultScaleFactor = (float) 0.5f;

// Convert input parameters to floats for consistency.
    float fNear = (float) znear;  //   0.1f;
    float fFar  = (float) zfar;   //1000.0f;
    float fFov = (float) fov;     //  90.0f;

// Initialize the projection structure.
    CurrentProjectionF.initialized = FALSE;
    CurrentProjectionF.znear = (float) znear;
    CurrentProjectionF.zfar  = (float) zfar;
    // #ps: Far must be bigger than near.
    CurrentProjectionF.fov = (float) fov;  // 90 degrees.

//
// The Scale factor
//

// Validate and set the scale factor.
// #todo: 
// We have a formula for this, the caller needs to respect it.
// % da tela.
    if ((float) scalefactor <= 0.0f){
        scalefactor = (float) DefaultScaleFactor;
    }
    CurrentProjectionF.scale_factor = (float) scalefactor;

// Calculate the field of view in radians.
// This is used for the projection matrix calculation.
// :::: The fov scaling factor. ::::
// This is the scale fator recorded into the projection matrix.
// This represents the scale factor but using radians instead of degree.
// Quanto menor for o angulo, maior será o objeto.
// Formula:   rad = degrees * (pi/180)
//            rad = (degree/180) * pi

    // Using Radians instead of degrees.
    //                         (     (degree/180)    *pi)
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);
    // Save it.
    CurrentProjectionF.fovRad = (float) fFovRad;

//
// The aspect ratio
//

// Calculate the aspect ratio of the viewport.

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

// -----------------------------------------------
// Construct the projection matrix.
// The projection matrix transforms 3D points into 2D coordinates on the screen.
// The projection matrix.
// This matrix was define on top of proj.c.
// See: 
// libs/libgr3d/include/grprim3d.h

// Top-left element: scales the x-coordinate based on aspect ratio and field of view.
// a*fx
    matProj.m[0][0] = (float) (fAspectRatio * fFovRad);

// Top-right element: scales the y-coordinate based on field of view.
//   fy
    matProj.m[1][1] = (float) fFovRad;

// Bottom-left and bottom-right elements:
// These elements are used to map the z-coordinate to the near and far clipping planes.
// #todo: Here we are normalizing the z values.
// The purpose is using values in a range of 0~1.
    matProj.m[2][2] = (float) (fFar / (fFar - fNear));
    //#bugbug: Talvez os 2 abaixo estão invertidos.
    matProj.m[2][3] = (float) 1.0f;
    matProj.m[3][2] = (float) ((-fFar * fNear) / (fFar - fNear));

// Bottom-right element: set to zero for perspective projection.
    matProj.m[3][3] = (float) 0.0f;

// Done:
    CurrentProjectionF.initialized = TRUE;
    return 0;
fail:
    return (int) -1;
}

