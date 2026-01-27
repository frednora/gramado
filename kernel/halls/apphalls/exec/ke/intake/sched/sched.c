// sched.c
// Scheduling support.
// Core scheduler code and related routines.
// Created by Fred Nora.

// The Core Purpose (in one sentence)Every time the scheduler is invoked, 
// rebuild a new, complete, ordered linked list of threads that are 
// ready to run in the next round-robin cycle — 
// starting with the idle thread, followed by 
// all eligible READY threads in TID order.

// Why Rebuild the List Every Time?Answer in one sentence:
// Because quantum is a per-round resource, not a per-thread static value — 
// and the next round’s quantum depends on 
// dynamic runtime feedback (credits, foreground status, input events, etc.).

// #test
// #todo
// In the case of server operating systems,
// input responsiveness is not a priority,
// in this case, the scheduler can boost the quantum of network 
// related tasks, not the display server or foreground thread.

#include <kernel.h>

// Scheduler main structure.
// see: sched.h
struct scheduler_info_d  SchedulerInfo;

// Normal priorities
static struct thread_d  *p1q;  // Lower
static struct thread_d  *p2q;
static struct thread_d  *p3q;
// System priorities
static struct thread_d  *p4q;
static struct thread_d  *p5q;
static struct thread_d  *p6q;  // Higher

// ----------------------------------------
// Event responter thread
int g_use_event_responder=TRUE;
struct thread_d *ev_responder_thread;

// Global counters 
unsigned long stage_selection_count[6+1];

//
// == Private functions: prototypes =============
//

static void __sched_notify_parent(struct thread_d *thread, int event_number);

static tid_t __scheduler_rr(unsigned long sched_flags);
static tid_t __scheduler_priorityinterleaving(unsigned long sched_flags);
//...

static struct thread_d *__build_stage_queue(int stage, unsigned long priority);

static void sched_quick_and_dirty_load_balancer(void);

// =======================================

// Quick and dirty load balancer
// No parameters, uses global SchedulerInfo.
// If ready count > 3, lower InitThread quantum.
// When the system is bootstrapping into ring 3 (user mode), 
// the InitThread is the very first process that sets up the environment. 
// At that moment, it really does need full attention.
static void sched_quick_and_dirty_load_balancer(void)
{
    int ReadyCount = SchedulerInfo.rr_ready_threads_in_round;

    // Safety: make sure InitThread is valid
    if ((void*) InitThread == NULL)
        return;
    if (InitThread->used != TRUE || InitThread->magic != 1234)
        return;

    // Check the current ready count
    if (ReadyCount > 3) {
        // Lower the quantum for the init thread
        InitThread->quantum = QUANTUM_NORMAL_THRESHOLD;
    } else {
        // Restore balanced quantum when load is light
        InitThread->quantum = QUANTUM_NORMAL_BALANCED;
    }
}

