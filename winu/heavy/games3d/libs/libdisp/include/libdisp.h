// libdisp.h
// Graphics device library.
// Created by Fred NOra.

#ifndef __LIBDISP_H
#define __LIBDISP_H  1

#include "vk.h"       // # view input events
#include "lt8x8.h"


int libgd_initialize(void);

int 
libdisp_backbuffer_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop );

int 
grBackBufferPutpixel2 ( 
    unsigned int color, 
    int x, 
    int y );

// put pixel
// low level.
int 
fb_BackBufferPutpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    unsigned long buffer_va );

int
putpixel0 ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags,
    unsigned long buffer_va );

void 
backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

void 
frontbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags );

int 
libgd_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    int back_or_front );

unsigned int grBackBufferGetPixelColor( int x, int y );

#endif    



