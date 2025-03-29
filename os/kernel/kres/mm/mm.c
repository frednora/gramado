
// mm.c
// + Initialize the memory support.
//   The kernel heap and the kernel stack.
// + The implementation of the main kernel allocator.
// + Initialize the physical memory manager.
// + Initialize the paging support.
// 2015 - Created by Fred Nora.

#include <kernel.h>

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

static int __init_heap(void);
static int __init_stack(void);

// ----------------------------

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


/*
 * __init_heap:
 *     Iniciar a gerência de Heap do kernel. 
 * Essa rotina controi a mão o heap usado pelo processo kernel.
 *     +Ela é chamada apenas uma vez.
 *     +Ela deve ser chamada entes de qualquer outra operação 
 * envolvendo o heap do processo kernel.
 * @todo: Rotinas de automação da criação de heaps para processos.
 */

// OUT: 0=OK.
static int __init_heap(void)
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
        debug_print("__init_heap: [FAIL] g_heap_pointer\n");
        goto fail;
    }

// Check Heap Pointer overflow.
    if (g_heap_pointer > kernel_heap_end){
        debug_print("__init_heap: [FAIL] Heap Pointer Overflow\n");
        goto fail;
    }

// Heap Start
    if (kernel_heap_start == 0){
        debug_print("__init_heap: [FAIL] HeapStart\n");
        goto fail;
    }

// Heap End
    if (kernel_heap_end == 0){
        debug_print("__init_heap: [FAIL] HeapEnd\n");
        goto fail;
    }

// Check available heap.
// #todo: Tentar crescer o heap.
    if (g_available_heap == 0){
        debug_print("__init_heap: [FAIL] g_available_heap\n");
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
    debug_print("__init_heap: Fail\n");
    //refresh_screen();
    return (int) -1;
}

/*
 * __init_stack:
 *     Iniciar a gerência de Stack do kernel. 
 *     #todo: Usar stackInit(). 
 */
// OUT: 0=OK.
static int __init_stack(void)
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
        debug_print("__init_stack: [FAIL] kernel_stack_end\n");
        goto fail;
    }
// Start
    if (kernel_stack_start == 0){
        debug_print("__init_stack: [FAIL] kernel_stack_start\n");
        goto fail;
    }

    KernelStack.initialized = TRUE;

// OUT: 0=OK.
    return 0;

fail:
    KernelStack.initialized = FALSE;
    return (int) -1;
}

