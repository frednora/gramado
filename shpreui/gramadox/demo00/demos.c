// demos.c
// A place for demos.
// Created by Fred Nora.

#include "gram3d.h"


/*
// Cube fake object file.
// 12 triangles for a cube.
// #bugbug
// Maybe this initialization is gonna fail.
float arrayFakeFile[] = { 
    -0.2f, -0.2f, 0.2f, 
     0.2f, -0.2f, 0.2f,
    -0.2f,  0.2f, 0.2f,
     0.2f,  0.2f, 0.2f,        
    -0.2f,  0.2f, -0.2f,
     0.2f,  0.2f, -0.2f,
    -0.2f, -0.2f, -0.2f,
     0.2f, -0.2f, -0.2f 
};
*/

struct gws_window_d *__demo_window;

int gUseDemos = TRUE;
static int game_update_taskbar=TRUE;
static int frames=0;
static int hits=0;
unsigned long beginTick=0;
unsigned long accumulatedDeltaTick=0;
unsigned long sec=0;
static char buf_fps[64];

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


struct gws_window_d *__create_demo_window (
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height );

static void drawTerrain(struct cube_model_d *cube, float fElapsedTime);
static void drawFlyingCube(struct cube_model_d *cube, float vel);


//======================



static void drawTerrain(struct cube_model_d *cube, float fElapsedTime)
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

    int sequence[3*16];  //cube
    int cull=FALSE;
    register int i=0;  //loop
    int nTriangles=12;
    int j=0;
    int off=0;
    int v=0;

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
	matRotX.m[0][0] = (float) 1.0f;
	matRotX.m[1][1] = (float) cosf(cube->fThetaAngle * 0.5f);
	matRotX.m[1][2] = (float) -sinf(cube->fThetaAngle * 0.5f);
	matRotX.m[2][1] = (float) sinf(cube->fThetaAngle * 0.5f);
	matRotX.m[2][2] = (float) cosf(cube->fThetaAngle * 0.5f);
	matRotX.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Y
// counter-clockwise
    matRotY.m[0][0] = cosf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[0][2] = sinf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[1][1] = (float) 1.0f;
    matRotY.m[2][0] = -sinf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[2][2] = cosf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Z
// counter-clockwise
	matRotZ.m[0][0] = (float) cosf(0.0f);//(cube->fThetaAngle);
	matRotZ.m[0][1] = (float) -sinf(0.0f);//(cube->fThetaAngle);
	matRotZ.m[1][0] = (float) sinf(0.0f);//(cube->fThetaAngle);
	matRotZ.m[1][1] = (float) cosf(0.0f);//(cube->fThetaAngle);
	matRotZ.m[2][2] = (float) 1.0f;
	matRotZ.m[3][3] = (float) 1.0f;

// 12 triangles.
// Order: north, top, south, bottom, east, west.
// clockwise
    sequence[0]  = (int) 1; sequence[1]  = (int) 2;  sequence[2] = (int) 4; //f 1 2 4 // north bottom  n
    sequence[3]  = (int) 1; sequence[4]  = (int) 4;  sequence[5] = (int) 3; //f 1 4 3 // north top     n
    sequence[6]  = (int) 3; sequence[7]  = (int) 4;  sequence[8] = (int) 6; //f 3 4 6 // top right     s
    sequence[9]  = (int) 3; sequence[10] = (int) 6; sequence[11] = (int) 5; //f 3 6 5 // top left      s   
    sequence[12] = (int) 5; sequence[13] = (int) 6; sequence[14] = (int) 8; //f 5 6 8 // south right   s
    sequence[15] = (int) 5; sequence[16] = (int) 8; sequence[17] = (int) 7; //f 5 8 7 // south left    s
    sequence[18] = (int) 7; sequence[19] = (int) 8; sequence[20] = (int) 2; //f 7 8 2 // bottom right  n
    sequence[21] = (int) 7; sequence[22] = (int) 2; sequence[23] = (int) 1; //f 7 2 1 // bottom left   n
    sequence[24] = (int) 2; sequence[25] = (int) 8; sequence[26] = (int) 6; //f 2 8 6 // east bottom   s
    sequence[27] = (int) 2; sequence[28] = (int) 6; sequence[29] = (int) 4; //f 2 6 4 // east top      n  
    sequence[30] = (int) 7; sequence[31] = (int) 1; sequence[32] = (int) 3; //f 7 1 3 // west bottom   n
    sequence[33] = (int) 7; sequence[34] = (int) 3; sequence[35] = (int) 5; //f 7 3 5 // west top      s 

