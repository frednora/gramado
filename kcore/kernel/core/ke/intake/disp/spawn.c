// spawn.c
// Created by Fred Nora.

#include <kernel.h>  

static int __spawn_eoi_is_necessary = FALSE;

//
// == Private functions: Prototypes =======
//

void 
__spawn_enter_usermode( 
    int eoi, 
    unsigned long entry_va, 
    unsigned long rsp3_va );

void 
__spawn_enter_kernelmode( 
    int eoi, 
    unsigned long entry_va,
    unsigned long rsp0_va );

static int __spawn_is_eoi_needed(void);

static void __spawn_load_pml4_table(unsigned long phy_addr);
static void __spawn_thread_by_tid_imp(tid_t tid);

//=====================

// global
// used by the taskswitching
void spawn_set_eoi_state(void)
{
    __spawn_eoi_is_necessary = TRUE;
}

// global
// used by the taskswitching
void spawn_reset_eoi_state(void)
{
    __spawn_eoi_is_necessary = FALSE;
}

// local
static void __spawn_load_pml4_table(unsigned long phy_addr)
{
    asm volatile ("movq %0,%%cr3"::"r"(phy_addr));
}

// local
static int __spawn_is_eoi_needed(void)
{
    return (int) __spawn_eoi_is_necessary;
}

// ------------------------
// RING 0:
// Do not check parameters.
void 
__spawn_enter_kernelmode( 
    int eoi,                 // do we need eoi ? TRUE or FALSE. 
    unsigned long entry_va,  // Entry point
    unsigned long rsp0_va )  // Stack pointer.
{
// #todo: This feature is suspended.

// This is the entry point of the new thread
    unsigned long entry = (unsigned long) entry_va;
    unsigned long rsp0  = (unsigned long) rsp0_va;

    // #debug
    //printk("rsp0: %x \n",rsp0);
    //refresh_screen();
    //while(1){}

    if (eoi == TRUE){
        asm volatile ("movb $0x20, %al \n");
        asm volatile ("outb %al, $0x20 \n");
    }

// #todo
// We need to review the stack frame for ring0
// only for ring 0 threads with iopl 0.

    asm volatile ( 
        " movq $0, %%rax  \n" 
        " mov %%ax, %%ds  \n" 
        " mov %%ax, %%es  \n" 
        " mov %%ax, %%fs  \n" 
        " mov %%ax, %%gs  \n"
        " movq %0, %%rax  \n" 
        " movq %1, %%rsp  \n" 
        " movq $0, %%rbp  \n" 
        " pushq $0x10     \n"  
        " pushq %%rsp     \n" 
        " pushq $0x0202   \n"  // Interrupts enabled for the thread that is not the first.
        " pushq $0x8      \n" 
        " pushq %%rax     \n" 
        " iretq           \n" :: "D"(entry), "S"(rsp0) );

// Paranoia
    PROGRESS("__spawn_enter_kernelmode: -- iretq fail ----\n");
    panic ("__spawn_enter_kernelmode: [ERROR] iretq fail\n");
}

// ------------------------
// RING 3:
// Do not check parameters.
void 
__spawn_enter_usermode( 
    int eoi,                 // do we need eoi ? TRUE or FALSE. 
    unsigned long entry_va,  // Entry point
    unsigned long rsp3_va )  // Stack pointer.
{
// Flying high!

// This is the entry point of the new thread
// Probably created by a ring 3 process.
    unsigned long entry = (unsigned long) entry_va;

// This is the stack pointer for the ring 3 thread.
// Probably given by a ring 3 process.
    unsigned long rsp3  = (unsigned long) rsp3_va;

    if (eoi == TRUE){
        asm ("movb $0x20, %al \n");
        asm ("outb %al, $0x20 \n");
    }

// #bugbug
// Only for ring 3 with iopl 3. weak protection.

    asm volatile ( 
        " movq $0, %%rax  \n" 
        " mov %%ax, %%ds  \n" 
        " mov %%ax, %%es  \n" 
        " mov %%ax, %%fs  \n" 
        " mov %%ax, %%gs  \n" 
        " movq %0, %%rax  \n" 
        " movq %1, %%rsp  \n" 
        " movq $0, %%rbp  \n" 
        " pushq $0x23     \n"  
        " pushq %%rsp     \n" 
        " pushq $0x3202   \n"  // Interrupts enabled for the thread that is not the first.
        " pushq $0x1B     \n" 
        " pushq %%rax     \n" 
        " iretq           \n" :: "D"(entry), "S"(rsp3) );

// Paranoia
    PROGRESS("__spawn_enter_usermode: -- iretq fail ----\n");
    panic ("__spawn_enter_usermode: [ERROR] iretq fail\n");
}

//
// $
// SPAWN
//

