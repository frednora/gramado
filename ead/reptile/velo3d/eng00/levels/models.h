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


// =============================================
// Model Type Tags
// =============================================
typedef enum {
    MODEL_TYPE_INVALID     = 0,
    MODEL_TYPE_HUMANOID    = 1,   // Player + enemies
    MODEL_TYPE_DISK        = 2,   // Flying saucers
    MODEL_TYPE_BUILDING    = 3,
    MODEL_TYPE_PROP        = 4,   // trees, rocks, collectibles...
    MODEL_TYPE_PROJECTILE  = 5,
    MODEL_TYPE_EFFECT      = 6,   // explosions, particles, etc.
    // ... add more as needed
} model_type_t;

// Ideas for the model:
// 1. Core Vital Stats
// 2. Position & Movement
// 3. Attributes / Stats (RPG / Action games)
// 4. Combat & Status
// 5. Progression & Economy
// 6. Inventory & Equipment
// 7. Appearance & Customization (especially in modern games)

struct model_d
{
    // Header
    int            id;          // Unique identifier / index
    model_type_t   tag;         // Type tag

    // Geometry

    // We don't need 32 vectors. But its ok.
    struct n3d_vec_d vecs[128]; //32
    int vertex_count; // how many faces are stored

    // Faces (12 triangles for a cube) 
    struct n3d_face_d faces[128];  // (16+1) each face has vi[3] indices 
    int face_count; // how many faces are stored

    unsigned int colors[128];  //32 #bugbug: colors needs to fit the number of faces.

    unsigned long rop;  // Desired ROP value

    float fThetaAngle;

// Position & Movement

// World origin 
// Translation offsets
    float origin_x;  //hposition;  //horisontal position
    float origin_y;  //vposition;  //vertical position
    float origin_z;  //model_distance;  // Current z position

// Motion deltas (per-frame changes) 
    float delta_x; // change in x per frame 
    float delta_y; // change in y per frame 
    float delta_z; // change in z per frame

    // Physics / Animation
    float a;  // acceleration: How fast the velocity changes
    float v;  // velocity
    float t;  // time / angle accumulator

//
// Game logic
//

    // Collision flag
    int collided;     // current collision type

// #test
// (STATE_IDLE, STATE_MOVING, STATE_COLLIDED, STATE_GHOST).
// Grounded / Airborne / Swimming / Flying state
    int state;

    int health_value;  // Its about damage level i guess.
    int score_value;

    int level;  // Level
    int xp;     // Experience Points (XP) / Progress toward next level

    int lives;  // Lives (if it's a lives-based game like platformers)


// Interaction radius → defines how close the player must be 
// to trigger exploration events (talk, collect, inspect).
    float interaction_radius;

    // Collectibles
    // a structure can handle this.

// Attributes / Stats (RPG / Action games) Strength, 
// Agility, Intelligence, Vitality, Luck, etc.
// Attack / Defense values
// Movement speed
// Attack speed / Cooldown reduction
// Critical chance / Critical damage

    int IsAlive;

    // Future extensions
    // void* extra_data;   // for type-specific data if needed later
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

    unsigned long rop;  // Desired ROP value
};

extern struct terrain_model_d *ground;


// Some initial values
#define DEFAULT_MODEL_INITIAL_Z_POSITION  (6.0f)


#define DEFAULT_MODEL_INITIAL_DELTA_X     (0.4f)
#define DEFAULT_MODEL_INITIAL_DELTA_Y     (0.4f)
#define DEFAULT_MODEL_INITIAL_DELTA_Z     (0.4f)

// -----------------------------------------

void __setupCatModel(int eyes, int whiskers, int mouth );
void __draw_cat(int eye_scale, int cat_x, int cat_y, int cat_z);
void __draw_demo_curve1(int position, int model_z);
void drawRectangle0(float modelz);

#endif   

