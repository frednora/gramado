
// x64mm.h

#ifndef __X64MM_H
#define __X64MM_H    1

// Salvando o último endereço válido de memória ram.
// usado em head.asm
// see: globals.c
extern unsigned long blSavedLastValidAddress;
extern unsigned long blSavedPhysicalMemoryInKB;

// Definições dos campos das páginas.
#define PAGE_PRESENT  0x001
#define PAGE_WRITE    0x002
#define PAGE_USER     0x004
// 0x008 write through
#define PAGE_NOCACHE   0x010
#define PAGE_ACCESSED  0x020
#define PAGE_DIRTY     0x040
//0x080 page atribute table
#define PAGE_GLOBAL  0x100
#define PAGE_AVAIL   0xE00
#define PAGE_PRESENT_WRITE       ( PAGE_WRITE | PAGE_PRESENT )
#define PAGE_PRESENT_WRITE_USER  ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT )
#define PAGE_KERNEL_PGPROT       (PAGE_PRESENT_WRITE)

// Kernel pml4. (cr3)
// The virtual address of the kernel pml4 table.
// see: pages.c
extern unsigned long gKernelPML4Address; 


/*
 // #test x86 32bit entry #todo 64bit
typedef struct page
{
   u32int present    : 1;   // Page present in memory
   u32int rw         : 1;   // Read-only if clear, readwrite if set
   u32int user       : 1;   // Supervisor level only if clear
   u32int accessed   : 1;   // Has the page been accessed since last refresh?
   u32int dirty      : 1;   // Has the page been written to since last refresh?
   u32int unused     : 7;   // Amalgamation of unused and reserved bits
   u32int frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;
*/

// Some useful data for memory management.

struct mm_data_d
{
    int used;
    int magic;

    unsigned long pml4_va; 
    unsigned long pml4_pa; 

// We just need one table to handle a basic initialization.

// Saving the pointer for the first table.
    unsigned long pdpt0_va; 
    unsigned long pdpt0_pa; 

// We just need one table to handle a basic initialization.

// Saving the pointer for the first table.
    unsigned long pd0_va; 
    unsigned long pd0_pa; 
};

// Kernel process.
struct mm_data_d kernel_mm_data;
// Init process.
struct mm_data_d init_mm_data;
// A new process been created.
struct mm_data_d newprocess_mm_data;
// ...




//
// == zones ==================================================
//

// Memoria total em duas partes.


//system zone. 
struct system_zone_d
{
    unsigned long systemzone_start;  //0x00000000. s�o os 32MB iniciais  
};
struct system_zone_d *systemzone;

//window zone.  
//#bugbug: maybe it is not a good name.
struct window_zone_d
{
    unsigned long windowzone_start;
    //Endere�o onde come�a a user session.
    unsigned long usersession_start;	//ficar� dentro de uma �rea paginada.
    struct usession_d *usersession;    
};
struct window_zone_d *windowzone;


//Zones.
// ** ESSA ESTRUTURA � A RAIZ DE TODA GER�NCIA DE MEM�RIA **
struct mm_zones_d
{
    struct system_zone_d *system_zone;  //Essa zona � para o sistema.
    struct window_zone_d *window_zone;  //Essa zona toda � uma user session.
};
struct mm_zones_d *zones;


// =============================================================

// variaveis blobais de endereços 
// usados no gerenciamento de zonas de memoria.

#define SYSTEMZONE_START 0
#define SYSTEMZONE_END   0x0FFFFFFF 
#define WINDOWZONE_START 0x10000000
//#define WINDOWZONE_END ??

unsigned long systemzoneStart;
unsigned long systemzoneEnd;
unsigned long systemzoneSize;
unsigned long windowzoneStart;
unsigned long windowzoneEnd;    //?? Devemos levar em consideraçao o calculo do tamanho da mem�ria
unsigned long windowzoneSize;



/*
 * page_directory_d:
 *     Estrutura para o 'page directory' de um processo.
 *     Todo processo tem seu pr�prio diret�rio de p�ginas.
 *     Assim v�rios processos podem usar o mesmo endere�o l�gico.
 *     Ex: 0x400000
 *     @todo: Um ponteiro para essa estrutura pode estar no PCB do processo.
 *            usar os processos criados por processos para testar a configura��o
 *           de page directory.
 *     Obs: Um diret�rio tem ponteiros para page tables. as page tables 
 * funcionam como pools de frames.
 */

