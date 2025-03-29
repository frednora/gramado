// ts.c
// Task switching.
// Actually it's thread scwitching.
// Created by Fred Nora.

#include <kernel.h>

//Status do mecanismo de task switch. 
unsigned long task_switch_status=0; // locked

//
// =============================================
//

static void __tsOnFinishedExecuting(struct thread_d *t);
static void __tsCry(unsigned long flags);
// Task switching implementation.
static void __task_switch (void);

//
// =============================================
//

/*
 * get_taskswitch_status:
 *     Obtem o status do mecanismo de taskswitch.
 * @todo: Mudar o nome dessa função para taskswitchGetStatus();.
 */
//#bugbug: Mudar para int.
unsigned long get_taskswitch_status(void)
{
    return (unsigned long) task_switch_status;
}

/*
 * set_taskswitch_status:
 *    Configura o status do mecanismo de task switch.
 *    Se o mecanismo de taskswitch estiver desligado 
 * não ocorrerá a mudança.
 * @todo: Mudar o nome dessa função para taskswitchSetStatus(.);
 */ 
// #bugbug: Mudar para int.
void set_taskswitch_status( unsigned long status )
{
    task_switch_status = (unsigned long) status;
}

void taskswitch_lock (void){
    task_switch_status = (unsigned long) LOCKED;
}

void taskswitch_unlock (void)
{
    task_switch_status = (unsigned long) UNLOCKED;
}

// Call extra routines scheduled to this moment.
// called by task_switch.
// #importante:
// Checaremos por atividades extras que foram agendadas pelo 
// mecanismo de request. Isso depois do contexto ter sido 
// salvo e antes de selecionarmos a próxima thread.
void tsCallExtraRoutines(void)
{
    debug_print ("tsCallExtraRoutines: [FIXME] \n");

    // Kernel requests.
    //processDeferredKernelRequest();

    // Unix signals.
    //xxSignal();

    // ...

    // #todo: 
    // Talvez possamos incluir mais atividades extras.
}

// :(
static void __tsCry(unsigned long flags)
{
    if (flags & 0x8000)
        cali_shutdown(0);
}

//
// $
// ON FINISHED EXECUTING
//

// ## PREEMPT ##
// Preempt
// >> MOVEMENT 3 (Running --> Ready).
// sofrendo preempção por tempo.
// Fim do quantum.
// Nesse momento a thread [esgotou] seu quantum, 
// então sofrerá preempção e outra thread será colocada 
// para rodar de acordo com a ordem estabelecida pelo escalonador.
// #todo: Mas isso só poderia acontecer se a flag
// ->preempted permitisse. 
// talvez o certo seja ->preenptable.

