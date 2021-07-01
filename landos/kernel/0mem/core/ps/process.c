

#include <kernel.h>


int caller_process_id;
int processNewPID;


unsigned long __GetProcessStats ( int pid, int index ){

    struct process_d *p;


    if (pid<0){
        panic ("__GetProcessStats: pid \n");
    }

    // Process

    p = (void *) processList[pid];

    if ( (void *) p == NULL ){
        printf ("__GetProcessStats: struct \n");
        return 0; 

    } else {
        //checar validade.
		//...
    };


    switch (index){

        case 1:  return (unsigned long) p->pid;  break; 
        case 2:  return (unsigned long) p->ppid;  break; 
        case 3:  return (unsigned long) p->uid;  break; 
        case 4:  return (unsigned long) p->gid;  break; 
        case 5:  return (unsigned long) p->state;  break; 
        case 6:  return (unsigned long) p->plane;  break; 
        case 7:  return (unsigned long) p->input_type;  break; 
        case 8:  return (unsigned long) p->personality;  break; 
        case 9:  return (unsigned long) p->appMode;  break; 

        case 10:  
            return (unsigned long) p->private_memory_size;
            break;  

        case 11:
            return (unsigned long) p->shared_memory_size;
            break;          

        case 12:
            return (unsigned long) p->workingset_size;
            break;          

        case 13:
            return (unsigned long) p->workingset_peak_size;
            break;          

        case 14:
            return (unsigned long) p->pagefaultCount;
            break;          

        //case 15:  return (unsigned long) p->DirectoryPA;  break;
        //case 16:  return (unsigned long) p->DirectoryVA;  break;
        
        case 17:  return (unsigned long) p->Image;  break;
        case 18:  return (unsigned long) p->ImagePA;  break;
        case 19:  return (unsigned long) p->childImage;  break;
        case 20:  return (unsigned long) p->childImage_PA;  break;
        case 21:  return (unsigned long) p->Heap;  break;
        case 22:  return (unsigned long) p->HeapEnd;  break;
        case 23:  return (unsigned long) p->HeapSize;  break;
        case 24:  return (unsigned long) p->HeapPointer;  break;
        case 25:  return (unsigned long) p->HeapLastValid;  break;
        case 26:  return (unsigned long) p->HeapLastSize;  break;
        case 27:  return (unsigned long) p->Stack;  break;
        case 28:  return (unsigned long) p->StackEnd;  break;
        case 29:  return (unsigned long) p->StackSize;  break;
        case 30:  return (unsigned long) p->StackOffset;  break;
        case 31:  return (unsigned long) p->iopl;  break;
        case 32:  return (unsigned long) p->base_priority;  break;
        case 33:  return (unsigned long) p->priority;  break;
        case 34:  return (unsigned long) p->step;  break;
        case 35:  return (unsigned long) p->quantum;  break;
        case 36:  return (unsigned long) p->timeout;  break;
        case 37:  return (unsigned long) p->ticks_remaining;  break;
        
        case 38:  
            return (unsigned long) p->profiler_percentage_running;
            break;

        case 39:
            return (unsigned long) p->profiler_ticks_running;
            break;

        case 40:
            return (unsigned long) p->profiler_last_ticks;
            break;

        case 41:  return (unsigned long) p->thread_count;  break;
        case 42:  return (unsigned long) p->bound_type;  break;
        case 43:  return (unsigned long) p->preempted;  break;
        case 44:  return (unsigned long) p->saved;  break;
        case 45:  return (unsigned long) p->PreviousMode;  break;
        case 46:  return (unsigned long) p->wait4pid;  break;
        case 47:  return (unsigned long) p->exit_code;  break;
        case 48:  return (unsigned long) p->signal;  break;
        case 49:  return (unsigned long) p->umask;  break;
        case 50:  return (unsigned long) p->dialog_address; break;
        case 51:  return (unsigned long) p->ImageSize;  break;
           
        // #todo:
        // Precisamos da quantidade de p�ginas usadas.
    
        // ...
    };

    return 0;
}

