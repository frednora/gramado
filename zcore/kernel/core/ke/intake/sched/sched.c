// sched.c
// Scheduling support.
// Core scheduler code and related routines.
// Created by Fred Nora.

// #test
// #todo
// In the case of server operating systems,
// input responsiveness is not a priority,
// in this case, the scheduler can boost the quantum of 
// network related tasks, not the display server or
// foreground thread.

#include <kernel.h>

// Scheduler main structure.
// see: sched.h
struct scheduler_info_d  SchedulerInfo;

//Status do scheduler.
unsigned long g_scheduler_status = LOCKED;

// Normal priorities
static struct thread_d  *p1q;
static struct thread_d  *p2q;
static struct thread_d  *p3q;
// System priorities
static struct thread_d  *p4q;
static struct thread_d  *p5q;
static struct thread_d  *p6q;  // Higher

//
// == Private functions: prototypes =============
//

static void __sched_notify_parent(struct thread_d *thread, int event_number);
static tid_t __scheduler_rr(unsigned long sched_flags);

// =======================================

static void __sched_notify_parent(struct thread_d *thread, int event_number)
{
// Notify parent process that something happened.


    //#todo
    // This is a work in progress!

    struct process_d *p_owner;
    struct process_d *p_parent;

    //printk ("__sched_notify_parent: #test\n");

// Parameter:
    if ((void*) thread == NULL)
        return;
    if (thread->magic != 1234)
        return;

    // #todo:
    //if (event_number<0)
        //return;

// Owner process
// The thread belongs to this process
    pid_t owner_pid = thread->owner_pid;
    if (owner_pid < 0)
        return;
    if (owner_pid >= PROCESS_COUNT_MAX)
        return;
    p_owner = (struct process_d *) processList[owner_pid];
    if ((void*) p_owner == NULL)
        return;
    if (p_owner->magic != 1234)
        return;

// Parent process
    pid_t parent_pid = p_owner->ppid;
    if (parent_pid < 0)
        return;
    if (parent_pid >= PROCESS_COUNT_MAX)
        return;
    p_parent = (struct process_d *) processList[parent_pid];
    if ((void*) p_parent == NULL)
        return;
    if (p_parent->magic != 1234)
        return;

// Get target thread.
    struct thread_d *target_thread;
    target_thread = (struct thread_d *) p_parent->control;
    if ((void*) target_thread == NULL)
        return;
    if (target_thread->magic != 1234)
        return;

// #test
// Send message to the control thread of
// the parent process.
    tid_t Sender   = thread->tid; 
    tid_t Receiver = target_thread->tid;
    ipc_post_message_to_tid (
        Sender,
        Receiver,
        MSG_NOTIFY_PARENT,
        Sender,
        Receiver );

    //printk ("__sched_notify_parent: done\n");
}

// Lock scheduler
void scheduler_lock (void){
    g_scheduler_status = (unsigned long) LOCKED;
}

// Unlock scheduler
void scheduler_unlock (void){
    g_scheduler_status = (unsigned long) UNLOCKED;
}

/*
 * scheduler_get_status:
 *     Pega o status do scheduler, se ele está travado ou não.
 */
unsigned long scheduler_get_status (void)
{
    return (unsigned long) g_scheduler_status;
}

// #test: (Not in use yet)
// Lets end this round putting a given thread at the end
// of this round.
void sched_cut_round(struct thread_d *last_thread)
{
    struct thread_d *Current;

// Parameter:
    if ((void *) last_thread == NULL){
        return;
    }
    if ( last_thread->used != TRUE || last_thread->magic != 1234 )
    {
        return;
    }

// The current thread.
    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX )
    {
        return;
    }
    Current = (struct thread_d *) threadList[current_thread];
    if ((void *) Current == NULL){
        return;
    }
    if ( Current->used != TRUE || Current->magic != 1234 )
    {
        return;
    }

// ==========================
// Cut round
// Set the last thread for this round.

    Current->next = (struct thread_d *) last_thread;
    last_thread->next = NULL; 
}


//
// $
// SCHEDULER
//

