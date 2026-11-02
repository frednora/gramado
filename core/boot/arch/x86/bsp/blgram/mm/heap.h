// heap.h
// Heap support for x86 Gramado Boot Loader.
// Created by Fred Nora.

#ifndef __HEAP_H
#define __HEAP_H    1

extern unsigned long heapCount;         //Conta os heaps do sistema.
extern unsigned long bl_heap_start;     //Start.
extern unsigned long bl_heap_end;       //End.
extern unsigned long g_heap_pointer;    //Pointer.
extern unsigned long g_available_heap;  //Available.
extern unsigned long mmblockCount;      //Conta os blocos de memória dentro de um heap. 


//@todo: diminuir
//Contagem de heap.
#define HEAP_COUNT_MAX  8

//#improvisando um valor. (provisório)
#define BL_HEAP_START  (0x01000000 + 0x100000)   //??? start + size.
#define BL_HEAP_END    (0x012FFFF0 - 0x8000)     // ??? stack start - stack size

#define BL_HEAP_SIZE   ( BL_HEAP_END - BL_HEAP_START)
// Contagem de mmblock.
#define MMBLOCK_COUNT_MAX    256 
// Isso é usado pelo heap.
#define MMBLOCK_HEADER_SIZE  64 

//fake process struct
struct process_d 
{
   int dummy;
};


/*
 * mmblock_d:
 *     Estrutura para memory blocks.
 *     Temos vários blocos de memória espalhados em lugares diferentes 
 * dentro de um heap.
 *     Essa estrutura é usada pelo bl para controlar as áreas de memória
 * alocadas dinâmicamente dentro do heap do bl. Porém poderemos
 * usar essa mesma estrutura para alocar memória em outros heaps. Como o heap 
 * de um processo ou o heap de um desktop. @todo: Para isso essa estrutura 
 * poderia conter informações sobre qual heap estamos usando. Mas me parece 
 * que o tamanho do header deve ser fixo.
 * @todo: 
 * Os blocos precisam de alguma organização. 
 * Por enquanto, o total é 256 heaps de tamanhos diferentes.
 * Os blocos formarão uma lista encadeada que será percorrida para se 
 * encontrar um bloco vazio. (walk).
 * Importante: 
 * A memória será organizada em bancos, que conterão memória 
 * privada e memória compartilhada. Os blocos de memória e os heaps 
 * precisam estar associadoas aos bancos, que conterão informações sobre 
 * processos e usuários. @todo:
 * Obs: Os bancos estão definidos em bank.h
 * Obs: Um heap de processo tem vários blocos dentro.
 * IMPORTANTE: 
 * Talvez tenhamos algum limite para o tamanho dessa estrutura 
 * em especial. (Não incluir nenhuma variável por enquanto!).
 */ 

struct mmblock_d 
{

	//
	// Essa estrutura é para gerenciar áreas de memória alocadas dinamicamente 
	// dentro do heap do processo kernel. Alocadas em tempo de eecução.
	//

// @todo: 
// Talvez não seja possível mudar essa estrutura. Éla é diferente.
// Portanto não definiremos inada o tipo de objeto que ela é e nem a classe.

	//object_type_t objectType;
	//object_class_t objectClass;

    //Identificadores.	
	unsigned long Header;      //Endereço onde começa o header do heap. *Importante.
	unsigned long headerSize;  //Tamanho do header em bytes.
	unsigned long Id;          //Id do header.
	unsigned long Used;        //Flag 'usado' ou 'livre'.
	unsigned long Magic;       //Magic number. Ver se não está corrompido.
	
	//Status.
	unsigned long Free;           //Se o bloco esta livre ou não.
	
	//Mensuradores. (sizes).	
	unsigned long requestSize;    //Tamanho, em bytes, da área solicitada.
	unsigned long unusedBytes;    //Quantidade de bytes não usados na área do cliente.	
	unsigned long userareaSize;   //Tamanho da área reservada para o cliente. 
	                              //(request size + unused bytes). 

	//@todo: 
	//    Incluir quando possível.
    // Lembrando que talvez o tamanho dessa estrutura seja fixo.
    // Talvez não mudaremos nada no tamanho dela.	
	//struct heap_d *heap;
	
    // User area. 
	// (Onde começa a área solicitada).
	unsigned long userArea;    //Início da área alocada.
	
	// Footer.
	unsigned long Footer;    //Endereço do início do footer.
	
	// Process info.
    // (Pertence à qual processo?).	
	int processId;
	struct process_d *process;
	
	//
	// Continua ?? ... 
	// Talvez não pode.
	//

// IMPORTANTE: 
// Talvez temos algum limite para o tamanho dessa estrutura em especial. 
// Não inluir nada por enquanto.

// navigation
	struct mmblock_d *Prev;
	struct mmblock_d *Next;
};

//see: heap.c
extern struct mmblock_d  *current_mmblock;

//see: heap.c
extern unsigned long mmblockList[MMBLOCK_COUNT_MAX]; 
extern unsigned long heapList[HEAP_COUNT_MAX]; 


// prototypes ====

unsigned long heapAllocateMemory( unsigned long size );
void FreeHeap (void *ptr);
int init_heap(void);

#endif    


