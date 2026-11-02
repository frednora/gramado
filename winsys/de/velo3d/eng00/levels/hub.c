// hub.c (Hub World)
// Explanation:
// A Hub World is a central, safe (or relatively safe) area that 
// connects multiple levels. Players return to it after finishing a level, 
// and from there they can choose which level to enter next.
// Created by Fred Nora.

#include "../gram3d.h"


int current_level = LEVEL_HUB;
int exit_level = FALSE;  // Is it time to exit the current level?

static int game_update_taskbar=TRUE;
static int hits=0;

//
// Obj files
//

static char *model_data_humanoid = NULL;
static char *model_data_saucer = NULL;
static char *model_data_ground = NULL;
// ...

// Humanoids
#define MODEL_MAX  8
unsigned long models[MODEL_MAX];

// Disks
#define DISK_MODEL_MAX  8
unsigned long m_disks[DISK_MODEL_MAX];

#define DRAWERLIST_MAX  (MODEL_MAX + DISK_MODEL_MAX +1)
unsigned long drawerList[DRAWERLIST_MAX];

// World yaw -- how much the world has turned around the hero's fixed
// position. Read by the draw functions later; written only here.
static float worldYawAngle = 0.0f;
// rotation of the main character only
static float heroYawAngle = 0.0f; 
// rotation for the camera
static float cameraYawAngle = 0.0f; 


#define WORLD_TURN_SPEED  (0.05f)  // radians per input step -- tune to taste
#define CAMERA_TURN_SPEED  (0.05f)


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

static int __load_all_obj_files(void);

static void __drawModelWithShading (struct model_d *model, float fElapsedTime);
static void __drawRotatingModel(struct model_d *model, float vel);
static void render_model(struct model_d *m, float elapsed);
static void demo_draw_hud(void);


static void assignBandColors(struct model_d *m, int blockSize,
                              const unsigned int *palette, int palette_len);

static void assignRangeColor(struct model_d *m,
                              int startFace, int endFace, unsigned int color);
static void assignSaucerColors(struct model_d *s_model);

static unsigned int computeColor(struct n3d_vec_d *vertex,
                                 struct n3d_vec_d *normal,
                                 unsigned int baseColor);


static void __rotateAroundPivotY(float *x, float *z, float px, float pz, float angle);

static void __setupTerrain(void);
static void assignTerrainColors(struct terrain_model_d *t);
static void __drawTerrain(struct terrain_model_d *t);

static void __reorderByZ(unsigned long *list, int count);
static int buildModelList(unsigned long *outList, int maxCount);

//======================

// models[] — OBJ06.TXT: 7 blocks of 12 faces (head, torso, L/R leg, L/R arm, chest)
static const unsigned int humanoidPalette[7] = {
    0xFFD1B3, // head        - skin tone
    0x6699CC, // torso       - shirt
    0x333333, // left leg    - pants
    0x333333, // right leg   - pants
    0xCC4444, // left arm    - sleeve
    0xCC4444, // right arm   - sleeve
    0x999999, // chest/pack  - gear
};

// m_disks[] — OBJ07.TXT: 4 bands of 16 faces (lower hull, rim, dome base, dome cap)
static const unsigned int saucerPalette[4] = {
    0x707070, // lower hull - dark metal skirt
    0xC0C0C0, // upper rim  - bright metal (widest disk edge)
    0x9999CC, // dome base  - canopy/glass tint
    0xFFCC00, // dome cap   - beacon light
};

// Colors one "band" of faces at a time — a band is a contiguous run of
// blockSize triangles sharing one color (a cube's 12 faces, or a
// ring-to-ring strip's 16 faces).
static void assignBandColors(struct model_d *m, int blockSize,
                              const unsigned int *palette, int palette_len)
{
    int nBlocks  = m->face_count / blockSize;
    int leftover = m->face_count % blockSize;
    int b, f;

    for (b = 0; b < nBlocks; b++) {
        unsigned int c = palette[b % palette_len];
        for (f = 0; f < blockSize; f++)
            m->colors[b*blockSize + f] = c;
    }
    if (leftover > 0) {
        unsigned int c = palette[nBlocks % palette_len];
        for (f = 0; f < leftover; f++)
            m->colors[nBlocks*blockSize + f] = c;
    }
}


// Colors a contiguous run of faces [startFace, endFace] (1-based, inclusive)
static void assignRangeColor(struct model_d *m,
                              int startFace, int endFace, unsigned int color)
{
    int f;
    for (f = startFace; f <= endFace && f <= m->face_count; f++)
        m->colors[f - 1] = color;   // colors[] is 0-based, faces[] is 1-based
}

// m_disks[] — OBJ07.TXT (64 faces): 3 hull bands of 16 + dome fan of 8 + bottom fan of 8
static void assignSaucerColors(struct model_d *s_model)
{
    assignRangeColor(s_model,  1, 16, 0x707070); // lower hull  - dark metal skirt
    assignRangeColor(s_model, 17, 32, 0xC0C0C0); // upper rim   - bright metal (widest point)
    assignRangeColor(s_model, 33, 48, 0x9999CC); // dome base   - canopy/glass tint
    assignRangeColor(s_model, 49, 56, 0xFFCC00); // dome cap    - beacon light
    assignRangeColor(s_model, 57, 64, 0x404040); // underside   - dark belly
}

// shading
static unsigned int computeColor(struct n3d_vec_d *vertex,
                                 struct n3d_vec_d *normal,
                                 unsigned int baseColor)
{
    // Light direction
    struct n3d_vec_d lightDir = {0.5f, 1.0f, -0.5f};
    float len = sqrtf(lightDir.x*lightDir.x + lightDir.y*lightDir.y + lightDir.z*lightDir.z);
    lightDir.x /= len; lightDir.y /= len; lightDir.z /= len;

    // Diffuse factor — use fabsf() instead of clamping negative to 0.
    // This makes lighting robust to winding-order mistakes: a face whose
    // normal happens to point the "wrong" way still gets a sensible
    // brightness instead of going near-black.
    float dp = normal->x*lightDir.x + normal->y*lightDir.y + normal->z*lightDir.z;
    dp = fabsf(dp);

    // Higher ambient floor (0.6 instead of 0.2) so palette colors keep
    // their identity — this is a gentle "hint of depth" look rather than
    // full directional shading.
    float brightness = 0.6f + 0.4f * dp;

    unsigned char r = (baseColor >> 16) & 0xFF;
    unsigned char g = (baseColor >> 8) & 0xFF;
    unsigned char b = baseColor & 0xFF;

    r = (unsigned char)(r * brightness);
    g = (unsigned char)(g * brightness);
    b = (unsigned char)(b * brightness);

    return (r << 16) | (g << 8) | b;
}



// Rotates a single (x,z) point around a fixed pivot by `angle` radians.
// Pure float math -- doesn't know or care which struct type owns the point.
static void __rotateAroundPivotY(float *x, float *z, float px, float pz, float angle)
{
    float dx = *x - px;
    float dz = *z - pz;
    float c = cosf(angle);
    float s = sinf(angle);

    *x = px + (dx * c - dz * s);
    *z = pz + (dx * s + dz * c);
}