//-------------------------------------
// __spawn_thread_by_tid_imp:
// main worker.
// Spawn a new thread.
// The flag 'new_clone' indicates this is the control thread
// of a clone process that is running for the first time.
static void __spawn_thread_by_tid_imp(tid_t tid)
{
    struct thread_d *target_thread;
    tid_t next_tid = (tid_t) (tid & 0xFFFF);

    //PROGRESS("__spawn_thread_by_tid_imp:\n");

    // #debug
    //printk ("spawn_thread: SPAWN !\n");
    //refresh_screen();

//
// Target thread
//

    if ( next_tid < 0 || next_tid >= THREAD_COUNT_MAX )
    {
        printk("__spawn_thread_by_tid_imp: next_tid=%d", next_tid );  
        keDie();
    }
    target_thread = (void *) threadList[next_tid]; 
    if ((void *) target_thread == NULL)
    {
        printk("__spawn_thread_by_tid_imp: target_thread, next_tid={%d}", 
            next_tid );  
        keDie();
    }
    if ( target_thread->used != TRUE || target_thread->magic != 1234 )
    {
        panic("__spawn_thread_by_tid_imp: target_thread validation\n");
    }

// Is it a new clone?
// new clone
    if (target_thread->new_clone == TRUE){
        debug_print("__spawn_thread_by_tid_imp: Spawning the control thread of a new clone\n");
    }

// Check tid validation
    if (target_thread->tid != next_tid){
        panic("__spawn_thread_by_tid_imp: target_thread->tid validation\n");
    }

// State: Needs to be in Standby.
    if (target_thread->state != STANDBY){
        printk("__spawn_thread_by_tid_imp: TID={%d} not in Standby\n", next_tid );
        keDie();
    }

// Saved:
// If the context is saved, so it is not the first time.
    if (target_thread->saved == TRUE){
        printk ("__spawn_thread_by_tid_imp: Saved TID={%d}\n", next_tid );
        keDie();
    }

//
// Initializing
//

// Not saved
    target_thread->saved = FALSE;
    target_thread->exit_in_progress = FALSE;

// ??
// More checks ?
// Prepare some elements.
// #todo
// Talvez ja exista um worker para essa rotina.

    target_thread->standbyCount = 0;
    target_thread->standbyCount_ms = 0;
    target_thread->runningCount = 0;
    target_thread->runningCount_ms = 0;
    target_thread->readyCount = 0;
    target_thread->readyCount_ms = 0;
    target_thread->waitingCount = 0;
    target_thread->waitingCount_ms = 0;
    target_thread->blockedCount = 0;
    target_thread->blockedCount_ms = 0;
    // ...

// Initial jiffie
// Spawn time.
    target_thread->initial_jiffy = (unsigned long) jiffies;
// How much ticks untill now?
    target_thread->step = 0;
    target_thread->initial_time_ms = 0;
    target_thread->total_time_ms = 0;

// Linked list
// The next thread will be the window server.

    //target_thread->next = (void *) tid0_thread; 
    target_thread->next = (void *) InitThread; 
//
// MOVEMENT 2 (Standby --> Running).
//

    if (target_thread->state == STANDBY)
    {
        target_thread->state = RUNNING;
        
        // #bugbug
        // #todo
        // We have some overflow issues to resolve.
        
        //queue_insert_data ( 
        //    queue, 
        //    (unsigned long) Target, 
        //    QUEUE_RUNNING );
    }

// Destrava o mecanismo de taskswitch.
// Destrava o Scheduler.
    set_taskswitch_status(UNLOCKED);
    scheduler_unlock(); 

// Paranoia: Check state.
    if (target_thread->state != RUNNING){
        printk ("__spawn_thread_by_tid_imp: State TID={%d}\n", next_tid );
        keDie();
    }

// Do we have an owner process?
    if ((void*) target_thread->owner_process == NULL){
        panic("__spawn_thread_by_tid_imp: target_thread->owner_process\n");
    }

// Pegamos o pid.
// #bugbug: 
// Talvez o ponteiro t->process nao foi devidamente inicializado.

    //pid_t cur_pid = (pid_t) target_thread->process->pid;
    pid_t cur_pid = (pid_t) target_thread->owner_pid;

    if ( cur_pid < 0 || cur_pid >= PROCESS_COUNT_MAX ){
        panic("__spawn_thread_by_tid_imp: cur_pid\n");
    }

// #important
// The current process will be the owner pid.
    set_current_process(cur_pid);

// Set current thread

    set_current_thread(target_thread->tid);
    if ( current_thread < 0 || 
         current_thread >= THREAD_COUNT_MAX )
    {
        panic("__spawn_thread_by_tid_imp: current_thread\n");
    }

    IncrementDispatcherCount(SELECT_INITIALIZED_COUNT);

// local
// Set cr3 and flush TLB.
    //debug_print ("__spawn_thread_by_tid_imp: Load pml4\n");
    __spawn_load_pml4_table(target_thread->pml4_PA);

// #bugbug
// #todo: Maybe we need to call a method for that.
    // #bugbug: rever isso.
    asm ("movq %cr3, %rax");
    // #todo: wait here, (remember: we're using registers.)
    asm ("movq %rax, %cr3");


//
// iretq
//

    //debug_print("__spawn_thread_by_tid_imp: iretq\n");

// #todo
// Configurar a stackframe para saltar para
// qualquer ring.

// #todo
// Precisamos de uma flag que nos diga que estamos
// lançando uma thread durante uma interrupçao de timer (pit)
// para efetuarmos o 'EOI' propriamente.

    int eoi_is_needed = __spawn_is_eoi_needed();
    if (eoi_is_needed != TRUE){
        panic("__spawn_thread_by_tid_imp: eoi_is_needed != TRUE\n");
    }

//
// cpl
//

/*
    if( target_thread->cpl != RING0 && 
        target_thread->cpl != RING3)
    {
        panic("spawn_thread: Invalid cpl\n");
    }
*/

    if (target_thread->cpl != RING3){
        debug_print("__spawn_thread_by_tid_imp: Invalid cpl\n");
        panic      ("__spawn_thread_by_tid_imp: Invalid cpl\n");
    }

// ===================
// ring 0

// #bugbug
// We're having problems when receiving syscalls from ring0.
// Our system interrupt is only for ring3 processes.

    if (target_thread->cpl == RING0)
    {
        // ring0 --> ring0 ?
        // No prob, this time!
        target_thread->transition_counter.to_supervisor++;

        // #bugbug
        // Suspended.
        // We're not running threads in ring0 for now.

        //#debug
        debug_print("__spawn_thread_by_tid_imp: RING0\n");
        //printk     ("spawn_thread: RING0\n");
        //debug_print("spawn_thread: RING0 not supported yet\n");
        panic      ("__spawn_thread_by_tid_imp: RING0 not supported yet\n");

        // #suspended
        //__spawn_enter_kernelmode( 
        //    TRUE,  // EOI
        //    (unsigned long) target_thread->context.rip,
        //    (unsigned long) target_thread->context.rsp );
    }

    // #debug
    // show_reg(Target->tid);
    // refresh_screen();
    // while(1){}
    
    // Target->context.ss     & 0xffff
    // Target->context.rsp
    // Target->context.rflags
    // Target->context.cs     & 0xffff
    // Target->context.rip

// ===================
// ring 1

    //if ( target_thread->cpl == RING1 )
    //    panic("spawn_thread: ring1\n");

// ===================
// ring 2

    //if ( target_thread->cpl == RING2 )
    //    panic("spawn_thread: ring2\n");


// ===================
// ring 3
    
    if (target_thread->cpl == RING3)
    {
        target_thread->transition_counter.to_user++;
        __spawn_enter_usermode( 
            TRUE,  // EOI
            (unsigned long) target_thread->context.rip,
            (unsigned long) target_thread->context.rsp );
    }

// The party is over!
    PROGRESS("-- iretq fail --------\n");
    panic("__spawn_thread_by_tid_imp: iretq fail\n");
}

