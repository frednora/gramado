
// stdlib.c
// Standard library.

#include <types.h>
#include <errno.h>
#include <stddef.h>
#include <mm.h>
#include <heap.h>
#include <ctype.h>
#include <float.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <rtl/gramado.h> 

static unsigned int randseed=0;
static unsigned long mmblockCount=0;

// -----------------
// Começo do Heap support.
// Na verdade é o gerenciamento de meória necessário para stdlib.c

// Variáveis internas. 
static unsigned long last_valid=0;       // Último heap pointer válido. 
static unsigned long last_size=0;        // Último tamanho alocado.
static unsigned long mm_prev_pointer=0;  // Endereço da úntima estrutura alocada.


// ================================================

unsigned long HEAP_START=0;
unsigned long HEAP_END=0;
unsigned long HEAP_SIZE=0;

// #todo: Padronizar 
unsigned long heapCount=0;  // Conta os heaps da stdlib.

unsigned long heap_start=0;        // Start
unsigned long heap_end=0;          // End
unsigned long g_heap_pointer=0;    // Pointer
unsigned long g_available_heap=0;  // Available 

// Heap pointer:
// Isso deve apontar para o heap buffer atual. 
// Quando acabar o heap atual deve-se chamar o kernel 
// para criar mais heap dentro da working set do processo.
void *Heap;

//static unsigned char HeapBuffer[HEAP_BUFFER_SIZE]; 

// Heap list.
// obs:. heapList[0] = The Kernel Heap!
unsigned long heapList[HEAP_COUNT_MAX];


//
// == Private functions: Prototypes ================================
//

static int stdlib_strncmp( char *s1, char *s2, int len );
static int __init_mm(void);
static int __init_heap(void);
static char *__findenv( const char *name, int *offset );

// ================================================

// #local
// stdlib_strncmp:
// Compara duas strings.
static int stdlib_strncmp ( char *s1, char *s2, int len )
{
    register int n=0;
    n = (int) len;

    while (n>0){
        n--;
        // Return 1.
        if ( *s1 != *s2 ){
            return (int) 1; 
        }
        *s1++; 
        *s2++;
    };
    // Return 2.
    if ( *s1 != '\0' || *s2 != '\0' )
    {
        return (int) 2;
    }
// Return 0.
    return 0;
}

// __init_heap:
// Iniciar a gerência de Heap na libC. 
// See: heap.h
static int __init_heap(void)
{
    int i=0;

    //Globals.
    //...@todo:

    unsigned long Max = (unsigned long) ( (HEAP_BUFFER_SIZE) -1 );

	//HEAP_START = (unsigned long) &HeapBuffer[0];
	//HEAP_END   = (unsigned long) &HeapBuffer[Max];
	//HEAP_SIZE  = (unsigned long) (HEAP_END - HEAP_START); 

	//VAMOS PEGAR O ENDEREÇO DO BUFFER DESSE PROCESSO.

	//int thisprocess_id = (int) stdlib_system_call ( 85, 0, 0, 0); 
	//unsigned char *heaptest = (unsigned char *) stdlib_system_call ( 184, thisprocess_id, 0, 0 );	

//
// Current process
//

// #important
// O kernel tem uma thread em ring0.
// Usada pelo window server.
// Se essa libc for usada por ela, então o pid pode ser
// o pid 0, do kernel.

    int thisprocess_id = (int) gramado_system_call ( 85, 0, 0, 0); 
    //if (thisprocess_id <= 0 ){
    if (thisprocess_id < 0 ){
        debug_print ("__init_heap: [FAIL] thisprocess_id  ~~>  :) \n");
        goto fail;
    }

//
// Heap test
//

// Pegamos o endereço do heap do processo.
// Isso precisa ser um ponteiro em uma região em ring3
// compartilhada com esse processo.

    unsigned char *heaptest = 
        (unsigned char *) gramado_system_call ( 184, thisprocess_id, 0, 0 );
    if ( (void*) heaptest == NULL )
    {
        debug_print ("__init_heap: [FAIL] heaptest \n");
        goto fail;
    }

// #bugbug
// #todo
// Temos que usar uma chamada que pegue o tamanho do heap do processo.
// Pois somente o processo init tem 2mb de heap, usando o extra heap 1.
// Os outros processo possuem apenas 128 KB de heap.
// Precisamos de uma chamada que pega o 'heap size'
// e o heap size deve estar na estrutura do processo.

// See: 
// #define G_DEFAULT_PROCESSHEAP_SIZE (1024*128)

//0x0000000030A00000 para init process
    HEAP_START = 
        (unsigned long) &heaptest[0];

//(1024*1024*2) ); //(HEAP_START + (1024*128) );  //128KB
    HEAP_END = 
        (unsigned long) (HEAP_START + (1024*128) ); 

    HEAP_SIZE = 
        (unsigned long) (HEAP_END - HEAP_START); 

//------

    heap_start  = (unsigned long) HEAP_START;
    heap_end    = (unsigned long) HEAP_END;

// Heap Pointer.
    g_heap_pointer = 
        (unsigned long) heap_start;
// Available heap.
    g_available_heap = 
        (unsigned long) (heap_end - heap_start);

// Counter. '1'?
    heapCount=0;

// ================================

//
// #bugbug: No permission
//
    //#testing heaps permission
    debug_print ("__init_heap: Testing heaps permission \n");    
    
    //Endereço valido somente para processo init
    //if ( heap_start != 0x0000000030A00000 ){
    //    debug_print ("__init_heap: [ERROR] wrong address  \n");
    //    while(1){}
    //}

// ================================

//Test. (Cria e inicializa uma estrutura)
    //heapSetLibcHeap(HEAP_START,HEAP_SIZE);

// #important:
// Último heap pointer válido.

    last_valid = (unsigned long) g_heap_pointer;
    last_size = 0;

// Check Heap Pointer.
    if ( g_heap_pointer == 0 ){
        printf ("__init_heap fail: Heap pointer!\n");
        goto fail;
    }

// Check Heap Pointer overflow.
    if ( g_heap_pointer > heap_end ){
        printf("__init_heap fail: Heap Pointer Overflow!\n");
        goto fail;
    }

// Heap Start.
    if ( heap_start == 0 ){
        printf ("__init_heap fail: HeapStart={%x}\n",heap_start);
        goto fail;
    }

// Heap End.
    if ( heap_end == 0 ){
        printf ("__init_heap fail: HeapEnd={%x}\n",heap_end);
        goto fail;
    }

// Check available heap.
    if ( g_available_heap == 0 )
    {
        //@todo: Tentar crescer o heap.
        printf("__init_heap fail: Available heap\n");
        goto fail;
    }

// Heap list ~ Inicializa a lista de heaps.
    while ( i < HEAP_COUNT_MAX ){
        heapList[i] = (unsigned long) 0;
        i++;
    };

    //KernelHeap = (void*) x??;

// More?

done:
    debug_print("__init_heap: done\n");
    //printf("Done.\n");
    return 0;

// Fail. 
// Falha ao iniciar o heap do kernel.
fail:
    printf("__init_heap: Fail\n");

/*
    printf("Debug: %x %x %x %x \n", 
        kernel_heap_start, 
        kernel_heap_end,
        kernel_stack_start,
        kernel_stack_end);

    refresh_screen(); 
    while(1){}
*/

    return (int) 1;
}


