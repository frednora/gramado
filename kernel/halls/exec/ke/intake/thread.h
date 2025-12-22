// thread.h
// Created by Fred Nora.

#ifndef __INTAKE_THREAD_H
#define __INTAKE_THREAD_H    1

// =================================
// System threads: (canonical)
// The first system thread is '0'. It is the ring 3 init thread.
// The first user thread is '100'.
// We're gonna give less privilegies for those threads 
// equal and above 100. 

// -----------------------------------------
// The system's thread counter starts here.
#define SYSTEM_THRESHOLD_TID  0
#define INIT_TID    SYSTEM_THRESHOLD_TID
//...

// #test
// Special TID for kernel messages.
// Actually this thread doesn't exist.
#define KERNEL_MESSAGE_TID  99

// -----------------------------------------
// The user's thread counter starts here.
#define USER_THRESHOLD_TID    100


// --------------------------------

#define THREAD_MAGIC  1234  // In use
#define THREAD_STOCK  4321  // Free for reuse
//...

// Preemption support.
// Se pode ou não entrar em preempção. 
#define PREEMPTABLE    TRUE   // Yes
#define UNPREEMPTABLE  FALSE  // No

// Threads:
// Entre as janelas overlapped, a que estiver ativa,
// deve mudar o plano da thread para FOREGROUND.
#define BACKGROUND_THREAD  1 
#define FOREGROUND_THREAD  2

// Thread types
typedef enum {
    THREAD_TYPE_NULL,
    THREAD_TYPE_SYSTEM,  // High priority
    THREAD_TYPE_NORMAL   // Low priority
}thread_type_t;


typedef enum {

// General
// 1:: Safety. Null/default and lifecycle waits are checked before anything else.
    WAIT_REASON_NULL,           // Default / no reason (uninitialized)

// Lifecycle / Termination
// 1:: Safety. Null/default and lifecycle waits are checked before anything else.
    WAIT_REASON_WAIT4PID,       // Waiting for a child process to terminate
    WAIT_REASON_WAIT4TID,       // Waiting for a specific thread to terminate
    WAIT_REASON_PEXIT,          // Waiting for process to exit
    WAIT_REASON_TEXIT,          // Waiting for thread to exit

// Synchronization / Blocking
// 2:: Correctness. Synchronization ensures shared resources aren’t corrupted.

    WAIT_REASON_SEMAPHORE,      // Waiting on a semaphore
    WAIT_REASON_MUTEX,          // Waiting on a mutex/lock (mutual exclusion)
    WAIT_REASON_CONDITION,      // Waiting on a condition variable
    WAIT_REASON_RWLOCK,         // Waiting on a read/write lock
    WAIT_REASON_BLOCKED,        // Generic blocked state (unspecified resource)

// Scheduling / Preemption
// 3:: Performance. Scheduling/preemption decides CPU fairness.
    WAIT_REASON_PREEMPTED,      // Preempted by a higher-priority thread
    WAIT_REASON_YIELD,          // Voluntarily yielded CPU, waiting to be rescheduled
    WAIT_REASON_PRIORITY_BOOST, // Waiting for priority adjustment/boost

// I/O / External Events
// 4:: Responsiveness. I/O and messaging keep the system interactive.
    WAIT_REASON_IO,             // Waiting for I/O completion (disk, network, etc.)
    WAIT_REASON_TIMER,          // Sleeping until a timer/timeout expires
    WAIT_REASON_SIGNAL,         // Waiting for a signal/event
    WAIT_REASON_INTERRUPT,      // Waiting for a hardware interrupt

// System / Messaging
// 5:: Responsiveness. I/O and messaging keep the system interactive.
    WAIT_REASON_LOOP,           // Waiting for a system message (event loop)

// Debug / Development
// 6:: Diagnostics. Debugging reasons shouldn’t interfere with normal operation.
    WAIT_REASON_TEST,           // Developer test reason (custom use)
    WAIT_REASON_BREAKPOINT,     // Suspended at a debug breakpoint
    WAIT_REASON_TRACE           // Waiting for tracing/profiling event
} thread_wait_reason_t;


/*
This part is about input redirection ...
What is the place where i need to send the 
input that came from the hardware?
If the input model is an unix-like stuff, 
so i need to send the input to a file and 
an application will read this file.
if the input model is windows-like, 
so i will send the input to an event queue in the raw input thread.
*/

