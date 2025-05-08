// schedi.c
// Scheduling support.
// Created by Fred Nora.

#include <kernel.h>  

void do_credits(struct thread_d *thread)
{
    if ((void*) thread == NULL)
        return;
    if (thread->used != TRUE)
        return;
    if (thread->magic != 1234){
        return;
    }
    thread->credits++;
}

void do_credits_by_tid(tid_t tid)
{
    struct thread_d *thread;

    if (tid<0 || tid>=THREAD_COUNT_MAX)
    {
        return;
    }

    thread = (struct thread_d *) threadList[tid];
    if ((void*) thread != NULL){
        do_credits(thread);
    }
}

// 12, 7
// do_thread_sleeping:
// Muda o state de uma thread pra blocked.
// #todo: Mudar o nome da função para do_thread_blocked.

void do_thread_blocked(tid_t tid)
{
    struct thread_d *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }

// structure
    t = (void *) threadList[tid];
    if ( (void*) t == NULL ){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

// #todo
// Se a thread estiver rodando no momento
// e ela mesmo ou uma thread de outro núcleo
// chamou essa rotina, então precisamos reescalonar ?

/*
    if (t->state == RUNNING){
        // Set the 'Need to reeschedule' flag.
    }
*/

    t->state = BLOCKED;
    t->blocked_jiffy = (unsigned long) jiffies;
}

// 10
void do_thread_dead(tid_t tid)
{
    struct thread_d *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }

// structure
    t = (void *) threadList[tid];
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

    t->state = DEAD;
}

// 0, 11
void do_thread_initialized(tid_t tid)
{
    struct thread_d *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }
// Structure
    t = (void *) threadList[tid];
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }
//  Change state.
    t->state = INITIALIZED;
}

// 3,6
void do_thread_ready(tid_t tid)
{
    struct thread_d  *t;

    if ( tid<0 || tid >= THREAD_COUNT_MAX ){
        return;
    }

// structure
    t = (void *) threadList[tid];
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

// #todo
// Se a thread estiver rodando no momento
// e ela mesmo ou uma thread de outro núcleo
// chamou essa rotina, então precisamos reescalonar ?

/*
    if (t->state == RUNNING){
        // Set the 'Need to reeschedule' flag.
    }
*/

    t->state = READY;
    t->ready_jiffy = (unsigned long) jiffies;
}

// 2,4
void do_thread_running(tid_t tid)
{
    struct thread_d *t; 

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }

    t = (void *) threadList[tid];
// Valid?
    if ((void *) t != NULL){
        if ( t->used == TRUE && t->magic == 1234 ){
            t->state = RUNNING;
        }
    }
}

// 1
void do_thread_standby(tid_t tid)
{
    struct thread_d *t; 

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }

// structure
    t = (void *) threadList[tid];
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

    t->state = STANDBY;
}

// 5, 13
void do_thread_waiting(tid_t tid, unsigned long ms)
{
    struct thread_d *t;
    unsigned long JiffiesToWait = ms;

    //printk("do_thread_waiting: %dms\n", ms);

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }

// structure
    t = (void *) threadList[tid];
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

    t->state = WAITING;
// Start
    t->waiting_jiffy = (unsigned long) jiffies;
// End
    if (JiffiesToWait == 0){
        JiffiesToWait = 1;
    }
// Time to wake up
    t->wake_jiffy = (unsigned long) (jiffies + JiffiesToWait);

// #debug
    //printk("do_thread_waiting: j1=%d | j2=%d |\n", jiffies, t->wake_jiffy);
    //printk("do_thread_waiting: done\n");
}

// 9
void do_thread_zombie(tid_t tid)
{
    struct thread_d *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }

// structure
    t = (void *) threadList[tid]; 
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

// Can't close the init thread for now.
// Only if we are in reboot/shutdown mode.
    if (tid == INIT_TID)
    {
        //
        return;
    }

    t->state = ZOMBIE;
    t->zombie_jiffy = (unsigned long) jiffies;
}

/*
 * do_waitpid:
 *     espera por qualquer um do processo filho.
 */

//#todo 
//vamos apenas lidar com a estrutura de processo.
//#importante: o kernel terá a obrigação de 
//finalizar o processo quando sua thread primária ficar em zombie. 
//poi um processo tem várias threads, e se uma fechar não significa que o 
//processo terminou. Por isso vamos apenas checar o estado dos processos e 
//não das threads.
//rever essa função, há muito o que fazer nela 
//ela precisa retornar valores padronizados e configurar 
//o status recebido

