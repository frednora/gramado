// mmpool.c
// Allocating shared virtual memory from a pool of pages.
// This code implements a paged pool allocator for 
// shared virtual memory (pre-mapped pages from a fixed VA range). 
// It's a classic kernel technique for fast shared buffers.
// Created by Fred Nora.

#include <kernel.h>   

static void *__pageObject(void);
static struct page_d* __page_create(int slot);
static int __firstSlotForAList(int size);

// =====================================================================

static struct page_d* __page_create(int slot)
{
    struct page_d *page = (struct page_d*) kmalloc(sizeof(struct page_d));
    if (page == NULL)
        return NULL;

    memset(page, 0, sizeof(struct page_d));

    page->id                = slot;
    page->used              = TRUE;
    page->free              = FALSE;
    page->locked            = FALSE;
    page->magic             = 1234;
    page->ref_count         = 1;
    page->next              = NULL;
    page->absolute_frame_number = 0;

    return (struct page_d*) page;
}

// __pageObject:
// Create a page structure.
// Register it into an empty slot in pageAllocList[] and 
// return the pointer for the structure.
// #ps: This list must to be initialized before calling this worker.

static void *__pageObject(void)
{
    struct page_d *New;
    int __slot = 0;

// Probe for an empty slot
    for ( __slot=0; 
          __slot < PAGE_COUNT_MAX; 
          __slot++ )
    {
        New = (void *) pageAllocList[__slot];

        if (New == NULL)
        {
            New = (void *) kmalloc( sizeof(struct page_d) );
            if (New == NULL)
            {
                debug_print ("__pageObject:\n");
                printk      ("__pageObject:\n");
                goto fail;
            }
            memset ( New, 0, sizeof(struct page_d) );

            New->id = __slot;   // Save the index
            New->free = FALSE;  // Not free 
            New->used = TRUE;
            New->magic = 1234;

            New->next = NULL;

            // #bugbug ... isso tá errado.
            // endereço físico do inicio do frame.
            // New->address = (unsigned long) Address;
            // ...

            // Register and return.
            pageAllocList[__slot] = ( unsigned long ) New; 
            return (void *) New;
        }
    };

// Overflow
fail:
    return NULL; 
}


/*
 * __firstSlotForAList:
 * Retorna o primeiro índice de uma sequência de slots livres 
 * em pageAllocList[].
 */

// IN:
// A quantidade de slots livres consecutivos que precisamos.
// Nosso limite é 1024, que é o tamanho do pool.
// OUT:
// Retorna o índice do primeiro slot 
// de uma sequencia concatenada de slots livres.
// Ou retorn '-1' no caso de erro.
// #todo: Explain it better.

/*
// #suspended:
// Suspending this old imlementation.
// Using a new one. Probably safer.
static int __firstSlotForAList(int size)
{
    register int i=0;

// Nosso limite é 512, que é o tamanho do pool.
// pois o pool tem 2mb,que dá 512 páginas de 4kb.
    int Max = PAGE_COUNT_MAX;  //512;  

    int Base=0;
    int Count=0;
    void *slot;

tryAgain:

    for (i=Base; i<Max; i++)
    {
        slot = (void *) pageAllocList[i];

        // tenta novamente, começando numa base diferente.
        if ((void *) slot != NULL)
        {
            Base = (int) (Base + Count);
            Base++;
            Count = 0;
            
            //#bugbug: Podemos fica aqui pra sempre?
            goto tryAgain;
        }

        Count++; 

        if (Count >= size)
        {
            // OUT: 
            // Retorna o índice do primeiro slot 
            // de uma sequencia concatenada de slots livres.
            return (int) Base; 
        }
    };

// Fail: No empty slot.
    return (int) -1;
}
*/


/*
 * __firstSlotForAList:
 * Find the starting index of 'size' consecutive free slots.
 * Returns -1 if no contiguous block of that size exists.
 */
static int __firstSlotForAList(int size)
{
    int start=0;
    int i=0;

    if (size <= 0 || size >= PAGE_COUNT_MAX)
        return -1;

    /* 
     * Outer loop: possible starting positions 
     * We stop early enough so we don't go out of bounds.
     */

    for ( 
        start = 0; 
        start <= PAGE_COUNT_MAX - size; 
        start++ )
    {
        /* Inner loop: check if we have 'size' consecutive free slots */
        int free_count = 0;

        for (i = start; i < start + size; i++)
        {
            if (pageAllocList[i] != 0)   // slot is occupied
            {
                break;                   // stop checking this range
            }
            free_count++;
        }

        /* We found a perfect contiguous block */
        if (free_count == size)
        {
            return start;
        }
    }

    return -1;   // No contiguous free block found
}


