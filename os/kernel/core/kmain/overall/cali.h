// cali.h
// Project California.
// Top level wrappers for some base kernel components.
// Created by Fred Nora.

#ifndef __CALI_CALI_H
#define __CALI_CALI_H    1


extern unsigned int g_processor_count;
extern int g_smp_initialized;

//
// == current ======================================================
//

// GRAMADO_JAIL, GRAMADO_P1 ...
extern int current_mode;


// x86 ...
extern int current_arch;
// Organização de usuários.
extern int current_user; 
extern int current_group; 


// Organização dos processos

// ------
// see: process.c
// Process used by the console during the job control.
// #hackhack: 
// For now we are using the control thread associated with the
// window with focus.
// #todo
// But we need to use the control thread of the foreground process
// associated with the console TTY.
extern pid_t foreground_process;

// [Processing time]
// Only these can read the keyboard input.
// Sometime it's the terminal.
// It's child will read into a file.
// See: ps/process.c
extern pid_t criticalsection_pid;
// ------


// ------
// See: ps/thread.c
extern tid_t current_thread;     // Currently having the processing time.
extern tid_t foreground_thread;  // Input thread.
// ------

// Current runlevel. Used in init process.
extern int current_runlevel;

// Organização dos discos.
extern int current_disk;
extern int current_volume;

// Organização dos arquivos.
extern int current_directory;
extern int current_file;

// Organização dos terminais
extern int current_tty;
extern int current_terminal;
//extern int current_pty;
//extern int current_pts;

//se ele esta inicializado ou nao
// see: kmain.c
extern int dead_thread_collector_status;
// Se e' para usalo ou nao
// see: kmain.c
extern int dead_thread_collector_flag;


// Logon
extern int g_logged;

// Drivers support
// Internal modules support.
// see: kmain.c
extern int g_driver_ps2keyboard_initialized;   //ps2 keyboard
extern int g_driver_ps2mouse_initialized;      //ps2 mouse
extern int g_driver_video_initialized;
extern int g_driver_apic_initialized;
extern int g_driver_pci_initialized;
extern int g_driver_rtc_initialized;
extern int g_driver_timer_initialized;
//...


// Internal modules support.
// see: kmain.c
extern int g_module_shell_initialized;
extern int g_module_debug_initialized;
extern int g_module_disk_initialized;
extern int g_module_volume_initialized;
extern int g_module_fs_initialized;
extern int g_module_gui_initialized;
extern int g_module_logoff_initialized;
extern int g_module_logon_initialized;
extern int g_module_mm_initialized;
extern int g_module_objectmanager_initialized;
extern int g_module_runtime_initialized;
extern int g_module_uem_initialized;             //user environment manager.
//...


// Kernel information variables.
extern unsigned long KernelSize;
extern unsigned long KernelHeapSize;
extern unsigned long KernelStackSize;
extern unsigned long KernelPages;
//...

// symbol table
extern int g_kernel_symbols_available;


//#bugbug
//move this to kernel (ring 0) crt support.
//error support.
extern unsigned long errno;




// --------------------------

struct node_d
{
    void *data;
    struct node_d *flink;
    struct node_d *blink;    
};

// Linked list support.
struct linkedlist_d
{
    struct node_d *head;
    struct node_d *tail;
};

// Save args support. 
struct kernel_args_d
{
    unsigned long arg1;
    unsigned long arg2;
    unsigned long arg3;
    unsigned long arg4;
}; 
struct kernel_args_d  KernelArgs;


//system classes.
struct system_classes_d
{
    //unsigned long Gramado; //@todo: Include this one.
    unsigned long Executive;
    unsigned long Microkernel;
    unsigned long Hal;
};
struct system_classes_d  SystemClasses;


//Kernel classes.
struct kernel_classes_d
{
    struct system_classes_d *System;
    //..
};
struct kernel_classes_d  KernelClasses;
//...




//
// == prototypes ==========================================
//


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


// x86_64 Initialization. (Main routine)
// see: x64init.c
int I_x64_initialize(void);

// ::(3)
// see: x64init.c
void I_x64ExecuteInitialProcess(void);

// ===========================

// Finalizations support.
void panic( const char *format, ... );
void soft_die(void);
void die(void);

#define PANIC  panic
#define DIE    die 

// ------------------------


int 
cali_post( 
    tid_t sender_tid,
    tid_t receiver_tid,
    struct msg_d *message );

void cali_reboot(void);
void cali_shutdown(int how);
void cali_die(void);
void cali_spawn_thread_by_tid(tid_t tid);


//--------------------
unsigned long cali_get_system_metrics(int index);
int cali_start_thread(struct thread_d *thread);
int cali_get_current_runlevel(void);
unsigned long cali_get_memory_size_mb(void);
void *cali_alloc_shared_ring3_pages(pid_t pid, int number_of_bytes);
void cali_set_cursor( unsigned long x, unsigned long y );

#endif   

