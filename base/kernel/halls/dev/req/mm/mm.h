// mm.h
// Main header for the mm/ kernel module.
// Memory manager
// Created by Fred Nora.

#ifndef __MM_MM_H
#define __MM_MM_H    1


//
//  PAGEABLE AREA
//

extern unsigned long gPagedPollStart;
extern unsigned long gPagedPollEnd;

// Número máximo de índices de framepool que 
// serão usados nessa área de alocação de frames.
// Uma certa quantidade de framepools serão usados
// para alocação de frames para os processos. 
// Durante a alocação sobre demanda, os frames usados 
// virão dessa área de memória.
extern int g_pageable_framepool_index_max;


// frame pool atual.
extern int g_current_framepool;
// O indice do framepool da user space para qualquer tamanho de memória.
extern int g_user_space_framepool_index;
// O máximo de framepools possíveis dado o tamanho da memória física.
extern unsigned long g_framepool_max;


// Lista de framepools livres.
// int framepoolFreeList[FRAMEPOOL_COUNT_MAX]
// frame_pool_t *framepoolFreeListHead;

// Contabilidade. kernel(talvez deva fazer uma estrututra) MS.
// Memória usada pelo kernel.
// +Paginada. paged
// +não paginada. nonpaged
// +fisica paginada
// +virtual paginada 
// +paginada limite
// +não pagianda limite.

extern unsigned long g_kernel_paged_memory;
extern unsigned long g_kernel_nonpaged_memory;




struct mm_info_d
{
    int initialized;
    //unsigned long countstuff;
    // ...
};
extern struct mm_info_d MMInfo;


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

// Pointer for a set of 2mb usermode shared buffers for coavases.
// #todo: We can expand this
//extern unsigned long shbufferList[2];

// =========================

void mmShowPML4Entry(int index, unsigned long pml4_va);
void mmShowMemoryInfo(void);
void mmShowPML4EntryForAllProcesses (int entry_number);
void mmShowPagedMemoryList(int max);
void mmShowMemoryBlocksForTheKernelAllocator(void);
void mmTestingPageAlloc(void);


int mm_gc(void);


void *mm_kmalloc(size_t size);
void *mm_kcalloc(size_t count, size_t size);
void mm_kfree(void *ptr);

int mmInitialize(int phase);

#endif    


