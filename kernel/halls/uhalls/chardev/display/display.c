// display.c
// Main file for the display manager.
// Created by Fred Nora.


#include <kernel.h>


//
// #
// INITIALIZATION
//

// Initialize the device driver for the device cotroller.
int displayInitialize(void)
{
    int Status = -1;

    debug_print("displayInitialize:\n");

// Device driver initialization
// Initialize the device driver for the bootloader display device.
// Device driver for bootloader display device.
// Initialize the structure for the bootloader display device.
// see: bldisp.c

    Status = (int) DDINIT_bldisp();
    // qemudisp? DDINIT_qemudisp()
    // vga?
    // ...

    if (Status < 0)
        debug_print("displayInitialize: on DDINIT_bldisp()\n");

    return 0;
}