// this is the generic draw routine with diffuse shading.
static void __drawModelWithShading (struct model_d *model, float fElapsedTime)
{
// No rotation. Small translation in positive z.

    char string0[16];

// Matrices
    struct n3d_mat4x4_d  matRotX;
    struct n3d_mat4x4_d  matRotY;
    struct n3d_mat4x4_d  matRotZ;

// Triangles
    struct n3d_triangle_d  tri;            // triângulo original.
    struct n3d_triangle_d  triRotatedX; 
    struct n3d_triangle_d  triRotatedXY;
    struct n3d_triangle_d  triRotatedXYZ;

    int cull=FALSE;

    int nTriangles=12;

    int v_index=0;       // vertex index

// Loop iterators
    register int i=0;
    int j=0;

// ---------
    if( (void*) model == NULL ){
        return;
    }

// ---------
// Initialize 4x4 matrices
// see: gprim.h
    for (i=0; i<4; i++){
        for (j=0; j<4; j++){
            matRotZ.m[i][j] = (float) 0.0f;
            matRotY.m[i][j] = (float) 0.0f;
            matRotX.m[i][j] = (float) 0.0f;
        };
    };

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
	matRotX.m[1][1] = (float)  cosf(model->fThetaAngle * 0.5f);
	matRotX.m[1][2] = (float) -sinf(model->fThetaAngle * 0.5f);
	matRotX.m[2][1] = (float)  sinf(model->fThetaAngle * 0.5f);
	matRotX.m[2][2] = (float)  cosf(model->fThetaAngle * 0.5f);
	matRotX.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Y
// counter-clockwise
// R_y(θ) =
//|  cos(θ)   0   sin(θ)  |
//|   0       1     0     |
//| -sin(θ)   0   cos(θ)  |
    matRotY.m[0][0] = cosf(0.0f);
    matRotY.m[0][2] = sinf(0.0f);
    matRotY.m[1][1] = (float) 1.0f;
    matRotY.m[2][0] = -sinf(0.0f);
    matRotY.m[2][2] =  cosf(0.0f);
    matRotY.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Z
// counter-clockwise
//R_z(θ) =
//|  cos(θ)  -sin(θ)   0  |
//|  sin(θ)   cos(θ)   0  |
//|    0        0      1  |
	matRotZ.m[0][0] = (float)  cosf(0.0f);
	matRotZ.m[0][1] = (float) -sinf(0.0f);
	matRotZ.m[1][0] = (float)  sinf(0.0f);
	matRotZ.m[1][1] = (float)  cosf(0.0f);
	matRotZ.m[2][2] = (float) 1.0f;
	matRotZ.m[3][3] = (float) 1.0f;

// 12 triangles.
// Order: north, top, south, bottom, east, west.
// clockwise? or ccw.

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

        // #backup
        tri.p[0].color = model->colors[i-1];
        tri.p[1].color = model->colors[i-1];
        tri.p[2].color = model->colors[i-1];


        //-----------------------------    
        // Rotate in X-Axis
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &tri.p[0], 
            (struct n3d_vec_d *) &triRotatedX.p[0], 
            (struct n3d_mat4x4_d *) &matRotX);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &tri.p[1], 
            (struct n3d_vec_d *) &triRotatedX.p[1], 
            (struct n3d_mat4x4_d *) &matRotX);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &tri.p[2], 
            (struct n3d_vec_d *) &triRotatedX.p[2], 
            (struct n3d_mat4x4_d *) &matRotX);

        //-----------------------------    
        // Rotate in Y-Axis
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedX.p[0], 
            (struct n3d_vec_d *) &triRotatedXY.p[0], 
            (struct n3d_mat4x4_d *) &matRotY);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedX.p[1], 
            (struct n3d_vec_d *) &triRotatedXY.p[1], 
            (struct n3d_mat4x4_d *) &matRotY);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedX.p[2], 
            (struct n3d_vec_d *) &triRotatedXY.p[2], 
            (struct n3d_mat4x4_d *) &matRotY);

        //-----------------------------    
        // Rotate in Z-Axis
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedXY.p[0], 
            (struct n3d_vec_d *) &triRotatedXYZ.p[0], 
            (struct n3d_mat4x4_d *) &matRotZ);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedXY.p[1], 
            (struct n3d_vec_d *) &triRotatedXYZ.p[1], 
            (struct n3d_mat4x4_d *) &matRotZ);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedXY.p[2], 
            (struct n3d_vec_d *) &triRotatedXYZ.p[2], 
            (struct n3d_mat4x4_d *) &matRotZ);


        triRotatedXYZ.p[0].color = tri.p[0].color;
        triRotatedXYZ.p[1].color = tri.p[1].color;
        triRotatedXYZ.p[2].color = tri.p[2].color;


        // --------------------------------

        // #test
        // World rotation -- pivot is the hero's own model_d origin.
        //{
        if (model != main_character)
        {
            float px = main_character->origin_x;
            float pz = main_character->origin_z;

            float newangle = -worldYawAngle;

            __rotateAroundPivotY(&triRotatedXYZ.p[0].x, &triRotatedXYZ.p[0].z, px, pz, newangle);
            __rotateAroundPivotY(&triRotatedXYZ.p[1].x, &triRotatedXYZ.p[1].z, px, pz, newangle);
            __rotateAroundPivotY(&triRotatedXYZ.p[2].x, &triRotatedXYZ.p[2].z, px, pz, newangle);
        }
        //}

        if (model == main_character)
        {
            // Hero rotation only: spin in place
            float c = cosf(heroYawAngle);
            float s = sinf(heroYawAngle);

            float x0 = triRotatedXYZ.p[0].x;
            float z0 = triRotatedXYZ.p[0].z;
            triRotatedXYZ.p[0].x = x0 * c - z0 * s;
            triRotatedXYZ.p[0].z = x0 * s + z0 * c;

            float x1 = triRotatedXYZ.p[1].x;
            float z1 = triRotatedXYZ.p[1].z;
            triRotatedXYZ.p[1].x = x1 * c - z1 * s;
            triRotatedXYZ.p[1].z = x1 * s + z1 * c;

            float x2 = triRotatedXYZ.p[2].x;
            float z2 = triRotatedXYZ.p[2].z;
            triRotatedXYZ.p[2].x = x2 * c - z2 * s;
            triRotatedXYZ.p[2].z = x2 * s + z2 * c;
        }

        // Now apply model translation
        // Now apply this model's own position (translation)
        triRotatedXYZ.p[0].z += model->origin_z; 
        triRotatedXYZ.p[1].z += model->origin_z; 
        triRotatedXYZ.p[2].z += model->origin_z; 

        triRotatedXYZ.p[0].y += model->origin_y; 
        triRotatedXYZ.p[1].y += model->origin_y; 
        triRotatedXYZ.p[2].y += model->origin_y; 

        triRotatedXYZ.p[0].x += model->origin_x; 
        triRotatedXYZ.p[1].x += model->origin_x; 
        triRotatedXYZ.p[2].x += model->origin_x; 


        // --------------------------------


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

        /*

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

        */

        //----------------------------------------------------
        // Use Cross-Product to get surface normal
        struct n3d_vec_d normal; 
        struct n3d_vec_d line1; 
        struct n3d_vec_d line2;

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

        // #test
        // Shading
        unsigned int base = model->colors[i-1]; // pick base color for this face
        triRotatedXYZ.p[0].color = computeColor(&triRotatedXYZ.p[0], &normal, base);
        triRotatedXYZ.p[1].color = computeColor(&triRotatedXYZ.p[1], &normal, base);
        triRotatedXYZ.p[2].color = computeColor(&triRotatedXYZ.p[2], &normal, base);


        //#ok
        //if ( (float) normal.z <  0.0f){ cull=FALSE;}  //pinta
        //if ( (float) normal.z >= 0.0f){ cull=TRUE; }  //não pinta

        // #test
        // Considering the camera position.
        if (CurrentCameraF.initialized == FALSE){ return; }

        /*
        struct n3d_vec_d viewDir;
        viewDir.x = CurrentCameraF.lookat.x - CurrentCameraF.position.x;
        viewDir.y = CurrentCameraF.lookat.y - CurrentCameraF.position.y;
        viewDir.z = CurrentCameraF.lookat.z - CurrentCameraF.position.z;
        float len = 
            sqrtf(viewDir.x*viewDir.x + viewDir.y*viewDir.y + viewDir.z*viewDir.z);
        if (len > 0.0f) 
        {
            viewDir.x /= len;
            viewDir.y /= len;
            viewDir.z /= len;
        }
        float tmp = 
            (float) (
            normal.x * viewDir.x +
            normal.y * viewDir.y +
            normal.z * viewDir.z );
        */

        
        float tmp = 
             (float) (
             normal.x * (triRotatedXYZ.p[0].x - CurrentCameraF.position.x) + 
             normal.y * (triRotatedXYZ.p[0].y - CurrentCameraF.position.y) +
             normal.z * (triRotatedXYZ.p[0].z - CurrentCameraF.position.z) );

        /*
        float tmp = 
             (float) (
             normal.x * (triRotatedXYZ.p[0].x - CurrentCameraF.lookat.x) + 
             normal.y * (triRotatedXYZ.p[0].y - CurrentCameraF.lookat.y) +
             normal.z * (triRotatedXYZ.p[0].z - CurrentCameraF.lookat.z) );
        */

        if ((float) tmp <  0.0f){ cull=FALSE; }  //paint
        if ((float) tmp >= 0.0f){ cull=TRUE;  }  //do not paint
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

        // cull = FALSE;
        if (cull == FALSE)
        {
            if ((void*) __root_window != NULL)
            {
                plotTriangleF(
                    (struct gws_window_d *) __root_window, 
                    (struct n3d_triangle_d *) &triRotatedXYZ,
                    fFillTriangle,
                    model->rop  // Rop value for the whole model 
                );
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

// this is the rotating object routine.
static void __drawRotatingModel (struct model_d *model, float vel)
{
    char string0[16];

// Matrices
    struct n3d_mat4x4_d  matRotX;
    struct n3d_mat4x4_d  matRotY;
    struct n3d_mat4x4_d  matRotZ;

// Triangles
    struct n3d_triangle_d  tri;            // Original triangle
    struct n3d_triangle_d  triRotatedX;    // Rotate in X
    struct n3d_triangle_d  triRotatedXY;   // Rotate in Y
    struct n3d_triangle_d  triRotatedXYZ;  // Rotate in Z (Projected)

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
            matRotY.m[i][j] = (float) 0.0f;
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
    matRotX.m[1][1] = (float)  cosf(model->fThetaAngle * 0.5f);
    matRotX.m[1][2] = (float) -sinf(model->fThetaAngle * 0.5f);
    matRotX.m[2][1] = (float)  sinf(model->fThetaAngle * 0.5f);
    matRotX.m[2][2] = (float)  cosf(model->fThetaAngle * 0.5f);
    matRotX.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Y
// counter-clockwise
// R_y(θ) =
//|  cos(θ)   0   sin(θ)  |
//|   0       1     0     |
//| -sin(θ)   0   cos(θ)  |
    matRotY.m[0][0] = cosf(0.0f);
    matRotY.m[0][2] = sinf(0.0f);
    matRotY.m[1][1] = (float) 1.0f;
    matRotY.m[2][0] = -sinf(0.0f);
    matRotY.m[2][2] = cosf(0.0f);
    matRotY.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Z
// counter-clockwise
//R_z(θ) =
//|  cos(θ)  -sin(θ)   0  |
//|  sin(θ)   cos(θ)   0  |
//|    0        0      1  |
    matRotZ.m[0][0] = (float) cosf(0.0f);
    matRotZ.m[0][1] = (float) -sinf(0.0f);
    matRotZ.m[1][0] = (float) sinf(0.0f);
    matRotZ.m[1][1] = (float) cosf(0.0f);
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
            (struct n3d_vec_d *) &tri.p[0], 
            (struct n3d_vec_d *) &triRotatedX.p[0], 
            (struct n3d_mat4x4_d *) &matRotX);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &tri.p[1], 
            (struct n3d_vec_d *) &triRotatedX.p[1], 
            (struct n3d_mat4x4_d *) &matRotX);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &tri.p[2], 
            (struct n3d_vec_d *) &triRotatedX.p[2], 
            (struct n3d_mat4x4_d *) &matRotX);

        //-----------------------------
        // Rotate in Y-Axis
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedX.p[0], 
            (struct n3d_vec_d *) &triRotatedXY.p[0], 
            (struct n3d_mat4x4_d *) &matRotY);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedX.p[1], 
            (struct n3d_vec_d *) &triRotatedXY.p[1], 
            (struct n3d_mat4x4_d *) &matRotY);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedX.p[2], 
            (struct n3d_vec_d *) &triRotatedXY.p[2], 
            (struct n3d_mat4x4_d *) &matRotY);

        //-----------------------------
        // Rotate in Z-Axis
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedXY.p[0], 
            (struct n3d_vec_d *) &triRotatedXYZ.p[0], 
            (struct n3d_mat4x4_d *) &matRotZ);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedXY.p[1], 
            (struct n3d_vec_d *) &triRotatedXYZ.p[1], 
            (struct n3d_mat4x4_d *) &matRotZ);
        gr_MultiplyAndProjectVector(
            (struct n3d_vec_d *) &triRotatedXY.p[2], 
            (struct n3d_vec_d *) &triRotatedXYZ.p[2], 
            (struct n3d_mat4x4_d *) &matRotZ);

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
        struct n3d_vec_d normal;
        struct n3d_vec_d line1;
        struct n3d_vec_d line2;

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
        // cull = FALSE;
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
            // Maybe this function can accept more parameters
            if ((void*) __root_window != NULL)
            {
                plotTriangleF(
                    (struct gws_window_d *) __root_window, 
                    (struct n3d_triangle_d *) &triRotatedXYZ,
                    fFillTriangle,
                    0 );


                /*
                // #test: Not working yet
                struct n3d_vec_d testv;
                testv.x = 1.0f;
                testv.y = 1.0f;
                testv.z = 5.0f;
                testv.color = COLOR_RED;
                plotPixelF(
                    (struct gws_window_d *) __root_window,
                    (struct n3d_vec_d*) &testv,
                    0 );
                */

            }
        }

    };  // loop: Number of triangles.
}


