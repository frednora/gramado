// rect.h
// Headers for rectangle support.
// Created by Fred Nora.

#ifndef __GRE_RECT_H
#define __GRE_RECT_H    1


// Structure for rectangle management.
struct rect_d 
{
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;

    // unsiged int ?
    int flag;

// We can invalidade the rectangle setting this flag.
// If the rectangle is dirty, it means that 
// it needs to be flushed to the LFB.
    int dirty;

    int style;  // ?

//dimensoes
    unsigned long x;
    unsigned long y;
    unsigned long cx;
    unsigned long cy;

//margins
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    unsigned long right;
    unsigned long bottom;

    unsigned int bg_color; 

    struct rect_d *next;
};


//
// ============================================
//

// Draw rectangle into the backbuffer.
int 
backbuffer_draw_rectangle( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags );

int 
frontbuffer_draw_rectangle( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags );

void 
refresh_rectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height );


void scroll_screen_rect (void);

#endif   

