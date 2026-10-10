// Projection 
// by Copilot

#include <stdio.h>
#include <math.h>

// ===============================
// Trigonometry workers
// ===============================

/*
To actually draw on a screen 
(which has pixel dimensions: width × height), 
you need to map from [-1,1] → [0,width] or [0,height].
*/
double screen_x(double x, double width) {
    return ((x + 1) / 2) * width;
}
double screen_y(double y, double height) {
    return (1 - (y + 1) / 2) * height;
}


// Compute x coordinate on circle of radius r
double circle_x(double r, double angle_rad) {
    return r * cos(angle_rad);
}

// Compute y coordinate on circle of radius r
double circle_y(double r, double angle_rad) {
    return r * sin(angle_rad);
}

// ===============================
// Projection workers
// ===============================

// Basic perspective projection (no aspect ratio)
void project_point(double x, double y, double z, double *x_out, double *y_out) {
    if (z == 0) {
        // Avoid division by zero
        *x_out = 0;
        *y_out = 0;
        return;
    }
    *x_out = x / z;
    *y_out = y / z;
}

// Perspective projection with aspect ratio correction
void project_point_ar(double x, double y, double z, double aspect_ratio,
                      double *x_out, double *y_out) {
    if (z == 0) {
        *x_out = 0;
        *y_out = 0;
        return;
    }
    *x_out = x / (z * aspect_ratio);
    *y_out = y / z;
}

// ===============================
// Demo usage
// ===============================
int main() {
    double r = 1.0;
    double angle = M_PI / 4; // 45 degrees

    double x = circle_x(r, angle);
    double y = circle_y(r, angle);
    printf("Circle point (r=%.2f, angle=45°): x=%.3f, y=%.3f\n", r, x, y);

    double xp, yp;
    project_point(2.0, 4.0, 2.0, &xp, &yp);
    printf("Projection (2,4,2): x'=%.3f, y'=%.3f\n", xp, yp);

    project_point_ar(2.0, 4.0, 2.0, 2.0, &xp, &yp);
    printf("Projection with AR=2 (2,4,2): x'=%.3f, y'=%.3f\n", xp, yp);

    return 0;
}