/*
 * #ps: Old comments.
 * Isso é feito com base no id do pageframe e no endereço virtual 
 * inicial do pool de pageframes.
 * Obs: 
 * Alocaremos uma página de memória virtual e retornaremos o ponteiro 
 * para o início da página. Para isso usaremos o alocador de frames 
 * de memória física.
 */

// mmNewPage:
// Allocate one page of ring 3 shared memory and return its virtual address.
// ...
// OUT:
// + The virtual address for the new page.

void *mmNewPage(void)
{

// This is the base virtual address for the pool of pages.
// These pages are shared with all the process that 
// cloned the kernel directory during its creation.
    unsigned long base = (unsigned long) g_pagedpool_va;

    unsigned long va=0;
    unsigned long pa=0;
    struct page_d *New;

    // #ps: 
    // For now each page in the pool has 4096 Byte.
    int PageSize = PAGE_SIZE;  // 4096

    //debug_print ("mmNewPage:\n");

// Invalid base address
// #todo: Maybe we can stablish some other limits here.
    if (base == 0){
        debug_print ("mmNewPage: base\n");
        panic       ("mmNewPage: base\n");
    }

// Create and register a page object.
    New = (void *) __pageObject();

// Struture validation
    if (New == NULL){
        debug_print ("mmNewPage: New\n");
        panic       ("mmNewPage: New\n");
    }
    if (New->used != TRUE){
        debug_print ("mmNewPage: New used\n");
        panic       ("mmNewPage: New used\n");
    }
    if (New->magic != 1234){
        debug_print ("mmNewPage: New magic\n");
        panic       ("mmNewPage: New magic\n");
    }

// Index validation
    if (New->id < 0){
        panic ("mmNewPage: id underflow\n");
    }
    if (New->id >= PAGE_COUNT_MAX){
        panic ("mmNewPage: id overflow\n");
    }

    New->locked = FALSE;

// Reference counter.
// How many processes are using it?
    New->ref_count = 1;

    // #debug
    //printk ("mmNewPage: base=%x id=%d \n",base,New->id);
    //while(1){}

//
// VA
//

// va = base + (index * page size);
    unsigned long va_offset = (unsigned long) (New->id * PageSize);
    va = (unsigned long) (base + va_offset);
    if (va == 0){
        panic("mmNewPage: va={0}\n");
    }

//
// PA
//

// Using the kernel table to get the physical address.
// See: x64mm.c

    unsigned long kernel_pml4_va = gKernelPML4Address;

    // IN: Virtual address, kernel pml4 va.
    pa = (unsigned long) virtual_to_physical( va, kernel_pml4_va );

// Invalid physical address.
// #todo
// SMALLSYSTEM_PAGEDPOLL_START is the physical address for 
// the base in small systems.
    if (pa==0){
        panic ("mmNewPage: pa==0\n");
    }

// ++
// -------------------------
// What is the position of the frame,
// starting fromt the beginning of the physical memory?
// #todo
// #bugbug
// The routine below is not good.
// Maybe we can change it.
// #test
// Calculando o número do frame com base
// no endereço físico.
    unsigned long alignedPA = (unsigned long) pa;
    unsigned long remainder = (unsigned long)( pa % PAGE_SIZE );
// Se temos um resto, ajustamos o endereço físico
// par apontar par ao início do frame.
    if (remainder != 0){
        alignedPA = (unsigned long) ( pa - remainder );
    }
// Com base no endereço do início do frame,
// calculamos o indic do frame.
// Os frames são contados à partir do início 
// da memória física.                 
    New->absolute_frame_number = 
        (unsigned int) (alignedPA / PAGE_SIZE);
// -------------------------
// --

// Return the virtual address.
// ( base + (New->id * PageSize)
    return (void *) va;

fail:
    debug_print ("mmNewPage: fail\n");
    panic       ("mmNewPage: fail\n");
    return NULL;
}