// ---------
// #test
// draw a rectangle
   //drawRectangle0((float) 0.08f);

// ---------
// draw a cube

    //cull=FALSE;

    for (i=1; i <= nTriangles; i++)
    {
        cull=FALSE;

        off = (int) ((i-1)*3);
        
        v = (int) sequence[off+0];
        tri.p[0].x = (float) cube->vecs[v].x;
        tri.p[0].y = (float) cube->vecs[v].y;
        tri.p[0].z = (float) cube->vecs[v].z;
        tri.p[0].color = COLOR_PINK;
        if(i >= 1 && i <= 12){
            tri.p[0].color = cube->colors[i-1];  // rectangle color
        }
        v = (int) sequence[off+1];
        tri.p[1].x = (float) cube->vecs[v].x;
        tri.p[1].y = (float) cube->vecs[v].y;
        tri.p[1].z = (float) cube->vecs[v].z;
        tri.p[1].color = COLOR_WHITE;  // not used

        v = (int) sequence[off+2];
        tri.p[2].x = (float) cube->vecs[v].x;
        tri.p[2].y = (float) cube->vecs[v].y;
        tri.p[2].z = (float) cube->vecs[v].z;
        tri.p[2].color = COLOR_WHITE;  // not used


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
        if ( (void*) __root_window != NULL )
        {
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
static void drawFlyingCube(struct cube_model_d *cube, float vel)
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

    int sequence[3*16];  //cube
    int cull=FALSE;
    register int i=0;  //loop
    int nTriangles=12;
    int j=0;
    int off=0;
    int v=0;
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
    matRotX.m[0][0] = (float) 1.0f;
    matRotX.m[1][1] = (float) cosf(cube->fThetaAngle * 0.5f);
    matRotX.m[1][2] = (float) -sinf(cube->fThetaAngle * 0.5f);
    matRotX.m[2][1] = (float) sinf(cube->fThetaAngle * 0.5f);
    matRotX.m[2][2] = (float) cosf(cube->fThetaAngle * 0.5f);
    matRotX.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Y
// counter-clockwise
    matRotY.m[0][0] = cosf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[0][2] = sinf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[1][1] = (float) 1.0f;
    matRotY.m[2][0] = -sinf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[2][2] = cosf(0.0f);//(cube->fThetaAngle * 0.5f);
    matRotY.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Z
// counter-clockwise
    matRotZ.m[0][0] = (float) cosf(0.0f);//(cube->fThetaAngle);
    matRotZ.m[0][1] = (float) -sinf(0.0f);//(cube->fThetaAngle);
    matRotZ.m[1][0] = (float) sinf(0.0f);//(cube->fThetaAngle);
    matRotZ.m[1][1] = (float) cosf(0.0f);//(cube->fThetaAngle);
    matRotZ.m[2][2] = (float) 1.0f;
    matRotZ.m[3][3] = (float) 1.0f;

// ?
// The 'face' has three vector.
// Now we're selecting the indexes for these three vectors. I guess.
// 12 triangles. (12*3) = 36 vectors.
// Order: north, top, south, bottom, east, west.
// clockwise
    sequence[0]  = (int) 1; sequence[1]  = (int) 2;  sequence[2] = (int) 4; //f 1 2 4 // north bottom  n
    sequence[3]  = (int) 1; sequence[4]  = (int) 4;  sequence[5] = (int) 3; //f 1 4 3 // north top     n
    sequence[6]  = (int) 3; sequence[7]  = (int) 4;  sequence[8] = (int) 6; //f 3 4 6 // top right     s
    sequence[9]  = (int) 3; sequence[10] = (int) 6; sequence[11] = (int) 5; //f 3 6 5 // top left      s   
    sequence[12] = (int) 5; sequence[13] = (int) 6; sequence[14] = (int) 8; //f 5 6 8 // south right   s
    sequence[15] = (int) 5; sequence[16] = (int) 8; sequence[17] = (int) 7; //f 5 8 7 // south left    s
    sequence[18] = (int) 7; sequence[19] = (int) 8; sequence[20] = (int) 2; //f 7 8 2 // bottom right  n
    sequence[21] = (int) 7; sequence[22] = (int) 2; sequence[23] = (int) 1; //f 7 2 1 // bottom left   n
    sequence[24] = (int) 2; sequence[25] = (int) 8; sequence[26] = (int) 6; //f 2 8 6 // east bottom   s
    sequence[27] = (int) 2; sequence[28] = (int) 6; sequence[29] = (int) 4; //f 2 6 4 // east top      n  
    sequence[30] = (int) 7; sequence[31] = (int) 1; sequence[32] = (int) 3; //f 7 1 3 // west bottom   n
    sequence[33] = (int) 7; sequence[34] = (int) 3; sequence[35] = (int) 5; //f 7 3 5 // west top      s 

// ---------
// #test
// draw a rectangle
   //drawRectangle0((float) 0.08f);

// - Loop --------------------------------
// Draw a list of triangles.
// A cube has 6 faces and 12 triangles.
// 1~12

    for (i=1; i <= nTriangles; i++)
    {
        cull=FALSE;

        // Jumping three offsets each time.
        off = (int) ((i-1)*3);
        
        // Build the vector 0.
        v = (int) sequence[off+0];  // Get the vertice index.
        tri.p[0].x = (float) cube->vecs[v].x;
        tri.p[0].y = (float) cube->vecs[v].y;
        tri.p[0].z = (float) cube->vecs[v].z;
        tri.p[0].color = COLOR_PINK;
        if (i >= 1 && i <= 12){
            tri.p[0].color = cube->colors[i-1];  // rectangle color
        }

        // Build the vector 1.
        v = (int) sequence[off+1];  // Get the vertice index.
        tri.p[1].x = (float) cube->vecs[v].x;
        tri.p[1].y = (float) cube->vecs[v].y;
        tri.p[1].z = (float) cube->vecs[v].z;
        tri.p[1].color = COLOR_WHITE;  // not used

        // Build the vector 2.
        v = (int) sequence[off+2];  // Get the vertice index.
        tri.p[2].x = (float) cube->vecs[v].x;
        tri.p[2].y = (float) cube->vecs[v].y;
        tri.p[2].z = (float) cube->vecs[v].z;
        tri.p[2].color = COLOR_WHITE;  // not used

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
            if ( (void*) __root_window != NULL )
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

struct gws_window_d *__create_demo_window (
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height )
{
    struct gws_window_d *w;

    if ( (void*) __root_window == NULL ){
        return NULL;
    }

// Create window

    w = 
        (struct gws_window_d *) CreateWindow ( 
                                    WT_SIMPLE, 
                                    0, //style
                                    1, //status 
                                    1, //view
                                    "DemoWin",  
                                    left, top, width, height,   
                                    __root_window, 0, 
                                    COLOR_BLACK, 
                                    COLOR_BLACK );

    if ( (void *) w == NULL ){
        return NULL;
    }
    if ( w->used != TRUE ||  w->magic != 1234 ){
        return NULL;
    }

// Register the window.
    int WindowId= -1;
    WindowId = (int) RegisterWindow(w);
    if (WindowId<0){
         return NULL;
    }
// ok
    return (struct gws_window_d *)  w;
}

void demoLines(void)
{
    int i=0;
    for (i=0; i<320; i+=5)
    {
        //3d
        plotLine3d ( 
            NULL,
            0,i,0,      //x,y,z 
            320-i,0,0,  //x,y,z 
            COLOR_WHITE); 

        //3d
        plotLine3d ( 
            NULL,
            0,i,0,      //x,y,z 
            -(320-i),0,0,  //x,y,z 
            COLOR_WHITE); 
      
        // ====

        //3d
        plotLine3d ( 
            NULL,
            -i,0,0,      //x,y,z 
            0,-(320-i),0,  //x,y,z 
            COLOR_WHITE); 

        //3d
        plotLine3d ( 
           NULL,
            i,0,0,      //x,y,z 
           0, -(320-i),0,  //x,y,z 
            COLOR_WHITE); 


    };
    refresh_screen();
}

void demoCat(void)
{
    register int i=0;
    int j=0;
    int count = 8;
    int scale_max = 100;

// ---------------
// Create a demo window
    struct gws_window_d *dw;
    dw = NULL;
    dw = (struct gws_window_d *) __create_demo_window(8,8,200,140);
    if( (void*) dw != NULL )
    {
       if(dw->magic==1234){
           __demo_window = dw;
       }
    }
//---------------------

// depth clipping
// IN: projection, znear, zfar.
    gr_depth_range( CurrentProjection, 0, 100 );

// The camera for the cat.
// 'int' values.
// IN: Position vector, upview vector, lookat vector.
    camera ( 
        0,0,0,
        0,0,0,
        0,0,0 );

// Setup model
// IN: eyes, whiskers, mouth
    __setupCatModel(TRUE,TRUE,TRUE);


// Loop
    while (count>0)
    {
        for (i=0; i<scale_max; i++)
        {
            validate_background();                 //begin paint
            demoClearSurface(dw,GRCOLOR_LIGHTCYAN);   // Clear surface
            // IN: eye scale, x,y,z
            __draw_cat(1,0,0,i);
            demoFlushSurface(dw);
            //invalidate_background();               // end paint
            //gr_dc_refresh_screen(gr_dc);

            // good for qemu,
            //for (j=0; j<8; j++){ gwssrv_yield();}  // Delay
            // good for kvm,
            //for (j=0; j<80; j++){ gwssrv_yield();}  // Delay
            
            //rtl_yield();
        };

        count--;
    };
}


void demoTriangle(void)
{

// ---------------
// Create a demo window
    struct gws_window_d *dw;
    dw = NULL;
    dw = (struct gws_window_d *) __create_demo_window(8,8,200,140);
    if( (void*) dw != NULL )
    {
       if(dw->magic==1234){
           __demo_window = dw;
       }
    }

//------------------------------------------
    struct gr_triangle_d *triangle;
    int line_size = 40;

// Create the triangle.
    triangle = (void *) malloc( sizeof( struct gr_triangle_d ) );
    if ( (void*) triangle == NULL )
        return;
    triangle->used = TRUE;
    triangle->magic = 1234;
    triangle->initialized = FALSE;
// down
    triangle->p[0].x = 0; 
    triangle->p[0].y = 0;
    triangle->p[0].z = 0;
    triangle->p[0].color = COLOR_RED;
// right
    triangle->p[1].x = (line_size>>1); 
    triangle->p[1].y = (line_size>>1);
    triangle->p[1].z =  0;
    triangle->p[1].color = COLOR_GREEN;
// left
    triangle->p[2].x = -(line_size>>1);
    triangle->p[2].y =  (line_size>>1);
    triangle->p[2].z =   0;
    triangle->p[2].color = COLOR_BLUE;

    triangle->initialized = TRUE;

    int i=0;
    int j=0;
    int max = 150;
    //int T=0;

    for(i=0; i<max; i++)
    {
        // clear
        demoClearSurface(dw,COLOR_BLACK);
        // Draw a lot of triangles.
        //for(j=0; j<max; j++)
        //{
            // translation
            triangle->p[0].x++;
            triangle->p[1].x++;
            triangle->p[2].x++;
            grTriangle3(dw,triangle);
        //};

        // flush surface
        demoFlushSurface(dw);
        rtl_yield();
        //T++;
    };
}

// demo: polygon type polyline
// asteroids space ship like.
void demoPolygon(void)
{
// Down arrow shape.

    struct gr_polygon_d *p;
    unsigned long vecList[8];
    
    struct gr_vec3D_d *v0;
    struct gr_vec3D_d *v1;
    struct gr_vec3D_d *v2;
    struct gr_vec3D_d *v3;
    struct gr_vec3D_d *v4;

    // ...

    int i=0;
    int TranslationOffset=0;
    int j=0;

// Structure
    p = (struct gr_polygon_d *) malloc( sizeof( struct gr_polygon_d ) );
    if ((void*)p==NULL){
        return;
    }

// polygon type
    p->type = POLYGON_POLYLINE;
// number of elements
    p->n = 6;

// clear vecList.
// This is a local list.

    p->list_address = (void*) vecList;

    for(i=0; i<8; i++){
        vecList[i] = 0;
    };

// Creating some points.
// 5 vectors

    v0 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v0==NULL){return;}

    v1 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v1==NULL){return;}

    v2 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v2==NULL){return;}

    v3 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v3==NULL){return;}

    v4 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v4==NULL){return;}

    vecList[0] = (unsigned long) v0;
    vecList[1] = (unsigned long) v1;
    vecList[2] = (unsigned long) v2;
    vecList[3] = (unsigned long) v3;
    vecList[4] = (unsigned long) v4;
    vecList[5] = (unsigned long) v0;  //circular