// #local
// __init_mm:
// Inicializa o memory manager.

static int __init_mm(void)
{
    register int i=0;
    int Status = 0;

    //#debug
    debug_print ("__init_mm:\n");

// @todo: 
// Inicializar algumas variáveis globais.
// Chamar os construtores para inicializar o básico.
// @todo: 
// Clear BSS.
// Criar mmClearBSS()

// Heap
    Status = (int) __init_heap();
    if (Status != 0){
        debug_print ("__init_mm: [FAIL] __init_heap\n");
        //printf      ("__init_mm: [FAIL] __init_heap\n");
        return (int) 1;
    }

// Lista de blocos de memória dentro do heap.
    i=0;
    while (i<MMBLOCK_COUNT_MAX){
        mmblockList[i] = (unsigned long) 0;
        i++;
    };

//Primeiro Bloco.
    //current_mmblock = (void *) NULL;

// #importante:
// #inicializando o índice la lista de ponteiros 
// par estruturas de alocação.
// #bugbug: temos que inicializar isso no kernel também.
    mmblockCount = 0;

// Continua...

    //#debug
    debug_print ("__init_mm: done\n");
    //printf      ("__init_mm: done\n");

    return (int) Status;
}

// __findenv:
// Interna de suporte à getenv. 
// Credits: apple open source.

static char *__findenv ( const char *name, int *offset )
{
    size_t len;
    const char *np;
    char **p, *c;  //??

    register int i=0;
    int fail;

    if ( (void *) name == NULL ){
        printf ("__findenv: name NULL\n");
        return (char *) 0;
    }

    if ( (char **) environ == 0 ){
        printf ("__findenv: environ is 0\n");
        return (char *) 0;
    }

// Tamanho da string do argumento.
    for (np = name; *np && *np != '='; ++np){ continue; };

    len = (size_t) (np - name);

    //printf (">>> len  %d \n", len);
    //return (char *) 0;

    for ( p = environ; (c = *p) != NULL; ++p )
    {
        //#obs: isso funcionou mostrando a primeira string do environ.
        //return c + len + 1;
        fail = 0;
        for (i=0; i<len; i++)
        {
             //se um deles for diferente.
             //se todos forem iguais, fail continua sendo 0.
             if ( c[i] != name[i] )
                 fail = 1;
        };
 
        //if ( strncmp ( c, (char *) name, len ) == 0 && c[len] == '=' ) 
        //if ( strncmp2 ( (const char *) c, (const char *) name, len ) == 0 && c[len] == '=' ) 
        
        
        //if ( c[0] == name[0] && c[len] == '=' )  //ok
        if (fail == 0){
            *offset = p - environ;
            // OK.
            return c + len + 1;
        }
    };

    *offset = p - environ;

    printf ("__findenv: overflow\n");

//done:
    return (char *) 0;
    //return NULL;
}

/*
unsigned long heap_set_new_handler( unsigned long address );
unsigned long heap_set_new_handler( unsigned long address )
{
    unsigned long Old;
    Old = kernel_heap_start;
    kernel_heap_start = address;
    return (Old);
};
*/

unsigned long rtGetHeapStart(void)
{
    return (unsigned long) heap_start;
}

unsigned long rtGetHeapEnd(void)
{
    return (unsigned long) heap_end;
}

unsigned long rtGetHeapPointer(void)
{
    return (unsigned long) g_heap_pointer;
}

unsigned long rtGetAvailableHeap(void)
{
    return (unsigned long) g_available_heap;
}

/*
 * heapSetLibcHeap:
 *    Configura o heap usado pela libc em user mode.
 *    Reconfiguração total do heap.
 *    @todo: Salvar em estrutura.
 */

void 
heapSetLibcHeap ( 
    unsigned long HeapStart, 
    unsigned long HeapSize )
{
    struct heap_d *h; 

// Check limits.

    if (HeapStart == 0){
        debug_print("heapSetLibcHeap: HeapStart\n");
        return;
    }

    if (HeapSize == 0){
        debug_print("heapSetLibcHeap: HeapSize\n");
        return;
    }

// start, end, pointer, available.    

    heap_start = (unsigned long) HeapStart; 
    heap_end   = (unsigned long) (HeapStart + HeapSize);

    g_heap_pointer = (unsigned long) heap_start;            
    g_available_heap  = (unsigned long) (heap_end - heap_start); 

// A estrutura fica no início do heap.??!!
    h = (void *) heap_start;

// Configurando a estrutura.
    h->HeapStart = (unsigned long) heap_start;   
    h->HeapEnd   = (unsigned long) heap_end;

    h->HeapPointer = (unsigned long) g_heap_pointer;      
    h->AvailableHeap = (unsigned long) g_available_heap; 

// Configura o ponteiro global em heap.h.
    Heap = (void *) h;

// Lista de heaps.
// Configuração inicial da lista de heaps. 
// Só temos 'um' ainda.
// Configura o primeiro heap da stdlib.
    heapList[0] = (unsigned long) Heap; 
    heapList[1] = (unsigned long) 0;
    heapList[2] = (unsigned long) 0;
    // ...
    //Contagem? ainda em zero.?!
}

/*
 * heapAllocateMemory:
 *     Aloca memória no heap do kernel.
 * IMPORTANTE: 
 * Aloca BLOCOS de memória dentro do heap do processo Kernel.
 * Obs: A estrutura usada aqui é salva onde?
 * @todo: 
 *     Ao fim dessa rotina, os valores da estrutura devem ser armazenas no 
 * header, lá onde foi alocado espaço para o header, assim tem-se informações 
 * sobre o header alocado.
 *  A estrutura header do heap, é uma estrutura e deve ficar antes da 
 *  área desejada. partes={header,client,footer}.
 * 2015 - Created.
 * sep 2016 - Revision.
 * ...
 */
// Called by malloc.

