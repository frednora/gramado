// heap.c
// Heap support for OS loader in 32bit.
// 2015 - Created by Fred Nora.

#include "../bl.h"  


// globals

unsigned long heapCount=0;         //Conta os heaps do sistema.
unsigned long bl_heap_start=0;     //Start.
unsigned long bl_heap_end=0;       //End.
unsigned long g_heap_pointer=0;    //Pointer.
unsigned long g_available_heap=0;  //Available.
unsigned long mmblockCount=0;      //Conta os blocos de memória dentro de um heap. 

// see: heap.h
struct mmblock_d  *current_mmblock;

// see: heap.h
unsigned long mmblockList[MMBLOCK_COUNT_MAX]; 
unsigned long heapList[HEAP_COUNT_MAX]; 

//Variáveis internas. 
//todo: use static?

//int mmStatus;
unsigned long last_valid=0;         // Último heap pointer válido. 
unsigned long last_size=0;          // Último tamanho alocado.
unsigned long mm_prev_pointer=0;    // Endereço da úntima estrutura alocada.


/*
 * heapAllocateMemory:
 *     Aloca memória no heap do bl.
 * IMPORTANTE: 
 *     Aloca BLOCOS de memória dentro do heap do bl.
 * @todo: 
 *     ?? Ao fim dessa rotina, os valores da estrutura devem ser 
 * armazenas no header, lá onde foi alocado espaço para o header, 
 * assim tem-se informações sobre o header alocado. ??
 *  A estrutura header do heap, é uma estrutura e deve ficar antes 
 * da área desejada. Partes={ header,client,footer }.
 * Obs: 
 *     ?? A estrutura usada aqui é salva onde, ou não é salva ??
 * IN:  size in bytes
 * OUT: address if success. 0 if fail.
 * History:
 *     2015 - Created by Fred Nora.
 */

