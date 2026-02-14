// char.h
// Created by Fred Nora.

#ifndef __LIBUI_CHAR_H
#define __LIBUI_CHAR_H  1


struct char_initialization_d
{
    int initialized;

    int width;
    int height;
};
extern struct char_initialization_d  CharInitialization;


// ========================================

void charSetCharWidth (int width);
void charSetCharHeight (int height);
int charGetCharWidth (void);
int charGetCharHeight (void);

void 
charBackbufferCharBlt ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long color, 
    unsigned long c );

// Draw a char but don't change the background color
void 
grBackbufferDrawCharTransparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    int ch );

// Given a pointer for the font base address
void 
grBackbufferDrawCharTransparent2 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    int ch,
    char *stock_address );

// Draw a char and change the background color
void 
grBackbufferDrawChar ( 
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor );

// Draw char into a given device context canvas
void 
dc_drawchar (
    struct dccanvas_d *dc, 
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor,
    unsigned long rop );

void
dc_drawchar_block_style(
    struct dccanvas_d  *dc,
    unsigned long   x,
    unsigned long   y,
    unsigned long   c,
    unsigned int    fgcolor,
    unsigned int    bgcolor,
    unsigned long   rop,
    int             scale,
    int             opaque_mode );

void
grBackbufferDrawCharBlockStyle(
    unsigned long x,          // top-left in screen space
    unsigned long y,
    unsigned int  fgcolor,
    int           ch,         // character code
    int           scale);      // 1 = classic 8×8, 2 = 16×16 blocks, etc.

void
grDrawCharBlockStyleInsideWindow(
    int           wid,
    unsigned long rel_x,
    unsigned long rel_y,
    unsigned int  fgcolor,
    int           ch,
    int           scale,
    unsigned long rop );

int char_initialize(void);

#endif

//
// End
//

