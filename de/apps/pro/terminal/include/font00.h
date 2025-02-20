// font00.h

#ifndef __FONT00_H
#define __FONT00_H  1

struct font_info_d
{
    int initialized;
    int id;
    unsigned long width;
    unsigned long height;
};
extern struct font_info_d  FontInfo;

#endif  

