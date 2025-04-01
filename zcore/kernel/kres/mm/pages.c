// pages.c
// Memory manager.
// It maps the first portions of memory ised by the kernel
// and the init process.
// Routines valid for Intel x64 machines.
// Created by Fred Nora.


/*
mmInitializePaging():
    This is the main function here. The goal is mapping some 
2MB blocks of memory that is gonna be used by the kernel and 
the init process.

These are static system areas, do not change them.
// va=0          | Ring 0 area.
// va=0x00200000 | Ring 3 area.
// va=0x30000000 | kernel image region.
// va=0x30200000 | Frontbuffer. (LFB)
// va=0x30400000 | Backbuffer.
// va=0x30600000 | Paged pool.
// va=0x30800000 | Heap pool.
// va=0x30A00000 | Extra heap 1. (Used by the kernel module.)
// va=0x30C00000 | Extra heap 2. (Slab allocator)
// va=0x30E00000 | Extra heap 3. (Slab allocator too)

 */

#include <kernel.h>

// global
// The virtual address of the kernel pml4 table.
unsigned long gKernelPML4Address=0;

// ==================================================
// This is not global variable.
// We will use this only on this initialization here.
// This is architecture dependent.
// This variables are only for x86_64.

// small system
static unsigned long SMALL_origin_pa=0;
static unsigned long SMALL_kernel_base_pa=0;
static unsigned long SMALL_user_pa=0;
static unsigned long SMALL_cga_pa=0;
static unsigned long SMALL_frontbuffer_pa=0; 
static unsigned long SMALL_backbuffer_pa=0;
static unsigned long SMALL_pagedpool_pa=0;
static unsigned long SMALL_heappool_pa=0;
static unsigned long SMALL_extraheap1_pa=0;
static unsigned long SMALL_extraheap2_pa=0;
static unsigned long SMALL_extraheap3_pa=0;
// medium system
static unsigned long MEDIUM_origin_pa=0;
static unsigned long MEDIUM_kernel_base_pa=0;
static unsigned long MEDIUM_user_pa=0;
static unsigned long MEDIUM_cga_pa=0;
static unsigned long MEDIUM_frontbuffer_pa=0;
static unsigned long MEDIUM_backbuffer_pa=0;
static unsigned long MEDIUM_pagedpool_pa=0;
static unsigned long MEDIUM_heappool_pa=0;
static unsigned long MEDIUM_extraheap1_pa=0;
static unsigned long MEDIUM_extraheap2_pa=0;
static unsigned long MEDIUM_extraheap3_pa=0;
// large system
static unsigned long LARGE_origin_pa=0;
static unsigned long LARGE_kernel_base_pa=0;
static unsigned long LARGE_user_pa=0;
static unsigned long LARGE_cga_pa=0;
static unsigned long LARGE_frontbuffer_pa=0;
static unsigned long LARGE_backbuffer_pa=0;
static unsigned long LARGE_pagedpool_pa=0;
static unsigned long LARGE_heappool_pa=0;
static unsigned long LARGE_extraheap1_pa=0;
static unsigned long LARGE_extraheap2_pa=0;
static unsigned long LARGE_extraheap3_pa=0;

// ==================================================

//
// private functions:
//

static void __initialize_canonical_physical_regions(void);

static void __initialize_canonical_kernel_pagetables(void);

static void __initialize_ring0area(void);
static void __initialize_ring3area(void);
static void __initialize_kernelimage_region(void);
static void __initialize_frontbuffer(void);
static void __initialize_backbuffer(void);

static void __initialize_pagedpool(void);

static void __initialize_heappool(void);
static void __initialize_extraheap1(void);
static void __initialize_extraheap2(void);
static void __initialize_extraheap3(void);

static void __mmSetupMemoryUsage(void);

// Local worker
static unsigned long 
__virtual_to_physical ( 
    unsigned long virtual_address, 
    unsigned long pml4_va );

// ==================================================

// Vamos criar uma pagetable com 512 entradas
// para mapearmos uma região da memória física.
// Em seguida instalar nas tabelas passadas via argumento.
// Os endereços virtuas das tabelas passados via parâmetro
// pertencem ao pml4 do kernel e nos permite acessar essas tabelas
// para configurarmos elas.
// #todo: This is a work in progress.
// #bugbug: 
// É uma rotina muito ruim, se possível substituir por algo melhor.

void *CreateAndIntallPageTable (
    unsigned long pml4_va,   // page map level 4
    unsigned long pml4_index,
    unsigned long pdpt_va,   // page directory pointer table
    unsigned long pdpt_index,
    unsigned long pd_va,     // page directory 
    int pd_index,            // Install the pagetable into this entry of the page directory. 
    unsigned long region_pa )
{
    panic("CreateAndIntallPageTable: suspended\n");
    return NULL;


/*
    int i=0;

    // #todo
    // Vamos criar uma pagetable com 512 entradas
    // para mapearmos uma região da memória física.

    debug_print("CreateAndIntallPageTable: [TODO] NOT TESTED YET\n");


// =========================
    if ( pml4_va == 0 ){
        debug_print("CreateAndIntallPageTable: [FAIL] Invalid pml4_va\n");
        return NULL;
    }
    if ( pml4_index <0 || pml4_index >= 512 )
    {
        debug_print("CreateAndIntallPageTable: [FAIL] Invalid pml4_index\n");
        return NULL;
    }

// ============================
    if ( pdpt_va == 0 ){
        debug_print("CreateAndIntallPageTable: [FAIL] Invalid pdpt_va\n");
        return NULL;
    }
    if ( pdpt_index <0 || pdpt_index >= 512 )
    {
        debug_print("CreateAndIntallPageTable: [FAIL] Invalid pdpt_index\n");
        return NULL;
    }

// ============================
    if ( pd_va == 0 ){
        debug_print("CreateAndIntallPageTable: [FAIL] Invalid pd_va\n");
        return NULL;
    }
    if ( pd_index <0 || pd_index >= 512 )
    {
        debug_print("CreateAndIntallPageTable: [FAIL] Invalid pd_index\n");
        return NULL;
    }


// ============================

    // Não queremos mapear o início da memória física.
    if ( region_pa == 0 ){
        panic ("CreateAndIntallPageTable: [FAIL] region_pa\n");
    }


//
// Page table
//

    unsigned long ptVA = (unsigned long) get_table_pointer_va();  //ok
    if ( ptVA == 0 ){
        panic ("CreateAndIntallPageTable: [FAIL] ptVA\n");
    }

    // Vamos mapear uma região de memória 
    // preenchendo a nossa page table recem criada.

    // Essa rotina preenche uma pagetable, mapeando
    // a região indicada.

    mm_fill_page_table( 
        (unsigned long) pd_va,      // directory va. 
        (int) pd_index,             // directory entry.
        (unsigned long) ptVA,       // page table va.
        (unsigned long) region_pa,  // Region 2mb pa.
        (unsigned long) 7 );        // flags.


//
// Install
//

// Vamos instalar a pagetable no diretorio de páginas.
// Antes vamos pegar o endereço físico da page table.
// Pois precisamos desse endereço pra colocar no diretório de páginas.

    // Nessa conversão precisamos usar o pml4 do kernel.
//
// Installing
//


    unsigned long *PageTable                 = (unsigned long *) ptVA;
    unsigned long *PageDirectory             = (unsigned long *) pd_va;
    unsigned long *PageDirectoryPointerTable = (unsigned long *) pdpt_va;
    unsigned long *PML4                      = (unsigned long *) pml4_va;

//
// #debug
//
    //printk("\n");
    //printk (" pt va   : %x\n",PageTable);
    //printk (" pd va   : %x\n",PageDirectory);
    //printk (" pdpt va : %x\n",PageDirectoryPointerTable);
    //printk (" pml4 va : %x\n",PML4);
    //refresh_screen();
    //while(1){}


    unsigned long __ptPA = (unsigned long) virtual_to_physical ( 
                                             PageTable, 
                                             gKernelPML4Address ); 


    unsigned long __pdPA = (unsigned long) virtual_to_physical ( 
                                             PageDirectory, 
                                             gKernelPML4Address ); 
                                             

    unsigned long __pdptPA = (unsigned long) virtual_to_physical ( 
                                             PageDirectoryPointerTable, 
                                             gKernelPML4Address ); 

 

//
// #debug
//
    //printk("\n");
    //printk (" pt pa   : %x\n",__ptPA);
    //printk (" pd pa   : %x\n",__pdPA);
    //printk (" pdpt pa : %x\n",__pdptPA);
    //refresh_screen();
    //while(1){}
 
    if ( __ptPA == 0 ){
        panic ("CreateAndIntallPageTable: __ptPA\n");
    }

    if ( __pdPA == 0 ){
        panic ("CreateAndIntallPageTable: __pdPA\n");
    }

    if ( __pdptPA == 0 ){
        panic ("CreateAndIntallPageTable: __pdptPA\n");
    }

    //============================
    // Page Directory
    // Instalando o ponteiro para a pagetable entrada do diretório.
    PageDirectory[pd_index] = (unsigned long) __ptPA;
    PageDirectory[pd_index] = (unsigned long) PageDirectory[pd_index] | 7; 


    //============================
    // Page Directory Pointer Table
    PageDirectoryPointerTable[pdpt_index] = (unsigned long) __pdPA;
    PageDirectoryPointerTable[pdpt_index] = (unsigned long) PageDirectoryPointerTable[pdpt_index] | 7; 


    //============================
    // PML4
    PML4[pml4_index] = (unsigned long) __pdptPA;
    PML4[pml4_index] = (unsigned long) PML4[pml4_index] | 7; 

    
    // Retornando o endereço virtual da pt criada.
    return (void*) ptVA;

    //panic ("CreateAndIntallPageTable: [TODO] This is a work in progress.\n");
    // #todo retorno.
    //return NULL;
    
    */
}



