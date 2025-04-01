
// dc.h
// Device Context.

#ifndef __DC_H
#define __DC_H    1


/*
// Losethos colors.
public BGREntry vga_palette4_std[16] = {
    0x000000,0x0000AA,0x00AA00,0x00AAAA,
    0xAA0000,0xAA00AA,0xAA5500,0xAAAAAA,
    0x555555,0x5555FF,0x55FF55,0x55FFFF,
    0xFF5555,0xFF55FF,0xFFFF55,0xFFFFFF };


public BGREntry vga_palette4_gray[16] = {
    0x000000,0x111111,0x222222,0x333333,
    0x444444,0x555555,0x666666,0x777777,
    0x888888,0x999999,0xAAAAAA,0xBBBBBB,
    0xCCCCCC,0xDDDDDD,0xEEEEEE,0xFFFFFF };
*/

#define PALETTE_COUNT_MAX    16

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
    
    unsigned int color;
    unsigned int bg_color;
    unsigned long rop;

// hw info
    // in pixels.
    unsigned long device_width;
    unsigned long device_height;
    unsigned long bpp;

// virtual window.
// relativos a tela?
    unsigned long absolute_x;
    unsigned long absolute_y;
    unsigned long width;
    unsigned long height;

    unsigned long right;
    unsigned long bottom;

    unsigned long znear;
    unsigned long zfar;
    unsigned long zrange;


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