struct page_directory_d
{
    object_type_t  objectType;
    object_class_t objectClass;

//identificadores.
    int id;
    int used;
    int magic;

//Qual processo � o dono do diret�rio de p�ginas.
//talvez seja possivel reaproveitar o diret�rio.
    struct process_d *process;

// Endereço onde ficar� o diret�rio de p�ginas.
// Obs: Para configurar um diret�rio de p�ginas talvez
// tenha que colocar um endere�o f�sico em CR3. Lembre-se
// que o malloc do kernel base aloca mem�ria no heap do 
// processo kernel que fica no �ltimo giga da mem�ria virtual.
    unsigned long Address;


    // ?? struct page_directory_entry_d[1024] ??

    //@todo: Mais informa��es sobre o diret�rio de p�ginas.

// Proximo diret�rio, significa pr�ximo processo.
// significa processos ligados em um job.
    struct page_directory_d *next;  
};

struct page_directory_d *pagedirectoryKernelProcess;    // KERNEL.
struct page_directory_d *pagedirectoryIdleProcess;      // IDLE.
struct page_directory_d *pagedirectoryCurrent;          // Current.
struct page_directory_d *pagedirectoryShared;           // Shared. 
//...


//
// Lista de diretórios. (Pois cada processo tem um diret�rio).
//

// Quantidade de diretórios que podem ser 
// criados na lista.
#define PAGEDIRECTORY_COUNT_MAX 1024  

//Lista de estruturas para diretórios de paginas.
unsigned long pagedirectoryList[PAGEDIRECTORY_COUNT_MAX]; 


// ------------


/*
 * page_table_d.
 *     Page table structure.
 *     Obs: Uma page table funciona como um pool de frames.
 *          Tamb�m pode ser compartilhada entre processo.(cuidado).
 */

struct page_table_d
{
    object_type_t  objectType;
    object_class_t objectClass;

    int id;
    int used;
    int magic;

// A qual diret�rio de p�ginas a page table perrtence.
// se bem que talvez possamos usar a mesma pagetable
// em mais de um diret�rio. ser�??
    struct page_directory_d *directory;

// Cada pagetable pertence � um processo.
    struct process_d *process;

// Travando uma pagetable inteira,
// nenhuma de suas p�ginas poder�o se descarregadas
// para o disco de swap.
    int locked;

	// ?? struct page_table_entry_d[1024] ??

//@todo: Mais informaçoes sobre a pagetable.
    struct page_table_d *next;
};

struct page_table_d *pagetableCurrent;
//...

//
// Lista de pagetables.
//

// Quantidade de page tables criadas na lista.
#define PAGETABLE_COUNT_MAX  1024 


unsigned long pagetableList[PAGETABLE_COUNT_MAX]; 

//Linked List talvez seja uma op��o.
//page_table_t *pagetableLinkedListHead;



// ------------

/**
 **  **  SUPER IMPORTANTE  **
 ** Super block.
 ** ESSAS VARI�VEIS GLOBAIS MARCAR�O O IN�CIO E O FIM 
 ** DA �REA DE MEM�RIA F�SICA DESTINADA AOS FRAMES DE MEM�RIA 
 ** F�SICA QUE SER�O USADOS PELO GERENCIADOR DE P�GINAS.
 ** Pertencer�o ao banco FDB. (Free Data Base).
 ** 0x10000000 � um bom lugar pra come�ar os frames ,
 ** na verdade os blocos 4MB, pois cada bloco de 4MB pode ser mapeado 
 ** usando apenas uma pagetable.
 ** mmFramesSuperBlockStart = 0x10000000
 ** mmFramesSuperBlockEnd   = 0x1FFFFFFF
 ** Esses s�o endere�os f�sicos.
 ** Obs: Ficar�o nesse lugar caso se tenha mem�ria dispon�vel para isso.
 **/

