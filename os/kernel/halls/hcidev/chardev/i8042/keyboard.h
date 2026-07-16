// keyboard.h
// Created by Fred Nora.

#ifndef ____KEYBOARD_H
#define ____KEYBOARD_H    1

int i8042_IsPS2KeyboardPooling(void);

int ps2kbd_initialize_driver(void);

void ps2kbd_poll(void);

#endif   


