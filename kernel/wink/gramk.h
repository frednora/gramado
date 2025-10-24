// gramk.h
// Main file for the gramk/ kernel module.
// Exporting some functions from gramk to the 
// other components of the base kernel.
// Created by Fred Nora.


#ifndef __GRAMK_GRAMK_H
#define __GRAMK_GRAMK_H    1


extern int config_use_progressbar;

void gramk_update_progress_bar(unsigned long percent);

void refresh_screen(void);
void gramk_refresh_screen(void);
void gramk_putchar_in_fgconsole(unsigned long _char);

// Early panic function.
// Print a panic message in the early stages.
void x_panic(const char *final_string);
void gramk_panic(const char *final_string);

void gramk_show_banner(int clear_console);
void gramk_initialize_default_kernel_font(void);
void gramk_initialize_background(void);
void gramk_initialize_video(void);
void gramk_initialize_virtual_consoles(void);
void gramk_set_cursor( unsigned long x, unsigned long y );

#endif  