int do_waitpid (pid_t pid, int *status, int options)
{
    struct process_d *p;
    pid_t current_process = (pid_t) get_current_process();

    //#debug
    //printk ( "do_waitpid: current_process=%d pid=%d \n", 
        //current_process, pid );

    // #todo
    // tem que bloquear o processo atual até que um dos seus 
    // processos filhos seja fechado.

    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        printk ("do_waitpid: Current process struct fail\n");
        return -1;
    } else {

        if ( p->used != TRUE || p->magic != 1234 )
        {
            debug_print ("do_waitpid: validation\n");
            //#todo: Fail?
        }
        
        if ( p->used == TRUE && p->magic == 1234 )
        {

            //#debug
            //printk ("blocking process\n");
            
            p->state = PROCESS_BLOCKED;

			//significa que está esperando por qualquer
			//um dos filhos.
            p->wait4pid = (pid_t) pid; 

            //checando se a thread atual é a thread de controle. 
            if (current_thread == p->control->tid )
            {
                //#debug
                //printk ("the current thread is also the control thread\n");
            }
           
            //printk ("blocking thread\n");
            //tem que bloquear todas as threads do pai.
            //Isso pode estar falhando;
            //block_for_a_reason ( (int) p->control, (int) WAIT_REASON_WAIT4PID );
            p->control->state = BLOCKED; 
       }
   };

	//aqui precisamos dar informações sobre o status 
	//do processo

    // #bugbug: Isso pode ter falhado.
    //fake value. 
    //*status = 1; 

    //#debug
    //printk ("do_waitpid: done. \n");
    //refresh_screen();

    return (int) (-1);
}

// schediSelectForExecution:
// Change the state to 'Standby'.
// MOVIMENT 1, (Initialized --> Standby).
// It means that the thread is waiting to enter in the READY state.
// Normally a thread is in Standby when it will run for the first time.
// Taskswitch routine will probe for threads in Standby and will spawn them.
void schediSelectForExecution(struct thread_d *Thread)
{
// Change the state to 'Standby'.
// MOVIMENT 1, (Initialized --> Standby).

    if ((void *) Thread == NULL){
        debug_print ("schediSelectForExecution: Thread fail\n");
        return;
    }

    //if ( (void*) Thread->magic != 1234 ){
    //    debug_print ("schediSelectForExecution: Thread validation\n");
    //    return;
    //}

// #todo
// @todo: if initialized ---> Standby.
// @todo: if zombie ---> Standby.
// Talvez aqui seja necess�rio checar o estado da thread.
// Quem pode entrar no estado standby??
// >> Uma thread no estado initialized pode entrar no estado standby 
// >> Uma thread no estado zombie pode entrar no estado standby.
// >> @todo: se uma thread estiver em qualquer um dos outros estados ela 
// não pode entrar em standby.

//setState:

//
// MOVIMENT 1, (Initialized --> Standby).
//
    Thread->state = (int) STANDBY;
    Thread->standbyCount = 0;

//
// #bugbug      OVERFLOW !!!!!
//

// This function is wrong .... 
// Maybe it is putting values outside the vector.

    // debug_print ("schediSelectForExecution: [FIXME] Overflow in queue_insert_data() \n");
    // queue_insert_data ( queue, (unsigned long) Thread, QUEUE_STANDBY );
}

// Get the TID of the current thread.
tid_t get_current_thread(void)
{
    return (tid_t) current_thread;
}

// Set the tid for the current_thread global variable.
void set_current_thread(tid_t tid)
{
    struct thread_d *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }
    if (tid == current_thread){
        return;
    }

// structure
    t = (void *) threadList[tid]; 
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

// Change global variable.
    current_thread = (tid_t) tid;
}

// Set the tid for the foreground_thread global variable.
void set_foreground_thread(tid_t tid)
{
    struct thread_d *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX){
        return;
    }
    if (tid == foreground_thread){
        return;
    }

// structure
    t = (void *) threadList[tid]; 
    if ((void*) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

// Change global variable.
    foreground_thread = (tid_t) tid;
}

// wait_for_a_reason:
//   Faz a thread esperar  por um motivo.
void wait_for_a_reason ( tid_t tid, int reason )
{
    struct thread_d  *t;

    // #debug
    printk ("wait_for_a_reason: %d\n", reason);

// tid
    if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
        debug_print ("wait_for_a_reason: validation\n");
        return;
    } 
// reason
    if ( reason < 0 || reason >= 10 ){
        debug_print ("wait_for_a_reason: limits\n");
        return;
    }

//
// Thread
//

    t = (struct thread_d *) threadList[tid];

    if ( (void *) t == NULL ){
        debug_print ("wait_for_a_reason: t\n");
        return;

    } else {

        if ( t->used != TRUE || t->magic != 1234 ){
            debug_print ("wait_for_a_reason: t validation\n");
        }

        if ( t->used == TRUE && t->magic == 1234 )
        {
            // t->wait_reason[reason] = 1;

            // ## Wait ##
            t->state =  WAITING;
        }
    };

   //psScheduler ();

   // #debug
   printk ("wait_for_a_reason: done\n");
   //refresh_screen();
}

