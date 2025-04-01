// tlib.c
// Thread library.
// Created by Fred Nora.

#include <kernel.h>


// show_slot:
// Show info about a thread.
void show_slot(int tid)
{
    struct thread_d  *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        printk ("show_slot: tid\n");
        goto fail;
    }
// structure
    t = (void *) threadList[tid];
    if ((void *) t == NULL){
        printk ("show_slot: t\n");
        goto fail;
    }

// Show one slot
    printk ("\n");
    printk ("TID   PID   pdPA  Prio  State Quan Jiffies initial_rip rflags   tName \n");
    printk ("====  ====  ====  ====  ===== ==== ====    ==========  ======  =====   \n");
    printk ("%d    %d    %x   %d    %d    %d    %d      %x          %x      %s      \n", 
        t->tid, 
        t->owner_pid,
        t->pml4_PA,
        t->priority, 
        t->state,
        t->quantum,    // Quantum
        t->step,       // >>> Jiffies
        t->initial_rip,
        t->context.rflags,
        t->name_address );

    printk(":: to supervisor{%d} | to user{%d}\n",
        t->transition_counter.to_supervisor,
        t->transition_counter.to_user);

    if (t->tid == foreground_thread)
        printk("[__FOREGROUND__]\n");
    if (t->state == ZOMBIE)
        printk("[__ZOMBIE__]\n");
    if (t->state == DEAD)
        printk("[__DEAD__]\n");

    goto done;
fail:
    printk ("Fail\n");
done:
    return; 
}

// show_slots:
// Show info about all threads.
// Loop
// Mostra as tarefas válidas, mesmo que estejam com problemas.

void show_slots(void)
{
    struct process_d  *p;
    struct thread_d   *t;
    register int i=0;

    printk("\n");
    //printk("Thread info: jiffies{%d} sec{%d} min{%d}\n", 
        //jiffies, seconds, (seconds/60));

    printk("Thread info: jiffies{%d}\n", 
        jiffies );
    printk("Threads running: %d\n", 
        UPProcessorBlock.threads_counter );
    printk("Foreground thread: {%d}\n",
        foreground_thread );

    for ( i=0; i<THREAD_COUNT_MAX; i++ )
    {
        t = (void *) threadList[i];
        if ( (void *) t != NULL && 
             t->used == TRUE && 
             t->magic == 1234 )
        {
            show_slot(t->tid);
        }
    };

    refresh_screen();
}

/*
 * show_reg:
 *     Show the content of the registers.
 *    rflags
 *    cs:rip
 *    ss:rsp
 *    ds,es,fs,gs
 *    a,b,c,d
 */

void show_reg(int tid)
{
    struct thread_d  *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        printk("show_reg: fail\n");
        return;
    }
// structure
    t = (void *) threadList[tid];
    if ((void *) t == NULL){
        printk ("show_reg: fail\n");
        return;
    } 

// Show registers
    printk("\n");
    printk("rflags=[%x]\n", 
        t->context.rflags);
    printk("cs:rip=[%x:%x] ss:rsp=[%x:%x]\n", 
        t->context.cs, t->context.rip, t->context.ss, t->context.rsp );
    printk("ds=[%x] es=[%x] fs=[%x] gs=[%x]\n",
        t->context.ds, t->context.es, t->context.fs, t->context.gs );
    printk("a=[%x] b=[%x] c=[%x] d=[%x]\n",
        t->context.rax, t->context.rbx, t->context.rcx, t->context.rdx );
// r8~r12
// ...
}

// threads
void show_thread_information (void)
{
    struct thread_d  *Idle;
    struct thread_d  *Current;

    printk ("show_thread_information:\n");

// =================================
// Idle thread
    Idle = (struct thread_d *) UPProcessorBlock.IdleThread;
    if ((void *) Idle != NULL)
    {
        if (Idle->magic == 1234){
            printk ("Idle->tid = %d\n", Idle->tid );
        }
    }

// =================================
// Current thread
    Current = (void *) GetCurrentThread();
    if ((void *) Current != NULL)
    {
        if (Current->magic == 1234){
            printk ("Current->tid   = %d\n", Current->tid );
            printk ("current_thread = %d\n", current_thread );
        }
    }

// Show all the slots
// see: tlib.c
    show_slots(); 

    printk("Done\n");
    refresh_screen();
}

int 
link_two_threads( 
    struct thread_d *primary,
    struct thread_d *secondary )
{
// Link two threads.