// Frames Super Block.
// Vari�veis globais parecem ser uma op��o melhor de estrutura
// para esse caso. 
// Obs: Temos listas de frames em algum lugar. 
// Criaremos listas aqui para o FSB, que ser� o nome do gerenciado, 
// para melhorar o controle dessa �rea.
unsigned long mmFramesSuperBlockStart;      //Endere�o onde come�a o FSB.
unsigned long mmFramesSuperBlockEnd;        //Endere�o onde termina o FSB.
unsigned long mmFramesSuperBlockSize;       //Tamanho do FSB dado em bytes.
unsigned long mmFramesSuperBlockTotal;      //Total de frames.
unsigned long mmFramesSuperBlockTotalFree;  //Total de frames livres. 
unsigned long mmFramesSuperBlockTotalUsed;  //Total de frames e uso. 
//Continua...

// ((0x1FFFFFFF - 0x10000000) / 512)  = QUANTIDADE DE FRAMES NESSA �REA.
#define FSB_FRAMES_MAX      (1*1024) //?? @todo: Determinar melhor isso
#define FSB_FREEFRAMES_MAX  (1*1024) //?? @todo: Determinar melhor isso

//## BUGBUG isso tornaria esse array bem grande.


//
// Lista com todos.
//

// Lista de ponteiros para as estruturas de 
// todos os frames do FSB.
unsigned long fsbFrames[FSB_FRAMES_MAX]; 

//
// Lista de livres.
//

// Lista de ponteiros para as estruturas de 
// todos os frames 'LIVRES' do FSB.
unsigned long fsbFreeFrames[FSB_FREEFRAMES_MAX];  
 
 

//
// memory:
//




// Quantidade m�xima de framepools.
// Um framepool � uma parti��o da mem�ria f�sica.
// Cada framepool � composto de 1024 pageframes.
// @todo: 
// #bugbug: 
// A quantidade de framepools deve ser equivalente � quantidade 
// de mem�ria f�sica dispon�vel.
// Por isso devemos criar �reas de mem�ria f�sica aloc�veis. O que 
// facilita a manuten��o de listas de framepools.
// Em outras palavra. Temos que concatenar parti��es de mem�ria f�sica
// pra facilitar. Ent�o chamaremos essa �rea onde est�o as parti��es de
// �rea pagin�vel. Pois existem �res de mem�ria que n�o s�o pafin�veis,
// elas simplesmente foram mapeadas para que alguma parte do sistema use.
// @todo: 
// Criar um ponteiro que indique o �n�cio da �rea p�gin�vel, assim como 
// acontece com o in�cio de um heap.
// � principio todo processo poderia ter acesso � apenas uma parti��o.
// Um framepool � garantido para um processo quando esse processo � criado
// mesmo antes de haver algum mapeamento. 
// Na verdade um processo ter� uma lista de framepools.


//List.(1024 framepools de 4MB d� 4GB).
#define FRAMEPOOL_COUNT_MAX 1024


//
// ## MEMORY PARTITION ##
//

// Um framepool tem 4MB de tamanho.
// #todo: Um framepool em 64bit talvez seja de 2mb.
#define MEMORY_PARTITION_SIZE   (4 * MB)


// Obs: 
// Criamos apenas algumas entradas no diret�rio de p�ginas
// do kernel, mas algumas dessas entradas ser�o iguais para todos 
// os processos. Ent�o os diret�rios de da p�ginas dos processos
// ser�o configurados de forma semelhante.

// Page frames da imagem do kernel:
// ================================
// Foram alocadas 1024 pageframes para a imagem do kernel.
// Isso equivale a um pageframe pool. Que � igual a 4MB. Pois
// s�o 1024 page frames de pag�nas de 4KB cada.

// + kernel area = 1024 page frames (4MB)
// + kernel image = 1024 pageframes (4MB)
// + user mode area = 1024 pageframes (4MB)
// + vga = 1024 pageframes (4MB) (Isso transpassa o real tamanho da vga)
// + lfb = (frontbuffer) 1024 pageframes (4MB) (Isso � muito pouco, pois 
//         uma placa de v�deo tem mais mem�ria que isso)
// + backbuffer = 1024 pageframes (4MB) (Isso realmente � pouco, o backbuffer 
//         deve caner uma imagem grande, que ser� dividade em v�rios monitores.)



/*
INVLPG

INVLPG is an instruction available since the i486 
that invalidates a single page in the TLB. 
Intel notes that this instruction may be implemented 
differently on future processes,
but that this alternate behavior must be 
explicitly enabled. INVLPG modifies no flags.

NASM example:

     invlpg [0]
Inline assembly for GCC (from Linux kernel source):

static inline void __native_flush_tlb_single(unsigned long addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

*/

