// dc.h
// Device Context.
// Created by Fred Nora.

#ifndef __GRAMADO_DC_H
#define __GRAMADO_DC_H    1

#define PALETTE_COUNT_MAX  16

// ===========================

// Flags.
// Field for 'dc->flags'.
#define DCF_USE_PALETTE     1
#define DCF_RECORD_EXTENTS  2
#define DCF_CLIPPING        4   // Clipping to a given window.
#define DCF_DONT_DRAW       8
// ...

// Information about the drawing attributes of a device.
// See:
// https://docs.microsoft.com/en-us/windows/win32/gdi/device-contexts
// https://docs.microsoft.com/en-us/windows/win32/gdi/about-device-contexts
// https://docs.microsoft.com/en-us/cpp/mfc/device-contexts?view=msvc-170
// ...
struct dc_d
{
    int used;
    int magic;
    int initialized;

// Buffer ?
    unsigned char *data;

// z-buffer
// Esse é o z-buffer da tela toda 
// do dispositivo sendo descrito aqui.
// As vezes o dispositivo pode ser virtual e referirmos
// somente a uma janela ou a área de cliente dela.
// Então nesse caso odemos ter um z-buffer pequeno,
// menor que a tela do dispositivo.
// #test:
// As janelas também possuem um ponteiro para z-buffer dedicado.
    unsigned int *depth_buf;

// The display attributes.
    struct gws_display_d *display;

    unsigned long flags;

// hw info
    // in pixels.
    unsigned long device_width;
    unsigned long device_height;
    unsigned long bpp;

    unsigned long absolute_x;
    unsigned long absolute_y;
    unsigned long width;
    unsigned long height;

    unsigned long right;
    unsigned long bottom;

// center
    unsigned long hotspot_x;
    unsigned long hotspot_y;

// Record extents.
// virtual screen?
    unsigned long min_x;
    unsigned long max_x;
    unsigned long min_y;
    unsigned long max_y;


// #todo:
// We have and array with 16 colors with 4 bytes each.
    unsigned int palette[PALETTE_COUNT_MAX];

// Clipping window
    //struct gws_window_d *clipping_window;
// Bitmap support.
     //void *bmp_address;

// Navigation
// We can use this for copying from a context to another.
    struct dc_d *next;
};

// see: globals.c
extern struct dc_d  *gr_dc;   // default dc?

#endif    


