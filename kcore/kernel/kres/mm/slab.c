
// slab.c
// O pool tem dois blocos de 2MB cada.
// O alocador vai alocar blocos de 1MB, 
// se falhar, retornará 0, permitindo que quem chamou o alocar
// possa chamar outro alocados de páginas.
// Quem usara esse alocador é a rotina de clonagem de processos 
// e dara 1MB para cada um dos 4 primeiros clones. Sortudos.
// Isso é provisorio ... so ate melhorarmos os alocadores.

#include <kernel.h>

struct newpagedpool_d
{
// Para gerenciar o slab allocator.

    int initialized;

    unsigned long a1_va;
    unsigned long a2_va;
    unsigned long b1_va;
    unsigned long b2_va;

// Status
// TRUE = FREE | FALSE = NOT FREE
    int Free[4];
};
// Local structure.
struct newpagedpool_d  NewPagedPool;


// --------------------------------------------------------------

// #todo:
// Explain it better.
// This code manages the extraheaps 2 and 3,
// creating buffers that are gonna be used by a slab allocator
// called slab_1MB_allocator().
void slab_initialize(void)
{
    NewPagedPool.initialized = FALSE;

// We need these two addresses.
// The virtual addresses for 
// extraheap 2 and extraheap3.

    if ( g_extraheap2_va == 0 || 
         g_extraheap3_va == 0 )
    {
        panic("slab_initialize: address\n");
    }

//-------------------
// Two reagions of 1MB each in the extraheap2.
// a1
    NewPagedPool.a1_va = 
        (unsigned long) g_extraheap2_va;
    NewPagedPool.Free[0] = TRUE;
// a2
    NewPagedPool.a2_va = 
        (unsigned long) (g_extraheap2_va + (1024*1024) );
    NewPagedPool.Free[1] = TRUE;

//-------------------
// Two reagions of 1MB each in the extraheap3.
// b1
    NewPagedPool.b1_va = 
        (unsigned long) g_extraheap3_va;
    NewPagedPool.Free[2] = TRUE;
// b2
    NewPagedPool.b2_va = 
        (unsigned long) (g_extraheap3_va + (1024*1024) );
    NewPagedPool.Free[3] = TRUE;
//----

// Now the slab allocator can use these addresses.
    NewPagedPool.initialized = TRUE;
}

// global
// Used by alloc_memory_for_image_and_stack() in process.c
void *slab_1MB_allocator(void)
{

// não inicializado
    if (NewPagedPool.initialized != TRUE)
        return NULL;


/*
// Se devemos ou não incremetar o contador de uso.
    int IncrementUsageCounter=TRUE; //P->allocated_memory
    struct process_d *process;
    process = (void*) get_current_process_pointer();
    if( (void*) process == NULL )
        IncrementUsageCounter=FALSE;
    if(process->magic!=1234)
        IncrementUsageCounter=FALSE;
*/

// #test
// At this moment we're gonna know what is the
// process that is calling the allocator.
// Lets see if its a valid process.
    struct process_d *process;
    process = (void*) get_current_process_pointer();
    if ((void*) process == NULL)
        panic ("slab_1MB_allocator: process\n");
    if (process->magic != 1234)
        panic ("slab_1MB_allocator: process validation\n");

//
// Procure um livre entre os 4 de 1MB cada.
//

// --------
// a1 - Get address a1 if it is available.
    if (NewPagedPool.Free[0] == TRUE)
    {
        NewPagedPool.Free[0] = FALSE;  // NOT FREE
        if ((void*) process != NULL)
            process->allocated_memory += (1*1024*1024);
        return (void *) NewPagedPool.a1_va; 
    }

// --------
// a2 - Get address a2 if it is available.
    if (NewPagedPool.Free[1] == TRUE)
    {
        NewPagedPool.Free[1] = FALSE;  // NOT FREE
        if ((void*) process != NULL)
            process->allocated_memory += (1*1024*1024);
        return (void *) NewPagedPool.a2_va; 
    }

// --------
// b1 - Get address b1 if it is available.
    if (NewPagedPool.Free[2] == TRUE)
    {
        NewPagedPool.Free[2] = FALSE;  // NOT FREE
        if ((void*) process != NULL)
            process->allocated_memory += (1*1024*1024);
        return (void *) NewPagedPool.b1_va; 
    }

// --------
// b2 - Get address b2 if it is available.
    if (NewPagedPool.Free[3] == TRUE)
    {
        NewPagedPool.Free[3] = FALSE;  // NOT FREE
        if ((void*) process != NULL)
            process->allocated_memory += (1*1024*1024);
        return (void *) NewPagedPool.b2_va; 
    }

// fail
    return NULL;
}






