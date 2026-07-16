// panic.c
// Created by Fred Nora.

#include <kernel.h>    

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
    CONSOLE_TTYS[fg_console].vc_mode = KD_TEXT;

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

void string_panic(char *string)
{
    if ((void*) string == NULL)
        panic("string_panic: Invalid string");
    if (*string == 0)
        panic("string_panic: Invalid string");
    panic(string);
}