unsigned long heapAllocateMemory(unsigned long size)
{
    struct mmblock_d *Current;

    // @todo: Aplicar filtro.
    // Aqui podemos checar se o quantidade de heap disponível
    // está coerente com o tamanho do heap. Se essa quantidade
    // for muito grande, maior que o heap total, então temos um problema.
 
    // Se não há espaço disponível no heap, não há muito o que fazer.
    // Uma opção seria tentar almentar o heap, se isso for possível.

    // Available heap.
    if (g_available_heap == 0)
    {
        //
        // @todo: Tentar crescer o heap para atender o size requisitado.
        //

        //try_grow_heap() ...

        //
        // @todo: Aqui poderia parar o sistema e mostrar essa mensagem.
        //

        printf ("heapAllocateMemory fail: g_available_heap={0}\n");
        goto fail;
    }


    // Size limits. 
    // (Min, max).

    // Se o tamanho desejado for igual a zero.
    // #todo: 
    // Aqui podemos converter o size para o tamanho mínimo.
    // não há problema nisso.
    
    if ( size == 0 )
    {
        //size = 1;
        printf ("heapAllocateMemory error: size={0}\n");
        refresh_screen ();
        
        // ?? NULL seria o retorno para esse caso ??
        return (unsigned long) g_heap_pointer;
    }

    // Se o tamanho desejado é maior ou 
    // igual ao espaço disponível.

    if ( size >= g_available_heap )
    {
        //
        // @todo: Tentar crescer o heap para atender o size requisitado.
        //

        //try_grow_heap() ...

        printf ("heapAllocateMemory error: size >= g_available_heap\n");
        goto fail;
    }
 

    //Salvando o tamanho desejado.
    last_size = (unsigned long) size;
    
    //
    // Contador de blocos.
    //

try_again:

    mmblockCount++;  

// #bugbug
// Mesmo tendo espaço suficiente no heap, estamos chegando 
// nesse limite de indices.
// Obs: Temos um limite para a quantidade de índices 
// na lista de blocos.

    if (mmblockCount >= MMBLOCK_COUNT_MAX){
        printf("heapAllocateMemory: MMBLOCK_COUNT_MAX\n");
        bl_die();
    }

// #importante
// A variável 'Header', no header do bloco, 
// é o início da estrutura que o define. 'b->Header'. 
// Ou seja, o endereço da variável marca o início da estrutura.
// Pointer Limits:
// ( Não vamos querer um heap pointer fora dos limites 
//   do heap do bl ).
// Se o 'g_heap_pointer' atual esta fora dos limites do heap, 
// então devemos usar o último válido, que provavelmente está 
// nos limites. ?? #bugbug: Mas se o último válido está sendo 
// usado por uma alocação anterior. ?? Temos flags que 
// indiquem isso ??
// #importante: 
// O HEAP POINTER TAMBÉM É O INÍCIO DE UMA ESTRUTURA. 
// NESSA ESTRUTURA PODEMOS SABER SE O HEAP ESTA EM USO OU NÃO.
// ISSO SE APLICA À TENTATIVA DE REUTILIZAR O ÚLTIMO HEAP POINTER VÁLIDO.

// Se estiver fora dos limites.
    if ( g_heap_pointer < BL_HEAP_START || 
         g_heap_pointer >= BL_HEAP_END )
    {
        // #bugbug: ?? Como saberemos, se o último válido,
        // não está em uso por uma alocação anterior. ??

        //Checa os limites o último last heap pointer válido.
        if ( last_valid < BL_HEAP_START || 
             last_valid >= BL_HEAP_END )
        {
            printf("heapAllocateMemory: last_valid\n");
            bl_die();
        }

        // #todo: Checar a disponibilidade desse último válido.
        // Ele é válido, mas não sabemos se está disponível.

		//Havendo um last heap pointer válido.
		//?? isso não faz sentido.
        g_heap_pointer = (unsigned long) last_valid + last_size;
        goto try_again;
    }

// Agora temos um 'g_heap_pointer' válido, salvaremos ele.
// 'last_valid' NÃO é global. Fica nesse arquivo. 
    last_valid = (unsigned long) g_heap_pointer;

    // #importante:
    // Criando um bloco, que é uma estrutura mmblock_d.
    // Estrutura mmblock_d interna.
    // Configurando a estrutura para o bloco atual.
    //
    // Obs: A estutura deverá ficar lá no espaço reservado 
    // para o header. (Antes da area alocada).
    //
    // Current = (void*) g_heap_pointer;

// ## importante ##
// O endereço do ponteiro da estrutura será o pointer do heap.
    Current = (void *) g_heap_pointer;    
    if ((void *) Current != NULL)
    {
        // #importante:
		// Obs: Perceba que 'Current' e 'Current->Header' 
		// devem ser iguais. 

		// Identificadores básicos:
		// Endereço onde começa o header.
		// Tamanho do header. (*TAMANHO DA STRUCT).
		// Id do mmblock. (Índice na lista)
		// used and magic flags.
		// 0=not free 1=FREE (*SUPER IMPORTANTE)

        Current->Header = (unsigned long) g_heap_pointer;  
        Current->headerSize = MMBLOCK_HEADER_SIZE;         
        Current->Id = mmblockCount;                        
        Current->Used = 1;                
        Current->Magic = 1234;            
        Current->Free = 0;                
        //Continua...

        //
        // Mensuradores. (tamanhos) (@todo:)
        //

        //
        // @todo:
        // Tamanho da área reservada para o cliente.
        // userareaSize = (request size + unused bytes)
        // Zera unused bytes, já que não foi calculado.
        //

        //
        // User Area base:
        // *Onde começa a área solicitada. 
        // *Fácil. Isso fica logo depois do header.
        //

        // Obseve que 'Current->headerSize' é igual 
        // a 'MMBLOCK_HEADER_SIZE'
        // E que 'Current->headerSize' é o início da estrutura.

        Current->userArea = 
            (unsigned long) (Current->Header + Current->headerSize);

        // Footer:
        // >> O footer começa no 
        // 'endereço do início da área de cliente' + 'o tamanho dela'.
        // >> O footer é o fim dessa alocação e início da próxima.
        //
		// #bugbug: Penso que aqui deveríamos considerar 
		// 'userareaSize' como tamanho da área de cliente, 
		// esse tamanho equivale ao tamanho solicitado mais o 
		// tanto de bytes não usados.
		// Obs: Por enquanto o tamanho da área de cliente tem 
		// apenas o tamanho do espaço solicitado.

        Current->Footer = 
            (unsigned long) (Current->userArea + size);

        // Heap pointer. 
        //     Atualiza o endereço onde vai ser a próxima alocação.

        //if ( Current->Footer < BL_HEAP_START){
        //    Current->Used = 0;                //Flag, 'sendo Usado' ou 'livre'.
        //    Current->Magic = 0;            //Magic number. Ver se não está corrompido.
        //	goto try_again;
        //}


        //
        // Obs: O limite da contagem de blocos foi checado acima.
        //

        //
        // Coloca o ponteiro na lista de blocos.
        //


        mmblockList[mmblockCount] = (unsigned long) Current;

        //
        // Salva o ponteiro do bloco usado como 'prévio'.
        // Obs: 'mm_prev_pointer' não é global, fica nesse arquivo.
        //

        mm_prev_pointer  = (unsigned long) g_heap_pointer; 

        //
        // *****************************************************
        //                **** SUPER IMPORTANTE ****
        // *****************************************************
        // Atualiza o ponteiro. 
        // Deve ser onde termina o último bloco configurado.
        // Isso significa que o próximo ponteiro onde começaremos 
        // a próxima estrutura fica exatamente onde começa o footer 
        // dessa estrutura.
        // Obs: O footer está aqui somente para isso. Para ajudar
        // a localizamarmos o início da próxima estrutura.

        g_heap_pointer = (unsigned long) Current->Footer;

        // Available heap:
        // Calcula o valor de heap disponível para as próximas alocações.
        // O heap disponível será o que tínhamos disponível menos o que 
        // gastamos agora.
        // O que gastamos agora foi o tamanho do header mais o tamanho da área
        // de cliente.

        g_available_heap = 
            (unsigned long) g_available_heap - (Current->Footer - Current->Header);

		// ## Finalmente ##
		// Retorna o ponteiro para o início da área alocada.
		// Essa área alocada chamado de user area.
		// Obs: Esse é o valor que será usado pela função malloc.
		//
		// Importante:
		// O que acontece se um aplicativo utilizar além da área alocada ??
		// O aplicativo invadirá a área do footer, onde está a estrutura do 
		// próximo bloco. Inutilizando as informações sobre aquele bloco.
		// Aviso: Cuidado com isso. @todo: Como corrigir.?? O que fazer??

        return (unsigned long) Current->userArea;
        //Nothing
    }else{
        // Se o ponteiro da estrutura de mmblock for inválido.
        printf("heapAllocateMemory fail: struct\n");
        goto fail;
    };

// @todo: 
// Checar novamente aqui o heap disponível. Se esgotou, tentar crescer.

    /*
     * *IMPORTANTE
     *
     * @todo:
     * Colocar o conteúdo da estrutura no lugar destinado para o header.
     * O header conterá informações sobre o heap.
     */


    /* errado #bugbug.
    Prev = (void*) mm_prev_pointer;

    if( (void*) Prev != NULL)
    {
        if( Prev->Used == 1 && 
            Prev->Magic == 1234 && 
            Prev->Free == 0)
        {
            Current->Prev = (void*) Prev;
            Prev->Next = (void*) Current;
        };
    };
    */

fail:
    refresh_screen ();
    //Se falhamos, retorna 0. Que equivalerá à NULL.
    return (unsigned long) 0;
}

