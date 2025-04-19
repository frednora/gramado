// rect.h
// Headers for rectangle support.
// Created by Fred Nora.


#ifndef __GRE_RECT_H
#define __GRE_RECT_H    1

/*
 * rect_d:
 *     Estrutura para gerenciamento de retângulos.
 *     Um retângulo pertence à uma janela.
 */
// #todo
// Usar isso para lidar com dirty rectangles.

struct rect_d 
{
    object_type_t  objectType;
    object_class_t objectClass;
    int used;
    int magic;

// Invalidate
// Sujo de tinta.
// If the rectangle is dirty, so it needs to be flushed into 
// the framebuffer.
// When we draw a window it needs to be invalidated.
    int dirty;

    int flag;
// Estilo de design
    int style;

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

    // 32 bit
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

