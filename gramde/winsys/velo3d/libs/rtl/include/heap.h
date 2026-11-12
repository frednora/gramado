/*
 * File: heap.h
 * Descrição:
 *     O propósito desse header é dar suporte ao gerenciamento de
 * do heap que será usado para a alocação de memória para um programa
 * em user mode. 
 *     Um suporte oferecido pela biblioteca libC99.
 *     Obs: O kernel poderá usar tanto o heap do processo quanto o heap 
 * do desktop ao qual o processo pertence.
 * History:
 *     2016 - Created by Fred Nora.
 */

#ifndef __HEAP_H
#define __HEAP_H  1

// Contagem de heap.
#define HEAP_COUNT_MAX  256 

// see: stdlib.c
extern unsigned long HEAP_START; 
extern unsigned long HEAP_END;
extern unsigned long HEAP_SIZE;
 
// see: stdlib.c
extern unsigned long heapCount;

// see: stdlib.c
extern unsigned long heap_start;        // Start
extern unsigned long heap_end;          // End
extern unsigned long g_heap_pointer;    // Pointer
extern unsigned long g_available_heap;  // Available 

// see: stdlib.c
void *Heap;

//
// HEAP
//

// 32KB, provisório.
#define HEAP_BUFFER_SIZE  (32*1024)


/*
 * heap_d:
 *     Estrutura para heap.
 *     Cada processo tem seu heap.
 *     Cada heap tem uma lista encadeada de blocos.  
 */
struct heap_d 
{
    int Id;    
    int Used;
    int Magic;
    unsigned long HeapStart;
    unsigned long HeapEnd;
    unsigned long HeapPointer;
    unsigned long AvailableHeap; 
    //lista linkada de blocos. 
    //struct mmblock_d *nextblock; 
};

// see: stdlib.c
extern unsigned long heapList[HEAP_COUNT_MAX];


//
// == prototypes ===============================================
//


void heapSetLibcHeap(unsigned long HeapStart, unsigned long HeapSize);
unsigned long heapAllocateMemory(unsigned long size);
unsigned long FreeHeap(unsigned long size);

#endif    

//
// End
//

