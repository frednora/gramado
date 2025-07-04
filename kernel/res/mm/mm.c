// mm.c
// + Initialize the memory support.
//   The kernel heap and the kernel stack.
// + The implementation of the main kernel allocator.
// + Initialize the physical memory manager.
// + Initialize the paging support.
// 2015 - Created by Fred Nora.

#include <kernel.h>

// List of main physical addresses.
unsigned long paList[32];
// List of main virtual addresses.
unsigned long vaList[32];

// --------------------------------
// Kernel Heap support.
unsigned long heapCount=0;          // Conta os heaps do sistema
unsigned long kernel_heap_start=0;  // Start
unsigned long kernel_heap_end=0;    // End
unsigned long g_heap_pointer=0;     // Pointer
unsigned long g_available_heap=0;   // Available

unsigned long heapList[HEAP_COUNT_MAX];  

// --------------------------------
unsigned long kernel_stack_end=0;       //va
unsigned long kernel_stack_start=0;     //va
unsigned long kernel_stack_start_pa=0;  //pa (endereço indicado na TSS).


// For debug purpose.
struct mmblock_d  *current_mmblock;


// # Not used yet.
unsigned long gPagedPollStart=0;
unsigned long gPagedPollEnd=0;


// ??
// Número máximo de índices de framepool que 
// serão usados nessa área de alocação de frames.
// Uma certa quantidade de framepools serão usados
// para alocação de frames para os processos. 
// Durante a alocação sobre demanda, os frames usados 
// virão dessa área de memória.
int g_pageable_framepool_index_max=0;


// frame pool atual.
int g_current_framepool=0;
// O indice do framepool da user space para qualquer tamanho de memória.
int g_user_space_framepool_index=0;
// O máximo de framepools possíveis dado o tamanho da memória física.
unsigned long g_framepool_max=0;


unsigned long g_kernel_paged_memory=0;
unsigned long g_kernel_nonpaged_memory=0;


// --------------------------------
struct kernel_heap_d 
{
    int initialized;
// va
    unsigned long start;
    unsigned long end;
};
// Global.
struct kernel_heap_d KernelHeap;
// --------------------------------

// --------------------------------
struct kernel_stack_d 
{
    int initialized;
// va
    unsigned long start;
    unsigned long end;
};
// Global.
struct kernel_stack_d KernelStack;
// --------------------------------

// --------------------------------
/*
 * mmblockCount:
 *     mm block support.
 *     Conta os blocos de memória dentro de um heap.
 *     dentro do heap usado pelo kernel eu acho ?? 
 */
static int mmblockCount=0;

unsigned long mmblockList[MMBLOCK_COUNT_MAX];  

// Endereço da última estrutura alocada.
static unsigned long mm_prev_pointer=0;

// ----------------------------

static int __init_kernel_heap(void);
static int __init_kernel_stack(void);

// ----------------------------


// mm_gc:
// Garbage collector.
// #todo:
// Let's clean up some memory let's reuse some freed structures.
int mm_gc(void)
{
    panic ("mm_gc: Unimplemented\n");
    return (int) -1;
}


/*
unsigned long slab_2mb_extraheap2(void)
{
    if(g_extraheap2_initialized != TRUE)
        panic("slab_2mb_extraheap2: not initialized\n");

    return (unsigned long) g_extraheap2_va;
}
*/

/*
unsigned long slab_2mb_extraheap3(void)
{
    if(g_extraheap3_initialized != TRUE)
        panic("slab_2mb_extraheap3: not initialized\n");

    return (unsigned long) g_extraheap3_va;
}
*/


// __init_kernel_heap:
// Initializes the kernel heap management.
// This routine builds a handed mande heap that is gonna be used 
// by the kernel. (ps: also kernel process)
// + It's called once.
// + Needs to be called before any other kernel operation that uses the heap.
// #todo:
// We need a routine for automation in the creation of new heaps.
// #ps: Maybe we already have one.
// OUT: 0=OK.