/*
typedef enum {
    MM_SYSTEM_SMALL,
    MM_SYSTEM_MEDIUM,
	MM_SYSTEM_BIG,
}SYSTEM_SIZE_T;
*/


// Kernel Stack suppport.
// see: mm.c
extern unsigned long kernel_stack_end;       //va
extern unsigned long kernel_stack_start;     //va
extern unsigned long kernel_stack_start_pa;  //pa (endere�o indicado na TSS).

/*
 * process_memory_info_d:
 *     Estrutura para informa��es sobre a 
 * mem�ria utilizada por um processo.
 * Obs: 
 * O gerenciamento de mem�ria � tarefa do 
 * modulo /sm portanto isso n�o deve 
 * ir para o /microkernel.
 */

struct process_memory_info_d
{
    object_type_t  objectType;
    object_class_t objectClass;

    struct process_d *process;

//valor em KB. (quantidade de p�ginas + tamanho da p�gina.)

    unsigned long WorkingSet;  //Working Set.
    unsigned long Private;     //Mem�ria n�o compartilhada. 
    unsigned long Shared;      //Mem�ria compartilhada.
    //...

	//unsigned long pageFaults; //Contagem de faltas.
	//Pico de Working Set.
	//??delta de conjunto de trabalho.
	//...
};
//struct process_memory_info_d *pmiCurrent;
//...


/*
 * physical_memory_info_d:
 *     Informa��es sobre a mem�ria f�sica.
 *     O arquivo system.h deve usar isso. 
 */

struct physical_memory_info_d
{
    object_type_t objectType;
    object_class_t objectClass;

//?? d�vidas.
    unsigned long Total;    //Total de mem�ria f�sica.(RAM).
    unsigned long InCache;  //Parte do total que est� em cache.(foi paginada e est� em cache).
    unsigned long Free;     //Livre.(Existe na RAM mas n�o foi paginada??).
    //...
};
//struct physical_memory_info_d *pmiMemoryInfo;
//...


/*
 * memory_info_d:
 *     Informa��es sobre a mem�ria.
 *     Isso pode ser usado pela configura��o do sistema. 
 */

struct memory_info_d
{
    object_type_t objectType;
    object_class_t objectClass;

// Physical.
    unsigned long TotalP;
    unsigned long AvailableP;

// Virtual
    unsigned long TotalV;
    unsigned long AvailableV;
};
//struct memory_info_d *miMemoryInfo;
//...





// -----------------------------------

// Estrutura para gerência de página.
struct page_d
{

// Identificador da estrutura.
// É um índice dentro da lista de páginas de um pool.

    int id;

    int used;
    int magic;

// Índice do frame.
// Começando a contar do início da memória física.
    unsigned int absolute_frame_number;

// Locked:
// Não pode ser descarregado para o disco.
// Não pode ser alterado.
    int locked;

// A página está livre para uso pelos processos.
    int free;

// Contador de referências.
    int ref_count;

// Navigation
    struct page_d *next;
};


// #importante
// Pool de memória paginável usado para alocação.
// Aqui ficam os ponteiros para estrutura do tipo page.

// #bugbug
// O pagepool so tem 2mb,
// então só podemos mapear 2*1024*1024/4096 páginas dentro dele.
// 512 páginas.
// 512 páginas de 4KB dá 2MB.

//#bugbug: a alocação estava invadindo a área usada por outro
// componente do kernel. Precisamos usar apenas nossos 2mb para isso.
//#define PAGE_COUNT_MAX    1024  
#define PAGE_COUNT_MAX    512

unsigned long pageAllocList[PAGE_COUNT_MAX];


/*
 * frame_pool_d:
 * Estrutura para uma partição da memória física.
 * Uma partição da memória física é chamada de framepool.
 * Cada framepool tem 1024 frames.
 * #todo: Poderia ser framepool_d?
 */

struct frame_pool_d
{
    //object_type_t objectType;
    //object_class_t objectClass;

// Índice na lista de frame pools.
    int id;

    int used;
    int magic;

//Não pode ser modificada.
    int locked;

// Endereço do início do framepool.
// va ou pa ??
    unsigned long address; 

//Qual processo é o dono desse framepool.
    struct process_d *process;

// Navigation
    struct frame_pool_d *next;
};

