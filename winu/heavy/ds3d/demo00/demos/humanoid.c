// humanoid.c
// Humanoid demo.
// Created by Fred Nora.

#include "../gram3d.h"


static int game_update_taskbar=TRUE;
static int hits=0;


#define MODEL_MAX  8
unsigned long models[MODEL_MAX];

#define STATIC_MODEL_MAX  8
unsigned long static_models[STATIC_MODEL_MAX];


// local
/*
static int __projection4x4[4][4] = { 
        {1,0,0,0}, 
        {0,1,0,0},
        {0,0,0,0},
        {1,1,0,0}
        };
static int __m1[4][4] = { 
        {0,0,0,0}, 
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0}
        };
static int __r[4][4] = { 
        {0,0,0,0}, 
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0}
        };
*/



static void __drawHumanoidModel(struct humanoid_model_d *model, float fElapsedTime);
static void __drawEnemy(struct humanoid_model_d *model, float vel);
static void __drawMainCharacter(struct humanoid_model_d *model, float fElapsedTime);
static void __drawEnemy00(struct humanoid_model_d *model, float fElapsedTime);
static void __drawStaticModel(struct humanoid_model_d *model);

//======================

static void __drawHumanoidModel(struct humanoid_model_d *model, float fElapsedTime)
{
// No rotation. Small translation in positive z.

    char string0[16];

// Matrices
    struct gr_mat4x4_d  matRotX;
    struct gr_mat4x4_d  matRotY;
    struct gr_mat4x4_d  matRotZ;

// Triangles
    struct gr_triangleF3D_d  tri;            // triângulo original.
    struct gr_triangleF3D_d  triRotatedX; 
    struct gr_triangleF3D_d  triRotatedXY;
    struct gr_triangleF3D_d  triRotatedXYZ;

    int cull=FALSE;

    int nTriangles=12;

    int v_index=0;       // vertex index

// Loop iterators
    register int i=0;
    int j=0;

// ---------
// Initialize 4x4 matrices.
// see: gprim.h
    for (i=0; i<4; i++){
        for (j=0; j<4; j++){
            matRotZ.m[i][j] = (float) 0.0f;
            matRotX.m[i][j] = (float) 0.0f;
        };
    };

// ---------
    if( (void*) model == NULL ){
        return;
    }

// Building the transformation matrices.
// O angulo muda com o passar do tempo.
    //model->fThetaAngle = (float) (model->fThetaAngle + fElapsedTime);
    //model->fThetaAngle = (float) (model->fThetaAngle + 1.0f * fElapsedTime);

//------------------------------------------------
// Rotation X
// counter-clockwise
// R_x(θ) =
//|  1    0          0   |
//|  0   cos(θ)  -sin(θ) |
//|  0   sin(θ)   cos(θ) |
	matRotX.m[0][0] = (float) 1.0f;
	matRotX.m[1][1] = (float) cosf(model->fThetaAngle * 0.5f);
	matRotX.m[1][2] = (float) -sinf(model->fThetaAngle * 0.5f);
	matRotX.m[2][1] = (float) sinf(model->fThetaAngle * 0.5f);
	matRotX.m[2][2] = (float) cosf(model->fThetaAngle * 0.5f);
	matRotX.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Y
// counter-clockwise
// R_y(θ) =
//|  cos(θ)   0   sin(θ)  |
//|   0       1     0     |
//| -sin(θ)   0   cos(θ)  |
    matRotY.m[0][0] = cosf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[0][2] = sinf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[1][1] = (float) 1.0f;
    matRotY.m[2][0] = -sinf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[2][2] = cosf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Z
// counter-clockwise
//R_z(θ) =
//|  cos(θ)  -sin(θ)   0  |
//|  sin(θ)   cos(θ)   0  |
//|    0        0      1  |
	matRotZ.m[0][0] = (float) cosf(0.0f);//(cube->fThetaAngle);
	matRotZ.m[0][1] = (float) -sinf(0.0f);//(cube->fThetaAngle);
	matRotZ.m[1][0] = (float) sinf(0.0f);//(cube->fThetaAngle);
	matRotZ.m[1][1] = (float) cosf(0.0f);//(cube->fThetaAngle);
	matRotZ.m[2][2] = (float) 1.0f;
	matRotZ.m[3][3] = (float) 1.0f;


// 12 triangles.
// Order: north, top, south, bottom, east, west.
// clockwise? or ccw.

// ---------
// #test
// draw a rectangle
   //drawRectangle0((float) 0.08f);

// ---------
// draw a cube

    //cull=FALSE;

    nTriangles = model->face_count;
    for (i=1; i <= nTriangles; i++)
    {
        cull=FALSE;

        // Grab indices directly from the face 
        int i0 = model->faces[i].vi[0]; 
        int i1 = model->faces[i].vi[1]; 
        int i2 = model->faces[i].vi[2];

        // Build triangle 
        tri.p[0] = model->vecs[i0]; 
        tri.p[1] = model->vecs[i1]; 
        tri.p[2] = model->vecs[i2];

        // Assign colors if desired 
        //tri.p[0].color = COLOR_PINK; 
        //if (i >= 1 && i <= 12){
        //    tri.p[0].color = model->colors[i-1];
        //} 
        //tri.p[1].color = COLOR_WHITE; 
        //tri.p[2].color = COLOR_WHITE;

        tri.p[0].color = model->colors[i-1];
        tri.p[1].color = model->colors[i-1];
        tri.p[2].color = model->colors[i-1];


        //-----------------------------    
        // Rotate in X-Axis
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &tri.p[0], 
            (struct gr_vecF3D_d *) &triRotatedX.p[0], 
            (struct gr_mat4x4_d *) &matRotX);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &tri.p[1], 
            (struct gr_vecF3D_d *) &triRotatedX.p[1], 
            (struct gr_mat4x4_d *) &matRotX);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &tri.p[2], 
            (struct gr_vecF3D_d *) &triRotatedX.p[2], 
            (struct gr_mat4x4_d *) &matRotX);

        //-----------------------------    
        // Rotate in Y-Axis
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[0], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[0], 
            (struct gr_mat4x4_d *) &matRotY);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[1], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[1], 
            (struct gr_mat4x4_d *) &matRotY);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[2], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[2], 
            (struct gr_mat4x4_d *) &matRotY);

        //-----------------------------    
        // Rotate in Z-Axis
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[0], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[0], 
            (struct gr_mat4x4_d *) &matRotZ);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[1], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[1], 
            (struct gr_mat4x4_d *) &matRotZ);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[2], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[2], 
            (struct gr_mat4x4_d *) &matRotZ);


        triRotatedXYZ.p[0].color = tri.p[0].color;
        triRotatedXYZ.p[1].color = tri.p[1].color;
        triRotatedXYZ.p[2].color = tri.p[2].color;


        // Translate in z. (main_character)

        /*
        // Increment distance
        //cube->origin_z = (float) (cube->origin_z + 0.00005f);
        model->origin_z = 
            (float) ( 
                model->origin_z + 
                model->delta_z );
        */
  

        /*
        // #ps: Old hardcoded value.
        // Restart distance
        if (model->origin_z > 14.0f)
        {
            model->origin_z = (float) 0.8f;
            //hits++;
            //memset(string0,0,16);  //clear
            //itoa(hits,string0);
            //wm_Update_TaskBar((char *)string0,FALSE);
            //wm_Update_TaskBar("hit",FALSE);
        }
        */


        // #test
        // Testing the structure for world information.
        // Restart distance using world limits
        
        /*
        if (model->origin_z > current_world_3d->z_size)
        {
            model->origin_z = 0.8f;  // restart
        }
        */

        /*
        // Check the transformed z of one vertex (or all three)
        if (triRotatedXYZ.p[0].z > current_world_3d->z_size ||
            triRotatedXYZ.p[1].z > current_world_3d->z_size ||
            triRotatedXYZ.p[2].z > current_world_3d->z_size)
        {
            model->origin_z = 0.8f;  // restart
        }
        */

        triRotatedXYZ.p[0].z =
            (float) ( triRotatedXYZ.p[0].z + model->origin_z ); 
        triRotatedXYZ.p[1].z = 
            (float) ( triRotatedXYZ.p[1].z + model->origin_z ); 
        triRotatedXYZ.p[2].z = 
            (float) ( triRotatedXYZ.p[2].z + model->origin_z ); 

        // Translate in x.
        // left or right

        triRotatedXYZ.p[0].x = 
            (float) (triRotatedXYZ.p[0].x + model->origin_x); 
        triRotatedXYZ.p[1].x = 
            (float) (triRotatedXYZ.p[1].x + model->origin_x); 
        triRotatedXYZ.p[2].x = 
            (float) (triRotatedXYZ.p[2].x + model->origin_x); 

        // translate in y
        triRotatedXYZ.p[0].y = 
            (float) (triRotatedXYZ.p[0].y + model->origin_y); 
        triRotatedXYZ.p[1].y = 
            (float) (triRotatedXYZ.p[1].y + model->origin_y); 
        triRotatedXYZ.p[2].y = 
            (float) (triRotatedXYZ.p[2].y + model->origin_y); 

        //----------------------------------------------------
        // Use Cross-Product to get surface normal
        struct gr_vecF3D_d normal; 
        struct gr_vecF3D_d line1; 
        struct gr_vecF3D_d line2;

        line1.x = (float) triRotatedXYZ.p[1].x - triRotatedXYZ.p[0].x;
        line1.y = (float) triRotatedXYZ.p[1].y - triRotatedXYZ.p[0].y;
        line1.z = (float) triRotatedXYZ.p[1].z - triRotatedXYZ.p[0].z;

        line2.x = (float) triRotatedXYZ.p[2].x - triRotatedXYZ.p[0].x;
        line2.y = (float) triRotatedXYZ.p[2].y - triRotatedXYZ.p[0].y;
        line2.z = (float) triRotatedXYZ.p[2].z - triRotatedXYZ.p[0].z;

        normal.x = (float) (line1.y * line2.z - line1.z * line2.y);
        normal.y = (float) (line1.z * line2.x - line1.x * line2.z);
        normal.z = (float) (line1.x * line2.y - line1.y * line2.x);

        // It's normally normal to normalise the normal.
        float l = 
            (float) sqrt( (double)
                        ( normal.x*normal.x + 
                          normal.y*normal.y + 
                          normal.z*normal.z) );

        normal.x = (float) (normal.x/l); 
        normal.y = (float) (normal.y/l); 
        normal.z = (float) (normal.z/l);

        //#ok
        //if ( (float) normal.z <  0.0f){ cull=FALSE;}  //pinta
        //if ( (float) normal.z >= 0.0f){ cull=TRUE; }  //não pinta

        // #test
        // Considering the camera position.
        if (CurrentCameraF.initialized == FALSE){ return; }
        float tmp = 
             (float) (
             normal.x * (triRotatedXYZ.p[0].x - CurrentCameraF.position.x) + 
             normal.y * (triRotatedXYZ.p[0].y - CurrentCameraF.position.y) +
             normal.z * (triRotatedXYZ.p[0].z - CurrentCameraF.position.z) );
        if( (float) tmp <  0.0f){ cull=FALSE; }  //paint
        if( (float) tmp >= 0.0f){ cull=TRUE;  }  //do not paint
        //cull=FALSE;
        //----------------------------------------------------

        // We need a valid window, 
        // to use the rasterization features.
        // #test: Testing rasterization.
        // #todo: Return the number of changed pixels.
        // Nesse momento os valores dos vetores ainda não
        // estão grandes o bastante para usarmos
        // uma rotina 2D de rasterização.
        // Isso será feito pela rotina de contrução de triangulos.
        int fFillTriangle = TRUE;

        // The 'image space'.
        // Our image space is not 1:1:1
        // It's something like 2:2:1000
        // No z normalization
        // #bugbug
        // We have a scale factor do x and y.
        // But we do not have a scale factor for z.
        // So, z can be any vallur between 0.01f and 1000.0f.

        if (cull == FALSE)
        {
            if ((void*) __root_window != NULL)
            {
                plotTriangleF(
                    (struct gws_window_d *) __root_window, 
                    (struct gr_triangleF3D_d *) &triRotatedXYZ,
                    fFillTriangle,
                    0 );
            }
        }
    };
}