// #todo
// Describe this thing.
unsigned long get_new_frame (void)
{
    int i=0;

    panic("get_new_frame: [FIXME] This is a work in progress\n");

/*
    if (FT.initialized != TRUE){
        panic("get_new_frame: FT.initialized\n");
    }

// Limits
    if (FT.start_va == 0 || FT.end_va == 0 ){
        panic("get_new_frame: limits\n");
    }
*/

//fail;
    return 0;
}


// Wrapper
unsigned long alloc_frame(void)
{
    panic("alloc_frame: [FIXME] This is a work in progress\n");
    return (unsigned long) get_new_frame();
}


// ===================================================================
//
// #important: DANGER !!!
//
// get_table_pointer_va:
// #bugbug
// Isso eh um improviso, precisamos de outro endereço.
// >>>> 0x1000
// O sistema esta usando esse endereço como inicio de um heap
// onde pegamos paginas (frames?) de memoria para criarmos diretorios de paginas.
// Isso porque precisamos de endereços que terminem com pelo menos 
// 12 bits zerados.
// #todo: 
// Precisamos encontrar outro lugar para esse heap, 
// tendo em vista que o numero de diretorios criados sera grande 
// e o heap invadir outras areas.
// #bugbug
// Vamos improvisar um limite por enquanto.
// See: mm/gpa.h
// 0x1000

unsigned long table_pointer_heap_base = 
    ____DANGER_TABLE_POINTER_HEAP_BASE;

// ======================================================

// Isso serve pra pegarmos um endereço físico
// que servira de base para criarmos uma pagetable.
// Mas endereço físico e virtual são iguais nessa região.
// Identidade 1:1.

unsigned long get_table_pointer_va(void)
{
    //debug_print ("get_table_pointer_va:\n");

// Incrementa 4KB.
    table_pointer_heap_base = 
        (unsigned long) (table_pointer_heap_base + 0x1000);

// #todo
// Precisamos de uma nova origem.
// Os primeiros 12bits precisam ser '0'.
// VM_BASE = 0x000B8000.
// #bugbug
// Todo o espaço entre 0x1000 e 0x000B8000 esta livre ?
// Onde estao a fat e o root dir?
// MBR_ADDRESS = 0x20000. /
// Esse o o endereço mais baixo entre os endereços usados
// pelo sistema de arquivos.


// Reach the max limit.
// 0x20000
    //if (table_pointer_heap_base >= MBR_ADDRESS)
    if (table_pointer_heap_base >= FAT_ADDRESS)
    {
        //0x00030000
        panic ("get_table_pointer_va: [FIXME] Limits\n");
    }

    return (unsigned long) table_pointer_heap_base;
}


void *CloneKernelPDPT0(void)
{
    register int i=0;
    unsigned long destAddressVA=0; 

// Pegamos um edereço que serve de base para
// criarmos um pdpt.
// Identidade 1:1.

    destAddressVA = (unsigned long) get_table_pointer_va(); 
    if ( destAddressVA == 0 ){
        panic ("CloneKernelPDPT0: destAddressVA\n");
    }

// The virtual address of the kernel page directory and
// the virtual address of the new page directory.
// #bugbug: What directory we are using right now? kernel?

    unsigned long *src = (unsigned long *) kernel_mm_data.pdpt0_va; //gKernelPML4Address;
    unsigned long *dst = (unsigned long *) destAddressVA;

// Copy
    for ( i=0; i < 512; ++i ){
        dst[i] = (unsigned long) src[i];
    };

// Done.
// The virtual address of the new pml4. 
    return (void *) destAddressVA;
}


void *CloneKernelPD0(void)
{
    register int i=0;
    unsigned long destAddressVA=0; 

// Pegamos um edereço que serve de base para
// criarmos um pd.
// Identidade 1:1.

    destAddressVA = (unsigned long) get_table_pointer_va();
    if ( destAddressVA == 0 ){
        panic ("CloneKernelPD0: destAddressVA\n");
    }

// The virtual address of the kernel page directory and
// the virtual address of the new page directory.
// #bugbug: What directory we are using right now? kernel?

    unsigned long *src = (unsigned long *) kernel_mm_data.pd0_va; //gKernelPML4Address;
    unsigned long *dst = (unsigned long *) destAddressVA;  

// Copy
    for ( i=0; i < 512; ++i ){
        dst[i] = (unsigned long) src[i];
    };

// The virtual address of the new pml4. 
    return (void *) destAddressVA;
}


/*
 * CloneKernelPML4:
 *    Clone the kernel pml4.
 *    OUT: The virtual address of the new directory.
 */

// OUT: va
void *CloneKernelPML4(void)
{
    register int i=0;
    unsigned long destAddressVA=0; 

// Pegamos um edereço físico que serve de base para
// criarmos um pml4.
// Mas endereço físico e virtual são iguais nessa região.
// Identidade 1:1.

    destAddressVA = (unsigned long) get_table_pointer_va(); 
    if (destAddressVA == 0){
        panic ("CloneKernelPML4: destAddressVA\n");
    }

// The virtual address of the kernel page directory and
// the virtual address of the new page directory.
// #bugbug: 
// What directory we are using right now? kernel?

    unsigned long *src = (unsigned long *) gKernelPML4Address;
    unsigned long *dst = (unsigned long *) destAddressVA;  

// Copy
    for ( i=0; i < 512; ++i ){
        dst[i] = (unsigned long) src[i];
    };

// Done.
// The virtual address of the new pml4. 
    return (void *) destAddressVA;
}


/*
 * clone_pml4:
 *     Clone a given page directory.
 */

// Clona um pml4 dado seu endereço.
// Queremos clonar o diretório atual,
// para que o processo filho tenha o mesmo diretório do processo pai. 
// #??
// Esse endereço virtual eh valido?
// Pertence ao diretorio que estamos usando no momento?
// #todo
// clone_pml4
// OUT:
// The address of the new pml4.

void *clone_pml4(unsigned long pml4_va)
{
    register int i=0;
    unsigned long destAddressVA=0; 

// #test
// no directory in the address '0'

    if ( pml4_va == 0 ){
        panic("clone_pml4: pml4_va\n");
    }

// Get a target address for the directory.
// #bugbug:
// We are using that routine to get a poiter for a table.
// Is that a virtual address ?
// What about the size?
// Pegamos um edereço que serve de base para
// criarmos um pml4.
// Identidade 1:1.

    destAddressVA = (unsigned long) get_table_pointer_va();
    if (destAddressVA == 0){
        panic ("clone_pml4: destAddressVA\n");
    }

// Initialization
    unsigned long *src = (unsigned long *) pml4_va;
    unsigned long *dst = (unsigned long *) destAddressVA;

// Copy
    for ( i=0; i < 512; ++i ){
        dst[i] = (unsigned long) src[i]; 
    };

// The address of the new pml4.
    return (void *) destAddressVA;
}


/*
// #test
// Worker
int __findhole_in_pml4(unsigned long pml4_va);
int __findhole_in_pml4(unsigned long pml4_va)
{
// IN: The va for the pml4.
// OUT: Return the index for the empty spot.

    register int i=0;
    int Max=512; //#todo: Use definition.
    unsigned long *base = (unsigned long *) pml4_va;


// #bugbug
// #todo
// Check valid va for pml4.
// There is a lot if invalid address to put a pml4.
// #todo: Actually we need to put them all into a 
// single big place. It's gonna be easier to track
// the validation.

    if (pml4_va == RING0AREA_VA)
        goto fail;
    if (pml4_va == RING3AREA_VA)
        goto fail;
    if (pml4_va == KERNELIMAGE_VA)
        goto fail;
    // ...   

    for (i=0; i<Max; i++)
    {
        if (base[i] == 0)
        {
            // Return the index for the empty spot.
            return (int) i;
        }    
    };
fail:
    return (int) -1;
}
*/


// #Importante
// Agora vamos calcular a quantidade de memória física usada 
// até agora.
// Levando em conta a inicialização que fizemos nessa rotina.
// Estamos deixando de fora a memória dos dispositivos, pois a 
// memória usada pelos dispositivos possuem endereço físico, 
// mas está na parte alta do endereçamento físico, 
// muito além da memória RAM instalada.
// Com a exceção da vga, que fica antes de 1MB.
// Os dispositivos por enquanto são memória de vídeo e 
// placa de rede.
// Tem a questão do dma a se considerar também.
// Tem dma abaixo da marca de 16mb.
// Tem dma que usa memória virtual.
// Used.
// #todo: mm_used_lfb ??

