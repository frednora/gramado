// dc.h
// Device Context 

#ifndef __DISP_DC_H
#define __DISP_DC_H    1

// #important:
// bl_display_device → describes the hardware device (the monitor + framebuffer).
// dc_d → describes a drawing context (a canvas you draw into, which may or may not be the device’s framebuffer).
// bl_display_device is the physical layer.
// dc_d is the logical layer.

// Drawing context.
struct dc_d
{
    int used;
    int magic;
    int initialized;

    // Canvas to draw into
    unsigned char *data;   // backbuffer, frontbuffer, or offscreen

    // Hardware info
    unsigned long device_width;
    unsigned long device_height;
    unsigned long bpp;     // bytes per pixel

    unsigned long pitch;  // Bytes per line

// Navigation
    //struct dc_d *next;
};

#endif   

