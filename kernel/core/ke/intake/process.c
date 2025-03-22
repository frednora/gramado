// process.c
// Created by Fred Nora.

#include <kernel.h>

// See: kpid.h
pid_t __gpidBoot=0;
pid_t __gpidInclude=0;
pid_t __gpidInit=0;
pid_t __gpidInstall=0;
pid_t __gpidLogoff=0;
pid_t __gpidLogon=0;
pid_t __gpidNetServer=0;
pid_t __gpidNotificationServer=0;
pid_t __gpidSCI=0;
pid_t __gpidSecurityServer=0;
pid_t __gpidSysIO=0;
pid_t __gpidSysLib=0;
pid_t __gpidSysMK=0;
pid_t __gpidSysSM=0;
pid_t __gpidWindowManager=0;
pid_t __gpidWindowServer=0;

// GLOBAL
// ------
// Process used by the console during the job control.
// #hackhack: 
// For now we are using the control thread associated with the
// window with focus.
// #todo
// But we need to use the control thread of the foreground process
// associated with the console TTY.
pid_t foreground_process=0;

// [Processing time]
// Only these can read the keyboard input.
// Sometime it's the terminal.
// It's child will read into a file.
// See: ps/process.c
pid_t criticalsection_pid=0;
// ------

//
// current pid
//

// PRIVATE
static pid_t __current_pid = (pid_t) (-1);  //fail

static pid_t caller_process_id=0;


//
// IPC
//

// Global spin lock.
// The process does not block, it spins in a loop.
// see: gspin.h
int __spinlock_ipc=0;
//...

struct process_d  *KernelProcess;  // Base kernel.
struct process_d  *InitProcess;    // Init process.

//==============================================

// #todo
// This is a work in progress.
// + Block all processes in the list but not the KernelProcess.
// + Invalidate all the strutures.
// ...
void close_all_processes(void)
{
    struct process_d  *p;
    int i=0;

    if ( (void *) KernelProcess == NULL ){
        panic ("close_all_processes: KernelProcess\n");
    }

    for ( i=0; 
          i <= PROCESS_COUNT_MAX; 
          i++ )
    {
        p = (void *) processList[i];
        
        if (p != KernelProcess )
        {
            p->state = PROCESS_BLOCKED;
            p->used = FALSE;
            p->magic = 0;
            processList[i] = (unsigned long) 0;
        }
    };
}

void set_current_process(pid_t pid)
{
    __current_pid = (pid_t) pid;
}

pid_t get_current_process(void)
{
    return (pid_t) __current_pid;
}

pid_t get_current_pid(void)
{
    return (pid_t) get_current_process();
}

// Return the pointer for a valid current process.
struct process_d *get_current_process_pointer(void)
{
    struct process_d *p;

// so podemos chamar essa rotina depois que o kernel lançou
// o primeiro thread.
    if (system_state != SYSTEM_RUNNING){
        //panic("get_current_process_pointer: system_state\n");
        return NULL;
    }
    
    pid_t __pid = (pid_t) get_current_process();
    if ( __pid < 0 || __pid >= PROCESS_COUNT_MAX )
    {
        return NULL;
    }
    p = (struct process_d *) processList[__pid];
    if (p->used!=TRUE){
        //panic ("get_current_process_pointer: used\n");
        return NULL;
    }
    if (p->magic!=1234){
        //panic ("get_current_process_pointer: magic\n");
        return NULL;
    }

    return (struct process_d *) p;
}


// #todo: 
// Observar metodos melhores para distribuir as informações.
// Unix-like coloca as informações emum arquivo para ser lido
// pelos aplicativos.
unsigned long get_process_stats(pid_t pid, int index)
{
    struct process_d *p;

    if (pid<0 || pid >= PROCESS_COUNT_MAX){
        panic ("get_process_stats: pid \n");
    }

    p = (void *) processList[pid];
    if ( (void *) p == NULL ){
        return 0;
    } 
    if (p->magic!=1234){
        return 0;
    } 

    switch (index){

        case 1:  return (unsigned long) p->pid;  break; 
        case 2:  return (unsigned long) p->ppid;  break; 
        case 3:  return (unsigned long) p->uid;  break; 
        case 4:  return (unsigned long) p->gid;  break; 
        case 5:  return (unsigned long) p->state;  break; 
        case 6:  return (unsigned long) p->plane;  break; 
        case 7:  return (unsigned long) p->input_type;  break; 
        case 8:  return (unsigned long) p->personality;  break; 
        case 9:  return (unsigned long) p->appMode;  break; 

        case 10:  
            return (unsigned long) p->private_memory_size;
            break;
        case 11:
            return (unsigned long) p->shared_memory_size;
            break;
        case 12:
            return (unsigned long) p->workingset_size;
            break;
        case 13:
            return (unsigned long) p->workingset_peak_size;
            break;
        case 14:
            return (unsigned long) p->pagefaultCount;
            break;          

        //case 15:  return (unsigned long) p->DirectoryPA;  break;
        //case 16:  return (unsigned long) p->DirectoryVA;  break;
        
        case 17:  return (unsigned long) p->Image;  break;
        case 18:  return (unsigned long) p->ImagePA;  break;
        case 19:  return (unsigned long) p->childImage;  break;
        case 20:  return (unsigned long) p->childImage_PA;  break;

        case 21:  return (unsigned long) p->HeapStart;  break;
        case 22:  return (unsigned long) p->HeapEnd;    break;
        case 23:  return (unsigned long) p->HeapSize;   break;

        case 24:  return (unsigned long) p->HeapPointer;  break;
        case 25:  return (unsigned long) p->HeapLastValid;  break;
        case 26:  return (unsigned long) p->HeapLastSize;  break;

        case 27:  return (unsigned long) p->StackStart;  break;
        case 28:  return (unsigned long) p->StackEnd;    break;
        case 29:  return (unsigned long) p->StackSize;   break;

        case 30:  return (unsigned long) p->StackOffset;  break;
        
        case 31:
            return (unsigned long) (p->rflags_iopl & 0xF);
            break;

        case 32:  return (unsigned long) p->base_priority;  break;
        case 33:  return (unsigned long) p->priority;  break;
        case 34:  return (unsigned long) p->step;  break;
        case 35:  return (unsigned long) p->quantum;  break;
        case 36:  return (unsigned long) p->timeout;  break;
        case 37:  return (unsigned long) p->ticks_remaining;  break;
        
        case 38:  
            return (unsigned long) p->profiler_percentage_running;
            break;

        case 39:
            return (unsigned long) p->profiler_ticks_running;
            break;

        case 40:
            return (unsigned long) p->profiler_last_ticks;
            break;

        case 41:  return (unsigned long) p->thread_count;  break;
        case 42:  return (unsigned long) p->bound_type;  break;
        case 43:  return (unsigned long) p->preempted;  break;
        case 44:  return (unsigned long) p->saved;  break;
        case 45:  return (unsigned long) p->PreviousMode;  break;
        case 46:  return (unsigned long) p->wait4pid;  break;
        case 47:  return (unsigned long) p->exit_code;  break;
        case 48:  return (unsigned long) p->signal;  break;
        case 49:  return (unsigned long) p->umask;  break;
        case 50:  return (unsigned long) p->dialog_address; break;
        case 51:  return (unsigned long) p->ImageSize;  break;
           
        // #todo:
        // Precisamos da quantidade de p�ginas usadas.
    
        // ...
    };

    return 0;
}