// loop
// animation loop.

    int times=0;

    while(1){

    times++;
    if(times>8){break;}
    
    // translation in Y
    TranslationOffset=0;
    
    // animation loop: 
    // clear the screen and draw the the model 10 times.
    
    for (i=0; i<10; i++){

    demoClearSurface(NULL,COLOR_BLACK);
           
    TranslationOffset = (TranslationOffset+i);
    
    v0->x = -(20);
    v0->y =  (20+TranslationOffset);
    v0->z =   0;
    v0->color = COLOR_WHITE;

    // fixed
    v1->x = (0);
    v1->y = (0+TranslationOffset);
    v1->z = 0;
    v1->color = COLOR_WHITE;

    v2->x = (20);
    v2->y = (20+TranslationOffset);
    v2->z =  0;
    v2->color = COLOR_WHITE;

    v3->x =   (0);
    v3->y = -(20-TranslationOffset);  
    v3->z =   0;
    v3->color = COLOR_WHITE;

    v4->x = -(20);
    v4->y =  (20+TranslationOffset);  
    v4->z =  0;
    v4->color = COLOR_WHITE;


// Draw
    xxxPolygonZ(p);
// Show
    demoFlushSurface(NULL);  
    
    for (j=0; j<8; j++){ gwssrv_yield(); }

    };
    };   //while
}