unsigned long heapAllocateMemory (unsigned long size)
{
    struct mmblock_d  *Current;

    debug_print ("heapAllocateMemory: $\n");


// Available heap.
// Se não há heap disponível, não há muito o que fazer.

    if (g_available_heap == 0)
    {
        // @todo: 
        // Tentar crescer o heap para atender o size requisitado.

        //try_grow_heap() ...

        // @todo: 
        // Aqui poderia parar o sistema e mostrar essa mensagem.

        debug_print ("heapAllocateMemory: [FAIL] g_available_heap={0}\n");
        printf      ("heapAllocateMemory: [FAIL] g_available_heap={0}\n");
        return (unsigned long) 0;
        //while(1){};
    }

    //debug_print ("heapAllocateMemory: [2]\n");

// Size limits. (Min, max).
// Se o tamanho desejado é igual a zero.
    if (size == 0){
        debug_print ("heapAllocateMemory: [ERROR] size=0 \n");
        printf      ("heapAllocateMemory: [ERROR] size=0 \n");
        return (unsigned long) g_heap_pointer;
    }

    //debug_print ("heapAllocateMemory: [3]\n");

// Se o tamanho desejado é maior ou igual ao espaço disponível.
// #todo: 
// Tentar crescer o heap para atender o size requisitado.

    if (size >= g_available_heap){
        debug_print ("heapAllocateMemory: [ERROR] size >= g_available_heap\n");
        printf      ("heapAllocateMemory: [ERROR] size >= g_available_heap\n");
        return (unsigned long) 0;
    }

// Salvando o tamanho desejado.
    last_size = (unsigned long) size;

// Contador de blocos.
    //debug_print ("heapAllocateMemory: [4] loop\n");

try_again:

    debug_print ("heapAllocateMemory-r3: try_again\n");

    mmblockCount++;
    if (mmblockCount >= MMBLOCK_COUNT_MAX)
    {
        debug_print ("heapAllocateMemory-r3: [ERROR] mmblockCount limits!\n");
        //printf      ("heapAllocateMemory: [ERROR] mmblockCount limits!\n");
 
        //printf ("heapAllocateMemory: [ERROR] g_heap_pointer=%x\n",
            //g_heap_pointer);
        //printf ("heapAllocateMemory: [ERROR] HEAP_START=%x\n", 
            //HEAP_START);
        //printf ("heapAllocateMemory: [ERROR] HEAP_END=%x\n", 
            //HEAP_END);

        //printf("*lib hang (fatal error)\n");
        
        asm("int $3");
        
        //exit(1);
        //while(1){};

        return (unsigned long) 0;
    }

// Identificadores
// O Header do header do bloco 
// é o inicio da estrutura que o define. (hã???)
// Pointer Limits. 
// (Não vamos querer um heap pointer fora dos limites do heap)
// Se o 'g_heap_pointer' atual esta fora dos limites do heap, então 
// devemos usar o último válido que provavelmente está nos limites.

    //debug_print ("heapAllocateMemory: [5]\n");

// ========================
// Fora dos limites
    if ( g_heap_pointer < HEAP_START || g_heap_pointer >= HEAP_END )
    {

        debug_print ("heapAllocateMemory: [ERROR] g_heap_pointer out of limits\n");
            
        //Checa os limites do último last heap pointer válido.
        if ( last_valid < HEAP_START || last_valid >= HEAP_END )
        {
            debug_print ("heapAllocateMemory: [ERROR] last valid heap pointer limits\n");
            printf      ("heapAllocateMemory: [ERROR] last valid heap pointer limits");
            //printf("*lib hang, (fatal error)\n");
            //refresh_screen();
            //while(1){}
            return 0;
        }

        //Havendo um last heap pointer válido.
        //?? isso não faz sentido.
        g_heap_pointer = (unsigned long) (last_valid + last_size);

        goto try_again;
    }

// Agora temos um 'g_heap_pointer' válido, salvaremos ele.
// 'last_valid' NÃO é global. Fica nesse arquivo.
    last_valid = (unsigned long) g_heap_pointer;

// Criando um bloco.
// Estrutura mmblock_d interna.
// Configurando a estrutura para o bloco atual.
// Obs: 
// A estutura deverá ficar lá no espaço reservado para o header. 
// (antes da area alocada).

    //debug_print ("heapAllocateMemory: [6]\n");

// O endereço do ponteiro da estrutura será o pointer do heap.

    Current = (void *) g_heap_pointer;

    if ( (void *) Current != NULL )
    {
        //debug_print ("heapAllocateMemory: [OK] Current != NULL\n");


        // Endereço onde começa o header.
        Current->Header = (unsigned long) g_heap_pointer;  

        //#debug
        //debug_print (" >>> Breakpoint :)\n");
        //while(1){}

        // Tamanho do header. TAMANHO DA STRUCT.
        // #bugbug: Porque nao usamos sizeof( struct mmblock_d ) ?
        // See: mm.h
        Current->headerSize = MMBLOCK_HEADER_SIZE;
        Current->Id    = mmblockCount;    //Id do mmblock.
        Current->Used  = 1;               //Flag, 'sendo Usado' ou 'livre'.
        Current->Magic = 1234;            //Magic number. Ver se não está corrompido.
        Current->Free  = 0;               //not free.
        // Continua ...

        //
        // Mensuradores. 
        // (tamanhos) (@todo:)
        //

        // @todo:
        // Tamanho da área reservada para o cliente.
        // userareaSize = (request size + unused bytes)
        // Zera unused bytes, já que não foi calculado.

        // User Area base:
        // Onde começa a área solicitada. Isso fica logo depois do header.
        Current->userArea = 
            (unsigned long) (Current->Header + Current->headerSize);

        // Footer:
        // O footer começa no 'endereço do 
        // início da área de cliente' + 'o tamanho dela'.
        // O footer é o fim dessa alocação e início da próxima.
        Current->Footer = 
            (unsigned long) (Current->userArea + size);

        // Heap pointer. 
        //     Atualiza o endereço onde vai ser a próxima alocação.

        //if ( Current->Footer < HEAP_START){
        //    Current->Used = 0;     // Flag, 'sendo Usado' ou 'livre'.
        //    Current->Magic = 0;    // Magic number. Ver se não está corrompido.    
        //    goto try_again;
        //}

        // Obs: 
        // O limite da contagem de blocos foi checado acima.

        // Coloca o ponteiro na lista de blocos.

        mmblockList[mmblockCount] = (unsigned long) Current;

        // Salva o ponteiro do bloco usado como 'prévio'.
        // Obs: 'mm_prev_pointer' não é global, fica nesse arquivo.

        mm_prev_pointer  = (unsigned long) g_heap_pointer;

        // #importante
        // Atualiza o ponteiro. 
        // Deve ser onde termina o último bloco configurado.
        g_heap_pointer = (unsigned long) Current->Footer;

        // Available heap:
        // Calcula o valor de heap disponível para as próximas alocações.
        g_available_heap = 
            (unsigned long) g_available_heap - (Current->Footer - Current->Header);

        //debug_print ("heapAllocateMemory: done\n");

        // Retorna o ponteiro para o início da área alocada.
        // Obs: Esse é o valor que será usado pela função malloc.

        return (unsigned long) Current->userArea;

        // Nothing

// Se o ponteiro da estrutura de mmblock for inválido.
    }else{
        debug_print ("heapAllocateMemory: [FAIL] Current == NULL\n");
        printf      ("heapAllocateMemory: [FAIL] Current\n");
        return (unsigned long) 0;
    };

// #todo: 
// Checar novamente aqui o heap disponível. 
// Se esgotou, tentar crescer.
// #importante
// #todo:
// Colocar o conteúdo da estrutura no lugar alocado para o header.
// O header conterá informações sobre o heap.

    // errado #bugbug.
    //Prev = (void*) mm_prev_pointer;

	//if( (void*) Prev != NULL)
	//{
	 //   if( Prev->Used == 1 && 
	//	    Prev->Magic == 1234 && 
	//		Prev->Free == 0)
	//	{
	//	    Current->Prev = (void*) Prev;
    //        Prev->Next = (void*) Current;
	//	};
	//};
	//

// Se falhamos, retorna 0. Que equivalerá à NULL.
fail:
    debug_print ("heapAllocateMemory: Fail\n");
    return (unsigned long) 0;
}