/*
 * FreeHeap:
 *
 */
void FreeHeap(void *ptr)
{
    struct mmblock_d *Header;

// Parameter:
    if ((void *) ptr == NULL){
        return;
    }
    if ( ptr < (void *) BL_HEAP_START || ptr >= (void *) BL_HEAP_END )
    {
        return;
    }

// Header
// Encontrando o endereço do header.
// O ponteiro passado é o endereço da área de cliente.
    unsigned long UserAreaStart = (unsigned long) ptr;

    Header = (void *) ( UserAreaStart - MMBLOCK_HEADER_SIZE );
    if ((void *) Header == NULL){
        return;
    }else{

        if ( Header->Used != 1 || Header->Magic != 1234 )
        {
            return;
        }

		// Check
        if ( mmblockList[mmblockCount] == (unsigned long) Header && 
             Header->Id == mmblockCount )
        {
            mmblockList[mmblockCount] = 0;
            mmblockCount--;
        }

        // Isso invalida a estrutura, para evitar mal uso.
        Header->Used = 0;
        Header->Magic = 0;

        g_heap_pointer = (unsigned long) Header;
    };
}

/*
 * init_heap:
 *     Iniciar a gerência de Heap do bl. 
 *     @todo: Usar heapInit() ou heapHeap(). memoryInitializeHeapManager().
 * Essa rotina controi a mão o heap usado pelo bl.
 *     +Ela é chamada apenas uma vez.
 *     +Ela deve ser chamada entes de quelquer outra operação 
 * envolvendo o heap do processo bl.
 * @todo: Rotinas de automação da criação de heaps para processos.
 */
// Called by OS_Loader_Main in main.c.
int init_heap(void)
{
    //int i=0;

// Globals.
    bl_heap_start = (unsigned long) BL_HEAP_START;  
    bl_heap_end = (unsigned long)   BL_HEAP_END;  

// Heap Pointer, Available heap and Counter.
    g_heap_pointer = (unsigned long) bl_heap_start;  
    g_available_heap = (unsigned long) (bl_heap_end - bl_heap_start); 
    heapCount = 0;      

// #importante
// Último heap pointer válido. 
    last_valid = (unsigned long) g_heap_pointer;
    last_size = 0;

// Check Heap Pointer.
    if ( g_heap_pointer == 0 ){
        printf ("init_heap fail: Heap pointer\n");
        goto fail;
    }

// Check Heap Pointer overflow.
    if ( g_heap_pointer > bl_heap_end ){
        printf ("init_heap fail: Heap Pointer Overflow\n");
        goto fail;
    }

// Heap Start.
    if ( bl_heap_start == 0 ){
        printf ("init_heap fail: HeapStart={%x}\n", bl_heap_start );
        goto fail;
    }

// Heap End.
    if ( bl_heap_end == 0 ){
        printf ("init_heap fail: HeapEnd={%x}\n", bl_heap_end );
        goto fail;
    }

// Check available heap.
    if ( g_available_heap == 0 )
    {
       //@todo: Tentar crescer o heap.

        printf ("init_heap fail: Available heap\n");
        goto fail;
    }

// Heap list:
// Inicializa a lista de heaps.
// #bugbug suspensa a lista de heaps;
//só tem um. 

	/*
	while ( i < HEAP_COUNT_MAX )
	{
        heapList[i] = (unsigned long) 0;
		i++;
    };
	*/
 
//More?!

//done:
    return 0;

// Fail. 
// Falha ao iniciar o heap do bl.
fail:

    printf("init_heap: Fail\n");
    refresh_screen();

	/*
	// #debug
	printf("* Debug: %x %x  \n", bl_heap_start, bl_heap_end );
	refresh_screen();
    while(1){}
	*/

    return 1;
    //return -1;
}