void demoPolygon2(void)
{
// Diamond shape.

    struct gr_polygon_d *p;
    unsigned long vecList[8];
    
    struct gr_vec3D_d *v0;
    struct gr_vec3D_d *v1;
    struct gr_vec3D_d *v2;
    struct gr_vec3D_d *v3;
    struct gr_vec3D_d *v4;

    // ...

    int i=0;
    int TranslationOffset=0;
    int j=0;

// structure

    p = (struct gr_polygon_d *) malloc( sizeof( struct gr_polygon_d ) );
    if((void*)p==NULL){
        return;
    }

// polygon type
    p->type = POLYGON_POLYPOINT;
    //p->type = POLYGON_POLYLINE;
// number of elements
    p->n = 6;

// clear vecList.
// This is a local list.

    p->list_address = (void*) vecList;

    for(i=0; i<8; i++){
        vecList[i] = 0;
    };

// Creating some points.

    v0 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v0==NULL){return;}

    v1 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v1==NULL){return;}

    v2 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v2==NULL){return;}

    v3 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v3==NULL){return;}

    v4 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v4==NULL){return;}

    vecList[0] = (unsigned long) v0;
    vecList[1] = (unsigned long) v1;
    vecList[2] = (unsigned long) v2;
    vecList[3] = (unsigned long) v3;
    vecList[4] = (unsigned long) v4;
    vecList[5] = (unsigned long) v0;  //circular

