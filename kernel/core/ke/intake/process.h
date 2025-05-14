// process.h
// Created by Fred Nora.

#ifndef __PS_PROCESS_H
#define __PS_PROCESS_H    1

//#bugbug
//talvez aqui nao seja o melhor lugar pra definir isso.
#define gramado_pid_t  pid_t

#define PROCESS_MAGIC  1234  // In use
#define PROCESS_STOCK  4321  // Free for reuse
//...


// #bugbug
// The 'heap pool' has 2MB in size
// and it starts at 0x01000000. (16MB mark).
// 2MB = 2048 KB.
// (2048/64) = 32 KB.
// It has 64 processes with 32KB each.
#define __HEAP_POOL_SIZE_IN_BYTES       (2048 * 1024)
#define G_DEFAULT_PROCESSHEAP_SIZE      (  32 * 1024)
#define G_DEFAULT_PROCESSHEAP_COUNTMAX  (64) 

//-----------------

//
// Process environment
//

// -----------------------------
// Environment subsystems.
// For commands and applications.
typedef enum {
    UnknownSubsystem,
    CaliSubsystem,     // ama/
    GramadoSubsystem   // zing/
    // ...
} env_subsystem_t;

// -----------------------------

// This process was made to Gramado OS.
#define PERSONALITY_GRAMADO  1000
// ...

// Processes:
// Os processos BATCH são BACKGROUND.
// Os processos GWS são FOREGROUND.
#define BACKGROUND_PROCESS  1 
#define FOREGROUND_PROCESS  2


// Image. 
// (Base da imagem de um processo de usuário).
// virtual address
#define UPROCESS_IMAGE_BASE  0x200000 
/* 0x00200000 = image base */
/* 0x00201000 = The entry point */


//Process Limit. (User process) 
//(O limite é o início do kernel).
#define UPROCESS_PROCESS_LIMIT  0x30000000 

// Heap.
// Base default do heap do processo.
// "Endere�o virtual de onde come�a o heap de um processo,
//  evidentemente, cada processo ter� seu heap em um 
//  endere�o f�sico diferente".

// #bugbug
#define UPROCESS_DEFAULT_HEAP_BASE  0x80000000 
//Tamanho default do heap do processo.  
#define UPROCESS_DEFAULT_HEAP_SIZE  0x2000

//Stack.
//Deslocamento default do in�cio da pilha em rela��o ao in�cio do kernel. #bugbug
#define UPROCESS_DEFAULT_STACK_OFFSET 0x2000   
//Base default da pilha do processo.
#define UPROCESS_DEFAULT_STACK_BASE ( UPROCESS_PROCESS_LIMIT - UPROCESS_DEFAULT_STACK_OFFSET )  
//Tamanho da pilha do processo.   
#define UPROCESS_DEFAULT_STACK_SIZE  0x2000    


//
// Flags for cloning a process.
//

// Used in copy_process();

#define CLONE_THREAD    1
// ...


typedef enum {
    PROCESS_TYPE_NULL,
    PROCESS_TYPE_SYSTEM,       // high priority
    PROCESS_TYPE_INTERACTIVE,  // medium priority
    PROCESS_TYPE_BATCH,        // low priority
}process_type_t;


/*
 * process_state_t
 *     Status de um processo.
 *     @todo: Pode-se usar mais status.
 *            como estado de transi��o.
 *  Os status de um processo s�o diferentes do status de uma thread.
 */ 
// canonical.
typedef enum {
    PROCESS_CREATED,
    PROCESS_INITIALIZED,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED,
    PROCESS_IN_MEMORY,        //O processo est� carregado na mem�ria.
    PROCESS_OUT_OF_MEMORY,    //O processo n�o est� carregado na mem�ria.
    // ...
}process_state_t;


/* 
 appmode_t
 
 Importante:
 
 APPMODE_TERMINAL = O kernel cria uma estrutura de terminal 
 com uma janela associada a essa estrutura, essa janela ser� a 
 janela de terminal para o aplicativo.
 APPMODE_WINDOW = O kernel n�o cria estrutura de terminal para 
 esse processo e o processo criar� janelas.
 */

