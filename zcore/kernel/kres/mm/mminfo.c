
// mminfo.c
// Memory info support.

#include <kernel.h>  

void mmShowPML4Entry(int index, unsigned long pml4_va)
{

// #fixme:
// 9 9 9 9 12
// Agora as tabelas possuem 512 entradas,
// pois é isso o que dá pra ter com apenas 9 bits.

    // pd address.
    unsigned long pd_va=0;


// #todo: 
// filtros.

    // #todo
    debug_print("mmShow_PML4Entry: [TODO]\n");
    
//
// Returning ...
//    
    
    return;

/*
 
// =======================================

    if (pml4_va == 0){
        return;
    }

//
// pd
//


    if (pd_va == 0){
        return;
    }

    unsigned long *dir = (unsigned long *) pd_va;

//#todo: filtros.

    if (index < 0){
        return;
    }

    unsigned long value = dir[index];

    printk (" DirVA = %x ", (unsigned long) pd_va );
    printk (" DirEntry %d = %x ", index, (unsigned long) value );

//
// pt
//

	//12 bit de flags     1000 0000 0000
	unsigned long pt_address = (unsigned long) (value & 0xFFFFF800);
	unsigned long *pt = (unsigned long *) pt_address;
	
	printk (" PT_Address = %x ", (unsigned long) pt_address );
	
	//primeira entrada da pt.
	printk (" PT_Entry_0 = %x \n", (unsigned long) pt[0] );	

*/
}

// mmShowMemoryInfo:
// Show memory info.
void mmShowMemoryInfo(void)
{
    unsigned long HeapTotal = 
        ((kernel_heap_end - kernel_heap_start)/1024);
    unsigned long StackTotal = 
        ((kernel_stack_start - kernel_stack_end)/1024);

    printk ("RAM Memory info:\n");

    printk ("\n");
    printk ("BaseMemory     (%d KB)\n", memorysizeBaseMemory );
    printk ("OtherMemory    (%d KB)\n", memorysizeOtherMemory );
    printk ("ExtendedMemory (%d KB)\n", memorysizeExtendedMemory );
    printk ("TotalMemory    (%d KB)\n", memorysizeTotal );

    printk ("\n");
    printk ("TotalUsed      (%d KB)\n", memorysizeUsed );
    printk ("TotalFree      (%d KB)\n", memorysizeFree );

    printk ("\n");    
    printk ("ring0 area     (%d KB)\n", mm_used_ring0_area );
    printk ("ring3 area     (%d KB)\n", mm_used_ring3_area );
    printk ("kernel image area  (%d KB)\n", mm_used_kernelimage );
    printk ("backbuffer     (%d KB)\n", mm_used_backbuffer );
    printk ("paged pool     (%d KB)\n", mm_used_pagedpool );
    printk ("heap pool      (%d KB)\n", mm_used_heappool );
    printk ("extraheap1     (%d KB)\n", mm_used_extraheap1 );
    printk ("extraheap2     (%d KB)\n", mm_used_extraheap2 );
    printk ("extraheap3     (%d KB)\n", mm_used_extraheap3 );
    printk ("frame table    (%d KB)\n", mm_used_frame_table );

//==================================
// System type. (size)
    printk ("\n");
    printk ("=======================\n");
    printk ("This is a ");

    if( g_mm_system_type == stSmallSystem ){
        printk ("SMALL system ");
    }else if( g_mm_system_type == stMediumSystem ){
        printk ("MEDIUM system ");
    }else if( g_mm_system_type == stLargeSystem ){
        printk ("LARGE system ");
    }else{
        printk ("(Undefined) system ");
    };

    printk ("with %d MB \n", (memorysizeTotal/1024) );

//==================================
// Other info:
// see: pages.c
    printk("\n");
    pages_print_info(g_mm_system_type);

//==================================
// Frame table

    printk ("\n");
    printk ("Frame Table:\n");
    printk ("Start PA: %xH | End PA: %xH\n", FT.start_pa, FT.end_pa );
    printk ("Size: %d KB\n", FT.size_in_kb );
    printk ("Total frames:     [%d]\n",    FT.size_in_frames );
    printk ("Used frames:      [%d]\n",    FT.number_of_used_frames );
    printk ("Reserved frames:  [%d]\n",    FT.number_of_reserved_frames );
    // ...

//==================================

    //#todo
    //See: gramnt/kernel/include/mm/x86mm.h

	//system zone
	//printk("systemzoneStart  = 0x%x\n", systemzoneStart);
	//printk("systemzoneEnd    = 0x%x\n", systemzoneEnd);
	//printk("systemzoneSize   = 0x%x\n", systemzoneSize);

	//window zone.
	//printk("windowzoneStart  = 0x%x\n", windowzoneStart);
	//printk("windowzoneEnd    = 0x%x\n", windowzoneEnd);
	//printk("windowzoneSize   = 0x%x\n", windowzoneSize);

	// #Suspenso
	// Suspendemos o uso de variáveis importadas do makefile.

/*
#ifdef IMPORTED_VARIABLES
	//=======================================
	// #Warning                            //
	// Variables imported from link.ld     //
	//=======================================
	
	// ## code rodata data bss ##
	
	extern unsigned long code_begin;
	extern unsigned long code_end;

	extern unsigned long rodata_begin;
	extern unsigned long rodata_end;
	
	extern unsigned long data_begin;
	extern unsigned long data_end;
	
	extern unsigned long bss_begin;
	extern unsigned long bss_end;
	
	printk("\n");
	printk("code_begin={%x} code_end={%x} \n", &code_begin, &code_end );
	printk("rodata_begin={%x} rodata_end={%x} \n", &rodata_begin, &rodata_end );
	printk("data_begin={%x} data_end={%x} \n", &data_begin, &data_end );
	printk("bss_begin={%x}  bss_end={%x}\n", &bss_begin, &bss_end );
	
#endif
*/

//
// kernel image 
//

    //#todo
    //unsigned long imageStart = KERNEL_IMAGE_BASE;
    //unsigned long imageEnd;
    //unsigned long imageSize = 

    unsigned long imageAvailableAreaStart = KERNEL_IMAGE_BASE;
    unsigned long imageAvailableAreaEnd   = (kernel_heap_start - 8);
    unsigned long imageAvailableAreaSize_in_Bytes = 
        (imageAvailableAreaEnd - imageAvailableAreaStart);
    unsigned long imageAvailableAreaSize_in_KB = 
        (imageAvailableAreaSize_in_Bytes/1024);

    printk("\n");
    printk("KERNEL IMAGE SIZE: #todo \n");
    printk("KERNEL IMAGE AVAILABLE AREA VA:  [%x...%x] Total={%d KB} \n",
        imageAvailableAreaStart, 
        imageAvailableAreaEnd, 
        imageAvailableAreaSize_in_KB );

//
//  heap e stack 
//
    
    //printk("\n[Kernel Heap and Stack info:]\n");

    printk("\n");
    printk("KERNEL HEAP VA:  [%x...%x] Total={%d KB} \n",
        kernel_heap_start, kernel_heap_end, HeapTotal );
    printk("AvailableHeap={%d KB}\n", (g_available_heap/1024) );

// @todo:
// Mostrar o tamanho da pilha..
// #bugbug: A informações sobre a stack estão incorretas, 
// pois essas variáveis mostram o endereço da stack na hora 
// da inicialização. Quando o processador retorna de ring3 
// para ring0 ele usa o endereço de pilha indicado na TSS.
// Pois bem, é mais digno mostrar aqui o endereço da pilha, 
// indicado no TSS.

    printk("\n");
    printk("KERNEL STACK VA: [%x...%x] Total={%d KB} \n", 
        kernel_stack_start, kernel_stack_end, StackTotal );
    printk("KERNEL STACK PA: StartPhysicalAddress={%x} \n", kernel_stack_start_pa );

//==================================
// Video info.
    pages_print_video_info();

    //...
}

