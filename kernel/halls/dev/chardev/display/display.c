// display.c
// Main file for the display manager.
// Created by Fred Nora.

#include <kernel.h>


// Initialized when the display device is initialized.
// See: bldisp.c, qemudisp.c ...
struct dc_d *dc_backbuffer;
struct dc_d *dc_frontbuffer;

// Local
static unsigned long __CurrentBackbufferPA = 0;
static unsigned long __CurrentBackbufferVA = 0;


// backbuffer pa
void display_set_backbuffer_pa(unsigned long address)
{
    __CurrentBackbufferPA = (unsigned long) address;
}
unsigned long display_get_backbuffer_pa(void)
{
    return (unsigned long) __CurrentBackbufferPA;
}

// backbuffer va
void display_set_backbuffer_va(unsigned long address)
{
    __CurrentBackbufferVA = (unsigned long) address;
}
unsigned long display_get_backbuffer_va(void)
{
    return (unsigned long) __CurrentBackbufferVA;
}




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




