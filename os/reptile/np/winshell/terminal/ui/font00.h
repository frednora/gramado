// font00.h

#ifndef __FONT00_H
#define __FONT00_H  1

// #todo
// Future Plan: Font Info Request.
// Getting info from the server.
#define __CHAR_WIDTH  8
//#define __CHAR_HEIGHT  8
#define __CHAR_HEIGHT  16

struct font_info_d
{
    int initialized;
    int id;
    unsigned long width;
    unsigned long height;
};
// Defined in ui.c
extern struct font_info_d  FontInfo;

#endif  