static int __init_kernel_heap(void)
{
    register int i=0;

    // #bugbug
    // não usar printk
    // printk ainda não funciona nesse momento.

    KernelHeap.initialized = FALSE;

//
// Globals
//

// #warning
// We will not clear this area at this moment.
// We need the full paging initialization 
// to play with the memory.

// start and end.
    kernel_heap_start = (unsigned long) KERNEL_HEAP_START;
    kernel_heap_end   = (unsigned long) KERNEL_HEAP_END;

    KernelHeap.start = (unsigned long) kernel_heap_start;
    KernelHeap.end   = (unsigned long) kernel_heap_end;

// Heap Pointer, Available heap and Counter.
    g_heap_pointer   = (unsigned long) kernel_heap_start; 
    g_available_heap = (unsigned long) (kernel_heap_end - kernel_heap_start);  
    heapCount = 0; 

// Check Heap Pointer.
    if (g_heap_pointer == 0){
        debug_print("__init_kernel_heap: g_heap_pointer\n");
        goto fail;
    }

// Check Heap Pointer overflow.
    if (g_heap_pointer > kernel_heap_end){
        debug_print("__init_kernel_heap: Heap Pointer Overflow\n");
        goto fail;
    }

// Heap Start
    if (kernel_heap_start == 0){
        debug_print("__init_kernel_heap: HeapStart\n");
        goto fail;
    }

// Heap End
    if (kernel_heap_end == 0){
        debug_print("__init_kernel_heap: HeapEnd\n");
        goto fail;
    }

// Check available heap.
// #todo: Tentar crescer o heap.
    if (g_available_heap == 0){
        debug_print("__init_kernel_heap: g_available_heap\n");
        goto fail;
    }

// Heap list: 
// Inicializa a lista de heaps.

    while (i < HEAP_COUNT_MAX){
        heapList[i] = (unsigned long) 0;
        i++;
    };

    //KernelHeap = (void*) x??;

    //More?!

    KernelHeap.initialized = TRUE;

// OUT: 0=OK.
    return 0;


// Falha ao iniciar o heap do kernel.
// ====================================
fail:
    KernelHeap.initialized = FALSE;
    debug_print("__init_kernel_heap: Fail\n");
    //refresh_screen();
    return (int) -1;
}

// __init_kernel_stack:
// Initializes the kernel stack management.
// + It's called once.
// + Needs to be called before any other kernel operation that uses the stack.
// OUT: 0=OK.
static int __init_kernel_stack(void)
{
// Globals

// #warning
// We will not clear this area at this moment.
// We need the full paging initialization 
// to play with the memory.

    KernelStack.initialized = FALSE;
    kernel_stack_end   = (unsigned long) KERNEL_STACK_END; 
    kernel_stack_start = (unsigned long) KERNEL_STACK_START; 
    KernelStack.end   = (unsigned long) kernel_stack_end;
    KernelStack.start = (unsigned long) kernel_stack_start;

// End
    if (kernel_stack_end == 0){
        debug_print("__init_kernel_stack: kernel_stack_end\n");
        goto fail;
    }
// Start
    if (kernel_stack_start == 0){
        debug_print("__init_kernel_stack: kernel_stack_start\n");
        goto fail;
    }

    KernelStack.initialized = TRUE;

// OUT: 0=OK.
    return 0;

fail:
    KernelStack.initialized = FALSE;
    return (int) -1;
}

struct heap_d *memory_create_new_heap ( 
    unsigned long start_va, 
    unsigned long size )
{
    panic ("memory_create_new_heap: Unimplemented\n");
    return NULL;
}

void memory_destroy_heap (struct heap_d *heap)
{
    panic ("memory_destroy_heap: Unimplemented\n");
}


/*
 * heapAllocateMemory:
 *   Allocates a block of memory from the kernel heap, tracked with an mmblock_d header.
 *
 *   The allocated block consists of:
 *     [mmblock_d header][User Area][Footer]
 *
 *   - The header stores metadata (address, size, usage, etc.).
 *   - The user area is returned to the caller.
 *   - The footer marks the end of the allocation.
 *
 *   The function ensures the heap pointer and allocation are valid, 
 *   updates all housekeeping fields, and maintains the list of allocations.
 *
 * IN:
 *   size   Size in bytes to allocate.
 * OUT:
 *   Returns the address of the allocated user area if successful, or 0 on failure.
 */
