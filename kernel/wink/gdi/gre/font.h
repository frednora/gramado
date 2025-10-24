// fonts.h
// Fonts support.
// ring 0.
// Created by Fred Nora.

#ifndef  __FONTS_H
#define  __FONTS_H    1

// Fontes: 
// Endereços das fonts presentes no CMOS BIOS. 
// Esse enderço esta abaixo de 2MB, então
// temos identidade 1:1

#define BIOSFONT8X8  0x00000000000FFA6E    //8x8
//#define BIOSFONT8X8 0x000FFA6E
//#define BIOSFONT8X8 0x000FFA6E
//#define BIOSFONT8X8 0x000FFA6E
//...

// Char support
#define DEFAULT_CHAR_WIDTH   8
#define DEFAULT_CHAR_HEIGHT  8
//...

typedef enum {
    FONTNULL,
    FONT8X8,
    FONT8X16
    // ...
}font_type_t;

struct font_initialization_d
{
    int initialized;
    unsigned long address;
    unsigned long width;
    unsigned long height;

// index
// 8x8 | 8x16 ...
    int font_size;
};
extern struct font_initialization_d  FontInitialization;

//
// == prototypes ============================
//

int gwsGetCurrentFontCharWidth (void);
void gwsSetCurrentFontCharWidth (int width);
int gwsGetCurrentFontCharHeight (void);
void gwsSetCurrentFontCharHeight (int height);

unsigned long fontGetCurrentAddress(void);

void fontSetCurrentAddress(unsigned long address);

int gwsInstallFont (char *file_name);

int font_initialize(void);

#endif    