/*
 // #suspenso. Vamos usar as flags em 't->input_flags'
typedef enum {
    THREAD_INPUTMODEL_NULL,      // Not defined.
    THREAD_INPUTMODEL_KERNEL,    // Kernel thread running in ring0.
    THREAD_INPUTMODEL_COMMAND,   // Command running on console or terminal.
    THREAD_INPUTMODEL_UNIXLIKE,  // Send the input to a file.
    THREAD_INPUTMODEL_KGWS,      // Running on Setup environment and using kgws.
    THREAD_INPUTMODEL_LOADABLEWINDOWSERVER   // Using the current loadable window server.
}thread_inputmodel_t;
*/

// #todo
// t->input_mode
// Do not process anything ...
// Just port the raw byte to the target thread.
#define IM_RAW_INPUT  1
// Kernel process the raw input
// and post the standard kernel input message.
#define IM_MESSAGE_INPUT  2

/*
Earth-bound States (Thread Creation and Termination)
INITIALIZED: Thread is created but not yet ready to run.
STANDBY: Thread is ready to be scheduled for execution.
ZOMBIE: Thread has finished executing but resources haven't been fully cleaned up.
DEAD: Thread has been completely terminated and resources released.

Space-bound States (Thread Execution)
READY: Thread is ready to run and is waiting for CPU time.
RUNNING: Thread is currently executing on a CPU.
WAITING: Thread is waiting for a specific condition or event to occur.
BLOCKED: Thread is waiting for an external resource (e.g., I/O) to become available.
*/

/*
Earth States (Lifecycle Management)
 + INITIALIZED (0) Context and parameters are created, but execution hasn’t started.
 + STANDBY (1) Prepared to run for the first time, but not yet scheduled.
 + ZOMBIE (2) Execution has terminated, but the thread object still exists (often used to collect exit status).
 + DEAD (3) Fully deleted; resources can be reclaimed.

Space States (Execution Flow)
 READY (4) Thread is ready to run again (waiting for CPU scheduling).
 RUNNING (5) Actively executing instructions on the CPU.
 WAITING (6) Suspended until some condition or resource becomes available.
 BLOCKED (7) Prevented from running due to an external event (e.g., I/O wait, synchronization).
*/

/*
Conceptual Difference
 + Earth group → Describes the existence and lifecycle of the thread object itself.
 + Space group → Describes the execution status of the thread while it exists.
*/

/*
 Thread states.
 Two groups:
 + Earth: (INITIALIZED, STANDBY, ZOMBIE, DEAD)
 + Space: (READY, RUNNING, WAITING, BLOCKED)
*/
typedef enum {
// Earth states (lifecycle-related states)
    INITIALIZED,  // 0 - The context and parameters were created.
    STANDBY,      // 1 - Ready to run for the first time.
    ZOMBIE,       // 2 - Execution terminated.
    DEAD,         // 3 - Deleted. Now we can delete the object.
// Space states (execution-related states)
    READY,        // 4 - Thread is ready to run again.
    RUNNING,      // 5 - Thread is currently running.
    WAITING,      // 6 - Thread is waiting.
    BLOCKED       // 7 - Thread is blocked by an event.
}thread_state_t;


// Input model
// t->input_flags
// Com essa flag o kernel deve enviar input de teclado
// para stdin.
#define INPUT_MODEL_STDIN         1
// Com essa flag o kernel deve enviar input para
// a fila de mensagens na current thread.
#define INPUT_MODEL_MESSAGEQUEUE  2

// Thread flags.
// t->flags:
#define TF_BLOCKED_SENDING      0x1000
#define TF_BLOCKED_RECEIVING    0x2000
// ...


struct thread_transition_counter_d
{
    unsigned long to_supervisor;
    unsigned long to_user;
};


// Page fault information
struct pf_info_d
{
// This thread is in a PF interrupt routine.
    int in_pf;
// Let's count the number of pf.
    unsigned int pf_counter;
};

// --------------------------------

// Performance/Efficiency mode.
// PE_MODE_PERFORMANCE
// PE_MODE_EFFICIENCY
#define PE_MODE_PERFORMANCE  1000
#define PE_MODE_EFFICIENCY   2000

// --------------------------------

#define MSG_QUEUE_MAX  64