// Systemcall 882.
// Pega o nome do processo.
int getprocessname ( int pid, char *buffer ){

    struct process_d *p;

    char *name_buffer = (char *) buffer;

    //#todo
    //checar validade dos argumentos.

    if (pid<0){
        debug_print ("getprocessname: [FAIL] pid\n");
        return -1;
    }

    //#todo
    //buffer validation
 
    p = (struct process_d *) processList[pid]; 

    if ( (void *) p == NULL ){
        debug_print ("getprocessname: [FAIL] p\n");
        return -1;
    }else{
        if ( p->used != TRUE || p->magic != 1234 ){
            debug_print ("getprocessname: [FAIL] VALIDATION\n");
            return -1;
        }
        
        // 64 bytes
        strcpy ( name_buffer, (const char *) p->__processname );  
        
        //#bugbug: Provavelmente isso ainda nem foi calculado.
        return (int) p->processName_len;
    };

    return -1;
}

/*
 * processObject:
 *     Cria uma estrutura do tipo processo, mas não inicializada.
 *     #todo: Criar a mesma rotina para threads e janelas.
 */

struct process_d *processObject (void){

    struct process_d *tmp;

    tmp = (void *) kmalloc ( sizeof(struct process_d) );

    if ( (void *) tmp == NULL ){
        panic ("ps-processObject: tmp");
    }

    return (struct process_d *) tmp;
}

/*
 * getNewPID:
 *     Pegar um slot vazio na lista de processos.
 *     +Isso pode ser usado para clonar um processo.
 */

// Começaremos a busca onde começa o range de IDs 
// de processos de usuário.
// Se encontramos um slot vazio, retornaremos o índice.
 
pid_t getNewPID (void){

    struct process_d *p;

    int i = USER_BASE_PID;

    while ( i < PROCESS_COUNT_MAX ){

        p = (struct process_d *) processList[i];

        if ( (void *) p == NULL ){ return (pid_t) i; }
        
        i++;
    };

    debug_print ("getNewPID: fail\n");

    return (pid_t) (-1);
}

/*
 * processTesting:
 *     Testando se o processo � v�lido. Se for v�lido retorna 1234.
 *     @todo: repensar os valores de retorno. 
 * system call (servi�o 88.)
 */

int processTesting (int pid){

    struct process_d *P;

    // Process.
    P = (void *) processList[pid];

    if ( (void *) P == NULL ){
        return 0;

    }else{
        if ( P->used == 1 && P->magic == 1234 ){ return (int) 1234; }
    };

    return 0;
}

/*
 * processSendSignal:
 *     Envia um sinal para um processo.
 *     Se o sinal e o processo forem v�lidos, um sinal � colocado
 * no PCB do processo.
 *     @todo: Rotinas envolvendo sinais devem ir para outro arquivo.
 */

int processSendSignal (struct process_d *p, unsigned long signal){
	
	//SIGNAL_COUNT_MAX
	
	//Limit
    //if(signal >= 32){
	//	return 1;
	//}
	
	if (signal == 0)
	{
		return 1;
	}
	
	//struct fail
	//if( (void*) p == NULL ){
	//	return 1;
	//}		
	
//ok:	
	//Ok
	if ( (void*) p != NULL )
	{	
		p->signal = (unsigned long) signal;
		return 0; //(int) signalSend(p,signal);
	}
	
	//...
	
//fail:
	
	return 1;
}

/*
 ***********************************************************
 * init_processes:
 *    Inicaliza o process manager.
 *   #todo: rever esse nome, pois na verdade estamos inicializando variaveis 
 * usadas no gerenciamento de processo.
 */

// Called by init_microkernel in mk.c

void init_processes (void){

    register int i=0;

    debug_print("init_processes:\n");


	//
	// Iniciando vari�veis globais.
	//

	kernel_request = 0;    // O que fazer com a tarefa atual.
	
	
	// ?? Contagem de tempo de execu��o da tarefa atual.
	//n�o precisa, isso � atualizado pelo request()
	//kernel_tick = 0;                                 

    kernel_switch = 0;     // ?? Ativa o kernel switch do scheduler.

    current_process = 0;


    // Clear process list.
    i=0;
    while (i < PROCESS_COUNT_MAX){
        processList[i] = (unsigned long) 0;
        i++;
    };

    // More ?
}

/*
 * CloseAllProcesses:
 *     Bloqueia todos os processos da lista de processos.
 *     Menos o processo '0'.
 *     processCloseAllProcesses();    
 */