unsigned long heapAllocateMemory(unsigned long size)
{
// This is a worker for kmalloc() and __kmalloc_impl() in kstdlib.c
// Allocate memory inside the kernel heap.

    struct mmblock_d *Current;

// Calculate sizes for header and user area.

// Header
    unsigned long HeaderBase = 0;
    unsigned long HeaderInBytes = (unsigned long) ( sizeof(struct mmblock_d) ); 

// User area
    unsigned long UserAreaBase = 0;
    unsigned long UserAreaInBytes = (unsigned long) size;

// Do not allow zero-sized allocations. Enforce a minimum.
// We can't use zero.
    if (UserAreaInBytes == 0){
        UserAreaInBytes = (unsigned long) 8;
    }

// Fail if the heap is exhausted.
    if (g_available_heap == 0){
        debug_print ("heapAllocateMemory: g_available_heap={0}\n");
        printk      ("heapAllocateMemory: g_available_heap={0}\n");
        goto fail;
    }

// #bugbug
// And if the available heap is an invalid big number?

// Fail if not enough heap space for this allocation.
    if (UserAreaInBytes >= g_available_heap)
    {
        debug_print ("heapAllocateMemory error: UserAreaInBytes >= g_available_heap\n");
        printk ("heapAllocateMemory error: UserAreaInBytes >= g_available_heap\n");

        // #todo: 
        // Tentar crescer o heap para atender o size requisitado.
        //try_grow_heap() ...
        goto fail;
    }

// Increment block count and ensure we do not exceed block tracking array size.
    mmblockCount++;
    if (mmblockCount >= MMBLOCK_COUNT_MAX){
        x_panic ("heapAllocateMemory: mmblockCount\n");
    }

// #importante
// A variável 'Header', no header do bloco, 
// é o início da estrutura que o define. 'b->Header'. 
// Ou seja, o endereço da variável marca o início da estrutura.
// Pointer Limits:
// ( Não vamos querer um heap pointer fora dos limites 
//   do heap do kernel ).
// Se o 'g_heap_pointer' atual esta fora dos limites do heap, 
// então devemos usar o último válido, que provavelmente está 
// nos limites. ?? #bugbug: Mas se o último válido está sendo 
// usado por uma alocação anterior. ?? Temos flags que 
// indiquem isso ??
// #importante: 
// O HEAP POINTER TAMBÉM É O INÍCIO DE UMA ESTRUTURA. 
// NESSA ESTRUTURA PODEMOS SABER SE O HEAP ESTA EM USO OU NÃO.
// ISSO SE APLICA À TENTATIVA DE REUTILIZAR O ÚLTIMO HEAP 
// POINTER VÁLIDO.

// Out of range.
// Se estiver fora dos limites do heap do kernel.

// Ensure heap pointer is within kernel heap boundaries.
    if ( g_heap_pointer < KERNEL_HEAP_START || 
          g_heap_pointer >= KERNEL_HEAP_END )
    {
        x_panic ("heapAllocateMemory: Out of kernel heap");
    }

// #importante:
// Criando um bloco, que é uma estrutura mmblock_d.
// Estrutura mmblock_d interna.
// Configurando a estrutura para o bloco atual.
// Obs: A estutura deverá ficar lá no espaço reservado 
// para o header. (Antes da area alocada).
// #importante
// O endereço do ponteiro da estrutura será o pointer do heap.

// Agora temos um ponteiro para a estrutura.

// Create and initialize mmblock_d header at the current heap pointer.
    Current = (void *) g_heap_pointer;
    if ((void *) Current == NULL){
        debug_print("heapAllocateMemory: Current\n");
        printk     ("heapAllocateMemory: Current\n");
        goto fail;
    }

//
// Header -------------------------
//

// #importante:
// obs: 
// Perceba que 'Current' e 'Current->Header' devem ser iguais. 

// Identificadores básicos:
// Endereço onde começa o header.
// Tamanho do header. (TAMANHO DA STRUCT).
// Id do mmblock. (Índice na lista)
// used and magic flags.
// 0=not free 1=FREE (SUPER IMPORTANTE)

// Saving the address of the pointer of the structure.

// Initialize header fields.
    HeaderBase =  (unsigned long) g_heap_pointer;
    Current->Header = (unsigned long) HeaderBase; 
    Current->headerSize = (unsigned long) HeaderInBytes; 

//
// User area
//

// Initialize user area fields.
    UserAreaBase = (unsigned long) (HeaderBase + HeaderInBytes);
    Current->userArea = (unsigned long) UserAreaBase;
    Current->userareaSize = (unsigned long) UserAreaInBytes;

//
// Footer
//

// Footer:
// >> O footer começa no 
// 'endereço do início da área de cliente' + 'o tamanho dela'.
// >> O footer é o fim dessa alocação e início da próxima.
// #bugbug: 
// Penso que aqui deveríamos considerar 
// 'userareaSize' como tamanho da área de cliente, 
// esse tamanho equivale ao tamanho solicitado mais o 
// tanto de bytes não usados.
// #obs: 
// Por enquanto o tamanho da área de cliente tem 
// apenas o tamanho do espaço solicitado.
 
// Calculate and set footer (end of allocation).
    Current->Footer = (unsigned long) (UserAreaBase + UserAreaInBytes);

//--------------------------------------------
// Update heap usage statistics.

// All the bytes used this time.
    unsigned long Total = 
        (unsigned long) (Current->Footer - Current->Header);

// New available bytes.
    g_available_heap = (unsigned long) g_available_heap - Total;

//--------------------------------------------

// Save previous heap pointer.
    mm_prev_pointer = (unsigned long) g_heap_pointer; 
// Next heap pointer.
// Move to next free position.
    g_heap_pointer = (unsigned long) Current->Footer;

//--------------------------------------------

// Set metadata for tracking this allocation.
    Current->Id = (int) mmblockCount; 
    Current->Free = FALSE;  // Not free! Block is now allocated.
    Current->Used = TRUE;
    Current->Magic = 1234;

// List of pointers.
// Store pointer to header in tracking array.
    mmblockList[mmblockCount] = (unsigned long) Current;

// OK
// Return the address of the start of the user area.
// Return pointer to the user-usable area.
    return (unsigned long) UserAreaBase;

// #todo: 
// Checar novamente aqui o heap disponível. Se esgotou, tentar crescer.
// Colocar o conteúdo da estrutura no lugar destinado para o header.
// O header conterá informações sobre o heap.
// Se falhamos, retorna 0. Que equivalerá à NULL.
fail:
    refresh_screen();
    return (unsigned long) 0;
}


