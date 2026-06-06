// gont.h
// Font support for libgui

#ifndef __LIBGUI_FONT_H
#define __LIBGUI_FONT_H    1


// #todo: Use this one
#define LIBGUI_DEFAULT_FONT_WIDTH  8
#define LIBGUI_DEFAULT_FONT_HEIGHT  8

// #deprecated
//#define DEFAULT_FONT_WIDTH  8
//#define DEFAULT_FONT_HEIGHT  8


struct libgui_font_info_d
{
    int initialized;
    int font_id;
    unsigned long address;
    unsigned long width;
    unsigned long height;
};

struct libgui_font_initialization_d
{
    int initialized;
    unsigned long address;
    unsigned long width;
    unsigned long height;
    int index_for_current_font;
};
extern struct libgui_font_initialization_d  FontInitialization;


// -----------------------------------------------

#define FONTDATAMAX  2048
extern unsigned char font_lin8x8[FONTDATAMAX];

#endif   