void CloseAllProcesses (void)
{
    //loop
    int i=0;
    
    struct process_d *P;

	// #importante:
	// Menos o 0, pois � o kernel. 

    //Pega, bloqueia e tira da lista.
    for ( i=1; i < PROCESS_COUNT_MAX; i++ )
    {
        P = (void *) processList[i];
        P->state = PROCESS_BLOCKED;
        
        // Not kernel.
        if (i != 100){
            processList[i] = (unsigned long) 0;
        }
    };

    //Check process 0.
    P = (void *) processList[0];

    if ( (void *) P == NULL ){
        panic ("CloseAllProcesses: P\n");
    }

    // #bugbug
    // The kernel is process 100.

    P = (void *) processList[100];

    if ( (void *) P == NULL ){
        panic ("CloseAllProcesses: kernel\n");
    }
}

// usado pelo comando "current-process" no shell
void show_currentprocess_info (void){

    struct process_d *Current;


    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX )
    {
        //printf("show_process_information: current_process fail\n");
        return;
    }


	//Struct.
    Current = (void *) processList[current_process];

    if ( (void *) Current == NULL )
    {
        printf ("show_currentprocess_info: struct \n");
        return; 

    } else {

		//Index.
        printf ("PID={%d} PPID={%d} UID={%d} GID={%d} \n",
            Current->pid, Current->ppid, Current->uid, Current->gid );
		//Name
        //printf ("Name={%s} \n", Current->name_address );
        printf ("Name={%s} \n", Current->name );
        
		//Image Address.
        printf ("ImageAddress={%x} \n", Current->Image );

		//Directory Address. *IMPORTANTE.
        //printf (">>DirectoryPA={%x} \n", Current->DirectoryPA );
        //printf (">>DirectoryVA={%x} \n", Current->DirectoryVA );

		//Heap and stack.
        printf("Heap={%x}  HeapSize={%d KB}  \n", Current->Heap, 
            Current->HeapSize );

        printf("Stack={%x} StackSize={%d KB} \n", Current->Stack, 
            Current->StackSize );

		//...
    };

    refresh_screen();
}

/*
 * show_process_information:
 *     Mostra informa��es sobre os processos. 
 *     #todo: na verdade um aplicativo em user mode deve fazer esse trabalho
 * solicitando informa��es sobre cada processo atrav�s de chamadas.
 */

// Mostrar informa��es sobre os processos da lista.
// obs: as imagens s�o carregadas em endere�os virtuais diferentes
// e o endere�o mostrado � em rela��o ao diret�rio de p�ginas do kernel
// pois o kernel � que controla o posicionamento das imagens.

void show_process_information (void)
{
    // loop
    int i=0;
    struct process_d *p;

    printf ("show_process_information: \n");


    for ( i=0; i<PROCESS_COUNT_MAX; i++ )
    {

        p = (void *) processList[i];

        if ( (void *) p != NULL && 
                      p->used  == TRUE && 
                      p->magic == 1234 )
        { 

            //printf("\n");
            printf("\n=====================================\n");
            printf(">>[%s]\n", p->__processname);
            printf("PID=%d PPID=%d \n", p->pid,  p->ppid );
            
            printf("image-base =%x image-size =%d \n", 
                p->Image, p->ImageSize );
            printf("heap-base  =%x heap-size  =%d \n", 
                p->Heap,  p->HeapSize );
            printf("stack-base =%x stack-size =%d \n", 
                p->Stack, p->StackSize );

            //printf("dir-pa=%x dir-va=%x \n", 
            //    p->DirectoryPA, p->DirectoryVA );

            printf("iopl=%d prio=%d state=%d \n", 
                p->iopl, p->priority, p->state );

            printf("syscalls = { %d }\n", p->syscalls_counter );
        }
    // Nothing.
    };

    refresh_screen();
}

