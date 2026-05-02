// display.c
// Main file for the display manager.
// Created by Fred Nora.

#include <kernel.h>

int gKernelOwnsDisplay = TRUE;


// Initialized when the display device is initialized.
// See: bldisp.c, qemudisp.c ...
struct dc_d *dc_backbuffer;
struct dc_d *dc_frontbuffer;

// Local
static unsigned long __CurrentBackbufferPA = 0;
static unsigned long __CurrentBackbufferVA = 0;


// This is the kernel-side interface that 
// calls the service inside the display device driver.
// #ps: display device driver knows nothing about dc. This 
// structure is a thing used by the kernel as facilitator.

int 
display_putpixel0 ( 
    struct dc_d *dc,
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
    int rv=0;

    if ((void*) dc == NULL)
        goto fail;
    if (dc->magic != 1234)
        goto fail;

// Local copies from dc
    unsigned char *dc_where = dc->data;
    unsigned long dc_width  = dc->device_width;
    unsigned long dc_height = dc->device_height;
    unsigned long dc_bpp    = dc->bpp;    // bits per pixel
    unsigned long dc_pitch  = dc->pitch;  // bytes per row
    //unsigned int Color = (unsigned int) (_color & 0xFFFFFFFF);


    unsigned long msg[10];

    msg[0] = (unsigned long) dc_where;  // Address

    msg[1] = (unsigned long) dc_width;
    msg[2] = (unsigned long) dc_height;
    msg[3] = (unsigned long) dc_bpp;

    msg[4] = (unsigned long) dc_pitch;

    msg[5] = (unsigned long) _color;
    msg[6] = (unsigned long) _x;
    msg[7] = (unsigned long) _y;
    msg[8] = (unsigned long) _rop_flags;

//
// Call the display device driver
//

// #importante:
// Put pixel is a very latency sensitive function. Let's keep it 
// inside the core kernel in order to avoid cache locality issues.
    rv = (int) bldisp_putpixel0( (unsigned long) &msg[0] );
    return (int) rv;

fail:
    return (int) -1;
}





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




