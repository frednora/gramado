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


// =====================================


struct gws_window_d *__create_demo_window (
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height );


#endif   