static void __sched_notify_parent(struct thread_d *thread, int event_number)
{
// Notify parent process that something happened.

    // #todo
    // This is a work in progress!

    struct te_d *p_owner;
    struct te_d *p_parent;

    //printk ("__sched_notify_parent: #test\n");

// Parameters:
    if ((void*) thread == NULL)
        return;
    if (thread->magic != 1234)
        return;
    // #todo:
    //if (event_number<0)
        //return;

// Owner process
// The thread belongs to this process
// The thread group id,
// also known as 'thread environment id' (fka PID)
    pid_t teid = (pid_t) thread->tgid;
    if (teid < 0)
        return;
    if (teid >= PROCESS_COUNT_MAX)
        return;
    p_owner = (struct te_d *) teList[teid];
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
    p_parent = (struct te_d *) teList[parent_pid];
    if ((void*) p_parent == NULL)
        return;
    if (p_parent->magic != 1234)
        return;

// Get target thread
    struct thread_d *target_thread;
    target_thread = (struct thread_d *) p_parent->flower;
    if ((void*) target_thread == NULL)
        return;
    if (target_thread->magic != 1234)
        return;

// #test
// Send message to the flower thread of
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

// Get structure pointer of init thread
struct thread_d *get_init_thread(void)
{
    return (struct thread_d *) InitThread;
}

// Try to get the next thread into a linked list.
// If the next is invalid, then get the init thread,
// that normally is the first in the round.
struct thread_d *get_next_on_queue_or_the_init_thread(struct thread_d *q)
{
    struct thread_d *next;

// Invalid queue pointer
    if ((void*) q == NULL)
        return (struct thread_d *) InitThread;
    if (q->used != TRUE)
        return (struct thread_d *) InitThread;
    if (q->magic != 1234)
        return (struct thread_d *) InitThread;

// Get next
    next = (struct thread_d *) q->next;

// Invalid next pointer
    if ((void*) next == NULL)
        return (struct thread_d *) InitThread;
    if (next->used != TRUE)
        return (struct thread_d *) InitThread;
    if (next->magic != 1234)
        return (struct thread_d *) InitThread;

// Return next thread on queue
    return (struct thread_d *) next;
}

// Get the event responder thread.
// NULL if it fails.
struct thread_d *get_ev_responter(void)
{
    if (g_use_event_responder != TRUE)
        goto fail;

// Invalid pointer
    if ((void*) ev_responder_thread == NULL)
        goto fail;
    if (ev_responder_thread->used != TRUE)
        goto fail;
    if (ev_responder_thread->magic != 1234)
        goto fail;

// Return the pointer for the event responder
    return (struct thread_d *) ev_responder_thread;
fail:
    return NULL;
}

// Set the event responder thread
int set_ev_responder(struct thread_d *thread)
{
    if ((void*) thread == NULL)
        goto fail;
    if (thread->used != TRUE)
        goto fail;
    if (thread->magic != 1234)
        goto fail;

// Set
    ev_responder_thread = thread;  
    ev_responder_thread->quantum = QUANTUM_MAX;    // Quantum
    ev_responder_thread->priority = PRIORITY_MAX;  // Priority
    return 0;
fail:
    return (int) -1;
}

// Do we have a pending event on this thread?
int has_pending_event(struct thread_d *thread)
{
    if ((void*) thread == NULL)
        return FALSE;
    if (thread->used != TRUE)
        return FALSE;
    if (thread->magic != 1234)
        return FALSE;

// Do we have a pending event on this thread?
    if (thread->has_pending_event == TRUE)
        return TRUE;

// Confirm. In the case of invalid values.
    thread->has_pending_event = FALSE;
    return FALSE;
}

void sched_show_info(void)
{
    int i=0;
    for (i=0; i<=6; i++)
    {
        // Stages
        printk("[sched] Stage %d selected %d times\n", 
            i, 
            stage_selection_count[i] );
    };

    // ...
}

// Lock scheduler
void scheduler_lock (void){
    SchedulerInfo.is_locked = (int) LOCKED;
}

// Unlock scheduler
void scheduler_unlock (void){
    SchedulerInfo.is_locked = (unsigned long) UNLOCKED;
}

// Get scheduler status
int is_scheduler_locked(void){
    return (int) SchedulerInfo.is_locked;
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

// Build the queue for a given stage.
// Now: include only READY threads that match the given priority.
static struct thread_d *__build_stage_queue(int stage, unsigned long priority)
{
    //struct thread_d *Idle = (struct thread_d *) UPProcessorBlock.IdleThread;
    struct thread_d *Idle = (struct thread_d *) InitThread;
    struct thread_d *TmpThread;
    struct thread_d *head, *tail;
    int i=0;

    unsigned long Priority = priority; // #todo: In the future
    unsigned long target_quantum = QUANTUM_NORMAL_THRESHOLD;


    if (stage > 0 && stage <= 6)
        stage_selection_count[stage] = stage_selection_count[stage] + 1;

// ------------------------------------
// Decide Init quantum once, based on display server flag
    if (DisplayServerInfo.initialized == TRUE) {
        InitThread->quantum = QUANTUM_MIN;   // runtime efficiency
    } else {
        InitThread->quantum = QUANTUM_MAX;   // fast boot
    };

// ------------------------------------
// Select target quantum for this stage

    switch (priority) {
        case PRIORITY_P6:  // System High
            target_quantum = QUANTUM_Q6;
            break;
        case PRIORITY_P5:  // System Balance
            target_quantum = QUANTUM_Q5;
            break;
        case PRIORITY_P4:  // System Low
            target_quantum = QUANTUM_Q4;
            break;
        case PRIORITY_P3:  // Normal High
            target_quantum = QUANTUM_Q3;
            break;
        case PRIORITY_P2:  // Normal Balance
            target_quantum = QUANTUM_Q2;
            break;
        case PRIORITY_P1:  // Normal Low
            target_quantum = QUANTUM_Q1;
            break;
        default:
            target_quantum = QUANTUM_NORMAL_THRESHOLD;
            break;
    }

    // Always start with Idle
    head = Idle;
    tail = Idle;
    head->next = NULL;   // clear linkage

    for (i=1; i < THREAD_COUNT_MAX; i++) 
    {
        TmpThread = (struct thread_d *) threadList[i];
        if (TmpThread && TmpThread->used == TRUE && TmpThread->magic == 1234) 
        {
            // Filter by priority
            if (TmpThread->priority == priority)
            {
                // For all the READY threads.
                if (TmpThread->state == READY) 
                {
                    // Clear linkage before appending
                    TmpThread->next = NULL;

                    // Append to this stage queue
                    tail->next = TmpThread;
                    tail = TmpThread;

                    // Reset counters
                    TmpThread->runningCount = 0;
                    TmpThread->scheduledCount++;

                    // Assign the stage’s target quantum
                    if (TmpThread != InitThread)
                        TmpThread->quantum = target_quantum;

                    // #importante
                    // In the future if the priority selection is enough, 
                    // we can remove these other filters.
 
                    
                    /*
                    // Display server: The king king
                    if (DisplayServerInfo.initialized == TRUE)
                    {
                        if (TmpThread->tid == DisplayServerInfo.tid)
                        {
                            //printk("ds: priority %d\n",priority);
                            //while(1){}
                            TmpThread->quantum = QUANTUM_MAX;  //*3
                            //Idle->quantum = QUANTUM_Q1;
                        }
                    }
                    */


                    /*
                    // Foreground thread: more responsive
                    if (TmpThread->tid == foreground_thread){
                        TmpThread->quantum = QUANTUM_MAX;
                        //Idle->quantum = QUANTUM_Q1;
                    }
                    */

                    // ...
                }
            }
        }
    };

    tail->next = NULL;

// return the head of the built queue
    return (struct thread_d *) head;
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

/*/
For each thread:

 + Exit in progress: 
   Marks it as ZOMBIE, invalidates foreground thread if needed, skips scheduling.

 + WAITING: 
   Checks alarms and wakeup jiffies; if time has passed, moves it to READY.

 + ZOMBIE: 
   Cleans up, unregisters display server if needed, prevents killing InitThread.

 + READY: 
   Appends to the round-robin queue (p1q->next), resets counters, 
   increments scheduledCount, balances priority, sets quantum, 
   applies special rules for foreground and display server threads, 
   adjusts quantum if credits are high.
*/
// + Build the p1q queue.
// + Setup p1q as the currentq, used by the task switcher.
// 
// Steps:
// + Initialization checks
// + Idle thread setup
// + Queue construction
// + Thread state handling
// + Quantum balancing
// + Counters and statistics

static tid_t __scheduler_rr(unsigned long sched_flags)
{ 
    struct thread_d *TmpThread;

    struct thread_d *Idle;
    register int i=0;
    tid_t FirstTID = -1;

// These are the queues,
// But RR will build only the p1q, the one with lower priority.
    p1q = NULL;  // Lower priority
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
// thread flower do processo init.
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
// thread flower do processo init.
// INIT_TID = SYSTEM_THRESHOLD_TID.

    //if (FirstTID != SYSTEM_THRESHOLD_TID)
    if (FirstTID != INIT_TID){
        panic("__scheduler_rr: FirstTID\n");
    }

// Setup Idle as the head of all queues.

    p1q = (void*) Idle;
    qlist_set_element(SCHED_P1_QUEUE,p1q);
    p2q = (void*) Idle;
    qlist_set_element(SCHED_P2_QUEUE,p2q);
    p3q = (void*) Idle;
    qlist_set_element(SCHED_P3_QUEUE,p3q);
    p4q = (void*) Idle;
    qlist_set_element(SCHED_P4_QUEUE,p4q);
    p5q = (void*) Idle;
    qlist_set_element(SCHED_P5_QUEUE,p5q);
    p6q = (void*) Idle;
    qlist_set_element(SCHED_P6_QUEUE,p6q);

// This is the head of the currentq.
// Setup Idle as the head of the currentq queue, used by the task switcher.

    currentq = (void *) p1q; // Not necessary.
    qlist_set_element(SCHED_DEFAULT_QUEUE,p1q);

// ---------------------------------------------
// The loop below is gonna build this list.
// The idle is the TID 0, so the loop starts at 1.

// ---------------------------------------------
// Walking
// READY threads in the threadList[].

// The Idle as the head of the p1q queue, 
// The loop below is gonna build this list.
// The idle is the TID 0, so the loop starts at 1.

    if (Idle->tid != 0)
        panic("sched: Idle->tid != INIT_TID\n");
    if (Idle->tid != INIT_TID)
        panic("sched: Idle->tid != INIT_TID\n");

// Wake up init thread.
    do_thread_ready(Idle->tid);
    //if (jiffies >= Idle->wake_jiffy)
        //do_thread_ready(TmpThread->tid);

// Start from init+1, because init we already got.
    static int Start = (INIT_TID +1);
    static int Max = THREAD_COUNT_MAX;

    int ReadyCounter=0;  // Number of READY threads scheduled

// This loop builds the queue of READY threads.
// Threads in other state will be checked and the 
// structure will be updated properlly. For example: 
// we will check if it's time wo wake up a thread.
// The Idle as the head of the p1q queue.
// The idle is the TID 0, so the loop starts at 1.

    for (i=Start; i<Max; ++i){
        TmpThread = (void *) threadList[i];

        // #test: I don't like this.
        if ((void *) TmpThread == NULL)
            continue;

        if ((void *) TmpThread != NULL)
        {
            // =============================
            // Exit in progress
            if (TmpThread->magic == 1234)
            {
                // The thread has an event from the server.
                // Wake up it, if possible
                if (TmpThread->msgctl.has_event_from_ds == TRUE)
                {
                    printk("msgctl: Unblocking thread for ds event...\n");
                    if (TmpThread->state == WAITING || TmpThread->state == BLOCKED)
                    {
                        thread_wait_reason_t Reason = TmpThread->wait_reason;
                        switch (Reason){
                        case WAIT_REASON_YIELD:
                        case WAIT_REASON_PREEMPTED:
                        case WAIT_REASON_PRIORITY_BOOST:
                        case WAIT_REASON_BLOCKED:  // Generic
                        case WAIT_REASON_LOOP:     // Empty msg queue
                            do_thread_ready(TmpThread->tid);
                            TmpThread->wait_reason = WAIT_REASON_NULL;
                            break;
                        };
                    }
                    TmpThread->msgctl.has_event_from_ds = FALSE;
                    continue;
                }

                // We already setted the reason in ipc.c
                if (TmpThread->msgctl.block_in_progress == TRUE)
                {
                    printk("msgctl: Blocking...\n");
                    do_thread_blocked(TmpThread->tid);
                    TmpThread->wait_reason = WAIT_REASON_LOOP; //Waiting for message
                    TmpThread->msgctl.block_in_progress = FALSE;
                    continue;
                }

                // Vira zombie e não sera selecionada para o proximo round
                // se não for a idle thread nem a init thread.
                // Can't be the init thread (Idle) in this case.
                if (TmpThread->Deferred.exit_in_progress == TRUE)
                {
                    if (TmpThread != Idle)
                    {
                        // >> Não sera mais selecionada pelo scheduler.
                        // #todo: O dead thred collector pode terminar de deleta
                        // essa thread e deletar o processo dela
                        // se ele estiver sinalizado como exit in progress
                        // e ela for a thread flower dele.
                        TmpThread->state = ZOMBIE;

                        // Invalidate the foreground thread variable.
                        if (TmpThread->tid == foreground_thread)
                            foreground_thread = -1;
                        
                        // Uma thread importante morreu?
                        // if (TmpThread->personality == PERSONALITY_POSIX )
                        //     oops();
                        
                        //se tick=1000 ticks per second.
                        //TmpThread->total_time_ms = initial_time_ms - TmpThread->steps;
                        
                        // #todo
                        // procure a thread que estava esperando esse evento
                        // e acorde ela.
                        // fazer loop.
                        // TmpThread->tid == x->wait4tid

                        continue;
                    }
                }
            }

            // =============================
            // :: WAITING
            // WAITING threads are checked against alarms/wakeups.
            // Wake up some threads, given them a chance. 
            // and not putting waiting threads into the round.
            // Alarm and wakeup.
            // A thread esta esperando.
            // #bugbug: The wakeup check (jiffies >= wake_jiffy) is 
            // only evaluated when the scheduler runs. That means wakeups 
            // aren’t real-time — threads may oversleep until the 
            // next scheduling tick.
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
                    //printk ("sched: j1=%d | j2=%d |\n", 
                        //jiffies, TmpThread->wake_jiffy);
                    //panic ("Wake!\n");
                    //printk("sched: Waking up\n");
                    do_thread_ready(TmpThread->tid);
                    //panic("Wake ok\n");
                } else {
                    continue;
                }
            }

            // =============================
            // :: ZOMBIE
            // ZOMBIE threads are cleaned up.
            // #todo: That’s fine, but you might want a dedicated reaper/collector 
            // to free resources instead of doing it inline.
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

                // #todo
                // Find some thread that is waiting for the dead of this thread.
                // t->wait_reason = WAIT_REASON_WAIT4TID
                // if (t->wait4tid == TmpThread->tid)
                //     waike up (t) 

                TmpThread->used = FALSE;
                TmpThread->magic = 0;
                TmpThread = NULL;

                continue;
            }

            // =============================
            // :: READY
            // READY threads are appended to the new round queue.
            // Schedule regular ready threads.
            // + Check the credits.
            // + Set the quantum.
            if ( TmpThread->used == TRUE && 
                 TmpThread->magic == 1234 && 
                 TmpThread->state == READY )
            {
                // Recreate the linked list.
                // The p1q and it's next.

                p1q->next = (void *) TmpThread;
                p1q       = (void *) p1q->next;

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
                //if (TmpThread->personality == PERSONALITY_POSIX){
                //    TmpThread->quantum = QUANTUM_MIN;
                //}

                // Quantum
                // #ps: 
                // Display server and forground thread needs to be
                // the most reponsive threads.

                // Everyone (2)
                TmpThread->quantum = QUANTUM_NORMAL_BALANCED;

                // >> INIT (2)
                if (TmpThread == Idle){
                    TmpThread->quantum = QUANTUM_NORMAL_BALANCED;
                }
                // >> FG (2)
                if (TmpThread->tid == foreground_thread){
                    TmpThread->quantum = QUANTUM_NORMAL_BALANCED;
                }
                // >> DS (5)
                if (DisplayServerInfo.initialized == TRUE){
                    if (TmpThread->tid == DisplayServerInfo.tid){
                        TmpThread->quantum = QUANTUM_SYSTEM_BALANCED;
                    }
                }

                // Credits:
                // If this thread received more than n credits during the last round, 
                // we increment the quantum.
                if (TmpThread->credits >= 2){
                    TmpThread->quantum = (TmpThread->quantum + 1);
                    TmpThread->credits = 0;
                }

                ReadyCounter++;
            }
        }
    };

    SchedulerInfo.rr_ready_threads_in_round = ReadyCounter;
    sched_quick_and_dirty_load_balancer();

// Finalizing the list.
// This way we need to re-scheduler at the end of each round.
    p1q->next = NULL;

// #todo
// Let's try some other lists.

// Increment the counter for rr.
    SchedulerInfo.rr_round_counter++;
// Start with the idle thread.
    return (tid_t) FirstTID;
}