// Systemcall 882.
// OUT: string len.
// type: ssize_t ?
int getprocessname ( pid_t pid, char *buffer )
{
    struct process_d  *p;
    char *name_buffer = (char *) buffer;

// #todo
// checar validade dos argumentos.

    if (pid<0 || pid >= PROCESS_COUNT_MAX){
        goto fail;
    }

    if ( (void*) buffer == NULL ){
        goto fail;
    }
 
    p = (struct process_d *) processList[pid]; 
    if ( (void *) p == NULL ){
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        goto fail;
    }

// 64 bytes
// #todo #bugbug: 
// Check the lenght and use another copy function.
    strcpy ( 
        name_buffer, 
        (const char *) p->__processname );  

// Return the len.
// #bugbug: 
// Provavelmente isso ainda nem foi calculado.

    return (int) p->processName_len;

fail:
    return (int) (-1);
}


/*
 * getNewPID:
 *     Pegar um slot vazio na lista de processos.
 *     +Isso pode ser usado para clonar um processo.
 */
// Começaremos a busca onde começa o range de IDs 
// de processos de usuário.
// Se encontramos um slot vazio, retornaremos o índice.
 
pid_t getNewPID (void)
{

// See:
// gpid.h

    // GRAMADO_PID_BASE = GRAMADO_PID_KERNEL = 0.

    int i = GRAMADO_PID_BASE;
    //register int i=10;

    struct process_d *p;

    while (i < PROCESS_COUNT_MAX){
        p = (struct process_d *) processList[i];
        if ( (void *) p == NULL ){ 
            // return the new pid.
            return (pid_t) i; 
        }
        i++;
    };

    debug_print ("getNewPID: fail\n");

    return (pid_t) (-1);
}


/*
 * processTesting:
 *     Testando se o processo � v�lido. Se for v�lido retorna 1234.
 *     @todo: repensar os valores de retorno. 
 * system call (servi�o 88.)
 */
// #todo: Change the type to 'pid_t'.
int processTesting (int pid)
{
    struct process_d  *P;
    P = (void *) processList[pid];
    if ( (void *) P == NULL ){
        return 0;
    }else{
        if ( P->used == TRUE && P->magic == 1234 ){
            return (int) 1234; 
        }
    };
    return 0;
}

/*
 * processSendSignal:
 *     Envia um sinal para um processo.
 *     Se o sinal e o processo forem v�lidos, um sinal � colocado
 * no PCB do processo.
 *     @todo: Rotinas envolvendo sinais devem ir para outro arquivo.
 */

int processSendSignal (struct process_d *p, unsigned long signal)
{
	//SIGNAL_COUNT_MAX

	//Limit
    //if(signal >= 32){
	//	return 1;
	//}

    if (signal == 0){
        return 1;
    }

	//struct fail
	//if( (void*) p == NULL ){
	//	return 1;
	//}		
	
//ok:	
    //Ok
    if ( (void*) p != NULL )
    {
        p->signal = (unsigned long) signal;
        return 0; //(int) signalSend(p,signal);
    }

	//...

//fail:
    return 1;
}

unsigned long GetProcessPML4_PA(struct process_d *process)
{
    if ((void *) process != NULL)
    {
        //@todo: checar used e magic.
        return (unsigned long) process->pml4_PA;
    }
// fail
    return (unsigned long) 0;
}

unsigned long GetProcessPML4_VA (struct process_d *process)
{
    if( (void *) process != NULL )
    {
        //@todo: checar used e magic.
        return (unsigned long) process->pml4_VA;
    }
// fail
    return (unsigned long) 0;
}

// VA, I guess.
unsigned long GetProcessHeapStart (pid_t pid)
{
    struct process_d  *process;

// #debug
    debug_print ("GetProcessHeapStart:\n");
    //printk ("GetProcessHeapStart: [DEBUG] pid %d\n", pid);
    //refresh_screen();

// pid.
    if ( pid < GRAMADO_PID_BASE || 
         pid >= PROCESS_COUNT_MAX )
    {
        debug_print ("pid\n");
        goto fail; 
    }

// process structure.
    process = (struct process_d *) processList[pid];
    if ( (void *) process == NULL ){
        debug_print ("process\n");
        goto fail;
    }
   if ( process->used != TRUE || process->magic != 1234 )
   {
       debug_print ("process validation\n");
       goto fail;
   }
// OUT: 
// The start address of the heap of a process.
    return (unsigned long) process->HeapStart;
fail:
    debug_print ("GetProcessHeapStart: fail\n");
    panic       ("GetProcessHeapStart: fail\n");
    return (unsigned long) 0;
}

