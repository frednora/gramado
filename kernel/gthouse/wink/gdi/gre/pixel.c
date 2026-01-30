// pixel.c
// Presentation: Put a pixel into the backbuffer.
// Created by Fred Nora.

// D - Destination bitmap
// P - Selected brush (also called pattern)
// S - Source bitmap

// See:
// bit block transfer
// https://en.wikipedia.org/wiki/Bit_blit

#include <kernel.h>


// ===================================================


int 
backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
// Putpixel at the given buffer address.
// Return the number of changed pixels.
// #todo:
// Maybe here is a good place for blending operations.

// #todo
// It depends on the current display device device driver.
// see: bldisp.c
/*
    return (int ) bldisp_putpixel0(
        _color,
        _x,
        _y,
        _rop_flags,
        BACKBUFFER_VA );
*/

    // #test: using decive context
    return (int) bldisp_putpixel0(dc_backbuffer, _color, _x, _y, _rop_flags);
}

int 
frontbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
// Putpixel at the given buffer address.
// Return the number of changed pixels.
// #todo:
// Maybe here is a good place for blending operations.

// #todo
// It depends on the current display device device driver.
// see: bldisp.c

    /*
    return (int) bldisp_putpixel0(
        _color,
        _x,
        _y,
        _rop_flags,
        FRONTBUFFER_VA );
    */

    // #test: using decive context
    return (int) bldisp_putpixel0(dc_frontbuffer, _color, _x, _y, _rop_flags);
}