// #todo
// Gradual Rollout: 
// Consider enabling the new policy only 
// for specific thread groups or subsystems

// Very simple priority interleaving worker.
// For now: only put the init thread in all queues.
// This guarantees at least one runnable thread.
// Stage 0 sets up queues with the init thread, and 
// stages 1–6 walk through each priority queue in turn.
// Each call advances the scheduler’s stage.
static tid_t __scheduler_priorityinterleaving(unsigned long sched_flags)
{
    struct thread_d *Init;
    tid_t FirstTID = -1;

    if (SchedulerInfo.initialized != TRUE){
        panic("__scheduler_priorityinterleaving: Scheduler not initialized\n");
    }
    if (SchedulerInfo.policy != SCHED_POLICY_PRIORITY_INTERLEAVING)
        panic("__scheduler_priorityinterleaving: Scheduler policy\n");

// Current stage. 
// Round it if necessary.
    if (SchedulerInfo.stage > SchedulerInfo.max_stage)
        SchedulerInfo.stage = 1;

// Get init thread
    Init = (struct thread_d *) InitThread;
    if ((void*) Init == NULL || Init->magic != 1234)
        panic("__scheduler_priorityinterleaving: InitThread validation\n");


    // #debug
    //printk("scheduler: stage={ %d }\n", SchedulerInfo.stage);

//
// Stage 0 (Preparation)
//

// Guaranteeing a runnable thread: 
// By always placing the init thread in all queues, 
// ensure there’s at least one valid candidate.

    if (SchedulerInfo.stage == 0)
    {
        printk("scheduler: stage={ %d }\n", SchedulerInfo.stage);
        FirstTID = Init->tid;

        p1q = p2q = p3q = p4q = p5q = p6q = NULL; 

        // Put init thread in all queues
        p1q = Init;
        qlist_set_element(SCHED_P1_QUEUE, p1q);
        p2q = Init;
        qlist_set_element(SCHED_P2_QUEUE, p2q);
        p3q = Init;
        qlist_set_element(SCHED_P3_QUEUE, p3q);
        p4q = Init;
        qlist_set_element(SCHED_P4_QUEUE, p4q);
        p5q = Init;
        qlist_set_element(SCHED_P5_QUEUE, p5q);
        p6q = Init;
        qlist_set_element(SCHED_P6_QUEUE, p6q);
        
        currentq = p1q;
        currentq->next = NULL;

        // Counter
        stage_selection_count[0] = stage_selection_count[0] + 1;

        // Advance to stage 1
        SchedulerInfo.stage = 1;

        return (tid_t) FirstTID;
    }

//
// Stages 1–6 (Execution)
// 

    // The task switching walked the old list
    // and now we need to give him the next list.

// Stage 1..6: execution
    switch (SchedulerInfo.stage) 
    {
        case 1:
            //printk("scheduler: stage={ %d }\n", SchedulerInfo.stage);
            p6q = __build_stage_queue(6, PRIORITY_P6);
            currentq = p6q;
            SchedulerInfo.stage = 2; 
            break;

        case 2: 
            //printk("scheduler: stage={ %d }\n", SchedulerInfo.stage);
            p5q = __build_stage_queue(5, PRIORITY_P5);
            currentq = p5q;
            SchedulerInfo.stage = 3;
            break;

        case 3: 
            //printk("scheduler: stage={ %d }\n", SchedulerInfo.stage);
            p4q = __build_stage_queue(4, PRIORITY_P4);
            currentq = p4q;
            SchedulerInfo.stage = 4;
            break;

        case 4: 
            //printk("scheduler: stage={ %d }\n", SchedulerInfo.stage);
            p3q = __build_stage_queue(3, PRIORITY_P3);
            currentq = p3q;
            SchedulerInfo.stage = 5;
            break;

        case 5: 
            //printk("scheduler: stage={ %d }\n", SchedulerInfo.stage);
            p2q = __build_stage_queue(2, PRIORITY_P2);
            currentq = p2q;
            SchedulerInfo.stage = 6;
            break;

        case 6: 
            //printk("scheduler: stage={ %d }\n", SchedulerInfo.stage);
            p1q = __build_stage_queue(1, PRIORITY_P1);
            currentq = p1q;
            SchedulerInfo.stage = 1;
            break;

        // Resets to stage 0 if something goes wrong, 
        // reconfigures with init thread, and returns.
        default:
            //printk("scheduler: stage={ Default }\n");
            FirstTID = Init->tid;
            p1q = Init;
            p1q->next = NULL;
            //qlist_set_element(SCHED_P1_QUEUE, p1q);
            currentq = p1q;
            // Next will be 0 for reconfiguration.
            SchedulerInfo.stage = 0;

            return (tid_t) currentq->tid;
            break;
    }

// done
// Always returns the init thread’s TID (currentq->tid).
    return (tid_t) currentq->tid;
}


