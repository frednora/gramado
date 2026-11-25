// panic.c
// Created by Fred Nora.

#include <kernel.h>    


static void __x_panic_show_message(const char *final_string, unsigned long flags);


// =======================================

// __x_panic_show_message: (worker)
// Draw a small red rectangle in the top/left
// and print a message into it.
// IN:
// final_message - string
// flags - #todo
static void __x_panic_show_message(const char *final_string, unsigned long flags)
{
    unsigned long rectLeft=0;
    unsigned long rectTop=0;
    unsigned long rectWidth = gSavedX;  // (Device width) see: kmain.c
    unsigned long rectHeight = (8*3); // Char height * 3  //28;
    unsigned int RectColor = COLOR_RED;

    unsigned long orLeft=rectLeft;
    unsigned long orTop=rectHeight;
    unsigned long orWidth = rectWidth;
    //unsigned long orHeight=1;
    unsigned int OrnamentColor = COLOR_YELLOW;

    unsigned long s1_left=8;
    unsigned long s1_top=8;
    unsigned int s1_color = COLOR_YELLOW;

    unsigned long s2_left=80;
    unsigned long s2_top=8;
    unsigned int s2_color = COLOR_WHITE;

// #todo
// Check the string size?

//---------------
// Box: Rectangle and horizontal line

    backbuffer_draw_rectangle ( 
        rectLeft, rectTop, rectWidth, rectHeight, 
        RectColor, 
        0 );
    backbuffer_draw_horizontal_line( 
        orLeft, orTop, orWidth,
        OrnamentColor, 
        0 );

//---------------
// Text: String 1 and 2

    draw_string(s1_left, s1_top, s1_color, "PANIC: ");
    if ((void*) final_string != NULL){
        draw_string(s2_left, s2_top, s2_color, final_string);
    }

    refresh_screen();  // Flush
}

// Print the string into a box and halt carefully
void x_panic(const char *final_string)
{
    const unsigned long Flags = 0;

    if ((void*) final_string == NULL){
        __x_panic_show_message("x_panic: final_string", Flags);
        goto done;
    }

    __x_panic_show_message(final_string, Flags);

done:
    while (1){
        asm ("cli");  
        asm ("hlt"); 
    };
}

// This function shows a formated string and hang the system.
// Environment: It prints into the foreground kernel console.
void panic( const char *format, ... )
{
    register int *varg = (int *) (&format);

// The kernel getback the display ownership
// to printout the messages into the screen.
// Maybe we need to reinitialize the display properties.

    Initialization.kernel_owns_display_device = TRUE;

// Target console
    if (fg_console < 0 || fg_console > 3)
        fg_console = DEFAULT_CONSOLE;

// Kernel draws glyphs into the framebuffer
    console_set_session_type(fg_console, TTY_SESSION_TYPE_CONSOLE);

// If we can log into the serial device
    if (Initialization.is_serial_log_initialized == TRUE)
    {
        PROGRESS("panic:\n");
    }

// If we can log into the kenrel console
    if (Initialization.is_console_log_initialized == TRUE)
    {
        printk("panic: KERNEL PANIC\n");
        print( 0, varg );
    }

    // #todo
    // If headless, use serial_printk().

// Hang
    keDie();
}

void panic_string(const char *string)
{
    if ((void*) string == NULL)
        panic("panic_string: string");
    if (*string == 0)
        panic("panic_string: *string");

//
// panic
//

    panic(string);
}
