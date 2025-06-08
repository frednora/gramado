// transf.h
// Transformation support.

#ifndef __TRANSF_H
#define __TRANSF_H    1

// =============================================================
// #projection:
// For orthographics projection there is no scaling factor.
// For perspective, we do have scaling.

// Camera and Perspective 
// camera:
// location, pointing at, pointing to top.
// .., lookat(view vector), top (upvector).
// perspective:
// view frustrum
// near, far ...
// from, to

// Window hotspot.
//static unsigned long WindowHotSpotX=0;
//static unsigned long WindowHotSpotY=0;

void transf_setup_hotspot(unsigned long x, unsigned long y);

void 
__transform_from_modelspace_to_screespace(
    int *res_x,
    int *res_y,
    int _x, int _y, int _z,
    int use_z_translation );



#endif  

