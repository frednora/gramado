// dtext.h
// Created by Fred Nora.

#ifndef __DTEXT_H
#define __DTEXT_H    1

//
// Text/String support.
//

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


#endif    