static void __tsOnFinishedExecuting(struct thread_d *t)
{
    if ((void*) t == NULL)
        panic("__tsOnFinishedExecuting: t\n");
    if (t->magic != 1234)
        panic("__tsOnFinishedExecuting: t magic\n");

// #bugbug
//  Isso está acontecendo.

    //if ( CurrentThread->state != RUNNING )
    //    panic("task_switch: CurrentThread->state != RUNNING");

    //if ( CurrentThread->state != RUNNING )
    //     goto ZeroGravity;

//
// Preempt
//
 
    if (t->state == RUNNING)
    {
        t->state = READY;
        t->readyCount = 0;
    }

// Spawn thread 
// Check for a thread in standby.
// In this case, this routine will not return.
// See: schedi.c
    schedi_check_for_standby();

// ---------------------------------------------------------

//
// Signals
//

// Peding signals for this thread.
// #todo: Put signals this way = t->signal |= 1<<(signal-1);

    // Se existe algum sinal para essa thread
    if (t->signal != 0)
    {
        //#test
        //if ( t->signal & (1<<(SIGALRM-1)) )
        //    printk("SIGALRM\n");
        //if ( t->signal & (1<<(SIGKILL-1)) )
        //    printk("SIGKILL\n");

        //refresh_screen();    
        //panic("__tsOnFinishedExecuting: t->signal\n");
    }

// ---------------------------------------------------------

//
// Services:
// 

// Nesse momento uma thread esgotou seu quantum,
// podemos checar se tem ela alguma mensagem para o kernel,
// responter a mensagem realizando uma chamada à algum
// serviço em ring 3 ou chamando alguma rotina interna. 

// #todo
// Podemos atualizar contadores, considerando
// essa condição de termos encerrado nossos creditos.

// Essa pode ser uma boa hora pra checar o working set de uma thread.
// Quais foram as páginas mais usadas?
// Quantas páginas?

// Esse pode ser um bom momento para checar estatísticas dessa thread.
// E avaliarmos o mal-comportamento dela. Como uso de systemcalls,
// recursos, pagefaults, etc...

// Esse pode ser um bom momento para enfileirarmos essa thread
// caso o tipo de scheduler nos permita. Se for round robin,
// não precisa, mas se for outra política, então podemos
// colocar ele na fila de prontos em tail, 
// pois serão retirados em head.

    // ready_q[tail] = (unsigned long) t;

//
// == EXTRA ==========
//

// Call extra routines scheduled to this moment.
// #hackhack
// Vamos validar isso, pois isso é trabalho de uma rotina
// do timer qua ainda não esta pronta.
        
    //extra = TRUE;
    extra = FALSE;

    if (extra == TRUE)
    {
        //#debug
        //debug_print (" X ");
        tsCallExtraRoutines();
        //processDeferredKernelRequest();
        extra = FALSE;
    }

// Dead thread collector
// Avalia se é necessário acordar a thread do dead thread collector.
// É uma thread em ring 0.
// Só chamamos se ele ja estiver inicializado e rodando.
// #bugbug
// precismos rever essa questão pois isso pode estar
// fazendo a idle thread dormir. Isso pode prejudicar
// a contagem.
// See: .c
// #bugbug
// #todo: This is a work in progress!

    if (dead_thread_collector_status == TRUE){
        check_for_dead_thread_collector();
    }
    
    if(t->tid == INIT_TID)
    {
        if (t->_its_my_party_and_ill_cry_if_i_want_to == TRUE)
            __tsCry(0);
    }
}


//
// $
// TASK SWITCH
//

/*
 * __task_switch:
 * + Switch the thread.
 * + Save and restore context.
 * + Preempt thread if the quantum is over.
 *   In this case we will spawn a new thread if there is one
 *   in standby state.
 * + Select the next thread and dispatch.
 * + Return to _irq0.
 */
static void __task_switch(void)
{
// Called by psTaskSwitch().

// Current
    struct process_d *CurrentProcess;
    struct thread_d  *CurrentThread;
// Target
    struct process_d *TargetProcess;
    struct thread_d  *TargetThread;
// The owner of the current thread.
    pid_t owner_pid = (pid_t) (-1);  //fail
// tmp tid
    //tid_t tmp_tid = -1;
// =======================================================

//
// Current thread
//

    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
        panic ("ts: current_thread\n");
    }
// structure
    CurrentThread = (void *) threadList[current_thread]; 
    if ((void *) CurrentThread == NULL){
        panic ("ts: CurrentThread\n");
    }
    if ( CurrentThread->used != TRUE || CurrentThread->magic != 1234 ){
        panic ("ts: CurrentThread validation\n");
    }
// =======================================================

// Current process
// The owner of the current thread.

// pid
    owner_pid = (pid_t) CurrentThread->owner_pid;
    if ( owner_pid < 0 || 
         owner_pid >= PROCESS_COUNT_MAX )
    {
        panic ("ts: owner_pid\n");
    }
// structure
    CurrentProcess = (void *) processList[owner_pid];
    if ((void *) CurrentProcess == NULL){
        panic ("ts: CurrentProcess\n");
    }
    if ( CurrentProcess->used != TRUE || 
         CurrentProcess->magic != 1234 )
    {
        panic ("ts: CurrentProcess validation\n");
    }
// check pid
    if (CurrentProcess->pid != owner_pid){
        panic("ts: CurrentProcess->pid != owner_pid\n");
    }