/*
 * FreeHeap:
 * @todo: Implementar essa função.
 * Objetivo: Liberar o bloco de memória, configurando a sua estrutura.
 * Libera memória.
 * O argumento é a diferença entra o ponteiro antigo e o novo ponteiro 
 * desejado. 
 * Ponteiros do início da área do cliente.
 * ??
 * @todo: FAZER IGUAL O DO KERNEL.
 */
 
unsigned long FreeHeap (unsigned long size)
{
    // #suspensa 
    // #todo
    return (unsigned long) g_heap_pointer;
}

/*
 * libcInitRT:
 *     Inicializa o gerenciamento em user mode de memória virtual
 * para a biblioteca libC99.
 * Obs: 
 * IMPORTANTE: Essa rotina deve ser chamada entes que a biblioteca
 * C seja usada. 
 */
// This routine ws called by crt0() in crt0.c

int libcInitRT (void)
{
    int Status = -1;
    // #debug
    debug_print ("libcInitRT:\n");
    Status = (int) __init_mm();
    if (Status != 0){
        debug_print ("libcInitRT: [FAIL] __init_mm\n");
        return (int) 1; 
    }
    //...
    //#debug
    debug_print ("libcInitRT: done\n");
    return 0;
}

//
// -----------------
// Fim do Heap support.
//


// #todo
char *mktemp(char *template)
{
    debug_print ("mktemp: [TODO]\n");
    return (char *) 0;
}

// Seed rand.
void srand(unsigned int seed)
{
    randseed = (unsigned int) seed;
}

// Generate a random number.
int rand(void)
{
    randseed = (randseed * 1234 + 5);
    return (int) randseed;
}

int random(void){
    return (int) rand();
}

void srandom(unsigned int seed)
{
    srand(seed);
}

/*
// #todo
// gerar número aleatório dentro de uma faixa.
int rtl_rand_in_a_range(int lim_inf, int lim_sup);
int rtl_rand_in_a_range(int lim_inf, int lim_sup)
{
    return (int) (lim_inf + (rand() % lim_sup));
}
*/

/*
void _abort(void);
void _abort(void)
{
    asm volatile("ud2");
    __builtin_unreachable();
}
*/

void abort(void)
{
    //_abort();
    exit(0);
    //exit(1);
}

void stdlib_die(char *str)
{
    debug_print("stdlib_die:\n");

    if ( (void*) str != NULL )
    {
        printf ("stdlib_die: %s \n", str);

        if ( (void*) stderr != NULL ){
            fprintf(stderr,"%s\n",str);
        }
    }

    abort();
}

void *rtl_malloc(size_t size)
{
    if (size<0){
        return NULL;
    }
    if (size==0){
        size++;
    }
    return (void *) malloc(size);
}

/*
 * malloc:
 *     Aloca memória para um programa em user mode. 
 * Explicação:
 *     O objetivo aqui é alocar memória para uma aplicação em user mode.
 * O Heap usado para isso é o Heap do processo ao qual aplicação pertence
 * ou o Heap do desktop ao qual a aplicação pertence.
 *     Obs: Podemos chamar o kernel para que ele aloque memória em um Heap 
 * específico. 
 *     Obs: Podemos chamar um servidor gerente de memória virtual para
 * ele fazer esse trabalho. (Nada impede que o servidor utilize recursos 
 * do kernel).
 *     Obs: VÁRIOS SERVIÇOS DE ALOCAÇÃO DE MEMÓRIA PODEM CONVIVER TANTO EM
 * USER MODE QUANTO KERNEL MODE.
 * @todo: O método usado nessa função ainda não foi definido.
 *        ** POR ENQUANTO, PARA TESTES, ESSA FUNÇÃO ALOCA MEMÓRIA NO HEAP DA 
 *         BIBLIOTECA, QUE É BEM PEQUENO, NA FORMA DE BUFFER (ARRAY)
 * History:
 *     2015 - Created by Fred Nora.
 */

void *malloc(size_t size)
{

// #todo
// Como o alocador local esta com problemas,
// entao trabalharemos nele no futuro.

    //int UseLocalAllocator = TRUE;
    int UseLocalAllocator = FALSE;    //ok

// ===========

    void *ptr;
    unsigned long new_size = ( unsigned long) size;

    //debug_print ("malloc:\n");

    if (size < 0){
        debug_print ("malloc: size\n");
        return NULL; 
    }
    if (size == 0){ 
        new_size = 1;
    }

    if ( UseLocalAllocator == TRUE ){
         ptr = (void *) heapAllocateMemory(new_size);
    }

    if ( UseLocalAllocator == FALSE ){
         ptr = (void *) shAlloc(new_size);
    }

    if ( (void *) ptr == NULL )
    {
        debug_print ("malloc: [FAIL] ptr\n");
        //printf("malloc: falha ao alocar memoria!\n");
        //refresh_screen();
        return NULL;
    }

/*
	if((void*) ret < KERNEL_HEAP_START){
	    printf("malloc: falha ao alocar memoria! Limits\n");
		refresh_screen();
		return NULL;
	};
*/

    return (void *) ptr; 
}


void *xmalloc (size_t size)
{
    void *ptr;

    if (size<=0){
        stdlib_die ("xmalloc: [FAIL] size\n");
    }
    ptr = (void*) malloc(size);
    if( (void*) ptr == NULL ){
        stdlib_die ("xmalloc: [FAIL] ptr\n");
    }

    return (void *) ptr;
}


void *xmemdup (void const *p, size_t s)
{
    void *ptr;

    if( (void*) p == NULL ){
        stdlib_die ("xmemdup: [FAIL] p\n");
    }

    if (s<=0){
        stdlib_die ("xmemdup: [FAIL] s\n");
    }

    ptr = (void*) xmalloc(s);

    if( (void*) ptr == NULL ){
        stdlib_die ("xmemdup: [FAIL] ptr\n");
    }

    return (void *) memcpy(ptr,p,s);
}


