// gws.h
// Created by Fred Nora.

#ifndef __UI_GWS_H
#define __UI_GWS_H    1


void invalidate(void);
void validate(void);

int isdirty(void);

void invalidate_background(void);
void validate_background(void);
int is_background_dirty(void);

void gws_show_backbuffer(void);

// init
int gwsInitGUI(void);
// ...

#endif    