// loop
// animation loop

    int times=0;

    while(1){

    times++;
    if(times>8){break;}
    
    // translation in Y
    TranslationOffset=0;
    
    // animation loop: 
    // clear the screen and draw the the model 10 times.
    
    for (i=0; i<10; i++){

    // clear surface
    demoClearSurface(NULL,COLOR_BLACK);
    
    TranslationOffset = (TranslationOffset+i);
    
    v0->x = -(20);
    v0->y =  (10+TranslationOffset);
    v0->z =   0;
    v0->color = COLOR_WHITE;

    // fixed
    v1->x = (20);
    v1->y = (10+TranslationOffset);
    v1->z =  0;
    v1->color = COLOR_WHITE;

    v2->x =  30;
    v2->y = (0+TranslationOffset);
    v2->z =  0;
    v2->color = COLOR_WHITE;

    v3->x = 0;
    v3->y = -(10-TranslationOffset);  
    v3->z =  0;
    v3->color = COLOR_WHITE;

    v4->x = -30;
    v4->y = (0+TranslationOffset);  
    v4->z = 0;
    v4->color = COLOR_WHITE;

// Draw

    //p->type = POLYGON_POLYPOINT;
    //xxxPolygonZ(p);
    //gws_refresh_rectangle(0,0,320,200);

    p->type = POLYGON_POLYLINE;
    xxxPolygonZ(p);

    // flush surface.
    demoFlushSurface(NULL);  
    
    //delay
    for (j=0; j<20; j++){ gwssrv_yield();}

    };
    };   //while
}