/*
 * heapFreeMemory:
 *   Marks a previously allocated heap block as reusable (stock),
 *   by modifying its mmblock_d header. Does NOT actually reclaim memory,
 *   but prepares the header for potential reuse in future allocations.
 *
 *   The pointer provided must point to the start of the user area
 *   (not the block header).
 *
 * IN:
 *   ptr   Pointer to the user area of the allocated block.
 */
// Mark the structure as 'reusable'. STOCK
// #todo: Precisamos de rotinas que nos mostre
// essas estruturas.
// IN: ptr.
// Esse ponteiro indica o início da área alocada para uso.
// Essa área fica logo após o header.
// O tamanho do header é MMBLOCK_HEADER_SIZE.
// A alocação de memória não é afetada por essa rotina,
// ela continua do ponteiro onde parou.
// This is a worker for kfree() in kstdlib.c
// It sets the ->magic flag to 4321, turning the
// mmblock_d structure reusable.
// #todo: We can clean up the user area.

void heapFreeMemory(void *ptr)
{
    struct mmblock_d *block_header;

// Validation
    if ((void *) ptr == NULL){
        debug_print ("heapFreeMemory: ptr\n");
        return;
    }

// Ensure pointer is within heap bounds.
    if ( ptr < (void *) KERNEL_HEAP_START || 
         ptr >= (void *) KERNEL_HEAP_END )
    {
        debug_print("heapFreeMemory: ptr limits\n");
        return;
    }

// Header
// Encontrando o endereço do header.
// O ponteiro passado é o endereço da área de cliente.

// Find header by subtracting header size from the given pointer.
    unsigned long UserAreaStart = (unsigned long) ptr; 
    unsigned long headerSize = sizeof(struct mmblock_d);
// The base of the header.
    block_header = (void *) (UserAreaStart - headerSize);

// Validate header.
    if ((void *) block_header == NULL)
    {
        debug_print("heapFreeMemory: block_header\n");
        return;
    }
    if ( block_header->Used != TRUE || block_header->Magic != 1234 )
    {
        debug_print("heapFreeMemory: block_header validation\n");
        return;
    }

// It's free now.
    //block_header->Free = 1;

// Mark the mmblock as reusable (stock); does not actually reclaim space.
    block_header->Used = TRUE;   // Still valid, but now reusable.
    block_header->Magic = 4321;  // Set magic to indicate reusable.
}