/*
 * __scheduler_rr:
 *    Troca a thread atual, escolhe uma nova thread atual 
 * para rodar no momento.
 *    O método é cooperativo, Round Robing.
 * Ordem de escolha:
 * ================
 *  +fase 1 - Pega a próxima indicada na estrutura.
 *  +fase 2 - Pega nos slots a de maior prioridade.
 *  +fase 3 - Pega a Idle. 
 *            @todo: Nessa fase devemos usar a idle atual, 
 *            indicada em current_idle_thread.  
 *  //...
 * Obs:
 *     O que estamos fazendo aqui é incrementar a tarefa atual e 
 * olhando se a próxima tarefa da lista threadList[] está pronta pra rodar.
 * Obs:
 *     Pega na fila ReadyQueue.
 *     O scheduler deve sempre pegar da fila do dispatcher.
 */
// #todo
// Podemos contar os rounds.
// Obs: 
// ## IMPORTANTE  ##
// A thread idle somente é usada quando o sistema 
// estiver ocioso ou quando ela for a única thread.
// E é importante que a thread idle seja usada, pois 
// ela tem as instruções sti/hlt que atenua a utilização 
// da CPU, reduzindo o consumo de energia.
// OUT: next tid.

static tid_t __scheduler_rr(unsigned long sched_flags)
{ 
// + Build the p6q queue.
// + Setup p6q as the currentq, used by the task switcher.

    struct thread_d *TmpThread;

    struct thread_d *Idle;
    register int i=0;
    tid_t FirstTID = -1;

// These are the queues,
// But RR will build only the p6q, the one with higher priority.
    p1q = NULL;
    p2q = NULL;
    p3q = NULL;
    p4q = NULL;
    p5q = NULL;
    p6q = NULL;  // Higher priority

// No current queue for the task switching.
// This is the conductor.
    currentq = NULL;

// BSP - bootstrap processor
// The linked list for the BSP will always start with the
// ____IDLE thread. 
// #todo:
// We need to create another hook for the AP cores.

    if (SchedulerInfo.initialized != TRUE){
        panic("__scheduler_rr: Scheduler not initialized\n");
    }
    if (SchedulerInfo.policy != SCHED_POLICY_RR){
        panic("__scheduler_rr: Scheduler policy\n");
    }

// --------------------------------------------------------
// Idle
// O processador atual precisa ter uma idle configurada.
// #todo: Por enquanto estamos usando o UP, mas usaremos
// um dado processador para escalonar para ele.
    Idle = (struct thread_d *) UPProcessorBlock.IdleThread;
    if ((void*) Idle == NULL){
        panic("__scheduler_rr: Idle\n");
    }
    if (Idle->used != TRUE || Idle->magic != 1234){
        panic("__scheduler_rr: Idle validation\n");
    }

// A idle thread precisa ser a 
// thread de controle do processo init.
    if (Idle != InitThread){
        panic("__scheduler_rr: Idle != InitThread\n");
    }

// Estabilize the priority.
    Idle->base_priority = PRIORITY_SYSTEM_THRESHOLD;
    Idle->priority      = PRIORITY_SYSTEM_THRESHOLD;
// Estabilize the credits.
    Idle->quantum = QUANTUM_NORMAL_THRESHOLD;       

    //Idle->affinity_processor = 0;
    //Idle->current_processor = 0;
    //Idle->next_processor = 0;

// Check TID.
// Por enquanto a Idle thread desse processador
// precisa ser a InitThread. Pois ela a a primeira
// thread em user mode do primeiro processador.

    FirstTID = (tid_t) Idle->tid;

// A idle thread precisa ser a 
// thread de controle do processo init.
// INIT_TID = SYSTEM_THRESHOLD_TID.

    if (FirstTID != SYSTEM_THRESHOLD_TID){
        panic("__scheduler_rr: FirstTID\n");
    }


// This is the head of the currentq.
// Setup Idle as the head of the currentq queue, 
// used by the task switcher.
    currentq = (void *) Idle;
    qlist_set_element(SCHED_CURRENT_QUEUE,currentq);

// Setup Idle as the head of the p6q queue, 
// The loop below is gonna build this list.
// The idle is the TID 0, so the loop starts at 1.
    p6q = (void*) Idle;
    qlist_set_element(SCHED_P6_QUEUE,p6q);

// ---------------------------------------------
// Walking
// READY threads in the threadList[].


// The Idle as the head of the p6q queue, 
// The loop below is gonna build this list.
// The idle is the TID 0, so the loop starts at 1.

    if (Idle->tid != 0)
        panic("sched: Idle->tid != INIT_TID\n");
    if (Idle->tid != INIT_TID)
        panic("sched: Idle->tid != INIT_TID\n");
// Começa a pegar da thread 1, porque a 0 ja pegamos.
    int Start = (Idle->tid + 1);

// The Idle as the head of the p6q queue, 
// The loop below is gonna build this list.
// The idle is the TID 0, so the loop starts at 1.

    for ( i=Start; i<THREAD_COUNT_MAX; ++i )
    {
        TmpThread = (void *) threadList[i];

        if ((void *) TmpThread != NULL)
        {
            // ---------------------------
            // :: WAITING threads
            // Wake up some threads, given them a chance. 
            // and not putting waiting threads into the round.
            // Alarm and wakeup.
            // A thread esta esperando.
            if ( TmpThread->used == TRUE && 
                 TmpThread->magic == 1234 && 
                 TmpThread->state == WAITING )
            {
                //panic ("Gotten!\n");

                // Check alarm
                if (jiffies > TmpThread->alarm){
                    TmpThread->alarm=0;
                    //TmpThread->signal = ?
                }
                // Wake up
                //if ( TmpThread->signal ){
                //    TmpThread->state = READY;
                //}
                // Wake up
                // #bugbug
                // Como o scheduler é chamado apenas de tempos em tempos
                // então essa checagem anao é real-time.
                // Na hora de checar, pode ser que o tempo limite ja passou.
                if (jiffies >= TmpThread->wake_jiffy)
                {
                     // #debug
                    printk ("sched: j1=%d | j2=%d |\n", 
                        jiffies, TmpThread->wake_jiffy);
                    //panic ("Wake!\n");
                    printk("sched: Waking up\n");
                    do_thread_ready(TmpThread->tid);
                    //panic("Wake ok\n");
                } 
            }

            // ---------------------------
            // :: ZOMBIE threads
            if ( TmpThread->used == TRUE && 
                 TmpThread->magic == 1234 && 
                 TmpThread->state == ZOMBIE )
            {
                // #bugbug
                // If we are destroying the display server
                // or the init process.
                
                // Unregister the window server.
                if (DisplayServerInfo.initialized == TRUE)
                {
                    if (TmpThread->tid == DisplayServerInfo.tid){
                        DisplayServerInfo.initialized = FALSE;
                    }
                }
                
                // Can't kill the init process
                if ((void*) TmpThread == InitThread){
                    panic("__scheduler_rr: Can't kill InitThread\n");
                }
                
                // #test
                // (This is a work in progress)
                // Notify parent process.
                // IN: thread struct, event number.
                // __sched_notify_parent(TmpThread,0);

                TmpThread->used = FALSE;
                TmpThread->magic = 0;
                TmpThread = NULL;
            }

            // ---------------------------
            // :: READY threads
            // Schedule regular ready threads.
            // + Check the credits.
            // + Set the quantum.
            if ( TmpThread->used == TRUE && 
                 TmpThread->magic == 1234 && 
                 TmpThread->state == READY )
            {
                // Recreate the linked list.
                // The p6q and it's next.

                p6q->next = (void *) TmpThread;
                p6q       = (void *) p6q->next;

                // Initialize counters.
                TmpThread->runningCount = 0;
                TmpThread->runningCount_ms = 0;
                
                // How many times it was scheduled.
                TmpThread->scheduledCount++;

                // Balance priority levels.
                if (TmpThread->base_priority < PRIORITY_MIN){
                    TmpThread->base_priority=PRIORITY_MIN;
                }
                if (TmpThread->base_priority > PRIORITY_MAX){
                    TmpThread->base_priority=PRIORITY_MAX;
                }
                // Update the current priority based on the base priority.
                TmpThread->priority = TmpThread->base_priority;

                // Balance
                // Non interactive system services and processes.
                //if (TmpThread->personality == PERSONALITY_GRAMADO){
                //    TmpThread->quantum = QUANTUM_MIN;
                //}

                // Quantum
                // #ps: 
                // Display server and forground thread needs to be
                // the most reponsive threads.

                // Threshold for everyone.
                TmpThread->quantum = QUANTUM_NORMAL_THRESHOLD;

                // Idle: (Threshold)
                if (TmpThread == Idle){
                    TmpThread->quantum = QUANTUM_NORMAL_THRESHOLD;
                }
                // Foreground thread: (High)(Most responsive)
                if (TmpThread->tid == foreground_thread){
                    TmpThread->quantum = QUANTUM_NORMAL_TIME_CRITICAL;
                }
                // Display server: (High)(Most responsive)
                if (DisplayServerInfo.initialized == TRUE){
                    if (TmpThread->tid == DisplayServerInfo.tid){
                        TmpThread->quantum = QUANTUM_SYSTEM_TIME_CRITICAL;
                    }
                }
                // This thread received an input event.
                if (TmpThread->isResponder == TRUE){
                    TmpThread->quantum = QUANTUM_SYSTEM_TIME_CRITICAL;
                    TmpThread->isResponder = FALSE;
                }
            }

            // ---------------------------
            // :: Exit
            // exit in progress
            if (TmpThread->magic == 1234)
            {
                // Vira zombie e não sera selecionada para o proximo round
                // se não for a idle thread nem a init thread.
                if (TmpThread->exit_in_progress == TRUE)
                {
                    if (TmpThread != Idle)
                    {
                        // >> Não sera mais selecionada pelo scheduler.
                        // #todo: O dead thred collector pode terminar de deleta
                        // essa thread e deletar o processo dela
                        // se ele estiver sinalizado como exit in progress
                        // e ela for a thread de controle dele.
                        TmpThread->state = ZOMBIE;

                        // Invalidate the foreground thread variable.
                        if (TmpThread->tid == foreground_thread)
                            foreground_thread = -1;
                        
                        // Uma thread importante morreu?
                        // if (TmpThread->personality == PERSONALITY_GRAMADO )
                        //     oops();
                        
                        //se tick=1000 ticks per second.
                        //TmpThread->total_time_ms = initial_time_ms - TmpThread->steps;
                        
                        // #todo
                        // procure a thread que estava esperando esse evento
                        // e acorde ela.
                        // fazer loop.
                        // TmpThread->tid == x->wait4tid
                    }
                }
            }
            
            // Credits:
            // If this thread received more than n credits, 
            // we increment the quantum.
            if (TmpThread->magic == 1234)
            {
                if (TmpThread->credits >= 2)
                {
                    TmpThread->quantum = (TmpThread->quantum + 1);
                    TmpThread->credits = 0;
                }
            }
        }
    };

// Finalizing the list.
// This way we need to re-scheduler at the end of each round.
    p6q->next = NULL;

// #todo
// Let's try some other lists.

// Increment the counter for rr.
    SchedulerInfo.rr_round_counter++;
// Start with the idle thread.
    return (tid_t) FirstTID;
}