static void __mmSetupMemoryUsage(void)
{
// Local worker.
// Called by mmInitializePaging().

    //debug_print("__mmSetupMemoryUsage: Setup memory usage\n");

// used memory:
    memorysizeUsed = 
        (unsigned long) ( 
        mm_used_ring0_area +  
        mm_used_ring3_area +  
        mm_used_kernelimage +
        mm_used_backbuffer + 
        mm_used_pagedpool + 
        mm_used_heappool + 
        mm_used_extraheap1 + 
        mm_used_extraheap2 + 
        mm_used_extraheap3 +
        mm_used_frame_table 
        );

// free memory
    memorysizeFree = 
        (unsigned long) (memorysizeTotal - memorysizeUsed);


// memorysizeFree  is the size in KB.
// memorysizeTotal is the size in KB.
// memorysizeUsed  is the size in KB.
// #todo:
// We need to change these names including KB at the end of them.
// ex: memorysizeTotal_KB

}

// Checar se a estrutura de página é nula
// This is very ugly
int isValidPageStruct(struct page_d *p)
{
    return (p == NULL) ? TRUE : FALSE;
}

// Selecionar a página como livre.
void freePage (struct page_d *p)
{
    if ((void*) p == NULL)
    {
        // #debug ?
        return;  
    }

    // Free it!
    if ( p->used == TRUE && p->magic == 1234 )
    {
        p->free = TRUE;
    }
}

// Selecionar a pagina como nao livre.
void notfreePage(struct page_d *p)
{
    if ((void*) p == NULL)
    {
        // #debug ?
        return; 
    }
 
    // Not free!
    if ( p->used == 1 && p->magic == 1234 ){ p->free = 0; }
}

// Worker
static unsigned long 
__virtual_to_physical ( 
    unsigned long virtual_address, 
    unsigned long pml4_va ) 
{

    //debug_print ("__virtual_to_physical: [TESTING] \n");

    //#debug
    //printk("virtual_address = %x \n",virtual_address);
    //printk("pml4_va = %x \n",pml4_va);
    //refresh_screen();
    //while(1){}

    if (virtual_address == 0){
        debug_print ("__virtual_to_physical: [?] virtual_address == 0 \n");
    }

// Why is it limited to 'int' size?

    unsigned int a = (unsigned int) ((virtual_address >> 39) & 0x1FF);  // 9 bits de pml4
    unsigned int b = (unsigned int) ((virtual_address >> 30) & 0x1FF);  // 9 bits de pdpt
    unsigned int d = (unsigned int) ((virtual_address >> 21) & 0x1FF);  // 9 bits de page directory
    unsigned int t = (unsigned int) ((virtual_address >> 12) & 0x1FF);  // 9 bits de page table. 
    unsigned int o = (unsigned int) (virtual_address         & 0xFFF);  // 12 bits de offset

    unsigned long tmp=0;
    unsigned long address=0;

    //printk ("a=%d b=%d d=%d t=%d o=%d \n",a,b,d,t,o);
    //refresh_screen();
    //while(1){}

    //offset limits
    //1111 1111 1111
    //FFF

// #debug
// In 64bit each table has only 512 entries.
// With 64KB pages, an offset can't have more than 4096 bytes.
    if ( a >= 512 || b >= 512 || d >= 512 || t >= 512 || 
         o >= 4096  )
    {
        printk("__virtual_to_physical: entry limits\n");
        printk("a=%d\n",a);
        printk("b=%d\n",b);
        printk("d=%d\n",d); //directory
        printk("t=%d\n",t); //page table
        printk("o=%d\n",o); //offset 4096 bytes limit
        refresh_screen();
        while (1){
        };
    }

// #todo
// Por enquanto estamos usando apenas as entradas '0'
// de pml4 e pdpt ... mas depois vamos usar todas.

    // #hackhack
    if (a != 0){
        printk ("__virtual_to_physical: [TODO] a != 0 \n");
        refresh_screen();
        while (1){
        };
        //return;
    }

    // #hackhack
    if (b != 0){
        printk ("__virtual_to_physical: [TODO] b != 0 \n");
        refresh_screen();
        while (1){
        };
        //return;
    }

    if (pml4_va == 0){
        debug_print ("__virtual_to_physical: [?] pml4_va == 0 \n");
        panic       ("__virtual_to_physical: [FAIL] Invalid pml4_va\n");
    }

// ==============================
// pml4
    //debug_print ("virtual_to_physical2: [pml4]\n");
    unsigned long *pml4VA = (unsigned long *) pml4_va;

    //printk (">> pml4VA[a] %x\n", pml4VA[a]);
    //refresh_screen();

    // Temos o pdpt junto com suas flags.
    tmp = (unsigned long) pml4VA[a];

// ==============================
// page directory pointer table.
    //debug_print ("virtual_to_physical2: [ptpt]\n");
    unsigned long *ptpt = (unsigned long *) (tmp & 0xFFFFFFFFF000);

    //printk (">> ptpt[d] %x\n", ptpt[b]);
    //refresh_screen();

    // Temos o pd junto com suas flags.
    tmp = (unsigned long) ptpt[b];

// ==============================
// page diretory
    //debug_print ("virtual_to_physical2: [dir]\n");
    unsigned long *dir = (unsigned long *) (tmp & 0xFFFFFFFFF000);

    //printk ("dir[d] %x\n", dir[d]);
    //refresh_screen();
    
    // Temos o endereço da pt junto com as flags.
    tmp = (unsigned long) dir[d];

    // Page table.
    //debug_print ("virtual_to_physical2: [pt]\n");
    unsigned long *pt = (unsigned long *) (tmp & 0xFFFFFFFFF000);

    // Encontramos o endereço base do page frame.
    tmp = (unsigned long) pt[t];

    address = (tmp & 0xFFFFFFFFF000);

    //debug_print ("virtual_to_physical2: done\n");

// OUT:
// Physical address.
    return (unsigned long) (address + o);
}

// IN:
//   + virtual_address
//   + pml4_va
// OUT:
//   + physical address
unsigned long 
virtual_to_physical ( 
    unsigned long virtual_address, 
    unsigned long pml4_va ) 
{
    return (unsigned long) __virtual_to_physical(virtual_address,pml4_va);
}

// Esta alinhado à página.
// #todo: Explain it better.
int mm_is_page_aligned_va(unsigned long va)
{
    if ( (va & 0xFFF) == 0 ){
        return TRUE;
    }
    return FALSE;
}

// #todo
void pages_calc_mem(void)
{
    int a=0;
    int b=0;
    int i=0;
    int j=0;
    int k=0;
    int free=0;

    unsigned long *pml4 = (unsigned long *) gKernelPML4Address;  //pa
    unsigned long *pdpt;
    unsigned long *pg_dir;
    unsigned long *pg_tbl;

    printk ("\n\n");

    //for(i=0 ; i<PAGING_PAGES ; i++)
    //    if (!mem_map[i]) free++;
    //printk("%d pages free (of %d)\n\r",free,PAGING_PAGES);


    for (a=0; a<512; a++)
    {
        if ( pml4[a] & 1 )
        {
            pdpt = (unsigned long *) (0xFFFFFFFFF000 & pml4[a]);
            for (b=0; b<512; b++)
            {
                if ( pdpt[b] & 1 )
                {
                    pg_dir = (unsigned long *) (0xFFFFFFFFF000 & pdpt[b]);
                    for (i=0; i<512; i++)
                    {
                        if ( pg_dir[i] & 1 )    //if (1 & pg_dir[i])
                        {
                            pg_tbl = (unsigned long *) (0xFFFFFFFFF000 & pg_dir[i]);

                            // ugly
                            for ( j=k=0; j<512; j++ )
                            {
                                if (pg_tbl[j] & 1){ k++; }
                            };
                            printk ("%d|%d:  dir[%d]  uses  %d  pages\n",a,b,i,k);
                        }
                    };
                }
            };
        }
    };

    refresh_screen();
}

// local
// Precisa ser um endereço físico.
// 64bit?
void load_pml4_table(void *phy_addr)
{
    asm volatile ("movq %0, %%cr3"::"r"(phy_addr));
}

// PAE and PGE
// inline?
//void __enable_pae(void);
void __enable_pae(void)
{
    // PAE and PGE.
    asm volatile ( " movq %%cr4, %%rax; "
                   " orl $0x20,  %%eax; "
                   " movq %%rax, %%cr4  " :: );
}

// inline?
void page_enable(void)
{
    // Only paging.
    asm volatile (" movq %%cr0, %%rax;      "
                  " orl $0x80000000, %%eax; "
                  " movq %%rax, %%cr0       " :: );
}