void 
SetProcessPML4_VA ( 
    struct process_d *process, 
    unsigned long va )
{
    if ((void *) process != NULL){
        process->pml4_VA = (unsigned long) va;  
    }
}

void 
SetProcessPML4_PA ( 
    struct process_d *process, 
    unsigned long pa )
{
    if ( (void *) process != NULL ){
        process->pml4_PA = (unsigned long) pa;  
    }
}

int get_caller_process_id (void)
{
    return (int) caller_process_id;
}

//#todo: use 'pid_t'.
void set_caller_process_id (int pid)
{
    caller_process_id = (int) pid;
}

// Service 227
// Entering critical section.
// Close gate. Turn it FALSE.
// #todo: use 'pid_t'.

void process_close_gate(int pid)
{
    struct process_d  *p;

// #todo: max limit
    if (pid<0){
        panic ("process_close_gate: pid \n");
    }

// Process

    p = (void *) processList[pid];

    if ( (void *) p == NULL ){
        panic ("process_close_gate: p \n");
    } else {

        // todo: validation
        
        __spinlock_ipc = __GATE_CLOSED;  //0;
        criticalsection_pid = (pid_t) 0;
        p->_critical = FALSE;  //0;
    };
}


// Service 228
// Exiting critical section
// Open gate. Turn it TRUE.
void process_open_gate (int pid)
{
    struct process_d  *p;

// #todo
// max limit

    if (pid<0){
        panic ("process_open_gate: pid \n");
    }

// Process.

    p = (void *) processList[pid];

    if ( (void *) p == NULL ){
        panic ("process_open_gate: p \n");
    } else {

        // todo: validation
        
        __spinlock_ipc = __GATE_OPEN; //1;
        criticalsection_pid = (pid_t) pid;
        p->_critical = TRUE; //1;
    };
}

//=============

// Pega uma stream na lista de arquivos dado o fd.

file *process_get_file_from_pid ( pid_t pid, int fd )
{
    struct process_d *p;
    //file *fp;

// #todo: max limit

    if (pid < 0){
        return NULL;
    }

// Get process pointer.
    p = (struct process_d *) processList[pid];

    //#todo: Check process validation.

// #todo: max limit

    if (fd<0){
        return NULL;
    }

// Get fp from list of open files.
// #bugbug: Overflow.
    
    return (file *) p->Objects[fd];  
}


// Return the file pointer from a given fd.
// the fd represents a index in the object list of the
// current process.
//#todo: IN: pid, fd
file *process_get_file (int fd)
{
    pid_t current_process = (pid_t) get_current_process();

// #todo: max limit
    if( fd<0){
        return NULL;
    }

    return (file *) process_get_file_from_pid (current_process, fd );
}


// Get tty id.
// Pega o número da tty de um processo, dado o pid.
// Serviço: 266.

int process_get_tty (int pid)
{
    // Usada para debug.

    struct process_d *p;
    struct tty_d *tty;

    if ( pid < 0 || pid >= PROCESS_COUNT_MAX )
    {
        return (int) (-EINVAL);
    }

    p = (struct process_d *) processList[pid];
    if ((void *) p == NULL)
    {
        debug_print ("process_get_tty: p \n");
        //printk ("p fail\n");
        //refresh_screen();
        return -1;
    }

// Get the private tty.
    tty = p->tty;
    if ((void *) tty == NULL){
        debug_print ("process_get_tty: tty fail\n");
        //printk ("tty fail\n");
        //refresh_screen();
        return -1;
    }

    //printk ("tty %d belongs to %d\n", tty->index, p->pid );
    //refresh_screen ();

// #bugbug
// Isso precisa ser o fd na lista de arquivos abertos 
// pelo processo.

    //file *f;
    
    //f = ()

    return (int) tty->index;
}

/*
 * alloc_memory_for_image_and_stack:
 *     Copia a imagem de um processo.
 *     Isso é usado na implementação de fork() e
 * na implementação da rotina de clonagem.
 *     Isso é usado por clone_and_execute_process()
 */

// O que copiar?
// >> code, data, bss, heap and stack.
// For now, all the processes has 4MB,
// and the stack begins at CONTROLTHREAD_STACK.
// We just use the control thread.
// #bugbug
// Imagem com limite de 200KB. (fail)
// heap ?? Depois              (fail)
// Stack de 32KB.              (ok)
// Explicando:
// Copia a imagem do processo atual e salva o endereço
// da copia num elemento da estrutura passada como argumento.
// OUT:
// #bugbug: 
// Na verdade não estamos mais copiando e 
// sim criando um endereçamento novo.