// Draw the cube.
// Elapsed time means the amount of time between two events.
// The cube is laid out in this classic sequence:
// Front Top Back Bottom Right Left
// For front:
// 1 bottom-front-left
// 2 bottom-front-right
// 3 top-front-left
// 4 top-front-right

static void __drawEnemy(struct humanoid_model_d *model, float vel)
{
    char string0[16];

// Matrices
    struct gr_mat4x4_d  matRotX;
    struct gr_mat4x4_d  matRotY;
    struct gr_mat4x4_d  matRotZ; 

// Triangles
    struct gr_triangleF3D_d  tri;            // Original triangle.
    struct gr_triangleF3D_d  triRotatedX;    // Rotate in X
    struct gr_triangleF3D_d  triRotatedXY;   // Rotate in Y
    struct gr_triangleF3D_d  triRotatedXYZ;  // Rotate in Z (Projected)

    int cull=FALSE;
    int nTriangles=12;

    int v_index=0; // Vertex index for the cube vectors.

// ---------

// Loop iterators
    register int i=0;
    int j=0;

// Initialize 4x4 matrices.
// see: gprim.h
    for (i=0; i<4; i++){
        for (j=0; j<4; j++){
            matRotZ.m[i][j] = (float) 0.0f;
            matRotX.m[i][j] = (float) 0.0f;
        };
    };

// ---------

    if ((void*) model == NULL){
        return;
    }

// Building the transformation matrices.

    //#todo
    //if( (float) fElapsedTime != (float) model->t ){
    //   fElapsedTime = (float) model->t;
    //}

    //float vel = (float) model->a * (float) model->t; 
    //float vel = (float) 1.0f * fElapsedTime;

    model->fThetaAngle = (float) (model->fThetaAngle + (float) vel);
    //model->fThetaAngle = (float) (model->fThetaAngle + (float) 1.0f * fElapsedTime);
    //model->fThetaAngle = (float) (model->fThetaAngle + 1.0f * fElapsedTime);

// ?
// Generating the matrices.
// Perceba que só atribuímos valores à matriz de rotação em X.
// Então logo abaixo quando efetuarmos as 3 possíveis rotações,
// o modelo fara rotação apenas em X.

//------------------------------------------------
// Rotation X
// counter-clockwise
// R_x(θ) =
//|  1    0          0   |
//|  0   cos(θ)  -sin(θ) |
//|  0   sin(θ)   cos(θ) |
    matRotX.m[0][0] = (float) 1.0f;
    matRotX.m[1][1] = (float) cosf(model->fThetaAngle * 0.5f);
    matRotX.m[1][2] = (float) -sinf(model->fThetaAngle * 0.5f);
    matRotX.m[2][1] = (float) sinf(model->fThetaAngle * 0.5f);
    matRotX.m[2][2] = (float) cosf(model->fThetaAngle * 0.5f);
    matRotX.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Y
// counter-clockwise
// R_y(θ) =
//|  cos(θ)   0   sin(θ)  |
//|   0       1     0     |
//| -sin(θ)   0   cos(θ)  |
    matRotY.m[0][0] = cosf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[0][2] = sinf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[1][1] = (float) 1.0f;
    matRotY.m[2][0] = -sinf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[2][2] = cosf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Z
// counter-clockwise
//R_z(θ) =
//|  cos(θ)  -sin(θ)   0  |
//|  sin(θ)   cos(θ)   0  |
//|    0        0      1  |
    matRotZ.m[0][0] = (float) cosf(0.0f);//(cube->fThetaAngle);
    matRotZ.m[0][1] = (float) -sinf(0.0f);//(cube->fThetaAngle);
    matRotZ.m[1][0] = (float) sinf(0.0f);//(cube->fThetaAngle);
    matRotZ.m[1][1] = (float) cosf(0.0f);//(cube->fThetaAngle);
    matRotZ.m[2][2] = (float) 1.0f;
    matRotZ.m[3][3] = (float) 1.0f;

//
// Surfaces
//

// Let's select what are the vectors for each one of the surfaces.
// Each 'face' of the triangle has three vector.
// Now we're selecting these indexes for these three vectors. 
// The vectors were created during the setup phase.

// We have 12 triangles. 
// (12*3) = 36 vectors.
// Order: north, top, south, bottom, east, west.
// Clockwise

// ---------
// #test
// draw a rectangle
   //drawRectangle0((float) 0.08f);

// - Loop --------------------------------
// Draw a list of triangles.
// A cube has 6 faces and 12 triangles.
// 1~12

    nTriangles = model->face_count;
    for (i=1; i <= nTriangles; i++)
    {
        cull=FALSE;

        // Grab indices directly from the face 
        int i0 = model->faces[i].vi[0]; 
        int i1 = model->faces[i].vi[1]; 
        int i2 = model->faces[i].vi[2];

        // Build triangle 
        tri.p[0] = model->vecs[i0]; 
        tri.p[1] = model->vecs[i1]; 
        tri.p[2] = model->vecs[i2];

        // Assign colors if desired 
        //tri.p[0].color = COLOR_PINK; 
        //if (i >= 1 && i <= 12){
            //tri.p[0].color = model->colors[i-1];
        //} 
        //tri.p[1].color = COLOR_WHITE; 
        //tri.p[2].color = COLOR_WHITE;

        tri.p[0].color = model->colors[i-1];
        tri.p[1].color = model->colors[i-1];
        tri.p[2].color = model->colors[i-1];

        // Now we have a triangle. A face.

        //-----------------------------
        // Rotate in X-Axis
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &tri.p[0], 
            (struct gr_vecF3D_d *) &triRotatedX.p[0], 
            (struct gr_mat4x4_d *) &matRotX);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &tri.p[1], 
            (struct gr_vecF3D_d *) &triRotatedX.p[1], 
            (struct gr_mat4x4_d *) &matRotX);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &tri.p[2], 
            (struct gr_vecF3D_d *) &triRotatedX.p[2], 
            (struct gr_mat4x4_d *) &matRotX);

        //-----------------------------
        // Rotate in Y-Axis
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[0], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[0], 
            (struct gr_mat4x4_d *) &matRotY);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[1], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[1], 
            (struct gr_mat4x4_d *) &matRotY);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[2], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[2], 
            (struct gr_mat4x4_d *) &matRotY);

        //-----------------------------
        // Rotate in Z-Axis
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[0], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[0], 
            (struct gr_mat4x4_d *) &matRotZ);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[1], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[1], 
            (struct gr_mat4x4_d *) &matRotZ);
        gr_MultiplyAndProjectVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[2], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[2], 
            (struct gr_mat4x4_d *) &matRotZ);

        // The color for the rotated triangle.
        // This is the original color.
        triRotatedXYZ.p[0].color = tri.p[0].color;
        triRotatedXYZ.p[1].color = tri.p[1].color;
        triRotatedXYZ.p[2].color = tri.p[2].color;

        // -z-------
        // Translate in z. (move)

        // Increment distance
        //model->origin_z = (float) (model->origin_z + 0.00005f);
        model->origin_z = 
            (float) ( model->origin_z + model->delta_z );

        // #test: Because each model has it's own delta.
        // Increment distance if we have a main_character.
        // if ((void*)main_character != NULL)
        //    model->origin_z = (float) main_character->origin_z;

        /*
        // Restart distance if we reached the limit in the z-axis.
        if (model->origin_z > 14.0f){
            model->origin_z = (float) 0.8f;
            //hits++;
            //memset(string0,0,16);  //clear
            //itoa(hits,string0);
            //wm_Update_TaskBar((char *)string0,FALSE);
            //wm_Update_TaskBar("hit",FALSE);
        }
        */

        // #test
        // Testing the structure for world information.
        // Restart distance using world limits

        if (model->origin_z > current_world_3d->z_size)
        {
            model->origin_z = 0.8f;  // restart
        }

        /*
        // Check the transformed z of one vertex (or all three)
        if (triRotatedXYZ.p[0].z > current_world_3d->z_size ||
            triRotatedXYZ.p[1].z > current_world_3d->z_size ||
            triRotatedXYZ.p[2].z > current_world_3d->z_size)
        {
            model->origin_z = 0.8f;  // restart
        }
        */


        // Change the z values in the triangle,
        // based on the the new z model position.

        triRotatedXYZ.p[0].z =
            (float) (triRotatedXYZ.p[0].z + model->origin_z ); 
        triRotatedXYZ.p[1].z = 
            (float) (triRotatedXYZ.p[1].z + model->origin_z );
        triRotatedXYZ.p[2].z = 
            (float) (triRotatedXYZ.p[2].z + model->origin_z ); 

        // Translate in x.
        // left or right

        //triRotatedXYZ.p[0].x = 
        //    (float) (triRotatedXYZ.p[0].x + model->origin_x); 
        //triRotatedXYZ.p[1].x = 
        //    (float) (triRotatedXYZ.p[1].x + model->origin_x); 
        //triRotatedXYZ.p[2].x = 
        //    (float) (triRotatedXYZ.p[2].x + model->origin_x); 

        // -x-------
        // Translate the triangle in x based in the terrain x position.
        // From the center, not from the top/left corner.
        // Because our 3D int engine assumes that.

        /*
        if ( (void*) main_character != NULL )
        {
            triRotatedXYZ.p[0].x = 
                (float) (triRotatedXYZ.p[0].x + main_character->origin_x + model->origin_x); 
            triRotatedXYZ.p[1].x = 
                (float) (triRotatedXYZ.p[1].x + main_character->origin_x + model->origin_x); 
            triRotatedXYZ.p[2].x = 
                (float) (triRotatedXYZ.p[2].x + main_character->origin_x + model->origin_x); 
        }
        */
            triRotatedXYZ.p[0].x = 
                (float) (triRotatedXYZ.p[0].x + model->origin_x); 
            triRotatedXYZ.p[1].x = 
                (float) (triRotatedXYZ.p[1].x + model->origin_x); 
            triRotatedXYZ.p[2].x = 
                (float) (triRotatedXYZ.p[2].x + model->origin_x); 

        // -y-------
        // Translate the triangle in y based in the main_character y position.
        // Coloca o cubo no chão do terreno.
        // From the center, not from the top/left corner.
        // Because our 3D int engine assumes that.

        /*
        if ( (void*) main_character != NULL )
        {
            triRotatedXYZ.p[0].y = 
                (float) (triRotatedXYZ.p[0].y + main_character->origin_y + model->origin_y); 
            triRotatedXYZ.p[1].y = 
                (float) (triRotatedXYZ.p[1].y + main_character->origin_y + model->origin_y); 
            triRotatedXYZ.p[2].y = 
                (float) (triRotatedXYZ.p[2].y + main_character->origin_y + model->origin_y); 
        }
        */
            triRotatedXYZ.p[0].y = 
                (float) (triRotatedXYZ.p[0].y + model->origin_y); 
            triRotatedXYZ.p[1].y = 
                (float) (triRotatedXYZ.p[1].y + model->origin_y); 
            triRotatedXYZ.p[2].y = 
                (float) (triRotatedXYZ.p[2].y + model->origin_y); 

        // ----------------------------------------------------
        // backface culling:
        // Normal vector for the triangle surface.
        // We wanna know if we need or not to draw this triangle.

        //----------------------------------------------------
        // Use Cross-Product to get surface normal.
        struct gr_vecF3D_d normal;
        struct gr_vecF3D_d line1;
        struct gr_vecF3D_d line2;

        // Vector 1 - Vector 0.
        line1.x = (float) triRotatedXYZ.p[1].x - triRotatedXYZ.p[0].x;
        line1.y = (float) triRotatedXYZ.p[1].y - triRotatedXYZ.p[0].y;
        line1.z = (float) triRotatedXYZ.p[1].z - triRotatedXYZ.p[0].z;

        // Vector 2 - Vector 0.
        line2.x = (float) triRotatedXYZ.p[2].x - triRotatedXYZ.p[0].x;
        line2.y = (float) triRotatedXYZ.p[2].y - triRotatedXYZ.p[0].y;
        line2.z = (float) triRotatedXYZ.p[2].z - triRotatedXYZ.p[0].z;

        // Normalize
        normal.x = (float) (line1.y * line2.z - line1.z * line2.y);
        normal.y = (float) (line1.z * line2.x - line1.x * line2.z);
        normal.z = (float) (line1.x * line2.y - line1.y * line2.x);

        // It's normally normal to normalise the normal.
        float l = 
            (float) sqrt( (double)
                        ( normal.x*normal.x + 
                          normal.y*normal.y + 
                          normal.z*normal.z ) );

        // Divide por um valor comum entre eles.
        // It is not shrinking the model. 
        // It’s a different kind of normalization:
        // This doesn’t change the vector’s direction — only its magnitude.
        normal.x = (float) (normal.x/l); 
        normal.y = (float) (normal.y/l); 
        normal.z = (float) (normal.z/l);

        //#ok
        //if ( (float) normal.z <  0.0f){ cull=FALSE;}  //pinta
        //if ( (float) normal.z >= 0.0f){ cull=TRUE; }  //não pinta

        // #test
        // Considering the camera position.
        // One method of implementing back-face culling is by 
        // discarding all triangles where the dot product of 
        // their surface normal and the camera-to-triangle 
        // vector is greater than or equal to zero.
        // In this case they are in the same direction.
        // That is not what we want. when the face of the triangle and 
        // the camera points to the same direcion we do not paint the triangle.
        // Nesse caso eles estão na mesma direção ou
        // são perpendiculares. Só queremos os vetores que
        // estão em direções opostas.
        // see:
        // https://en.wikipedia.org/wiki/Back-face_culling
        
        // No camera.
        if (CurrentCameraF.initialized == FALSE){
            return;
        }

        // Dot product.
        // Normal 'vezes' a distancia entre um dado vetor e a camera.
        // Dot product with camera direction
        // When the dot product is less than 0 it means that points to opposite directions ... 
        // maybe its the major rule.
        float tmp = 
             (float) (
             normal.x * (triRotatedXYZ.p[0].x - CurrentCameraF.position.x) + 
             normal.y * (triRotatedXYZ.p[0].y - CurrentCameraF.position.y) +
             normal.z * (triRotatedXYZ.p[0].z - CurrentCameraF.position.z) );
        // It needs to be in opposite direction. (negative).
        // Culling = abate.
        
        // Same direction or perpendicular.
        // Do not paint.
        if ( (float) tmp >= 0.0f ){ cull=TRUE;  }
        // Opposite direction.
        // Paint. (Não abate). Muuuuu.
        if ( (float) tmp < 0.0f ){ cull=FALSE; }

        //----------------------------------------------------

        // We need a valid window, 
        // to use the rasterization features.
        // #test: Testing rasterization.
        // #todo: Return the number of changed pixels.
        // Nesse momento os valores dos vetores ainda não
        // estão grandes o bastante para usarmos
        // uma rotina 2D de rasterização.
        // Isso será feito pela rotina de contrução de triangulos.
        int fFillTriangle = TRUE;

        // It means that the vectors are in opposite directions.
        // So, we're gonna paint this surface.
        // Muuuuu!
        if (cull == FALSE)
        {
            // The 'image space'.
            // Our image space is not 1:1:1
            // It's something like 2:2:1000
            // No z normalization
            // #bugbug
            // We have a scale factor do x and y.
            // But we do not have a scale factor for z.
            // So, z can be any vallur between 0.01f and 1000.0f.
            // #todo
            // Maybe this function can accept more parameters.
            if ((void*) __root_window != NULL)
            {
                plotTriangleF(
                    (struct gws_window_d *) __root_window, 
                    (struct gr_triangleF3D_d *) &triRotatedXYZ,
                    fFillTriangle,
                    0 );

            }
        }

    };  // loop: Number of triangles.
}

