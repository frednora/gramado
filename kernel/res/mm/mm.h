// mm.h
// Main header for the mm/ kernel module.
// Memory manager
// Created by Fred Nora.

#ifndef __MM_MM_H
#define __MM_MM_H    1

// Aligns a value on a boundary.
//#define ALIGN(x, a)    (((x) + ((a) - 1)) & ~((a) - 1))


// Indexes
// Canonical kernel pagetables. (Physical addresses)
typedef enum {
    MM_COMPONENT_SYSTEM_ORIGIN_PA,
    MM_COMPONENT_KERNEL_BASE_PA,  // kernel image starts here.
    MM_COMPONENT_USER_BASE_PA,
    MM_COMPONENT_FRONTBUFFER_PA,
    MM_COMPONENT_BACKBUFFER_PA,
    MM_COMPONENT_HEAPPOOL_PA,
    MM_COMPONENT_EXTRAHEAP1_PA,
    MM_COMPONENT_EXTRAHEAP2_PA,
    MM_COMPONENT_EXTRAHEAP3_PA,
    MM_COMPONENT_PAGEDPOOL_PA     // We have 4 pagedpool areas. See: x86gpa.h

}mm_component_physical_d;

extern unsigned long paList[32];

// Indexes
// Canonical kernel pagetables. (Virtual addresses)
typedef enum {
    MM_COMPONENT_SYSTEM_ORIGIN_VA,
    MM_COMPONENT_KERNEL_BASE_VA,  // kernel image starts here.
    MM_COMPONENT_USER_BASE_VA,
    MM_COMPONENT_FRONTBUFFER_VA,
    MM_COMPONENT_BACKBUFFER_VA,
    MM_COMPONENT_PAGEDPOOL_VA,
    MM_COMPONENT_HEAPPOOL_VA,
    MM_COMPONENT_EXTRAHEAP1_VA,
    MM_COMPONENT_EXTRAHEAP2_VA,
    MM_COMPONENT_EXTRAHEAP3_VA,

    // #test
    // All the user processes have the same virtual address.
    MM_COMPONENT_USERPROCESS_BASE_VA,
    MM_COMPONENT_USERPROCESS_ENTRYPOINT_VA,
    MM_COMPONENT_USERPROCESS_STACK_VA

}mm_component_virtual_d;

extern unsigned long vaList[32];

// =========================

int mm_gc(void);

int mmInitialize(int phase);

#endif    


