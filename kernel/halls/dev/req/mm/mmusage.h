
// mm/mmusage.h
// Memory usage.
// Created by Fred Nora.

#ifndef __MM_MMUSAGE_H
#define __MM_MMUSAGE_H    1

//========================================================
// Used memory:
// Here we're tracking the usage of phusical memory.
// These are some canonical regions created in the kernel initialization.
// Remember, the same region in the physical memory can be mapped 
// more than one.
// #todo
// We need to check every single physical address here and check 
// if there is some sobreposition.
// see: mminit.c

extern unsigned long mm_used_ring0_area;  // start = 0 size = 4MB
extern unsigned long mm_used_ring3_area;  // start = 0x400000 size = 4MB
extern unsigned long mm_used_kernelimage;
extern unsigned long mm_used_backbuffer;  // start = 0x800000 size = 4MB
extern unsigned long mm_used_pagedpool;   // start = 0xC00000 size = 4MB  
extern unsigned long mm_used_heappool;    // start = 0x01000000 size = 4MB   
extern unsigned long mm_used_extraheap1;  // start = (0x01000000 + 0x400000) size = 4MB
extern unsigned long mm_used_extraheap2;  // start = (0x01000000 + 0x800000) size = 4MB
extern unsigned long mm_used_extraheap3;  // start = (0x01000000 + 0xC00000) size = 4MB
extern unsigned long mm_used_frame_table;
// start = ?? size = 2MB
extern unsigned long mm_used_lfb; 

#endif   

