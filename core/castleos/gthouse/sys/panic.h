// panic.h 
// Created by Fred Nora.

#ifndef __PANIC_H
#define __PANIC_H    1

void panic( const char *format, ... );
#define PANIC  panic

void panic_string(const char *string);

#endif   