static void __drawMainCharacter(struct humanoid_model_d *model, float fElapsedTime)
{
    __drawHumanoidModel(
        (struct humanoid_model_d *) model,
        (float) fElapsedTime );
}

static void __drawEnemy00(struct humanoid_model_d *model, float fElapsedTime)
{
    if (!model) 
        return;

    __drawHumanoidModel(
        (struct humanoid_model_d *) model,
        (float) fElapsedTime );
}

static void __drawStaticModel(struct humanoid_model_d *model)
{
    if (!model) 
        return;

    //model->origin_z = 0.0f;

    // Force no movement
    //model->delta_z = 0.0f;

    // Draw with the worker
    __drawHumanoidModel(model, 0.0f);
}



// Control + arrow key
// It moves a given model.
// In: model id, direction, value.
void demoHumanoidMoveCharacter(int number, int direction, float value)
{
    struct humanoid_model_d *model;

// Model ID
    if (number < 0)
        return;
    if (number >= MODEL_MAX)
        return;
// Model structure
    model = (struct humanoid_model_d *) models[number];
    if ((void*) model == NULL)
        return;

// Move model
    // left
    if (direction == 1){
        model->origin_x = (float) (model->origin_x - value); 
    }
    // right
    if (direction == 2){
        model->origin_x = (float) (model->origin_x + value); 
    }
    // front
    if (direction == 3){
        model->origin_z = (float) (model->origin_z + value); 
    }
    // back
    if (direction == 4){
        model->origin_z = (float) (model->origin_z - value); 
    }

/*
// Move camera
    // left
    if(left_right == 1){
        CurrentCameraF.position.x = (float) (CurrentCameraF.position.x - value); 
    }
    // right
    if(left_right == 2){
        CurrentCameraF.position.x = (float) (CurrentCameraF.position.x + value); 
    }
*/
}