typedef enum {
    APPMODE_NULL,      // Isso se aplica ao processo kernel e ao processo idle por exemplo.
    APPMODE_TERMINAL,  // O kernel cria uma janela de terminal para o aplicativo.
    APPMODE_WINDOW,    // O kernel não cria janela de terminal para o aplicativo
}appmode_t;


//
// Group of users.
//

#define GID_DEFAULT  0
// ...

// Security Access token.
struct token_d
{
    int used;
    int magic;

    int token_id;
    int token_type;
    // ...

    // pid_t created_by_pid;

    // Capabilities

    // Expiration time.

// ================================================
// USER

// UID (User ID): 
// Represents the user identity of the process owner. 
// It determines what actions a user can perform on the system.
    uid_t uid;

// RUID (Real User ID): 
// The actual user ID of the person who started the process. 
// It remains unchanged to track the original owner.
    uid_t ruid;

// EUID (Effective User ID):
// Indicates the user identity under which a process is currently operating. 
// It can temporarily differ from the UID for privilege escalation.
// (Run as administrator)
    uid_t euid;

// SUID (Saved User ID): 
// Stores the EUID for use when processes need to return to their original privileges 
// after executing with elevated rights.
    uid_t suid;

// ================================================
// GROUP OF USERS

// GID, (Group Identification).
    gid_t gid;

// RGID (Real Group ID): 
// Denotes the group ID of the user who initiated the process, 
// reflecting the user's primary group membership.
    gid_t rgid;

// EGID (Effective Group ID): 
// Represents the group identity the process is actively operating under, 
// which determines group-based permissions.
    gid_t egid;

// SGID (Saved Group ID): Works like the SUID but for group IDs, 
// allowing processes to save their effective group identity.
    gid_t sgid;

// ================================================
// More ...

};


// PCB - Process Control Block

struct process_d 
{
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;
    process_type_t type;

// --------------------------
// PROCESS

// Process IDentifier. (posix)
    pid_t pid;
// Parent Process IDentifier 
    ppid_t ppid;
// Process Group IDentifier.
    pgid_t pgid;

//
// Name
//

// Short name
// +name     (EXEMPLO.BIN)
    char __processname[64];
    size_t processName_len;

// Long name. (extra)
    char *name;  //Nome do processo. 

// +pathname (/root/boot/EXEMPLO.BIN)
	//char *pathname;  //@todo: Incluir.	

// +cmd      (EXEM) 
    char *cmd;  //Nome curto que serve de comando.

//
// == Priorities ============
//

// Priority levels.
// Used by processes and threads.
// Classes:
// 1 ~ 5 = variable.
// 6 ~ 9 = realtime.
// variable:
//     Can be changed on the fly.
// realtime:
//     Can't be changed on the fly.
// # ps:
// The base priority is never changed. It's used to classify
// the priority level.
// The priority can't be changed to a level below the base priority.
// The base priority is static and the current priority is dinamic.

    unsigned long base_priority;  // static 
    unsigned long priority;       // dinamic

// Security Access Token
    struct token_d  token;

// --------------------------
// #test
    //struct ns_proxy_d *nsproxy;

// --------------------------

// The target OS for the process.
// It can be a different version of Gramado OS or another OS.
    int personality;

// The environment subsystem that this process is running into.
// unknown, cali, gramado.
    env_subsystem_t env_subsystem;

// Importante:
// isso substituir� a flag 'terminal'
// APPMODE_TERMINAL = O kernel cria uma estrutura de terminal 
// com uma janela associada a essa estrutura, essa janela ser� a 
// janela de terminal para o aplicativo.
// APPMODE_WINDOW = O kernel n�o cria estrutura de terminal para 
// esse processo e o processo criar� janelas.

    appmode_t appMode;

// #todo
// Usage, in percentage.
// How much this process is using the shared system resources.