// The thread structure
struct thread_d 
{
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;

// type:
// (SYSTEM, INTERACTIVE, BATCH)
// #todo: If the thread is not interactive,
// so it will not receive keyboard input.
    thread_type_t type;

// The thread environment structure. (fka Process)
// thread->te->pid is returned by getpid().
    struct te_d *te;

// Thread Group ID (thread environment id)
// Shared among all threads in the same environment.
// This is the same value found in thread->te->pid.
// Returned by getpid().
    tgid_t tgid;

// Thread ID
// Unique per thread, used by the scheduler.
// Returned by gettid().
    tid_t tid;

// flags:
// TF_BLOCKED_SENDING - Blocked when trying to send.
// TF_BLOCKED_RECEIVING - Blocked when trying to receive.
// ...
    unsigned long flags;

// Priority support
    unsigned long base_priority;  // static 
    unsigned long priority;       // dynamic
// Can we boost the priority or not?
    int disable_boost;

// Quantum - Time slice
    unsigned long quantum_limit_min;
    unsigned long quantum_limit_max;
    unsigned long quantum;

// If this thread is a virtual terminal or not.
    int isVirtualTerminal;

// The kernel console associated with this thread.
// 0~3
    int __console_id;


// Input model
// Setup the input model for this thread ...
// So the kernel will know where are the places
// to send the input when this is the current thread.
// #todo:
// NOT_RECEIVING
    //unsigned long input_flags;

// #test
// #todo
// Do not process anything ...
// Just port the raw byte to the target thread.
// #define IM_RAW_INPUT    1
// Kernel process the raw input
// and post the standard input message.
// #define IM_MESSAGE_INPUT 2
    unsigned int input_mode;

// Delegate a second stdin reader for the foreground thread.
// Only the foreground thread can change this.
    tid_t stdin_second_reader_tid;

// Performance/Efficiency mode.
// PE_MODE_PERFORMANCE
// PE_MODE_EFFICIENCY
    unsigned int pe_mode;

// Surface
    struct rect_d *surface_rect;

// #todo
// Other process can't take some actions on this thread
// if it is protected. 
// ex: It can't be killed by another process.

    int _protected;

// flag, Estado atual da tarefa. ( RUNNING, DEAD ... ).
    thread_state_t state;

// This is the first thread of a new clone process. (TID?)
    int new_clone;

// #todo
// Identifica uma thread como sendo uma thread que 
// pertence à um servidor. Então as threads desse tipo 
// ganham mais prioridade e mais tempo de processamento.

    // int isServerThread;


// ---------------------------------
// Yield::
// 1 = Sinaliza que a thread está dando a preferência
// e que deve sair quando for seguro fazer isso.
    int yield_in_progress;

// ---------------------------------
// Sleep::
    int sleep_in_progress;
    unsigned long desired_sleep_ms;

// ---------------------------------
// Callback::
// During a callback, the thread can't have it's context saved.
// The callback state is a ghost context. If we save the context,
// it will overwrite the normal context.
// The callback is in progress
// The thread is following the path of: iret --> app --> restorer.
    int callback_in_progress;

// Handler (procedure)
// The app can install a handler anytime.
// Service 44000, simply install the handler
    unsigned long cb_r3_address;

// The thread is in an alertable state
// The thread enter in the alertable state when it calls
// a message loop and wait for message for example.
    int is_alertable;

// ---------------------------------
// Signal::
    int signal_in_progress;

// #todo: We need the sinal handler. (r3 address)
    unsigned long sig_handlers[32];  // installed via posix standard syscall.
    unsigned long signal;
    unsigned long umask;

// ---------------------------------
// Exit::
// We are waiting the right time to close a thread.
// The scheduler will do this job.
    int exit_in_progress;
    int exit_code;  // Reason to close the thread

// -------------

// Test
    int _its_my_party_and_ill_cry_if_i_want_to;

//
// Wait support.
//

// The thread needs priority to respont to an event.
    int has_pending_event;

// The thread is waiting for a reason.
    thread_wait_reason_t wait_reason;

// Deadlock support
// Purpose: A flag (often 0 or 1) that indicates whether 
// this thread has been identified as part of a deadlock cycle.
    int deadlock_detected;

// Plano de execução.
// Threads:
// Entre as janelas overlapped, a que estiver ativa,
// deve mudar o plano da thread para FOREGROUND.

