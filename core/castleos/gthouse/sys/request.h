
// request.h
// #todo
// This is not the right place for this file.

#ifndef ____REQUEST_H 
#define ____REQUEST_H    1

// #importante:
// Devemos incluir aqui mais eventos importantes,
// principalmente os eventos referentes a espera bloqueante.
// Obs:
// + eventos de input (mouse, teclado) ...
// + eventos com threads, ( dormir , acordar, iniciar, fechar ... )
// ###importante: Esses eventos com threads permitirão muitas melhorias no estado 
// atual do sistema.

// #bugbug
// Lembrando que no momento em que o kernel request é atendido
// é o momento em que uma thread termina seu quantum,
// então não podemos ter referências a ela como thread atual.
 
//Enumerando. 
#define  KR_NULL        0 
#define  KR_NONE        KR_NULL  // Request sem motivo. 
#define  KR_TIME        1        // Contador de vezes que a tarefa rodou.
#define  KR_SLEEP       2        // Faz a tarefa atual dormir. 
#define  KR_WAKEUP      3        // Acorda a tarefa atual.
#define  KR_ZOMBIE      4        // Torna a tarefa atual zombie.
#define  KR_NEW         5        // *Inicia uma nova tarefa.
#define  KR_NEXT        6        // Reinicia a execução de uma tarefa.
#define  KR_TIMER_TICK  7        // O tick do timer.
#define  KR_TIMER_LIMIT 8        // Um limite de contagem.
#define  KR_CHECK_INITIALIZED 9  // Um limite de contagem.

// Chama o procedimento do sistema depois
// do procedimento de janela do sistema.
// sento assim a thread atual talvez seja outra.
// Então a thread atual no momento da chamada não será 
// a mesma da thread que chamou.
#define  KR_DEFERED_SYSTEMPROCEDURE 10

// ...

#define KERNEL_REQUEST_MAX 1024

unsigned long kernel_request;


/*
 * request_d:
 *     Estrutura para o request.
 *     Aqui ficarao os argumentos para atender o request.
 *     Serao os mesmos das mensagens, para inclusive interagir com elas.
 */
struct request_d
{

// Número do request.
    unsigned long kernel_request;

// 0 = No request.
// 1 = Driver request. (high priority)
// 2 = Server request. (medium priority)
// 3 = User request.   (low priority)
    int status;

// Temporizador.
// O request pode ser atendido imediatamente ou
// adiado até que esse temporizador seja zerado.
// 0 ~ ?
// #todo: Use 'unsigned long'.
    int timeout;

    int target_pid;
    int target_tid;

//
// Action support
//

// basic
    int msg;
    unsigned long long1;
    unsigned long long2;
// extra
    unsigned long long3;
    unsigned long long4;
    unsigned long long5;
    unsigned long long6;
};
struct request_d  REQUEST;

//
// Prototypes ===============
//

int 
createDeferredKernelRequest ( 
    unsigned long number, 
    int status, 
    int timeout,
    int target_pid,
    int target_tid,
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

void clearDeferredKernelRequest(void);

int processDeferredKernelRequest(void);

#endif    





