// wakeup_thread_reason:
//acordar uma determinada thread se ela estiver 
//esperando por um evento desse tipo.
//uma outra função pode fazer um loop acordando 
//todas as threads que esperam pelo memso motivo.

int wakeup_thread_reason ( tid_t tid, int reason )
{
    panic("wakeup_thread_reason: #todo\n");

/*
    struct thread_d *t;


    // tid
    if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
        debug_print ("wakeup_thread_reason: tid\n");
        goto fail;
    } 

    // reason
    if ( reason < 0 || reason >= 10 ){
        debug_print ("wakeup_thread_reason: reason\n");
        goto fail;
    }

//
// Thread
//

    t = (struct thread_d *) threadList[tid];

    if ( (void *) t == NULL ){
        debug_print ("wakeup_thread_reason: t\n");
        goto fail;

    } else {

        if ( t->used != TRUE || t->magic != 1234 ){
            debug_print ("wakeup_thread_reason: t validation\n");
            goto fail;
        }

		// OK.
        if ( t->wait_reason[reason] != 1 ){
            debug_print ("wakeup_thread_reason: status\n");
            goto fail;

        } else {

            //ok 

            switch (reason)
            {
				//Esperando um processo morrer. Mas qual ??
				case WAIT_REASON_WAIT4PID: 
				    //se o processo que acabamos de fechar for o mesmo que 
					//a thread estava esperando.
					if ( current_dead_process > 0 && 
					     current_dead_process == t->wait4pid )
					{
					    t->wait_reason[reason] = 0;
					    do_thread_ready (tid);
					}
					break;
					
				//Esperando uma thread morrer. Mas qual ??
				case WAIT_REASON_WAIT4TID:
				    //se a thread que acabamos de fechar for a mesma que 
					//a thread estava esperando.
					if ( current_dead_thread > 0 && 
					     current_dead_thread == t->wait4tid )
					{
					    t->wait_reason[reason] = 0;
					    do_thread_ready (tid);
					}
					break;
					
				case WAIT_REASON_TEST:
					t->wait_reason[reason] = 0;
					do_thread_ready (tid);
				    break;
			    
                // ...
			}
			
            goto done;
		}
		
		//nothing
    };

fail:
    return (int) 1;
*/

done:
    return 0;
}

// wakeup_scan_thread_reason:
// Acorda todas as threads da lista que estão esperando por 
// evento de determinado tipo.
int wakeup_scan_thread_reason (int reason)
{
    panic("wakeup_scan_thread_reason: #todo\n");

/*
    //loop
    register int i=0;

    // #debug ??
    // printk ("wakeup_scan_thread_reason: %d", reason);
    // refresh_screen();

    // reason
    if ( reason < 0 || reason >= 10 ){
        debug_print ("wakeup_scan_thread_reason: reason\n");
        goto fail;
    } 

    // loop
    for ( i=0; i<THREAD_COUNT_MAX; ++i )
    {
        wakeup_thread_reason ( i, reason );
    };

//done:
    // #debug
    printk ("wakeup_scan_thread_reason: done\n");
    refresh_screen();    
    return 0;
fail:
    // #debug
    printk ("wakeup_scan_thread_reason: fail\n");
    refresh_screen();
*/
    return (int) 1;
}

/*
 * wakeup_thread: 
 *    Para acordar uma thread, basta colocar ela no estado READY 
 * se ela estiver com seu contexto salvo e seu estado WAITING.
 *    Estando em READY o scheduler vai escalonar ela quando for possível.
 */
void wakeup_thread(tid_t tid)
{
    struct thread_d  *t;
    int Status=0;

    if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
        return;
    }

// structure
    t = (void *) threadList[tid]; 
    if ((void *) t == NULL){
        return;
    }
    if (t->used != TRUE){
        return;
    }
    if (t->magic != 1234){
        return;
    }

// Se o contexto não foi salvo. 
// Não tem como acorda-la.
// Pois acordar significa apenas retornar ao estado RUNNING.
    if (t->saved == FALSE){
        return; 
    }

// Se a thread ja está acordada, então não há mais o que fazer.
    if (t->state == READY){
        return;
    }

// Não podemos acordar uma thread que ja está rodando.
// Ela precisa estar esperando.
    if (t->state == RUNNING){
        return;
    }

// Se estiver bloqueada, 
// não tem como acordar ainda. 
// precisa desbloquear.
    if (t->state == BLOCKED){
        return; 
    }

// Isso acorda a thread.
    do_thread_ready(tid);
}