// Set the new current process.
    set_current_process(owner_pid);

//
// == Conting =================================
//

// Increment total ticks.
    CurrentThread->step++;
// Total time in ms.
    CurrentThread->total_time_ms = 
        (unsigned long) CurrentThread->total_time_ms + (DEFAULT_PIT_FREQ/sys_time_hz);

// Increment tick counter for the current quantum.
    CurrentThread->runningCount++;
// How many ticks in this quantum given in ms.
    CurrentThread->runningCount_ms = 
        (unsigned long) CurrentThread->runningCount_ms + (DEFAULT_PIT_FREQ/sys_time_hz);

//
// -----------------------------------------
//

// The ts needs to be in an UNLOCKED state.

// Locked?
// Taskswitch locked? 
// Return without saving.
    if (task_switch_status == LOCKED){
        IncrementDispatcherCount (SELECT_CURRENT_COUNT);
        debug_print ("ts: Locked\n");
        return; 
    }

// Not Unlocked?
// Nesse momento a thread atual sofre preempção por tempo
// Em seguida tentamos selecionar outra thread.
// Save the context.
// Not unlocked?
    if (task_switch_status != UNLOCKED){
        panic ("ts: task_switch_status != UNLOCKED\n");
    }

//
// Save context
//

// #todo:
// Put the tid as an argument.

    save_current_context();
    CurrentThread->saved = TRUE;

// #test
// signal? timer ?
// O contexto da thread está salvo.
// Podemos checar se ha um timer configurado 
// para esse dado tick e saltarmos para o handler
// de single shot configurado para esse timer.

    //if( (jiffies % 16) == 0 ){
        //spawn_test_signal();
    //}
//=======================================================

//
// == Checar se esgotou o tempo de processamento ==
//

// #obs:
// Ja salvamos os contexto.
// Se a thread ainda não esgotou seu quantum, 
// então ela continua usando o processador.
// A preempção acontecerá por dois possíveis motivos.
// + Se o timeslice acabar.
// + Se a flag de yield foi acionada.

// Ainda não esgotou o timeslice.
// Yield support.
// preempção por pedido de yield.
// Atendendo o pedido para tirar a thread do estado de rodando
// e colocar ela no estado de pronta.
// Coloca no estado de pronto e limpa a flag.
// Em seguida vamos procurar outra.
// A mesma thread vai rodar novamente,
// Não precisa restaurar o contexto.
// O seu contexto está salvo, mas o handler em assembly
// vai usar o contexto que ele já possui.

// :: ----
// Still in quantum:
// The thread still have some processing time in its quantum value.
// Let's return and allow the thread to run for a while.

    if ( CurrentThread->runningCount < CurrentThread->quantum ){

        // Yield in progress. 
        // Esgota o quantum e ela saírá naturalmente
        // no próximo tick.
        // Revertemos a flag acionada em schedi.c.

        // :: yield
        if ( CurrentThread->state == RUNNING && 
             CurrentThread->yield_in_progress == TRUE )
        {
            CurrentThread->runningCount = CurrentThread->quantum;  // Esgoto
            CurrentThread->yield_in_progress = FALSE;
        }

        // :: sleep
        if ( CurrentThread->state == RUNNING && 
             CurrentThread->sleep_in_progress == TRUE )
        {
            printk ("ts: Do sleep until\n");
            CurrentThread->runningCount = CurrentThread->quantum;  // Esgoto
            // set waiting
            sleep_until(
                CurrentThread->tid, 
                CurrentThread->desired_sleep_ms );
            CurrentThread->sleep_in_progress = FALSE;
            printk ("ts: Status=%d\n",CurrentThread->state);
        }

        IncrementDispatcherCount (SELECT_CURRENT_COUNT);
        //debug_print (" The same again $\n");
        //debug_print ("s");  // the same again
        return; 

// :: ----
// End of quantum:
// The quantum is over, let's preempt the thread,
// check for a new thread in standby, check for signals, etc ...
// Preempt: >> MOVEMENT 3 (Running --> Ready).

    } else if ( CurrentThread->runningCount >= CurrentThread->quantum ){
        __tsOnFinishedExecuting(CurrentThread);
        // Jumping to this label for the first time.
        goto ZeroGravity;

// :: ----
// Undefined quantum state.
// We're lost with the processing time for this thread.
// #todo:
// Maybe we can reset the quantum and balance the processing time 
// for this thread.

    }else{
        panic ("ts: CurrentThread->runningCount\n");
    };