// get_process_heap_pointer:
// ?? Pega o 'heap pointer' do heap de um processo. ??
unsigned long get_process_heap_pointer (pid_t pid)
{
    struct process_d *p;
    unsigned long heapLimit=0;

    if (pid < 0 || pid >= PROCESS_COUNT_MAX){
        printk ("get_process_heap_pointer: pid\n");
        goto fail;
    }

// Process
    p = (void *) processList[pid];
    if ( (void *) p == NULL ){
        printk ("get_process_heap_pointer: p\n");
        goto fail;
    }
    if (p->used != TRUE || p->magic != 1234){
        printk ("get_process_heap_pointer: p validation\n");
        goto fail;
    }

// #
// Cada processo tem seu heap.
// É memória em ring3 compartilhada.
// Mas tem processo em ring0. Onde fica o heap nesse caso?

    heapLimit = (unsigned long) (p->HeapStart + p->HeapSize);

    if ( p->HeapPointer < p->HeapStart || 
         p->HeapPointer >= heapLimit )
    {
        printk("get_process_heap_pointer: heapLimit\n");
        goto fail;
    }

// Retorna o heap pointer do processo. 
    return (unsigned long) p->HeapPointer;

fail:
    return (unsigned long) 0; 
}

//
// #
// INITIALIZATION
//

// mmInitialize:
// Inicializa o memory manager.
// Init Memory Manager for x64:
// Heap, Stack, Pages, mmblocks, memory sizes, memory zones ...
// OUT: TRUE or FALSE.
// -------------------------------
// Initialize mm phase 0.
// + Initialize video support.
// + Inittialize heap support.
// + Inittialize stack support. 
// + Initialize memory sizes.
// -------------------------------
// Initialize mm phase 1.
// + Initialize framepool support.
// + Initializing the paging infra-structure.
//   Mapping all the static system areas.

int mmInitialize(int phase)
{
// Called by I_kmain() in kmain.c.
// + Initialize the memory support.
//   The kernel heap and the kernel stack.
// + The implementation of the main kernel allocator.
// + Initialize the physical memory manager.
// + Initialize the paging support.

    int Status=0;
    register int i=0;

    //debug_print("mmInitialize: [TODO] [FIXME]\n");

    if (phase == 0){

        // Video support
        gramk_initialize_video();

        // #todo: 
        // Inicializar algumas variáveis globais.
        // Chamar os construtores para inicializar o básico.
        // #todo: 
        // Clear BSS.
        // Criar mmClearBSS()

        // Initializing kernel heap.
        Status = (int) __init_kernel_heap();
        if (Status != 0){
            debug_print("mmInitialize: Heap\n");
            goto fail;
        }
        // Initializing kernel stack.
        Status = (int) __init_kernel_stack();
        if (Status != 0){
            debug_print ("mmInitialize: Stack\n");
            goto fail;
        }

        // Initialize the list of pointer.
        while (i<MMBLOCK_COUNT_MAX){
            mmblockList[i] = (unsigned long) 0;
            i++;
        };

        // Primeiro Bloco.
        // current_mmblock = (void *) NULL;

        // #importante:
        // Inicializando o índice la lista de ponteiros 
        // para estruturas de alocação.

        mmblockCount = (int) 0;

        // ...

        // Initialize the size of the physical memory
        // and the size of the system based on the memory size.
        // It needs to be before the pagetables initialization.
        // see: mmsize.c
        mmsize_initialize();

        // #debug
        //while(1){}
   
        // End of phase 0.
        goto InitializeEnd;


    // phase 1
    } else if (phase == 1) {

        // Inicializando o framepool (paged pool).
        // see mmpool.c
        initializeFramesAlloc();

        // Continua...

        // Initializing the paging infrastructure.
        // Mapping all the static system areas.
        // See: pages.c
        int PagingStatus=-1;
        PagingStatus = (int) pagesInitializePaging();
        if (PagingStatus<0){
            x_panic("mmInitialize: Paging");
        }

        // End of phase 1.
        goto InitializeEnd;
    } else {
        // Wrong phase number.
        // goto fail;
    };

InitializeEnd:
    //#debug
    //debug_print("mmInitialize: done\n");
    //refresh_screen();
    //while(1){}
    return TRUE;

fail:
    debug_print("mmInitialize: fail\n");
    //refresh_screen();
    //while(1){}
    return FALSE;
}