/*
// 8 pagetables
// #bugbug: IT IS NOT WORKING!
// #todo: THIS IS A WORK IN PROGRESS
void *mm_allocate_memory_for_new_paged_pool(void);
void *mm_allocate_memory_for_new_paged_pool(void)
{
    //#test: mapeando varias pagetables.
    //cada pagetable aloca 2mb.

    // 16 mb começando em 200mb mark.
    unsigned long region_start_pa = (200*1024*1024);

    void *first_va_allocated_for_pts;
    
// alocando memoria virtual
// onde colocaremos 8 pagetables.

    first_va_allocated_for_pts = (void *) allocPages(8);
    if ((void*) first_va_allocated_for_pts == NULL )
    {
        //fail
        return NULL;
    }
    
    //IN:
    mm_fill_n_pagetables(
        KERNEL_PD_PA, // va do pd do kernel
        400,          // directory first entry of a sequence
        8,            // number of entries in the dir.
        first_va_allocated_for_pts, // first pt va of a sequence.
        region_start_pa,     // region start pa of a sequence
        8*(2*1024*1024),  // quantos megas na area alocada.
        7 );              // flags para user code. 

    //#todo: retornar o endereço referente
    //à primeira entrada.
    // (400 * (2*1024*1024)
    // 2mb por entrada.
    
    // o endereço virtual onde começa a área alocada.
    // no espaço de endereçamento do kernel.
    unsigned long base_va = (400 * (2*1024*1024));

    //flush the current pml4..
    //#bugbg: mas precisa ser o do kernel
    //entao precisamos chamar essa rotina na inicializaçao do kernel.
    asm ("movq %cr3, %rax");
    asm ("movq %rax, %cr3");
    
    return (void*) base_va;
}
*/



/*
//
// IN:
//
// directory_va: 
//     Endereço virtual do diretório de páginas
//     onde serão colocadas 'n' entradas que apontam 
//     para n pagetables. 
//
// directory_first_entry: 
//     Índice da primeira entrada no diretorio indicado,
//     Essa é a primeira de uma sequencia de n entradas.
//
// number_of_entries:
//     Numero de entadas que iremos criar,começando
//     da primeira indicada logo acima.
//
// first_pt_va:
//     Endereço virtual da primeira tabela de páginas.
//     Lembrando que as outras pagetables devem
//     ser subsequentes a essa. Cada uma tem 4kb.
//
// region_start_pa: 
//     Endereço físico do inicio da região que queremos
// mapear. O tamanho dessa região deve estar de acordo
// com a quantidade de tabelas que queremos criar.
// cada tabela consegue mapear 2mb.
//
// region_size_in_mb:
//     tamanho da regiao dado em mb.
//     usado pra conferir se os dados acima estao consistentes
//     ajudar na contabilidade.
//
// flags:
//     As flags usadas em todas as entradas da pagetable
//     e na entrada do diretório de páginas.

// #todo: THIS IS A WORK IN PROGRESS
int 
mm_fill_n_pagetables( 
    unsigned long directory_va, 
    int           directory_first_entry,
    int           number_of_entries,
    unsigned long first_pt_va,
    unsigned long region_start_pa,
    int           region_size_in_mb,
    unsigned long flags );

int 
mm_fill_n_pagetables( 
    unsigned long directory_va, 
    int           directory_first_entry,
    int           number_of_entries,
    unsigned long first_pt_va,
    unsigned long region_start_pa,
    int           region_size_in_mb,
    unsigned long flags )
{

// o pd do kernel tem endreços fisico e virtual iguais.
    //unsigned long pd_pa = directory_va;
    unsigned long pd_pa = KERNEL_PD_PA;
    
// entries iterator
    int e  =directory_first_entry;
    int max=number_of_entries;


// #bugbug: Talvez esses endereços não sejam válidos 
// para fazermos isso.
// #temos que criar um array de endereços de pagetables
// e esses endereços serão usados pra essa alocação.
    unsigned long pt_va = first_pt_va;
    unsigned long address = region_start_pa;


    for( e = directory_first_entry;
         e < number_of_entries;
         e++ )
    {
        mm_fill_page_table( 
            (unsigned long) pd_pa,         // pd 
            (int) e,            // entry
            (unsigned long) pt_va,    // pt
            (unsigned long) address,  // region base 
            (unsigned long) flags );  // flags
        
        // proximos 2mb.
        address = address + (2*1024*1024);
        pt_va   = pt_va   + (4*1024);  //
    }
    
    return 0;
}
*/


// ---------------------------
// mm_fill_page_table:
// Cria uma page table com 512 entradas
// para uma região de 2mb e configura uma
// determinada entrada no diretório de páginas indicado.
// IN:
// directory_va: 
//     Endereço virtual do diretório de páginas.
// directory_entry: 
//     Índice da entrada no diretório indicado.
// pt_va:
//     Endereço virtual da tabela de páginas.
// region_2mb_pa: 
//     Endereço físico da região de 2MB que queremos mapear.
// flags:
//     As flags usadas em todas as entradas da pagetable
//     e na entrada do diretório de páginas.

int 
mm_fill_page_table( 
    unsigned long directory_va, 
    int           directory_entry,
    unsigned long pt_va,
    unsigned long region_2mb_pa,
    unsigned long flags )
{
    register int i=0;
    unsigned long *dir = (unsigned long *) directory_va;
    unsigned long *pt  = (unsigned long *) pt_va;
    unsigned long pa   = (unsigned long) region_2mb_pa;

// #todo
// We need to validate some limits here.

// Validation
    if ( directory_entry < 0 || 
         directory_entry >= 512 )
    {
        return -1;
    }

// Fill the pagetable with 512 entries.
    for ( i=0; i<512; ++i )
    {
        pt[i] = (unsigned long) (pa | flags);
        pa    = (unsigned long) (pa + 4096);
    };

// Create a directory entry in the given index.
    dir[directory_entry] = 
        (unsigned long) pt_va;  //&pt[0];
    dir[directory_entry] = 
        (unsigned long) (dir[directory_entry] | flags);

    return 0;
}


// local worker
// Inicializando as variáveis de contagem de uso de memória ram.
// Talvez ja tenhamos feito isso antes, mas não tem problema.
void __initialize_ram_usage_varables(void)
{
// Called by mmInitializePaging().

// The whole range.
    memorysizeUsed = 0;
    memorysizeFree = 0;
    memorysizeUsed = 0;

// Components.
    mm_used_ring0_area = 0;
    mm_used_ring3_area = 0; 
    mm_used_kernelimage = 0;
    mm_used_backbuffer = 0; 
    mm_used_pagedpool = 0; 
    mm_used_heappool = 0;
    mm_used_extraheap1 = 0;
    mm_used_extraheap2 = 0; 
    mm_used_extraheap3 = 0; 
    mm_used_frame_table = 0;
}

static void __initialize_canonical_physical_regions(void)
{
// Called by mmInitializePaging().

// small systems
    SMALL_origin_pa      = (unsigned long) SMALLSYSTEM_ORIGIN_ADDRESS;
    SMALL_kernel_base_pa = (unsigned long) SMALLSYSTEM_KERNELBASE;
    SMALL_user_pa        = (unsigned long) SMALLSYSTEM_USERBASE;
    SMALL_cga_pa         = (unsigned long) SMALLSYSTEM_CGA;
    SMALL_frontbuffer_pa = (unsigned long) gSavedLFB;                    //frontbuffer // #todo: precisamos que o bl passe  endereço físico para mapearmos o lfb.
    SMALL_backbuffer_pa  = (unsigned long) BACKBUFFER_PA;       //backbuffer
    SMALL_pagedpool_pa   = (unsigned long) SMALLSYSTEM_PAGEDPOLL_START;  //PAGED POOL
    SMALL_heappool_pa    = (unsigned long) SMALLSYSTEM_HEAPPOLL_START;
    SMALL_extraheap1_pa  = (unsigned long) SMALLSYSTEM_EXTRAHEAP1_START;
    SMALL_extraheap2_pa  = (unsigned long) SMALLSYSTEM_EXTRAHEAP2_START;
    SMALL_extraheap3_pa  = (unsigned long) SMALLSYSTEM_EXTRAHEAP3_START;

// medium systems
    MEDIUM_origin_pa      = (unsigned long) MEDIUMSYSTEM_ORIGIN_ADDRESS;
    MEDIUM_kernel_base_pa = (unsigned long) MEDIUMSYSTEM_KERNELBASE;
    MEDIUM_user_pa        = (unsigned long) MEDIUMSYSTEM_USERBASE;
    MEDIUM_cga_pa         = (unsigned long) MEDIUMSYSTEM_CGA;
    MEDIUM_frontbuffer_pa = (unsigned long) gSavedLFB; // #todo: precisamos que o bl passe  endereço físico para mapearmos o lfb.
    MEDIUM_backbuffer_pa  = (unsigned long) BACKBUFFER_PA;
    MEDIUM_pagedpool_pa   = (unsigned long) MEDIUMSYSTEM_PAGEDPOLL_START;
    MEDIUM_heappool_pa    = (unsigned long) MEDIUMSYSTEM_HEAPPOLL_START;
    MEDIUM_extraheap1_pa  = (unsigned long) MEDIUMSYSTEM_EXTRAHEAP1_START;
    MEDIUM_extraheap2_pa  = (unsigned long) MEDIUMSYSTEM_EXTRAHEAP2_START;
    MEDIUM_extraheap3_pa  = (unsigned long) MEDIUMSYSTEM_EXTRAHEAP3_START;

// large systems
    LARGE_origin_pa      = (unsigned long) LARGESYSTEM_ORIGIN_ADDRESS;
    LARGE_kernel_base_pa = (unsigned long) LARGESYSTEM_KERNELBASE;
    LARGE_user_pa        = (unsigned long) LARGESYSTEM_USERBASE;
    LARGE_cga_pa         = (unsigned long) LARGESYSTEM_CGA;
    LARGE_frontbuffer_pa = (unsigned long) gSavedLFB; // #todo: precisamos que o bl passe  endereço físico para mapearmos o lfb.
    LARGE_backbuffer_pa  = (unsigned long) BACKBUFFER_PA;
    LARGE_pagedpool_pa   = (unsigned long) LARGESYSTEM_PAGEDPOLL_START;
    LARGE_heappool_pa    = (unsigned long) LARGESYSTEM_HEAPPOLL_START;
    LARGE_extraheap1_pa  = (unsigned long) LARGESYSTEM_EXTRAHEAP1_START;
    LARGE_extraheap2_pa  = (unsigned long) LARGESYSTEM_EXTRAHEAP2_START;
    LARGE_extraheap3_pa  = (unsigned long) LARGESYSTEM_EXTRAHEAP3_START;
}


