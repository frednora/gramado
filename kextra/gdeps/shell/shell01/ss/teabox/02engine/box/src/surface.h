// surface.h
// Created by Fred Nora.

#ifndef __SURFACE_H
#define __SURFACE_H    1

void 
demoClearSurface(
    struct gws_window_d *clipping_window, 
    unsigned int color );

void demoFlushSurface(struct gws_window_d *clipping_window);
void demoClearWA(unsigned int color);

#endif   

