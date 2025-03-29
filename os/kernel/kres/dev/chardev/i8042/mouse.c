// mouse.c
// This is the ps2/mouse driver, embedded into the kernel base.
// ps/2 mouse support.
// ring 0, kernel base.
// Created by Fred Nora.

#include <kernel.h>

const char *device_name_ps2mouse = "PS2MOUSE";


unsigned long g_mousepointer_width=0;
unsigned long g_mousepointer_height=0;
// Estado dos botões do mouse
int mouse_button_1=FALSE;
int mouse_button_2=FALSE;
int mouse_button_3=FALSE;
// Estado anterior dos botões do mouse.
int old_mouse_button_1=FALSE;
int old_mouse_button_2=FALSE;
int old_mouse_button_3=FALSE;
// Se ouve alguma modificação no estado dos botões.
int mouse_button_action=FALSE;

// ==============

//
// $
// POLLING
//

// #test
// Poll keyboard
void ps2mouse_poll(void)
{

// #bugbug
// #todo
// We need a loop for mouse polling.
// It's because a packet uses more than on interrupt.

/*
    if (PS2Mouse.initialized != TRUE)
        return;
    if (PS2Mouse.irq_is_working == TRUE)
        return;
    if (PS2Mouse.use_polling == TRUE){
        DeviceInterface_PS2Mouse();
    }
*/
}


int ps2mouse_initialize_driver(void)
{
    file *fp;

    fp = (file *) kmalloc( sizeof(file) );
    if ((void *) fp == NULL){
        panic("ps2mouse.c: fp\n");
    }
    memset ( fp, 0, sizeof(file) );
    fp->used = TRUE;
    fp->magic = 1234;
    fp->____object = ObjectTypeFile;

    fp->isDevice = TRUE;
// #todo
    fp->dev_major = 0;
    fp->dev_minor = 0;

// Register the device.
    devmgr_register_device ( 
        (file *) fp, 
        device_name_ps2mouse,  // name 
        DEVICE_CLASS_CHAR,     // class (char, block, network)
        DEVICE_TYPE_LEGACY,    // type (pci, legacy)
        NULL,                  // Not a pci device.
        NULL );                // Not a tty device. (not for now)

    return 0;
}

//
// $
// HANDLER
//

// ps/2 mouse irq handler.
__VOID_IRQ 
irq12_MOUSE (void)
{
// If ps2 mouse isn't initialized yet.
    if (PS2.mouse_initialized != TRUE){
        in8(0x60);
        return;
    }
    PS2Mouse.irq_is_working = TRUE;
    PS2Mouse.last_jiffy = (unsigned long) get_systime_totalticks();

// Disable keyboard port.
// Call the main routine.
// Reanable keyboard port.
// See: ps2mouse.c
    wait_then_write(0x64,0xAD);
    DeviceInterface_PS2Mouse();
    wait_then_write(0x64,0xAE);
}