//CrazyFail:
// Crazy Fail!
// #bugbug
// Não deveríamos estar aqui.
// Podemos abortar ou selecionar a próxima provisóriamente.
    //#debug
    //panic ("ts.c: crazy fail");
    goto dispatch_current; 

// =====================================================================

//
// == TRY NEXT THREAD =====
//

// Zero gravity!
// At this point we don't have a thread to run and
// the old thread already have its context saved.

// Try next thread.
ZeroGravity:
// try_next: 

// Two situations:
// We have '0' threads or only '1' thread.
// We are in a Uni-Processor mode.
// No threads
// #todo: Can we reintialize the kernel?
// See: up.h and cpu.h
// Only '1' thread.
// Is that thread the idle thread?
// Can we use the mwait instruction ?
// See: up.h and cpu.h
// tid0_thread
// This is a ring0 thread.
// See: x86_64/x64init.c
// If we will run only the idle thread, 
// so we can use the mwait instruction. 
// asm ("mwait"); 
// Only 1 thread.
// The Idle thread is gonna be the scheduler condutor.

// No threads.
// The counter is telling us that there is no thread in 
// this system.
    if (UPProcessorBlock.threads_counter == 0){
        panic("ts: No threads\n");
    }

// Only one thread.
// The counter is telling us that there is only on thread in the system.
// It needs to be the init thread.
// Let's put this thread in the first place of the current queue.
    if (UPProcessorBlock.threads_counter == 1)
    {
        if ((void *)InitThread != UPProcessorBlock.IdleThread){
            panic("ts: Invalid single thread\n");
        }

        currentq = (void *) UPProcessorBlock.IdleThread;
        goto go_ahead;
    }

// Pick a thread and break the round?
// We can do this if a thread was selected as 'timeout_thread'.
// It means that the thread need to run now, in the next tick.

    if ((void*) timeout_thread != NULL)
    {
        if (timeout_thread->magic == 1234)
        {
            if (timeout_thread->waiting_for_timeout == TRUE)
            {
                timeout_thread->waiting_for_timeout = FALSE;
                currentq = (void *) timeout_thread;
                currentq->next = NULL;
                goto go_ahead;
            }
        }
    }

//
// End of round ?
//

// ---------------------------------
// Reescalonar se chegamos ao fim do round.
// #bugbug
// Ao fim do round estamos tendo problemas ao reescalonar 
// Podemos tentar repedir o round só para teste...
// depois melhoramos o reescalonamento.
// #importante:
// #todo: #test: 
// De tempos em tempos uma interrupção pode chamar o escalonador,
// ao invés de chamarmos o escalonador ao fim de todo round.
// #critério:
// Se alcançamos o fim da lista encadeada cujo ponteiro é 'Conductor'.
// Então chamamos o scheduler para reescalonar as threads.
// Essa rotina reescalona e entrega um novo current_thread e
// Conductor.
// #?
// Estamos reconstruindo o round muitas vezes por segundo.
// Isso é ruin quando tem poucas threads, mas não faz diferença
// se o round for composto por muitas threads.

// The queue is over.
// End of round. Rebuild the round.
    if ((void *) currentq->next == NULL){
        current_thread = (tid_t) psScheduler();
        goto go_ahead;
    }

// The queue is not over,
// let's get the next thread in the linked list.
    if ((void *) currentq->next != NULL){
        currentq = (void *) currentq->next;
        goto go_ahead;
    }

// Not reached.
// This is the end of our options when trying to select a thread.
    panic("ts: Unexpected error\n");

