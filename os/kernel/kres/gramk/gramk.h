// gramk.h
// Exporting some functions from gramk to the 
// other components of the base kernel.
// Created by Fred Nora.


#ifndef __GRAMK_GRAMK_H
#define __GRAMK_GRAMK_H    1


void refresh_screen(void);
void gramk_refresh_screen(void);
void gramk_putchar_in_fgconsole(unsigned long _char);

// Early panic function.
// Print a panic message in the early stages.
void x_panic(const char *final_string);
void gramk_panic(const char *final_string);

#endif  