// Curva e string.
void demoCurve(void)
{
    register int i=0;
    register int j=0;
    int count=8;

    // IN: ?, near, far
    gr_depth_range( CurrentProjection, 0, 100 );
// Loop
    while (count>0){
    count--;
    for (i=0; i<10; i++)
    {
        validate_background();
        demoClearSurface(NULL,GRCOLOR_LIGHTYELLOW);
        // IN: position, modelz
        __draw_demo_curve1(i,0);
        //__draw_demo_curve1(i,i*4);
        //invalidate_background();
        demoFlushSurface(NULL);      // flush surface
        // delay  
        //for (j=0; j<8; j++){ gwssrv_yield();}
    };
    }
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

// Called by the engine
// obs: 
// We have an embedded model into this function
// it's because we still don't have a function to read floating point data from
// a file. In the future we will have the model into a file and read it using our new function.
//
void demoFlyingCubeSetup(void)
{
// first cube
    struct cube_model_d *cube;
// Cube1
    register int i=0;
    
    accumulatedDeltaTick=0;
    sec=0;
// Clear the buffer for the string in the yellow bar.
    memset(buf_fps,0,64);

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
        if (count==0){
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

        // The model for a regular cube.
        // #todo: >> Load this from a file.
        // #todo: Maybe import these values from an array.
        // see: arrayFakeFile[]

        cube->vecs[1].x = (float) -0.2f;  cube->vecs[1].y = (float) -0.2f;  cube->vecs[1].z = (float) 0.2f;
        cube->vecs[2].x = (float)  0.2f;  cube->vecs[2].y = (float) -0.2f;  cube->vecs[2].z = (float) 0.2f;
        cube->vecs[3].x = (float) -0.2f;  cube->vecs[3].y = (float)  0.2f;  cube->vecs[3].z = (float) 0.2f;
        cube->vecs[4].x = (float)  0.2f;  cube->vecs[4].y = (float)  0.2f;  cube->vecs[4].z = (float) 0.2f;

        cube->vecs[5].x = (float) -0.2f;  cube->vecs[5].y = (float)  0.2f;  cube->vecs[5].z = (float) -0.2f;
        cube->vecs[6].x = (float)  0.2f;  cube->vecs[6].y = (float)  0.2f;  cube->vecs[6].z = (float) -0.2f;
        cube->vecs[7].x = (float) -0.2f;  cube->vecs[7].y = (float) -0.2f;  cube->vecs[7].z = (float) -0.2f;
        cube->vecs[8].x = (float)  0.2f;  cube->vecs[8].y = (float) -0.2f;  cube->vecs[8].z = (float) -0.2f;

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
        // cada cubo tera um incremento diferente
        // na sua velocidade.
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

void demoFlyingCube(int draw_desktop, unsigned int bg_color)
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
    //unsigned long beginTick = rtl_jiffies();

// -------------------------
// Clear canvas.
    //demoClearWA(COLOR_BLACK);            //clear surface
    gramado_clear_surface(NULL,bg_color);  //clear surface

// -------------------------
// Draw desktop?
// IN: ?
    if (draw_desktop){
        wm_update_desktop(TRUE,TRUE);
    }
// -------------------------
// Draw terrain.
// No rotation. Small translation in positive z.
// 12 triangles.
    drawTerrain(terrain,0.0f);

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
            
            drawFlyingCube( 
                (struct cube_model_d *) tmp_cube,
                (float) tmp_cube->v );
        }

        n++;
    };

    //yellowstatus0("test",FALSE);  //draw but don't refresh
    //if(game_update_taskbar){
    //    wm_Update_TaskBar("53$",FALSE); //redraw, but not refresh
    //    game_update_taskbar = FALSE;
    //}

    unsigned long endTick = rtl_jiffies();
    accumulatedDeltaTick += endTick - beginTick;
// New frame.
    frames++;

// Ja se passou 1 segundo?
    if (accumulatedDeltaTick > 1000)
    {
        sec++; // New second.
        memset(buf_fps,0,64);
        itoa(frames,buf_fps);
            strcat(buf_fps," FPS");
        accumulatedDeltaTick=0;
        frames=0;
    }

// Draw yellow bar.
    yellowstatus0(buf_fps,FALSE);

// Flush the backbuffer into the framebuffer.
    gramado_flush_surface(NULL);
}

