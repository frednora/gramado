// proj.h
// Projection
// Created by Fred Nora.

#ifndef __LIBUI_PROJ_H
#define __LIBUI_PROJ_H    1

// ??
// graphical projection perspective
// See:
// https://en.wikipedia.org/wiki/Perspective_(graphical)
struct gr_projection_d
{
    int used;
    int magic;
    int initialized;

    // perspective or orthographic
    int type;

// fovy   Number The angle between the upper and lower 
// sides of the viewing frustum.
// aspect Number The aspect ratio of the viewing window. (width/height).
 
// The frustrum:   
// The rectangle that the can see.
// The limits in z axis. zmin and zmax;
// projection window.

    struct gr_rectangle_d  *frustrum_view;
    
// Distance to the near clipping plane along the -Z axis.
    int zNear;
// Distance to the far clipping plane along the -Z axis.
// In other words, the larger the z, the further is the object. 
// After the flip, the coordinate system is no longer a 
    int zFar; 
// zRange = zNear - zFar;
    int zRange;
// fov: field of view.
    int angle_of_view;
// The apex.
    // mid_x = (left + right) * 0.5;
    // mid_y = (bottom + top)  * 0.5;
    struct gr_vec3D_d  *frustrum_apex;
 // aspect ratio
    // ar = screen width / screen height
    // #bugbug: We are using int, not float.
    int ar;

// ------------------
// viewport: 
// All the view port information is found 
// the local device context.

    struct dc_d *dc;
};


extern struct gr_projection_d  *CurrentProjection;

// ======================================

int projection_initialize(void);
// Chaging the view for a given projection
int view (struct gr_projection_d *projection, int near, int far);
int gr_depth_range(struct gr_projection_d *projection, int near, int far);






#endif   

