// globals.h
// Global definitions. See globals.c
// Created by Fred Nora.

#ifndef __KWRAP_GLOBALS_H
#define __KWRAP_GLOBALS_H    1


// IOPL constants.
// Intel/AMD
#define KernelMode  0
#define UserMode   3
#define RING0  0
#define RING1  1
#define RING2  2
#define RING3  3


//#define LOBYTE(w)  ((char)(((unsigned long )(w)) & 0xff))
//#define HIBYTE(w)  ((char)((((unsigned long)(w)) >> 8) & 0xff))


// See:
// globals.c
// admin/product.h
extern int g_product_type;

// See: kernel.h
extern int system_state;

// The kernel has booted?
// see: globals.c
extern int has_booted;

// x86 ...
extern int current_arch;

// GRAMADO_JAIL, GRAMADO_P1 ...
extern int current_mode;


// ===================================================

// Current runlevel. Used in init process.
extern int runlevel;

// User support
extern int current_user; 
extern int current_group; 

// Disk and volume
extern int current_disk;
extern int current_volume;

// Directories and files
extern int current_directory;
extern int current_file;

// TTYs
extern int current_tty;
extern int current_terminal;
//extern int current_pty;
//extern int current_pts;


// Keyboard suppport 
extern int abnt2;
//...

extern unsigned int g_processor_count;
extern int g_smp_initialized;

// Logon
extern int g_logged;


extern int g_redirect_printk_to_serial_during_syscalls;


// symbol table
extern int g_kernel_symbols_available;

// Salvando o último endereço válido de memória ram.
// usado em head.asm
// see: globals.c
extern unsigned long blSavedLastValidAddress;
extern unsigned long blSavedPhysicalMemoryInKB;

// Kernel information variables.
extern unsigned long KernelSize;
extern unsigned long KernelHeapSize;
extern unsigned long KernelStackSize;
extern unsigned long KernelPages;
// ...

// Regions
// see: globals.c
extern unsigned long g_ring0area_va;
extern unsigned long g_ring3area_va;
extern unsigned long g_kernelimage_va;
extern unsigned long g_pagedpool_va;  //pagedpool virtual address
extern unsigned long g_heappool_va;
extern unsigned long g_extraheap1_va;
extern unsigned long g_extraheap2_va;
extern unsigned long g_extraheap3_va;

//
// Heap support
//

// see: globals.c
extern int g_heap_count;
extern int g_heap_count_max;
extern unsigned long g_heap_size;

// extra heap 1
extern unsigned long g_extraheap1_size;
extern int g_extraheap1_initialized;
// extra heap 2
extern unsigned long g_extraheap2_size;
extern int g_extraheap2_initialized;
// extra heap 3
extern unsigned long g_extraheap3_size;
extern int g_extraheap3_initialized;

//#bugbug
//move this to kernel (ring 0) crt support.
//error support.
extern unsigned long errno;

#endif   

//
// End
//

