// xpixel.c
// Presentation: Put a pixel into the backbuffer.
// Created by Fred Nora.

//D - Destination bitmap
//P - Selected brush (also called pattern)
//S - Source bitmap

// See:
// bit block transfer
// https://en.wikipedia.org/wiki/Bit_blit
// https://docs.microsoft.com/en-us/windows/win32/gdi/ternary-raster-operations

#include <kernel.h>


int 
backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
// Putpixel at the given buffer address.
// Return the number of changed pixels.

// #todo
// It depends on the current display device device driver.
// see: bldisp.c
    return (int ) bldisp_putpixel0(
        _color,
        _x,
        _y,
        _rop_flags,
        BACKBUFFER_VA );
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

// #todo
// It depends on the current display device device driver.
// see: bldisp.c
    return (int) bldisp_putpixel0(
        _color,
        _x,
        _y,
        _rop_flags,
        FRONTBUFFER_VA );
}

