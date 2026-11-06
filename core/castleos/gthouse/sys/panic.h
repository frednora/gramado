// panic.h 
// Created by Fred Nora.

#ifndef __PANIC_H
#define __PANIC_H    1

// Early panic function.
// Print a panic message in the early stages.
void x_panic(const char *final_string);

void panic( const char *format, ... );
#define PANIC  panic

void panic_string(const char *string);

#endif   

