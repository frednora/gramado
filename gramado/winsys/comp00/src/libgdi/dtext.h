// dtext.h
// Draw text.
// Created by Fred Nora.

#ifndef __LIBUI_DTEXT_H
#define __LIBUI_DTEXT_H    1

//
// Text/String support.
//


void 
dc_drawstring ( 
    struct dccanvas_d *dc, 
    unsigned long x,
    unsigned long y,
    unsigned int fg_color,
    unsigned int bg_color,
    unsigned long rop,
    unsigned char *string );


//---
void 
grDrawString ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    unsigned char *string );
              
void 
dtextDrawText ( 
    struct gws_window_d *window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    char *string );

void 
dtextDrawText2 ( 
    struct gws_window_d *window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    char *string,
    int flush );
//---

void 
DrawStringBlock(
    int wid,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *str,
    int scale );


#endif    