// ----------------------

// local worker
// Primeiros 2MB.  0 ~ 0x1FFFFF
// 0virt
// RING0AREA_VA
//   0 = Primeiros 2MB da memória RAM.
// mm_used_ring0_area = (1024 * 1);  //1mb, pois seremos sobrepostos pela imagem do kernel.  
// mm_used_ring0_area = (1024 * 2);  
// kernel_address_pa = 0h;
// (0fis = 0virt)
// Essa é a área do kernel. Começa no início da memória RAM
// e possui 2MB de tamanho.
// #importante
// Essa primeira entrada esta funcionando.
// Conseguimos usar essa identidade 1:1,
// tanto aqui no bl, quanto no kernel.
// #todo: Essa variável salva a quantidade de memória
// usada por essa área.
// (2 MB).
// Criamos a pagetable.
// Criando a primeira entrada do diretório.
// Isso mapeia os primeiros 2MB da memória RAM em ring0.
// SMALL_origin_pa = kernel_address_pa;

static void __initialize_ring0area(void)
{
    unsigned long *pt_ring0area = 
        (unsigned long *) get_table_pointer_va();  //PAGETABLE_RING0AREA; 

// kernel_address_pa: 
// Início da memória RAM.

// pa
    unsigned long kerneladdress_pa = (unsigned long) SYSTEM_ORIGIN;
// va
    g_ring0area_va = (unsigned long) RING0AREA_VA;
// pdindex
    int pdindex = (int) X64_GET_PDE_INDEX(RING0AREA_VA);
// size
// 1MB, pois seremos sobrepostos pela imagem do kernel.  
    mm_used_ring0_area = (1024 * 1);  

    // kernel_address_pa = 0h;
    // (0fis = 0virt)
    // Essa é a área do kernel. Começa no início da memória RAM
    // e possui 2MB de tamanho.

// #importante
// Essa primeira entrada esta funcionando.
// Conseguimos usar essa identidade 1:1,
// tanto aqui no bl, quanto no kernel.
// #todo: Essa variável salva a quantidade de memória
// usada por essa área.
// (2 MB).
// Criamos a pagetable.
// Criando a primeira entrada do diretório.
// Isso mapeia os primeiros 2MB da memória RAM em ring0.
// SMALL_origin_pa = kernel_address_pa;

// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,       // pd 
        (int) pdindex,                      // entry
        (unsigned long) &pt_ring0area[0],   // pt
        (unsigned long) kerneladdress_pa,   // region base
        (unsigned long) ( PAGE_WRITE | PAGE_PRESENT ) );  // flags=3
}


// local worker
// Uma área em user mode. 0x00200000 ~ 0x003FFFFF
// 0x00200000vir - Começa na marca de 32mb fis.
// RING3AREA_VA
//   1 = Area em user mode que começa em 32MB da memória física.
// mm_used_ring3_area = (1024 * 2);  //2mb
// user_address_pa = 0x02000000
// 32MB mark
// 0x02000000pys = 0x00200000vir  ?? 
// Essa é uma área em user mode
// (2 MB).
// Criamos a pagetable.
// Criando a entrada número 1 do diretório.
// Isso mapeia 2 MB de memória em user mode.
// SMALL_user_pa = user_address_pa
// user_address_pa = 0x02000000
// 32MB mark
// 0x02000000pys = 0x00200000vir  ?? 
// Essa é uma área em user mode
// size = (2 MB).

static void __initialize_ring3area(void)
{
// #todo
// Pra que essa area esta sendo usada?

    unsigned long *pt_ring3area = 
        (unsigned long *) get_table_pointer_va();

// pa
// user_address_pa:   
// User area, (32MB mark) 0x02000000.
    unsigned long useraddress_pa = (unsigned long) USER_BASE_PA;
// va
    g_ring3area_va = (unsigned long) RING3AREA_VA;
// pd index
    int pdindex = (int) X64_GET_PDE_INDEX(RING3AREA_VA);
// size
    mm_used_ring3_area = (1024 * 2);  //2mb

// Criamos a pagetable.
// Criando a entrada número 1 do diretório.
// Isso mapeia 2 MB de memória em user mode.
// SMALL_user_pa = user_address_pa

// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,      // pd 
        (int) pdindex,                     // entry
        (unsigned long) &pt_ring3area[0],  // pt
        (unsigned long) useraddress_pa,    // region base
        (unsigned long) ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT ) );  // flags=7
}


// local worker
// A imagem do kernel.  0x30000000 ~ 0x301FFFFF
// 0x30000000virt
// KERNELIMAGE_VA
// 384 = Imagem do kernel que começa em 1MB da memória física.
// mm_used_kernelimage = (1024 * 2);  //2mb
// kernel_base_pa = 0x100000pys
// (0x100000pys = 0x30000000virt).
// Configurando a área de memória onde ficará a imagem do kernel.
// Isso mapeia 2MB começando do primeiro mega. 
// (kernel mode).
// Preenchendo a tabela pt_ring0area.
// 'kernel_base_pa' é o endereço físico da imagem do kernel.
// Criamos a pagetable.
// Criamos a entrada 384 apontando para a pagetable.
// SMALL_kernel_base_pa = kernel_base_pa;

static void __initialize_kernelimage_region(void)
{
    unsigned long *pt_kernelimage = 
        (unsigned long *) get_table_pointer_va();  //PAGETABLE_KERNELIMAGE; 

// pa
// kernel_base_pa:    
// Início da imagem do kernel. (1MB mark).
    unsigned long kernelimage_pa = (unsigned long) KERNEL_BASE_PA;
// va
    g_kernelimage_va = (unsigned long) KERNELIMAGE_VA;
// pd index
    int pdindex = (int) X64_GET_PDE_INDEX(KERNELIMAGE_VA);
// size
    mm_used_kernelimage = (1024 * 2);  //2mb

// kernel_base_pa = 0x100000pys
// (0x100000pys = 0x30000000virt).
// Configurando a área de memória onde ficará a imagem do kernel.

// Isso mapeia 2MB começando do primeiro mega. 
// (kernel mode).
// Preenchendo a tabela pt_ring0area.
// 'kernel_base_pa' é o endereço físico da imagem do kernel.
// Criamos a pagetable.
// Criamos a entrada 384 apontando para a pagetable.
// SMALL_kernel_base_pa = kernel_base_pa;

// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,        // pd 
        (int) pdindex,                       // entry
        (unsigned long) &pt_kernelimage[0],  // pt
        (unsigned long) kernelimage_pa,      // region base
        (unsigned long) ( PAGE_WRITE | PAGE_PRESENT ) );  // flags=3
}


// The frontbuffer - (LFB)
// local worker
// see: x64gva.h
// FRONTBUFFER_VA - 0x0000000030200000
// The physical address of the LFB was given by the bootloader.
// The bootloader got this address via VESA initialization
// in 16bit mode using BIOS.
// ------------------------------------
// Size?
// We don't know the size of the LFB.
// #todo: We need a routine to get this information.
// So, this way we're gonna map only 2MB of the LFB,
// given us a resolution of 800x600.
// ------------------------------------
// mm_used_lfb = (1024 * 2); 
// ------------------------------------
// In this routine:
//  + Criamos uma pagetable.
//  + Apontamos a pagetable para uma entrada do diretório do kernel.