int alloc_memory_for_image_and_stack(struct process_d *process)
{
// CAlled by clone_process() in clone.c
// #bugbug: Limit 400KB.

    unsigned long __new_base=0;   // Image base.
    unsigned long __new_stack=0;  // App stack.

    if ((void *) process == NULL){
        panic("alloc_memory_for_image_and_stack: process\n");
    }
    if (process->magic != 1234)
        panic("alloc_memory_for_image_and_stack: process validation\n");

// ==================================================

//
// Image base
//

// #bugbug
// Precisamos de memória física para a imagem e para a pilha.
// 4mb de memória física nos permite criarmos um processo
// com sua pilha no topo dos 4mb.
// Por isso que precisamos de um alocador de frames,
// que considere a memória ram inteira.
// E precisamos de uma rotina que mapeie esses frames individualmente,
// mesmos que eles sejam pegos esparçamente.
// #bugbug
// Esse alocador abaixo está limitado à uma região de 4MB,
// previamente mapeado.
// #obs:
// A não ser que a pilha possa ficar em endereço
// virtual aleatório.
// Me parece que endereço virtual aleatório é
// usado por questão de segurança.
// Podemos tentar usar pilha com endereço virtual aleatório.
// 200 KB.   200kb/4096 =  quantidade de páginas.
// Allocating memory for the process's image.
// #todo: We need this size.
// 1024*200 = 200k
// 50 páginas.
// Retorna um endereço virtual.
// Mas usaremos apenas o endereço físico extraído desse endereço.

/*
     #bugbug
     Os aplicativos estão ficando grandes e a falta de
     memória pra a criação deles esta causando problemas.
     Estamos usando 300KB de memória para a criação do processo
     pois 200KB já não é o bastante e causa falha na inicialização
     do processo.
     Lembrando que precisamos de memória para a imagem do processo
     e para sua pilha.
     Os aplicativos estão com tamanhos que variam de 100KB à 200KB.
*/

// ==================================================
// Image

// Duas tentativas:
// Se o slab allocator se esgotar, então usaremos
// o allocador de páginas.
// O slab allocator nos dar 1MB e o alocador d páginas
// nos dara quantas páginas pedirmos. Mas ele é muito limitado ainda.
// see: gramado/config.h

    int imagesize_in_kb = IMAGESIZE_LIMIT_IN_KB; //400;
    int number_of_pages_on_image=0;
    number_of_pages_on_image = (int) (imagesize_in_kb*1024)/4096;     // 

    __new_base = (unsigned long) slab_1MB_allocator();
    
// Se o slab se esgotou, então tenta o alocador normal.
    if (__new_base == 0)
    {
        __new_base = 
            (unsigned long) mmAllocPages(number_of_pages_on_image); 

        // #todo
        // Here we can clean 1MB.
    }
// Check!
    if (__new_base == 0){
        panic ("alloc_memory_for_image_and_stack: __new_base\n");
    }

// #warning:
// Dangerous
// Clear only 400KB
    memset( (void*) __new_base, 0, (imagesize_in_kb*1024) );

// ==================================================

// ==================================================
// Stack

// 32 KB.
// Allocating memory for the process's stack.
// #todo: We need this size.
// Retorna um endereço virtual.
// Mas usaremos apenas o endereço físico extraído desse endereço.
// 32 KB
// Quantas páginas temos em 32KB?

    int number_of_pages_on_stack=0;
    number_of_pages_on_stack = (int) (32*1024)/4096;
    __new_stack = 
        (unsigned long) mmAllocPages(number_of_pages_on_stack); 
    if (__new_stack == 0){
        panic("alloc_memory_for_image_and_stack: __new_stack\n");
    }

    // Clear th 32 KB.
    memset (__new_stack, 0, (32*1024));

// ==================================================

//
// == Copying memory ==========
//

// #todo
// Faremos isso somente se a flag indicar 
// que queremos realizar um fork()

    //if ( clone_flags & DO_FORK ){ ... }

// Copying base and stack.
// Copiando do processo atual para o buffer que alocamos
// logo acima.

// #bugbug: So precisamos copiar 
// se tivermos fazendo uma rotina de fork()
// que pessa pra copiar. A clonagem nao copia.
// Copia a imagem do processo.
// Copia do início da imagem. 200KB.

    //if( ... ){
    memcpy ( 
        (void *) __new_base,  
        (const void *) CONTROLTHREAD_ENTRYPOINT, 
        (imagesize_in_kb*1024) );
    //}

// Copia a pilha do process.
// Copia do fim da stack. 32KB.
    memcpy ( 
        (void *) __new_stack, 
        (const void *) ( CONTROLTHREAD_STACK-(32*1024) ), 
        (32*1024) );

    //memcpy ( 
        //(void *) __new_stack, 
        //(const void *) ( CONTROLTHREAD_STACK-(128*1024) ), 
        //(128*1024) );

//
// Getting the physical addresses.
//

// Obtendo o edereço físico da base da imagem e da pilha.

    unsigned long new_base_PA  = 
        (unsigned long) virtual_to_physical( __new_base, gKernelPML4Address ); 

    unsigned long new_stack_PA = 
        (unsigned long) virtual_to_physical( __new_stack, gKernelPML4Address ); 

    if (new_base_PA == 0)
    {
        //printk("processCopyMemory: new_base_PA\n");
        //refresh_screen();

        panic("alloc_memory_for_image_and_stack: new_base_PA\n");
        return -1;
    }
    if (new_stack_PA == 0)
    {
        //printk("processCopyMemory: new_stack_PA\n");
        //refresh_screen();

        printk("alloc_memory_for_image_and_stack: new_stack_PA\n");
        return -1;
    }

    // #debug
    //refresh_screen();
    //while(1){}

// #todo
// Agora temos que fazer esses endereços físicos serem
// mapeados em 0x400000 do diretório de páginas do processo filho.
// Lembrando que o diretório de páginas do processo filho
// será uma cópia do diretório do processo pai.
// Como a cópia do diretórios anda não foi feita,
// vamos salvar esses endereços para mapearmos depois.

// virtual
// Esse endereço virtual não nos server mais.
// precisamos substituir pelo endereço virtual padrão 
// para aplicativos. Faremos isso em clone.c quando retornarmos.

// Salvando os endereços virtuais onde 
// carregamos a imagem e a pilha.
    process->childImage = (unsigned long) __new_base;
    process->childStack = (unsigned long) __new_stack;

// Salvando endereços físicos obtidos anteriormente.  
// Esses endereços representam a base da imagem e o inicio da pilha.
    process->childImage_PA = (unsigned long) new_base_PA;
    process->childStackPA  = (unsigned long) new_stack_PA;

// #debug
// Showing the addresses of base and stack pointers.

    //printk("processCopyMemory: new base PA %x | new stack PA %x \n",
        //new_base_PA, new_stack_PA );

// Done.
    //#debug
    //printk ("processCopyMemory: ok\n");
    //refresh_screen ();
    
    return 0;
}

