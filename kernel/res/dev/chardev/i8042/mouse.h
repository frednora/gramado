// mouse.h
// Created by Fred Nora.

#ifndef ____MOUSE_H
#define ____MOUSE_H    1

int i8042_IsPS2MousePooling(void);

int ps2mouse_initialize_driver(void);
void ps2mouse_poll(void);

#endif   