// Build, paint and display the frame.
// Called by the engine, by the function on_execute() in main.c.
// + Clear the surface 
// + Draw the frame.
//   background.
//   (main_character + 7 models).
//   It means 12*8 triangles.
// #todo:
// We're drawing the cube based on a static model
// given all the dots of this model.
// We need to create a function that will draw 3D models.

// Define cube geometry
// You store 8 vertices (cube->vecs) and 12 triangles (faces split into two triangles each).

void demoHumanoidDrawScene(unsigned long sec)
{
// The function on_execute() in main.c initilizes this demos
// and spins into a loop calling this function to draw all the scene.
// #todo: It means that of this demo was not initialized,
// we need to abort this function.

    struct humanoid_model_d *enemy;

    // #todo
    // This demo was initialized before calling this drawing routine?


// Begin time.
// Moved to the main loop of the server.
    //unsigned long gBeginTick = rtl_jiffies();

// Draw the main character
// Humanoid number 0.
    __drawMainCharacter(main_character, 0.0f);


// Static scenery 
    int i=0;
    struct humanoid_model_d *s_model;
    for (i = 0; i < STATIC_MODEL_MAX; i++) 
    {
        // Pick one
        s_model = (struct humanoid_model_d*) static_models[i]; 
        if (s_model != NULL) 
        {
            __drawStaticModel(s_model); 
            //__drawHumanoidModel(s_model, 0.0f); 
        } 
    };

// Draw all the enemies
// 1~n humanoids.
    register int n=1; // main_character =0
    while (1){

        if (n >= MODEL_MAX){
            break;
        }

        // Get a pointer for the next cube.
        enemy = (struct humanoid_model_d *) models[n];
        if ((void*) enemy == NULL){
            //printf("enemy\n");
            //exit(1);
            break;
        }

        //IN: cube number, direction, amount
        //FlyingCubeMove( n, 1, (float) 0.01f );

        // Acceletarion: How fast the velocity changes.
        // Each cube has it's own acceleration.
        // Cada cubo tem uma aceleração diferente.
        // Então, com o passar do tempo,
        // cada cubo tera um incremento diferente na sua velocidade.

        if (enemy != NULL)
        {
            enemy->t = (float) enemy->t + (float) sec * 0.1f;
            enemy->v = (float) enemy->t * enemy->a;  

            //__drawEnemy( 
            //    (struct humanoid_model_d *) enemy,
            //    (float) enemy->v );

            __drawEnemy00( 
                (struct humanoid_model_d *) enemy,
                (float) enemy->v );

        }

        n++;
    };

/*
// Static buildings 
    for (int i = 0; i < building_count; i++) { 
        __drawHumanoidModel(building_models[i], fElapsedTime); 
    }
*/

}