/*
 * allocPages:
 * @param número de páginas contíguas.
 * Obs: Pode ser que os pageframes não sejam contíguos mas as páginas serão.
 * estamos usando uma page table toda já mapeada. 
 * @TODO: ESSA ROTINA ESTÁ INCOMPLETA ... REVISAR. #bugbug
 * #bugbug: 
 * Se estamos lidando com o endereço base vitual, então estamos 
 * lidando com páginas pre alocadas e não pageframes.
 */
// #bugbug
// Estamos alocando memória compartilhada?
// então seria sh_allocPages() 
// Essa rotina aloca uma quantidade de páginas de um pool de páginas.
// São compartilhadas.
// #todo: Explicar o ring e as permissões.
// #tomos que ter um marcador de páginas disponíveis para
// livres para alocação.
// Nosso limite é 512 páginas, pois so temos 2mb de pool.
// #todo: change to 'ssize_t number_of_pages'.
// IN: number of pages.
void *allocPages(size_t size)
{
// Esse é o endereço virtual do início do pool de pageframes.
// #bugbug: O paged pool so tem 2mb, veja pages.c
// então só podemos mapear 2*1024*1024/4096 páginas.
    const unsigned long base = (unsigned long) g_pagedpool_va;
    void *final_va;
    int __slot=0;
// Página inicial da lista
    struct page_d *pRet; // pagina inicial da lista criada nessa rotina. 
    struct page_d *pageConductor = NULL;
    struct page_d *p;  //#todo: use page instead of p.
    unsigned long va=0;
    unsigned long pa=0;
    int Count=0;
    int __first_free_slot = -1;

    // #debug
    //debug_print ("allocPages:\n");

// Se devemos ou não incremetar o contador de uso.
    int IncrementUsageCounter=TRUE;  //P->allocated_memory

    /*

    // #suspended:
    // It's suspended because the allocator is called during the kernel initialization,
    // when we still do not have a pointer for the current process yet.

    struct te_d *cp_pointer;  // Pointer for the current process.
    cp_pointer = (void*) get_current_process_pointer();
    if ((void*) cp_pointer == NULL)
    {
        IncrementUsageCounter=FALSE;
        panic ("allocPages: cp_pointer\n");
    }
    if (cp_pointer->magic != 1234)
    {
        IncrementUsageCounter=FALSE;
        panic("allocPages: cp_pointer->magic\n");
    }
    */

// Checando limites

// Invalid size
    if (size <= 0){
        return NULL;
    }

// If it is for allocating only one page
    if (size == 1)
    {
        final_va = (void *) mmNewPage();

        if ((void*) final_va != NULL)
        {
            memset( final_va, 0, PAGE_SIZE );
            /*
            if (IncrementUsageCounter == TRUE)
            {
                if ( (void*) cp_pointer != NULL )
                    cp_pointer->allocated_memory += PAGE_SIZE;
            }
            */
        }
        
        return (void*) final_va;
    }

// #bugbug
// Se o size for maior que o limite total, para alem do disponivel
    if (size >= PAGE_COUNT_MAX){
        panic ("allocPages: size limits\n");
    }

// Isso encontra slots o suficiente para alocarmos 
// tudo o que queremos.
// PANIC !!
// A memória para a locação acabou.
// #todo:
// Liberar páginas mandando para o disco conforme
// critéria à definir ainda,

    __first_free_slot = (int) __firstSlotForAList(size);

    //if ( __first_free_slot < 0 )
    if (__first_free_slot == -1)
    {
        debug_print ("allocPages: No more free slots\n");
        panic       ("allocPages: No more free slots\n");
    }

// Procurar slot vazio.
// Começamos a contar do frame logo após o condutor.
// #bugbug: (__first_free_slot + size + 1) pode estar alem do fim da lista.

    int TargetSize = (__first_free_slot + size + 1);
    if (TargetSize >= PAGE_COUNT_MAX)
        panic ("allocPages: TargetSize\n");

    for ( 
        __slot = __first_free_slot; 
        __slot < TargetSize;
        __slot++ )
    {
        p = (void *) pageAllocList[__slot];
        if ((void*) p != NULL)
            panic("allocPages: p\n");

        // Slot livre
        if (p == NULL)
        {
            // #bugbug
            // Isso pode esgotar o heap do kernel
            p = (void *) kmalloc( sizeof(struct page_d) );
            if ((void*) p == NULL){
                panic("allocPages: fail 2\n");
            }
            memset ( p, 0, sizeof(struct page_d) );

            //printk("#");
            
            p->id = (int) __slot;
            p->free = FALSE;
            p->locked = FALSE;

            // Contador de referências
            p->ref_count = 1;

            p->used = TRUE;
            p->magic = 1234;

            // #fixme
            // Precisamos usar pml4

            // Pegando o endereço virtual.
            // #bugbug: 
            // Estamos usando o pml4 do kernel para todos os programas que chamam essa rotina?
            // Não deveria ser um endereço para cada processo?
            va = (unsigned long) ( base + (p->id * PAGE_SIZE) ); 
            pa = (unsigned long) virtual_to_physical ( va, gKernelPML4Address ); 

            //++
            //-----------
            // Getting the absolute frame number,
            // starting from the beginning of the physiscal memory.
            if ( ( pa % PAGE_SIZE ) != 0 ) {
                pa = pa - ( pa % PAGE_SIZE);
            }
            p->absolute_frame_number = (pa / PAGE_SIZE);
            if (pa == 0){
                p->absolute_frame_number = 0;
            }
            //-----------
            //--

            //---

            pageAllocList[__slot] = ( unsigned long ) p;

            // Linking the pages.
            if ((void*) pageConductor == NULL){
                pageConductor = (void *) p;
                pageConductor->next = NULL;
            } else if ((void*) pageConductor != NULL){
                pageConductor->next = (void *) p;
                pageConductor = (void *) pageConductor->next;
            }

            // #obs:
            // Vamos precisar da estrutura da primeira página alocada.
            // #Importante:
            // Retornaremos o endereço virtual inicial do primeiro 
            // pageframe da lista. Ou seja, da primeira página.

            Count++;
            if ( Count >= size )
            {
                pRet = (void *) pageAllocList[__first_free_slot];
                
                /*
                if (IncrementUsageCounter==TRUE)
                {
                    if ((void*) cp_pointer != NULL)
                        cp_pointer->allocated_memory += (size*PAGE_SIZE);
                }
                */

                if ((void *) pRet == NULL)
                    panic ("allocPages: pRet\n");
                if (pRet->used != TRUE)
                    panic ("allocPages: pRet->used\n");
                if (pRet->magic != 1234)
                    panic ("allocPages: pRet->magic\n");

                return (void *) ( base + (pRet->id * PAGE_SIZE) );
            }
            //fail
        };
    };

fail:

    // #debug
    // For now its necessary
    debug_print("allocPages: fail\n");
    printk     ("allocPages: fail\n");
    panic      ("allocPages: fail\n");

    return NULL;
}