static void __initialize_frontbuffer(void)
{

// The pagetable for the frontbuffer.
// #warning: 
// It has 512 entries, given us 2MB, using 4KB pages.
    unsigned long *pt_frontbuffer = 
        (unsigned long *) get_table_pointer_va();  //PAGETABLE_FRONTBUFFER;

// -----------------------------------------
// pa
// Saving the physical address into a global variable.
    g_frontbuffer_pa = 
        (unsigned long) SMALL_frontbuffer_pa;
    // Local variable.
    unsigned long framebuffer_pa = 
        (unsigned long) SMALL_frontbuffer_pa;

// -----------------------------------------
// va
// Saving the virtual address into a global variable.
    g_frontbuffer_va = (unsigned long) FRONTBUFFER_VA;

// -----------------------------------------
// pd index
// Using the va.
    int pdindex = (int) X64_GET_PDE_INDEX(FRONTBUFFER_VA);

// -----------------------------------------
// size
// Provisorio.
    mm_used_lfb = (1024 * 2);

// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.

    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,        // pd 
        (int) pdindex,                       // entry
        (unsigned long) &pt_frontbuffer[0],  // pt
        (unsigned long) framebuffer_pa,      // region base
        (unsigned long) ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT ) );  // flags=7
}


// The backbuffer.
// local worker
// (2MB Backbuffer)
// -------------------------
// va:
//   BACKBUFFER_VA = 0x30400000.
//   (0x30400000 ~ 0x305FFFFF? )
// -----------------------------
// mm_used_backbuffer = (1024 * 2);  
// -----------------------------
//   #warning:
//   We already reserved a big area in the physical memory
//   for the backbuffer. Maybe we're gonna have a bigger backbuffer,
//   stay tuned.
// -----------------------------
// This routine:
//  + Mapeando 2mb de memória em ring3 para o backbuffer.
//  + Criamos a pagetable.
//  + Apontamos a pagetable para uma entrada do diretório do kernel.
//

static void __initialize_backbuffer(void)
{
// Mapping the backbuffer.
// see: BACKBUFFER_PA in x64gpa.h
// see: BACKBUFFER_VA in x64gva.h


// The pagetable for the backbuffer.
// #warning: 
// It has 512 entries, given us 2MB, using 4KB pages.
    unsigned long *pt_backbuffer = 
        (unsigned long *) get_table_pointer_va();

// #warning:
// SMALL_backbuffer_pa points to BACKBUFFER_PA.

// ----------------------------
// pa
// Saving the physical address into a global variable.
    g_backbuffer_pa = (unsigned long) SMALL_backbuffer_pa; 
    // Local variable.
    unsigned long backbuffer_pa = (unsigned long) SMALL_backbuffer_pa; 

// ----------------------------
// va
// Saving the virtual address into a global variable.
    g_backbuffer_va = (unsigned long) BACKBUFFER_VA;

// ----------------------------
// pd index
// Based on our va.
    int pdindex = (int) X64_GET_PDE_INDEX(BACKBUFFER_VA);
// size
    mm_used_backbuffer = (1024 * 2);

// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
// #importante:
// Flags for user mode.
// The display server is gonna need this.

    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,       // pd 
        (int) pdindex,                      // entry
        (unsigned long) &pt_backbuffer[0],  // pt
        (unsigned long) backbuffer_pa,      // region base
        (unsigned long) ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT ) );  // flags=7
}


// local worker
// Paged pool area. 0x30600000 ~ 0x307FFFFF
// 0x30600000
// PAGEDPOOL_VA
// 387 = Paged pool area.
// mm_used_pagedpool = (1024 * 2);  //2mb 
// 0x30600000;  // 2mb a mais que o backbuffer
// g_pagedpool_va = (unsigned long) PAGEDPOOL_VA;
// mapeando 2mb de memória em ring3 para o pagedpool.

static void __initialize_pagedpool(void)
{
    unsigned long *pt_pagedpool = 
        (unsigned long *) get_table_pointer_va();  //PAGETABLE_PAGEDPOOL;
// pa
    unsigned long pagedpool_pa = 
        (unsigned long) SMALL_pagedpool_pa;
// va
// Esse é o endereço virtual do início do pool de pageframes.
// #bugbug: O paged pool so tem 2mb, veja pages.c
// então só podemos mapear 2*1024*1024/4096 páginas.
// 0x30600000;  // 2mb a mais que o backbuffer
    g_pagedpool_va = (unsigned long) PAGEDPOOL_VA;
// pd index
    int pdindex = (int) X64_GET_PDE_INDEX(PAGEDPOOL_VA);
// size
    mm_used_pagedpool = (1024 * 2);  //2mb 

// mapeando 2mb de memória em ring3 para o pagedpool.
// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,      // pd 
        (int) pdindex,                     // entry
        (unsigned long) &pt_pagedpool[0],  // pt
        (unsigned long) pagedpool_pa,      // region base 
        (unsigned long) ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT ) );  // flags=7
}

// ---------------------------
// mm_map_2mb_region:
// OUT: 0=OK -1=FAIL.
int 
mm_map_2mb_region(
    unsigned long pa,
    unsigned long va)
{
// + Only ring 0.
// + Only in the kernel's main page directory.
// ---------------------------
// Flags: = 0x1B
// 10=cache desable 
// 8= Write-Through 
// 0x002 = Writeable 
// 0x001 = Present
// 0001 1011

// #todo
// We can create another version of this function,
// but with flags into parameters.

    unsigned long _pa = (unsigned long) pa;
    unsigned long _va = (unsigned long) va;

// pt
// 0x00088000
    unsigned long *_pt = (unsigned long *) get_table_pointer_va();
    if ((void*) _pt == NULL){
        goto fail;
    }

// pd index:
    int pdindex = (int) X64_GET_PDE_INDEX(_va);
    if (pdindex < 0)
        goto fail;
    if (pdindex >= 512)
        goto fail;

// Flags:
// 10=cache desable 
// 8= Write-Through 
// 0x002 = Writeable 
// 0x001 = Present
// 0001 1011
    unsigned long flags = (unsigned long) 0x1B;

// + Only ring 0.
// + Only in the kernel's main page directory.

    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,  // pd 
        (int) pdindex,                 // entry
        (unsigned long) &_pt[0],       // pt
        (unsigned long) _pa,           // region base
        (unsigned long) flags );       // flags=1b

//ok
    return 0;
fail:
    return (int) -1;
}

// local worker
// Heaps support.
// Pool de heaps.
// Esses heaps serão usados pelos processos.
// Preparando uma área de memória grande o bastante para conter 
// o heap de todos os processos.
// ex: 
// Podemos dar 128 KB para cada processo inicialmente.
// 2048 KB = (2 MB).
// >> (user mode).
// #importante:
// Os endereços físico e virtual são iguais para essa tabela.
// #bugbug
// Lembrando que esses heaps estão em ring3.
// O Window server é um processo em ring0.
// Vamos garantir que ele não use um heap vindo desse pool.
// Pois ele tem seu próprio heap em ring0.

static void __initialize_heappool(void)
{
// The pagetable.
    unsigned long *pt_heappool = 
        (unsigned long *) get_table_pointer_va();  //PAGETABLE_HEAPPOOL; 
// The pa.
    unsigned long heappool_pa = 
        (unsigned long) SMALL_heappool_pa;
// The va. 0x30800000;
    g_heappool_va = (unsigned long) HEAPPOOL_VA;
// pd index
    int pdindex = (int) X64_GET_PDE_INDEX(HEAPPOOL_VA);
// Used memory. 2048 KB.
    mm_used_heappool = (1024 * 2);

// -----------------------
// The heaps in the pool.
// see: process.h and x64gpa.h

// Counting the heaps we get from the pool.
    g_heap_count = 0;
// Maximum number of heaps in the pool.
// 64.
    g_heap_count_max = G_DEFAULT_PROCESSHEAP_COUNTMAX;
// The heap size.
// 32KB.
    g_heap_size = G_DEFAULT_PROCESSHEAP_SIZE;

// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,     // pd 
        (int) pdindex,                    // entry
        (unsigned long) &pt_heappool[0],  // pt
        (unsigned long) heappool_pa,      // region base
        (unsigned long) ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT ) );  // flags=7
}

// ====================================================================
// 2mb, ring0, start = 0x30A00000.
// Ring 0 kernel module. MOD0.BIN.
// See: x64init.c When we setup the Heap pointer.
// InitProcess->Heap = (unsigned long) g_extraheap1_va; :)
// 2048 KB = (2 MB).

static void __initialize_extraheap1(void)
{
    unsigned long *pt_extraheap1 = 
        (unsigned long *) get_table_pointer_va();
// pa
    unsigned long extraheap1_pa = 
        (unsigned long) SMALL_extraheap1_pa;
// va
// 0x30A00000
    g_extraheap1_va = (unsigned long) EXTRAHEAP1_VA;
// pd index
    int pdindex = (int) X64_GET_PDE_INDEX(EXTRAHEAP1_VA);
// size
    g_extraheap1_size = (1024 * 2); 
    mm_used_extraheap1 = (1024 * 2);

// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,       // pd 
        (int) pdindex,                      // entry
        (unsigned long) &pt_extraheap1[0],  // pt
        (unsigned long) extraheap1_pa,      // region base
        (unsigned long) ( PAGE_WRITE | PAGE_PRESENT ) );  // flags=3

    g_extraheap1_initialized = TRUE;
}

