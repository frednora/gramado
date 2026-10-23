// kmain.h
// Kernel initialization support
// 2015 - Created by Fred Nora.

#ifndef __KMAIN_H
#define __KMAIN_H    1


// see: kmain.c
extern struct admin_utsname_d  kernel_utsname;


// Saving the bootblock address passed by the blgram.
extern unsigned long saved_bootblock_base;

//
// 1
//

#define Error1_I_initialize_kernel  0x1
// Kernel initialization error codes
// Each code corresponds to a main worker (initialization level).
// Main workers called by I_initialize_kernel().
// [1:1] earlyinit()
#define Error11_earlyinit     0x11  // Early environment setup failure
// [1:2:0] mmInitialize(0)
#define Error120_mmPhase0     0x120  // Memory manager phase 0 failure (heap/stack/memory sizing)
// [1:2:1] mmInitialize(1)
#define Error121_mmPhase1     0x121  // Memory manager phase 1 failure (frame pools/paging)
// [1:3:0] keInitialize(0)
#define Error130_kePhase0     0x130  // Executive phase 0 failure (font/background/display/linker data)
// [1:3:1] keInitialize(1)
#define Error131_kePhase1     0x131  // Executive phase 1 failure (scheduler/dispatcher/process/thread setup)
// [1:3:2] keInitialize(2)
#define Error132_kePhase2     0x132  // Executive phase 2 failure (graphics engine/GUI structures/callbacks)
// [1:4] archinit()
#define Error14_archinit      0x14  // Architecture initialization failure (processor probe/SMP/APIC)
// [1:5] deviceinit()
#define Error15_deviceinit    0x15  // Device initialization failure (PS/2/input targets/consoles)
// [1:6] lateinit()
#define Error16_lateinit      0x16  // Late initialization failure (PTYs/user subsystem/network/modules/init process)

//
// 2
//

#define Error2_I_x64CreateKernelProcess     0x2

//
// 3
//

#define Error3_I_x64CreateInitialProcess    0x3


//
// 4
//

#define Error4_ke_x64ExecuteInitialProcess  0x4

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

    // ...
}; 

// Externam reference.
// see: init.c
extern struct initialization_d  Initialization;


// ======================
// #test
// Queue feeder (i/o channel)
// I/O channels are specialized, independent processors that 
// manage data transfer between peripheral devices and main memory, 
// largely reducing CPU workload
/*
struct dpc_queue_d 
{
    int on;

    int head;  // >> Get
    int tail;  // << Put

// 1000: Raw keyboard event
// 2000: Mouse event
// ...
    //int destination[32];
    int msg[32];

// Registers 1, 2, 3, 4
    unsigned long long1[32];
    unsigned long long2[32];
    unsigned long long3[32];
    unsigned long long4[32];

// Registers 5, 6, 7, 8
    unsigned char char1[32];
    unsigned char char2[32];
    unsigned char char3[32];
    unsigned char char4[32];
};
*/
//extern struct dpc_queue_d DPC_QUEUE;

// Current message
// Only the loop reads this message
//extern int qf_buf_msg[1];
//extern unsigned long qf_buf_longs[4];
//extern unsigned char qf_buf_chars[4];
// ========================


void
qf_post_message(
    int msg,
    unsigned long long1,
    unsigned long long2,
    unsigned long long3,
    unsigned long long4,
    unsigned char char1,
    unsigned char char2,
    unsigned char char3,
    unsigned char char4 );

int qf_get_message(void);


// #test: For dialog with APs
void welcome_ap_hlt(void);
void welcome_ap_pause(void);


// ::(1)
// The kernel starts at ...
// see: '_kernel_entry_point' in head_64.asm.

// ::(2)
// First function called by the BSP processor.
void I_kmain(int arch_type);

void start_kernel(int arch_type);
void x86_64_start_kernel(int arch_type);


#endif    


//
// End.
//

