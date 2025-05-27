/*
 * File: kmain.h
 *     Initialization support.
 * History:
 *     2015 - Created by Fred Nora.
 */

#ifndef __KMAIN_H
#define __KMAIN_H    1

// Saving the bootblock address passed by the blgram.
extern unsigned long saved_bootblock_base;

// Initialization support.
struct initialization_d
{

// Current phase
// #todo: Review this thing.
    int current_phase;

// Checkpoints:
// #bugbug: Some names here are deprecated.
    int phase1_checkpoint; 
    int phase2_checkpoint;
    int phase3_checkpoint;

// mm
    int mm_phase0;
    int mm_phase1;
    
// ke
    int ke_phase0;
    int ke_phase1;
    int ke_phase2;

//
// Flags
//

// Se ja podemos usar o dispositivo serial para log.
    int is_serial_log_initialized;
// Se ja podemos usar o console virtual para log.
    int is_console_log_initialized;

// Bootloader display device:
// See: bldisp.c
    int is_bldisp_initialized;

// #todo
    //int is_early_ps2_initialized;
    // int is_blkdev_initialized;
    // ...
    // int is_standard_stream_initialized;


// The kernel can print string into the screen
// only when it owns the display driver.
// When the window server is initialized,
// the kernel is not able to print string into the screen anymore.
// It's because we are not in raw mode anymore.
// In the case of SYSTEM PANIC the kernel needs to get back
// the display ownership to print out the final messages.
// The kernel has the ownership when we are using the
// embedded kernel shell.
    int kernel_owns_display_device;

// The system is operating in headless mode.
// See: config.h
    int headless_mode;

// Redirect printk to serial debug.
    int printk_to_serial;

    // ...
}; 

// Externam reference.
// see: init.c
extern struct initialization_d  Initialization;

// ========================


//
// Used during the kernel initialization.
//

// ::(1)
// The kernel starts at ke/x86_64/startup/
// see: '_kernel_begin' in head_64.asm.

// ::(2)
// Global initialization.
// see: init.c
void I_kmain(int arch_type);


// See: kmain.c
void init_globals(void);

#endif    


//
// End.
//