// 2mb, ring0, start = 0x30C00000.
// local worker
// used by the slab allocator.
static void __initialize_extraheap2(void)
{
    unsigned long *pt_extraheap2 = 
        (unsigned long *) get_table_pointer_va();
// pa
    unsigned long extraheap2_pa = 
        (unsigned long) SMALL_extraheap2_pa;
// va
// 0x30C00000
    g_extraheap2_va = (unsigned long) EXTRAHEAP2_VA;
// pd index
    int pdindex = (int) X64_GET_PDE_INDEX(EXTRAHEAP2_VA);
// size
    mm_used_extraheap2 = (1024 * 2); 
    g_extraheap2_size = (1024 * 2);  

// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
    mm_fill_page_table( 
      (unsigned long) KERNEL_PD_PA,       // pd 
      (int) pdindex,                      // entry
      (unsigned long) &pt_extraheap2[0],  // pt
      (unsigned long) extraheap2_pa,      // region base
      (unsigned long) ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT ) );  // flags=7

    g_extraheap2_initialized = TRUE;
}

// 2mb, ring3, start = 0x30E00000.
// used by the slab allocator.
static void __initialize_extraheap3(void)
{
    unsigned long *pt_extraheap3 = 
        (unsigned long *) get_table_pointer_va();
// pa
    unsigned long extraheap3_pa = 
        (unsigned long) SMALL_extraheap3_pa;
// va
// 0x30E00000
    g_extraheap3_va = (unsigned long) EXTRAHEAP3_VA;
// pd index
    int pdindex = (int) X64_GET_PDE_INDEX(EXTRAHEAP3_VA);
// size
    mm_used_extraheap3 = (1024 * 2); 
    g_extraheap3_size = (1024 * 2);  

    mm_fill_page_table( 
        (unsigned long) KERNEL_PD_PA,       // pd 
        (int) pdindex,                      // entry
        (unsigned long) &pt_extraheap3[0],  // pt
        (unsigned long) extraheap3_pa,      // region base
        (unsigned long) ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT ) );  // flags=7

    g_extraheap3_initialized = TRUE;
}

// PAGE TABLES.
// Vamos criar algumas pagetables e apontá-las
// como entradas no diretório 'kernel_pd0'.
// Entries:
//   0 = Primeiros 2MB da memória RAM.
//   1 = Area em user mode que começa em 32MB da memória física.
// 384 = Imagem do kernel que começa em 1MB da memória física.
// 385 = frontbuffer. LFB.
// 386 = backbuffer. Começa na área de 16MB da memória física.
// 387 = Paged pool area.
// 388 = Heap pool.
// 389 = extraheap1
// 390 = extraheap2
// 391 = extraheap3. The window server image.

static void __initialize_canonical_kernel_pagetables(void)
{
// Called by mmInitializePaging().
// Install some pagetables into the 
// kernel page directory 0.

// --------------------------
// va=0          | Ring 0 area.
    __initialize_ring0area();

// --------------------------
// va=0x00200000 | Ring 3 area.
    __initialize_ring3area();

// --------------------------
// va=0x30000000 | kernel image region.
    __initialize_kernelimage_region();

// --------------------------
// va=0x30200000 | Frontbuffer.
    __initialize_frontbuffer();

// --------------------------
// va=0x30400000 | Backbuffer.
    __initialize_backbuffer();

// --------------------------
// va=0x30600000 | Paged pool.
    __initialize_pagedpool();

// --------------------------
// va=0x30800000 | Heap pool.
    __initialize_heappool();

// -----------------------------------
// Extraheap 1: Used by the kernel module.
// va=0x30A00000 | Extra heap 1.
    __initialize_extraheap1();

// -----------------------------------
// New paged pool: //  It uses the extraheap2 and 3.
// Extraheap 2 and 3: Used by the slab allocator.
// va=0x30C00000 | Extra heap 2.
    __initialize_extraheap2();
// va=0x30E00000 | Extra heap 3.
    __initialize_extraheap3();
// Criado com dois blocos consecutivos de 2mb cada,
// previamente alocados.
// see: slab.c
    slab_initialize();

//...

}

/*
// --------------------------------------------
// #test: trying to map a region.
void test(void);
void test(void)
{
// --------------------------------------------
// #test
// #bugbug
// Something goes wrong in the system when we map this reagion.

    int i=0;
    unsigned long pa=0;
    unsigned long va=0;
    int status = -1;
    for (i=0; i<8; i++)
    {
        pa = __BIG_BUFFER_PA + (0x200000 * i);
        va = BIG_BUFFER_VA   + (0x200000 * i);
        status = (int) mm_map_2mb_region(pa,va);
        if (status != 0)
        {
            debug_print("mmInitializePaging: bigbuffer\n");
            x_panic    ("mmInitializePaging: bigbuffer\n");
        }
    };
// --------------------------------------------
}
// --------------------------------------------
*/


// ======================================
// mmInitializePaging:
// Main routine.
// This routine initializes the paging infrastructure.
// Initalizing the paging support.
// Mapping the static system areas.
// Called by mmInit() in mminit.c