    if ((void*) primary == NULL)
        goto fail;
    if (primary->magic != 1234)
        goto fail;

    if ((void*) secondary == NULL)
        goto fail;
    if (secondary->magic != 1234)
        goto fail;

// Link
    primary->link = (struct thread_d *) secondary;
    primary->is_linked = TRUE;
// Link
    secondary->link = (struct thread_d *) primary;
    secondary->is_linked = TRUE;

    return 0;
fail:
    return (int) -1;
}

int 
unlink_two_threads( 
    struct thread_d *primary,
    struct thread_d *secondary )
{
// Link two threads.

    if ((void*) primary == NULL)
        goto fail;
    if (primary->magic != 1234)
        goto fail;

    if ((void*) secondary == NULL)
        goto fail;
    if (secondary->magic != 1234)
        goto fail;

// Unlink
    primary->link = NULL;
    primary->is_linked = FALSE;
// Unlink
    secondary->link = NULL;
    secondary->is_linked = FALSE;

    return 0;
fail:
    return (int) -1;
}

// set_thread_priority:
// Muda a prioridade e o quantum de acordo com a prioridade.
// #bugbug
// Isso nao eh bom, a funcao deve fazer exatamente
// o que o nome diz, #todo: devemos criar
// outra rotina para mudar o quantum.

void 
set_thread_priority ( 
    struct thread_d *t, 
    unsigned long priority )
{
    //unsigned long NewPriority = priority;
    unsigned long OldPriority = 0;
    unsigned long BasePriority = 0;

// Limits
    if (priority < PRIORITY_MIN){
        priority=PRIORITY_MIN;
        return;
    }
    if (priority > PRIORITY_MAX){
        priority=PRIORITY_MAX;
        return;
    }

// Thread validation
    if ((void *) t == NULL){
        return;
    }
    if ( t->used != TRUE || t->magic != 1234 )
    {
        return;
    }

// Save old values.
    OldPriority  = t->priority;
    BasePriority = t->base_priority;

// Se aprioridade solicitada for igual da prioridade atual.
    if (priority == OldPriority){
        return;
    }

// Se a prioridade solicitada for menor que a prioridade basica.
// #todo: Então poderiamos usar a prioridade basica.
    if (priority < BasePriority){
        return;
    }

// Se a prioridade basica pertencer a classe de tempo real
// nao podemos mudar a prioridade.
    //if (BasePriority > PRIORITY_SYSTEM_THRESHOLD){ return; }

// Limits again
    if (priority > PRIORITY_MAX){
        t->priority = PRIORITY_MAX;
    }

// Set new priority!
// Set new quantum.
// Se aprioridade solicitada for diferente da prioridade atual.
// Tambem nao pode ser menor que a base.
    if (priority != OldPriority){
        t->priority = priority;
    }

// Quantum:
// Não mudaremos os creditos, somente a prioridade no escalonamento.
// Não se preocupe.
// O scheduler equilibra ao fim do round
// Quantum fora dos limites.

    if (t->quantum < QUANTUM_MIN){
        t->quantum = QUANTUM_MIN;
    }
    if (t->quantum > QUANTUM_MAX){
        t->quantum = QUANTUM_MAX;
    }
}

// #suspenso
// muda a prioridade para alem dos limites ... teste.
void threadi_power(
    struct thread_d *t, 
    unsigned long priority )
{

// structure
    if ((void *) t == NULL){
        return;
    }
    if ( t->used != TRUE || t->magic != 1234 )
    {
        return;
    }

// Priority
    t->priority = PRIORITY_MAX;
// Aceita se tiver nos limites.
    if ( t->priority >= PRIORITY_MIN && 
         t->priority <= PRIORITY_MAX )
    {
         t->priority = priority;
    }

// Credits
    t->quantum = QUANTUM_MAX;
}

/*
 * release:
 * #importante
 * Isso deve liberar uma thread que estava esperando 
 * ou bloqueada por algum motivo.
 * Obs: Aqui não devemos julgar se ela pode ou não ser
 * liberada, apenas alteramos se estado.
 */
// #importante:
// Não estamos selecionando ela para execução,
// apenas estamos dizendo que ela está pronta para executar.

void release(tid_t tid)
{
// Put the thread in the READY state.
    if (tid<0 || tid>=THREAD_COUNT_MAX){
        return;
    }
    do_thread_ready(tid);
}