void demoUpdate(void)
{
    int i=0;
    struct humanoid_model_d *model;

    // Update only static models
    // Not the hero
    for (i = 1; i < MODEL_MAX; i++) 
    {
        model = (struct humanoid_model_d *) models[i];
        if (!model) continue;

        // Apply deltas
        model->origin_x += model->delta_x;
        model->origin_y += model->delta_y;
        model->origin_z += model->delta_z;

        // Optional: reset if they go out of bounds
        if (model->origin_z > current_world_3d->z_size) 
        {
            model->origin_z = DEFAULT_CUBE_INITIAL_Z_POSITION;
        }
    }
}

//
// #
// INITIALIZATION
//

// Called by the engine
// obs: 
// We have an embedded model into this function
// it's because we still don't have a function to read floating point data from
// a file. In the future we will have the model into a file and read it using our new function.
//

//Index 0 → main_character (the player).
//Index 1–7 → enemies (other humanoids).

void demoHumanoidSetup(void)
{
// This is called once.

// first cube
    struct humanoid_model_d *model;
    struct humanoid_model_d *s_model;  // static model

// Cube1
    register int i=0;


// The seqeunce values.
// These are the 12 faces in order.
     int seq[36] = { 
        1,2,4, 1,4,3, // north 
        3,4,6, 3,6,5, // top 
        5,6,8, 5,8,7, // south 
        7,8,2, 7,2,1, // bottom 
        2,8,6, 2,6,4, // east 
        7,1,3, 7,3,5  // west 
        };

    int seq_i=0;
    int seq_max = 12 * 3;


/*
    for (i=0; i<8; i++){
        cube_x[i] = (float) 0.0f;
        cube_y[i] = (float) 0.0f;
    };
*/

// Clear the list.
    for (i=0; i<MODEL_MAX; i++){
        models[i] = (unsigned long) 0;
    };
    for (i=0; i<STATIC_MODEL_MAX; i++){
        static_models[i] = (unsigned long) 0;
    };

    int count=0;
    int rand1=0;

// ==========================================================
// enemies

    for (count=0; count<MODEL_MAX; count++)
    {
        model = (void*) malloc( sizeof(struct humanoid_model_d) );
        if ((void*) model == NULL){
            printf("demoHumanoidSetup: model\n");
            exit(1);
        }

        // Create terrain
        if (count == 0){
            main_character = (struct humanoid_model_d *) model;
        }

        model->fThetaAngle = (float) 0.0f;
                
        // Initialize vectors
        //for (i=0; i<32; i++)
        for (i=0; i<128; i++)
        {
            model->vecs[i].x = (float) 0.0f;
            model->vecs[i].y = (float) 0.0f;
            model->vecs[i].z = (float) 0.0f;
        };

    
        // -- Test -----------------------------------------------------
        struct obj_element_d elem;
        //struct gr_vecF3D_d vertex;
        // Multi-line string containing vertex data.
        //const char *cubeData = "v 1.0 2.0 3.0 \n v 4.0 5.0 6.0 \n v 7.0 8.0 9.0 \n";
        
        /*
        // Original
        const char *cubeData =
            "v -0.2 -0.2  0.2\n"
            "v  0.2 -0.2  0.2\n"
            "v -0.2  0.2  0.2\n"
            "v  0.2  0.2  0.2\n"
            "v -0.2  0.2 -0.2\n"
            "v  0.2  0.2 -0.2\n"
            "v -0.2 -0.2 -0.2\n"
            "v  0.2 -0.2 -0.2\n";
        */

        /*
        // "tapered" or truncated-pyramid shape using eight vertices.
        // Same Vertex Count & Order. Different Geometry.
        const char *cubeData =
            "v -0.3 -0.2 0.3\n"   // Vertex 1: bottom front left (expanded base)
            "v 0.3 -0.2 0.3\n"    // Vertex 2: bottom front right (expanded base)
            "v -0.1 0.2 0.2\n"    // Vertex 3: top front left (contracted top)
            "v 0.1 0.2 0.2\n"     // Vertex 4: top front right (contracted top)
            "v -0.1 0.2 -0.2\n"   // Vertex 5: top back left (contracted top)
            "v 0.1 0.2 -0.2\n"    // Vertex 6: top back right (contracted top)
            "v -0.3 -0.2 -0.3\n"  // Vertex 7: bottom back left (expanded base)
            "v 0.3 -0.2 -0.3\n";  // Vertex 8: bottom back right (expanded base)
        */

        //const char *cubeData = (char *) demosReadFileIntoBuffer("cube.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("cube02.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("cube03.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("obj00.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("obj01.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("obj02.txt");
        const char *cubeData = (char *) demosReadFileIntoBuffer("obj02.txt");
        // ...
        if ((void*)cubeData == NULL){
            printf("on demosReadFileIntoBuffer()\n");
            exit(0);
        }
        const char *nextLine = cubeData;

        int VertexCounter = 1; 
        int FaceCounter = 1;
        do {
            //if (VertexCounter > 8)
                //break;
            const char *temp = 
                scan00_read_element_from_line(
                    nextLine, 
                    (struct obj_element_d *) &elem );
            // Process (print) the current vertex.
            //printf("Parsed Vertex: x = %f, y = %f, z = %f\n",
                //vertex.x, vertex.y, vertex.z);
            
            // Populate
            if (elem.initialized == TRUE)
            {
                if (elem.type == OBJ_ELEMENT_TYPE_VECTOR)
                {
                    model->vecs[VertexCounter].x = (float) elem.vertex.x;
                    model->vecs[VertexCounter].y = (float) elem.vertex.y;
                    model->vecs[VertexCounter].z = (float) elem.vertex.z;
                    VertexCounter++;
                }
                else if (elem.type == OBJ_ELEMENT_TYPE_FACE)
                {
                    model->faces[FaceCounter].vi[0] = elem.face.vi[0];
                    model->faces[FaceCounter].vi[1] = elem.face.vi[1];
                    model->faces[FaceCounter].vi[2] = elem.face.vi[2];
                    //model->face_count++; //#bugbug: It was naver initialized.
                    FaceCounter++;
                }
                // ...
            }

            nextLine = temp;

        } while (nextLine != NULL);

        // Register totals at the end
        model->vertex_count = VertexCounter - 1; // subtract wasted slot
        model->face_count = FaceCounter - 1;

        //printf ("v: %d  f: %d\n", model->vertex_count, model->face_count );
        //refresh_screen();
        //while(1){}

        /*
        int it=0;
        //for (it=0; it<FaceCounter; it++)
        for (it=1; it<(16+1); it++)
        {
            printf ("f: %d %d %d \n",
                model->faces[it].vi[0], 
                model->faces[it].vi[1], 
                model->faces[it].vi[2] );
        }
        printf ("break point\n");
        refresh_screen();
        while(1){}
        */
        // -------------------------------------------------------

        // The model for a regular model.
        // #todo: >> Load this from a file.
        // #todo: Maybe import these values from an array.
        // see: arrayFakeFile[]

        // Here we're creating the vectors for our model.
        // During the drawing phase we're gonna select vectors to create the triangles.
        // We have two triangles per surface,

        int it=0;

        // Head (faces 0–11)
        for (it=0; it<12; it++) model->colors[it] = 0xFFB6B6; //COLOR_RED;

        // Torso (faces 12–23)
        for (it=12; it<24; it++) model->colors[it] = 0xCBAACB; //COLOR_GREEN;

        // Left leg (faces 24–35)
        for (it=24; it<36; it++) model->colors[it] = 0xFFD8B1; //COLOR_BLUE;

        // Right leg (faces 36–47)
        for (it=36; it<48; it++) model->colors[it] = 0xFFD8B1; //COLOR_BLUE;

        // more 2 models is too much for a file with 1KB limitation.

        // Left arm (faces 48–59) (Not implemented)
        for (it=48; it<60; it++) model->colors[it] = COLOR_ORANGE;

        // Right arm (faces 60–71) (Not implemented)
        for (it=60; it<72; it++) model->colors[it] = COLOR_PURPLE;

    
        model->origin_x = 
            (float) -3.0f + (float) 0.8f * (float) count; // spread across X axis
        model->origin_y = (float) 0.0f;
        model->origin_z = (float) DEFAULT_CUBE_INITIAL_Z_POSITION; 

        // Translations ...
        model->delta_x = (float) 0.0f; 
        model->delta_y = (float) 0.0f; 
        model->delta_z = (float) DEFAULT_CUBE_INITIAL_DELTA_Z + 1.0f;

        
        // Initializing.
        // Cada cubo tem uma aceleração diferente.
        // Então, com o passar do tempo,
        // cada cubo tera um incremento diferente na sua velocidade.
        model->v = (float) count * 0.00001f;
        model->t = (float) 1.0f;
        model->a = (float) model->v / model->t;
        // v = a*t;

        // Save the model pointer.
        models[count] = (unsigned long) model;
    };

// =======================================================================
// Static models

    for (count=0; count<STATIC_MODEL_MAX; count++)
    {
        s_model = (void*) malloc( sizeof(struct humanoid_model_d) );
        if ((void*) s_model == NULL){
            printf("demoHumanoidSetup: s_model\n");
            exit(1);
        }

        s_model->fThetaAngle = (float) 0.0f;
       
        // Initialize vectors
        for (i=0; i<128; i++)
        {
            s_model->vecs[i].x = (float) 0.0f;
            s_model->vecs[i].y = (float) 0.0f;
            s_model->vecs[i].z = (float) 0.0f;
        };

    
        // -- Test -----------------------------------------------------
        struct obj_element_d elem;
        //struct gr_vecF3D_d vertex;
        // Multi-line string containing vertex data.
        //const char *cubeData = "v 1.0 2.0 3.0 \n v 4.0 5.0 6.0 \n v 7.0 8.0 9.0 \n";
        
        /*
        // Original
        const char *cubeData =
            "v -0.2 -0.2  0.2\n"
            "v  0.2 -0.2  0.2\n"
            "v -0.2  0.2  0.2\n"
            "v  0.2  0.2  0.2\n"
            "v -0.2  0.2 -0.2\n"
            "v  0.2  0.2 -0.2\n"
            "v -0.2 -0.2 -0.2\n"
            "v  0.2 -0.2 -0.2\n";
        */

        /*
        // "tapered" or truncated-pyramid shape using eight vertices.
        // Same Vertex Count & Order. Different Geometry.
        const char *cubeData =
            "v -0.3 -0.2 0.3\n"   // Vertex 1: bottom front left (expanded base)
            "v 0.3 -0.2 0.3\n"    // Vertex 2: bottom front right (expanded base)
            "v -0.1 0.2 0.2\n"    // Vertex 3: top front left (contracted top)
            "v 0.1 0.2 0.2\n"     // Vertex 4: top front right (contracted top)
            "v -0.1 0.2 -0.2\n"   // Vertex 5: top back left (contracted top)
            "v 0.1 0.2 -0.2\n"    // Vertex 6: top back right (contracted top)
            "v -0.3 -0.2 -0.3\n"  // Vertex 7: bottom back left (expanded base)
            "v 0.3 -0.2 -0.3\n";  // Vertex 8: bottom back right (expanded base)
        */

        //const char *cubeData = (char *) demosReadFileIntoBuffer("cube.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("cube02.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("cube03.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("obj00.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("obj01.txt");
        //const char *cubeData = (char *) demosReadFileIntoBuffer("obj02.txt");
        const char *cubeData = (char *) demosReadFileIntoBuffer("obj02.txt");
        // ...
        if ((void*)cubeData == NULL){
            printf("on demosReadFileIntoBuffer()\n");
            exit(0);
        }
        const char *nextLine = cubeData;

        int VertexCounter = 1; 
        int FaceCounter = 1;
        do {
            //if (VertexCounter > 8)
                //break;
            const char *temp = 
                scan00_read_element_from_line(
                    nextLine, 
                    (struct obj_element_d *) &elem );
            // Process (print) the current vertex.
            //printf("Parsed Vertex: x = %f, y = %f, z = %f\n",
                //vertex.x, vertex.y, vertex.z);
            
            // Populate
            if (elem.initialized == TRUE)
            {
                if (elem.type == OBJ_ELEMENT_TYPE_VECTOR)
                {
                    s_model->vecs[VertexCounter].x = (float) elem.vertex.x;
                    s_model->vecs[VertexCounter].y = (float) elem.vertex.y;
                    s_model->vecs[VertexCounter].z = (float) elem.vertex.z;
                    VertexCounter++;
                }
                else if (elem.type == OBJ_ELEMENT_TYPE_FACE)
                {
                    s_model->faces[FaceCounter].vi[0] = elem.face.vi[0];
                    s_model->faces[FaceCounter].vi[1] = elem.face.vi[1];
                    s_model->faces[FaceCounter].vi[2] = elem.face.vi[2];
                    //model->face_count++; //#bugbug: It was naver initialized.
                    FaceCounter++;
                }
                // ...
            }

            nextLine = temp;

        } while (nextLine != NULL);

        // Register totals at the end
        s_model->vertex_count = VertexCounter - 1; // subtract wasted slot
        s_model->face_count = FaceCounter - 1;

        //printf ("v: %d  f: %d\n", model->vertex_count, model->face_count );
        //refresh_screen();
        //while(1){}

        /*
        int it=0;
        //for (it=0; it<FaceCounter; it++)
        for (it=1; it<(16+1); it++)
        {
            printf ("f: %d %d %d \n",
                model->faces[it].vi[0], 
                model->faces[it].vi[1], 
                model->faces[it].vi[2] );
        }
        printf ("break point\n");
        refresh_screen();
        while(1){}
        */
        // -------------------------------------------------------

        // The model for a regular model.
        // #todo: >> Load this from a file.
        // #todo: Maybe import these values from an array.
        // see: arrayFakeFile[]

        // Here we're creating the vectors for our model.
        // During the drawing phase we're gonna select vectors to create the triangles.
        // We have two triangles per surface,

        int it=0;

        // Head (faces 0–11)
        for (it=0; it<12; it++) s_model->colors[it] = 0xC1E1C1; //COLOR_RED;

        // Torso (faces 12–23)
        for (it=12; it<24; it++) s_model->colors[it] = 0xF5F5DC; //COLOR_GREEN;

        // Left leg (faces 24–35)
        for (it=24; it<36; it++) s_model->colors[it] = 0xD3D3D3; //COLOR_BLUE;

        // Right leg (faces 36–47)
        for (it=36; it<48; it++) s_model->colors[it] = 0xD3D3D3; //COLOR_BLUE;

        // more 2 models is too much for a file with 1KB limitation.

        // Left arm (faces 48–59) (Not implemented)
        for (it=48; it<60; it++) s_model->colors[it] = COLOR_ORANGE;

        // Right arm (faces 60–71) (Not implemented)
        for (it=60; it<72; it++) s_model->colors[it] = COLOR_PURPLE;

        s_model->origin_x = 
            (float) -3.0f + (float) 0.8f * (float) count; // spread across X axis
        s_model->origin_y = (float) -1.2f;
        float factor = (float) count;
        s_model->origin_z = 
            (float) DEFAULT_CUBE_INITIAL_Z_POSITION + (8.0f * factor); 

        // Translations ...
        s_model->delta_x = (float) 0.0f;
        s_model->delta_y = (float) 0.0f;
        s_model->delta_z = (float) 0.0f; 

        // Initializing.
        // Cada cubo tem uma aceleração diferente.
        // Então, com o passar do tempo,
        // cada cubo tera um incremento diferente na sua velocidade.
        s_model->v = (float) count * 0.00001f;
        s_model->t = (float) 1.0f;
        s_model->a = (float) s_model->v / s_model->t;
        // v = a*t;

        // Save the model pointer.
        static_models[count] = (unsigned long) s_model;
    };


// ========================================================================
// Hero
// Special values for the hero.
    if ( (void*) main_character != NULL )
    {
        int iter=0;

        // Head (faces 0–11)
        for (iter=0; iter<12; iter++) main_character->colors[iter] = 0xA7C7E7; //COLOR_BLUE;

        // Torso (faces 12–23)
        for (iter=12; iter<24; iter++) main_character->colors[iter] = 0xFFFACD; //COLOR_PINK;

        // Left leg (faces 24–35)
        for (iter=24; iter<36; iter++) main_character->colors[iter] = 0xFFD1DC; //COLOR_PURPLE;

        // Right leg (faces 36–47)
        for (iter=36; iter<48; iter++) main_character->colors[iter] = 0xFFD1DC; //COLOR_PURPLE;

        // more 2 models is too much for a file with 1KB limitation.

        // Left arm (faces 48–59) (Not implemented)
        for (iter=48; iter<60; iter++) main_character->colors[iter] = COLOR_ORANGE;

        // Right arm (faces 60–71) (Not implemented)
        for (iter=60; iter<72; iter++) main_character->colors[iter] = COLOR_PURPLE;


        main_character->origin_x = (float)  0.0f;  // center horizontally
        main_character->origin_y = (float) -3.0f;  // slightly lower (ground level)
        main_character->origin_z = (float) (float) DEFAULT_CUBE_INITIAL_Z_POSITION + 1.0f;  // visible depth

        // Translations
        main_character->delta_x = (float) 0.0f;
        main_character->delta_y = (float) 0.0f;
        main_character->delta_z = (float) DEFAULT_CUBE_INITIAL_DELTA_Z;

        // Initializing.
        // Cada cubo tem uma aceleração diferente.
        // Então, com o passar do tempo,
        // cada cubo tera um incremento diferente na sua velocidade.
        main_character->v = (float) count * 0.00001f;
        main_character->t = (float) 1.0f;
        main_character->a = (float) main_character->v / main_character->t;
        // v = a*t;
    }

//----------------
// Taskbar
    //demoClearWA(COLOR_BLACK);
    //wm_Update_TaskBar("Hello",TRUE);
    game_update_taskbar = FALSE;


//
// World
//

    if ((void *) current_world_3d == NULL)
    {
        printf("current_world_3d\n");
        exit(0);
    }
    if (current_world_3d->magic != 1234)
    {
        printf("current_world_3d magic\n");
        exit(0);
    }
}