// Wrapper for __scheduler_rr(), __scheduler_priorityinterleaving()
//  or other types.
// Esperamos que o worker construa um round e
// que a primeira tid seja a idle.
// OUT: next tid.

tid_t scheduler(void)
{
    struct thread_d *Idle;
    tid_t first_tid = (-1);
    //#todo: Create a method for this.
    int Policy = (int) SchedulerInfo.policy;
    //#todo: Create a method for this.
    unsigned long sched_flags = (unsigned long) SchedulerInfo.flags;

// System state
    if (system_state != SYSTEM_RUNNING){
        panic("scheduler: system_state\n");
    }
    system_state = SYSTEM_SCHEDULING;

// Thread counter
// #todo: We are still in Uni-Processor mode.
    if (UPProcessorBlock.threads_counter == 0){
        panic("scheduler: UPProcessorBlock.threads_counter == 0\n");
    }
// Initialized
    if (SchedulerInfo.initialized != TRUE){
        panic("scheduler: SchedulerInfo.initialized\n");
    }

// Select the current policy
// IN: sched_flags

    if (Policy == SCHED_POLICY_RR){

        first_tid = (tid_t) __scheduler_rr(0);

    } else if (Policy == SCHED_POLICY_PRIORITY_INTERLEAVING){

        // #debug 
        // #ps: The scheduler will not be called by the ts.c 
        // if the processor has only one thread. It uses the InitThread.

        first_tid = (tid_t) __scheduler_priorityinterleaving(0);

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

// #bugbug 
// Porque retornamos 0 ???
// Scheduler Status. (LOCKED, UNLOCKED).

    if (SchedulerInfo.is_locked == LOCKED){
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
/*
    if (UPProcessorBlock.threads_counter == 1)
    {
        // #debug
        //debug_print("psScheduler: Idle $\n");
        
        currentq = (struct thread_d *) UPProcessorBlock.IdleThread;
        current_thread = (tid_t) currentq->tid;
        return (tid_t) current_thread;
    }
*/

// We have more than one thread into the processor,
// let's squedule it using our routine and return the tid.
    return (tid_t) scheduler();
fail:
    // Returning an invalid tid.
    return (tid_t) -1;
}

// Count the active threads.
// Active threads states: READY and RUNNING.
unsigned long sched_count_active_threads(void)
{
    register int i=0;
    unsigned long Counter=0;
    struct thread_d *t;

    for (i=0; i<THREAD_COUNT_MAX; i++)
    {
        t = (struct thread_d *) threadList[i];
        if ((void*)t != NULL)
        {
            if (t->used == TRUE && t->magic == 1234)
            {
                if (t->state == READY || t->state == RUNNING){
                    Counter++;
                }
            }
        }
    };

    return (unsigned long) Counter;
}

void sched_boost_ds_thread(void)
{
    struct thread_d  *t;
    tid_t target_tid = -1;

    if (DisplayServerInfo.initialized != TRUE)
        return;
    
    target_tid = (tid_t) DisplayServerInfo.tid;

    // Validate thread ID
    if (target_tid < 0 || target_tid >= THREAD_COUNT_MAX){
        return;
    }

    // Get thread object
    t = (void *) threadList[target_tid];
    if ((void *) t == NULL){
        return;
    }

    // Validate thread structure
    if (t->used != TRUE || t->magic != 1234){
        return;
    }

    // Boost quantum
    t->quantum = QUANTUM_SYSTEM_TIME_CRITICAL;
}

// Boost quantum for the foreground thread because of an input event
void sched_boost_foreground_thread(void)
{
    struct thread_d  *t;
    tid_t target_tid = foreground_thread;

    // Validate thread ID
    if (target_tid < 0 || target_tid >= THREAD_COUNT_MAX){
        return;
    }

    // Get thread object
    t = (void *) threadList[target_tid];
    if ((void *) t == NULL){
        return;
    }

    // Validate thread structure
    if (t->used != TRUE || t->magic != 1234){
        return;
    }

    // Boost quantum
    t->quantum = QUANTUM_SYSTEM_TIME_CRITICAL;
}

// Lower quantum for the current thread because it's not foreground
void sched_lower_current_thread(void)
{
    struct thread_d *t;
    tid_t target_tid = current_thread;

    // Validate thread ID
    if (target_tid < 0 || target_tid >= THREAD_COUNT_MAX){
        return;
    }

    t = (void *) threadList[target_tid];
    if ((void *) t == NULL){
        return;
    }
    if (t->used != TRUE || t->magic != 1234){
        return;
    }

    // Lower quantum temporarily
    t->quantum = QUANTUM_NORMAL_THRESHOLD;
}


//
// $
// SYSCALL HANDLERS
//

// #test
// 777 - Implementation of rtl_nice() from ring 3 library. 
void sys_nice(unsigned long decrement)
{
    struct thread_d  *t;
    tid_t target_tid = current_thread;

// #todo: Privilegies

    if (decrement == 0)
        return;

// Thread validation
    if (target_tid < 0 || target_tid >= THREAD_COUNT_MAX){
        return;
    }
    t = (void *) threadList[target_tid];
    if ((void *) t == NULL){
        return;
    }
    if (t->used != TRUE || t->magic != 1234){
        return;
    }

// Drop priority, but not below base_priority
    unsigned long DesiredPriority = (t->priority - decrement);

// Fix the problem. hahaha
    if (DesiredPriority < t->base_priority)
        t->priority = t->base_priority;

// The new priority is ok.
    if (DesiredPriority >= t->base_priority)
        t->priority = DesiredPriority;
}

// #todo: Explain it.
// IN:
// tid, ticks to wait.
void sys_sleep(tid_t tid, unsigned long ms)
{
    // #debug
    // printk("sci2: [266] Sleep until\n");

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
// See: config.h, sched.h

// #todo I guess we can create definitions in config.h
// in order to select the desired policy

    //SchedulerInfo.policy = SCHED_POLICY_RR;  // Default
    //SchedulerInfo.policy = SCHED_POLICY_PRIORITY_INTERLEAVING;
    SchedulerInfo.policy = CONFIG_CURRENT_SCHEDULING_POLICY;

//
// For rr policy
//
    SchedulerInfo.rr_round_counter = 0;
    SchedulerInfo.rr_ready_threads_in_round = 0;

//
// For queues policy
//
    SchedulerInfo.stage = 0;       // start at stage 0 (preparation)
    SchedulerInfo.max_stage = 6;   // you have 6 queues (p1q..p6q)

// Couter.
// Counting how many times each stage was selected.
    stage_selection_count[0] = 0;
    stage_selection_count[1] = 0;
    stage_selection_count[2] = 0;
    stage_selection_count[3] = 0;
    stage_selection_count[4] = 0;
    stage_selection_count[5] = 0;
    stage_selection_count[6] = 0;

// ----
    SchedulerInfo.flags = (unsigned long) sched_flags;
    SchedulerInfo.initialized = TRUE;
    return 0;
}

//
// End
//
