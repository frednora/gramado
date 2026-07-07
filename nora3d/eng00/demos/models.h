// models.h
// This is a place for structures of default models.

#ifndef __DEMOS_MODELS_H
#define __DEMOS_MODELS_H    1


// -----------------------------------------
struct cat_model_d
{
    int eyesVisible;
    int whiskersVisible;
    int mouthVisible;
    // ...
};
extern struct cat_model_d  CatModel;   // Cat model 0.


//struct humanoid_model_d
struct model_d
{
    // We don't need 32 vectors. But its ok.
    struct n3d_vec_d vecs[128]; //32
    int vertex_count; // how many faces are stored

    // Faces (12 triangles for a cube) 
    struct n3d_face_d faces[128];  // (16+1) each face has vi[3] indices 
    int face_count; // how many faces are stored

    unsigned int colors[128];  //32 #bugbug: colors needs to fit the number of faces.

    float fThetaAngle;

// World origin 

// Translation offsets
    float origin_x;  //hposition;  //horisontal position
    float origin_y;  //vposition;  //vertical position
    float origin_z;  //model_distance;  // Current z position

// Motion deltas (per-frame changes) 
    float delta_x; // change in x per frame 
    float delta_y; // change in y per frame 
    float delta_z; // change in z per frame

    // Motion parameters
    float a;  // Acceletarion: How fast the velocity changes.
    float v;  // Velocity:
    float t;  // Time:
};

extern struct model_d *main_character;


//
// Terrain
//

// -----------------------------------------
struct terrain_model_d
{
    struct n3d_vec_d vecs[200];      // 12x12 grid = 144, headroom for future files
    int vertex_count;

    struct n3d_face_d faces[400];    // 242 faces, headroom to ~14x14 grids later
    int face_count;

    unsigned int colors[400];        // must match faces[] capacity

    // Placement only -- terrain doesn't rotate, animate, or move once placed.
    float origin_x;
    float origin_y;
    float origin_z;
};

extern struct terrain_model_d *ground;


// Some initial values
#define DEFAULT_MODEL_INITIAL_Z_POSITION  (5.0f)
#define DEFAULT_MODEL_INITIAL_DELTA_Z     (0.005f)


// -----------------------------------------

void __setupCatModel(int eyes, int whiskers, int mouth );
void __draw_cat(int eye_scale, int cat_x, int cat_y, int cat_z);
void __draw_demo_curve1(int position, int model_z);
void drawRectangle0(float modelz);

#endif   