// Cada framepool abaixo � o framepool inicial de uma regi�o.
// Uma regis�o pode ter v�rios framepools.

//kernel space.
struct frame_pool_d *framepoolKernelSpace;            //0x00000000  Kernel Space. In�cio do kernel space.

//user space
struct frame_pool_d *framepoolSmallSystemUserSpace;   //0x00400000  Para um sistema pequeno o kernel space tem 4MB.
struct frame_pool_d *framepoolMediumSystemUserSpace;  // 
struct frame_pool_d *framepoolLargeSystemUserSpace;   //0x40000000  Para um sistema grande o kernel space tem um giga. 
//...

//Cada front buffer � uma placa de v�deo.
struct frame_pool_d *framepoolFrontBuffer1;   //In�cio do linear frame buffer 1.
struct frame_pool_d *framepoolFrontBuffer2;   //In�cio do linear frame buffer 2.
struct frame_pool_d *framepoolFrontBuffer3;   //In�cio do linear frame buffer 3.
struct frame_pool_d *framepoolFrontBuffer4;   //In�cio do linear frame buffer 4.
//...

//Backbuffer
struct frame_pool_d *framepoolBackBuffer1;   //In�cio do backbuffer.
struct frame_pool_d *framepoolBackBuffer2;   //In�cio do backbuffer.
struct frame_pool_d *framepoolBackBuffer3;   //In�cio do backbuffer.
struct frame_pool_d *framepoolBackBuffer4;   //In�cio do backbuffer.
//...

//area onde poderemos alocar frames para os processos usarem...
//Esse eh o framepool inicial de usa �re grande de mem�ria.
struct frame_pool_d *framepoolPageableSpace;   


//Current.
struct frame_pool_d *framepoolCurrent;
//...

//struct frame_pool_d *framepoolKernelPagedPool;
//struct frame_pool_d *framepoolKernelNonPagedPool;
//struct frame_pool_d *framepoolUserPagedPool;
//struct frame_pool_d *framepoolUserNonPagedPool;

//
//  PAGEABLE AREA
//

extern unsigned long gPagedPollStart;
extern unsigned long gPagedPollEnd;


//Obs: ISSO � UM TESTE.
//N�mero m�ximo de framepools na �rea de aloca�ao de frames.
//Obs: Isso � uma esp�cie de cache. Por�m cache � coisa de disco.
//
// ( 1 * 4MB) =   4 MB.
// ( 2 * 4MB) =   8 MB. (Usando esse num sistema de 32mb de ram).
// ( 4 * 4MB) =  16 MB.
// ( 8 * 4MB) =  32 MB.
// (16 * 4MB) =  64 MB.
// (32 * 4MB) = 128 MB.
//

//#define PAGEABLE_FRAMEPOOL_MAX  1
#define PAGEABLE_FRAMEPOOL_MAX  2      // ??
//#define PAGEABLE_FRAMEPOOL_MAX  4
//#define PAGEABLE_FRAMEPOOL_MAX  8
//#define PAGEABLE_FRAMEPOOL_MAX  16
//#define PAGEABLE_FRAMEPOOL_MAX  32


// Número máximo de índices de framepool que 
// serão usados nessa área de alocação de frames.
// Uma certa quantidade de framepools serão usados
// para alocação de frames para os processos. 
// Durante a alocação sobre demanda, os frames usados 
// virão dessa área de memória.
extern int g_pageable_framepool_index_max;

// List.(1024 framepools).
unsigned long framepoolList[FRAMEPOOL_COUNT_MAX];

// Lista de framepools livres.
// int framepoolFreeList[FRAMEPOOL_COUNT_MAX]
// frame_pool_t *framepoolFreeListHead;

// frame pool atual.
extern int g_current_framepool;
// O indice do framepool da user space para qualquer tamanho de memória.
extern int g_user_space_framepool_index;
// O máximo de framepools possíveis dado o tamanho da memória física.
extern unsigned long g_framepool_max;

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

// -------------------------------------
// Sobre gerência de memória física:
// + Resevada para hardware.
// + Em uso.
// + Modificada. 
// + Em espera.
// + Livre.
// -------------------------------------

// -------------------------------------
// Sobre gerência de memória física:
// + Total.
// + Em cache.
// + Disponível.
// + Livre.
// -------------------------------------