char *xstrdup(char const *string)
{
    size_t Size=0;

    // #Atenção
    // strlen já teria problemas de o ponteiro fosse inválido.

    if( (void*) string == NULL ){
        stdlib_die ("xstrdup: [FAIL] string\n");
    }

    Size = strlen (string);

    if (Size<=0){
        stdlib_die ("xstrdup: [FAIL] Size\n");
    }

    Size = (Size + 1);
    
    return (char *) xmemdup (string,Size);
}


void *realloc ( void *start, size_t newsize )
{
    void *newstart;

    newstart = (void *) malloc(newsize);
    
    if ( (void *) newstart == NULL ){
        //free (newstart);
        return NULL;
    }else{

        bcopy (start, newstart, newsize);
   
        //#bugbug: Not the last allocation.
        //free(start);
    
        return newstart;
    };

//fail.
   return NULL;
}


/*
 * free:
 *     The free() function frees the memory space pointed 
 * to by ptr, which must have been returned by a previous call 
 * to malloc(), calloc() or realloc(). 
 * Otherwise, or if free(ptr) has already been called before, 
 * undefined behavior occurs.
 * If ptr is NULL, no operation is performed.
 *  @todo: fazer igual no kernel.
 * Vamos apenas liberar a estrutura para permitir que 
 * outra alocação a use. o GC pode limpar a estrutura ou destrui-la.
 */

void free(void *ptr)
{
    debug_print ("free: [TODO]\n"); 

/*
   //@todo: Copiar o do kernel base.
   //essa rotina foi melhorada la no kernel base.

    int Index;
    struct mmblock_d *Block;	

	//>> If ptr is NULL, no operation is performed.
	if( (void*) ptr == NULL ){
		printf("free fail: null pointer.\n");
		goto fail;
	}
	
	
	//test:
	//Calculando o início do header,dado o argumento, que é
	//o início da área de cliente.

	unsigned long UserAreaStart = (unsigned long) ptr; 
	
	Block = (void*) ( UserAreaStart - MMBLOCK_HEADER_SIZE);
	
	//O início da estrutura de mmblock_d é um valor inválido.
	if( (void*) Block == NULL ){
		printf("free fail: struct pointer.\n");
		goto fail;
	}else{
		
		
		if( Block->Used != 1 ){
			printf("free fail: Used.\n");
		    goto fail;	
		};
			
		if( Block->Magic != 1234 ){
			printf("free fail: Magic.\n");
			goto fail;
		};

		if( Block->userArea != UserAreaStart ){
			printf("free fail: userArea address.\n");
			goto fail;			
		};	
		
		if( Block->Free == 0 ){
		    Block->Free = 1;   //Liberando o bloco para uso futuro.
			goto done;	
		} 
		
		//Se estamos aqui é porque algo deu errado.
        goto fail;
	};
//Nothing.
fail:	
    refresh_screen();
	while(1){}
done:	
*/

   return;
}


/*
void *calloc(size_t num, size_t size);
void *calloc(size_t num, size_t size)
{
    return zmalloc(num*size);
}
*/


void *rtl_calloc (size_t count, size_t size)
{
    size_t t = (count*size);

    if (t == 0)
    {
        count=1;
        size=1;
    }

    return (void *) calloc(count,size);
}

// calloc: 
// Aloca e preenche com zero.
 
void *calloc (size_t count, size_t size)
{
    void *ptr;
    size_t new_size = (size_t) (count * size);

    if (count <= 0){
        new_size = (1*size);
    }
    ptr = (void*) malloc(new_size);
    if ( (void*) ptr != NULL ){
        memset(ptr, 0, new_size);
    }

    return (void*) ptr;
}

void *xcalloc (size_t count, size_t size)
{
    void *ptr;

    if (size <= 0)
    {
        count = 1;
        size  = 8;
    }
    ptr = (void*) calloc(count,size);
    if ( (void*) ptr == NULL ){
        stdlib_die ("xcalloc: [FAIL] ptr\n");
    }

    return (void *) ptr;
}


void *xzalloc (size_t n)
{
    if (n<=0){
        n=1;
    }
    return (void *) xcalloc(n,1);
}

// zmalloc:
// Alloca memória e zera o conteúdo. 
void *zmalloc (size_t size)
{
    void *ptr;

    if(size<=0){
        size=1;
    }

    ptr = (void*) malloc(size);

    if ( (void *) ptr == NULL ){
        //free (ptr);
        return NULL;
    }else{
        memset (ptr, 0, size);
        return (void*) ptr;
    };

// fail
    return NULL;
}

// system:
// chama um comando com base em uma cmdline.
// #todo
// No implemented yet.
// #todo
// Call the shell application?