// Allocate single page
void *mm_alloc_single_page(void)
{
    return (void *) mmNewPage();
}

// Allocate n contiguous pages
void *mm_alloc_contig_pages(size_t size)
{
    if (size <= 0)
        return NULL;
      
    return (void *) allocPages(size);
}

void *mmAllocPage(void)
{
    return (void*) mmNewPage();
}

// IN: Number of pages
void *mmAllocPages(size_t size)
{
    if (size <= 0)
        return NULL;

// IN: Number of pages
    return (void*) allocPages(size);
}


// Initializes the list of pages
// + Initializes the pageAllocList[] list
// + Setup the first slot just for testing purpose.
void initializeFramesAlloc(void)
{
    int __slot = 0;
    struct page_d  *p;

    //debug_print("initializeFramesAlloc:\n");

// Initializes the list of pages. 512 pages.
    for ( __slot=0; 
          __slot < PAGE_COUNT_MAX; 
          __slot++ )
    {
        pageAllocList[__slot] = (unsigned long) 0;
    };

// ---------------------------------
// Create and save the pointer for the first entry,
// #todo: Maybe it's not necessary.
    p = (void *) kmalloc(sizeof(struct page_d));
    if ((void*) p == NULL){
        debug_print("initializeFramesAlloc:\n");
        panic      ("initializeFramesAlloc:\n");
    }
    memset( p, 0, sizeof(struct page_d) );
    p->id = 0;
    p->free = TRUE;  // Free
    p->used = TRUE;
    p->magic = 1234;
    p->next = NULL; 
    // ...
    pageAllocList[0] = (unsigned long) p;
// ---------------------------------

}



