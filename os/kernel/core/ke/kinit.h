/*
 * File: kinit.h
 *     Initialization support.
 * History:
 *     2015 - Created by Fred Nora.
 */

#ifndef __KE_INIT_H
#define __KE_INIT_H    1


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

    // ...
}; 

// Externam reference.
// see: init.c
extern struct initialization_d  Initialization;

// ========================

// See: kmain.c
// See: ke/x86_64/x64init.c
void keInitGlobals(void);

#endif    


//
// End.
//