void 
SetThread_PML4PA ( 
    struct thread_d *thread, 
    unsigned long pa )
{
    if ((void *) thread == NULL)
    {
        //
        return;
    }

// #todo:
// Aqui podemos checar a validade da estrutura,
// antes de operarmos nela.
    thread->pml4_PA = (unsigned long) pa;
}


void check_for_dead_thread_collector(void)
{
    // #importante
    // Essa flag é acionada quando uma thread 
    // entra em estado zombie.

    switch (dead_thread_collector_flag)
    {
        // waik up
        // Liberamos a thread.
        // O próprio dead thread collector vai sinalizar que 
        // quer dormir, dai o case default faz isso.

        case 1:
            // #suspenso
            // release ( RING0IDLEThread->tid );
            break;

        // sleep
        default:
            // #suspenso
            //block_for_a_reason ( 
            //    RING0IDLEThread->tid, 
            //    WAIT_REASON_BLOCKED );
            
            dead_thread_collector_flag = FALSE;
            break;
    };
}



// dead_thread_collector:
// Procura por uma thread no estado zombie mata ela.
// #todo: Alertar o processo que a thread morreu.
// #todo: O scheduler colocou algumas threads em zombie state
// e agora, temos a condição de matar as threads zombie,
// mas não pode ser a thread idle nem a thread atual.

void dead_thread_collector(void)
{
    register int i=0;
    struct thread_d *Target;
    struct thread_d *Idle; 

// Check idle

    Idle = (struct thread_d *) UPProcessorBlock.IdleThread;
    if ((void *) Idle == NULL){
        panic("dead_thread_collector: Idle\n");
    }
    if (Idle->magic != 1234){
        panic("dead_thread_collector: Idle validation\n");
    }

// Scan
// Kill zombie

    for ( i=0; i<THREAD_COUNT_MAX; i++ )
    {
        Target = (void *) threadList[i];

        if ((void *) Target != NULL)
        {
            if ( Target->state == ZOMBIE  && 
                 Target->used  == TRUE    && 
                 Target->magic == 1234 )
            {
                // Não podemos matar a Idle thread.
                if (Target == Idle){
                    panic ("dead_thread_collector: We can't kill the Idle thread!\n");
                }

                // Não podemos matar thread atual,
                // pois precisamos retornar a execução dela.
                if (Target->tid == current_thread){
                    panic ("dead_thread_collector: We can't kill the current_thread!\n");
                }

                // Muda o state para DEAD
                // e libera a estrutura para reuso.
                kill_thread(Target->tid);
            };
            //Nothing.
        };
        //Nothing.
    };

// #todo:
// MOVEMENT 10 (zombie --> Dead)
// MOVEMENT 11 (zombie --> Initialized) .. reinicializar.

}

/*
 * kill_thread:
 *     Destrói uma thread.
 *     Destroi a estrutura e libera o espaço na lista.
 */
// Muda o state para DEAD
// e libera a estrutura para reuso.
void kill_thread(tid_t tid)
{
// Kill a ZOMBIE thread.
// + Only free the object for future use.

    struct thread_d *t;

// #debug
// This is a test yet.

    printk ("kill_thread: {%d}\n", tid);
    printk ("This is a test yet\n");
    panic ("kill_thread: #debug\n");

// Can't kill the control thread of the init process.
// INIT_TID = SYSTEM_THRESHOLD_TID.

    if (tid == INIT_TID)
        return;

// tid limits.

    if (tid<0)
        return;
    if (tid>=THREAD_COUNT_MAX)
        return;

// pointer
// Thread validation

    t = (struct thread_d *) threadList[tid];
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE)
        return;
    if (t->magic != 1234)
        return;
    if (t->state != ZOMBIE)
        return;

// Change the state.
    t->state = DEAD;
// Change the validation
// Now this structure can be reused.
    t->magic = 4321;
}

// #todo
// It only can be used at the end of the reboot/shutdown routine.
void kill_all_threads(void)
{
    register int i=0;
// Start at '1'.
// Can't kill Idle thread.
// #todo: Review these numbers.
    for ( i=1; i < THREAD_COUNT_MAX; ++i )
    { 
        kill_thread(i);
    };
}

void kill_zombie_threads(void)
{
    struct thread_d *t;
    register int i=0;
    for (i=0; i<THREAD_COUNT_MAX; i++)
    {
        t = (struct thread_d *) threadList[i];
        if ((void*) t != NULL)
        {
            if (t->used == TRUE)
            {
                if (t->magic == 1234)
                {
                    if (t->state == ZOMBIE){
                        kill_thread(t->tid);
                    }
                }
            }
        }
    };
}

