// libgr3d.c
// Support for 3D routines.
// sing float values.
// Created by Fred Nora.

/*
// Functions found here:

gr_MultiplyMatrixVector:
transforms a 3D vector by a 4x4 matrix, handling translation and perspective division.

grVectorCrossProduct:
gives you a perpendicular vector (useful for normals).

dot_productF:
calculates how much two vectors point in the same direction.

gr_discriminant:
is used for intersection tests (like in ray tracing) to check if a quadratic equation has real solutions.
*/

#include "include/libgr3d.h"

int libgr_dummy=0;

// =============================================

// gr_MultiplyMatrixVector
// Multiplies a 3D vector by a 4x4 transformation matrix.
// This is commonly used for transformations like rotation, scaling, translation, and 
// projection in 3D graphics.
// Inputs:
//   i - input vector (x, y, z). Assumes w = 1 for position vectors.
//   o - output vector (result of transformation)
//   m - 4x4 transformation matrix
// #ps: Normalized.
void 
gr_MultiplyMatrixVector(
    struct gr_vecF3D_d *i, 
    struct gr_vecF3D_d *o, 
    struct gr_mat4x4_d *m )
{
    // Multiply the input vector by the matrix to get the transformed coordinates.
    // The last row/column handle translation and perspective divide.
    // Output is calculated as:
    // o.x = i.x * m00 + i.y * m10 + i.z * m20 + m30

    o->x = 
        (float) (
        i->x * m->m[0][0] + 
        i->y * m->m[1][0] + 
        i->z * m->m[2][0] + 
        m->m[3][0] );

    o->y = 
        (float) (
        i->x * m->m[0][1] + 
        i->y * m->m[1][1] + 
        i->z * m->m[2][1] + 
        m->m[3][1] );
    
    o->z = 
        (float) (
        i->x * m->m[0][2] + 
        i->y * m->m[1][2] + 
        i->z * m->m[2][2] + 
        m->m[3][2] );

    // Compute the homogeneous coordinate w
    float w = 
        (float) (
        i->x * m->m[0][3] + 
        i->y * m->m[1][3] + 
        i->z * m->m[2][3] + 
        m->m[3][3] );

// Normalization
// If w is not 1.0, normalize the result to convert from homogeneous coordinates to 3D space
// This ensures that distant objects shrink properly, creating depth perception.
// Key Takeaway
// Without this final normalization, the transformed points would remain in homogeneous space, 
// meaning they wouldn’t correctly scale with distance. The division by 𝑤 ensures proper 
// perspective distortion, making your 3D scene visually correct.

    if (w != 0.0f)
    {
        o->x = (float) (o->x / w); 
        o->y = (float) (o->y / w); 
        o->z = (float) (o->z / w);
    }
}

// grVectorCrossProduct
// Calculates the cross product of two 3D vectors.
// The cross product produces a vector that is perpendicular to both input vectors.
// Useful for computing normals in 3D graphics.
struct gr_vecF3D_d *grVectorCrossProduct(
    struct gr_vecF3D_d *v1, 
    struct gr_vecF3D_d *v2 )
{
//#todo: Not tested yet.

    struct gr_vecF3D_d vRes;

    // Cross product formula:
    // x = y1*z2 - z1*y2
    // y = z1*x2 - x1*z2
    // z = x1*y2 - y1*x2
    vRes.x = (float) (v1->y * v2->z - v1->z * v2->y);
    vRes.y = (float) (v1->z * v2->x - v1->x * v2->z);
    vRes.z = (float) (v1->x * v2->y - v1->y * v2->x);

    return (struct gr_vecF3D_d *) &vRes;
}

// dot_productF
// Computes the dot product (scalar product) of two 3D vectors.
// The result is a scalar value that describes how aligned the vectors are.
// If the result is 0, the vectors are perpendicular.
// If positive, they point in similar directions. If negative, opposite.
// Dot product.
// The dot product describe the 
// relationship between two vectors.
// Positive: Same direction
// negative: Opposite direction
// 0:        Perpendicular.
float dot_productF( struct gr_vecF3D_d *v1, struct gr_vecF3D_d *v2 )
{

// Safety check for NULL pointers
// Fake perpendicular
    if ( (void*) v1 == NULL ){ return (float) 0.0f; }
    if ( (void*) v2 == NULL ){ return (float) 0.0f; }

// Standard formula: x1*x2 + y1*y2 + z1*z2
// (x*x + y*y + z*z)
    return (float) ( v1->x * v2->x + 
                     v1->y * v2->y + 
                     v1->z * v2->z );
}

// -------------------------------
// gr_discriminant
// Computes the discriminant (delta) for a quadratic equation: ax^2 + bx + c = 0
// Used for determining the number of intersections (e.g., in ray tracing).
// Returns:
//   < 0 : No real roots (no intersection)
//   = 0 : One real root (tangent or touching)
//   > 0 : Two real roots (intersection)
// -------------------------------
// Get delta for bhaskara. (pt-br)
// d<0: (negative) "Raiz de número negativo em Baskara"
// d=0: (null)     duas raizes reais iguais.
// d>0: (positive) duas raizes reais diferentes. (Intersection)
// -------------------------------
// Used to test for intesection in the ray tracing.
// Discriminant: Delta da função em bhaskara.
float gr_discriminant(float a, float b, float c)
{
    float Discriminant = (float) ((b*b) - (4*a*c));
    return (float) Discriminant;
}


/*
void 
transform_oblique_matrix(
    float x, float y, float z, 
    float hotspotx, float hotspoty, 
    int lefthand, 
    float *outX, float *outY ) 
{
    float zproj = (z >= 0) ? z : -z;
    int sign    = (z >= 0) ? 1 : -1;

    if (lefthand) {
        *outX = hotspotx + x + sign * zproj;
        *outY = hotspoty - y - sign * zproj;
    } else {
        *outX = hotspotx + x - sign * zproj;
        *outY = hotspoty - y + sign * zproj;
    }
}
*/


// Summary
// This function mimics your original algorithm using matrix multiplication.
// The matrix coefficients change depending on the sign of z.
// You still need to handle the sign and absolute value logic outside the matrix, 
// as is typical for this kind of “oblique” projection.

/*
void 
transform_oblique_matrix(
    float x, float y, float z,
    float hotspotx, float hotspoty,
    int lefthand,
    float *outX, float *outY ) 
{
    float zproj = (z >= 0) ? z : -z;
    int sign    = (z >= 0) ? 1 : -1;

    float mat[2][3];

    if (lefthand) {
        mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = sign;
        mat[1][0] = 0; mat[1][1] = -1; mat[1][2] = -sign;
    } else {
        mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = -sign;
        mat[1][0] = 0; mat[1][1] = -1; mat[1][2] = sign;
    };

    *outX = hotspotx + mat[0][0]*x + mat[0][1]*y + mat[0][2]*zproj;
    *outY = hotspoty + mat[1][0]*x + mat[1][1]*y + mat[1][2]*zproj;
}
*/