// ---------------------------------------------------
// Go ahead
// #importante:
// Nesse momento já temos uma thread selecionada,
// vamos checar a validade e executar ela.
// #importante:
// Caso a thread selecionada não seja válida, temos duas opções,
// ou chamamos o escalonador, ou saltamos para o início dessa rotina
// para tentarmos outros critérios.

go_ahead:

// :)
//#########################################//
//  # We have a new selected thread now #  //
//#########################################//

// TARGET:
// Esse foi o ponteiro configurado pelo scheduler
// ou quando pegamos a próxima na lista.

    TargetThread = (void *) currentq;
    if ((void *) TargetThread == NULL){
        debug_print ("ts: Struct ");
        current_thread = (tid_t) psScheduler();
        goto ZeroGravity;
    }
    if ( TargetThread->used != TRUE || TargetThread->magic != 1234 ){
        debug_print ("ts: val ");
        current_thread = (tid_t) psScheduler();
        goto ZeroGravity;
    }
// Not ready?
    if (TargetThread->state != READY){
        debug_print ("ts: state ");
        current_thread = (tid_t) psScheduler();
        goto ZeroGravity;
    }

//
// == Dispatcher ====
//
    
// Current selected.
    current_thread = (int) TargetThread->tid;
    goto dispatch_current;

// #debug
// Not reached
    //panic("ts: [FAIL] dispatching target\n");

// =================
// Dispatch current 
// =================

// Validation
// Check current thread limits.
// The target thread will be the current.

dispatch_current:

// tid
    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
        panic ("ts-dispatch_current: current_thread\n");
    }

// structure
    TargetThread = (void *) threadList[current_thread];
    if ((void *) TargetThread == NULL){
        panic ("ts-dispatch_current: TargetThread\n");
    }
    if ( TargetThread->used != TRUE || TargetThread->magic != 1234 ){
        panic ("ts-dispatch_current: validation\n");
    }

// Not ready?
    if (TargetThread->state != READY){
        panic ("ts-dispatch_current: Not ready\n");
    }

    // #todo
    //UPProcessorBlock.CurrentThread = 
    //    (struct thread_d *) TargetThread;
    //UPProcessorBlock.NextThread = 
    //    (struct thread_d *) TargetThread->next;

// Counters
// Clean
// The spawn routine will do something more.

    TargetThread->standbyCount = 0;
    TargetThread->standbyCount_ms = 0;
    TargetThread->runningCount = 0;
    TargetThread->runningCount_ms = 0;
    TargetThread->readyCount = 0;
    TargetThread->readyCount_ms = 0;
    TargetThread->waitingCount = 0;
    TargetThread->waitingCount_ms = 0;
    TargetThread->blockedCount = 0;
    TargetThread->blockedCount_ms = 0;

// Base Priority
    if (TargetThread->base_priority > PRIORITY_MAX){
        TargetThread->base_priority = PRIORITY_MAX;
    }
// Priority
    if (TargetThread->priority > PRIORITY_MAX){
        TargetThread->priority = PRIORITY_MAX;
    }
// Credits limit
    if (TargetThread->quantum > QUANTUM_MAX){
        TargetThread->quantum = QUANTUM_MAX;
    }

// Call dispatcher.
// #bugbug
// Talvez aqui devemos indicar que a current foi selecionada. 
    IncrementDispatcherCount(SELECT_DISPATCHER_COUNT);

// MOVEMENT 4 (Ready --> Running).
// It also restores the context.
    dispatcher(DISPATCHER_CURRENT); 

//done:
// We will return in the end of this function.

// Owner validation.
// Owner PID.

    pid_t targetthread_OwnerPID = (pid_t) TargetThread->owner_pid;
    if ( targetthread_OwnerPID < 0 || 
         targetthread_OwnerPID >= THREAD_COUNT_MAX )
    {
       printk ("ts: targetthread_OwnerPID ERROR\n", targetthread_OwnerPID );
       die();
    }

