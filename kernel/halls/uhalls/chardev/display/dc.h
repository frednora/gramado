// dc.h
// Device Context 

#ifndef __DISPLAY_DC_H
#define __DISPLAY_DC_H    1

// #important:
//
// bl_display_device: 
// Describes the hardware device (the monitor + framebuffer).
//
// dc_d: 
// Describes a drawing context (a canvas you draw into, 
// which may or may not be the device’s framebuffer).
//
// bl_display_device is the physical layer.
// dc_d is the logical layer.

// Drawing context
// #ps: 
// Check surface.h where we have the structure surface_d, 
// which is the surface structure, used to represent a drawing surface, 
// which may be a window, a pixmap, or the screen itself.
struct dc_d
{
    int used;
    int magic;
    int initialized;

// Canvas to draw into
// backbuffer, frontbuffer, or offscreen
    unsigned char *data;

// Hardware info
    unsigned long device_width;
    unsigned long device_height;
    unsigned long bpp;  // Bits per pixel

    unsigned long pitch;  // Bytes per line

// Navigation
    //struct dc_d *next;
};

#endif   