    int plane;

//
// Names
//

// #todo
// We ned to work on that thing
// creating a canonical name for path.

    //char *name;  //@todo: Usar isso.
    unsigned long name_address;
    unsigned long name_pointer; 
    char short_name[4];
    char *cmd;  // path?
    
    // #test
    // estamos usando esse aqui.
    // Assim fica mais fácil enviar para o aplicativo.
    char __threadname[64];    // HOSTNAME_BUFFER_SIZE
    size_t threadName_len;    // len 

//
// == CPU =========================================
//

// What processor the thread is running now.
    int current_processor;
// What will be the next processor for this thread.
    int next_processor;
// Processor affinity. (CPU pinning)
// The thread will execute only on the designated CPU.
// See: https://en.wikipedia.org/wiki/Processor_affinity
    int affinity_processor;

// ORDEM: 
// O que segue é referenciado durante a interrupção de timer.

    //...

// ORDEM: 
// O que segue é referenciado durante o processo de task switch.

//
// == pml4 =======================================
//

// #todo
// COLOCAR O DIRETÓRIO DE PÁGINAS QUE A THREAD USA, 
// ISSO AJUDA NA HORA DO TASKSWITCH.

// The pml4
    unsigned long pml4_VA;
    unsigned long pml4_PA;
// The first pdpt
    unsigned long pdpt0_VA;
    unsigned long pdpt0_PA;
// The first pd
    unsigned long pd0_VA;
    unsigned long pd0_PA;


// Page Fault information.
    struct pf_info_d  PF;

// cpl
// The initial privilege level.
// The current privilege level.

    unsigned int cpl;
    unsigned int rflags_initial_iopl;
    unsigned int rflags_current_iopl;

// The thread is running in the ring0 phase
// after the isr request.
//    int in_syscall;

// Is preemptable or not.
    int is_preemptable;

// ========================================================
// ORDEM: 
// O que segue é referenciado durante o processo de dispatch.

// Heap and Stack
// #todo: Is it a virtual address?
    unsigned long HeapStart;
    unsigned long HeapSize;
    unsigned long StackStart;
    unsigned long StackSize;

// Service table?
// Endereço de um array contendo ponteiros para varios serviços
// que a thread pode usar.
    // unsigned long ServiceTable;

// ========================================================
// ORDEM: 
// Timer stuff

//
// == Time =============================
//

// ?
// Podemos criar a estrutura 'thread_time_d' t->time.step

// Transitions.
    struct thread_transition_counter_d  transition_counter;

// Quanto tempo passou, 
// mesmo quando a tarefa não esteve rodando.

    // unsigned long jiffies_alive;
    // unsigned long total_jiffies;

// step: 
// How many jiffies. 
// total_jiffies.
// Quantas vezes ela já rodou no total.
// Tempo total dado em jiffies.

// initial_jiffie: Spawn jiffie. Jiffie at spawn moment.
// ready_jiffie:   Time when the thred became ready.
// waiting_jiffie: Time when the thread started to wait.
// blocked_jiffie: Time when blocked.
// zombie_jiffie:  Time when the thread became a zombie.

    unsigned long initial_jiffy;
    unsigned long ready_jiffy;
    unsigned long waiting_jiffy;
    unsigned long blocked_jiffy;
    unsigned long zombie_jiffy;

// Time when the thread needs to wakeup.
    unsigned long wake_jiffy;

// How much jiffies until now.
    unsigned long step;

// Quando ela foi criada.
// systime inicial da thread.
// Tempo total dado em milisegundos.

