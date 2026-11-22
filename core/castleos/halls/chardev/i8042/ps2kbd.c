// ps2kbd.c
// Inteface for ps2 kbd support.
// This is the ps2/keyboard driver, embedded into the kernel base.
// ps/2 keyboard support.
// Created by Fred Nora.

// See:
// https://www.gnu.org/software/libc/manual/html_node/Canonical-or-Not.html


#include <kernel.h>


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


int ps2kbd_initialize_driver(const char *dev_name)
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


    if ((void*) dev_name == NULL)
        panic("ps2kbd_initialize_driver: dev_name");

    int rv = -1;
    rv = 
    (int) devmgr_register_legacy_device (
        (file *) fp, 
        dev_name,              // name 
        DEVICE_CLASS_CHAR,     // class (char, block, network)
        DEVICE_TYPE_LEGACY );  // type (pci, legacy)

    if (rv < 0){
        panic("kbd: on devmgr_register_legacy_device()\n");
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

// ps/2 keyboard irq handler
__VOID_IRQ 
irq1_KEYBOARD (void)
{

// Hardware interrupts (keyboard, mouse, NIC, ...).

    if (gszLastStackFrame == 3){
        lapic_info[0].irql = IRQL_IRQ_KERNEL; //#bugbug: We need the cpu id.

    } else if (gszLastStackFrame == 5){
        lapic_info[0].irql = IRQL_IRQ_USER; //#bugbug: We need the cpu id.

    } else {
        panic("irq1_KEYBOARD: gszLastStackFrame\n");
    }


// #todo:
// Actually it needs to be a pluggable callback. Like a filter.

    ps2khw_pluggable_handler();


/*
// #debug: 
// Tracing if some interrupt happens when the thread is in an invalid state.
// Current ------
    struct thread_d *CurrentThread;
    if (current_tid < 0 || current_tid >= THREAD_COUNT_MAX)
        return;
    CurrentThread = (struct thread_d *) threadList[current_tid];
    if (CurrentThread->magic != 1234)
        return;
    // It needs to be in a running state
    if (CurrentThread->state != RUNNING && CurrentThread->state != READY)
        panic ("K");
*/

// ExitIRQ:
    // #test
    // apic eoi
    if (CONFIG_INITIALIZE_IOAPIC_UNMASK_KBD == 1)
        local_apic_eoi(0);  // BSP

}

