// dispatch.c
// Created by Fred Nora.

#include <kernel.h>  


int dispatcherType=0;
int dispatcherQueueIndex=0;

// Contador de dispatch por categoria de dispatch.
// #todo: Cada processador pode ter uma dessa?
// See: dispatch.h
struct dispatch_count_d  *DispatchCountBlock;


unsigned long dispatcherReadyList[PRIORITY_MAX +1];

//
// =====================================
//

/*
 * IncrementDispatcherCount:
 *     Mensura os critérios de escolha.
 *     Contagem por critério de seleção.
 *     Faz uma contagem de quantas vezes o dispatcher 
 * fez uso de cada critério.
 * Obs: Esse modelo apresentou bons resultados por muito 
 * tempo. Vamos preserva-lo. 
 */
// #todo
// Create error messages.
// Isse é chamado várias vezes, é melhor não ter mensagem de debug.
// See: dispatch.h
void IncrementDispatcherCount(int type)
{

// Parameter:
// #todo: max limit.
    if (type < 0){
        debug_print("IncrementDispatcherCount: [FAIL] type\n");
        return;
    }

    if ( (void *) DispatchCountBlock == NULL ){
        panic ("IncrementDispatcherCount:\n");
    }
    
    if (DispatchCountBlock->magic != 1234){
        panic ("IncrementDispatcherCount: validation\n");
    }

    switch (type){
        case SELECT_IDLE_COUNT:
            DispatchCountBlock->SelectIdleCount++;
            break;
        case SELECT_INITIALIZED_COUNT:
            DispatchCountBlock->SelectInitializedCount++;
            break;
        case SELECT_NEXT_COUNT:
            DispatchCountBlock->SelectNextCount++;
            break;
        case SELECT_CURRENT_COUNT:
            DispatchCountBlock->SelectCurrentCount++;
            break;
       case SELECT_ANY_COUNT:
            DispatchCountBlock->SelectAnyCount++;
            break;
       case SELECT_IDEAL_COUNT:
            DispatchCountBlock->SelectIdealCount++;
            break;
       case SELECT_DISPATCHER_COUNT:
            DispatchCountBlock->SelectDispatcherQueueCount++;
            break;

       //...

       // Nothing.
       // Aqui poderia ter um contador de indefinições.
       default:
           debug_print("IncrementDispatcherCount: [FIXME] default\n");
           break;
    };

    // ??
}

/*
 * dispatcher:
 *     Despacha a thread atual que foi escolhida pelo scheduler. 
 *     Despacha, colocando ela no estado RUNNING e restaurando os valores 
 * dos registradores.
 *     Essa função é chamada no final da rotina task_switch, antes dela 
 * retornar.
 *     Obs: 
 *     Despacha a tarefa de acordo dom o tipo de dispatcher.
 *     Porém os tipos diferentes de dispacher ainda não estão habilitados, 
 * só um funciona.
 */
// #todo
// Change the return type and return with error
// if something goes wrong. So this way we can try another thing.
// int dispatcher ( int type ){

void dispatcher(int type)
{
    struct thread_d *TargetThread;

    //debug_print (" DISPATCHER: ");

//
// type
//

// See:
// dispatch.h

//#define DISPATCHER_UNDEFINED  0  // Type undefined.
//#define DISPATCHER_CURRENT    1  // Dispatch the current thread.
//#define DISPATCHER_IDLE       2  // Dispatch the current idle thread and select it as the current.
// ...

    //#hack
    if (type != DISPATCHER_CURRENT){
        type = DISPATCHER_CURRENT;
    }

dispatch_current:

// Target thread.

    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
        panic("dispatcher: current_thread\n");
    }
    TargetThread = (void *) threadList[current_thread];
    if ( (void *) TargetThread == NULL ){
        panic ("dispatcher: TargetThread\n");
    }
    if (TargetThread->magic != 1234){
        panic ("dispatcher: magic\n");
    }

// state
// #todo: dispatch idle?
    if (TargetThread->state != READY){
        panic ("dispatcher: state\n");
    }

// #importante
// >> MOVEMENT 4 (Ready --> Running).
// A thread passa para o estado RUNNING.
// Reinicia a contagem.
// Os outros elementos são reiniciados pelo caller

    if (TargetThread->state == READY)
    {
        // #check
        //if( TargetThread->readyCount > TargetThread->ready_limit)
        //{}

        TargetThread->state = RUNNING;  // :)

        TargetThread->runningCount = 0;
        TargetThread->runningCount_ms = 0;

        // Inicializa a contagem de ready, waiting e blocked.
        TargetThread->readyCount = 0;
        TargetThread->readyCount_ms = 0;
        TargetThread->waitingCount = 0;
        TargetThread->waitingCount_ms = 0;
        TargetThread->blockedCount = 0;
        TargetThread->blockedCount_ms = 0;
    }

// Priority
    if (TargetThread->base_priority < PRIORITY_MIN){
        TargetThread->base_priority = PRIORITY_MIN;
    }
    if (TargetThread->priority < PRIORITY_MIN){
        TargetThread->priority = PRIORITY_MIN;
    }

// Não faz sentido liberar uma thread com quantum '0'.
    if (TargetThread->quantum < QUANTUM_MIN){
        TargetThread->quantum = QUANTUM_MIN;
    }

    UPProcessorBlock.CurrentThread = 
        (struct thread_d *) TargetThread;

//
// ## RESTORE CONTEXT ##
//

// Flag sinalizando que o contexto não está mais salvo.
// Esse flag é acionada quando o contexto é salvo no início 
// da task switch.
// Chama a rotina que colocará as informações da estrutura de thread 
// nas variáveis usadas pelo assembly para configurar 
// os registradores antes do iretd.
// #todo
// Change the return type and return with error
// if something goes wrong. So this way we can try another thing.

    TargetThread->saved = FALSE;
    restore_current_context();  // update cr3.
    return;
    //return 0;
}


//
// $
// INITIALIZATION
//

int init_dispatch(void)
{
    register int i=0;

    dispatcherType = 0;
    dispatcherQueueIndex = 0;

// List
// Dispatcher ready list.
// #ps: Maybe it is not used.
    for ( i=0; i < PRIORITY_MAX; i++ ){
        dispatcherReadyList[i] = (unsigned long) 0;
    };


// Dispatch Count Block
// see: dispatch.c

    DispatchCountBlock = (void *) kmalloc( sizeof(struct dispatch_count_d) );
    if ((void *) DispatchCountBlock == NULL){
        printk ("init_dispatch: DispatchCountBlock\n");
        return FALSE;
    }

    DispatchCountBlock->SelectIdleCount = 0;
    DispatchCountBlock->SelectInitializedCount = 0;
    DispatchCountBlock->SelectNextCount = 0;
    DispatchCountBlock->SelectCurrentCount = 0;
    DispatchCountBlock->SelectAnyCount = 0;
    DispatchCountBlock->SelectIdealCount = 0;
    DispatchCountBlock->SelectDispatcherQueueCount = 0;
    // ...
    DispatchCountBlock->used=TRUE;
    DispatchCountBlock->magic=1234;
    DispatchCountBlock->initialized = TRUE;

    return 0;
}


