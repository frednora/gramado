/*
 * File mm.h 
 *
 * Descrição:
 *     Suporte para libc99 em user mode.
 */
 
//
// MM BLOCK.
// 

//Tamanho do header do bloco.
//#define MMBLOCK_HEADER_SIZE   64
#define MMBLOCK_HEADER_SIZE   128

// Contagem de mmblock.
//#define MMBLOCK_COUNT_MAX    256 
//#define MMBLOCK_COUNT_MAX  (1024)
#define MMBLOCK_COUNT_MAX  (2048)
//#define MMBLOCK_COUNT_MAX  (2*4096)


//
// mm block support.
//

// Conta os blocos de memória dentro de um heap. 
// #test: Moved to stdlib.c
//unsigned long mmblockCount;         
 

/*
 * mmblock_d:
 *     Estrutura para memory blocks.
 *     Temos vários blocos de memória espalhados em lugares diferentes 
 *     dentro de um heap.
 * 
 *     @todo: 
 *         Os blocos precisam de alguma organização.
 *         Por enquanto, o total é 256 heaps de tamanhos diferentes.
 *
 *         Os blocos formarão uma lista encadeada que será percorrida 
 *         para se encontrar um bloco vazio ... walk ...
 *
 *         Um heap de processo tem vários blocos dentro.
 */ 
typedef struct mmblock_d mmblock_descriptor_t;
struct mmblock_d 
{
    // Identificadores.
	unsigned long Header;      // Endereço onde começa o header do heap.
	unsigned long headerSize;  // Tamanho do header em bytes.
	unsigned long Id;          // Id do header.
	unsigned long Used;        // Flag 'usado' ou 'livre'.
	unsigned long Magic;       // Magic number. Ver se não está corrompido.
	
	unsigned long Free;       //se o bloco esta livre ou não
	
	
	// Mensuradores. (sizes) (tamanhos)	
	unsigned long requestSize;  // Tamanho, em bytes, da área solicitada.
	unsigned long unusedBytes;  // Quantidade de bytes não usados na área do cliente.	
	unsigned long userareaSize; // Tamanho da área reservada para o cliente. 
	                            // (request size + unused bytes). 
	
	//
    // User area. (Onde começa a área solicitada).
	//
	
	unsigned long userArea;  // Início da área alocada.
	
	
	//
	// Footer.
	//
	
	unsigned long Footer;  // Endereço do início do footer.
	
	
	//
	// Process. (Pertence à qual processo??)
	//
	
	int processId;
	
	struct process_d *process;
	
	struct mmblock_d *Next;
	struct mmblock_d *Prev;
};

struct mmblock_d  *current_mmblock;

//mmblock_descriptor_t *current_mmblock;
//mmblock_descriptor_t *SystemCache_mmblock;
//mmblock_descriptor_t *ProcessCache_mmblock;
//...

 
//Lista de blocos. 
//lista de blocos de memória dentro de um heap.
//@todo: na verdade temos que usar lista encadeada. 
unsigned long mmblockList[MMBLOCK_COUNT_MAX]; 

//
// End.
//

