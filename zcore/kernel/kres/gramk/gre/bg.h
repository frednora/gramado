
// bg.h
// Screen background support.
// Maybe wallpaper support.

#ifndef __BG_H
#define __BG_H    1

struct bg_info_d
{
    int initialized;
    unsigned int bg_color;
    unsigned long paint_cnt;
    //...
};
// see: bg.c
extern struct bg_info_d  background_info;

int displayInitializeBackground(unsigned int color,int show);

#endif    