    //int memory_usage_in_percentage;
    //int cpu_usage_in_percentage;
    //int disk_usage_in_percentage;
    //int network_usage_in_percentage;

// Other process can't take some actions on this process 
// if it is protected. ex: It can't be killed by another process.
    int _protected;

// State.
// flag ?
    process_state_t state; 


// Plano de execuçao.
// Processes:
// Os processos BATCH são BACKGROUND.
// Os processos GWS são FOREGROUND.

    int plane;

    // see: layer.h
    int _layer;


//
// Input
//

// #importante
// De onde vem o o input do processo. (stdin)
// O stdin pode vim de um terminal. Pois um terminal
// pode estar gerenciando o input de teclado ou mouse e
// transferindo os dados para o processo atrav�s de uma 
// disciplina de linhas.
// ex: #todo INPUTTYPE_TTY INPUTTYPE_RAW ...

    int input_type;

//
// tty 
//

// Essa é a tty do processo.
// Ela será master se o processo for um shell e
// Será slave se o processo for um terminal.
// O terminal(slave) encontrará o shell(master) em tty->link.

    struct tty_d  *tty;

// Um buffer no app en ring3
// onde o driver de rede pode colocar conte�do e depois
// avisar o processo via mensagem.

    char *net_buffer;


// ============================================

// ---------------------------
// #test
// Controlling the acccess to the main folders
// of the system disk.
// List of root chars
// 0 = '/' - Regular root dir.
// 1 = '@' - Access to /GRAMADO/ root dir.
// 2 = '#' - Access to /DE/ root dir
// TRUE = Can access
// FALSE = Can't access
    //int root_access[3]; (#todo: put this into the structure)
    //struct fs_access_d  fs_access;
    // ex: p->fs_access.root_access[0] = TRUE;
    // ex: p->fs_access.root_access[1] = FALSE;
// ---------------------------

// Open files.
// #todo:
// Check this.
// We're still using 32 in all over the kernel.
    unsigned long Objects[64];

// Connectors.
// 2 Indexes to the table above, Objects[i];
// The connectors are created in copy_process_struct() in clone.c.
// when the terminal is clonning himself to create a child.
    int Connectors[2];
// Only the terminals can create connectors.
    int _is_terminal;           // Father process
    int _is_child_of_terminal;  // Child process.

// ORDEM: 
// O que segue eh referenciado com pouca frequencia.

// Ponteiros para as streams do fluxo padrão
// stdin, stdout, stderr.
// For now, all the processes are using the same 
// standard stream. The same three files.

    unsigned long standard_streams[3];

//
// == fs ===============
//

// #important
// This is gonna help us to navigate in the levels of the pathname.
// See: get_dir() on namei.c on linux 0.01

// #todo
// NOT initialized yet!

// #todo #test
// Colocando um pouco de simetria nos simbolos.

// absolute pathname and relative pathname. 

    file *file_root;
    file *file_cwd;
    struct inode_d *inode_root;
    struct inode_d *inode_cwd;

// #todo #bugbug
// Size ?? 
// Esse tamanho deve ser igual ao 
// encontrado no modulo /fs.
 