// Target process 
    TargetProcess = (void *) processList[targetthread_OwnerPID];
    if ((void *) TargetProcess == NULL){
        printk ("ts: TargetProcess %s struct fail\n", TargetProcess->name );
        die();
    }
    if ( TargetProcess->used != TRUE || TargetProcess->magic != 1234 ){
        printk ("ts: TargetProcess %s validation\n", 
            TargetProcess->name );
        die();
    }

// pid
    if (TargetProcess->pid != targetthread_OwnerPID){
        panic("ts: TargetProcess->pid != targetthread_OwnerPID\n");
    }

// Set current process.
// Update global variable.

    //current_process = (pid_t) TargetProcess->pid;
    set_current_process(TargetProcess->pid);

// check pml4_PA
    if ( (unsigned long) TargetProcess->pml4_PA == 0 ){
        printk ("ts: Process %s pml4 fail\n", TargetProcess->name );
        die();
    }

// #bugug
// #todo
    // current_process_pagedirectory_address = (unsigned long) P->DirectoryPA;
    // ?? = (unsigned long) P->pml4_PA;

    return;
fail:
    panic ("ts: Unspected error\n");
}

/*
 * tsTaskSwitch:
 *     Interface para chamar a rotina de Task Switch.
 *     gerencia a rotina de 
 * troca de thread, realizando operações de salvamento e 
 * restauração de contexto utilizado variáveis globais e 
 * extrutura de dados, seleciona a próxima thread através 
 * do scheduler, despacha a thread selecionada através do 
 * dispatcher e retorna para a função _irq0 em hw.inc, 
 * que configurará os registradores e executará a 
 * thread através do método iret.
 * #importante:
 * Na verdade, é uma interface pra uma rotina que faz tudo isso.
 */
/*
// @todo: Fazer alguma rotina antes aqui ?!
// Obs: A qui poderemos criar rotinas que não lidem com a troca de 
// threads propriamente, mas com atualizações de variáveis e gerenciamento 
// de contagem.
// >> Na entrada da rotina podemos atualizar a contagem da tarefa que acabou de rodar.
// >> A rotina task_switch fica responsável apenas troca de contexto, não fazendo 
// atualização de variáveis de contagem.
// >> ?? Na saída ??
// ?? quem atualizou as variáveis de critério de escolha ??? o dispacher ??
*/
// Called by:
// Called by irq0_TIMER() int pit.c.
// See also: hw.asm

void tsTaskSwitch(void)
{
    pid_t current_process_pid = -1;
    pid_t ws_pid = -1;

// Filters.

// #::
// A interrupçao de timer pode acontecer 
// enquanto um processo em ring3 ou ring0 esta rodando.
// Durante essa rotina de handler de timer
// pode ser que chamaremos um callout dentro do
// window server, mas esse callout não sera interrompido
// por outra interrupçao de timer, pois ainda não chamamos EOI.

    current_process_pid = (pid_t) get_current_process();
    if ( current_process_pid < 0 || 
         current_process_pid >= PROCESS_COUNT_MAX )
    {
        printk ("psTaskSwitch: current_process_pid %d", 
            current_process_pid);
        die();
    }

// Current thread.
// First check!
// This variable was set at the last release or the last spawn.
// Global variable.

    if ( current_thread < 0 || 
         current_thread >= THREAD_COUNT_MAX )
    {
        printk ("psTaskSwitch: current_thread %d", current_thread); 
        die();
    }

//
// Callback
//

// Permitindo que o assembly chame o callback.
// Somente quando o processo interrompido for o init.
    ws_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_WS);
// Se estamos na thread do window server.
    if (current_process_pid == ws_pid)
    {
        // Se o callback ja foi inicializado 
        // por uma chamada do window server.
        // see: callback.c
        // Return. No taskswitching
        if (ws_callback_info.ready == TRUE)
        {
            prepare_next_ws_callback();
            return;
        }
    }


// The task switching routine.
    __task_switch();
}

//
// $
// INITIALIZATION
//

int init_ts(void)
{
    taskswitch_lock();
    return 0;
}

