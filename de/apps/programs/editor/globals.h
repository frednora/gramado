
#ifndef __GLOBALS_H
#define __GLOBALS_H    1

#define PORTS_WS  4040
#define PORTS_NS  4041
#define PORTS_FS  4042


#define WINDOW_COUNT_MAX  1024
extern unsigned long windowList[WINDOW_COUNT_MAX];

extern unsigned long gScreenWidth;
extern unsigned long gScreenHeight;

// The display structure.
extern struct gws_display_d *Display;


#endif   

