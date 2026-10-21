// pixel.h
// Created by Fred Nora.

#ifndef __GRE_PIXEL_H
#define __GRE_PIXEL_H    1

//
// pixel
//

int 
backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

int 
frontbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );


#endif  