    char root_string[32];
    char  cwd_string[32];

// Counting th e read/write operations.
// Operatinons using fd.
// #warning
// The goal here is counting how many timer the operation was called.
    unsigned int read_counter;
    unsigned int write_counter;

// ===================================================

// Banco de dados
// #??
// This is a work in progress.
// Memory management.
// Obs: 
// Um processo � um cliente de banco de dados.
// Um processo tem contas conjuntas e pessoais.
// Um processo poder� compartilhar esses objetos.

// BANCOS:
// Acesso ao banco de dados do kernel. (n�o � uma lista).
    //struct bank_d *kdb;
//Lista de acessos � bancos de contas conjuntas.
    //struct bank_d *gdbListHead;
//Lista de acessos � bancos de contas pessoais.
    //struct bank_d *ldbListHead;

//
// CONTAS
//

//Lista de contas conjuntas que o processo tem.
    //struct aspace_d *aspaceSharedListHead;
//Lista de contas pessoais que o processo tem.
    //struct aspace_d *aspacePersonalListHead;
//Lista de contas conjuntas que o processo tem.
    //struct dspace_d *dspaceSharedListHead;
//Lista de contas pessoais que o processo tem.
    //struct dspace_d *dspacePersonalListHead;

//
// Frame pool
//

// Process Page Table. (PPT)
// Quais s�o as p�ginas que o processo est� usando e onde elas est�o.
// na tabela tem que ter: O n�mero da p�gina. onde ela est�, se no disco ou se na mem�ria
// qual � o n�mero do pageframe da p�gina e qual o status de modifica��o
// Bom, acho que pra isso, o processo deve trabalhar em um n�mero limitado de pagetables
// pois o n�mero do frame deve ser o �ndice da pte dentro da page table.
// Uma pagetable tem 1024 entradas, garantindo um n�mero de 1024 frames, o que d� 4MB.
// Mas se um processo tiver mais que 4MB? Ent�o o diret�rio de p�ginas do processo
// ter� que ter mais de uma page table.
// Obs: 
// O kernel deve manter uma lista de frames que podem ser usados.
// na hora de criar uma pagetable pra um page directory de um processo
// � necess�rio pegar na lista de frames 1024 frames livres ou quanto for necess�rio.
// IMPORTANTE
// Uma lista de framepool. (Lista de parti��es de mem�ria f�sica).
// Cada framepool � composto de 1024 frames.
// Obs: IMPORTANTE: Quando um processo � criado, pelo menos um framepool 
// deve ser atribu�do a ele, mesmo antes de mapear os frames desse 
// framepool em alguma pagetable do page directory do processo.

// See: 
// mm/x64mm.h

    struct frame_pool_d *framepoolListHead;

// Memory usage in bytes.
// Increment when the process call an allocator.
// Essa é a memoria alocada pelo processo,
// não a memoria alocada pelo sistema para o processo.
    unsigned long allocated_memory;

// Quantidade de memória física usada pelo processo 
// que nao pode ser compartilhada
// com outros processos. (em KB).
    unsigned long private_memory_size;

// Quantidade de memória física usada pelo processo 
// que pode ser compartilhada
// com outros processos. (em KB).
    unsigned long shared_memory_size;

// Quantidade de memória usada por um processo 
// em determinado espaço de tempo.
// workset = (private + shared);
    unsigned long workingset_size;
    unsigned long workingset_peak_size;

// Qualquer pagefault deve ser registrada na estrutura do processo corrente.
// ?? n�o seria na thread ??
    unsigned long pagefaultCount;

	//...

    // ticks
    // jiffies.
    // Soma dos ticks de todas as suas threads.
    //unsigned long steps;

// quantas vezes no total o dispacher atuou sobre ele.
// Essa contagem n�o deve ser feita na thread.
// Ou colocaremos aqui o total de todos os context switches das threads?
    //unsigned long ContextSwitches;   

// Importante:
// working set support.
// The working set of a process is the set of pages in the 
// virtual address space of the process that are currently 
// resident in physical memory. 
// Quantas trocas de context sofreu durante um determinado tempo de an�lise.
// Esse deve ser o mesmo tempo de an�lise usado para calcular o working set.
    //unsigned long ContextSwitchesDelta;  

//
// MEMORY SUPPORT
//

// image = Imagem do programa principal do processo.
// heap  = Heap do processo.
// stack = Stack do proceso.

// #bugbug
// Me parece que isso considera que o processo s� tem um programa.
// Como ficaria no caso do processo ter v�rios programas.
// Podemos fazer aqui s� as informa��es para o programa principal.
// Para os outros programas teremos apenas uma lista de ponteiros
// para as estruturas que gerenciam eles.

// ORDEM: 
// O que segue � referenciado durante o processo de task switch.

//
// == pml4 ==============================
//

    // The pml4 table
    unsigned long pml4_VA;
    unsigned long pml4_PA;

    // The first pdpt
    unsigned long pdpt0_VA;
    unsigned long pdpt0_PA;

