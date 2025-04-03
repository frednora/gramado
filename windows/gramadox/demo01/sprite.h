
// sprite.h

#ifndef  ____SPRITE_H
#define  ____SPRITE_H    1


// Types
// #todo: maybe create a 'enum'
// nope: we don't need a enum.

#define SPRITE_NULL   0
#define SPRITE_POINT  1
#define SPRITE_LINE       2
#define SPRITE_POLYLINE   3
#define SPRITE_TRIANGLE   4  
#define SPRITE_RECTANGLE  5
#define SPRITE_CIRCLE     6
#define SPRITE_ELLIPSE    7
#define SPRITE_BITMAP     8   // raw array of bytes.
#define SPRITE_POLYGON    9   // 
#define SPRITE_MESH       10  // 
// ...

struct gws_sprite_d
{
    int used;
    int magic;
    int initialized;

// ==================================

    // #todo
    // Position: Where the sprite begin?
    // x,y: inside a window?
    // pointer for memory position?
    // offset?
    // >>>> base

    void *base;
    // associated window.
    struct gws_window_d *window;
// ==================================

//  shape
    int type;

    unsigned long flags;

// color for raw objects.
    unsigned int color;
// dither color.
    unsigned int dither_color;
    unsigned int transparent_color;

// Used for max number in an iteration routine.
    int num;  //max

//#todo:
    //int vector_cnt;
    //int triangle_cnt;

// point support
// 2d and 3d
    int point_x;
    int point_y;
    int point_z;

// line and trianglesupport
// 2d and 3d
    int x1;
    int y1;
    int z1;
    unsigned int color1;
    int x2;
    int y2;
    int z2;
    unsigned int color2;
    int x3;
    int y3;
    int z3;
    unsigned int color3;

// circle support
    int r;  // radius
    int a;  // angle

// string support.
    void *string;
    int string_size;

// Navigation

    //struct gws_sprite_d *last;
    struct gws_sprite_d *next;
};


#define SPRITE_MAX_NUMBER 32
// Sprite list.
unsigned long spriteList[SPRITE_MAX_NUMBER];


/*
struct gws_spritedata_d
{
    int id;
    
    // window
    struct gws_window_d *window;
    int x;  // inside the client window.
    int y;

    struct gws_sprite_d *sprite;
    struct gws_sprite_d *next;
};
*/


//
// == prototypes ==============
//

int gr_display_sprite_list( struct gws_sprite_d *root );
int gr_sprite_cnt_list( struct gws_sprite_d *root );

void sprite_test1(void);

int 
gr_display_sprite( 
    struct dc_d *dc, 
    struct gws_sprite_d *sprite, 
    int x, int y, int z );

int 
PlotSprite3D ( 
    struct gws_display_d *display,
    int x, int y, int z,
    unsigned long sprite_list_address,
    int sprite_list_size,
    int just_one );


#endif    