int system (const char *command)
{

// #todo: 
// Checar se comando é válido, se os primeiros caracteres
// são espaço. Ou talvez somente compare, sem tratar o argumento.
// #todo:
// Criar rotina para pular os caracteres em branco no início do comando.
// #todo: version, ...
// OBS: 
// ESSES SÃO OS COMANDOS DO SISTEMA, USADOS POR TODOS OS PROGRAMAS
// QUE INCLUIREM A LIBC. 

// test - Exibe uma string somente para teste.
    if ( stdlib_strncmp ( (char *) command, "test", 4 ) == 0 )
    {
        printf("system: Testing commands ...\n");
        goto exit;
    }
  
// ls - List files in a folder.
    if ( stdlib_strncmp ( (char *) command, "ls", 2 ) == 0 )
    {
        printf("system: @todo: ls ...\n");
        goto exit;
    }

// makeboot - Cria arquivos e diretórios principais.
    if ( stdlib_strncmp ( (char *) command, "makeboot", 8 ) == 0 )
    {
        printf("system: @todo: makeboot ...\n");
        //ret_value = fs_makeboot();
        //if(ret_value != 0){
        //    printf("shell: makeboot fail!");
        //};
        goto exit;
    }

// format.
    if ( stdlib_strncmp ( (char *) command, "format", 6 ) == 0 )
    {
        printf("system: @todo: format ...\n");
        //fs_format(); 
        goto exit;
    }

// debug.
    if ( stdlib_strncmp ( (char *) command, "debug", 5 ) == 0 )
    {
        printf("system: @todo: debug ...\n");
        goto exit;
    }

// dir.
    if ( stdlib_strncmp ( (char *) command, "dir", 3 ) == 0 )
    {
        printf("system: @todo: dir ...\n");
        //fs_show_dir(0); 
        goto exit;
    }

// newfile.
    if ( stdlib_strncmp ( (char *) command, "newfile", 7 ) == 0 )
    {
        printf("system: ~newfile - Create empty file.\n");
        //fs_create_file( "novo    txt", 0);
        goto exit;
    }

// newdir.
    if ( stdlib_strncmp ( (char *) command, "newdir", 7 ) == 0 )
    {
        printf("system: ~newdir - Create empty folder.\n");
        //fs_create_dir( "novo    dir", 0);
        goto exit;
    }

// mbr - Testa mbr.
    if ( stdlib_strncmp ( (char *) command, "mbr", 3 ) == 0 )
    {
        printf("system: ~mbr\n");
        //testa_mbr();
        goto exit;
    }

// root - Testa diretório /root.
    if ( stdlib_strncmp ( (char *) command, "root", 4 ) == 0 )
    {
        printf("system: ~/root\n");
        //testa_root();
        goto exit;
    }

// start.
    if ( stdlib_strncmp ( (char *) command, "start", 5 ) == 0 )
    {
        printf("~start\n");
        goto exit;
    }

// help.
    if ( stdlib_strncmp ( (char *) command, "help", 4 ) == 0 )
    {
        //printf(help_string);
        //print_help();
        goto exit;
    }

// cls.
    if ( stdlib_strncmp ( (char *) command, "cls", 3 ) == 0 )
    {
        //black
        //api_clear_screen(0);
        goto exit;
    }

// save.
    if ( stdlib_strncmp ( (char *) command, "save", 4 ) == 0 )
    {
        printf("system: ~save root\n");
        goto exit;
    }

// install.
// muda um arquivo da area de transferencia para 
// o sistema de arquivos...
    if ( stdlib_strncmp ( (char *) command, "install", 7 ) == 0 )
    {
        printf("system: ~install\n");
        //fs_install();
        goto exit;
    }

// boot - Inicia o sistema.
    if ( stdlib_strncmp ( (char *) command, "boot", 4 ) == 0 )
    {
        printf("system: ~boot\n");
        //boot();
        goto exit;
    }

// service
    if ( stdlib_strncmp ( (char *) command, "service", 7 ) == 0 )
    {
        printf("system: ~service\n");
        //test_services();
        goto exit;
    }

// slots - slots de processos ou threads.
    if ( stdlib_strncmp ( (char *) command, "slots", 5 ) == 0 )
    {
        printf("system: ~slots - mostra slots \n");
        //mostra_slots();
        goto exit;
    }

// Continua ...

// exit - Exit the current program
    if ( stdlib_strncmp ( (char *) command, "exit", 4 ) == 0 )
    {
        //exit(exit_code);
        //exit(0);
        printf ("#todo: ~exit");
        goto fail;
    }

// reboot.
    if ( stdlib_strncmp ( (char *) command, "reboot", 6 ) == 0 )
    {
        //stdlib_system_call ( 110, (unsigned long) 0, (unsigned long) 0, 
        //    (unsigned long) 0 );

        gramado_system_call ( 
            110, 
            (unsigned long) 0, 
            (unsigned long) 0, 
            (unsigned long) 0 );

        //apiReboot(); 
        goto fail;
    }

// shutdown.
    if ( stdlib_strncmp ( (char *) command, "shutdown", 8 ) == 0 )
    {
        //apiShutDown();
        goto fail;
    }

// #todo
// Call the shell application?
// #todo: exec
//:default
    printf("system: Unknown command!\n");

// o que devemos fazer aqui é pegar o nome digitado e comparar
// com o nome dos arquivos do diretório do sistema. se encontrado,
// devemos carregar e executar.

// Fail. Palavra não reservada.	
fail:
	printf("system: FAIL!\n");
    return (int) 1;

// #todo: 
// Esse exit como variavel local precisa mudar de nome
// para não confundir com a função exit de sair do processo.
// uma opção é usar 'done:'. 
exit:    
    return (int) 0;
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 *	used for environment searching: see getenv
 */

//unix v7 
//used by getenv
char *nvmatch ( char *s1, char *s2 )
{
    while (*s1 == *s2++)
        if (*s1++ == '=')
            return(s2);
    if (*s1 == '\0' && *(s2-1) == '=')
        return (s2);

    return NULL;
}

//unix v7
char *v7_getenv(char *name)
{
    register char **p = environ;
    register char *v;

    while (*p != NULL)
        if ((v = nvmatch(name, *p++)) != NULL)
            return(v);
    return(NULL);
}

/*
void bak2for(char *str);
void bak2for(char *str) 
{
    //#todo: check pointer validation
    
	while (*str) 
	{
		if (*str == '\\')
			*str = '/';
		
		str++;
	};
}
*/

// getenv:
// Credits: Apple open source.

char *getenv (const char *name)
{
    int offset=0;
    char *result;

    debug_print ("getenv:\n");

    //_DIAGASSERT(name != NULL);

    //printf ("getenv2: %s \n", (const char *) name);
    //return NULL;

    if ( (void *) name == NULL )
    {
        debug_print ("getenv: [FAIL] name\n");
        return (char *) 0;
    }

    if (*name == 0)
    {
        debug_print ("getenv: [FAIL] *name\n");
        return (char *) 0;
    }

    //rwlock_rdlock(&__environ_lock);
    result = __findenv (name, &offset);
    //rwlock_unlock(&__environ_lock);

    //debug_print ("getenv: done\n");
    return (char *) result;
}

//#todo
int setenv (const char *name, const char *value, int overwrite)
{
    //debug_print("setenv: [TODO]\n"); 

    if ( (void *) name == NULL ){
        debug_print ("setenv: [FAIL] name\n");
        return -1;
    }

    if (*name == 0){
        debug_print ("setenv: [FAIL] *name\n");
        return -1;
    }

//
// value
//

    if ( (void *) value == NULL ){
        debug_print ("setenv: [FAIL] value\n");
        return -1;
    }

    if (*value == 0){
        debug_print ("setenv: [FAIL] *value\n");
        return -1;
    }

// #todo: ...
    return (int) (-1);
}

//#todo
int unsetenv (const char *name)
{
    debug_print("unsetenv: [TODO]\n"); 
    return (int) (-1);
}

// #todo
int clearenv(void)
{
    //size_t environ_size = 0;
    //for (; environ[environ_size]; ++environ_size) {
    //    environ[environ_size] = NULL;
    //}
    //*environ = NULL;
    
    return 0;
}

/*
int atoi2(char const* str);
int atoi2(char const* str)
{
    long value = strtol(str, NULL, 10);
    if (value > INT_MAX) {
        return INT_MAX;
    }
    return value;
}
*/

// atoi:
int atoi(const char *str)
{
    int rv=0; 
    char sign = 0;

    /* skip till we find either a digit or '+' or '-' */
    while (*str) 
    {
        if (*str <= '9' && *str >= '0')
            break;

        if (*str == '-' || *str == '+') 
            break;

        str++;
    }; 

    if (*str == '-')
        sign=1;

    // sign = (*s == '-');
    if (*str == '-' || *str == '+') 
        str++;

    while (*str && *str >= '0' && *str <= '9') 
    {
        rv = (rv * 10) + (*str - '0');
        str++;
    };


    if (sign){ 
        return (int) (-rv);
    }else{ 
        return (int) (rv);
    };

    //return (int) (sign ? -rv : rv);
}


/*
 *itoa simplificado. apple open source.
 */
/*
#define INT_DIGITS 19		//enough for 64 bit integer 
char *itoa (int i)
{
  //Room for INT_DIGITS digits, - and '\0' 
  
  //static char buf[INT_DIGITS + 2];
  char buf[INT_DIGITS + 2];  
  
  char *p = buf + INT_DIGITS + 1;	//points to terminating '\0' 
  
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			// i < 0 
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
};
*/

// reverse:  
// reverse string s in place 
void reverse (char s[])
{
    register int i=0; 
    int j=0;
    char c=0;

    for ( i=0, j = strlen(s)-1; 
          i<j; 
          i++, j-- )
    {
        c = s[i];
 
        s[i] = s[j];
        s[j] = c;
    };
}

// itoa:  
// convert n to characters in s.
void itoa(int n, char s[])
{
// #bugbug: 
// What is the size of s[]?

    int i=0; 
    int sign=0;

// Record the sign and make 'n' sopitive.
    if ((sign = n) < 0)
    {
        n = -n;
    }

/* generate digits in reverse order */
/* get next digit */
/* delete it */
    i=0;
    do {
        s[i++] = n%10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0){
        s[i++] = '-';
    }
    s[i] = '\0';

    reverse(s);
}

/*
double atof (const char *ascii);
double atof (const char *ascii)
{
    return (strtod(ascii, NULL));
}
*/

/*
double
strtod (char *str, char **ptr)
{
  char *p;
  if (ptr == (char **)0)
    return atof (str);
  
  p = str;
  
  while (ISSPACE (*p))
    ++p;
  
  if (*p == '+' || *p == '-')
    ++p;
  // INF or INFINITY.  
  if ((p[0] == 'i' || p[0] == 'I')
      && (p[1] == 'n' || p[1] == 'N')
      && (p[2] == 'f' || p[2] == 'F'))
    {
      if ((p[3] == 'i' || p[3] == 'I')
          && (p[4] == 'n' || p[4] == 'N')
          && (p[5] == 'i' || p[5] == 'I')
          && (p[6] == 't' || p[6] == 'T')
          && (p[7] == 'y' || p[7] == 'Y'))
        {
          *ptr = p + 8;
          return atof (str);
        }
      else
        {
          *ptr = p + 3;
          return atof (str);
        }
    }
  // NAN or NAN(foo).  
  if ((p[0] == 'n' || p[0] == 'N')
      && (p[1] == 'a' || p[1] == 'A')
      && (p[2] == 'n' || p[2] == 'N'))
    {
      p += 3;
      if (*p == '(')
        {
          ++p;
          while (*p != '\0' && *p != ')')
            ++p;
          if (*p == ')')
            ++p;
        }
      *ptr = p;
      return atof (str);
    }

  // digits, with 0 or 1 periods in it.  
  if (ISDIGIT (*p) || *p == '.')
    {
      int got_dot = 0;
      while (ISDIGIT (*p) || (!got_dot && *p == '.'))
        {
          if (*p == '.')
            got_dot = 1;
          ++p;
        }
      // Exponent.  
      if (*p == 'e' || *p == 'E')
        {
          int i;
          i = 1;
          if (p[i] == '+' || p[i] == '-')
            ++i;
          if (ISDIGIT (p[i]))
            {
              while (ISDIGIT (p[i]))
                ++i;
              *ptr = p + i;
              return atof (str);
            }
        }
      *ptr = p;
      return atof (str);
    }
  // Didn't find any digits.  Doesn't look like a number.  
  *ptr = str;
  return 0.0;
}
*/


// ok.
int abs(int j)
{
    int result = (int) (j < 0 ? -j : j);
    return (int) result;
}

/*
int rtl_abs(int i);
int rtl_abs(int i)
{
  if (i<0)
    return -i;
  else
    return i;
}
*/


/*
int rtl_sign(int i);
int rtl_sign(int i)
{
  if (i<0)
    return -1;
  else if (!i)
    return 0;
  else
    return 1;
}
*/


/*
DWORD to_hex8(char *st);
DWORD to_hex8(char *st)
{
  DWORD result=0,i;
  char ch;
  for (i=0;i<8;i++) {
    result=result*16;
    ch=st[i];
    if (ch>='0' && ch <='9')
      result=result+ch-'0';
    else if (ch>='A' && ch<='F')
      result=result+ch-'A'+10;
    else if (ch>='a' && ch<='f')
      result=result+ch-'a'+10;
  }
  return result;
}
*/


/*
 // dumpo
 //credits: HOPPY OS
void d_common(BYTE *buf,DWORD count,bool memory);  //Hex Dump
void d_common(BYTE *buf,DWORD count,bool memory)  //Hex Dump
{
  DWORD i,j,k;
  for (i=0;i<count;i=i+16) {
    if (memory)
      ? buf+i," ";
    else
      ? i," ";
    for (j=0;j<16;j++) {
      if (j==8) ? "- ";
      k=i+j;
      if (k<count)
	? buf[k]," ";
      else
	spaces(3);
    }
    space;
    for (j=0;j<16;j++) {
      k=i+j;
      if (k<count) {
	if (buf[k]>=0x20 && buf[k]<=0x7E)
	  putchar(buf[k]);
	else
	  ? ".";
      }
    }
    crlf;
  }
}
*/

//================================================


// strtod:
// String to double.
// This is a worker for some other routines.
// see:
// https://man7.org/linux/man-pages/man3/strtod.3.html
double strtod(const char *nptr, char **endptr)
{
// #atention:
// We are supporting floating point only in the libc for desktop/.
// see: Makefile flags.

    // #todo
    // Not implemented yet.
    return (double) 0.0;
}

// strtof
// String to float.
float strtof(const char *str, char **endptr)
{
    return (float) strtod(str, endptr);
}

// atof
// ascii to float.
double atof(const char *str)
{
    if ( (void*) str == NULL ){
        return (double) 0.0;
    }
    return (double) strtod(str,NULL);
}

//================================================



/*
void abort();
void abort()
{
    //#todo
    exit (1);
}
*/


long labs(long j)
{
    long result = (long) (j < 0 ? -j : j);
    return (long) result;
}

//mkstemp(): 4.3BSD, POSIX.1-2001.
int mkstemp(char *template)
{
    debug_print("mktemp: [TODO]\n"); 
    return -1; 
}

//mkostemp() and mkostemps(): are glibc extensions.
int mkostemp(char *template, int flags)
{ 
    debug_print("mkostemp: [TODO]\n"); 
    return -1; 
}

//mkstemps(): unstandardized, but appears on several other systems.
int mkstemps(char *template, int suffixlen)
{ 
    debug_print("mktemps: [TODO]\n"); 
    return -1; 
}

//mkostemp() and mkostemps(): are glibc extensions.
int mkostemps(char *template, int suffixlen, int flags)
{ 
    debug_print("mkostemps: [TODO]\n"); 
    return -1; 
}

/*
static char ptsname_buf[32];
char* ptsname(int fd)
{
    if (ptsname_r(fd, ptsname_buf, sizeof(ptsname_buf)) < 0)
        return nullptr;
    return ptsname_buf;
}
*/

// ptsname():
// POSIX.1-2001, POSIX.1-2008.
//ptsname() is part of the UNIX 98 pseudoterminal support.
//The ptsname() function returns the name of the slave pseudoterminal
//       device corresponding to the master referred to by fd.
char *ptsname(int fd)
{
    debug_print("ptsname: [TODO]\n"); 

    if (fd<0)
    {
        errno = EBADF;
        return NULL;
    }

    //#todo

    return NULL; 
}

//ptsname_r() is a Linux extension, that is proposed for inclusion in
//the next major revision of POSIX.1
// The ptsname_r() function is the reentrant equivalent of ptsname().
// It returns the name of the slave pseudoterminal device as a null-
// terminated string in the buffer pointed to by buf.  The buflen
//argument specifies the number of bytes available in buf.
int ptsname_r(int fd, char *buf, size_t buflen)
{ 
    debug_print("ptsname_r: [TODO]\n"); 

    if(fd<0)
    {
        errno = EBADF;
        return -1;;
    }

    if( (void*) buf == NULL )
    {
        errno = EINVAL;
        return -1;;
    }

    //#todo buflen

    return -1;   //use syscall !!
}



/*
int abs(int i)
{
    return i < 0 ? -i : i;
}
*/

//serenity os
/*
int mkstemp(char* pattern)
{
    char* path = mktemp(pattern);

    int fd = open(path, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR); // I'm using the flags I saw glibc using.
    if (fd >= 0)
        return fd;

    return -1;
}
*/



//serenity os
//todo: struct ?
/*
div_t div(int numerator, int denominator)
{
    div_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;

    if (numerator >= 0 && result.rem < 0) {
        result.quot++;
        result.rem -= denominator;
    }
    return result;
}
*/


//serenity os
//todo: struct ?
/*
ldiv_t ldiv(long numerator, long denominator)
{
    ldiv_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;

    if (numerator >= 0 && result.rem < 0) {
        result.quot++;
        result.rem -= denominator;
    }
    return result;
}
*/


/*
ldiv_t ldiv(long num, long den);
ldiv_t ldiv(long num, long den)
{
	return (ldiv_t){ num/den, num%den };
}
*/

//posix_openpt - open a pseudoterminal device 
//posix_openpt() is part of the UNIX 98 pseudoterminal support.
//This function is specified in POSIX.1-2001. 
int posix_openpt (int flags)
{
    debug_print("posix_openpt: [FIXME]\n"); 
    return (int) open ("/dev/ptmx", flags, 0);
}

//grantpt - grant access to the slave pseudoterminal 
//POSIX.1-2001. 
int grantpt(int fd)
{ 
    debug_print("grantpt: [TODO]\n");

    if(fd<0)
    {
        errno = EBADF;
        return -1;
    }

    //#todo

    return -1; 
}

//POSIX.1-2001. 
//unlockpt - unlock a pseudoterminal master/slave pair 
int unlockpt(int fd)
{ 
    debug_print("unlockpt: [TODO]\n"); 

    if(fd<0)
    {
        errno = EBADF;
        return -1;
    }

    return -1; 
}


/*
int gramado_unlockpt(int fd);
int gramado_unlockpt(int fd)
{
    int unlock = 0;
    if(fd<0)
    {
        errno = EBADF;
        return -1;
    }
    return ioctl(fd, TIOCSPTLCK, &unlock);
}
*/

/*
uint32_t bswap_32(uint32_t x);
uint32_t bswap_32(uint32_t x)
{
    return x>>24 | x>>16&0xff00 | x<<16&0xff0000 | x<<24;
}
*/

// libgen stuff
// from musl
/*
The functions dirname() and basename() break a null-terminated 
 pathname string into directory and filename components.
  In the usual case, dirname() returns the string up to, 
 but not including, the final '/', and basename() returns the 
 component following the final '/'. Trailing '/' characters are 
 not counted as part of the pathname. 

#todo: These two functions go to libgen
See: https://linux.die.net/man/3/basename
*/

/*
char *basename(char *s);
char *basename(char *s)
{
	size_t i;
	if (!s || !*s) return ".";
	i = strlen(s)-1;
	for (; i&&s[i]=='/'; i--) s[i] = 0;
	for (; i&&s[i-1]!='/'; i--);
	return s+i;
}
*/

/*
char *dirname(char *s);
char *dirname(char *s)
{
	size_t i;
	if (!s || !*s || !strchr(s, '/')) return ".";
	i = strlen(s)-1;
	for (; i&&s[i]=='/'; i--);
	for (; i&&s[i-1]!='/'; i--);
	for (; i&&s[i-1]=='/'; i--);
	if (!i && *s=='/') i++;
	s[i] = 0;
	return s;
}
*/

//getpt - open the pseudoterminal master (PTM) 
//getpt() is glibc-specific; use posix_openpt(3) instead. 
int getpt(void)
{
    debug_print("getpt: [TODO]\n"); 
    return -1; 
}

//static char const* __progname = NULL;

const char *getprogname(void)
{
    debug_print("getprogname: [TODO]\n");
    return NULL;
    //return __progname;
}

void setprogname(const char *progname)
{
    //for (int i = strlen(progname) - 1; i >= 0; i--) {
    //    if (progname[i] == '/') {
    //        __progname = progname + i + 1;
    //        return;
    //    }
    //}

    //__progname = progname;
}


void *bsearch ( 
    const void *key, 
    const void *base, 
    size_t nmemb,
    size_t size,
    int (*compar)(const void *, const void *)
    )
{
    debug_print("bsearch: [TODO]\n");
    return NULL;
}


void 
qsort (
    void *base, 
    size_t nmemb, 
    size_t size,
    int (*compar)(const void *, const void *)
    )
{
    debug_print("qsort: [TODO]\n");
}

void 
qsort_r (
    void *base, 
    size_t nmemb, 
    size_t size,
    int (*compar)(const void *, const void *, void *),
    void *arg 
    )
{
    debug_print("qsort_r: [TODO]\n");
}


// See:
// https://linux.die.net/man/3/putenv
int putenv(char *string)
{
    int rval=0;
    register char *p; 
    register char *equal;

    if ( (void*) string == NULL )
        return -1;
    
    if (*string == 0)
        return -1;

    if (!(p = strdup(string)))
        return(1);

    if (!(equal = index(p, '='))) 
    {
        //free(p);
        return(1);
    }

    *equal = '\0';

    rval = setenv(p, equal + 1, 1);
    
    //free(p);

    return (int) rval;
}

//
// End
//

