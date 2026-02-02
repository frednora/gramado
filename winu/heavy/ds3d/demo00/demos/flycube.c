// demos.c
// A place for demos.
// Created by Fred Nora.

#include "../gram3d.h"

// for open()
//#include <fcntl.h>
//#include <unistd.h>


static int game_update_taskbar=TRUE;

static int hits=0;

// For the demo.
#define CUBE_MAX  8
unsigned long cubes[CUBE_MAX];

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


static void __drawTerrain(struct cube_model_d *cube, float fElapsedTime);
static void __drawFlyingCube(struct cube_model_d *cube, float vel);


//======================



static void __drawTerrain(struct cube_model_d *cube, float fElapsedTime)
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
    if( (void*) cube == NULL ){
        return;
    }

// Building the transformation matrices.
// O angulo muda com o passar do tempo.
    //cube->fThetaAngle = (float) (cube->fThetaAngle + fElapsedTime);
    //cube->fThetaAngle = (float) (cube->fThetaAngle + 1.0f * fElapsedTime);

//------------------------------------------------
// Rotation X
// counter-clockwise
// R_x(θ) =
//|  1    0          0   |
//|  0   cos(θ)  -sin(θ) |
//|  0   sin(θ)   cos(θ) |
	matRotX.m[0][0] = (float) 1.0f;
	matRotX.m[1][1] = (float) cosf(cube->fThetaAngle * 0.5f);
	matRotX.m[1][2] = (float) -sinf(cube->fThetaAngle * 0.5f);
	matRotX.m[2][1] = (float) sinf(cube->fThetaAngle * 0.5f);
	matRotX.m[2][2] = (float) cosf(cube->fThetaAngle * 0.5f);
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

    nTriangles = cube->face_count;
    for (i=1; i <= nTriangles; i++)
    {
        cull=FALSE;

        // Grab indices directly from the face 
        int i0 = cube->faces[i].vi[0]; 
        int i1 = cube->faces[i].vi[1]; 
        int i2 = cube->faces[i].vi[2];

        // Build triangle 
        tri.p[0] = cube->vecs[i0]; 
        tri.p[1] = cube->vecs[i1]; 
        tri.p[2] = cube->vecs[i2];

        // Assign colors if desired 
        tri.p[0].color = COLOR_PINK; 
        if (i >= 1 && i <= 12){
            tri.p[0].color = cube->colors[i-1];
        } 
        tri.p[1].color = COLOR_WHITE; 
        tri.p[2].color = COLOR_WHITE;


        //-----------------------------    
        // Rotate in X-Axis
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &tri.p[0], 
            (struct gr_vecF3D_d *) &triRotatedX.p[0], 
            (struct gr_mat4x4_d *) &matRotX);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &tri.p[1], 
            (struct gr_vecF3D_d *) &triRotatedX.p[1], 
            (struct gr_mat4x4_d *) &matRotX);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &tri.p[2], 
            (struct gr_vecF3D_d *) &triRotatedX.p[2], 
            (struct gr_mat4x4_d *) &matRotX);

        //-----------------------------    
        // Rotate in Y-Axis
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[0], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[0], 
            (struct gr_mat4x4_d *) &matRotY);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[1], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[1], 
            (struct gr_mat4x4_d *) &matRotY);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[2], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[2], 
            (struct gr_mat4x4_d *) &matRotY);

        //-----------------------------    
        // Rotate in Z-Axis
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[0], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[0], 
            (struct gr_mat4x4_d *) &matRotZ);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[1], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[1], 
            (struct gr_mat4x4_d *) &matRotZ);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[2], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[2], 
            (struct gr_mat4x4_d *) &matRotZ);


        triRotatedXYZ.p[0].color = tri.p[0].color;
        triRotatedXYZ.p[1].color = tri.p[1].color;
        triRotatedXYZ.p[2].color = tri.p[2].color;


        // Translate in z. (terrain)

        // Increment distance
        //cube->model_distance = (float) (cube->model_distance + 0.00005f);
        cube->model_distance = 
            (float) ( 
                cube->model_distance + 
                cube->model_distance_delta );
        
        // Restart distance
        if (cube->model_distance > 14.0f){
            cube->model_distance = (float) 0.8f;
            //hits++;
            //memset(string0,0,16);  //clear
            //itoa(hits,string0);
            //wm_Update_TaskBar((char *)string0,FALSE);
            //wm_Update_TaskBar("hit",FALSE);
        }

        triRotatedXYZ.p[0].z =
            (float) (
            triRotatedXYZ.p[0].z + 
            cube->model_initial_distance +
            cube->model_distance ); 
        triRotatedXYZ.p[1].z = 
            (float) (
            triRotatedXYZ.p[1].z + 
            cube->model_initial_distance +
            cube->model_distance ); 

        triRotatedXYZ.p[2].z = 
            (float) (
            triRotatedXYZ.p[2].z + 
            cube->model_initial_distance +
            cube->model_distance ); 

        // Translate in x.
        // left or right

        triRotatedXYZ.p[0].x = 
            (float) (triRotatedXYZ.p[0].x + cube->hposition); 
        triRotatedXYZ.p[1].x = 
            (float) (triRotatedXYZ.p[1].x + cube->hposition); 
        triRotatedXYZ.p[2].x = 
            (float) (triRotatedXYZ.p[2].x + cube->hposition); 

        // translate in y
        triRotatedXYZ.p[0].y = 
            (float) (triRotatedXYZ.p[0].y + cube->vposition); 
        triRotatedXYZ.p[1].y = 
            (float) (triRotatedXYZ.p[1].y + cube->vposition); 
        triRotatedXYZ.p[2].y = 
            (float) (triRotatedXYZ.p[2].y + cube->vposition); 

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