// Create process
struct process_d *create_process ( 
    struct room_d    *room,
    struct desktop_d *desktop,
    struct window_d  *window,
    unsigned long base_address, 
    unsigned long priority, 
    int ppid, 
    char *name, 
    unsigned long iopl,
    unsigned long pml4_va )
{

    struct process_d  *Process;
    pid_t PID = -1;

    // Para a entrada vazia no array de processos.
    struct process_d *EmptyEntry; 
    
    // loop
    register int i=0;

    // loop
    // indice usado na inicializaçao da lista de 
    // conexoes pendentes do processo servidor.
    register int sIndex=0;

    unsigned long BasePriority=0;
    unsigned long Priority=0;


    debug_print ("create_process: [FIXME] It's a work in progress!\n");


    //=================================
    // check parameters

    if( (void*) room == NULL ){
        debug_print ("create_process: [FIXME] room parameter is NULL\n");
    }
    
    if( (void*) desktop == NULL ){
        debug_print ("create_process: [FIXME] desktop parameter is NULL\n");
    }
    
    if( (void*) window == NULL ){
        debug_print ("create_process: [FIXME] window parameter is NULL\n");
    }

    // #todo
    // Maybe the virtual 0 is n option in the future. Maybe.
    if( base_address == 0 ){
        panic ("create_process: [ERROR] base_address\n");
    }

    if( ppid < 0 ){
        panic ("create_process: [ERROR] ppid\n");
    }
  
    if( (void*) name == NULL ){
        panic ("create_process: [ERROR] name\n");
    }
  
    if( *name == 0 ){
        panic ("create_process: [ERROR] *name\n");
    }

    if( pml4_va == 0 ){
        panic ("create_process: [ERROR] pml4_va\n");
    }

    // ...
    //=================================

	// @todo:
	// Melhorar esse esquema de numera��o e 
	// contagem de processos criados.
	// processNewPID � global ?

    if ( processNewPID < USER_BASE_PID || 
         processNewPID >= PROCESS_COUNT_MAX )
    {
        processNewPID = (int) USER_BASE_PID;
    }

    // Base priority.
    // Please, don't inherit base priority!

    BasePriority = (unsigned long) PRIORITY_NORMAL; 
    Priority     = (unsigned long) priority;

//
// Process
//
    Process = (void *) kmalloc ( sizeof(struct process_d) );

    // #todo: 
    // Aqui pode retornar NULL.
    if ( (void *) Process == NULL ){
        panic ("create_process: Process\n");
    }

//get_next:

	// Get empty.
	// Obt�m um �ndice para um slot vazio na lista de processos.
	// Se o slot estiver ocupado tentaremos o pr�ximo.
	// Na verdade podemos usar aquela fun��o que procura por um vazio. 

    while (1){

        PID = (int) getNewPID();

        if ( PID <= 0 || PID >= PROCESS_COUNT_MAX )
        {
            debug_print ("create_process: [FAIL] getNewPID \n");
            printf      ("create_process: [FAIL] getNewPID %d \n", PID);
            goto fail;
        }

        EmptyEntry = (void *) processList[PID];
 
        if ( (void *) EmptyEntry == NULL ){ break; }
    };
 
// ====================

    Process->objectType  = ObjectTypeProcess;
    Process->objectClass = ObjectClassKernelObjects;
    Process->used  = TRUE;
    Process->magic = PROCESS_MAGIC;

    // Undefined
    Process->position = 0;

    Process->iopl = iopl; 

    // Not a protected process!
    Process->_protected = 0;

    processNewPID = (int) PID;
        
    // Identificadores.
    // PID. PPID. UID. GID.
    Process->pid  = (int) PID; 
    Process->ppid = (int) ppid; 
    Process->uid  = (int) GetCurrentUserId(); 
    Process->gid  = (int) GetCurrentGroupId(); 
    // ...

    // sessão crítica.
    Process->_critical = 0;

    //State of process
    Process->state = INITIALIZED;  

    //@TODO: ISSO DEVERIA VIR POR ARGUMENTO
     Process->plane = FOREGROUND;

    //Error.
    //Process->error = 0;

    //Name.
    Process->name = (char *) name; //@todo: usar esse.
    //Process->cmd = NULL;  //nome curto que serve de comando.
    //Process->pathname = NULL;
 
    //#test
    //64 bytes m�x.
    strcpy ( Process->__processname, (const char *) name); 

    Process->processName_len = sizeof(Process->__processname);


    // Standard stream.
    // See: kstdio.c for the streams initialization.
    // #todo: We need a flag.
    
    if (kstdio_standard_streams_initialized != TRUE )
    {
        panic ("create_process: [ERROR] Standard stream is not initialized\n");
    }
    
    for ( i=0; i<32; ++i ){ Process->Objects[i] = 0; }

    if ( (void *) stdin == NULL ){
        panic ("create_process: [TEST] stdin");
    }

    if ( (void *) stdout == NULL ){
        panic ("create_process: [TEST] stdout");
    }

    if ( (void *) stderr == NULL ){
        panic ("create_process: [TEST] stderr");
    }

    Process->Objects[0] = (unsigned long) stdin;
    Process->Objects[1] = (unsigned long) stdout;
    Process->Objects[2] = (unsigned long) stderr;


    //Process->terminal =

//
// Banco de dados
//
		//bancos de dados e contas do processo.
		//Process->kdb =
		//Process->gdbListHead =
		//Process->ldbListHead =
		//Process->aspaceSharedListHead =
		//Process->aspacePersonalListHead =
		//Process->dspaceSharedListHead =
		//Process->dspacePersonalListHead =
		
		// Inicializando a lista de framepools do processo.
		// @todo: Todo processo deve ser criado com pelo menos um 
		// frame pool, o que � equivalente a 4MB. (uma parti��o)
		// Obs: Um framepool indica onde � a �rea de mem�ria fisica
		// que ser� usada para mapeamento das p�ginas usadas pelo processo.

    Process->framepoolListHead = NULL;

		//Thread inicial.
		//Process->thread =
		
		//Process->processImageMemory =
		//Process->processHeapMemory =
		//Process->processStackMemory =

        // ORDEM: 
        // O que segue � referenciado durante o processo de task switch.

		// Page Directory: 
		//     Alocar um endere�o f�sico para o diret�rio de p�ginas do 
		// processo a ser criado, depois chamar a fun��o que cria o diret�rio.
		//
		// @todo:
		// *IMPORTANTE: Por enquanto os processos s�o criadas usando o 
		// diret�rio de p�ginas do processo Kernel. Mas temos que criar 
		// um diret�rio novo pra cada processo criado.
		// O diret�rio de todos os processos de usu�rio ser�o iguais. 
		// Ter�o uma �rea de us�rio particular e uma �rea compartilhada 
		// em kernel mode.
		//
		//@todo: Alocar um endere�o f�sico antes, depois chamar a fun��o que 
		// cria o pagedirectory.
		//@todo: 
        //op��o: KERNEL_PAGEDIRECTORY; //@todo: Usar um pra cada processo.

		// #obs:
		// Vari�vel recebida via argumento.


//
// pml4_va
//

    if (pml4_va == 0)
    {
        debug_print("create_process: [FAIL] pml4_va\n");
        printf     ("create_process: [FAIL] pml4_va\n");
        goto fail;
        //return NULL;
    }

    Process->pml4_VA = (unsigned long) pml4_va;
    Process->pml4_PA = (unsigned long) virtual_to_physical ( 
                                               pml4_va, 
                                               gKernelPML4Address );

		// cancelados. 
		// Process->mmBlocks[32]
		// Process->mmblockList[32]

		// Process->processMemoryInfo

		// #todo: 
		// Precisa alocar espa�o na mem�ria f�sica.
		// Precisa criar page tables para essas areas de cada processo.
		// Os endere�os virtuais dessas areas dos processos s�o sempre os mesmos.
		// mas os endere�os f�sicos dessas areas variam de processo pra processo.

		// Imagem do processo.
		// ?? Provavelmente esse endere�o � virtual.
		// Queremos que esse endere�o seja padronizado e que todos 
		// os processos usem o mesmo endere�o.
		
		// #bugbug
		// Todos os processos de usu�rio come�am no mesmo endere�o virtual.
		// Por�m temos os processos em kernel mode e os processos do gramado core
		// que usam endere�os virtuais diferentes.
		// #todo: Rever isso.
		// #todo: estamos suspendendo essa informa��o.
		
		//
		// # IMPORTANTE 
		//
		
		// Base da imagem do processo.
		// Na verdade precisamos aceitar o endere�o passado via 
		// argumento, pois nem todos processos come�am no endere�o 
		// default.

//
// Image
//

    // Endere�o virtual e endere�o f�sico.
    Process->Image   = (unsigned long) base_address;  
    Process->ImagePA = (unsigned long) virtual_to_physical ( 
                                           Process->Image, 
                                           gKernelPML4Address ); 
                                               


//
// Child image
//

    // Endere�o virtual e endere�o f�sico de um processo filho.
    // Isso � usado durante a clonagem.
    Process->childImage = 0;
    Process->childImage_PA = 0;


    // #todo
    // Precisamos saber o tamanho da imagem do processo para
    // calcularmos quantas p�ginas ele vai usar.
    // Precisamos dividir a imagem em code, data, heap e stack
    // Pois a �rea de dados poder� sofrer swap.

    // Tamanho da imagem do processo.
    // Temos que chamar a fun��o que pega o tamanho de um arquivo,
    // #bugbug: Porem, no momento o kernel n�o consegue ler arquivos
    // que est�o em subdiret�rios corretamente e os programas est�o 
    // em subdiret�rios.
    // #obs: O tamanho tamb�m poderia ser passado por arguemento.
    // #ou um argumento com ponteiro pra estrutura de informa��o 
    // sobre uma imagem.

    Process->ImageSize = 0;

    // #todo: 
    // Estrutura com informa��es sobre a imagem do processo.
    
    Process->image_info = NULL;


//
// == Heap and Stack ===========
//


		// @todo: #BugBug 
		// O Heap e a Stack devem estar dentro da �rea de mem�ria do processo.
		// Uma pagetable do diret�rio � para o heap e outra para a stack.
        // Cada pagetable no diret�rio do processo � pra uma coisa.
        //
		// Obs: O endere�o virtual do heap e da stack dos processos ser�o 
		// os mesmos para todos os processos, assim como o endere�o virtual 
		// de carregamento da imagem.
		
		// Heap and Stack. 
		// #importante: (Endere�os virtuais).
		// Por isso pode ser o mesmo para todos os processos.
		
		
		// #### HEAP ####
		
		// directory va, index, region pa
		//CreatePageTable ( Process->DirectoryVA, 512, 0 );
		
		//Process->Heap = (unsigned long) 0x00400000; //funciona
		//Process->Heap = (unsigned long) 0xC0C00000; //funciona
		
		// g_heappool_va
		// endere�o virtual do pool de heaps.
		// os heaps nessa �rea ser�o dados para os processos.
		// base + (n*size)


    if ( g_heap_count < 0 || 
         g_heap_count >= g_heap_count_max )
    {
        panic ("create_process: [FAIL] g_heap_count limits\n");
    }

    // #atenção
    // Estamos usando o heappool pra pegarmos esses endereços.
    // me parece que isso é memória compartilhada em ring3
    // e que o malloc da libc está usando isso sem problemas.

    // #todo: 
    // #test: A stack de um process recem criado
    // poderia ficar no fim de seu heap ???

    if (g_heappool_va == 0){
        panic ("clone_and_execute_process: g_heappool_va");
    }

    // Ignoraremos esse pois vai falhar na criacao do primeiro heap.
    //if (g_heap_count == 0)
        //panic("clone_and_execute_process: g_heap_count");

    if (g_heap_size == 0){
        panic ("clone_and_execute_process: g_heap_size");
    }

    // #bugbug
    // There is a limit here. End we will have a huge problem 
    // when reach it.

    Process->Heap     = (unsigned long) g_heappool_va + (g_heap_count * g_heap_size);
    Process->HeapSize = (unsigned long) g_heap_size;
    Process->HeapEnd  = (unsigned long) (Process->Heap + Process->HeapSize); 
    g_heap_count++;


    // Endere�o do in�cio da Stack do processo.
    // Endere�o do fim da stack do processo.
    // Tamanho da pilha, dada em KB.
    // #importante: 
    // Deslocamento do endere�o do in�cio da pilha em rela��o 
    // ao in�cio do processo. 

    // #bugbug
    // Isso indica que a stack será no endereço virtual tradicional,
    // porém qual é o endereço físico da stack do processo criado
    // com essa rotina.
    // #bugbug: Com esse erro todos os processo criados
    // estão usando a mesma stack, pois todas apontam para o mesmo
    // endereço físico.


//
// #bugbug #bugbug #bugbug #bugbug
//

    // Wrong !!!!!!!!!!!!!!!!!!!!

    Process->Stack       = (unsigned long) UPROCESS_DEFAULT_STACK_BASE; 
    Process->StackSize   = (unsigned long) UPROCESS_DEFAULT_STACK_SIZE; //?? usamos isso na hora de criar a stack?? 
    Process->StackEnd    = (unsigned long) (Process->Stack - Process->StackSize);  
    Process->StackOffset = (unsigned long) UPROCESS_DEFAULT_STACK_OFFSET;  //??


//
// PPL - (Process Permition Level).(gdef.h)
//

    // Determina as camadas de software que um processo ter� acesso irrestrito.
    // Process->ppl = pplK0;

    //Process->callerq          //head of list of procs wishing to send.
    //Process->sendlink;        //link to next proc wishing to send.
    //Process->message_bufffer  //pointer to message buffer.
    //Process->getfrom_pid      //from whom does process want to receive.
    //Process->sendto_pid       //pra quem.

    //Signal
    //Process->signal = 0;
    //Process->signalMask = 0;

    //cancelada.
    //Process->process_message_queue[8]

//
// Priority
//
    Process->base_priority = (unsigned long) BasePriority;
    Process->priority      = (unsigned long) Priority;

    //Que tipo de scheduler o processo utiliza. (rr, realtime ...).
    //Process->scheduler_type = ; 
    
    // Syscalls counter.
    Process->syscalls_counter = 0;

    // #todo
    // Counters

    //Process->step
    //Process->quantum
    //Process->timeout
    //Process->ticks_remaining

    //As threads do processo iniciam com esse quantum.
    //Process->ThreadQuantum   

//
// == Thread =====================
//

    //Process->threadCount = 0;    //N�mero de threads do processo.
    //Process->tList[32] 

    Process->threadListHead = NULL;
    Process->control = NULL;

    //Process->event

    // #importante
    // user session, room and desktop.

    // #bugbug: 
    // N�o temos informa��o sobre a user session, 
    // devemos pegar a estrutura de current user session. 
    // Para isso ela deve ser configurada na inicializa��o do gws,
    // antes da cria��o dos processo.

//
// Security
//

    Process->usession = CurrentUserSession;  // Current.
    Process->room     = room;                // Passado via argumento.
    Process->desktop  = desktop;             // Passado via argumento.


    // absolute pathname and relative pathname. 

    Process->file_root = (file *) 0;
    Process->file_cwd  = (file *) 0;
    Process->inode_root = (struct inode_d *) 0;
    Process->inode_cwd  = (struct inode_d *) 0;

    // wait4pid: 
    // O processo esta esperando um processo filho fechar.
    // Esse � o PID do processo que ele est� esperando fechar.

    Process->wait4pid = (pid_t) 0;
        
    // Número de processos filhos.
    Process->nchildren = 0;

    Process->zombieChildListHead = NULL;
    Process->exit_code = 0;

    // ?? 
    // Procedimento eem ring 0 por enquanto.
    //Process->dialog_address = (unsigned long) &system_procedure;

    // Signal
    Process->signal = 0;
    Process->umask = 0;

//
// Msg
//

    //#bugbug
    //deleta isso.
		//Msg support.
		//Argumentos do procedimento de janela.
		//@todo: Isso pode ser um ponteiro de estrutura,
		//a fila de mensgens pode ser uma fila de ponteiros.
        //Process->window = NULL;    //arg1. 
        //Process->msg = 0;          //arg2.
        //Process->long1 = 0;        //arg3.
        //Process->long2 = 0;        //arg4.

//
// == Socket ===================================
//

    // loop
    // pending connections;
    
    for (sIndex=0; sIndex<SOCKET_MAX_PENDING_CONNECTIONS; ++sIndex)
    {
        Process->socket_pending_list[sIndex] = 0; 
    };

    Process->socket_pending_list_head = 0;
    Process->socket_pending_list_tail = 0;
    Process->socket_pending_list_max  = 0;  // atualizado pelo listen();

//
// tty support
//

    printf ("create_process: calling tty_create[DEBUG]\n");

    Process->tty = ( struct tty_d *) tty_create(); 

    if ( (void *) Process->tty == NULL ){
        panic ("create_process: Couldn't create tty\n");
    }
    tty_start(Process->tty);

//
// Navigation
//

    Process->prev = NULL; 
    Process->next = NULL; 

    // Register
    // List
    // Coloca o processo criado na lista de processos.

    processList[PID] = (unsigned long) Process;

    // #todo
    // last_created = PID;
    
    // ok
    return (void *) Process;

// Fail

fail:
    //Process = NULL;
    refresh_screen();
    return NULL;
}















































