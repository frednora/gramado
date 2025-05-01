
// mmpool.c

#include <kernel.h>   

static void *__pageObject(void);
static int __firstSlotForAList(int size);

/*
 * __pageObject:
 * Cria uma estrutura de página.
 * Procura um slot vazio para registrar ela.
 * Inicializa a estrutura.
 * Retorna o ponteiro da estrutura criada.
 */
// OUT:
// Retorna o ponteiro de estrutura.

static void *__pageObject(void)
{
    struct page_d *New;
    int __slot = 0;

// Vamos procurar um slot vazio.

    for ( __slot=0; 
          __slot < PAGE_COUNT_MAX; 
          __slot++ )
    {
        // #
        // Essa lista precisa ser inicializada com 0
        // antes de usarmos ela aqui nessa rotina.

        New = (void *) pageAllocList[__slot];

        if ( New == NULL )
        {
            New = (void *) kmalloc( sizeof(struct page_d) );

            // ?? panic ??
            if (New == NULL)
            {
                debug_print ("__pageObject:\n");
                printk      ("__pageObject:\n");

                // #todo: free() ??
                goto fail;
            }
            memset ( New, 0, sizeof(struct page_d) );

            New->id = __slot;  // Índice dentro da lista.
            
            New->used  = TRUE;
            New->magic = 1234;

            New->free = 0;        // Not free!
            New->next = NULL;

            // #bugbug ... isso tá errado.
            // endereço físico do inicio do frame.
            // New->address = (unsigned long) Address;
            // ...

            pageAllocList[__slot] = ( unsigned long ) New; 

            //debug_print ("page: ok\n");

            return (void *) New;
        };
    };

// Overflow.
//#debug ??
fail:
    // Message?
    //debug_print ("page: fail\n");
    return NULL; 
}


/*
 * newPage:
 * Aloca uma página em memoria compartilhada de ring3 
 * e retorna seu endereço virtual inicial. 
 * Isso é feito com base no id do pageframe e no endereço virtual 
 * inicial do pool de pageframes.
 * Obs: 
 * Alocaremos uma página de memória virtual e retornaremos o ponteiro 
 * para o início da página. Para isso usaremos o alocador de frames 
 * de memória física.
 */

// OUT:
// Retorna o endereço virtual da página alocada.
// #todo
// #fixme