// Yield
// Set a flag that this thread will be preempted.
// Desiste do tempo de processamento.
// cooperativo.
// Muda o seu tempo executando para: Próximo de acabar.
// Flag 
// 1 = Sinaliza que a thread está dando a preferência
// e que deve sair quando for seguro fazer isso.
// Agenda o yield.
// >>> O ts.c vai fazer isso na hora certa.
// O ts.c vai esgotar o quantum dessa thread
// no momento do task switching.
// Isso não deve afetar a prioridde da thread.
// see: __task_switch() ts.c

void yield (tid_t tid)
{
    struct thread_d  *t;
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
// Set the flag.
    t->yield_in_progress = TRUE;
}

// Called by __task_switch() in ts.c
void sleep_until (tid_t tid, unsigned long ms)
{
// Dormimos e agendamos a hora de acordarmos.
// Worker. Do not call it in a sci.

    //printk("sleep_until:\n");

    if (tid<0 || tid >= THREAD_COUNT_MAX){
        return;
    }
    if (ms == 0){
        ms = 1;
    }
    do_thread_waiting(tid, ms);
}

// Yield
// Set a flag that this thread will be preempted.
// Desiste do tempo de processamento.
// cooperativo.
// Muda o seu tempo executando para: Próximo de acabar.
// Flag 
// 1 = Sinaliza que a thread está dando a preferência
// e que deve sair quando for seguro fazer isso.
// Agenda o yield.
// >>> O ts.c vai fazer isso na hora certa.
// O ts.c vai esgotar o quantum dessa thread
// no momento do task switching.
// Isso não deve afetar a prioridde da thread.

void sleep(tid_t tid, unsigned long ms)
{
// Dormimos e agendamos a hora de acordarmos.

    struct thread_d  *t;

    //printk ("sleep\n");

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

// __task_switch() will call sleep_until() to put 
// this thread waiting for ms ticks.
// After that the thread will wake up.
// #todo
// This is the type of feature that can be used 
// in the sys_select() implementation.
    t->sleep_in_progress = TRUE;
    t->desired_sleep_ms = ms;
}


// #test: (Not in used yet)
void schedi_drop_quantum(struct thread_d *thread)
{
    if ((void*) thread == NULL){
        return;
    }
    if (thread->magic != 1234){
        return;
    }
    thread->quantum = QUANTUM_MIN;
}

/*
 * schedi_check_for_standby:
 * Check for a thread in standby.
 * In this case, this routine will not return.
 * Procura na lista de threads no estado StandyBy.
 * Se tiver uma thread nessa lista, ela se torna 
 * a current. Para rodar pela primeira vez, atravéz de Spawn.
 * Não retorna se encontrar uma threa na lista.
 */
// Called by __on_finished_executing() in ts.c.

void schedi_check_for_standby(void)
{
    struct thread_d *t;
    tid_t target_tid = -1;
    register int i=0;
    register int Max = THREAD_COUNT_MAX;

    do {

        t = (void *) threadList[i];

        if ((void *) t != NULL)
        {
            if ( t->used == TRUE && 
                 t->magic == 1234 && 
                 t->state == STANDBY ) 
            {
                // Set the target thread and spawn it.
                target_tid = (tid_t) t->tid;
                goto do_spawn;
            }
        }
        i++;
    // Todas as threads da lista global.
    } while (i < Max); 

// Done: 
// Nenhuma tarefa precisa ser inicializada.
// Podemos apenas retornar para o taskswitch.
    return;

// Spawn
// See: spawn.c
do_spawn:
// tid validation?
    if ( target_tid < 0 || target_tid >= THREAD_COUNT_MAX )
    {
        goto fail;
    }
// Can't spawn the INIT_TID.
// It was the first launched thread.
    if (target_tid == INIT_TID){
        panic("schedi_check_for_standby: Can't spawn INIT_TID\n");
    }

// Set the current thread and spawn it.
    set_current_thread(target_tid);
    psSpawnThreadByTID(target_tid);

// Not reached
fail:
    panic("schedi_check_for_standby: fail\n");
}

/*
int 
sched_thread (
    struct thread_d *t, 
    unsigned long quantum,
    unsigned long priority );
int 
sched_thread (
    struct thread_d *t, 
    unsigned long quantum,
    unsigned long priority )
{
    if ( (void*) t == NULL )
        return -1;
    if (t->magic != 1234)
        return -1;

// quantum
    if (quantum>QUANTUM_MAX){
        return -1;
    }
    t->quantum = (unsigned long) quantum;

// priority
    if (priority>PRIORITY_MAX){
        return -1;
    }
    t->priority = (unsigned long) priority;

    return 0;
}
*/

//
// End
//