    // The first pd
    unsigned long pd0_VA;
    unsigned long pd0_PA;


// Page directory information:
// ==========================
//     Todo processo deve ter seu pr�prio diret�rio.
//     As threads usam o diret�rio do processo ao qual pertencem.
//     O endere�o do diret�rio � carregado no CR3.

// Ponteiro para a estrutura do diret�rio de p�ginas do processo.
    //struct page_directory_d *page_directory;  

// #bugbug: Do not use this!
    //unsigned long DirectoryVA;
    //unsigned long DirectoryPA;

//
// Image support.
//

// #IMPORTANTE
// Com base na origem da imagem e no seu tamanho podemos
// determinar a quantidade de p�ginas que o programa principal do processo
// est� usando.
// Como o programa foi caregado pelo boot loader, ainda n�o temos a lista
// de p�ginas usadas por esse processo.
// #obs: A lista � grande. Ent�o devemos fazer apenas um ponteiro para 
// ela, ou colocarmos um ponteiro de estrutura head de uma lista encadeada.

// Base da imagem do processo.
// Tamanho da imagem do processo.
// Quantas p�ginas foram usadas por essa imagem. ImageSize/PageSize

// #todo
// Precisamos rever esses nomes e criar nomes
// que reflitam com propriedade os endereços usados pela imagem do processo.
// >> E se o processo estiver usando mais que uma imagem?
 
    unsigned long Image;        // VA?
    unsigned long ImagePA;      // PA
    unsigned long ImageSize;    // ?? in bytes ??
    //unsigned long ImageSizeInBytes;
    //unsigned long ImageSizeInKB;
    unsigned long PagesPerImage; 

// Usado durante fork().
// Essas variáveis são usadas para facilitar a rotina 
// de clonagem de processo.
// Talvez isso seja provisório.

    unsigned long childImage; 
    unsigned long childImage_PA; 
    unsigned long childStack; 
    unsigned long childStackPA; 


// #todo: 
// Estrutura com informações sobre a imagem do processo.
// See: image.h
// #bugbug
// O problema é que tem processo que tem mais de uma imagem.
// Como é caso do processo kernel, que tem KERNEL.BIN e GWSSRV.BIN.
// #obs: A segunda imagem do kernel é considerada um módulo.

    struct image_info_d  *image_info;

//#test
    //struct page_control_t *page_list_head;

// =================================
// Heap
// #important
// Isso é endereço virtual, 
// usado o espaço de endereçamento do kernel.

    unsigned long HeapStart;      // Início do heap.
    unsigned long HeapEnd;        // Fim do heap.
    unsigned long HeapSize;       // Tamanho do heap.
    unsigned long HeapPointer;    // Ponteiro do heap do processo para o pr�xima aloca��o.
    unsigned long HeapLastValid;  // ultimo ponteiro v�lido.
    unsigned long HeapLastSize;   // ultimo tamanho alocado..
    //struct heap_d *processHeap; // @todo: Usar essa estrutura.

// =================================
// Stack
// #important
// Isso é endereço virtual, 
// usado o espaço de endereçamento do kernel.
// Lembrando que a pilha é invertida. Que o início da pilha
// fica no endereço mais alto e o fim fica no endereço mais baixo.

// #bugbug
// Precisamos rever cuidadosamente os endereços usados
// pela stack dos processos.
// Lembrando que o processo kernel tem mais de uma imagem?

// The stack for the process.
// Maybe it needs to reflect only the control thread.

