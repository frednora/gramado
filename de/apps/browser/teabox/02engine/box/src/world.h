
// world.h

#ifndef __WORLD_H
#define __WORLD_H    1

//================================================

struct gr_world_d
{
    int used;
    int magic;
    int initialized;

    int x_size;
    int y_size;
    int z_size;

    struct gr_vec3D_d center;
// ==========================
// horizon
    struct gr_vec3D_d h1;
    struct gr_vec3D_d h2;
// vanishing points
    struct gr_vec3D_d vp1;
    struct gr_vec3D_d vp2;
};

extern struct gr_world_d  *CurrentWorld;

// ==============================================================
// Not using float.

int world_initialize(void);
int unveil_world(void);


#endif   



