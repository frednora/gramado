// keyboard.c
// This is the ps2/keyboard driver, embedded into the kernel base.
// ps/2 keyboard support.
// Created by Fred Nora.

// See:
// https://www.gnu.org/software/libc/manual/html_node/Canonical-or-Not.html


#include <kernel.h>

const char *device_name_ps2kbd = "PS2KBD";

// Status
// @todo: Status pode ser (int).
// variáveis usadas pelo line discipline para controlar o 
// estado das teclas de controle.
// #todo: talvez isso possa ir pra dentro da estrutura 
// de teclado ps2.
unsigned long key_status=0;
unsigned long escape_status=0;
unsigned long tab_status=0;
unsigned long winkey_status=0;
unsigned long ctrl_status=0;
unsigned long alt_status=0;
unsigned long shift_status=0;
unsigned long capslock_status=0;
unsigned long numlock_status=0;
unsigned long scrolllock_status=0;
//...

// ==================================

//
// #test
// POLLING
//

// #test
// Poll keyboard
void ps2kbd_poll(void)
{
    if (PS2Keyboard.initialized != TRUE)
        return;
    if (PS2Keyboard.irq_is_working == TRUE)
        return;
    if (PS2Keyboard.use_polling == TRUE){
        DeviceInterface_PS2Keyboard();
    }
}



int ps2kbd_initialize_driver(void)
{
    file *fp;

    fp = (file *) kmalloc( sizeof(file) );
    if ((void *) fp == NULL){
        panic("kbd: fp\n");
    }
    memset ( fp, 0, sizeof(file) );
    fp->used = TRUE;
    fp->magic = 1234;
    fp->____object = ObjectTypeFile;
    fp->isDevice = TRUE;
// #todo
    fp->dev_major = 0;
    fp->dev_minor = 0;

// #test
// Registrando o dispositivo.
    devmgr_register_device ( 
        (file *) fp, 
        device_name_ps2kbd,  // name 
        DEVICE_CLASS_CHAR,   // class (char, block, network)
        DEVICE_TYPE_LEGACY,  // type (pci, legacy)
        NULL,                // Not a pci device.
        NULL );              // Not a tty device. (not for now)

    return 0;
}

//
// $
// HANDLER
//

// ps/2 keyboard irq handler.
__VOID_IRQ 
irq1_KEYBOARD (void)
{
// If ps2 keyboard isn't initialized yet.
    if (PS2.keyboard_initialized != TRUE){
        in8(0x60);
        return;
    }
    PS2Keyboard.irq_is_working = TRUE;
    PS2Keyboard.last_jiffy = (unsigned long) get_systime_totalticks();

// + Disable mouse port.
// + Call the main routine.
// + Reenable the mouse port if ps2 mouse was initialized.
// See: ps2kbd.c
    wait_then_write(0x64,0xA7);
    DeviceInterface_PS2Keyboard();
    if (PS2.mouse_initialized == TRUE){
        wait_then_write(0x64,0xA8);
    }
}



