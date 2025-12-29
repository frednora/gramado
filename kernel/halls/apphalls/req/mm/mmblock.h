// mm/mmblock.h
// Blocks
// Created by Fred Nora.

#ifndef __MM_MMBLOCK_H
#define __MM_MMBLOCK_H    1

// Number of indexes into the list.
// see: mmblockList[i]
#define MMBLOCK_COUNT_MAX  (2*4096)


/*
 * mmblock_d:
 * Represents a single dynamically allocated memory block within the kernel's heap.
 * 
 * Each block tracks its metadata, allocation status, and linkage for heap management.
 * This structure is strictly used for kernel heap allocations (not physical memory).
 * 
 * -------------------------------------------------------------------------
 * Structure layout overview:
 *   [Header][User Area][Footer]
 * 
 * - Header:    Metadata and management info.
 * - User Area: The usable, allocated memory for the kernel.
 * - Footer:    Marks the end of the block, useful for integrity checks.
 * -------------------------------------------------------------------------
 * 
 * NOTE: Do not change the layout or size of this structure. It must remain
 *       fixed for compatibility with the memory allocator.
 * 
 * See also: aspace_d for physical memory block management.
 */
// This is the structure to handle each allocation into the kernel heap.
// The structure has three major components:
// + The 'header base'. 
//     This is the base of the struture.
// + The 'user area base'.
//    This is the start of the allocated area.
// + The 'footer address'.
//    This is the end of the allocated area.
struct mmblock_d 
{

//
// Header
//

// Address of the start of this structure in memory.
    unsigned long Header;
// Size of the header (this structure) in bytes.
    unsigned long headerSize;

//
// User area
//

// Address of the start of the usable (allocated) area for the user/kernel.
    unsigned long userArea;
// Size of the user area (the memory given to the allocator/requester).
    unsigned long userareaSize;

//
// Footer
//

// Address marking the end of the user area (footer).
    unsigned long Footer;

// ----------------

// Unique identifier for this memory block (for debugging/tracking).
    int Id;

// Structure validation
// Magic number for structure validation and integrity checking.
    int Used; 
    int Magic;

// This block is free or not.
// Free flag: 1 if this memory block is free (available), 0 if allocated.
    int Free;

// PID of the process owning this block (if applicable)
    pid_t pid;

// TID of the thread owning this block (if applicable).
    tid_t tid;

// Navigation
// Pointer to the next memory block in the heap's linked list.
    struct mmblock_d  *Next;
};
extern struct mmblock_d  *current_mmblock;

// Lista de blocos. 
// Lista de blocos de memória dentro de um heap.
// #todo: Na verdade temos que usar lista encadeada. 
// see: mm.c
extern unsigned long mmblockList[MMBLOCK_COUNT_MAX];  



#endif    


