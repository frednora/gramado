/*
 * File: dispatch.h
 *     Dispatcher support.
 *     2015 - Created by Fred Nora.
 */

#ifndef __DISPATCH_H  
#define __DISPATCH_H    1

//
// Dispatcher types
//

// Used in disptcher();

#define DISPATCHER_UNDEFINED  0  // Type undefined.
#define DISPATCHER_CURRENT    1  // Dispatch the current thread.
#define DISPATCHER_IDLE       2  // Dispatch the current idle thread and select it as the current.
// ...



//Enumerador de criterio de sele��o de thread.
#define SELECT_IDLE_COUNT         1
#define SELECT_INITIALIZED_COUNT  2
#define SELECT_NEXT_COUNT         3 
#define SELECT_CURRENT_COUNT      4  
#define SELECT_ANY_COUNT          5
#define SELECT_IDEAL_COUNT        6
#define SELECT_DISPATCHER_COUNT   7


// Seleciona o tipo de dispatcher sendo usado no momento.
extern int dispatcherType;
extern int dispatcherQueueIndex;

// Contador de dispatch por categoria de dispatch.
// #todo: Cada processador pode ter uma dessa?
// #todo: Translate comments.
struct dispatch_count_d
{
    int used;
    int magic;
    int initialized;

// #todo: ??
// Selecionamos a melhor possivel, seguindo os crit�rios
// definidos por algum algor�timo de otimiza��o de escolha.
    unsigned long SelectIdealCount;
// Quando dispachamos a thread atual.
// Isso acontece o tempo todo. Pois selecionamos a current thread
// para rodar novamente at� esgotarem seus cr�ditos (quantum).
// >> Esse � o mais comum por enquanto.
    unsigned long SelectCurrentCount;
// Quando selecionamos uma thread no momento de sua inicializa��o.
    unsigned long SelectInitializedCount;
// Quando selecionamos a pr�xima indicada na estrutura da thead.
    unsigned long SelectNextCount;
// Quando selecionamos uma procurando por ela.
    unsigned long SelectAnyCount;
// ??
// Quando selecionamos a proxima na fila do dispatcher.
    unsigned long SelectDispatcherQueueCount;
    // ...
    //unsigned long SelectHighestPriorityCount;
    //unsigned long SelectLowestPriorityCount;

// Quando selecionamos a idle.
// Por fim. Quando n�o temos trabalho pra fazer.
    unsigned long SelectIdleCount;
};
// See: dispatch.c
extern struct dispatch_count_d  *DispatchCountBlock;

/*
 * dispatcherReadyList:
 *     SHORT-TERM SCHEDULER FOR THREADS
 *     Esta � a lista do dispatcher.
 *     Lista ponteiros para as heads de listas.
 *     as listas s�o para threads, uma lista para cada prioridade.
 *     IMPORTANTE:
 *     As threads aqui est�o no estado READY e
 *     ordenadas por prioridade.
 *     A �ltima thread da lista � a thread de maior prioridade.
 *     Cada elemento dessa lita � o olemento que est� na HEAD de uma lista
 *     Cada uma dessas lista � uma lista para uma prioridade espec�fica.
 *     Apenas a head de cada uma das listas � colocada aqui 
 * nessa lista do dispacher.
 *     OBS: Nesse momento essa lista do dispacher n�o est� sendo usada.  
 *          o kernel esta usando a lista de threads criadas. threadList[]
 *          n�o considerando a prioridade. ;)
 */ 
extern unsigned long dispatcherReadyList[PRIORITY_MAX +1];

//
// == prototypes ===============================
//

void IncrementDispatcherCount (int type);
void dispatcher (int type);

//
// $
// INITIALIZATION
//

int init_dispatch(void);

#endif    


//
// End
//

