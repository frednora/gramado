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


// Kernel initialization error codes
// Each code corresponds to a main worker (initialization level).
// Main workers called by I_initialize_kernel().

// [1] earlyinit()
#define Error10_earlyinit     0x10  // Early environment setup failure

// [2:0] mmInitialize(0)
#define Error20_mmPhase0      0x20  // Memory manager phase 0 failure (heap/stack/memory sizing)

// [2:1] mmInitialize(1)
#define Error21_mmPhase1      0x21  // Memory manager phase 1 failure (frame pools/paging)

// [3:0] keInitialize(0)
#define Error30_kePhase0      0x30  // Executive phase 0 failure (font/background/display/linker data)

// [3:1] keInitialize(1)
#define Error31_kePhase1      0x31  // Executive phase 1 failure (scheduler/dispatcher/process/thread setup)

// [3:2] keInitialize(2)
#define Error32_kePhase2      0x32  // Executive phase 2 failure (graphics engine/GUI structures/callbacks)

// [4] archinit()
#define Error40_archinit      0x40  // Architecture initialization failure (processor probe/SMP/APIC)

// [5] deviceinit()
#define Error50_deviceinit    0x50  // Device initialization failure (PS/2/input targets/consoles)

// [6] lateinit()
#define Error60_lateinit      0x60  // Late initialization failure (PTYs/user subsystem/network/modules/init process)

// Extras
#define Error90_breakpoint    0x90  // Debug-only breakpoint


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
// The kernel starts at ...
// see: '_kernel_entry_point' in head_64.asm.

// ::(2)
// First function called by the BSP processor.
void I_kmain(int arch_type);
// First function called by all the AP processors.
void AP_kmain(void);

// See: kmain.c
void init_globals(void);

// See: kmain.c
void init_globals(void);


#endif    


//
// End.
//

