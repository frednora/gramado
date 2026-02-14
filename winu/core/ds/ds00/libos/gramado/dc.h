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

// Struture for Device Context.
// Information about the drawing attributes of a device.
struct dc_d
{
    int used;
    int magic;
    int initialized;

// #test
// Low level component for device context support.
    struct dccanvas_d  *dc_canvas;

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

// hw info:

    // In pixels
    unsigned long device_width;
    unsigned long device_height;
    unsigned long bpp;  // Bytes Per Pixel

    // #todo:
    // unsigned long pitch; // bytes per line.


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