// #todo: Move this routine to another file.
int kernel_gc(void)
{
    panic ("kernel_gc: Unimplemented\n");
    return -1;
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


// mmInit:
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
        zero_initialize_video();

        // #todo: 
        // Inicializar algumas variáveis globais.
        // Chamar os construtores para inicializar o básico.
        // #todo: 
        // Clear BSS.
        // Criar mmClearBSS()

        // heap and stack
        Status = (int) __init_heap();
        if (Status != 0){
            debug_print("mmInitialize: Heap\n");
            goto fail;
        }
        Status = (int) __init_stack();
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
        PagingStatus = (int) mmInitializePaging();
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

/*
 * heapAllocateMemory:
 *     Aloca memória no heap do kernel.
 * IMPORTANTE: 
 *     Aloca BLOCOS de memória dentro do heap do processo Kernel.
 * @todo: 
 *     ?? Ao fim dessa rotina, os valores da estrutura devem ser 
 * armazenas no header, lá onde foi alocado espaço para o header, 
 * assim tem-se informações sobre o header alocado. ??
 *  A estrutura header do heap, é uma estrutura e deve ficar antes 
 * da área desejada. Partes={ header,client,footer }.
 * Obs: 
 *     ?? A estrutura usada aqui é salva onde, ou não é salva ??
 */

// IN:
// Size in bytes.
// OUT: 
// Address if success. '0' if fail.

unsigned long heapAllocateMemory(unsigned long size)
{
// This is a worker for kmalloc() and __kmalloc_impl() in kstdlib.c
// Allocate memory inside the kernel heap.

    struct mmblock_d *Current;

// Header
    unsigned long HeaderBase = 0;
    unsigned long HeaderInBytes = (unsigned long) ( sizeof(struct mmblock_d) ); 

// User area
    unsigned long UserAreaBase = 0;
    unsigned long UserAreaInBytes = (unsigned long) size;

// The desired size if 0.
// Can't allocate 0 size.
    if (UserAreaInBytes == 0){
        UserAreaInBytes = (unsigned long) 8;
    }

// No more available heap.
// 0 bytes.
    if (g_available_heap == 0){
        debug_print ("heapAllocateMemory: g_available_heap={0}\n");
        printk      ("heapAllocateMemory: g_available_heap={0}\n");
        goto fail;
    }

// #bugbug
// And if the available heap is an invalid big number?

// Se o tamanho desejado é maior ou 
// igual ao espaço disponível.

    if (UserAreaInBytes >= g_available_heap)
    {
        debug_print ("heapAllocateMemory error: UserAreaInBytes >= g_available_heap\n");
        printk ("heapAllocateMemory error: UserAreaInBytes >= g_available_heap\n");

        // #todo: 
        // Tentar crescer o heap para atender o size requisitado.
        //try_grow_heap() ...
        goto fail;
    }

// Contador de blocos.
// #obs: 
// Temos um limite para a quantidade de índices na lista de blocos.
// #bugbug
// Mesmo tendo espaço suficiente no heap, estamos chegando 
// nesse limite de indices.

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
    Current = (void *) g_heap_pointer;
    if ((void *) Current == NULL){
        debug_print("heapAllocateMemory: [FAIL] struct\n");
        printk     ("heapAllocateMemory: [FAIL] struct\n");
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
    HeaderBase =  (unsigned long) g_heap_pointer;
    Current->Header = (unsigned long) HeaderBase; 
    Current->headerSize = (unsigned long) HeaderInBytes; 

//
// User area
//

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
 
    Current->Footer = (unsigned long) (UserAreaBase + UserAreaInBytes);

//--------------------------------------------

// All the bytes used this time.
    unsigned long Total = 
        (unsigned long) (Current->Footer - Current->Header);

// New available bytes.
    g_available_heap = (unsigned long) g_available_heap - Total;

//--------------------------------------------

// Previous heap pointer.
    mm_prev_pointer = (unsigned long) g_heap_pointer; 
// Next heap pointer.
    g_heap_pointer = (unsigned long) Current->Footer;

//--------------------------------------------

    Current->Id = (int) mmblockCount; 

    Current->Free = FALSE;  // Not free!
    Current->Used = TRUE;
    Current->Magic = 1234;

// List of pointers.
    mmblockList[mmblockCount] = (unsigned long) Current;

// OK
// Return the address of the start of the user area.
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

// Mark the structure as 'reusable'. STOCK
// #todo: Precisamos de rotinas que nos mostre
// essas estruturas.
// IN: ptr.
// Esse ponteiro indica o início da área alocada para uso.
// Essa área fica logo após o header.
// O tamanho do header é MMBLOCK_HEADER_SIZE.
// A alocação de memória não é afetada por essa rotina,
// ela continua do ponteiro onde parou.
void heapFreeMemory(void *ptr)
{
// This is a worker for kfree() in kstdlib.c
// It sets the ->magic flag to 4321, turning the
// mmblock_d structure reusable.
// #todo: We can clean up the user area.

    struct mmblock_d *block_header;

// Validation
    if ((void *) ptr == NULL){
        debug_print ("heapFreeMemory: ptr\n");
        return;
    }

// Validation
// Out of range.
    if ( ptr < (void *) KERNEL_HEAP_START || 
         ptr >= (void *) KERNEL_HEAP_END )
    {
        debug_print("heapFreeMemory: ptr limits\n");
        return;
    }

// Header
// Encontrando o endereço do header.
// O ponteiro passado é o endereço da área de cliente.

    unsigned long UserAreaStart = (unsigned long) ptr; 
    unsigned long headerSize = sizeof(struct mmblock_d);


// The base of the header.
    block_header = (void *) (UserAreaStart - headerSize);

// Invalid block header.
    if ((void *) block_header == NULL){
        debug_print("heapFreeMemory: block_header\n");
        return;
    }
// Invalid block header.
    if ( block_header->Used != TRUE || block_header->Magic != 1234 ){
        debug_print("heapFreeMemory: block_header validation\n");
        return;
    }

// It's free now.
    //block_header->Free = 1;

// Apenas marcamos a estrutura como reusável,
// pois agora ela esta no STOCK.
    block_header->Used = TRUE;   // still alive.
    block_header->Magic = 4321;  // reusable, stock
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

