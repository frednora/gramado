// comp.h
// Compositor. 
// Created by Fred Nora.

#ifndef __COMP_H
#define __COMP_H    1

// Compose
void compose(void);

// Mouse
long comp_get_mouse_x_position(void);
long comp_get_mouse_y_position(void);
void comp_set_mouse_position(long x, long y);
void mouse_at(void);
void comp_initialize_mouse(void);

#endif    
