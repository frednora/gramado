// char.h
// Created by Fred Nora.


#ifndef __GRE_CHAR_H
#define __GRE_CHAR_H    1

//
// char
//

void set_char_width (int width);
int get_char_width (void);

void set_char_height (int height);
int get_char_height (void);


void 
d_draw_char ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor );

void 
d_drawchar_transparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    unsigned long c );

#endif   

