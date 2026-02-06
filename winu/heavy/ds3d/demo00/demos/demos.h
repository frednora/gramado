// demos.h
// Commom for all demos.

#ifndef __DEMOS_DEMOS_H
#define __DEMOS_DEMOS_H    1

// Use demos or not
extern int gUseDemos;

// The demos window. This is the canvas for demos.
extern struct gws_window_d *__demo_window;

// Global counter used by the demos.
extern unsigned long gBeginTick;

#define DIRECTION_LEFT   1
#define DIRECTION_RIGHT  2
#define DIRECTION_FRONT  3
#define DIRECTION_BACK   4

// =====================================

// Process key combinations only for demos 
int demos_on_combination(int msg_code);

char *demosReadFileIntoBuffer(const char *filename);

struct gws_window_d *__create_demo_window (
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height );


#endif   


