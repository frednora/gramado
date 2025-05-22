// globals.h
// Global definitions. See globals.c
// Created by Fred Nora.

#ifndef __KMAIN_GLOBALS_H
#define __KMAIN_GLOBALS_H    1

// Are we using the GUI or not?
#define GUI_ON   1
#define GUI_OFF  0

// #deprecated
// We still need to delete this when possible.
// Layers
#define KERNEL      0
#define GRAMADO     1   //1 kgws
#define EXECUTIVE   2   //2
#define MICROKERNEL 3   //3
#define HAL         4   //4

// IOPL constants.
// Intel/AMD
#define KernelMode  0
#define UserMode   3
#define RING0  0
#define RING1  1
#define RING2  2
#define RING3  3


//#define LOBYTE(w) ((char)(((unsigned long )(w)) & 0xff))
//#define HIBYTE(w) ((char)((((unsigned long)(w)) >> 8) & 0xff))


// ===================================================

// Keyboard suppport 
extern int abnt2;
//...


// Regions
// see: globals.c
extern unsigned long g_ring0area_va;
extern unsigned long g_ring3area_va;
extern unsigned long g_kernelimage_va;
extern unsigned long g_frontbuffer_va;   
extern unsigned long g_backbuffer_va;
extern unsigned long g_pagedpool_va;  //pagedpool virtual address
extern unsigned long g_heappool_va;
extern unsigned long g_extraheap1_va;
extern unsigned long g_extraheap2_va;
extern unsigned long g_extraheap3_va;

// frontbuffer and backbuffer.
// see: video.c
extern unsigned long g_frontbuffer_pa; 
extern unsigned long g_backbuffer_pa;

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


// The kernel has booted?
// see: globals.c
extern int has_booted;

#endif   

//
// End
//