void mmShowPML4EntryForAllProcesses (int entry_number)
{
    struct process_d  *p;
    int i=0;

    printk ("mmShowPML4EntryForAllProcesses:\n");

    if (entry_number < 0 || entry_number >= 512 )
    {
        return;
    }

    for ( i=0; i<111; i++)
    {
        p = (struct process_d *) processList[i];

        if ( (void *) p != NULL )
        {

            // #fixme

            // Mostra a entrada 1, que se refere ao endereço lógico 0x400000
            //if ( p->DirectoryVA != 0 )
            //{
            //    printk ("Process %d: ", i);
            //    mmShowPDE ( entry_number, (unsigned long) p->DirectoryVA );
            //}
        }
    };
}

// mmShowPagedMemoryList:
// Mostra as estruturas de pagina 
// usadas para paginação no pagedpool.
// Usada pelo alocador de páginas. 
// shared ring3 memory?
void mmShowPagedMemoryList(int max)
{
    struct page_d *p;
    register int i=0;

    if (max < 0 || max > 512){
        return;
    }

    for ( i=0; i<max; i++ )
    {
        p = (void *) pageAllocList[i]; 

        if ( (void *) p != NULL )
        {
            printk ("id=%d free=%d frameno=%d ref=%d \n", 
                p->id,            // Número do slot.
                p->free,          // ?
                p->absolute_frame_number,  // 
                p->ref_count );   // Quantos processos estão usando essa pag.
        }
    };
}  


/*
 * mmShowMemoryBlocksForTheKernelAllocator:
 *     IMPORTANTE.
 *     Mostra as informações das estruturas de memória. 
 *     Essas são as estruturas usadas pelo kmalloc.
 * @todo: 
 *     Mostrar a memória usada pelos processos.
 *     Mostrar o quanto de memória o processo usa.
 *     Importante: Esse tipo de rotina mereçe muita atenção
 * principalmente na fase inicial de construção do sistema.
 * Apresentar as informações em uma janela bem grande e 
 * chamar através do procedimento de janela do sistema é uma opção bem 
 * interessante.
 * Por enquanto apenas escrevemos provavelmente na janela como 
 * foco de entrada e o procedimento de janela do sistema efetua um 
 * refresh screen
 */

void mmShowMemoryBlocksForTheKernelAllocator(void)
{
// Print the information for each valid memory block
// in the ring0 kernel allocator.
// This memory was allocated inside the main kernel heap.

    struct mmblock_d *B;
    register int i = 0;

// Title
    printk("Memory Block Information:\n");

    // Mostra os heap da lista.
    while (i < MMBLOCK_COUNT_MAX) 
    {
        B = (void *) mmblockList[i];

        i++;
        if ( (void *) B != NULL )
        {
            if ( B->Used == 1 && B->Magic == 1234 )
            {
                printk ("Id={%d} Header={%x} userA={%x} Footer{%x}\n",
                    B->Id, B->Header, B->userArea, B->Footer );
            }
            //Nothing.
        }
        //Nothing.
    };

// Quantos slots estão sendo usados?
    //printk ("used slots: %d/%d\n",
    //    mmblockCount, MMBLOCK_COUNT_MAX );

// Aqui podemos aprentar informações sobre o heap.
// como tamanho, espaço disponível, ponteiro, à que processo ele 
// pertence.
// mas estamos lidando a estrtutura de mmblock_d, que é especial e meio 
// engessada.

    //More?!
}  

void mmTestingPageAlloc(void)
{
    // #todo
}

//
// End
//