// Wrapper for __scheduler_rr() or other type.
// Esperamos que o worker construa um round e
// que a primeira tid seja a idle.
// OUT: next tid.

tid_t scheduler(void)
{
    struct thread_d *Idle;
    tid_t first_tid = (-1);
    //#todo: Create a method for this.
    int Policy = SchedulerInfo.policy;
    //#todo: Create a method for this.
    unsigned long sched_flags = (unsigned long) SchedulerInfo.flags;

// System state
    if (system_state != SYSTEM_RUNNING){
        panic("scheduler: system_state\n");
    }
    system_state = SYSTEM_SCHEDULING;
// Thread counter
    if (UPProcessorBlock.threads_counter == 0){
        panic("scheduler: UPProcessorBlock.threads_counter == 0\n");
    }
// Initialized
    if (SchedulerInfo.initialized != TRUE){
        panic("scheduler: SchedulerInfo.initialized\n");
    }

// Policy
// #todo: 
// Pegaremos a sched_flags de uma variavel global
// pois eh configuravel.
// IN: sched_flags
    if (Policy == SCHED_POLICY_RR){
        first_tid = (tid_t) __scheduler_rr(0);
    } else if (Policy == SCHED_POLICY_QUEUES){
        panic("scheduler: Policy not supported\n");
    } else {
        panic("scheduler: Invalid policy\n");
    };

    // ...

// ===================
    Idle = (struct thread_d *) UPProcessorBlock.IdleThread;
    if ((void *) Idle == NULL){
        panic("scheduler: Idle\n");
    }
    if (Idle->magic != 1234){
        panic("scheduler: Idle validation\n");
    }

// A primeira thread precisa ser a idle thread.
    if ( first_tid != Idle->tid ){
        panic("scheduler: first_tid != Idle->tid\n");
    }

// Update the system state
    system_state = SYSTEM_RUNNING;

// Return tid
    return (tid_t) first_tid;
}

