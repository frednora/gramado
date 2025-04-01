// create.c
// Created by Fred Nora.

#include <kernel.h>

const char *init_thread_default_name = "InitThread-R3";

// ==================================================
// create_init_thread:
// The control thread of the first ring3 process.
// This is the init thread.

struct thread_d *create_init_thread(void)
{
// Called by I_x64CreateInitialProcess() in x64init.c 

    struct thread_d *t;
// This is the first thread.
// See: thread.h
    tid_t TID = (tid_t) INIT_TID;
// Loops
    register int r=0;  // Wait reason
    register int i=0;  // Message queue
    register int q=0;  // Message queue
    char *ThreadName = init_thread_default_name;

    debug_print ("create_init_thread:\n");

// We need a valid init process structure.
    if ((void *) InitProcess == NULL){
        panic ("create_init_thread: InitProcess\n");
    }
    if (InitProcess->magic != 1234){
        panic ("create_init_thread: InitProcess validation\n");
    }
// Thread structure.
    t = (void *) kmalloc( sizeof(struct thread_d) );
    if ((void *) t == NULL){
        panic ("create_init_thread: t\n");
    } 
    memset( t, 0, sizeof(struct thread_d) );

    t->objectType = ObjectTypeThread;
    t->objectClass = ObjectClassKernelObject;
    t->type = THREAD_TYPE_SYSTEM;

// TID
    t->tid = (int) (TID & 0xFFFF);

// The pointer for the process we belong to.
    t->owner_process = (void *) InitProcess;
// The PID for the process we belong to.
    t->owner_pid = (pid_t) InitProcess->pid; 

// Low priority
    t->base_priority = PRIORITY_SYSTEM_THRESHOLD;  // Static
    t->priority      = PRIORITY_SYSTEM_THRESHOLD;  // Dynamic

// Initializing values.
// For ring0 stack address we will use the value found in the TSS,
    t->HeapStart = 0;
    t->HeapSize = 0;
    t->StackStart = 0;
    t->StackSize = 0;

// #todo
// Explain it better.   
    t->link = NULL;
    t->is_linked = FALSE;

    t->exit_in_progress = FALSE;

// The kernel console associated with this thread.
// 0~3
    t->__console_id = (int) CONSOLE0;


// Local name.
    t->name_address = (unsigned long) ThreadName; 
// Execution plane.
    t->plane = BACKGROUND_THREAD;
    t->surface_rect = NULL;
    // ...
    t->input_mode = IM_MESSAGE_INPUT;

//
// PML4
//

    // pml4 physical address
    t->pml4_PA = (unsigned long ) InitProcess->pml4_PA;
    if (t->pml4_PA == 0){
        panic("create_init_thread: t->pml4_PA\n");
    }

// Page fault information.
    t->PF.in_pf = FALSE;  // Not in a pf routine.
    t->PF.pf_counter = 0;

    // Clean the 'wait reason'.
    //for ( r=0; r<8; ++r ){ t->wait_reason[r] = (int) 0; };

    t->flags = 0;

//
// == Message =========
//

// ===============================
// Message queue.

    for (q=0; q<MSG_QUEUE_MAX; ++q){
        t->MsgQueue[q] = 0;
    };
    t->MsgQueueHead = 0;
    t->MsgQueueTail = 0;

// Allocate memory for all the 32 structures.
// Add the pointers in the list.

    struct msg_d  *tmp;

    for (i=0; i<MSG_QUEUE_MAX; ++i)
    {
        tmp = (struct msg_d *) kmalloc( sizeof(struct msg_d) );
        if ((void*) tmp == NULL)
        {
            debug_print("create_init_thread: tmp\n");
            panic      ("create_init_thread: tmp\n");
        }
        memset( tmp, 0, sizeof(struct msg_d) );
        
        tmp->opaque_window = NULL;
        tmp->msg = 0;
        tmp->long1 = 0;
        tmp->long2 = 0;
        
        tmp->long3 = 0;
        tmp->long4 = 0;
        tmp->used = TRUE;
        tmp->magic = 1234;
        t->MsgQueue[i] = (unsigned long) tmp;
    };
// ============================================

// Pode sofrer preempção por tempo.
    t->is_preemptable = PREEMPTABLE;

//
// Temporizadores.
//

    // Jiffies
    t->step = 0; 

// Credits:
// O acumulo de creditos gera incremento de quantum.
    t->credits = 0;

    t->quantum           = QUANTUM_NORMAL_THRESHOLD;
    t->quantum_limit_min = QUANTUM_MIN;
    t->quantum_limit_max = QUANTUM_MAX;

    t->standbyCount = 0;

    t->runningCount = 0;
    t->runningCount_ms = 0;

    t->readyCount  = 0;
    t->ready_limit = QUANTUM_MAX;