// Worker for create_process.
// Do not check parameters validation.
void ps_initialize_process_common_elements(struct process_d *p)
{
// Called by create_process().

    register int i=0;

    //if( (void*) p == NULL )
        //return;

    p->objectType = ObjectTypeProcess;
    p->objectClass = ObjectClassKernelObject;

    p->signal = 0;
    p->umask = 0;

// --------------
// UID
    p->uid = (uid_t) GetCurrentUserId(); 
    p->ruid = (uid_t) p->uid;  // Real
    p->euid = (uid_t) p->uid;  // Effective
    p->suid = (uid_t) p->uid;  // Saved

// --------------
// UID
    p->gid = (gid_t) GetCurrentGroupId(); 
    p->rgid = (gid_t) p->gid; // Real
    p->egid = (gid_t) p->gid; // Effective
    p->sgid = (gid_t) p->gid; // Saved

    p->syscalls_counter = 0;

//
// Threads
//

// The control thread.
    p->control = NULL;
// List of threads.
    p->threadListHead = NULL;
// Absolute pathname and relative pathname.
    p->file_root = (file *) 0;
    p->file_cwd  = (file *) 0;
    p->inode_root = (struct inode_d *) 0;
    p->inode_cwd  = (struct inode_d *) 0;

// Memory usage in bytes.
// Increment when the process call an allocator.
    p->allocated_memory = 0;
    p->private_memory_size=0;
    p->shared_memory_size=0;
    p->workingset_size=0;
    p->workingset_peak_size=0;

// wait4pid: 
// O processo esta esperando um processo filho fechar.
// Esse � o PID do processo que ele est� esperando fechar.
    p->wait4pid = (pid_t) 0;
// Número de processos filhos.
    p->nchildren = 0;
    p->zombieChildListHead = NULL;
    p->exit_code = 0;

// ==========
// Standard stream.
// See: 
// kstdio.c for the streams initialization.
// #todo: We need a flag.
// #todo: Melhorar esse nome.

    if (kstdio_standard_streams_initialized != TRUE ){
        panic ("ps_initialize_process_common_elements: [ERROR] Standard stream is not initialized\n");
    }
// Check standard streams.
    if ((void *) stdin == NULL){
        panic("ps_initialize_process_common_elements: stdin\n");
    }
    if ((void *) stdout == NULL){
        panic("ps_initialize_process_common_elements: stdout\n");
    }
    if ((void *) stderr == NULL){
        panic("ps_initialize_process_common_elements: stderr\n");
    }

// ---------------
// Objects[]

    for (i=0; i<NUMBER_OF_FILES; ++i){
        p->Objects[i] = (unsigned long) 0;
    };

    p->Objects[0] = (unsigned long) stdin;
    p->Objects[1] = (unsigned long) stdout;
    p->Objects[2] = (unsigned long) stderr;

    // Counters
    p->read_counter = 0;
    p->write_counter = 0;

// ==============

//
// tty support
//

//++
// IN: type, subtype.
    p->tty = (struct tty_d *) tty_create(
        TTY_TYPE_PTY,
        TTY_SUBTYPE_UNDEFINED ); 

    if ((void *) p->tty == NULL){
        panic("ps_initialize_process_common_elements: Couldn't create tty\n");
    }
    tty_start(p->tty);
//--

    // ...

    return;
}

/*
 * processObject:
 *     Cria uma estrutura do tipo processo, mas não inicializada.
 *     #todo: Criar a mesma rotina para threads e janelas.
 */
// OUT:
// Pointer to a new structure.
// NULL if it fails.

struct process_d *processObject(void)
{
    struct process_d *p;
    p = (void *) kmalloc( sizeof(struct process_d) );
    if ((void *) p == NULL){
        return NULL;
    }
    memset( p, 0, sizeof(struct process_d) );

// #todo
// Maybe we can clean up the structure
// or initialize some basic elements.
    return (struct process_d *) p;
}

//
// $
// CREATE OBJECT
//

// OUT: process struture pointer.
struct process_d *create_and_initialize_process_object(void)
{
// Called by clone_process() in clone.c
//...

    pid_t NewPID = (pid_t) (-1);  //fail
    struct process_d  *new_process;
    register int i=0;

// Process structure.
    new_process = (struct process_d *) processObject();
    if ((void *) new_process == NULL){
        debug_print("create_and_initialize_process_object: [FAIL] new_process\n");
        printk     ("create_and_initialize_process_object: [FAIL] new_process\n");
        goto fail;
    }

// Default personality
    new_process->personality = PERSONALITY_GRAMADO;
// see: layer.h
    new_process->_layer = LAYER_UNDEFINED;
// #test
// No environment yet.
    new_process->env_subsystem = UnknownSubsystem;

// Get PID.
// Obtêm um índice para um slot vazio na lista de processos.
// Precisa estar dentro do range válido para processos
// em ring3.

    // Invalidate.
    new_process->pid = -1;

    new_process->_is_terminal = FALSE;
    new_process->_is_child_of_terminal = FALSE;

// Get new pid.
// #:: We have a valid range here!
// #todo: Change to generate_new_pid();

    NewPID = (pid_t) getNewPID();
    
    if ( NewPID < GRAMADO_PID_BASE || 
         NewPID >= PROCESS_COUNT_MAX )
    {
        debug_print("create_and_initialize_process_object: [FAIL] NewPID\n");
        printk     ("create_and_initialize_process_object: [FAIL] NewPID={%d}\n", 
            NewPID );
        goto fail;
    }

// PID
// Initializing the process structure.
// Saving the process pointer in the list.
    new_process->pid = (pid_t) NewPID;  // :)

// UID
    new_process->uid = (uid_t) current_user;
    new_process->ruid = (uid_t) current_user;  // real
    new_process->euid = (uid_t) current_user;  // effective 
    new_process->suid = (uid_t) current_user;  // saved

// GID
    new_process->gid = (gid_t) current_group;
    new_process->rgid = (gid_t) current_group;  // real
    new_process->egid = (gid_t) current_group;  // effective
    new_process->sgid = (gid_t) current_group;  // saved