    unsigned long initial_time_ms;
    unsigned long total_time_ms; 

// Credits:
// O acumulo de creditos gera incremento de quantum.
    unsigned long credits;

    
// Quantos jiffies a thread ficou no estado e espera para
// pronta para rodar.
    unsigned long standbyCount;
    unsigned long standbyCount_ms;

// Quantos jiffies ela está rodando antes de parar.
    unsigned long runningCount; 
    unsigned long runningCount_ms; 

// obs: 
// ??
// A soma das 3 esperas é a soma do tempo de espera
// depois que ela rodou pela primeira vez.

//
// Contando o tempo nos estados de espera.
//

// Tempo de espera para retomar a execução.
// Limite esperando para rodar novamente.
// Talvez essa contagem nao precise agora. 
    unsigned long readyCount;
    unsigned long readyCount_ms;
    unsigned long ready_limit;

// Quantos jiffies esperando por algum evento.
// Quantos jiffies a thread pode esperar no maximo.
    unsigned long waitingCount; 
    unsigned long waitingCount_ms; 
    unsigned long waiting_limit;

// Quantos jiffies ficou bloqueada.
// Qauntos jiffies a thread pode esperar bloqueada.
    unsigned long blockedCount;
    unsigned long blockedCount_ms;
    unsigned long blocked_limit;

// How many times it was scheduled.
    unsigned long scheduledCount;

// Zombie ?


// #todo: 
// Deadline.
// Quando tempo a tarefa tem para que ela complete a sua execução.
// unsigned long DeadLine.
// unsigned long RemainingTime; 
// Ticks remaining. (Deadline)
// Contagem do prazo limite.
// Contagem regressiva.
// Isso eh usado por threads 'real-time'
    unsigned long ticks_remaining;

// Alarm
// Used by the alarm() standard syscall.
    unsigned long alarm;

//
// == Profiler ==================================
//

// #todo: Create a structure. (local, no pointer)
// Quanto por cento do tempo a thread ficou rodando.
// No caso do processo, é a soma do quanto ficou rodando 
// todas as suas threads.
// ? these names are not good.
    unsigned long profiler_percentage_running;
    unsigned long profiler_percentage_running_res;
    unsigned long profiler_percentage_running_mod;
    unsigned long profiler_ticks_running;
    unsigned long profiler_last_ticks;

// ORDEM: 
// O que segue é referenciado com pouca frequencia.

//lista de arquivos ??
//fluxo padrão. stdio, stdout, stderr
	//unsigned long iob[8];
    
//#bugbug: o vetor Stream[] conterá essas stream também.
//ponteiros para as streams do fluxo padrão.
//O processo tem streams ... Stream[] ...
//cada tread pode ter suas stream ... mesmo que herde streams 
//de processo ...
// ?? threads diferentes do mesmo processo podem atuar em streams 
// diferentes ??
	//unsigned long standard_streams[3];
	//unsigned long Streams[8];

//Obs: Cada processo está atuando em um diretório,
// mas será cada thread precisa atuar em um diretório diferente??
    //struct _iobuf *root;  // 4 root directory
    //struct _iobuf *pwd;   // 5 (print working directory) 
    //...

//
// link
//

// #test
// 'Group of two threads'.
// Copy when writing.
// + The purpose here is connecting two threads
// to post messages to two thread at the same time.
// + If the thread is linked to another thread,
// so, also post the same message to the linked thread.
// + It can produce a kind of echo for 'debuggers'.
// + Maybe some operations are only for connected threads.
// ex: When can change the priority oof both threads at the same time.
// >>> Se cada thread for encarada como se fosse um dispositivo serial,
//     entao faz sentido enviarmos mensagens pequenas entre elas.
//     Uma thread aduando como computador e outra como terminal.
    struct thread_d *link;
    int is_linked;

//
// Security
//

// #importante
// Isso é usado para gerência de memória e 
// para privilégios de acesso.

// usersession and cgroup
    struct usession_d *usession;  // user session.
    struct cgroup_d *cg;          // cgroup.

// #ORDEM:  
// O que segue é referenciado durante as trocas de mensagens.
// utilização de canais e IPC.

//
// LIS - Local Input State
//

// #important
// Here we have some elements to handle the input events
// in this thread.
// + We have a block for single shot event.
// + We have a block for a queue of events.
// + We have a set of variables to handle the flow.
// ...
// Keyboard input and window focus information:
// + Which window has keyboard focus?
// + Which window is active?
// + Which keys are considered pressed down?
// + The state of the caret.
// Mouse cursor management information
// + Which window has mouse capture?
// + The shape of the mouse cursor?
// + The visibility of the mouse cursor.
// ...
// #todo
// We don't need to have a window as an element in the event.
// We can include this window later when the app calls get_message().
// When the thread calls GetMessage(), 
// the keyboard event is removed from the queue and 
// assigned to the window that currently has input focus.
// #test
// Sending a message to the kernel.
// no queue.
// Flag avisando que tem nova mensagem.
// O kernel deve checar essa flag. Se ela estiver acionada,
// significa que o kernel deve processar essa mensagem.

//
// == Event queue ===========================================
//

// This is the event queue.
// It is used by the ring 3 processes.
// #todo: 
// We can put all these into a structure.
// Lists:
// window_list: pointer to window structure.
// msg_list:    event type.
// long1_list:  data 1
// long2_list:  data 2
// long3_list:  data 3
// long4_list:  data 4
// MAXEVENTS
// See: events.h

// ====================================================
// Message Queue
// For the msg_d structure,
// see: window.h
    unsigned long MsgQueue[MSG_QUEUE_MAX];
    int MsgQueueHead;
    int MsgQueueTail;

