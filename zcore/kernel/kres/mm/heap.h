
// heap.h
// Created by Fred Nora.

#ifndef  __HEAP_H
#define  __HEAP_H    1

// Kernel Heap support.
// See: mm.c
extern unsigned long heapCount;          // Conta os heaps do sistema
extern unsigned long kernel_heap_start;  // Start
extern unsigned long kernel_heap_end;    // End
extern unsigned long g_heap_pointer;     // Pointer
extern unsigned long g_available_heap;   // Available


/*
 * heap_d:
 *     Estrutura para heap.
 *     Cada processo tem seu heap.
 *     Cada heap tem uma lista encadeada de blocos.  
 */  

struct heap_d 
{
    object_type_t objectType;
    object_class_t objectClass;
    int id;
    int used;
    int magic;
    unsigned long HeapStart;  
    unsigned long HeapEnd;
    unsigned long HeapPointer; 
    unsigned long AvailableHeap; 

// Ponteiro para a lista de blocos de um heap.
// Lista encadeada de blocos que formam o heap.
// A estrutura para um bloco é: mmblock_d e está definida em mm.h
// Obs: 
// Foram alocados vários blocos de memória dentro de um heap. Portanto 
// podemos colocar os ponteiros para as estruturas desses blocos dentro 
// de uma lista encadeada e o ponteiro para a lista colocaremos aqui.
// Cada bloco desse começa com um 'header', 
// definido na estrutura de bloco.
// See: x86mm.h

    struct mmblock_d  *mmblockListHead;  

// Se esses arrays ocuparem muito espaço, 
// então faremos com listas encadeads.
	//struct mmblock_d *mmblockUsedBlocks;  
	//struct mmblock_d *mmblockFreeBlocks; 
	//struct mmblock_d *mmblockAllBlocks;  

//#test:
// Endereços para os blocos de um heap.
// Uma lista de blocos que estão sendo usados
// Uma lista de blocos livres e já alocados.
// Uma lista com todos os blocos.
	//unsigned long usedBlocks[32];
	//unsigned long freeBlocks[32];
	//unsigned long Blocks[64];

// Um heap pertence à um processo.
    struct process_d *process;
// Um heap pode pertencer à um thread.
    struct thread_d *thread;

// The heap belongs to a cgroup;
    struct cgroup_d *cgroup;

// Compartilhamento de heap:
// ========================
// + As threads de um mesmo processo podem compartilhar o mesmo heap
//   pois estão na mesma área de memória.
// + #todo: 
//   O desafio é fazer os processes que estão no mesmo cgroup
//   compartilharem o mesmo heap, porque eles deveriam estar
//   na mesma área de memória para isso. 

//
// Podemos criar aqui flags de porteção.
//
    //int read;  //Apenas leitura.
    //int write;

    //...

// navigation
    //struct heap_d *next;
};

#define HEAP_COUNT_MAX  8

// see: mm.c
extern unsigned long heapList[HEAP_COUNT_MAX];  

//
// Prototypes ==========================
//

// Allocate and free memory.
// These are the workers for kmalloc and kfree.
unsigned long heapAllocateMemory(unsigned long size);
void heapFreeMemory(void *ptr);

struct heap_d *memory_create_new_heap ( 
    unsigned long start_va, 
    unsigned long size );
void memory_destroy_heap (struct heap_d *heap);

unsigned long get_process_heap_pointer(pid_t pid);

#endif    



