static void __drawTerrain(struct terrain_model_d *t)
{
    struct n3d_mat4x4_d matRotX;
    struct n3d_mat4x4_d matRotY;
    struct n3d_mat4x4_d matRotZ;

    struct n3d_triangle_d tri;
    struct n3d_triangle_d triRotatedX;
    struct n3d_triangle_d triRotatedXY;
    struct n3d_triangle_d triRotatedXYZ;

    int cull;
    int i, j;

    // Terrain has no fThetaAngle field -- it never rotates on its own,
    // so this is a literal 0.0f rather than a struct member. The matrix
    // math below is identical in shape to the other draw routines even
    // though it always resolves to identity for now.
    float angle = 0.0f;

    if ((void*) t == NULL) return;

// ---------
// Zero all three matrices, same as the other draw routines.
    for (i=0; i<4; i++){
        for (j=0; j<4; j++){
            matRotX.m[i][j] = (float) 0.0f;
            matRotY.m[i][j] = (float) 0.0f;
            matRotZ.m[i][j] = (float) 0.0f;
        };
    };

//------------------------------------------------
// Rotation X (identity while angle stays 0.0f)
    matRotX.m[0][0] = (float) 1.0f;
    matRotX.m[1][1] = (float) cosf(angle * 0.5f);
    matRotX.m[1][2] = (float) -sinf(angle * 0.5f);
    matRotX.m[2][1] = (float) sinf(angle * 0.5f);
    matRotX.m[2][2] = (float) cosf(angle * 0.5f);
    matRotX.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Y (identity)
    matRotY.m[0][0] = cosf(0.0f);
    matRotY.m[0][2] = sinf(0.0f);
    matRotY.m[1][1] = (float) 1.0f;
    matRotY.m[2][0] = -sinf(0.0f);
    matRotY.m[2][2] = cosf(0.0f);
    matRotY.m[3][3] = (float) 1.0f;
//------------------------------------------------
// Rotation Z (identity)
    matRotZ.m[0][0] = (float) cosf(0.0f);
    matRotZ.m[0][1] = (float) -sinf(0.0f);
    matRotZ.m[1][0] = (float) sinf(0.0f);
    matRotZ.m[1][1] = (float) cosf(0.0f);
    matRotZ.m[2][2] = (float) 1.0f;
    matRotZ.m[3][3] = (float) 1.0f;

    for (i=1; i <= t->face_count; i++)
    {
        cull = FALSE;

        int i0 = t->faces[i].vi[0];
        int i1 = t->faces[i].vi[1];
        int i2 = t->faces[i].vi[2];

        tri.p[0] = t->vecs[i0];
        tri.p[1] = t->vecs[i1];
        tri.p[2] = t->vecs[i2];

        tri.p[0].color = t->colors[i-1];
        tri.p[1].color = t->colors[i-1];
        tri.p[2].color = t->colors[i-1];

        //-----------------------------
        // Rotate in X-Axis
        gr_MultiplyAndProjectVector(&tri.p[0], &triRotatedX.p[0], &matRotX);
        gr_MultiplyAndProjectVector(&tri.p[1], &triRotatedX.p[1], &matRotX);
        gr_MultiplyAndProjectVector(&tri.p[2], &triRotatedX.p[2], &matRotX);

        //-----------------------------
        // Rotate in Y-Axis
        gr_MultiplyAndProjectVector(&triRotatedX.p[0], &triRotatedXY.p[0], &matRotY);
        gr_MultiplyAndProjectVector(&triRotatedX.p[1], &triRotatedXY.p[1], &matRotY);
        gr_MultiplyAndProjectVector(&triRotatedX.p[2], &triRotatedXY.p[2], &matRotY);

        //-----------------------------
        // Rotate in Z-Axis
        gr_MultiplyAndProjectVector(&triRotatedXY.p[0], &triRotatedXYZ.p[0], &matRotZ);
        gr_MultiplyAndProjectVector(&triRotatedXY.p[1], &triRotatedXYZ.p[1], &matRotZ);
        gr_MultiplyAndProjectVector(&triRotatedXY.p[2], &triRotatedXYZ.p[2], &matRotZ);

        triRotatedXYZ.p[0].color = tri.p[0].color;
        triRotatedXYZ.p[1].color = tri.p[1].color;
        triRotatedXYZ.p[2].color = tri.p[2].color;

        // Translate into world space.
        triRotatedXYZ.p[0].x += t->origin_x;
        triRotatedXYZ.p[1].x += t->origin_x;
        triRotatedXYZ.p[2].x += t->origin_x;

        triRotatedXYZ.p[0].y += t->origin_y;
        triRotatedXYZ.p[1].y += t->origin_y;
        triRotatedXYZ.p[2].y += t->origin_y;

        triRotatedXYZ.p[0].z += t->origin_z;
        triRotatedXYZ.p[1].z += t->origin_z;
        triRotatedXYZ.p[2].z += t->origin_z;


        // #test
        // World rotation -- terrain_model_d has no fThetaAngle or spin
        // state of its own. The pivot is the hero's model_d origin (the
        // one shared reference point every draw function rotates
        // around), applied here against t's own already-translated
        // triRotatedXYZ triangle.
        //{
            float px = main_character->origin_x;
            float pz = main_character->origin_z;
            __rotateAroundPivotY(&triRotatedXYZ.p[0].x, &triRotatedXYZ.p[0].z, px, pz, worldYawAngle);
            __rotateAroundPivotY(&triRotatedXYZ.p[1].x, &triRotatedXYZ.p[1].z, px, pz, worldYawAngle);
            __rotateAroundPivotY(&triRotatedXYZ.p[2].x, &triRotatedXYZ.p[2].z, px, pz, worldYawAngle);
        //}


        // Surface normal, same cross-product technique as the other draw paths.
        struct n3d_vec_d normal, line1, line2;

        line1.x = triRotatedXYZ.p[1].x - triRotatedXYZ.p[0].x;
        line1.y = triRotatedXYZ.p[1].y - triRotatedXYZ.p[0].y;
        line1.z = triRotatedXYZ.p[1].z - triRotatedXYZ.p[0].z;

        line2.x = triRotatedXYZ.p[2].x - triRotatedXYZ.p[0].x;
        line2.y = triRotatedXYZ.p[2].y - triRotatedXYZ.p[0].y;
        line2.z = triRotatedXYZ.p[2].z - triRotatedXYZ.p[0].z;

        normal.x = (float) (line1.y*line2.z - line1.z*line2.y);
        normal.y = (float) (line1.z*line2.x - line1.x*line2.z);
        normal.z = (float) (line1.x*line2.y - line1.y*line2.x);

        float l = (float) sqrt( (double)
                    (normal.x*normal.x + normal.y*normal.y + normal.z*normal.z) );

        normal.x = normal.x / l;
        normal.y = normal.y / l;
        normal.z = normal.z / l;

        // Shading -- same computeColor pass used everywhere else.
        unsigned int base = t->colors[i-1];
        triRotatedXYZ.p[0].color = computeColor(&triRotatedXYZ.p[0], &normal, base);
        triRotatedXYZ.p[1].color = computeColor(&triRotatedXYZ.p[1], &normal, base);
        triRotatedXYZ.p[2].color = computeColor(&triRotatedXYZ.p[2], &normal, base);

        // Backface cull, same camera-relative test as the other draw paths.
        if (CurrentCameraF.initialized == FALSE){ return; }
        float tmp =
            normal.x * (triRotatedXYZ.p[0].x - CurrentCameraF.position.x) +
            normal.y * (triRotatedXYZ.p[0].y - CurrentCameraF.position.y) +
            normal.z * (triRotatedXYZ.p[0].z - CurrentCameraF.position.z);
        if (tmp >= 0.0f) { cull = TRUE;  }
        if (tmp <  0.0f) { cull = FALSE; }

        if (cull == FALSE)
        {
            if ((void*) __root_window != NULL)
            {
                plotTriangleF(
                    (struct gws_window_d *) __root_window,
                    (struct n3d_triangle_d *) &triRotatedXYZ,
                    TRUE,
                    t->rop  // Desired ROP for the model 
                );
            }
        }
    }
}