static void __drawFlyingCube(struct cube_model_d *cube, float vel)
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

    if ((void*) cube == NULL){
        return;
    }

// Building the transformation matrices.

    //#todo
    //if( (float) fElapsedTime != (float) cube->t ){
    //   fElapsedTime = (float) cube->t;
    //}

    //float vel = (float) cube->a * (float) cube->t; 
    //float vel = (float) 1.0f * fElapsedTime;

    cube->fThetaAngle = (float) (cube->fThetaAngle + (float) vel);
    //cube->fThetaAngle = (float) (cube->fThetaAngle + (float) 1.0f * fElapsedTime);
    //cube->fThetaAngle = (float) (cube->fThetaAngle + 1.0f * fElapsedTime);

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
    matRotX.m[1][1] = (float) cosf(cube->fThetaAngle * 0.5f);
    matRotX.m[1][2] = (float) -sinf(cube->fThetaAngle * 0.5f);
    matRotX.m[2][1] = (float) sinf(cube->fThetaAngle * 0.5f);
    matRotX.m[2][2] = (float) cosf(cube->fThetaAngle * 0.5f);
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

    nTriangles = cube->face_count;
    for (i=1; i <= nTriangles; i++)
    {
        cull=FALSE;

        // Grab indices directly from the face 
        int i0 = cube->faces[i].vi[0]; 
        int i1 = cube->faces[i].vi[1]; 
        int i2 = cube->faces[i].vi[2];

        // Build triangle 
        tri.p[0] = cube->vecs[i0]; 
        tri.p[1] = cube->vecs[i1]; 
        tri.p[2] = cube->vecs[i2];

        // Assign colors if desired 
        //tri.p[0].color = COLOR_PINK; 
        //if (i >= 1 && i <= 12){
            //tri.p[0].color = cube->colors[i-1];
        //} 
        //tri.p[1].color = COLOR_WHITE; 
        //tri.p[2].color = COLOR_WHITE;

        tri.p[0].color = cube->colors[i-1];
        tri.p[1].color = cube->colors[i-1];
        tri.p[2].color = cube->colors[i-1];

        // Now we have a triangle. A face.

        //-----------------------------
        // Rotate in X-Axis
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &tri.p[0], 
            (struct gr_vecF3D_d *) &triRotatedX.p[0], 
            (struct gr_mat4x4_d *) &matRotX);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &tri.p[1], 
            (struct gr_vecF3D_d *) &triRotatedX.p[1], 
            (struct gr_mat4x4_d *) &matRotX);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &tri.p[2], 
            (struct gr_vecF3D_d *) &triRotatedX.p[2], 
            (struct gr_mat4x4_d *) &matRotX);

        //-----------------------------
        // Rotate in Y-Axis
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[0], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[0], 
            (struct gr_mat4x4_d *) &matRotY);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[1], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[1], 
            (struct gr_mat4x4_d *) &matRotY);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedX.p[2], 
            (struct gr_vecF3D_d *) &triRotatedXY.p[2], 
            (struct gr_mat4x4_d *) &matRotY);

        //-----------------------------
        // Rotate in Z-Axis
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[0], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[0], 
            (struct gr_mat4x4_d *) &matRotZ);
        gr_MultiplyMatrixVector(
            (struct gr_vecF3D_d *) &triRotatedXY.p[1], 
            (struct gr_vecF3D_d *) &triRotatedXYZ.p[1], 
            (struct gr_mat4x4_d *) &matRotZ);
        gr_MultiplyMatrixVector(
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
        //cube->model_distance = (float) (cube->model_distance + 0.00005f);
        cube->model_distance = 
            (float) ( 
                cube->model_distance + 
                cube->model_distance_delta );

        // #test: Because each cube has it's own delta.
        // Increment distance if we have a terrain.
        // if ((void*)terrain != NULL)
        //    cube->model_distance = (float) terrain->model_distance;

        // Restart distance if we reached the limit in the z-axis.
        if (cube->model_distance > 14.0f){
            cube->model_distance = (float) 0.8f;
            //hits++;
            //memset(string0,0,16);  //clear
            //itoa(hits,string0);
            //wm_Update_TaskBar((char *)string0,FALSE);
            //wm_Update_TaskBar("hit",FALSE);
        }

        // Change the z values in the triangle,
        // based on the the new z model position.

        triRotatedXYZ.p[0].z =
            (float) (
            triRotatedXYZ.p[0].z + 
            cube->model_initial_distance + 
            cube->model_distance ); 
        triRotatedXYZ.p[1].z = 
            (float) (
            triRotatedXYZ.p[1].z + 
            cube->model_initial_distance +
            cube->model_distance );
        triRotatedXYZ.p[2].z = 
            (float) (
            triRotatedXYZ.p[2].z + 
            cube->model_initial_distance +
            cube->model_distance ); 

        // Translate in x.
        // left or right

        //triRotatedXYZ.p[0].x = 
        //    (float) (triRotatedXYZ.p[0].x + cube->hposition); 
        //triRotatedXYZ.p[1].x = 
        //    (float) (triRotatedXYZ.p[1].x + cube->hposition); 
        //triRotatedXYZ.p[2].x = 
        //    (float) (triRotatedXYZ.p[2].x + cube->hposition); 

        // -x-------
        // Translate the triangle in x based in the terrain x position.
        // From the center, not from the top/left corner.
        // Because our 3D int engine assumes that.

        if ( (void*) terrain != NULL )
        {
            triRotatedXYZ.p[0].x = 
                (float) (triRotatedXYZ.p[0].x + terrain->hposition + cube->hposition); 
            triRotatedXYZ.p[1].x = 
                (float) (triRotatedXYZ.p[1].x + terrain->hposition + cube->hposition); 
            triRotatedXYZ.p[2].x = 
                (float) (triRotatedXYZ.p[2].x + terrain->hposition + cube->hposition); 
        }

        // -y-------
        // Translate the triangle in y based in the terrain y position.
        // Coloca o cubo no chão do terreno.
        // From the center, not from the top/left corner.
        // Because our 3D int engine assumes that.

        if ( (void*) terrain != NULL )
        {
            triRotatedXYZ.p[0].y = 
                (float) (triRotatedXYZ.p[0].y + terrain->vposition + cube->vposition); 
            triRotatedXYZ.p[1].y = 
                (float) (triRotatedXYZ.p[1].y + terrain->vposition + cube->vposition); 
            triRotatedXYZ.p[2].y = 
                (float) (triRotatedXYZ.p[2].y + terrain->vposition + cube->vposition); 
        }

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

        // Normalize.
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

// Control + arrow key
void FlyingCubeMove(int number, int direction, float value)
{
    struct cube_model_d *cube;

    if (number < 0)
        return;
    if (number >= CUBE_MAX)
        return;
    cube = (struct cube_model_d *) cubes[number];
    if ((void*) cube == NULL)
        return;

// Move model
    // left
    if (direction == 1){
        cube->hposition = (float) (cube->hposition - value); 
    }
    // right
    if (direction == 2){
        cube->hposition = (float) (cube->hposition + value); 
    }
    // front
    if (direction == 3){
        cube->model_distance = (float) (cube->model_distance + value); 
    }
    // back
    if (direction == 4){
        cube->model_distance = (float) (cube->model_distance - value); 
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
//   (terrain + 7 cubes).
//   It means 12*8 triangles.
// #todo:
// We're drawing the cube based on a static model
// given all the dots of this model.
// We need to create a function that will draw 3D cubes.

// Define cube geometry
// You store 8 vertices (cube->vecs) and 12 triangles (faces split into two triangles each).

void demoFlyingCube(int draw_terrain,unsigned long sec)
{
// The function on_execute() in main.c initilizes this demos
// and spins into a loop calling this function to draw
// all the scene.
// #todo: It means that of this demo was not initialized,
// we need to abort this function.

    struct cube_model_d *tmp_cube;

    // #todo
    // This demo was initialized before calling this drawing routine?


// Begin time.
// Moved to the main loop of the server.
    //unsigned long gBeginTick = rtl_jiffies();


// -------------------------
// Draw terrain.
// No rotation. Small translation in positive z.
// 12 triangles.
    if (draw_terrain == TRUE){
        __drawTerrain(terrain,0.0f);
    }

//- Loop ------------------------------
// Draw all the cubes.
// (12*7) triangles.
    register int n=1; // terrain =0
    while (1){

        if (n >= CUBE_MAX){
            break;
        }

        // Get a pointer for the next cube.
        tmp_cube = (struct cube_model_d *) cubes[n];
        if ((void*) tmp_cube == NULL){
            //printf("tmp_cube\n");
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

        if (tmp_cube != NULL)
        {
            tmp_cube->t = (float) tmp_cube->t + (float) sec * 0.1f;
            tmp_cube->v = (float) tmp_cube->t * tmp_cube->a;  
            
            __drawFlyingCube( 
                (struct cube_model_d *) tmp_cube,
                (float) tmp_cube->v );
        }

        n++;
    };
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
void demoFlyingCubeSetup(void)
{
// This is called once.

// first cube
    struct cube_model_d *cube;
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
    for (i=0; i<CUBE_MAX; i++){
        cubes[i] = (unsigned long) 0;
    };

    int count=0;
    int rand1=0;
    
    for (count=0; count<CUBE_MAX; count++)
    {
        cube = (void*) malloc( sizeof(struct cube_model_d) );
        if ((void*) cube == NULL){
            printf("cube\n");
            exit(1);
        }

        // Create terrain
        if (count == 0){
            terrain = (struct cube_model_d *) cube;
        }

        cube->fThetaAngle = (float) 0.0f;
                
        // Initialize vectors.
        for (i=0; i<32; i++)
        {
            cube->vecs[i].x = (float) 0.0f;
            cube->vecs[i].y = (float) 0.0f;
            cube->vecs[i].z = (float) 0.0f;
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
                    cube->vecs[VertexCounter].x = (float) elem.vertex.x;
                    cube->vecs[VertexCounter].y = (float) elem.vertex.y;
                    cube->vecs[VertexCounter].z = (float) elem.vertex.z;
                    VertexCounter++;
                }
                else if (elem.type == OBJ_ELEMENT_TYPE_FACE)
                {
                    cube->faces[FaceCounter].vi[0] = elem.face.vi[0];
                    cube->faces[FaceCounter].vi[1] = elem.face.vi[1];
                    cube->faces[FaceCounter].vi[2] = elem.face.vi[2];
                    //cube->face_count++; //#bugbug: It was naver initialized.
                    FaceCounter++;
                }
                // ...
            }

            nextLine = temp;

        } while (nextLine != NULL);

        // Register totals at the end
        cube->vertex_count = VertexCounter - 1; // subtract wasted slot
        cube->face_count = FaceCounter - 1;

        //printf ("v: %d  f: %d\n", cube->vertex_count, cube->face_count );
        //refresh_screen();
        //while(1){}

        /*
        int it=0;
        //for (it=0; it<FaceCounter; it++)
        for (it=1; it<(16+1); it++)
        {
            printf ("f: %d %d %d \n",
                cube->faces[it].vi[0], 
                cube->faces[it].vi[1], 
                cube->faces[it].vi[2] );
        }
        printf ("break point\n");
        refresh_screen();
        while(1){}
        */
        // -------------------------------------------------------

        // The model for a regular cube.
        // #todo: >> Load this from a file.
        // #todo: Maybe import these values from an array.
        // see: arrayFakeFile[]

        // Here we're creating the vectors for our cube.
        // During the drawing phase we're gonna select vectors to create the triangles.
        // We have two triangles per surface,

        /*
        cube->vecs[1].x = (float) -0.2f;  cube->vecs[1].y = (float) -0.2f;  cube->vecs[1].z = (float) 0.2f;
        cube->vecs[2].x = (float)  0.2f;  cube->vecs[2].y = (float) -0.2f;  cube->vecs[2].z = (float) 0.2f;
        cube->vecs[3].x = (float) -0.2f;  cube->vecs[3].y = (float)  0.2f;  cube->vecs[3].z = (float) 0.2f;
        cube->vecs[4].x = (float)  0.2f;  cube->vecs[4].y = (float)  0.2f;  cube->vecs[4].z = (float) 0.2f;

        cube->vecs[5].x = (float) -0.2f;  cube->vecs[5].y = (float)  0.2f;  cube->vecs[5].z = (float) -0.2f;
        cube->vecs[6].x = (float)  0.2f;  cube->vecs[6].y = (float)  0.2f;  cube->vecs[6].z = (float) -0.2f;
        cube->vecs[7].x = (float) -0.2f;  cube->vecs[7].y = (float) -0.2f;  cube->vecs[7].z = (float) -0.2f;
        cube->vecs[8].x = (float)  0.2f;  cube->vecs[8].y = (float) -0.2f;  cube->vecs[8].z = (float) -0.2f;
        */

        // 12 faces, 12 colors.
        cube->colors[0] = GRCOLOR_LIGHTYELLOW;
        cube->colors[1] = GRCOLOR_LIGHTMAGENTA;
        cube->colors[2] = GRCOLOR_DARKBLUE;
        cube->colors[3] = GRCOLOR_DARKGREEN;
        cube->colors[4] = GRCOLOR_DARKRED;
        cube->colors[5] = GRCOLOR_DARKCYAN;
        cube->colors[6] = GRCOLOR_DARKMAGENTA;
        cube->colors[7] = GRCOLOR_DARKYELLOW;
        cube->colors[8] = COLOR_ORANGE;  //GRCOLOR_DARKWHITE;
        cube->colors[9] = GRCOLOR_LIGHTBLACK;
        cube->colors[10] = GRCOLOR_LIGHTBLUE;
        cube->colors[11] = GRCOLOR_LIGHTGREEN;


        int it=0;

        // Head (faces 0–11)
        for (it=0; it<12; it++) cube->colors[it] = COLOR_RED;

        // Torso (faces 12–23)
        for (it=12; it<24; it++) cube->colors[it] = COLOR_GREEN;

        // Left leg (faces 24–35)
        for (it=24; it<36; it++) cube->colors[it] = COLOR_BLUE;

        // Right leg (faces 36–47)
        for (it=36; it<48; it++) cube->colors[it] = COLOR_BLUE;

        // more 2 cubes is too much for a file with 1KB limitation.

        // Left arm (faces 48–59) (Not implemented)
        for (it=48; it<60; it++) cube->colors[it] = COLOR_ORANGE;

        // Right arm (faces 60–71) (Not implemented)
        for (it=60; it<72; it++) cube->colors[it] = COLOR_PURPLE;


        // All the cubes.
        cube->model_initial_distance = 
            (float) DEFAULT_CUBE_INITIAL_Z_POSITION;
            //(float) 8.0f;
        cube->model_distance = (float) 0.0f;
        cube->model_distance_delta = 
            (float) DEFAULT_CUBE_INITIAL_DELTA_Z;
            //(float) 0.00005f;

        // left or right
        //srand(count);
        //rand1 = (rand() % 25);
        //cube->hposition = (float) 0.0f;
        cube->hposition = (float) -2.0f + (float) 0.8f * (float) count;
        //cube->hposition = (float) -1.5f + (float) 0.4f * (float) rand1;
        //cube->hposition = (float) 0.0f;

        cube->vposition = (float) 0.0f;
        
        // Initializing.
        // Cada cubo tem uma aceleração diferente.
        // Então, com o passar do tempo,
        // cada cubo tera um incremento diferente na sua velocidade.
        cube->v = (float) count * 0.00001f;
        cube->t = (float) 1.0f;
        cube->a = (float) cube->v / cube->t;
        // v = a*t;

        // Save the cube pointer.
        cubes[count] = (unsigned long) cube;
    };

// Terrain
// Special values for the terrain.

    if ( (void*) terrain != NULL )
    {

        // The model for the terrain.
        // #todo: Load this from a file.

        terrain->vecs[1].x = (float) -80.0f;  terrain->vecs[1].y = (float) -0.12f;  terrain->vecs[1].z = (float) 8.0f;
        terrain->vecs[2].x = (float)  80.0f;  terrain->vecs[2].y = (float) -0.12f;  terrain->vecs[2].z = (float) 8.0f;
        terrain->vecs[3].x = (float) -80.0f;  terrain->vecs[3].y = (float)  0.12f;  terrain->vecs[3].z = (float) 8.0f;
        terrain->vecs[4].x = (float)  80.0f;  terrain->vecs[4].y = (float)  0.12f;  terrain->vecs[4].z = (float) 8.0f;
        terrain->vecs[5].x = (float) -80.0f;  terrain->vecs[5].y = (float)  0.12f;  terrain->vecs[5].z = (float) -0.8f;
        terrain->vecs[6].x = (float)  80.0f;  terrain->vecs[6].y = (float)  0.12f;  terrain->vecs[6].z = (float) -0.8f;
        terrain->vecs[7].x = (float) -80.0f;  terrain->vecs[7].y = (float) -0.12f;  terrain->vecs[7].z = (float) -0.8f;
        terrain->vecs[8].x = (float)  80.0f;  terrain->vecs[8].y = (float) -0.12f;  terrain->vecs[8].z = (float) -0.8f;
        //terrain->model_initial_distance = (float) 8.0f;
        //terrain->model_distance = (float) 0.0f;

        // 12 faces, 12 colors.
        terrain->colors[0] = GRCOLOR_DARKWHITE;
        terrain->colors[1] = GRCOLOR_DARKWHITE;
        terrain->colors[2] = GRCOLOR_DARKWHITE;
        terrain->colors[3] = GRCOLOR_DARKWHITE;
        terrain->colors[4] = GRCOLOR_DARKWHITE;
        terrain->colors[5] = GRCOLOR_DARKWHITE;
        terrain->colors[6] = GRCOLOR_DARKWHITE;
        terrain->colors[7] = GRCOLOR_DARKWHITE;
        terrain->colors[8] = GRCOLOR_DARKWHITE;
        terrain->colors[9] = GRCOLOR_DARKWHITE;
        terrain->colors[10] = GRCOLOR_DARKWHITE;
        terrain->colors[11] = GRCOLOR_DARKWHITE;


        // z translation support.
        terrain->model_initial_distance = 
            (float) DEFAULT_TERRAIN_INITIAL_Z_POSITION;
            //(float) 4.0f;
        terrain->model_distance = (float) 0.0f;
        terrain->model_distance_delta = 
            (float) DEFAULT_TERRAIN_INITIAL_DELTA_Z;
            //(float) 0.00005f;

        terrain->hposition = (float)  0.0f;
        terrain->vposition = (float) -3.0f;

        // Initializing.
        //terrain->a = (float) 1.0f;
        terrain->v = (float) 0.0001f;
        terrain->t = (float) 0.0001f;  //0.01f;
        terrain->a = (float) terrain->v / terrain->t;
    }

//----------------
// Taskbar
    //demoClearWA(COLOR_BLACK);
    //wm_Update_TaskBar("Hello",TRUE);
    game_update_taskbar = FALSE;
}