    unsigned long StackStart;  // Início da stack.
    unsigned long StackEnd;    // Fim da stack;
    unsigned long StackSize;   // Tamanho da stack.

// Deslocamento da stack em relação ao início do kernel.
    unsigned long StackOffset;

// Teste: 
// Blocos de memoria usados pelo processo.
// Lista de ponteiros de estruturas do tipo mmblock_d.
    //unsigned long mmblockList[32];

// ??
// Muitas informaçoes sobre a meméria 
// usada pro um processo.
    //struct process_memory_info_d *processMemoryInfo;

//
// Environment.
//

// CPL
    unsigned int cpl;

// IOPL of the task. (ring).
    unsigned int rflags_iopl; 

// Que tipo de scheduler o processo utiliza. 
// (rr, realtime ...).
    //int scheduler_type;    

// syscalls counter.
    unsigned long syscalls_counter;

// #test
// #todo
// Maybe we can use these new terms for these counters:
// But, in this system the syscalls do NOT causes context switch,
// it only goes from ring3 to ring 0. In some cases i guess we already have
// the voluntary context switch, that is the sleep/wait syscalls.
// unsigned long voluntary_context_switches;     // syscalls
// unsigned long nonvoluntary_context_switches;  // preemptions

//
// Timer
//

// Temporização da tarefa. 
    unsigned long step;             // Quantas vezes a tarefa usou o processador. 
    unsigned long quantum;          // thread quantum
    unsigned long timeout;          // Tempo em modo de espera. 
    unsigned long ticks_remaining;  // rt, quanto tempo a tarefa tem dispon�vel para ser concluida.

// As threads do processo iniciam com esse quantum.
    //unsigned long ThreadQuantum;

// QUANTUM:
// Quantum é a cota atrituida � cada processo pelo
// scheduler. (time slice) 
// Esse é o tempo limite imposto pelo scheduler para
// execuçao de um processo. Ocorre ent�o uma preemp��o por tempo.
// 'ProcessingTime' é atribu�do pelo processo. � o quanto
// ele precisa. o quanto ele deseja.

//
// Working set support.
//

	//unsigned long ws_number_of_frames; 
	//...

// profiler
// quanto por cento do tempo o processo ficou rodando.
// é a soma do quanto ficou rodando todas as suas threads.

    unsigned long profiler_percentage_running;
    unsigned long profiler_ticks_running;
    unsigned long profiler_last_ticks;

//
// == Thread =================================
//

// Tipo de thread
// 0 = cpu-bound
// 1 = i/o bound.

    int bound_type;


//
// Thread support
//

    struct thread_d *control;  // The control thread.
    struct thread_d *extra;    // The extra thread.

// The thread list.
// #todo
// We need to decide what kind of list we're gonna use.

// Linked list?
    struct thread_d *threadListHead;
// Array?
    unsigned long tList[32];
    int thread_count;          // Thread counter.

// ==============

// event: 
// Tipo de evento que fazem a tarefa entrar em modo de espera. 

    //event_type_t event;

//
// == Security ============================
//

// User session, cgroup.
    struct usession_d  *usession;  // user session
    struct cgroup_d *cg;           // cgroup.        


// @todo:
// Outros:
//tempo de cpu.
//prioridade b�sica. //Obs: prioridade poderia ser uma estrutura.
//contagem de threads usadas no momento.
//path name - Endere�o completo do arquivo ex:/root/users/fred/text.bin
//comando. - Comando que abre o programa.
//Contagem de objetos sendo usado. (estatisticas ajudam melhorar o sistema.)
//i/o read count - Contagem de leituras de disco feitas pelo processo.
//i/o write count - Contagem de grava��es de disco feitas pelo processo.
//generic i/o - Outras opera��es de i/o que n�o s�o leitura e escrita em disco.
//todal de bytes lidos do disco pelo processo.
//total de bytes gravados no disco pelo processo.
//generic bytes i/o. contagem de bytes para opera��es gen�ricas de i/o, excluindo disco.

// wait4pid: 
// O processo esta esperando um processo filho fechar.
// Esse é o PID do processo que ele está esperando fechar.
    pid_t wait4pid;

// Número de processos filhos.
    int nchildren;

// Lista de processos filhos que estao no estado zumbi.
// List of terminated childs
    struct process_d  *zombieChildListHead;

//
// IPC - Inter-Process Communication.
//

// ---------------------------------

// #test
// Signal
// See: ksignal.h
    unsigned int signal;
    unsigned int umask;
    struct sigaction_d sigaction[32];   // 32 structures.
    unsigned long restorer_address;     // The restorer address.

// ---------------------------------

// ??
// Tempo para o proximo alarme, dado em ticks.
    unsigned long alarm;

// Critical section;
// Devemos checar isso quando um processo terminar,
// para que ele não fique em sua sessão crítica 
// evitando que outros possam entrar.
// #bugbug: Talvez isso deva pertencer à 
// estrutura de threads.