    new_process->syscalls_counter = 0;

// tty
//++
    new_process->tty = 
        (struct tty_d *) tty_create( TTY_TYPE_PTY, TTY_SUBTYPE_UNDEFINED );
    if ((void *) new_process->tty == NULL){
        panic ("create_and_initialize_process_object: Couldn't create TTY\n");
    }
    tty_start(new_process->tty);
//--

// #bugbug
// #todo
// Ok mesma coisa precisa ser feito para o endereço
// virtual da pilha.

// #Cuidado
// Heap for Clone.
// Essa é a rotina usada na criação de processo 
// pra gerar um heap para ele.
// Vamos tentar usar isso na rotina de clonagem.

    if (g_heappool_va == 0){
        panic("create_and_initialize_process_object: g_heappool_va\n");
    }
    if (g_heap_count == 0){
        panic("create_and_initialize_process_object: g_heap_count\n");
    }
    if (g_heap_size == 0){
        panic("create_and_initialize_process_object: g_heap_size\n");
    }

// #bugbug
// There is a limit here. End we will have a huge problem 
// when reach it.

//===========================================================

// Heap support
    new_process->HeapStart = 
        (unsigned long) (g_heappool_va + (g_heap_count * g_heap_size));
    new_process->HeapSize = 
        (unsigned long) g_heap_size;
    new_process->HeapEnd = 
        (unsigned long) (new_process->HeapStart + new_process->HeapSize); 
    g_heap_count++;

//#debug
    //printk ("clone_and_execute_process: HeapStart %x | HeadSize %x | HeapEnd %x \n",
    //    new_process->HeapStart, new_process->HeapSize, new_process->HeapEnd );

//#breakpoint
    //refresh_screen();
    //while(1){}

//===========================================================

// Stack support
// Stack for the clone. 
// #bugbug: The stack size?
    new_process->control->context.rsp = 
        (unsigned long) CONTROLTHREAD_STACK;
    new_process->StackStart = 
        (unsigned long) CONTROLTHREAD_STACK;
    new_process->StackSize = 
        (unsigned long) (32*1024);  
    new_process->StackEnd = 
        (unsigned long) ( new_process->StackStart - new_process->StackSize );

//#todo
//#debug: print stack info.


// #todo: Explain it better.
// 0x200000
    new_process->Image = 
        (unsigned long) CONTROLTHREAD_BASE;

    new_process->used = TRUE;
    new_process->magic = 1234;

// Save a finalized structure.
    processList[NewPID] = (unsigned long) new_process;

// OUT:
// Pointer for a structure of a new process.
    return (struct process_d *) new_process;
fail:
    return NULL;
}

//
// $
// CREATE PROCESS
//