    t->waitingCount  = 0;
    t->waiting_limit = QUANTUM_MAX;

    t->blockedCount  = 0;    //Tempo bloqueada.
    t->blocked_limit = QUANTUM_MAX;

    t->ticks_remaining = 1000;

    t->initial_time_ms = get_systime_ms();
    t->total_time_ms   = 0;

// Signal
    t->signal = 0;
    t->umask  = 0;

    t->yield_in_progress =  FALSE;
    t->sleep_in_progress =  FALSE;
    t->desired_sleep_ms = 0;

// ===================================

// Transition counter.
    t->transition_counter.to_supervisor = 0;
    t->transition_counter.to_user = 0;

// #obs: 
// Essa parte eh dependente da arquitetura, 
// deveria estar em uma pasta, por exemplo, x86/.
    // if(MachineType == i386Type){...};

//
// == Context ===================
//

// #todo: 
// Isso deve ser uma estrutura de contexto.
// #
// 0x3200 é o estado inicial de eflags.
// Existe um spawn especial para essa thread,
// onde eflags inicia com o valor 0x3000.
// See: x86init.c
// Stack frame.
// See: gva.h
// cpl and iopl
// #bugbug: weak protection for threads in ring 3.
// 0x3202
// Os processos em ring 3 chamam uma interrupção especial
// que mudam seu iopl para 0. int 199.

    t->cpl = (unsigned int) RING3;
    t->rflags_initial_iopl = (unsigned int) 3; 
    t->rflags_current_iopl = (unsigned int) 3; 

// Stack frame.
    t->context.ss     = 0x23;
    t->context.rsp    = (unsigned long) CONTROLTHREAD_STACK; 
    t->context.rflags = 0x3202;  // #caution
    t->context.cs     = 0x1B;
    t->context.rip    = (unsigned long) CONTROLTHREAD_ENTRYPOINT; 

    t->context.ds = 0x23;  
    t->context.es = 0x23;
    t->context.fs = 0x23;
    t->context.gs = 0x23;

// General purpose
    t->context.rax = 0;
    t->context.rbx = 0;
    t->context.rcx = 0;
    t->context.rdx = 0;
    t->context.rsi = 0;
    t->context.rdi = 0;
    t->context.rbp = 0;
    // rsp
    t->context.r8 = 0;
    t->context.r9 = 0;
    t->context.r10 = 0;
    t->context.r11 = 0;
    t->context.r12 = 0;
    t->context.r13 = 0;
    t->context.r14 = 0;
    t->context.r15 = 0;

// Entry point.
// O endereço incial, para controle.
    t->initial_rip = (unsigned long) t->context.rip; 

    t->saved = FALSE; 

// #bugbug
// Obs: As estruturas precisam ja estar devidamente inicializadas.
	//IdleThread->root = (struct _iobuf *) file_root;
	//IdleThread->pwd  = (struct _iobuf *) file_pwd;

//CPU configuration.
	//IdleThread->cpuID = 0;              //Qual processador.
	//IdleThread->confined = 1;           //Flag, confinado ou nao.
	//IdleThread->CurrentProcessor = 0;   //Qual processador.
	//IdleThread->NextProcessor = 0;      //Proximo processador. 

// Navigation
    t->next = NULL;
    
// Coloca na lista de estruturas.
    threadList[TID] = (unsigned long) t;

// counter
// #importante
// Contador de threads
// Vamos atualizar o contador de threads, 
// pois mais uma thread existe, mesmo que nao esteja rodando ainda.
// #importante 
// nesse caso o contador foi configurado manualmente. 
// isso acontece com as threads do gramado core.
// #importante
// A criacao da thread idle vai inicializar o contador,
// para depois so incrementarmos.

// Usado pelo scheduler.
// See: sched.c
    currentq = (struct thread_d *) t;

// The Idle thread for this processor.
    UPProcessorBlock.IdleThread = (struct thread_d *) t;

// The Next thread for this processor.
    UPProcessorBlock.NextThread = (struct thread_d *) t;

// The thread counter for this processor.
// This is the first thread.
    UPProcessorBlock.threads_counter = (int) 1; // :)

//
// == Queue =====================================
//
 
// #bugbug
// Nao ha a necessidade de colocar na fila de inicializadas
// se logo em seguida estamos selecionando para execuçao
// colocando no estado standby.

    // #bugbug: Overflow
    //queue_insert_data ( queue, (unsigned long) t, QUEUE_INITIALIZED );

// State
    t->state = INITIALIZED;

    t->used = TRUE;
    t->magic = 1234;

// == Execution ===============================
// #todo
// This method really need a prefix.
// :::: MOVEMENT 1 ( Initialized ---> Standby ).

    SelectForExecution(t);    

    //debug_print ("create_init_thread: done\n");

    return (struct thread_d *) t;
}

//
// End
//