// psSpawnThread:
// Interface to spawn a thread.
void psSpawnThreadByTID(tid_t tid)
{
    PROGRESS("psSpawnThreadByTID:\n");

    if ( tid < 0 || tid >= THREAD_COUNT_MAX )
    {
        printk("psSpawnThreadByTID: TID=%d\n", tid );
        keDie();
    }
    __spawn_thread_by_tid_imp(tid);

// Not reached
    panic("psSpawnThreadByTID:\n");
}

/*
// spawn_pid:
// Spawn the control thread of a process.
// Remember we need to call this after 
// the irq0 interrupt. Cause the spawn routine
// has the eoi.
// #bugbug
// Not tested yet.
void spawn_pid(pid_t pid)
{
    struct process_d *p;
    tid_t __tid=-1;

// pid
    if (pid < 0 || pid >= PROCESS_COUNT_MAX ){
        panic("spawn_pid: pid\n");
    }

// process structure.
    p = (struct process_d *) processList[pid];
    if ((void*) p == NULL){
        panic("spawn_pid: p\n");
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        panic("spawn_pid: p validation\n");
    }

// Spawn
    __tid = (tid_t) p->control->tid;
    psSpawnThread(__tid);
// Not reached
    panic("spawn_pid: Fail\n");
}
*/

/*
void spawn_tid(int tid)
{
    struct thread_d *t;

    if (tid < 0 || tid >= THREAD_COUNT_MAX )
        panic("spawn_tid: tid\n");

    t = (struct thread_d *) threadList[tid];
    if ((void*) t == NULL)
        panic("spawn_tid: tid\n");
    if ( t->used != TRUE || t->magic != 1234 )
        panic("spawn_tid: validation\n");

// Spawn
    psSpawnThread(tid);
// Not reached
    panic("spawn_tid: Fail\n");
}
*/