// Create process
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
    int personality )
{
    struct process_d  *Process;
    register pid_t PID = -1;
    // Para a entrada vazia no array de processos.
    struct process_d *EmptyEntry;
    unsigned long BasePriority=0;
    unsigned long Priority=0;
    int Personality = personality;

    debug_print ("create_process:\n");
    printk      ("create_process:\n");

//=================================
// check parameters

    // cgroup
    if ((void*) cg == NULL){
        //debug_print ("create_process: cg\n");
    }

// #todo
// Maybe the virtual 0 is n option in the future. Maybe.

    if (base_address == 0){
        panic ("create_process: base_address\n");
    }
    if (ppid < 0){
        panic ("create_process: ppid\n");
    }
    if ((void*) name == NULL){
        panic ("create_process: name\n");
    }
    if ( *name == 0 ){
        panic ("create_process: *name\n");
    }

//===============================
// Tables

    if (pml4_va == 0){
        panic ("create_process: pml4_va\n");
    }
    if (pdpt0_va == 0){
        panic ("create_process: pdpt0_va\n");
    }
    if (pd0_va == 0){
        panic ("create_process: pd0_va\n");
    }
    // ...

//=================================

// @todo:
// Melhorar esse esquema de numeraçao e 
// contagem de processos criados.
// processNewPID eh global ?

    //if ( processNewPID < GRAMADO_PID_BASE || 
    //     processNewPID >= PROCESS_COUNT_MAX )
    //{
    //    processNewPID = (int) GRAMADO_PID_BASE;
    //}

// Process
    Process = (void *) kmalloc( sizeof(struct process_d) );
    if ((void *) Process == NULL){
        panic("create_process: Process\n");
    }
    memset( Process, 0, sizeof(struct process_d) );

// Worker:
// Initializing the elements common for all types of processes.
    ps_initialize_process_common_elements((struct process_d *) Process);

// ====================
// get_next:
// Get empty.
// Obtem um indice para um slot vazio na lista de processos.
// Se o slot estiver ocupado tentaremos o proximo.
// Na verdade podemos usar aquela funçao que procura por um vazio. 

    while (1){
        PID = (pid_t) getNewPID();
        if ( PID < GRAMADO_PID_BASE || PID >= PROCESS_COUNT_MAX )
        {
            debug_print ("create_process: getNewPID\n");
            printk      ("create_process: getNewPID %d\n", PID);
            goto fail;
        }
        EmptyEntry = (void *) processList[PID];
        if ((void *) EmptyEntry == NULL){ 
            break;
        }
    };
// ====================

    Process->pid = (pid_t) PID; 
    Process->ppid = (pid_t) ppid;

// Name

    Process->name = (char *) name; //@todo: usar esse.
    //Process->cmd = NULL;  //nome curto que serve de comando.
    //Process->pathname = NULL; 
    //#test
    //64 bytes max.
    strcpy ( Process->__processname, (const char *) name);
    Process->processName_len = sizeof(Process->__processname);

    BasePriority = (unsigned long) priority; 
    Process->base_priority = BasePriority;

    Priority = (unsigned long) priority;
    Process->priority = Priority;

    Process->personality = (int) Personality;

// ------------
// cpl
    if (cpl != RING0 && cpl != RING3){
        panic("create_process: cpl\n");
    }
    Process->cpl = (unsigned int) cpl;

// ------------
// iopl
// Qual é o privilágio padrão?
// Weak protection!
    Process->rflags_iopl = (unsigned int) 3;  //weak protection 

    // Not a protected process!
    Process->_protected = 0;

    // sessão crítica.
    Process->_critical = 0;

// see: layer.h
    Process->_layer = LAYER_UNDEFINED;

// #todo: Via argument
     Process->plane = FOREGROUND_PROCESS;

    //Error.
    //Process->error = 0;

    Process->exit_in_progress = FALSE;

//
// Banco de dados
//
	//bancos de dados e contas do processo.
	//Process->kdb =
	//Process->gdbListHead =
	//Process->ldbListHead =
	//Process->aspaceSharedListHead =
	//Process->aspacePersonalListHead =
	//Process->dspaceSharedListHead =
	//Process->dspacePersonalListHead =

// Inicializando a lista de framepools do processo.
// @todo: Todo processo deve ser criado com pelo menos um 
// frame pool, o que � equivalente a 4MB. (uma parti��o)
// Obs: Um framepool indica onde � a �rea de mem�ria fisica
// que ser� usada para mapeamento das p�ginas usadas pelo processo.

    Process->framepoolListHead = NULL;

//Thread inicial.
    //Process->thread =

    //Process->processImageMemory =
    //Process->processHeapMemory =
    //Process->processStackMemory =

// ORDEM: 
// O que segue � referenciado durante o processo de task switch.

// Page Directory: (em 64bit agora é pml4)
//     Alocar um endere�o f�sico para o diret�rio de p�ginas do 
// processo a ser criado, depois chamar a fun��o que cria o diret�rio.
// @todo:
// IMPORTANTE: Por enquanto os processos s�o criadas usando o 
// diret�rio de p�ginas do processo Kernel. Mas temos que criar 
// um diret�rio novo pra cada processo criado.
// O diret�rio de todos os processos de usu�rio ser�o iguais. 
// Ter�o uma �rea de us�rio particular e uma �rea compartilhada 
// em kernel mode.
//@todo: Alocar um endere�o f�sico antes, depois chamar a fun��o que 
// cria o pagedirectory.
//@todo: 
//op��o: KERNEL_PAGEDIRECTORY; //@todo: Usar um pra cada processo.
// #obs:
// Vari�vel recebida via argumento.

//
// pml4_va
//

    if (pml4_va == 0)
    {
        debug_print("create_process: [FAIL] pml4_va\n");
        printk     ("create_process: [FAIL] pml4_va\n");
        goto fail;
    }

// pml4
    Process->pml4_VA = (unsigned long) pml4_va;
    Process->pml4_PA = (unsigned long) virtual_to_physical ( 
                                               pml4_va, 
                                               gKernelPML4Address );
// pdpt0
    Process->pdpt0_VA = (unsigned long) pdpt0_va;
    Process->pdpt0_PA = (unsigned long) virtual_to_physical ( 
                                               pdpt0_va, 
                                               gKernelPML4Address );
// pd0
    Process->pd0_VA = (unsigned long) pd0_va;
    Process->pd0_PA = (unsigned long) virtual_to_physical ( 
                                               pd0_va, 
                                               gKernelPML4Address );

// cancelados. 
    // Process->mmBlocks[32]
    // Process->mmblockList[32]
    // Process->processMemoryInfo

// #todo: 
// Precisa alocar espa�o na mem�ria f�sica.
// Precisa criar page tables para essas areas de cada processo.
// Os endere�os virtuais dessas areas dos processos s�o sempre os mesmos.
// mas os endere�os f�sicos dessas areas variam de processo pra processo.
// Imagem do processo.
// ?? Provavelmente esse endere�o � virtual.
// Queremos que esse endere�o seja padronizado e que todos 
// os processos usem o mesmo endere�o.
// #bugbug
// Todos os processos de usu�rio come�am no mesmo endere�o virtual.
// Por�m temos os processos em kernel mode e os processos do gramado core
// que usam endere�os virtuais diferentes.
// #todo: Rever isso.
// #todo: estamos suspendendo essa informa��o.
// # IMPORTANTE 
// Base da imagem do processo.
// Na verdade precisamos aceitar o endere�o passado via 
// argumento, pois nem todos processos come�am no endere�o 
// default.

// -----------------------------------
// Image:
// Virtual and physical address for the image.

    Process->Image   = (unsigned long) base_address;  
    Process->ImagePA = (unsigned long) virtual_to_physical ( 
                                           Process->Image, 
                                           gKernelPML4Address ); 
                                               
// -----------------------------------
// Child Image:
// Virtual and physical address for the image.
// This is used during the cloning routine.

    Process->childImage = 0;
    Process->childImage_PA = 0;

// #todo
// Precisamos saber o tamanho da imagem do processo para
// calcularmos quantas p�ginas ele vai usar.
// Precisamos dividir a imagem em code, data, heap e stack
// Pois a �rea de dados poder� sofrer swap.
// Tamanho da imagem do processo.
// Temos que chamar a fun��o que pega o tamanho de um arquivo,
// #bugbug: Porem, no momento o kernel n�o consegue ler arquivos
// que est�o em subdiret�rios corretamente e os programas est�o 
// em subdiret�rios.
// #obs: O tamanho tamb�m poderia ser passado por arguemento.
// #ou um argumento com ponteiro pra estrutura de informa��o 
// sobre uma imagem.

    Process->ImageSize = 0;

// #todo: 
// Estrutura com informa��es sobre a imagem do processo.
    
    Process->image_info = NULL;

//
// == Heap and Stack ===========
//

// #obs: 
// O 'endereço virtual' do heap e da stack dos processos serão 
// os mesmos para todos os processos, assim como o 'endereço virtual' 
// de carregamento da imagem.
// #todo:
// #bugbug: 
// O Heap e a Stack devem estar dentro da 
// área de memória do processo.
// #wrong: Uma pagetable do diretório é para o heap e outra para a stack.
// #wrong: Cada pagetable no diretório do processo é pra uma coisa.

// ## HEAP ##

// directory va, index, region pa

    //CreatePageTable ( Process->DirectoryVA, 512, 0 );

    //Process->Heap = (unsigned long) 0x00400000; //funciona
    //Process->Heap = (unsigned long) 0xC0C00000; //funciona

// g_heappool_va
// endere�o virtual do pool de heaps.
// os heaps nessa �rea ser�o dados para os processos.
// base + (n*size)

    if ( g_heap_count < 0 || 
         g_heap_count >= g_heap_count_max )
    {
        debug_print ("create_process: [FIXME] g_heap_count limits\n");
        //panic ("create_process: [FAIL] g_heap_count limits\n");
    }

// #atenção
// Estamos usando o heappool pra pegarmos esses endereços.
// me parece que isso é memória compartilhada em ring3
// e que o malloc da libc está usando isso sem problemas.
// #todo: 
// #test: A stack de um process recem criado
// poderia ficar no fim de seu heap ???

    if (g_heappool_va == 0){
        debug_print ("clone_and_execute_process: g_heappool_va\n");
        panic ("clone_and_execute_process: g_heappool_va\n");
    }

// Ignoraremos esse pois vai falhar na criacao do primeiro heap.
    //if (g_heap_count == 0)
        //panic("clone_and_execute_process: g_heap_count");

    if (g_heap_size == 0){
        debug_print ("clone_and_execute_process: g_heap_size\n");
        panic ("clone_and_execute_process: g_heap_size\n");
    }

// #bugbug
// There is a limit here. End we will have a huge problem 
// when reach it.

    Process->HeapStart = (unsigned long) g_heappool_va + (g_heap_count * g_heap_size);
    Process->HeapSize  = (unsigned long) g_heap_size;
    Process->HeapEnd   = (unsigned long) (Process->HeapStart + Process->HeapSize); 
    g_heap_count++;

// Endere�o do in�cio da Stack do processo.
// Endere�o do fim da stack do processo.
// Tamanho da pilha, dada em KB.
// #importante: 
// Deslocamento do endere�o do in�cio da pilha em rela��o 
// ao in�cio do processo. 
// #bugbug
// Isso indica que a stack será no endereço virtual tradicional,
// porém qual é o endereço físico da stack do processo criado
// com essa rotina.
// #bugbug: Com esse erro todos os processo criados
// estão usando a mesma stack, pois todas apontam para o mesmo
// endereço físico.

//
// #bugbug #bugbug #bugbug #bugbug
//

// Wrong !!!!!!!!!!!!!!!!!!!!

    Process->StackStart  = (unsigned long) UPROCESS_DEFAULT_STACK_BASE; 
    Process->StackSize   = (unsigned long) UPROCESS_DEFAULT_STACK_SIZE; //?? usamos isso na hora de criar a stack?? 
    Process->StackEnd    = (unsigned long) (Process->StackStart - Process->StackSize);  
    Process->StackOffset = (unsigned long) UPROCESS_DEFAULT_STACK_OFFSET;  //??


//
// PPL - (Process Permition Level).(gdef.h)
//

// Determina as camadas de software que um processo 
// tera acesso irrestrito.

    // Process->ppl = pplK0;

    //Process->callerq          //head of list of procs wishing to send.
    //Process->sendlink;        //link to next proc wishing to send.
    //Process->message_bufffer  //pointer to message buffer.
    //Process->getfrom_pid      //from whom does process want to receive.
    //Process->sendto_pid       //pra quem.

    //Signal
    //Process->signal = 0;
    //Process->signalMask = 0;

    //cancelada.
    //Process->process_message_queue[8]

// Que tipo de scheduler o processo utiliza. (rr, realtime ...).
    //Process->scheduler_type = ; 

// #todo
// Counters

    //Process->step
    //Process->quantum
    //Process->timeout
    //Process->ticks_remaining

// As threads do processo iniciam com esse quantum.

    //Process->ThreadQuantum   

    //Process->event

// #importante
// user session and cgroup.
// #bugbug: 
// Nao temos informaçao sobre a user session, 
// devemos pegar a estrutura de current user session. 
// Para isso ela deve ser configurada na inicializa��o do gws,
// antes da cria��o dos processo.

// Security
    Process->usession = CurrentUserSession;  // Current.
    Process->cg = (struct cgroup_d *) cg;    // Passado via argumento.
// Navigation
    Process->prev = NULL; 
    Process->next = NULL; 
// Register
// List
// Coloca o processo criado na lista de processos.
    processList[PID] = (unsigned long) Process;
// #todo
    // last_created = PID;
    Process->state = INITIALIZED;
// Validation.
    Process->used = TRUE;
    Process->magic = PROCESS_MAGIC;

    // #debug
    //debug_print ("create_process: done\n");
    //printk      ("create_process: done\n");

    // ok
    return (void *) Process;

fail:
    //Process = NULL;
    //refresh_screen();
    return NULL;
}

//
// $
// INITIALIZATION
//

/*
 * init_processes:
 *    Inicaliza o process manager.
 *    #todo: rever esse nome, pois na verdade estamos 
 * inicializando variaveis usadas no gerenciamento de processo.
 */
// Called by keInitializeIntake() in ke.c
void init_processes (void)
{
    register int i=0;

    debug_print("init_processes:\n");

// Globals

// O que fazer com a tarefa atual.
    kernel_request = 0;

// ?? Contagem de tempo de execu��o da tarefa atual.
// n�o precisa, isso � atualizado pelo request()
    //kernel_tick = 0;

    set_current_process(0);
    //current_process = 0;

// Clear process list.

    i=0;
    while (i < PROCESS_COUNT_MAX){
        processList[i] = (unsigned long) 0;
        i++;
    };

    // More ?
}