int mmInitializePaging(void)
{
    register unsigned int i=0;

    //if( serial_debug == TRUE )
        //debug_print("mmInitializePaging:\n");

// local worker:
// RAM usage management.
    __initialize_ram_usage_varables();

// local worker:
// Default physical regions.
    __initialize_canonical_physical_regions();

// =========================================================

//
// pml4
//
    // 0x0009C000 = Kernel pml4
// 9 9 9 9 12
// Agora as tabelas possuem 512 entradas,
// pois é isso o que dá pra ter com apenas 9 bits.
// #todo
// Precisamos encontra espaço na memória para kernel_pml4 e kernel_pdpt0.
// Precisamos criar 'worker functions' para essas rotinas repetitivas.
// See:
// gpa.h for some physical addresses.
// =====================================
// Levels: PML4, PDPT, PD, PT
// PML4 - Page Map Level 4
// PDPT - Page Directory Pointer Table
// PD   - Page Directory
// PT   - Page Table


// Isso porque endereço físico e virtual são igual abaixo de 1 mb.
// 0x0009C000va = 0x0009C000pa
// See: x64gpa.h

    gKernelPML4Address = (unsigned long) KERNEL_PML4_VA;

// See: x64gva.h

    // level 4: 0x000000000009C000
    unsigned long *kernel_pml4  = (unsigned long *) KERNEL_PML4_PA;
    // level 3: 0x000000000009B000
    unsigned long *kernel_pdpt0 = (unsigned long *) KERNEL_PDPT_PA;
    // level 2: 0x000000000009A000
    unsigned long *kernel_pd0   = (unsigned long *) KERNEL_PD_PA;
    // level 1
    // A lot of page tables in level 1.

//
// Saving the data
//

// Saving some info used by the kernel to handle 
// the memory he needs.
// See:
// x64mmm.h
// x64gva.h

    // pml4
    kernel_mm_data.pml4_va = (unsigned long) kernel_pml4;
    kernel_mm_data.pml4_pa = (unsigned long) KERNEL_PML4_PA;

    // pdpt0
    kernel_mm_data.pdpt0_va = (unsigned long) kernel_pdpt0;
    kernel_mm_data.pdpt0_pa = (unsigned long) KERNEL_PDPT_PA;

    // pd0
    kernel_mm_data.pd0_va = (unsigned long) kernel_pd0;
    kernel_mm_data.pd0_pa = (unsigned long) KERNEL_PD_PA;

    kernel_mm_data.used = TRUE;
    kernel_mm_data.magic = 1234; 


// Check
// #bugbug: x_panic is not available yet.

    if ( kernel_mm_data.pml4_va  == 0 || 
         kernel_mm_data.pml4_pa  == 0 ||
         kernel_mm_data.pdpt0_va == 0 || 
         kernel_mm_data.pdpt0_pa == 0 ||
         kernel_mm_data.pd0_va   == 0 || 
         kernel_mm_data.pd0_pa   == 0 )
    {
        debug_print ("mmInitializePaging: [FAIL] Invalid kernel_mm_data\n");
        x_panic     ("mmInitializePaging: [FAIL] Invalid kernel_mm_data\n");
    }

//
// SYSTEM MEMORY - PAGED POOLS 
//

//Criaremos por enquanto apenas uma pagetable com mem�ria paginada.
//unsigned long *paged_page_table = (unsigned long *) ??;  //BUFFER_PAGETABLE.	

// Message. (verbose).

// fast check
// #bugbug: x_panic is not available yet.

    //if( framebuffer_pa == 0 )
        //x_panic("mmSetupPaging: framebuffer_pa");

// # DIRECTORIES
// Preenchendo todo o diret�rio de p�ginas do kernel com p�ginas 
// n�o presentes. Usando um endere�o nulo de p�gina.
// Inicializando quatro diret�rios.
// o bit 7 da entrada permanece em 0, 
// indicando que temos p�ginas de 4KB.
// kernel
// Diret�rio de p�ginas do processo kernel.
// 0 no bit 2 indica qual level ??
// 010 em bin�rio.
// #importante:
// O endere�o f�sico e virtual s�o iguais para essa tabela.

//
// ==================================================
//

// Clear levels 4, 3 and 2.
// Páginas não presentes, pode ler e escrever.

// Clear level 4
    for ( i=0; i < 512; i++ ){
        kernel_pml4[i] = (unsigned long) 0 | PAGE_WRITE;
    };

// Clear level 3
    for ( i=0; i < 512; i++ ){
       kernel_pdpt0[i] = (unsigned long) 0 | PAGE_WRITE;
    };

// Clear level 2
    for ( i=0; i < 512; i++ ){
        kernel_pd0[i] = (unsigned long) 0 | PAGE_WRITE;
    };

//
// ============================
//

// #bugbug
// Estaríamos dizendo que todas as entradas do primeiro diretorio
// esntão em ring0, e que todas as entradas do primeiro pdpt
// também estão?

// pd >> pdpt
// Pointing the 'page directory' address 
// at the first entry in the 'page directory pointer table'.

// Páginas presentes, pode ler e escrever, user mode.
// flags=7
    kernel_pdpt0[0] = 
        (unsigned long) &kernel_pd0[0];
    kernel_pdpt0[0] = 
        (unsigned long) ( kernel_pdpt0[0] | PAGE_USER | PAGE_WRITE | PAGE_PRESENT );

// pdpt >> pml4
// Pointing the 'page directory pointer table' address 
// at the first entry in the kernel_pml4.

// Páginas presentes, pode ler e escrever, user mode.
// flags=7
    kernel_pml4[0] = 
        (unsigned long) &kernel_pdpt0[0];
    kernel_pml4[0] = 
        (unsigned long) ( kernel_pml4[0] | PAGE_USER | PAGE_WRITE | PAGE_PRESENT );

// local worker: 
// Initialize kernel page tables.
    __initialize_canonical_kernel_pagetables();

    if (memorysizeTotal == 0){
        debug_print ("mmInitializePaging: [FIXME] We need the memorysizeTotal\n");
        //while(1){}
    }
    //debug_print ("mmInitializePaging: [DEBUG] memorysizeTotal is not zero\n");
    //while(1){}


// ------
// Initialize frame table.
// It depends on the size of the physical memory.
// It starts at mark of 128MB and goes untill
// the mark of almost 256MB.
// So this routine needs to check if we have memory
// enough for this operation.
// see: mmft.c

    I_initialize_frame_table();

// ---------------------------------
// #todo
// Let's try to map something more if have 
// free memory for that.

// #warning
// Start at 256MB mark, 
// it's because the frame table does no reach that mark.
// It almost did.
// __256MB_MARK_PA


// ...

// ==============================================

// ---- Last thing ----
// local worker: Setup memory usage.
// This moment we're gonna know how much memory was mapped
// and how much memory we still have available.
    __mmSetupMemoryUsage();

// ==============================================

    //debug_print("mmInitializePaging: [DANGER] Load cr3\n");

// pae
    //printk ("SetUpPaging: __enable_pae\n");

    //isso ja foi feito no bl.
    //__enable_pae();

    //while(1){}

// Load it in cr3.
    //printk ("SetUpPaging: load_pml4_table\n");

// #importante
// O kernel já está em long mode,
// a única coisa que precisamos fazer é
// carregar o cr3, mas acho que agora usaremos
// endereços de 64bit?

// Isso precisa ser um endereço físico.
// Existe identidade.
// Esse é um ponteiro para uma região da memória
// abaixo de 1MB.

    load_pml4_table( (void *) &kernel_pml4[0] );

//-----------------------------------------------------

// Done:
// Ok, nesse momento estamos usado um novo
// espaço de endereçamento do kernel.
// #todo
// + Agora podemos usar os endereços virtuais
//   configurados nessa rotina.
// + Agora podemos limpar os buffers e heaps
//   criados logo acima. Pois no momento em que foram criados
//   não era possível limpá-los pois não havíamos
//   setado um novo pml4 no cr3.

// #test
// Cleaning buffers.
// #danger: 
// + If we clear a wrong address. 
// + If overflow.
    //size_t _size_in_kb=0;
    //size_t _size_in_bytes=0;


/*
// Clear the extraheap 1
    _size_in_kb = 
        (size_t) (g_extraheap1_size & 0xFFFFFFFF);
    _size_in_bytes =
        (size_t) (_size_in_kb/1024);
    memset( EXTRAHEAP1_VA, 0, _size_in_bytes );
*/

/*
// Clear the extraheap 2
    _size_in_kb = 
        (size_t) (g_extraheap2_size & 0xFFFFFFFF);
    _size_in_bytes =
        (size_t) (_size_in_kb/1024);
    memset( EXTRAHEAP2_VA, 0, _size_in_bytes );
*/

/*
// Clear the extraheap 3
    _size_in_kb = 
        (size_t) (g_extraheap3_size & 0xFFFFFFFF);
    _size_in_bytes =
        (size_t) (_size_in_kb/1024);
    memset( EXTRAHEAP3_VA, 0, _size_in_bytes );
*/

/*
// Clear the paged pool.
    _size_in_kb = 
        (size_t) (mm_used_pagedpool & 0xFFFFFFFF);
    _size_in_bytes =
        (size_t) (_size_in_kb/1024);
    memset( PAGEDPOOL_VA, 0, _size_in_bytes );
*/


    // ==============================================
    //printk ("SetUpPaging: cpuSetMSR\n");
    
    // #todo
    // long mode exige uma configuração usando msr.
    //int msrStatus = FALSE;
    //msrStatus = cpuHasMSR();
    //in: MSR, LO, HI
    //cpuSetMSR( 0xC0000080, 0x100, 0 );

    // #debug
    //while(1){
    //    asm ("cli");
    //}

//done:
    //debug_print("mmInitializePaging: done\n");
// OK
    return 0;
fail:
    return (int) -1;
}


void pages_print_info(int system_type)
{

    if (system_type<0)
        return;

    switch (system_type){

    case stSmallSystem:
            printk("Origin PA:            %xH \n", SMALL_origin_pa );
            printk("Base kernel start PA: %xH \n", SMALL_kernel_base_pa );
            printk("User area start PA:   %xH \n", SMALL_user_pa );
            printk("cga memory PA:        %xH \n", SMALL_cga_pa );
            printk("frontbuffer PA:       %xH \n", SMALL_frontbuffer_pa );
            printk("backbuffer PA:        %xH \n", SMALL_backbuffer_pa );
            printk("paged memory pool PA: %xH \n", SMALL_pagedpool_pa );
            printk("heap pool PA:         %xH \n", SMALL_heappool_pa );
            printk("extraheap1 PA:        %xH \n", SMALL_extraheap1_pa );
            printk("extraheap2 PA:        %xH \n", SMALL_extraheap2_pa );
            printk("extraheap3 PA:        %xH \n", SMALL_extraheap3_pa );
        break;
    case stMediumSystem:
            printk("Origin PA:            %xH \n", MEDIUM_origin_pa );
            printk("Base kernel start PA: %xH \n", MEDIUM_kernel_base_pa );
            printk("User area start PA:   %xH \n", MEDIUM_user_pa );
            printk("cga memory PA:        %xH \n", MEDIUM_cga_pa );
            printk("frontbuffer PA:       %xH \n", MEDIUM_frontbuffer_pa );
            printk("backbuffer PA:        %xH \n", MEDIUM_backbuffer_pa );
            printk("paged memory pool PA: %xH \n", MEDIUM_pagedpool_pa );
            printk("heap pool PA:         %xH \n", MEDIUM_heappool_pa );
            printk("extraheap1 PA:        %xH \n", MEDIUM_extraheap1_pa );
            printk("extraheap2 PA:        %xH \n", MEDIUM_extraheap2_pa );
            printk("extraheap3 PA:        %xH \n", MEDIUM_extraheap3_pa );
        break;
    case stLargeSystem:
            printk("Origin PA:            %xH \n", LARGE_origin_pa );
            printk("Base kernel start PA: %xH \n", LARGE_kernel_base_pa );
            printk("User area start PA:   %xH \n", LARGE_user_pa );
            printk("cga memory PA:        %xH \n", LARGE_cga_pa );
            printk("frontbuffer PA:       %xH \n", LARGE_frontbuffer_pa );
            printk("backbuffer PA:        %xH \n", LARGE_backbuffer_pa );
            printk("paged memory pool PA: %xH \n", LARGE_pagedpool_pa );
            printk("heap pool PA:         %xH \n", LARGE_heappool_pa );
            printk("extraheap1 PA:        %xH \n", LARGE_extraheap1_pa );
            printk("extraheap2 PA:        %xH \n", LARGE_extraheap2_pa );
            printk("extraheap3 PA:        %xH \n", LARGE_extraheap3_pa );
        break;
    default:
        break;
    };
}

void pages_print_video_info(void)
{
// Video info
    printk("\n\n");
    printk ("Frontbuffer PA: {%x} | Frontbuffer VA: {%x}\n", 
        SMALL_frontbuffer_pa, g_frontbuffer_va );
    printk ("Backbuffer PA: {%x} | Backbuffer VA: {%x}\n", 
        SMALL_backbuffer_pa, g_backbuffer_va );
}

//
// End
//