// Control + arrow key
// It moves a given model.
// In: model id, direction, value.
void demoHumanoidMoveCharacter(int number, int direction, float value)
{
    struct model_d *model;

// Model ID
    if (number < 0)
        return;
    if (number >= MODEL_MAX)
        return;
// Model structure
    model = (struct model_d *) models[number];
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

static void updateCameraFollowCharacter(void);
/*
static void updateCameraFollowCharacter(void)
{
    float radius = 2.0f;   // distance behind the character
    float height = 1.0f;   // lift camera above ground

    // Pivot around character
    float px = main_character->origin_x;
    float py = main_character->origin_y;
    float pz = main_character->origin_z;

    CurrentCameraF.position.x = px + radius * cosf(worldYawAngle);
    CurrentCameraF.position.z = pz + radius * sinf(worldYawAngle);
    CurrentCameraF.position.y = py + height;

    CurrentCameraF.lookat.x = px;
    CurrentCameraF.lookat.y = py;
    CurrentCameraF.lookat.z = pz;
}
*/
static void updateCameraFollowCharacter(void)
{
    //float radius = 2.0f;   // distance behind the character
    //float height = 1.0f;   // lift camera above ground

    // Pivot around character
    //float px = main_character->origin_x;
    //float py = main_character->origin_y;
    //float pz = main_character->origin_z;

    //CurrentCameraF.position.x = px + radius * cosf(worldYawAngle);
    //CurrentCameraF.position.z = pz + radius * sinf(worldYawAngle);
    //CurrentCameraF.position.y = py + height;

    float px = main_character->origin_x;
    float py = main_character->origin_y;
    float pz = main_character->origin_z;

    __rotateAroundPivotY(
        &CurrentCameraF.position.x, 
        &CurrentCameraF.position.z, px, pz, worldYawAngle);

    CurrentCameraF.lookat.x = px;
    CurrentCameraF.lookat.y = py;
    CurrentCameraF.lookat.z = pz;
}


// Called when a key combination is pressed
void demoCameraOrbit(int direction, float value)
{
/*
// #suspended
    printf("Camera\n");

    // Orbit left
    if (direction == 1) {
        cameraYawAngle -= (float)(value * CAMERA_TURN_SPEED);
    }
    // Orbit right
    if (direction == 2) {
        cameraYawAngle += (float)(value * CAMERA_TURN_SPEED);
    }

    // Update camera position based on new yaw
    //float radius = 10.0f;   // distance from hero
    //float height = 3.0f;    // camera height
    float radius = 0.5f;   // distance from hero
    float height = 0.5f;    // camera height

    CurrentCameraF.position.x = main_character->origin_x + radius * cosf(cameraYawAngle);
    CurrentCameraF.position.z = main_character->origin_z + radius * sinf(cameraYawAngle);
    CurrentCameraF.position.y = main_character->origin_y + height;

    // Always look at hero
    CurrentCameraF.lookat.x = main_character->origin_x;
    CurrentCameraF.lookat.y = main_character->origin_y;
    CurrentCameraF.lookat.z = main_character->origin_z;
*/
}

// Orbit the whole world when pressing Ctrl+Arrow
void demoCameraSpinWorld(int direction)
{
    // Turn speed constant
    //const float CAMERA_TURN_SPEED = 0.05f;

    if (direction == 1) // Ctrl+Left
    {
        worldYawAngle -= CAMERA_TURN_SPEED;
        heroYawAngle  += CAMERA_TURN_SPEED;
    }
    else if (direction == 2) // Ctrl+Right
    {
        worldYawAngle += CAMERA_TURN_SPEED;
        heroYawAngle  -= CAMERA_TURN_SPEED;
    }

    // Camera stays fixed — no position update needed.
    // It will still render correctly because the world + hero angles changed.
}


// New worker, separate from demoHumanoidMoveCharacter.
// Instead of moving a model's origin, this turns the whole world
// around the hero's fixed position. The hero itself never moves
// sideways anymore -- everything else swings around them.
//
// direction: 1 = turn left, 2 = turn right.
// value: same "how much" input demoHumanoidMoveCharacter already takes.

// ok, lemme rethink it. 
// The main char needs to stay at the center of the screen (kinda), 
// when a keyboard input happens, the char needs to rotate, 
// but not change its position in the screen, 
// the world and the other models can rotate too if necessary and 
// the camera can move too if necessary.

/*
Goals:
The main character stays fixed at the center of the screen (its origin doesn’t move in screen space).
When you press a key, the character rotates in place (changes orientation, not position).
The world and other models rotate around the hero if you want the illusion of turning.
The camera can orbit as well, so the player sees the rotation from behind or above.
*/

void demoHumanoidRotateWorld(int direction, float value)
{
    // turn left
    if (direction == 1){
        worldYawAngle -= (float) (value * WORLD_TURN_SPEED);
        heroYawAngle  += (float) (value * WORLD_TURN_SPEED);
    }
    // turn right
    if (direction == 2){
        worldYawAngle += (float) (value * WORLD_TURN_SPEED);
        heroYawAngle  -= (float) (value * WORLD_TURN_SPEED);
    }

    //updateCameraFollowCharacter();
}

// Bubble sort for small lists
static void __reorderByZ(unsigned long *list, int count) 
{
    int swapped;
    int i=0;
    do {
        swapped = 0;
        for (i=0; i < count - 1; i++) 
        {
            struct model_d *ma = (struct model_d *) list[i];
            struct model_d *mb = (struct model_d *) list[i+1];
            if ((void*)ma != NULL && (void*)mb != NULL)
            {
                // Swap only if it is smaller
                if (ma->origin_z < mb->origin_z) 
                {
                    unsigned long tmp = list[i];
                    list[i] = list[i+1];
                    list[i+1] = tmp;
                    swapped = 1;
                }
            }
        };
    } while (swapped);
}

// Worker: build + reorder list
static int buildModelList(unsigned long *outList, int maxCount) 
{
    int count = 0;
    int i=0;

    // Collect humanoids
    for (i=0; i < MODEL_MAX; i++) 
    {
        if (count >= maxCount)
            goto fail;
        if (models[i] != 0) {
            outList[count] = models[i];
            count++;
        }
    }

    // Collect saucers/discs
    for (i=0; i < DISK_MODEL_MAX; i++) 
    {
        if (count >= maxCount)
            goto fail;
        if (m_disks[i] != 0) {
            outList[count] = m_disks[i];
            count++;
        }
    }

    // Reorder by origin_z
    __reorderByZ(outList, count);

    return (int) count;  // number of models collected
fail:
    return (int) -1;
}

// =============================================
// HUD / On-screen Text
// =============================================
static void demo_draw_hud(void)
{
    char buffer[64];
    int i;

    if ((void*)__root_window == NULL)
        return;

    // ─────────────────────────────────────
    // Left Side - Main Info
    // ─────────────────────────────────────
    dtextDrawText(__root_window, 12, 12, 0xFFFF00, (unsigned char*)"HUB WORLD");     // Bright Yellow

    sprintf(buffer, "Level: %d", current_level);
    dtextDrawText(__root_window, 12, 34, 0xFFFFFF, (unsigned char*)buffer);         // White

    sprintf(buffer, "Score: %d", hits);
    dtextDrawText(__root_window, 12, 56, 0x00FFAA, (unsigned char*)buffer);         // Cyan-Green

    // ─────────────────────────────────────
    // Center Top - Player Status
    // ─────────────────────────────────────
    if (main_character)
    {
        sprintf(buffer, "Health: %d", main_character->health_value);
        unsigned int health_color = (main_character->health_value > 30) ? 
                                    0xFF4444 : 0xFF0000;   // Red → Bright Red when low
        dtextDrawText(__root_window, 280, 12, health_color, (unsigned char*)buffer);

        sprintf(buffer, "Lives: %d", main_character->lives);
        dtextDrawText(__root_window, 280, 34, 0xFFFFFF, (unsigned char*)buffer);   // White
    }

    // ─────────────────────────────────────
    // Right Side - Stats
    // ─────────────────────────────────────
    int alive_enemies = 0;
    for (i = 1; i < MODEL_MAX; i++)
    {
        struct model_d *m = (struct model_d*)models[i];
        if (m && m->IsAlive) alive_enemies++;
    }

    sprintf(buffer, "Enemies: %d", alive_enemies);
    dtextDrawText(__root_window, 480, 12, 0x00FFFF, (unsigned char*)buffer);   // Cyan

    sprintf(buffer, "Disks: %d", DISK_MODEL_MAX);
    dtextDrawText(__root_window, 480, 34, 0xFFAA00, (unsigned char*)buffer);   // Orange

    // ─────────────────────────────────────
    // Debug Info
    // ─────────────────────────────────────
    /*
    if (debug_mode)
    {
        sprintf(buffer, "Models: %d | Yaw: %.2f", 
                buildModelList(drawerList, DRAWERLIST_MAX), worldYawAngle);
        dtextDrawText(__root_window, 12, 90, 0xAAAAAA, (unsigned char*)buffer);   // Light Gray

        if (main_character)
        {
            sprintf(buffer, "Hero id=%d @ (%.1f, %.1f, %.1f)", 
                    main_character->id,
                    main_character->origin_x,
                    main_character->origin_y,
                    main_character->origin_z);
            dtextDrawText(__root_window, 12, 110, 0xFF8800, (unsigned char*)buffer); // Orange
        }
    }
    */
}

// #test
static void render_model(struct model_d *m, float elapsed)
{
    if (!m) return;

    // Type-specific pre-processing
    switch (m->tag)
    {
        case MODEL_TYPE_DISK:
            //m->fThetaAngle += elapsed * 3.0f;   // constant spin
            break;

        case MODEL_TYPE_HUMANOID:
            if (m == main_character)
                heroYawAngle = m->fThetaAngle;  // sync
            break;
    }

    // Final draw
    __drawModelWithShading(m, elapsed);
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

// #ps:
// The demo is called 'Humanoid'. 
// Here we're gonna draw the whole scene.
// Containing humanoids, sauces and ground.
void demoHumanoidDrawScene(unsigned long sec)
{
// The function xxx() in main.c initilizes this demos
// and spins into a loop calling this function to draw all the scene.
// #todo: It means that of this demo was not initialized,
// we need to abort this function.

    //struct model_d *enemy;

    // #todo
    // This demo was initialized before calling this drawing routine?


// Begin time.
// Moved to the main loop of the server.
    //unsigned long gBeginTick = rtl_jiffies();

// === 1. Draw Terrain First ===
    if (ground != NULL) {
        __drawTerrain(ground);
    }

// === 2. Build and sort dynamic models ===

// Save the pointers for the models here
// Clear the list before using it
    int i;
    for (i = 0; i < DRAWERLIST_MAX; i++) {
        drawerList[i] = 0;
    }
    // IN: address for the list
    int n;
    n = (int) buildModelList(drawerList, DRAWERLIST_MAX);
    if (n <= 0 || n >= 64)
    {
        printf("on buildModelList()\n");
        exit(1);
    }

// === 3. Draw models with intelligence ===

// Static scenery 
    //int i=0;
    struct model_d *model;
    for (i = 0; i < DRAWERLIST_MAX; i++) 
    {
        // Pick one
        model = (struct model_d*) drawerList[i]; 

        if (model != NULL)
        {
            if (model != main_character)
            {
                model->t = (float) model->t + (float) sec * 0.1f;
                model->v = (float) model->t * model->a;  
            }

            // New worker
            render_model(model, model->v);

            // Low level worker
            // __drawModelWithShading(model, model->v);
        } 
    };

/*
// Draw all the enemies
// 1~n humanoids.
    register int n=1; // main_character =0
    while (1){

        if (n >= MODEL_MAX){
            break;
        }

        // Get a pointer for the next cube.
        enemy = (struct model_d *) models[n];
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

            //__drawRotatingModel ( 
            //    (struct model_d *) enemy,
            //    (float) enemy->v );

            __drawModelWithShading ( 
                (struct model_d *) enemy,
                (float) enemy->v );

        }

        n++;
    };
*/

/*
// Static buildings 
    for (int i = 0; i < building_count; i++) { 
        __drawModelWithShading (building_models[i], fElapsedTime); 
    }
*/

    demo_draw_hud();
}

// + Update position for the models
// + Reset if they go out of bounds (out of the world)
/*
void demoHumanoidUpdate(void)
{
    int i=0;
    struct model_d *model;

// Update only static models
// Not the hero
// #ps: It starts at 1, not 0.

    for (i=1; i < MODEL_MAX; i++) 
    {
        model = (struct model_d *) models[i];
        if (!model)
            continue;

        // Apply deltas
        model->origin_x += model->delta_x;
        model->origin_y += model->delta_y;
        model->origin_z += model->delta_z;

        // Reset if they go out of bounds
        if (model->origin_z > current_world_3d->z_size) 
        {
            model->origin_z = DEFAULT_MODEL_INITIAL_Z_POSITION;
        }
    };
}
*/
void demoHumanoidUpdate(void)
{
// #ps: The game/demo is called humanoid.
// This routine can affect every part of the game.

//
// Limits
//

    float limit_x = (float) current_world_3d->x_size / (float) 8.0f;
    //float limit_y = (float) current_world_3d->y_size / (float) 2.0f;
    float limit_z = (float) current_world_3d->z_size / (float) 8.0f;



    int i=1;
    int HitLimit = FALSE;


// disks
// #ps: We gotta start at 1.
    int disk_count = DISK_MODEL_MAX;
    for (i = 1; i < disk_count; i++)
    {
        struct model_d *m = m_disks[i];

        m->origin_y -= 0.01f;
        if (m->origin_y <= ground->origin_y)
            m->origin_y = ground->origin_y;
    }

// humanoids
// #ps: We gotta start at 1.
    int humanoid_count = MODEL_MAX;
    for (i = 1; i < humanoid_count; i++) 
    {
        struct model_d *m = models[i];  // Humanoids
        int reset = FALSE;

        if ((i%2) == 0)
        {
            m->origin_x += 0.2f;
            if (m->collided == TRUE)
            {
                m->collided = FALSE;
                reset = TRUE;
            }
        }
        if ((i%2) != 0)
        {
            m->origin_x -= 0.2f;
            if (m->collided == TRUE)
            {
                m->collided = FALSE;
                reset = TRUE;
            }
        }

        if (reset == TRUE)
        {
            float zoffset = (float) (rand() % 8);

            // origin
            m->origin_x = 
                (float) -3.0f + (float) 0.8f * (float) i; // spread across X axis
            m->origin_y = (float) -3.0f;  // slightly lower (ground level)
            m->origin_z = (float) DEFAULT_MODEL_INITIAL_Z_POSITION + (float) zoffset;

            // Translations ...
            m->delta_x = (float) 0.0f;  //DEFAULT_MODEL_INITIAL_DELTA_X; 
            m->delta_y = (float) 0.0f;  // DEFAULT_MODEL_INITIAL_DELTA_Y; 
            m->delta_z = (float) DEFAULT_MODEL_INITIAL_DELTA_Z;
        }

        m->origin_y = ground->origin_y;  // Stay grounded
        m->origin_z += m->delta_z;       // March forward

        /*
        // --- Collision with the main charactere ---
        if ( m->origin_x > (main_character->origin_x - 0.4f) &&  
             m->origin_x < (main_character->origin_x + 0.4f) &&
             m->origin_z > (main_character->origin_z - 0.4f) &&
             m->origin_z < (main_character->origin_z + 0.4f) )
        {
            m->collided = TRUE;
        }
        */
        // --- Collision with the main charactere ---
        //float radius = 0.8f; // collision radius
        float radius = (float) main_character->interaction_radius;
        if ( (float) radius < (float) 0.5f )
        { 
            radius = (float) 0.5f; 
        };
        float dx = m->origin_x - main_character->origin_x;
        float dz = m->origin_z - main_character->origin_z;
        float distance = sqrtf(dx*dx + dz*dz);
        if (distance < radius) {
            m->collided = TRUE;
        }


        //
        // -- Collision with the limits
        //

        if ( m->origin_x < (-limit_x) )
        { 
            m->origin_x = (-limit_x);
            m->collided = TRUE;
        }
        if ( m->origin_x > limit_x    )
        { 
            m->origin_x = limit_x;
            m->collided = TRUE;
        }
        if ( m->origin_z < (-limit_x) )
        { 
            m->origin_z = (-limit_z);
            m->collided = TRUE;
        }
        if ( m->origin_z > limit_z    )
        { 
            m->origin_x = limit_z;
            m->collided = TRUE;
        }
    };
}


static int __load_all_obj_files(void)
{
    // Humanoid
    model_data_humanoid = (char *) demosReadFileIntoBuffer("obj06.txt");
    if ((void*) model_data_humanoid == NULL){
        printf("on loading obj06.txt\n");
        exit(1);
    }

    // Flying saucer
    model_data_saucer = (char *) demosReadFileIntoBuffer("obj07.txt");
    if ((void*) model_data_saucer == NULL){
        printf("on loading obj07.txt\n");
        exit(1);
    }

    // Ground
    model_data_ground= (char *) demosReadFileIntoBuffer("obj08.txt");
    if ((void*) model_data_ground == NULL){
        printf("on loading obj08.txt\n");
        exit(1);
    }

    return 0;  //ok
}

// ground[] — OBJ08.TXT: colored per-face from local vertex height,
// not fixed ranges, since the grid mesh has no clean contiguous "bands"
// the way the saucer's rings do.
static const unsigned int terrainPalette[3] = {
    0x8B5A2B, // low ground (dips)   - dirt/soil brown
    0x6B8E23, // mid ground          - olive farmland green
    0x9ACD32, // high ground (rises) - bright grass green
};

static void assignTerrainColors(struct terrain_model_d *t)
{
    int f;
    for (f = 1; f <= t->face_count; f++)
    {
        int i0 = t->faces[f].vi[0];
        int i1 = t->faces[f].vi[1];
        int i2 = t->faces[f].vi[2];

        float avgY = (t->vecs[i0].y + t->vecs[i1].y + t->vecs[i2].y) / 3.0f;

        unsigned int c;
        if      (avgY < -0.03f) c = terrainPalette[0];
        else if (avgY <  0.03f) c = terrainPalette[1];
        else                    c = terrainPalette[2];

        t->colors[f - 1] = c;
    }
}


static void __setupTerrain(void)
{
    struct terrain_model_d *t;
    int i;

    t = (struct terrain_model_d *) malloc( sizeof(struct terrain_model_d) );
    if ((void*) t == NULL){
        printf("__setupTerrain: t\n");
        exit(1);
    }

    // Initialize vectors
    for (i=0; i<200; i++)
    {
        t->vecs[i].x = (float) 0.0f;
        t->vecs[i].y = (float) 0.0f;
        t->vecs[i].z = (float) 0.0f;
    };

    // t->rop = 42;
    t->rop = 0;

    struct obj_element_d elem;
    const char *nextLine = model_data_ground;

    int VertexCounter = 1;
    int FaceCounter = 1;
    do {
        const char *temp =
            scan00_read_element_from_line(
                nextLine,
                (struct obj_element_d *) &elem );

        if (elem.initialized == TRUE)
        {
            if (elem.type == OBJ_ELEMENT_TYPE_VECTOR)
            {
                t->vecs[VertexCounter].x = (float) elem.vertex.x;
                t->vecs[VertexCounter].y = (float) elem.vertex.y;
                t->vecs[VertexCounter].z = (float) elem.vertex.z;
                VertexCounter++;
            }
            else if (elem.type == OBJ_ELEMENT_TYPE_FACE)
            {
                t->faces[FaceCounter].vi[0] = elem.face.vi[0];
                t->faces[FaceCounter].vi[1] = elem.face.vi[1];
                t->faces[FaceCounter].vi[2] = elem.face.vi[2];
                FaceCounter++;
            }
        }

        nextLine = temp;

    } while (nextLine != NULL);

    t->vertex_count = VertexCounter - 1;
    t->face_count   = FaceCounter - 1;

    // Colors — patchy farmland look, computed from real mesh height.
    assignTerrainColors(t);

    // Position: sits at "ground level", under the hero's feet.
    // Hero is at origin_y = -3.0f, so put the plane a little below that.
    t->origin_x = (float) 0.0f;
    t->origin_y = (float) -3.2f;
    t->origin_z = (float) DEFAULT_MODEL_INITIAL_Z_POSITION;

// #ps: Defined in models.c
    ground = t;

/*
// ----------------------------------------
// #ps: 
// >>> keeping the world dimensions larger than the terrain <<<
// Compute terrain bounds.
// It will become the dimensions for the world.

    float minX = 999999.0f, maxX = -999999.0f;
    float minY = 999999.0f, maxY = -999999.0f;
    float minZ = 999999.0f, maxZ = -999999.0f;


    for (i = 1; i <= t->vertex_count; i++) {
    if (t->vecs[i].x < minX) minX = t->vecs[i].x;
    if (t->vecs[i].x > maxX) maxX = t->vecs[i].x;
    if (t->vecs[i].y < minY) minY = t->vecs[i].y;
    if (t->vecs[i].y > maxY) maxY = t->vecs[i].y;
    if (t->vecs[i].z < minZ) minZ = t->vecs[i].z;
    if (t->vecs[i].z > maxZ) maxZ = t->vecs[i].z;
    }

    // Dimensions
    float width  = maxX - minX;
    float height = maxY - minY;
    float depth  = maxZ - minZ;

// Sync world size to terrain
    if (current_world_3d != NULL) {
    current_world_3d->x_size = width;
    current_world_3d->y_size = height;
    current_world_3d->z_size = depth;
    }
*/
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


// Setup function for humanoid or disk model
// -----------------------------------------
// Each model has:
// - Up to 128 vertices (low-poly budget)
// - 72 faces (triangles) that need base colors
// - Colors must be initialized ONCE here, not inside draw()
// - Shading later uses computeColor(vertex, normal, baseColor)
// -----------------------------------------

void demoHumanoidSetup(void)
{
// This is called once

    struct model_d *model;
    struct model_d *s_model;  // static

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

// Clear the list
    for (i=0; i<MODEL_MAX; i++){
        models[i] = (unsigned long) 0;
    };
    for (i=0; i<DISK_MODEL_MAX; i++){
        m_disks[i] = (unsigned long) 0;
    };


// Loading obj files
    __load_all_obj_files();

// Setup terrain
    __setupTerrain();

    int count=0;
    int rand1=0;

// =============================================================
// Humanoids (Player + Enemies)
// =============================================================

    for (count=0; count<MODEL_MAX; count++)
    {
        model = (void*) malloc( sizeof(struct model_d) );
        if ((void*) model == NULL){
            printf("demoHumanoidSetup: model\n");
            exit(1);
        }

        // === Set header ===
        model->id = count;  // Humanoid number
        model->tag = MODEL_TYPE_HUMANOID;

        // The main char
        if (count == 0){
            main_character = (struct model_d *) model;
        }

        model->fThetaAngle = (float) 0.0f;
        model->rop = 0;
        model->interaction_radius = (float) 0.8f;
                
        // Initialize vectors
        for (i=0; i<128; i++){
            model->vecs[i].x = (float) 0.0f;
            model->vecs[i].y = (float) 0.0f;
            model->vecs[i].z = (float) 0.0f;
        };

    
        // -- Test -----------------------------------------------------
        struct obj_element_d elem;
        //struct n3d_vec_d vertex;     

        // Humanoid
        const char *nextLine = model_data_humanoid;

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

        // Assign colors for the humanoid model
        // enemy loop
        assignBandColors(model, 12, humanoidPalette, 7);

        // --------------------------------

        float zoffset = (float) (rand() % 8);

        // origin
        model->origin_x = 
            (float) -3.0f + (float) 0.8f * (float) count; // spread across X axis
        model->origin_y = (float) -3.0f;  // slightly lower (ground level)
        model->origin_z = (float) DEFAULT_MODEL_INITIAL_Z_POSITION + (float) zoffset; 

        // Translations ...
        model->delta_x = (float) 0.0f;  //DEFAULT_MODEL_INITIAL_DELTA_X; 
        model->delta_y = (float) 0.0f;  // DEFAULT_MODEL_INITIAL_DELTA_Y; 
        model->delta_z = (float) DEFAULT_MODEL_INITIAL_DELTA_Z;
        
        // Initializing
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

// =============================================================
// Disks / Saucers
// =============================================================

    for (count=0; count<DISK_MODEL_MAX; count++)
    {
        s_model = (void*) malloc( sizeof(struct model_d) );
        if ((void*) s_model == NULL){
            printf("demoHumanoidSetup: s_model\n");
            exit(1);
        }

        // === Set header ===
        s_model->id = count;  // Disk number
        s_model->tag = MODEL_TYPE_DISK;

        // Tilt the saucer so we see it 3/4-on instead of edge-on.
        // fThetaAngle is used as (angle * 0.5f) in the rotation matrix,
        // so ~0.8f here gives a visible ~23° pitch.
        s_model->fThetaAngle = (float) 0.0f;  //0.8f;

        s_model->rop = 0;

        // Initialize vectors
        for (i=0; i<128; i++)
        {
            s_model->vecs[i].x = (float) 0.0f;
            s_model->vecs[i].y = (float) 0.0f;
            s_model->vecs[i].z = (float) 0.0f;
        };

    
        // -- Test -----------------------------------------------------
        struct obj_element_d  elem;
        //struct n3d_vec_d vertex;

        // Flying sauce
        const char *nextLine = model_data_saucer;

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

        // Assign colors for the flying sauces. (disk with something on top)
        // static loop
        assignSaucerColors(s_model);

        // ----------------------
        // Position in the world

        // Spread wider in X so the (radius ~2.0) disks don't overlap.
        // Centered around x=0 instead of starting at -3.0f.
        s_model->origin_x =
            (float) ((count - (DISK_MODEL_MAX - 1) / 2.0f) * 5.0f);

        // Lift into "sky" airspace, above the humanoids (y=0) and hero (y=-3.0).
        // Slight stagger per model so they're not perfectly level with each other.
        s_model->origin_y = (float) (3.0f + 0.4f * count);

        // Tighter z spread so distant ones don't shrink to invisible dots.
        float factor = (float) count;
        s_model->origin_z =
            (float) DEFAULT_MODEL_INITIAL_Z_POSITION + (3.0f * factor);

        // Translations ...
        s_model->delta_x = (float) 0.0f;
        s_model->delta_y = (float) 0.0f;
        s_model->delta_z = (float) 0.0f; 

        // Initializing
        // Cada cubo tem uma aceleração diferente.
        // Então, com o passar do tempo,
        // cada cubo tera um incremento diferente na sua velocidade.
        s_model->v = (float) count * 0.00001f;
        s_model->t = (float) 1.0f;
        s_model->a = (float) s_model->v / s_model->t;
        // v = a*t;

        // Save the model pointer
        m_disks[count] = (unsigned long) s_model;
    };


// ========================================================================
// Hero
// Special values for the hero.
    if ((void*) main_character != NULL)
    {

        // Assign colors for the main char (also humanoid)
        // hero — reuse the same body-part palette instead of the manual 6-range block
        assignBandColors(main_character, 12, humanoidPalette, 7);

        // ------------------------------

        // Origin
        //main_character->origin_x = (float)  0.0f;  // center horizontally
        //main_character->origin_y = (float) -3.0f;  // slightly lower (ground level)
        //main_character->origin_z = (float) (DEFAULT_MODEL_INITIAL_Z_POSITION + 1.0f);  // visible depth

        // Origin
        main_character->origin_x = (float)  0.0f;  // center horizontally
        main_character->origin_y = (float) -3.0f;  // slightly lower (ground level)
        main_character->origin_z = (float) (DEFAULT_MODEL_INITIAL_Z_POSITION + 4.0f);  // visible depth


        // Translations
        main_character->delta_x = (float) 0.0f;
        main_character->delta_y = (float) 0.0f;
        main_character->delta_z = (float) DEFAULT_MODEL_INITIAL_DELTA_Z;

        // Initializing.
        // Cada cubo tem uma aceleração diferente.
        // Então, com o passar do tempo,
        // cada cubo tera um incremento diferente na sua velocidade.
        main_character->v = (float) count * 0.00001f;
        main_character->t = (float) 1.0f;
        main_character->a = (float) main_character->v / main_character->t;
        // v = a*t;

        //main_character->rop = 51;  // ROP for the hero
        main_character->rop = 0;  // ROP for the hero
        main_character->interaction_radius = (float) 0.8f;
    }

//----------------
// Taskbar
    //demoClearWA(COLOR_BLACK);
    //wm_Update_TaskBar("Hello",TRUE);
    game_update_taskbar = FALSE;


//
// World
//

    if ((void *) current_world_3d == NULL){
        printf("current_world_3d\n");
        exit(0);
    }
    if (current_world_3d->magic != 1234){
        printf("current_world_3d magic\n");
        exit(0);
    }
}

