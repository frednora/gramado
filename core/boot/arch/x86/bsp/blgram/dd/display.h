
// display.h
// Display device.

#ifndef __DD_DISPLAY_H
#define __DD_DISPLAY_H    1

struct display_device_d
{
    int initialized;

    unsigned long width;
    unsigned long height;
    unsigned long bpp;  // bytes per pixel.

    unsigned long pitch;  // line size in bytes.
    unsigned long screen_size_in_bytes;
};
extern struct display_device_d DisplayDevice;

#endif    


