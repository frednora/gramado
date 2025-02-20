
// comp.h

#ifndef __COMP_H
#define __COMP_H    1

long comp_get_mouse_x_position(void);
long comp_get_mouse_y_position(void);

void compose(void);
void comp_set_mouse_position(long x, long y);

void comp_initialize_mouse(void);

void mouse_at(void);

#endif    


