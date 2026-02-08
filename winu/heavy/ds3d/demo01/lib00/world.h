// world.h
// Created by Fred Nora

#ifndef __LIB00_WORLD_H
#define __LIB00_WORLD_H    1


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


struct gr_word3d_d 
{
    int used;
    int magic;
    int initialized;

    float x_size;
    float y_size;
    float z_size;

    //struct gr_vecF3D_d *
};
extern struct gr_word3d_d  *current_world_3d;


// ==============================================================
// Not using float.

int world_initialize(void);
int unveil_world(void);


#endif   