void *newPage(void)
{
// Esse é o endereço virtual do início do pool de páginas.
    unsigned long base = (unsigned long) g_pagedpool_va;
    unsigned long va=0;
    unsigned long pa=0;
    int PageSize = PAGE_SIZE;  //4096;
    struct page_d *New;

    //debug_print ("newPage:\n");

// Invalid base address.
    if (base == 0){
        debug_print ("newPage: base\n");
        panic       ("newPage: base\n");
    }

// Create and register a page object.
    New = (void *) __pageObject();
    if (New == NULL){
        debug_print ("newPage: New\n");
        panic       ("newPage: New\n");
    }
    if (New->used != TRUE){
        debug_print ("newPage: New used\n");
        panic       ("newPage: New used\n");
    }
    if (New->magic != 1234){
        debug_print ("newPage: New magic\n");
        panic       ("newPage: New magic\n");
    }

// Index validation.
    if (New->id < 0){
        panic ("newPage: id underflow\n");
    }
    if (New->id >= PAGE_COUNT_MAX){
        panic ("newPage: id overflow\n");
    }

    New->locked = FALSE;

// Reference counter.
// How many processes?
    New->ref_count = 1;

    // #debug
    //printk ("newPage: base=%x id=%d \n",base,New->id);
    //while(1){}

//
// VA
// va = base + (index * page size);
//

    unsigned long va_offset = (unsigned long) (New->id * PageSize);
    va = (unsigned long) (base + va_offset);
    if (va==0){
        panic("newPage: va==0\n");
    }

//
// PA
// Using the kernel table to get the physical address.
//

    // IN: Virtual address, kernel pml4 address.
    pa = (unsigned long) virtual_to_physical(va,gKernelPML4Address);

// Invalid physical address.
// #todo
// SMALLSYSTEM_PAGEDPOLL_START is the physical address for the base
// in small systems.
    if (pa==0){
        panic ("newPage: pa==0\n");
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
    return (void *) va;
    //return (void *) ( base + (New->id * PageSize) );

fail:
    debug_print ("newPage: fail\n");
    panic       ("newPage: fail\n");
    return NULL;
}


// Allocate single page.
void *mm_alloc_single_page(void)
{
    //return (void *) allocPages (1);
    return (void *) newPage();
}

// Allocate n contiguous pages.
void *mm_alloc_contig_pages(size_t size)
{
    debug_print("mm_alloc_contig_pages: [TODO] [FIXME]\n");
    panic("mm_alloc_contig_pages: [TODO] its a work in progress\n");

    //if (size<=0)
       //panic("mm_alloc_contig_pages: [FIXME] invalid size");
      
    //#todo
    //return (void *) allocPages(size);
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

static int __firstSlotForAList(int size)
{
    register int i=0;

// Nosso limite é 512, que é o tamanho do pool.
// pois o pool tem 2mb,que dá 512 páginas de 4kb.
    int Max = PAGE_COUNT_MAX;//512;  

    int Base = 0;
    int Count = 0;
    void *slot;

tryAgain:

    for ( i=Base; i < Max; i++ )
    {
        slot = (void *) pageAllocList[i];

        // tenta novamente, começando numa base diferente.
        if ( (void *) slot != NULL )
        {
            Base = (Base + Count);
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



/*
 * allocPages:
 * @param número de páginas contíguas.
 * Obs: Pode ser que os pageframes não sejam 
 * contíguos mas as páginas serão.
 * estamos usando uma page table toda já mapeada. 4MB.
 * @TODO: ESSA ROTINA ESTÁ INCOMPLETA ... REVISAR. #bugbug
 * #bugbug: 
 * Se estamos lidando com o endereço base vitual, então estamos 
 * lidando com páginas pre alocadas e não pageframes.
 */
// #bugbug
// Estamos alocando memória compartilhada?
// então seria sh_allocPages() 
// Essa rotina aloca uma quantidade de páginas
// de um pool de páginas.
// São compartilhadas.
// #todo: Explicar o ring e as permissões.
// #tomos que ter um marcador de páginas disponíveis para
// livres para alocação.
// Nosso limite é 512 páginas, pois so temos 2mb de pool.
// #todo: change to 'ssize_t number_of_pages'.
// IN: number of pages.
void *allocPages(int size)
{
// Esse é o endereço virtual do início do pool de pageframes.
// #bugbug: O paged pool so tem 2mb, veja pages.c
// então só podemos mapear 2*1024*1024/4096 páginas.
    unsigned long base = (unsigned long) g_pagedpool_va;
    void *final_va;
    int __slot=0;
// Página inicial da lista
    struct page_d *Ret;   
    struct page_d *pageConductor;
    struct page_d *p;  //#todo: use page instead of p.
    unsigned long va=0;
    unsigned long pa=0;
    int Count=0;
    int __first_free_slot = -1;

    // #debug
    //debug_print ("allocPages:\n");

// Se devemos ou não incremetar o contador de uso.
    int IncrementUsageCounter=TRUE; //P->allocated_memory
    struct process_d *process;
    process = (void*) get_current_process_pointer();
    if( (void*) process == NULL )
        IncrementUsageCounter=FALSE;
    if(process->magic!=1234)
        IncrementUsageCounter=FALSE;

// Checando limites.

// Problemas com o size.
    if (size <= 0)
    {
        //size = 1;
        //if debug
        panic("allocPages: size 0\n");
        //return NULL;
    }

// Se é pra alocar apenas uma página.
    if (size == 1)
    {
        final_va = (void *) newPage();
        
        if ( (void*) final_va != NULL )
        {
            memset( final_va, 0, PAGE_SIZE );
            if (IncrementUsageCounter == TRUE)
            {
                if ( (void*) process != NULL )
                    process->allocated_memory += PAGE_SIZE;
            }
        }
        
        return (void*) final_va;
    }

// Se o size for maior que o limite.
    if ( size >= PAGE_COUNT_MAX ){
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
    if ( __first_free_slot == -1 ){
        debug_print ("allocPages: No more free slots\n");
        panic       ("allocPages: No more free slots\n");
    }

// Procurar slot vazio.
// Começamos a contar do frame logo após o condutor.
    for ( 
        __slot = __first_free_slot; 
        __slot < (__first_free_slot+size+1);
        __slot++ )
    {
        p = (void *) pageAllocList[__slot];

        // Slot livre
        if ( p == NULL )
        {
            // #bugbug
            // Isso pode esgotar o heap do kernel
            p = (void *) kmalloc( sizeof(struct page_d) );
            if ((void*) p == NULL){
                panic("allocPages: fail 2\n");
            }
            memset ( p, 0, sizeof(struct page_d) );

            //printk("#");
            
            p->id = __slot;
            p->used  = TRUE;
            p->magic = 1234;
            p->free = FALSE;
            p->locked = FALSE;

            // Contador de referências
            p->ref_count = 1;

            // #fixme
            // Precisamos usar pml4

            // Pegando o endereço virtual.
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

            pageConductor->next = (void *) p;
            pageConductor = (void *) pageConductor->next;

            // #obs:
            // Vamos precisar da estrutura da primeira página alocada.
            // #Importante:
            // Retornaremos o endereço virtual inicial do primeiro 
            // pageframe da lista. Ou seja, da primeira página.

            Count++;
            if ( Count >= size )
            {
                Ret = (void *) pageAllocList[__first_free_slot];
                
                if (IncrementUsageCounter==TRUE)
                {
                    if( (void*) process != NULL )
                        process->allocated_memory += (size*PAGE_SIZE);
                }
                
                return (void *) ( base + (Ret->id * PAGE_SIZE) );
            }
            //fail
        };
    };

fail:
    // #debug
    debug_print("allocPages: fail\n");
    //printk   ("allocPages: fail\n");
    panic      ("allocPages: fail\n");
    return NULL;
}

void *mmAllocPage(void)
{
    return (void*) newPage();
}

void *mmAllocPages(int size)
{
// IN: Number of pages.
    return (void*) allocPages(size);
}

// initializeFramesAlloc:
// Inicializa o framepool. 
void initializeFramesAlloc(void)
{
    struct page_d  *p;
    int __slot = 0;

    //debug_print("initializeFramesAlloc:\n");

// Inicializando a lista de pages.
// 512 pages

    for ( __slot=0; 
          __slot < PAGE_COUNT_MAX; 
          __slot++ )
    {
        pageAllocList[__slot] = (unsigned long) 0;
    };

// Criando o primeiro para testes.
// #bugbug
// Talvez seja desnecessário criar essa entrada.

    p = (void *) kmalloc( sizeof(struct page_d) );
    if ((void*) p == NULL){
        debug_print("initializeFramesAlloc:\n");
        panic      ("initializeFramesAlloc:\n");
    }
    memset ( p, 0, sizeof(struct page_d) );
    p->used = TRUE;
    p->magic = 1234;

    p->id = 0;
    p->free = TRUE;  //free
    p->next = NULL; 
    // ...
    pageAllocList[0] = (unsigned long) p;
}