    int _critical;

// Quando um processo só pode receber mensagens de um 
// determinado processo. Ou de qualquer um.
// Ex: ANY=-1, PID ...
    // pid_t receive_from_pid;

// diálogo com o processo.
// importante no caso de servidores e drivers
// é como um procedimento de janelas.

    unsigned long dialog_address;


//
// == Socket ================================== 
//

// Opaque
// O socket privado do processo.
// Vamos colocar aqui a estrutura de socket.
    void *priv;
    //void *private_socket;

// list of sockets to accept connection.
    //int accept[5];
    //file *file_accept;   //aceitando através de ponteiro de arquivo.

// #todo
// Precisamos de uma lista de conexoes pendentes.
// O cliente invocou a conexao apenas uma vez
// e precisa uasr o servidor varias vezes
 
// Lista de ponteiros para estruturas de sockets.
// max =  32 cliente com conexoes pendentes.
// See: rtl/net/gdef.h
// See: rtl/net/socket.h

// error.
    //unsigned long error; 

// En exit operation is in progress.
    int exit_in_progress;
// The reson this process is exiting.
    int exit_code;

    struct process_d  *prev;
    struct process_d  *next;
};

// see: process.c
extern struct process_d  *KernelProcess;  // Base kernel.
extern struct process_d  *InitProcess;    // Init process.

// Max number of processes.
#define  PROCESS_COUNT_MAX  1024 
// Process table.
extern unsigned long processList[PROCESS_COUNT_MAX];

// ----------------------------------------------------------

// Invalidate pml4.
// Linux style. 

#define invalidate() \
    asm ("movq %%rax, %%cr3"::"a" (0))


//
// == Prototypes =====================================================
//

int destroy_process_structure(struct process_d *process);
int gc_process_structure(struct process_d *process);

struct process_d *get_kernel_process(void);
struct process_d *get_init_process(void);

void close_all_threads_of_this_process(struct process_d *process);

void close_all_processes(void);

void set_current_process(pid_t pid);
pid_t get_current_process(void);

pid_t get_current_pid(void);
struct process_d *get_current_process_pointer(void);


//==============
// plib.c
void show_currentprocess_info (void);
void show_process_information (void);

unsigned long get_process_stats ( pid_t pid, int index );

int getprocessname ( pid_t pid, char *buffer );

pid_t getNewPID (void);
int processTesting (pid_t pid);
int processSendSignal (struct process_d *p, unsigned long signal);

// ===
  
unsigned long GetProcessPML4_PA(struct process_d *process);
unsigned long GetProcessPML4_VA(struct process_d *process);

unsigned long GetProcessHeapStart(pid_t pid);

void 
SetProcessPML4_VA ( 
    struct process_d *process, 
    unsigned long va );

void 
SetProcessPML4_PA ( 
    struct process_d *process, 
    unsigned long pa );

pid_t get_caller_process_id (void);
void set_caller_process_id (pid_t pid);

// Critical section
#define __GATE_CLOSED    0
#define __GATE_OPEN      1
void process_close_gate(int pid);   //227 close
void process_open_gate(int pid);    //228 open

file *process_get_file_from_pid ( pid_t pid, int fd );
file *process_get_file (int fd);

int process_get_tty (pid_t pid);

int alloc_memory_for_image_and_stack(struct process_d *process);

// Worker for create_process.
void ps_initialize_process_common_elements(struct process_d *p);

struct process_d *processObject (void);

//
// $
// CREATE OBJECT
//

// Create and initialize a process structure.
struct process_d *create_and_initialize_process_object(void);

//
// $
// CREATE PROCESS
//

struct process_d *create_process ( 
    struct cgroup_d *cg,
    unsigned long base_address, 
    unsigned long priority, 
    ppid_t ppid, 
    const char *name, 
    unsigned int cpl,
    unsigned long pml4_va,
    unsigned long pdpt0_va,
    unsigned long pd0_va,
    int personality );

//
// $
// INITIALIZATION
//

void init_processes (void);

#endif    


