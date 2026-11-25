
// color.h
// Small set of colors used by the 
// raw display device routines inside the kernel.

#ifndef __COLOR_H
#define __COLOR_H    1

// Basics
#define COLOR_WHITE    0x00FFFFFF
#define COLOR_BLACK    0x00000000
#define COLOR_RED      0x00FF0000 
#define COLOR_GREEN    0x0000FF00
#define COLOR_BLUE     0x000000FF
#define COLOR_GRAY     0x00808080 
#define COLOR_PINK     0x00FFC0CB
#define COLOR_ORANGE   0x00FFA500
#define COLOR_YELLOW   0x00FFFF00

// Cyan
#define COLOR_CYAN     0x0000FFFF

// Gray scale.
#define xCOLOR_BLACK  0x000000  //preto
#define xCOLOR_GRAY1  0x202020  //cinza mais escuro
#define xCOLOR_GRAY2  0x404040  //cinza
#define xCOLOR_GRAY3  0x606060  //cinza
#define xCOLOR_GRAY4  0x808080  //cinza
#define xCOLOR_GRAY5  0xa0a0a0  //cinza
#define xCOLOR_GRAY6  0xc0c0c0  //cinza
#define xCOLOR_GRAY7  0xe0e0e0  //cinza mais clarinho
#define xCOLOR_WHITE  0xffffff  //branco  

// Background
#define COLOR_EMBEDDED_SHELL_BG  COLOR_BLUE
#define COLOR_KERNEL_BACKGROUND  COLOR_BLUE

// Default bg color
#define CONSOLE_BLUE  COLOR_BLUE
// Default fg color
#define CONSOLE_GRAY   0x00303030
#define CONSOLE_WHITE  COLOR_WHITE

#endif   


