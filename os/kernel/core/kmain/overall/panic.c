// panic.c
// Created by Fred Nora.

#include <kernel.h>    

// This function shows a formated string and 
// hang the system.

void panic( const char *format, ... )
{
    register int *varg = (int *) (&format);

// The kernel getback the display ownership
// to printout the messages into the screen.
// Maybe we need to reinitialize the display properties.
    Initialization.kernel_owns_display_device = TRUE;

// Se podemos usar o dispositivo serial para log.
    if (Initialization.is_serial_log_initialized == TRUE)
    {
        PROGRESS("panic:\n");
    }
// Se podemos usar o console virtual para log.
    if (Initialization.is_console_log_initialized == TRUE)
    {
        printk("panic: KERNEL PANIC\n");
        print( 0, varg );
    }
// Hang
    keDie();
}