// ======================================================

// Frame structure.
// This structure is gonna handle a free frame.
struct frame_d
{
    pid_t owner;

// ?
// Reference counter.
    int count; 

// ?
// Reference time.
    int age; 

    unsigned long virtual_address;

// O número da entrada na tabela FT.frame_table[entry_number].
// Também é o índice que representa o offset de lba
// no armazenamento secundário.
    int entry_number;

// Se esse frame está no disco.
// Isso significa que o frame pode ser considerado livre.
// The pointer is in the SWAPPED_FRAMES[] list.
    int swapped;

    //...
    
    // #todo: navigation?
    //struct frame_d *next;
};

// ===================================================

// O armazenamento secundário pode ser um arquivo
// ou uma partição de swap.

// 1024 frames de fácil acesso.
// pode ser usado pra alocar rapidamente memória para um novo processo.
// NULL = entrada vazia
// !NULL = ponteiro para a estrutura do tipo free_frame_d.


// #todo: A plan for the future.
// Lista de processos livres.
// Isso facilita, possivelmente evitando a busca 
// na tabela global FT.frame_table[]
// struct frame_d FREE_FRAMES[1024];

// #todo: A plan for the future.
//This entries are in the disk.
// struct frame_d SWAPPED_FRAMES[1024];

// Maybe we can have more lists here.
// ...


/*
struct x64_address_space_d 
{
    int used;
    int magic;
    int id;

    unsigned long pml4_pa;
    unsigned long pml4_va;
};
*/


//
// == prototypes =================================================
//

void pages_print_info(int system_type);
void pages_print_video_info(void);

//unsigned long slab_2mb_extraheap2(void);
//unsigned long slab_2mb_extraheap3(void);

void *CreateAndIntallPageTable (
    unsigned long pml4_va,   // page map level 4
    unsigned long pml4_index,
    unsigned long pdpt_va,   // page directory pointer table
    unsigned long pdpt_index,
    unsigned long pd_va,     // page directory 
    int pd_index,            // Install the pagetable into this entry of the page directory. 
    unsigned long region_pa );


// IN:
// Endereço virtual do diretório de páginas.
// Índice da entrada no diretório indicado.
// Endereço virtual da tabela de páginas.
// Endereço físico da região de 2MB que queremos mapear.
// As flags usadas em todas as entradas da pagetable
// e na entrada do diretório de páginas.
int 
mm_fill_page_table( 
    unsigned long directory_va, 
    int           directory_entry,
    unsigned long pt_va,
    unsigned long region_2mb_pa,
    unsigned long flags );

// Memory initialization.
// This routine initializes the paging infrastructure.
int mmInitializePaging(void);

// Memory info.
void mmShowMemoryInfo(void);
void mmShowPML4Entry(int index, unsigned long pml4_va);
void mmShowPML4EntryForAllProcesses (int entry_number);
void mmShowMemoryBlocksForTheKernelAllocator(void);
void mmShowPagedMemoryList(int max);

// mm support
void mmTestingPageAlloc(void);

int kernel_gc (void);

unsigned long get_new_frame (void);
unsigned long alloc_frame(void);

// #danger
unsigned long get_table_pointer_va(void);

void load_pml4_table(void *phy_addr);

void *CloneKernelPDPT0(void);
void *CloneKernelPD0(void);
void *CloneKernelPML4 (void);
void *clone_pml4 ( unsigned long pml4_va );

int isValidPageStruct(struct page_d *p);
void freePage (struct page_d *p);
void notfreePage (struct page_d *p);

// Wrapper
unsigned long 
virtual_to_physical ( 
    unsigned long virtual_address, 
    unsigned long pml4_va );

int mm_is_page_aligned_va(unsigned long va);

// #todo
void pages_calc_mem (void);

// #todo
// #fixme
void *newPage(void);

void *mm_alloc_single_page(void);
void *mm_alloc_contig_pages(size_t size);

void *allocPages(int size);

// Interface.
void *mmAllocPage(void);
void *mmAllocPages(int size);

void initializeFramesAlloc(void);

// see: slab.c
void slab_initialize(void);
void *slab_1MB_allocator(void);

int 
mm_map_2mb_region(
    unsigned long pa,
    unsigned long va);

#endif    