    // #test
    // For mouse move optimization
    // Saving the position for the last input.
    //int MsgLastMessageIndex;

// ====================================================

// #test
// Contador de mensagens enviadas.
// Também podemos contar alguns tipos de mensagens.

    //unsigned long post_message_counter;

// Waiting for a child:
// id do processo que a thread está esperando morrer.
    int wait4pid;
// id da thread que a thread está esperando morrer.
    int wait4tid;

//---------------------------------------

//
// Context
//

// see: x64cont.h
    struct x64_context_d  context;
// The context is already saved or not?
    int saved;
// Para o kernel saltar para o novo processo.
    unsigned long ring0_rip;  //usado com o pd do kernel?
    unsigned long ripPA;
// O endereço incial, para controle.
    unsigned long initial_rip;

//
//  tss
//

// #todo
// isso é muito necessário.
// #todo: Create the structure.
    //struct x64tss_d *tss;

// Navigation
    struct thread_d *prev;
    struct thread_d *next;
};

// See: thread.c
extern struct thread_d  *InitThread;
extern struct thread_d  *ClonedThread;


// Maximum number of kernel threads in the system.
// Cada lista poderá usasr uma prioridadr diferente,
// um quantum diferente e talvez ter uma frequencia de timer diferente.
// All the threads
// see: thread.c

#define THREAD_COUNT_MAX  1024 
//#define THREAD_COUNT_MAX  4096
extern unsigned long threadList[THREAD_COUNT_MAX];

//
// == prototypes ===========================
//


// From tlib.c
void show_slot(tid_t tid);
void show_slots(void);
void show_reg(tid_t tid);
void show_thread_information(void);

// See: main.c
void keEarlyRing0IdleThread(void);

// From thread.c

int destroy_thread_structure(struct thread_d *thread);
int gc_thread_structure(struct thread_d *thread);

unsigned long GetThreadStats( tid_t tid, int index );

int getthreadname(tid_t tid, char *buffer);
void *FindReadyThread (void);
thread_state_t GetThreadState(struct thread_d *thread);
thread_type_t GetThreadType(struct thread_d *thread);

void SetCurrentTID(tid_t tid);
tid_t GetCurrentTID(void);

void *GetThreadByTID(tid_t tid);
void *GetCurrentThread(void);
void *GetForegroundThread(void);
void *GetDSThread(void);

void 
set_thread_priority ( 
    struct thread_d *t, 
    unsigned long priority );
    
void threadi_power(
    struct thread_d *t, 
    unsigned long priority );

void release(tid_t tid);

void SelectForExecution ( struct thread_d *Thread );

unsigned long 
thread_get_profiler_percentage (struct thread_d *thread);

void thread_show_profiler_info(void);
int thread_profiler(int service);

int 
link_two_threads( 
    struct thread_d *primary,
    struct thread_d *secondary );

int 
unlink_two_threads( 
    struct thread_d *primary,
    struct thread_d *secondary );

//
// Creation
//

struct thread_d *copy_thread_struct(struct thread_d *thread);

// =====

//
// Exit
//

int exit_thread(tid_t tid);
int exit_current_thread(void);

// ===

void 
SetThread_PML4PA ( 
    struct thread_d *thread, 
    unsigned long pa );

void check_for_dead_thread_collector (void);
void dead_thread_collector (void);

void kill_thread(tid_t tid);
void kill_all_threads(void);
void kill_zombie_threads(void);


//
// $
// CREATE THREAD
//

struct thread_d *create_thread ( 
    thread_type_t thread_type,
    struct cgroup_d  *cg,
    unsigned long init_rip, 
    unsigned long init_stack, 
    ppid_t pid, 
    const char *name,
    unsigned int cpl );

//
// $
// INITIALIZATION
//

int init_threads(void);

#endif    