/*
 * psScheduler:
 *    Interface para chamar a rotina de scheduler.
 *    Troca as threads que estão em user mode, 
 * usando o método cooperativo. 
 * Round Robing. 
 *    As tarefas tem a mesma prioridade.
 *    + Quando encontrada uma tarefa de maior prioridade, 
 * escolhe ela imediatamente.
 *    + Quando encontrar uma tarefa de menor prioridade, 
 * apenas eleva a prioridade dela em até dois valores 
 * acima da prioridade base, pegando a próxima tarefa. 
 *    + Quando uma tarefa está rodando à dois valores 
 * acima da sua prioridade, volta a prioridade para a 
 * sua prioridade básica e executa.
 */
// OUT: next tid.
tid_t psScheduler(void)
{

// #bugbug
// Quem está chamando? 
// Filtros?
// #todo: 
// Talvez haja mais casos onde não se deva trocar a tarefa.

//#bugbug 
//Porque retornamos 0 ???
//Scheduler Status. (LOCKED, UNLOCKED).

    if (g_scheduler_status == LOCKED){
        debug_print ("psScheduler: Locked\n");
        goto fail;
    }

// Uniprocessor with no threads in it.
    if (UPProcessorBlock.threads_counter == 0){
        panic("psScheduler: No threads\n");
    }

// There is only one thread in the uniprocessor.
// Put it into the currentq queue.
// It needs to become the current_thread and return it.
    if (UPProcessorBlock.threads_counter == 1)
    {
        // #debug
        //debug_print("psScheduler: Idle $\n");
        
        currentq = (struct thread_d *) UPProcessorBlock.IdleThread;
        current_thread = (tid_t) currentq->tid;
        return (tid_t) current_thread;
    }

// We have more than one thread into the processor,
// let's squedule it using our routine and return the tid.
    return (tid_t) scheduler();

fail:
    // Returning an invalid tid.
    return (tid_t) -1;
}

