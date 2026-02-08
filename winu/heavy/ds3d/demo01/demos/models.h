// models.h
// This is a place for structures of default models.


// -----------------------------------------
struct cat_model_d
{
    int eyesVisible;
    int whiskersVisible;
    int mouthVisible;
    // ...
};
extern struct cat_model_d  CatModel;   // Cat model 0.


struct humanoid_model_d
{
    // We don't need 32 vectors. But its ok.
    struct gr_vecF3D_d vecs[128]; //32
    int vertex_count; // how many faces are stored

    // Faces (12 triangles for a cube) 
    struct gr_face_d faces[128]; // (16+1) each face has vi[3] indices 
    int face_count; // how many faces are stored

    unsigned int colors[128]; //32 #bugbug: colors needs to fit the number of faces.

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

extern struct humanoid_model_d *main_character;

#define DEFAULT_CUBE_INITIAL_Z_POSITION  (5.0f)
#define DEFAULT_CUBE_INITIAL_DELTA_Z     (0.005f)


// -----------------------------------------

void __setupCatModel(int eyes, int whiskers, int mouth );
void __draw_cat(int eye_scale, int cat_x, int cat_y, int cat_z);
void __draw_demo_curve1(int position, int model_z);
void drawRectangle0(float modelz);






