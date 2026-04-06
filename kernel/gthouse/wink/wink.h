// wink.h
// Main file for the wink/ kernel module.
// Exporting some functions from wink to the 
// other components of the base kernel.
// Created by Fred Nora.

#ifndef __WINK_WINK_H
#define __WINK_WINK_H    1

extern int config_use_progressbar;

void 
wink_draw_string ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string );

int wink_load_gramado_icons(void);


void wink_update_progress_bar(unsigned long percent);

void refresh_screen(void);
void wink_refresh_screen(void);
void wink_putchar_in_fgconsole(unsigned long _char);

// Early panic function.
// Print a panic message in the early stages.
void x_panic(const char *final_string);
void wink_panic(const char *final_string);

void wink_show_banner(int clear_console);
void wink_initialize_default_kernel_font(void);
void wink_initialize_background(void);
void wink_initialize_video(void);
void wink_initialize_virtual_consoles(void);
void wink_set_cursor(unsigned long x, unsigned long y);

void wink_use_windowing_system(void);

#endif  