//
// $
// SYSCALL HANDLERS
//

// #test
// #todo: Explain it better.
// 777 - kinda nice() 
void sys_broken_vessels(tid_t tid)
{
    struct thread_d  *t;

    // #todo
    // Privilegies

// tid
    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }
// structure
    t = (void *) threadList[tid];
    if ((void *) t == NULL){
        return;
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        return;
    }

// Increase quatum
    if ( (t->quantum +1) <= t->quantum_limit_max )
    {
        t->quantum = (t->quantum +1);
    }
// Check limit
    if ( t->quantum > QUANTUM_MAX )
    {
        t->quantum = QUANTUM_MAX;
    }
}

// #todo: Explain it.
// IN:
// tid, ticks to wait.
void sys_sleep(tid_t tid, unsigned long ms)
{
    // #debug
    printk("sci2: [266] Sleep until\n");

// tid
    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }
// ms
    if (ms == 0){
        return;
    }
// See: schedi.c
    sleep(tid, ms);
}

void sys_yield(tid_t tid)
{
    debug_print("sys_yield:\n");
    if (tid<0 || tid >= THREAD_COUNT_MAX)
        return;
    yield(tid);
}

//
// $
// INITIALIZATION
//

/*
 * init_scheduler:
 *    Inicaliza o scheduler.
 *    @todo: Mudar para schedulerInit()
 *    #burbug: faz o mesmo que scheduler_start.
 */
// #todo: 
// Implementar inicialização de variaveis do scheduler.
// O nome poderia ser schedulerInit().
// Formato de classes.Init é um método. 
// Called by init_microkernel in ps/mk.c

int init_scheduler(unsigned long sched_flags)
{
    //register int i=0;

    debug_print("init_scheduler:\n");

    SchedulerInfo.initialized = FALSE;

    scheduler_lock();
    qlist_initialize();

// -------------------------------
// Scheduler policies
// See: sched.h
    SchedulerInfo.policy = SCHED_POLICY_RR;
    SchedulerInfo.rr_round_counter = 0;
    SchedulerInfo.flags = (unsigned long) sched_flags;

    SchedulerInfo.initialized = TRUE;
    return 0;
}


//
// End
//
