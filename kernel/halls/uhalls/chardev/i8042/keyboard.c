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
    //if (PS2Keyboard.irq_is_working == TRUE)
        //return;
    if (PS2Keyboard.use_polling == TRUE){
        irq1_KEYBOARD();
    }
}



int ps2kbd_initialize_driver(void)
{
    file *fp;

    fp = (file *) kmalloc(sizeof(file));
    if ((void *) fp == NULL){
        panic("kbd: fp\n");
    }
    memset ( fp, 0, sizeof(file) );
    fp->used = TRUE;
    fp->magic = 1234;

    fp->____object = ObjectTypeLegacyDevice;
    fp->isDevice = TRUE;

// #todo
    fp->dev_major = 0;
    fp->dev_minor = 0;

/*
// #test
// Registrando o dispositivo.
    devmgr_register_device ( 
        (file *) fp, 
        device_name_ps2kbd,  // name 
        DEVICE_CLASS_CHAR,   // class (char, block, network)
        DEVICE_TYPE_LEGACY,  // type (pci, legacy)
        NULL,                // Not a pci device.
        NULL );              // Not a tty device. (not for now)
*/

    int rv = -1;
    rv = 
    (int) devmgr_register_legacy_device (
        (file *) fp, 
        device_name_ps2kbd,  // name 
        DEVICE_CLASS_CHAR,   // class (char, block, network)
        DEVICE_TYPE_LEGACY );// type (pci, legacy)

    if (rv < 0){
        panic("kbd: devmgr_register_legacy_device fail\n");
    }

    return 0;
}

int i8042_IsPS2KeyboardPooling(void)
{
    return (int) PS2Keyboard.use_polling;
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
    PS2Keyboard.last_jiffy = (unsigned long) get_ticks();

// When calling during the IRQ
    if (PS2Keyboard.use_polling != TRUE){
        //return;
    }

// + Disable mouse port.
// + Call the main routine.
// + Reenable the mouse port if ps2 mouse was initialized.
// See: ps2kbd.c
    wait_then_write(0x64,0xA7);
    DeviceInterface_PS2Keyboard();
    if (PS2.mouse_initialized == TRUE){
        wait_then_write(0x64,0xA8);
    }

/*
// #debug: 
// Tracing if some interrupt happens when the thread is in an invalid state.
// Current ------
    struct thread_d *CurrentThread;
    if (current_thread < 0 || current_thread >= THREAD_COUNT_MAX)
        return;
    CurrentThread = (struct thread_d *) threadList[current_thread];
    if (CurrentThread->magic != 1234)
        return;
    // It needs to be in a running state
    if (CurrentThread->state != RUNNING && CurrentThread->state != READY)
        panic ("K");
*/
}

