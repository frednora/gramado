// font.h
// Headers for font support.
// Created by Fred Nora.

#ifndef __UI_FONT_H
#define __UI_FONT_H    1


#define DEFAULT_FONT_WIDTH  8
#define DEFAULT_FONT_HEIGHT  8
extern char *font_lt8x8;


// 8x8
#define FONTDATAMAX  2048
extern unsigned char font_lin8x8[FONTDATAMAX];

// 8x16
#define FNT8X16_FONTDATAMAX   4096
extern unsigned char fontdata_8x16[FNT8X16_FONTDATAMAX];

// 8x8
// Losethos font.
// Inverted order.
// Credits: Terry Davis.
extern unsigned char __inverted_font_lt8x8[256*8];


//
// Font
//

struct font_info_d
{
    int initialized;
    int font_id;
    unsigned long address;
    unsigned long width;
    unsigned long height;
};

struct font_initialization_d
{
    int initialized;
    unsigned long address;
    unsigned long width;
    unsigned long height;
    int index_for_current_font;
};
// See: font.c
extern struct font_initialization_d  FontInitialization;

#define FONTLIST_MAX  4
extern unsigned long fontList[FONTLIST_MAX];

//
// == prototypes =============================
//

void 
fontSetInfo(
    unsigned long address, 
    unsigned long char_width, 
    unsigned long char_height );

void fontSetCurrentFontIndex(int index);

int font_initialize(void);

#endif   







