// x64init.c
// Created by Fred Nora.

#include <kernel.h>

// Task switching support.
extern void turn_task_switch_on (void);
extern void x64_clear_nt_flag (void);

// private
static int InitialProcessInitialized = FALSE;


//Onde ficam os códigos e arquivos de configuração usados na inicialização.
//A ideia é que se a inicialização precisar de algum arquivo, deve procurá-lo
//nos diretórios previamente combinados.
//melhor que sejam poucos e que sempre sejam os mesmos.

#define INIT_ROOT_PATH  "/"
#define INIT_BIN_PATH   "/BIN"
//...

// The default name for the init process
const char *kernel_process_default_name = "KERNEL-PROCESS";

// -------------------
// Ring0 Module.
// The name of the kernel of the root partition.
// see: gramp/oskernel/

//const char *mod_image_name = "MOD0    BIN";
const char *mod_image_name = "HVMOD0  BIN";

// -------------------
// Image name for the init process. INIT.BIN
const char *init_image_name = "INIT    BIN";
// The default name for the init process
const char *init_process_default_name = "INIT-PROCESS";

// =========================================

//
// == Private functions: Prototypes ========
//

static int __setup_stdin_cmdline(void);

static int I_initKernelComponents(void);

static int I_x64CreateKernelProcess(void);
static int __load_mod_image(void);
static int I_x64CreateInitialProcess(void);
static int __load_initbin_image(void);

static int I_x64CreateTID0(void);

// =========================================

/*
// #deprecated
// local
// Call a mm routine for that.
void x64init_load_pml4_table(unsigned long phy_addr)
{
    asm volatile ("movq %0,%%cr3"::"r"(phy_addr));
}
*/

// Setup stdin file.
// Clean the buffer.
// Set the offset position.
// See: kstdio.c
static int __setup_stdin_cmdline(void)
{
    char cmdline[64];

// The command line for init.bin.
// --hl = Headless mode. No cmdline interpreter.
// --cmd = Run the cmdline interpreter.
// --server = Run embedded server.
// --reboot = Reboot the machine.
// --shutdown = Shutdown the machine.

    const char *initbin_cmdline1 = "INIT.BIN: --cmd --server";
    //const char *initbin_cmdline2 = "INIT.BIN: --cmd --server";
    const char *initbin_cmdline3 = "INIT.BIN: --hl";
    //const char *initbin_cmdline4 = "INIT.BIN: --reboot";
    //const char *initbin_cmdline5 = "INIT.BIN: --shutdown";
    //...

    memset(cmdline, 0, 64);

// Write
// Put the command line string into the local buffer.

    // #debug
    //printk("k: Initializing cmdline\n"); 
    //refresh_screen();

// Run both modes, cmdline and then server mode.
// #todo: We can copy a different comand line depending on
// the runlevel number.

    switch (g_product_type)
    {
        case PT_GRAMADO_HYPERVISOR_HEADLESS:
        case PT_GRAMADO_SERVER_HEADLESS:
            crt_sprintf( cmdline, initbin_cmdline3 );
            break;
        default:
            crt_sprintf( cmdline, initbin_cmdline1 );
            break;
    };


/*
// Running only in servermode.
// Good for headless machine.
    crt_sprintf(
        cmdline, 
        "INIT.BIN: --server");
*/

    if ((void*) stdin == NULL){
        panic("__setup_stdin_cmdline: stdin\n");
    }
    if (stdin->magic != 1234){
        panic("__setup_stdin_cmdline: stdin validation\n");
    }

// Rewind
    k_fseek( stdin, 0, SEEK_SET );
// Write into the file
    file_write_buffer( stdin, cmdline, 64 );

    return 0;
}

// Local worker.
// The virtual address for the base of the image.
// ps: We are using the kernel page directories now,
// this way but we're gonna clone the kernel pages
// when creating the init process.
// #define CONTROLTHREAD_BASE        0x00200000
// See: x64gva.h
// #bugbug:
// Por que esse endereço está disponível para uso?
// Por que não precisamos alocar memoria para esse processo?
// Porque estamos usando 512 páginas?
// Talvez seja porque a gerência de memória
// reservou esse espaço de 2mb para o primeiro processo em ring3
// na hora de mapear as regiões principais da memória ram.
// #todo
// Check the information in the elf header.
// Save some of this information in the process structure. 
// see: exec_elf.h and process.h
static int __load_initbin_image(void)
{

// The virtual address for the init process image.
// #bugbug
// Estamos carregando a imagem na marca de 2MB virtual,
// que significa 32MB fisico.
// e ainda nem criamos o processo init.
// No espaço de endereçamento do kernel, virtual e fisico
// sao iguais para esse endereço.
// #bugbug
// Explain it better.
    unsigned long ImageAddress = 
        (unsigned long) CONTROLTHREAD_BASE;

// #bugbug
// We have a limit for the image size.
    unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (512 * 4096);

    int Status = -1;

// Check the validation of the name.
    if ((void*) init_image_name == NULL){
        panic("__load_initbin_image: init_image_name\n");
    }

// ---------------------
// It loads a file into the memory.
// IN:
//     fat_address  = FAT address.
//     dir_addresss = Directory address.
//     dir_entries  = Number of entries in the given directory.
//     file_name    = File name.
//     buffer = Where to load the file. The pre-allocated buffer.
//     buffer_size_in_bytes = Maximum buffer size.
// -----------------
// OUT: 
//    1 = fail 
//    0 = ok

    Status = 
        (unsigned long) fsLoadFile( 
                            VOLUME1_FAT_ADDRESS, 
                            VOLUME1_ROOTDIR_ADDRESS, 
                            FAT16_ROOT_ENTRIES,    //#bugbug: number of entries.
                            init_image_name, 
                            (unsigned long) ImageAddress,  // buffer
                            BUGBUG_IMAGE_SIZE_LIMIT );     // buffer limits

    if (Status != 0){
        printk("__load_initbin_image: on fsLoadFile()\n");
        goto fail;
    }

// OUT: 
//    1 = fail 
//    0 = ok

    return 0;
fail:
    return -1;
}

// Load INIT.BIN.
// Create a process for the first ring3 process.
// + Carrega a imagem do primeiro processo que vai rodar em user mode.
// + Configura sua estrutura de processo.
// + Configura sua estrutura de thraed.
// Não passa o comando para o processo.

static int I_x64CreateInitialProcess(void)
{
// This is a ring 3 process.
// It loads the first ring3 program, the INIT.BIN.

    pid_t InitProcessPID = -1;

// #debug
    //debug_print ("I_x64CreateInitialProcess: \n");
    //printk      ("I_x64CreateInitialProcess:\n");
    //refresh_screen();

    InitialProcessInitialized = FALSE;

    if (system_state != SYSTEM_BOOTING){
        printk ("I_x64CreateInitialProcess: system_state\n");
        return FALSE;
    }

// #todo
// Check the information in the elf header.
// Save some of this information in the process structure. 
// see: exec_elf.h and process.h
// #bugbug
// Estamos carregando a imagem na marca de 2MB virtual,
// que significa 32MB fisico.
// Estamos carregando a imagem na marca de 2MB
// e ainda nem criamos o processo init.
// No espaço de endereçamento do kernel, virtual e fisico
// sao iguais para esse endereço.

    int ret0 = -1;
    ret0 = (int) __load_initbin_image();
    if (ret0 != 0){
        printk("I_x64CreateInitialProcess: Coldn't load INIT.BIN\n");
        return FALSE;
    }

// Creating init process.
// > Cria um diretório que é clone do diretório do kernel base 
// Retornaremos o endereço virtual, para que a função create_process possa usar 
// tanto o endereço virtual quanto o físico.
// > UPROCESS_IMAGE_BASE;
// #todo
// temos que checar a validade do endereço do dir criado
// antes de passarmos..

    void *init_pml4_va = (void *) CloneKernelPML4();

    if (init_pml4_va == 0){
        printk("I_x64CreateInitialProcess: init_pml4_va\n");
        return FALSE;
    }

    init_mm_data.pml4_va = init_pml4_va;  // #todo: type.
    init_mm_data.pml4_pa = 
        (unsigned long) virtual_to_physical( 
                            init_pml4_va, 
                            gKernelPML4Address );

    if (init_mm_data.pml4_pa == 0){
        printk("I_x64CreateInitialProcess: init_mm_data.pml4_pa\n");
        return FALSE;
    }

    // ...

    init_mm_data.used = TRUE;
    init_mm_data.magic = 1234;

// ===========================================

//
// Create init process
//

// #todo
// Comment about the properties of this process.

    unsigned long BasePriority = PRIORITY_SYSTEM_THRESHOLD;
    unsigned long Priority     = PRIORITY_SYSTEM_THRESHOLD;

    InitProcess = 
        (void *) create_process( 
                     NULL,  
                     (unsigned long) CONTROLTHREAD_BASE,  //0x00200000 
                     BasePriority, 
                     (int) KernelProcess->pid, 
                     init_process_default_name, 
                     RING3, 
                     (unsigned long) init_pml4_va,
                     (unsigned long) kernel_mm_data.pdpt0_va,
                     (unsigned long) kernel_mm_data.pd0_va,
                     PERSONALITY_GRAMADO );

// validation
    if ((void *) InitProcess == NULL){
        printk("I_x64CreateInitialProcess: InitProcess\n");
        return FALSE;
    }
    if ( InitProcess->used != TRUE || 
         InitProcess->magic != 1234 )
    {
        printk("I_x64CreateInitialProcess: InitProcess validation\n");
        return FALSE;
    }

// #bugbug: 
// Who gave this pid to this process? create_process did?

    InitProcessPID = (pid_t) InitProcess->pid;
    if ( InitProcessPID != GRAMADO_PID_INIT ){
        printk ("I_x64CreateInitialProcess: InitProcessPID\n");
        return FALSE;
    }

// Let's update the GID for this process.
// It was initialized with 0.
    InitProcess->gid = (gid_t) GID_INIT;
    InitProcess->rgid = (gid_t) GID_INIT;  // real 
    InitProcess->egid = (gid_t) GID_INIT;  // effective
    InitProcess->sgid = (gid_t) GID_INIT;  // saved

// The init process is a system application.
    InitProcess->type = PROCESS_TYPE_SYSTEM;

    InitProcess->base_priority = BasePriority;    
    InitProcess->priority = Priority;

// see: layer.h
    InitProcess->_layer = LAYER_INIT;

// The init process. This is part og the gramland subsystem.
    InitProcess->env_subsystem = GramadoSubsystem;

// -----------------------------------------------
// init_mm_data

    if ( init_mm_data.used != TRUE || 
         init_mm_data.magic != 1234 )
    {
        printk ("I_x64CreateInitialProcess: init_mm_data validation\n");
        return FALSE;
    }

// -----------------------------------------------

// Esse foi configurado agora.
    InitProcess->pml4_VA = init_mm_data.pml4_va;
    InitProcess->pml4_PA = init_mm_data.pml4_pa; 

// Herdado do kernel
    InitProcess->pdpt0_VA = kernel_mm_data.pdpt0_va;
    InitProcess->pdpt0_PA = kernel_mm_data.pdpt0_pa; 

// Herdado do kernel
    InitProcess->pd0_VA = kernel_mm_data.pd0_va;
    InitProcess->pd0_PA = kernel_mm_data.pd0_pa; 

    fs_initialize_process_cwd ( InitProcessPID, "/" );

//====================================================
// Create thread

// Criamos um thread em ring3.
// O valor de eflags é 0x3200. The app is gonna change that.
// The control thread of the first ring 3 process.
// See: ithread.c
// Struct, and struct validation.

    InitThread = (struct thread_d *) create_init_thread();
    if ((void *) InitThread == NULL){
        printk ("I_x64CreateInitialProcess: InitThread\n");
        return FALSE;
    }
    if ( InitThread->used != TRUE || InitThread->magic != 1234 )
    {
        printk("I_x64CreateInitialProcess: InitThread validation\n");
        return FALSE;
    }
// Invalid TID.
// Tem que ser a primeira thread.
// INIT_TID
    if (InitThread->tid != 0){
        printk("I_x64CreateInitialProcess: InitThread->tid\n");
        return FALSE;
    }
// Invalid PID.
// owner pid
    if ( InitThread->owner_pid != GRAMADO_PID_INIT ){
        printk ("I_x64CreateInitialProcess: GRAMADO_PID_INIT\n");
        return FALSE;
    }

//
// Paging
//

// Herdando do processo configurado logo antes.
    InitThread->pml4_VA  = InitProcess->pml4_VA;
    InitThread->pml4_PA  = InitProcess->pml4_PA;
    InitThread->pdpt0_VA = InitProcess->pdpt0_VA;
    InitThread->pdpt0_PA = InitProcess->pdpt0_PA;
    InitThread->pd0_VA   = InitProcess->pd0_VA;
    InitThread->pd0_PA   = InitProcess->pd0_PA;

// #todo 
// #bugbug
    //InitThread->tss = current_tss;

// ===========================

// #todo #bugbug
//registra um dos servidores do gramado core.
    //server_index, process, thread

    //ipccore_register ( 
        //(int) 0, 
        //(struct process_d *) InitProcess, 
        //(struct thread_d *) InitThread ); 

    InitThread->pe_mode = PE_MODE_EFFICIENCY;

// ===========================

// Set the control thread for the init process.
    InitProcess->control = InitThread;

// Set the current process
    set_current_process(InitProcessPID);

// Set the current thread
    current_thread = InitThread->tid;

// Agora ja temos um processo em user mode devidamente 
// configurado. Então a rotina de inicialização em init/
// poderá pasasr o comando para ele quando quizer.

    InitialProcessInitialized = TRUE;
    return TRUE;
}

// =========================================
// ::(3)
// Passa o comando para o primeiro processo em user mode.
// Esse processo ja foi previamente configurado.
// Called by I_kmain() in kmain.c
void I_x64ExecuteInitialProcess(void)
{
    struct thread_d *t;
    register int i=0;

    // #debug
    PROGRESS("I_x64ExecuteInitialProcess:\n");
    //PROGRESS("::(3)\n");   
    //debug_print("I_x64ExecuteInitialProcess:\n");
    // For real machine.
    //printk      ("I_x64ExecuteInitialProcess: [TODO]\n");

    // #debug
    //panic("I_x64ExecuteInitialProcess: breakpoint :)");

    if (system_state != SYSTEM_BOOTING){
        panic ("I_x64ExecuteInitialProcess: system_state\n");    
    }

// Se essa rotina foi chamada antes mesmo
// do processo ter sido devidamente configurado.
    if (InitialProcessInitialized != TRUE){
        debug_print("I_x64ExecuteInitialProcess: InitialProcessInitialized\n");
        panic      ("I_x64ExecuteInitialProcess: InitialProcessInitialized\n");
    }
    //if (InitialProcessInitialized != TRUE){
    //    debug_print ("I_x64ExecuteInitialProcess: InitialProcessInitialized\n");
    //    panic       ("I_x64ExecuteInitialProcess: InitialProcessInitialized\n");
    //}

// Setup c0/  (Again)
// Change the foreground console.
    console_set_current_virtual_console(CONSOLE0);

// Setup command line for the init process.
    __setup_stdin_cmdline();

// The first thread to run will the control thread 
// of the init process. It is called InitThread.

    t = (struct thread_d *) InitThread; 

    if ((void *) t == NULL){
        panic("I_x64ExecuteInitialProcess: t\n");
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        panic("I_x64ExecuteInitialProcess: t validation\n");
    }
    if ( t->tid < 0 || t->tid > THREAD_COUNT_MAX ){
        panic("I_x64ExecuteInitialProcess: tid\n");
    }

// It its context is already saved, 
// so this is not the fist time.
    if (t->saved != FALSE){
        panic("I_x64ExecuteInitialProcess: saved\n");
    }

// Set the current and the foreground threads.
    set_current_thread(t->tid);
    set_foreground_thread(t->tid);

// State
// The thread needs to be in Standby state.
    if (t->state != STANDBY){
        printk("I_x64ExecuteInitialProcess: state tid={%d}\n", t->tid );
        die();
    }

// :: MOVEMENT 2 ( Standby --> Running )

    if (t->state == STANDBY){
        t->state = RUNNING;
        debug_print("I_x64ExecuteInitialProcess: Now RUNNING!\n");
    }

//
// Current process.
//

    if (t->owner_process->pid != GRAMADO_PID_INIT){
        panic("I_x64ExecuteInitialProcess: t->owner_process->pid\n");
    }
    set_current_process(GRAMADO_PID_INIT);

// List
// Dispatcher ready list.
// #ps: Maybe it is not used.
    for ( i=0; i < PRIORITY_MAX; i++ ){
        dispatcherReadyList[i] = (unsigned long) t;
    };

// Counting the type of the dispatching criteria.
    IncrementDispatcherCount(SELECT_IDLE_COUNT);

// Clear nested task flag.
// Bit 14, 0x4000.
// We're in 64bit,
// but in 32bit systems, if we don't clear this flag,
// an iret will cause taswitching.
// See: headlib.asm
    x64_clear_nt_flag();

// CLTS — Clear Task-Switched Flag in CR0
// The processor sets the TS flag every time a task switch occurs. 
// For taskswitching via hardware i guess.
// see:
// https://www.felixcloutier.com/x86/clts
    asm volatile ("clts \n");

// #todo 
// check this
// turn_task_switch_on:
//  + Creates a vector for timer irq, IRQ0.
//  + Enable taskswitch. 

    turn_task_switch_on();

// #todo
// Isso deve ser liberado pelo processo init
// depois que ele habilitar as interrupções.
    
    //taskswitch_lock();
    //scheduler_lock();


// Setup cr3.
// This is a Physical address.
// See: x64.c
// Reload tlb.
// See: https://en.wikipedia.org/wiki/Translation_lookaside_buffer
// #bugbug: rever isso.

    unsigned long __pml4_pa = (unsigned long) t->pml4_PA;
    // Set CR3 register
    x64_load_pml4_table(__pml4_pa);
    // Refresh
    asm volatile ("movq %cr3, %rax");
    asm volatile ("movq %rax, %cr3");

// #maybe
// Vamos iniciar antes para que
// possamos usar a current_tss quando criarmos as threads
    //x64_init_gdt();

// #importante
// Mudamos para a última fase da inicialização.
// Com isso alguns recursos somente para as fases anteriores
// deverão ficar indisponíveis.

// -------------------------------
// Starting phase 4.
    Initialization.current_phase = 4;

// =============
// # go!
// Nos configuramos a idle thread em user mode e agora vamos saltar 
// para ela via iret.
// #todo:
// #importante:
// Podemos usr os endereços que estão salvos na estrutura.
// #bugbug:
// temos a questão da tss:
// será que a tss está configurada apenas para a thread idle do INIT ??
// temos que conferir isso.
// base dos arquivos.
// #todo
// Rever se estamos usando a base certa.
// sm.bin (ELF)
// See: fs.c

// #bugbug
// O processo init deve ter suas proprias tabelas de paginas.
// checar um endereço usando a tabela de paginas do kernel
// esta errado.
// >>> Mas logo acima, acabamos de mudar as tabelas.

    int elfStatus = -1;
    elfStatus = (int) fsCheckELFFile((unsigned long) CONTROLTHREAD_BASE);
    if (elfStatus < 0)
    {
        debug_print("I_x64ExecuteInitialProcess: .ELF signature\n");
        panic      ("I_x64ExecuteInitialProcess: .ELF signature\n");
    }

/*
// ==============
// #test
// ok. It's working fine.
// Testing the structure in exec_elf.h

// #todo
// Create a helper for this routine.

    struct elf_header_64bit_d *elf_header;

    // The base of the image.
    // The header is in the top.
    elf_header = (struct elf_header_64bit_d *) CONTROLTHREAD_BASE;

// signature
    printk ("Signature: %c %c %c \n",
        elf_header->e_ident[1],     // 'E'
        elf_header->e_ident[2],     // 'L'
        elf_header->e_ident[3] );   // 'F'

// file class
// 1 = 32 bit, 2 = 64 bit
    if( elf_header->e_ident[EI_CLASS] != ELFCLASS64 )
    {
        //
    } 
    printk ("Class: %x\n", elf_header->e_ident[EI_CLASS]);

// type
// 1 = relocatable, 2 = executable, 3 = shared, 4 = core
    if( elf_header->e_type != ET_EXEC )
    {
        //
    }
    printk ("Type: %x\n", elf_header->e_type);

// machine
// x86  3 | IA-64  0x32 | x86-64  0x3E(62)
    if( elf_header->e_machine != EM_X86_64 )
    {
        //
    }
    printk ("Machine: %x\n", elf_header->e_machine);

// entry
    if( elf_header->e_entry != CONTROLTHREAD_ENTRYPOINT )
    {
        //
    }
    printk ("Entry: %x\n", elf_header->e_entry);

    //#breakpoint
    refresh_screen();
    while(1){}
// ==============
*/

// ==============
// #debug

    //debug_print("I_x64ExecuteInitialProcess: [x64] Go to user mode! IRETQ\n");
    //printk     ("I_x64ExecuteInitialProcess: [x64] Go to user mode! IRETQ\n");
    //refresh_screen();

// Here is where the boot routine ends.
    system_state = SYSTEM_RUNNING;

// The kernel has booted.
    has_booted = TRUE;

// =============
// Fly!
// #important:
// This is an special scenario,
// Where we're gonna fly with the eflags = 0x3000,
// it means that the interrupts are disabled,
// and the init process will make a software interrupt
// to reenable the interrupts. 
// Softwre interrupts are not affecte by this flag, I guess.
// #bugbug
// This routine is very ugly and very gcc dependent.
// We deserve a better thing.
// We need to have the same stack in the TSS.
// ss, rsp, rflags, cs, rip;
// See:
// gva.h

// cpl
    if (t->cpl != RING3){
        panic ("I_x64ExecuteInitialProcess: cpl\n");
    }

// iopl
// weak protection
    if (t->rflags_initial_iopl != 3){
        panic ("I_x64ExecuteInitialProcess: rflags_initial_iopl\n");
    }

    //PROGRESS(":: Go to ring3!\n");

    //printk("go!\n");
    //while(1){}

// Entry point and ring3 stack.
// CONTROLTHREAD_ENTRYPOINT

    unsigned long entry = (unsigned long) 0x0000000000201000;
    unsigned long rsp3  = (unsigned long) 0x00000000002FFFF0;

// rflags:
// 0x3002
// iopl 3. weak protection.
// Interrupts disabled for the first thread.

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
        " pushq $0x3002   \n" 
        " pushq $0x1B     \n" 
        " pushq %%rax     \n" 
        " iretq           \n" :: "D"(entry), "S"(rsp3) );

// Paranoia
    PROGRESS("::(3) I_x64ExecuteInitialProcess: Unexpeted error\n");
    panic("I_x64ExecuteInitialProcess: Unexpeted error\n");
}

// Local worker
// #todo
// Check the information in the elf header.
// Save some of this information in the process structure. 
// see: exec_elf.h and process.h

static int __load_mod_image(void)
{
// The virtual address for the module image.
// #warning
// This is a static address. Why not?
// Hack me!
    unsigned long ImageAddress = (unsigned long) 0x30A00000;
// #bugbug
// We have a limit for the image size.
    unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (512 * 4096);

    unsigned long fileret=1;

// Check the validation of the name.
    if ((void*) mod_image_name == NULL){
        panic("__load_mod_image: init_image_name\n");
    }

// ---------------------
// It loads a file into the memory.
// IN:
//     fat_address  = FAT address.
//     dir_addresss = Directory address.
//     dir_entries  = Number of entries in the given directory.
//     file_name    = File name.
//     buffer = Where to load the file. The pre-allocated buffer.
//     buffer_size_in_bytes = Maximum buffer size.
// -----------------
// OUT: 
//    1 = fail 
//    0 = ok

    fileret = 
        (unsigned long) fsLoadFile( 
                            VOLUME1_FAT_ADDRESS, 
                            VOLUME1_ROOTDIR_ADDRESS, 
                            FAT16_ROOT_ENTRIES,    //#bugbug: number of entries.
                            mod_image_name, 
                            (unsigned long) ImageAddress,
                            BUGBUG_IMAGE_SIZE_LIMIT ); 

    if (fileret != 0){
        printk("__load_mod_image: on fsLoadFile()\n");
        goto fail;
    }

// OUT: 
//    1 = fail 
//    0 = ok
    return 0;
fail:
    return -1;
}

// Create the kernel process.
// It will create a process for two images:
// This is a ring0 process.
// This process has two images,
// KERNEL.BIN loaded by the boot loader and
// MOD0.BIN loaded by the kernel base.
// This is the virtual address for the base of the image.
// We are using the kernel pagetables for that.
// #define EXTRAHEAP1_VA   0x0000000030A00000
// MOD0_IMAGE_VA
// See: x64gva.h

static int I_x64CreateKernelProcess(void)
{
    int Status=FALSE;
    unsigned long BasePriority = PRIORITY_MAX;
    unsigned long Priority     = PRIORITY_MAX;
    register int i=0;

    //debug_print ("I_x64CreateKernelProcess:\n");

//
// Kernel process
//

// #todo
// Comment about the properties of this process.

// IN: 
// Desktop, Window
// base address, priority, ppid, name, iopl, page directory address.
// See: process.c
// KERNELIMAGE_VA

    ppid_t MyPPID = 0;

    KernelProcess = 
        (void *) create_process( 
                    NULL,  
                    (unsigned long) 0x30000000, 
                    BasePriority, 
                    (int) MyPPID,
                    kernel_process_default_name, 
                    RING0,   
                    (unsigned long ) gKernelPML4Address,
                    (unsigned long ) kernel_mm_data.pdpt0_va,
                    (unsigned long ) kernel_mm_data.pd0_va,
                    PERSONALITY_GRAMADO );

// Struct and struct validation.
    if ((void *) KernelProcess == NULL){
        printk("I_x64CreateKernelProcess: KernelProcess\n");
        return FALSE;
    }
    if (KernelProcess->used != TRUE || KernelProcess->magic != 1234){
        printk("I_x64CreateKernelProcess: KernelProcess validation\n");
        return FALSE;
    }

// pid
// #bugbug: Who gave this pid to this process?
// #warning
// It's because the kernel was the first
// process created. Then the pid is equal 0.
    if (KernelProcess->pid != GRAMADO_PID_KERNEL){
        printk ("I_x64CreateKernelProcess: pid\n");
        return FALSE;
    }

// Let's update the GID for this process.
// It was initialized with 0.
    KernelProcess->gid = (gid_t) GID_KERNEL;
    KernelProcess->rgid = (gid_t) GID_KERNEL;  // real 
    KernelProcess->egid = (gid_t) GID_KERNEL;  // effective
    KernelProcess->sgid = (gid_t) GID_KERNEL;  // saved

// The kernel process is a system program.
// KERNEL.BIN and GWSSRV.BIN

    KernelProcess->type = PROCESS_TYPE_SYSTEM;

    KernelProcess->base_priority = BasePriority;
    KernelProcess->priority = Priority;

// see: layer.h
    KernelProcess->_layer = LAYER_KERNEL;

// This is the kernel process.
    KernelProcess->env_subsystem = CaliSubsystem;

//
// mm
//

// kernel_mm_data validation.

    if (kernel_mm_data.used != TRUE || kernel_mm_data.magic != 1234){
        printk ("I_x64CreateKernelProcess: kernel_mm_data validation\n");
        return FALSE;
    }

    KernelProcess->pml4_VA = kernel_mm_data.pml4_va;
    KernelProcess->pml4_PA = kernel_mm_data.pml4_pa; 

    KernelProcess->pdpt0_VA = kernel_mm_data.pdpt0_va;
    KernelProcess->pdpt0_PA = kernel_mm_data.pdpt0_pa; 

    KernelProcess->pd0_VA = kernel_mm_data.pd0_va;
    KernelProcess->pd0_PA = kernel_mm_data.pd0_pa; 


// cwd
    fs_initialize_process_cwd ( KernelProcess->pid, "/" ); 

// ==================
// The control thread.
// This is the control thread for the 
// window server image.

    /*
    Status = I_x64CreateTID0();
    if ( Status != TRUE ){
        printk("Couldn't Create the WS thread\n");
        return FALSE;
    }
    */

//
// the first Kernel module
//

// ====================
// Initialize the kernel module list.
    for (i=0; i<KMODULE_MAX; i++){
        kmList[i] = 0;
    };

// Load the kernel module image.
// #todo
// Check the information in the elf header.
// Save some of this information in the process structure. 
// see: exec_elf.h and process.h
    __load_mod_image();

// ====================
// Setup the first kernel module.
// It is not a dynlinked module.
// This is just a loadable ring0 thread with shared symbols.

    // see: mod.c
    int mod_status = -1;
    mod_status = (int) mod_initialize_first_module();
    if (mod_status < 0)
        panic ("mod_status\n");

// ...

    return TRUE;
}

// #suspended
// Create a ring0 thread for the window server image.
// It belongs to the kernel process.
static int I_x64CreateTID0(void)
{
/*
    //debug_print ("I_x64CreateTID0:\n");

// Thread
// This is the control thread of the window server module.
// See: create.c, thread.h.

    tid0_thread = (void *) create_tid0();

    if ( (void *) tid0_thread == NULL ){
        printk ("I_x64CreateTID0: tid0_thread\n");
        return FALSE;
    }

    if ( tid0_thread->used != TRUE || 
         tid0_thread->magic != 1234 )
    {
        printk ("I_x64CreateTID0: tid0_thread validation\n");
        return FALSE;
    }

// tid
    if ( tid0_thread->tid != TID0_TID ){
        printk ("I_x64CreateTID0: TID0_TID");
        return FALSE;
    }

// owner pid
    if ( tid0_thread->ownerPID != GRAMADO_PID_KERNEL ){
        printk ("I_x64CreateTID0: GRAMADO_PID_KERNEL");
        return FALSE;
    }

// Memory

    tid0_thread->pml4_VA  = KernelProcess->pml4_VA;
    tid0_thread->pml4_PA  = KernelProcess->pml4_PA;

    tid0_thread->pdpt0_VA = KernelProcess->pdpt0_VA;
    tid0_thread->pdpt0_PA = KernelProcess->pdpt0_PA;

    tid0_thread->pd0_VA   = KernelProcess->pd0_VA;
    tid0_thread->pd0_PA   = KernelProcess->pd0_PA;

//
// tss
//

// #bugbug 
// #todo
    
    // tid0_thread->tss = current_tss;

// Priority

    set_thread_priority( 
        (struct thread_d *) tid0_thread, 
        PRIORITY_MAX );

// #importante
// Sinalizando que ainda não podemos usar as rotinas que dependam
// de que o dead thread collector esteja funcionando.
// Esse status só muda quando a thread rodar.

    dead_thread_collector_status = FALSE;

// Idle thread
// For now,
// the control thread of the window server will be our idle thread.
// But it is not actually a idle routine, 
// it is a standard server code.

    ____IDLE = (struct thread_d *) tid0_thread;
    UPProcessorBlock.IdleThread = (struct thread_d *) ____IDLE;

    //if ( UPProcessorBlock.IdleThread != ____IDLE)
    //   x_panic("here");


// ??
// This is the control thread of the kernel process.
// OK, the loadable tharead that belongs to the ws is
// the kernel process's control thread. :)

    if ((void*)KernelProcess != NULL)
    {
        KernelProcess->control = (struct thread_d *) ____IDLE;
    }

*/
    return TRUE;
}

// ==============================
// I_initKernelComponents:
// ::(5)(3)
// Phases 1.
// Called in the initialization phase 0.
// Called by I_x64_initialize() in x64init.c
// OUT: TRUE if it is ok.
// + Initialize globals.
// + Initialize device manager.
// + Initialize hal components.
// + Initialize storage manager.
// + Initialize filesystem support.

static int I_initKernelComponents(void)
{
    int Status = FALSE;

    //PROGRESS("I_initKernelComponents:\n");

// Check kernel phase.
    if (Initialization.current_phase != 1){
        printk ("I_initKernelComponents: Initialization phase is Not 1.\n");
        return FALSE;
    }

// Initialize globals.
// See: kmain.c
    keInitGlobals();
    //PROGRESS("init_globals ok\n"); 


// Create the device list for all the devices
// in our system, including the hal stuff i guess.
    devmgr_initialize();


// Initialize system HAL.
    Status = halInitialize();
    if (Status != TRUE){
        printk("I_initKernelComponents: halInitialize fail\n");
        return FALSE;
    }
    //PROGRESS("halInitialize ok\n"); 

// Initializat PCI interface.
    init_pci();


//
// $
// STORAGE
//

// ++
// ===============================

// Storage manager
// Ordem: storage, disk, volume, file system.
// #importante 
// A estrutura 'storage' vai ser o nível mais baixo.
// É nela que as outras partes devem se basear.

    //PROGRESS("storage, disk, volume, fs\n"); 
// Storage
// Create the main 'storage' structure.
    int st_status=FALSE;
    st_status = init_storage_support();
    if (st_status != TRUE){
       printk("I_initKernelComponents: init_storage_support fail\n");
       return FALSE;
    }

    // #bugbug
    // The current compilation method is not allowing us
    // to move the routines bellow as part of init_storage_support().

// Disks and volumes.
// see: storage.c
    disk_init();
    volume_init();

// Initializat ata device driver.
// see: ata.c
// IN: msg, data1.
    DDINIT_ata( 1, FORCEPIO );

// >> Precisa do bootdisk e do ata device.
// Set the number of sectors in the boot disk.
// It depends on the disk and ata initialization.
// So, now we can do this.
    Status = (int) storage_set_total_lba_for_boot_disk();
    if (Status != TRUE){
        printk ("I_initKernelComponents: storage_set_total_lba_for_boot_disk fail\n"); 
        return FALSE;
    }
    //PROGRESS("storage_set_total_lba_for_boot_disk ok\n"); 

// mbr info
// It depends on the total lba value for boot disk.
// Its because we're gonna rad the disk to get the partition tables.
    disk_initialize_mbr_info();

// ===============================
//--

// File systems support.
// see: fs.c

    //vfsInit();
    fsInit();
    // ...

// ok
// Return to the main initialization routine
// in x64init.c
    return TRUE;

// ====================
// fail
// Return to the main initialization routine
// in x64init.c

//fail1:
    // If we already have printk verbose.
fail0:
    PROGRESS("::(5)(3)(?): Fail");
    debug_print ("I_initKernelComponents: fail\n");
    return FALSE;
}

//================================
// ::(2)(?)
int I_x64_initialize(void)
{
// Called by keInitialize().

    int Status = FALSE;

// -------------------------------
// Phase counter: 
// Starting phase 0.
// We already did that before in kmain().
    Initialization.current_phase = 0;

    // The first ring3 process.
    // Ainda não configuramos qual será o primeiro processo
    // a rodar em user mode.
    InitialProcessInitialized = FALSE;

// Obs: 
// O video já foi inicializado em main.c.
// Isso atualiza a estrutura de console do console atual.
// #bugbug: 
// A inicialização não funciona, pois os elementos das estruturas
// não guardam os valores corretamente.
// Talvez está escrevendo em lugar inapropriado.
// #test: 
// Mudamos isso para o momento em que inicializamos os consoles.
 
    // #debug
    //debug_print ("I_x64main:\n");

// #debug
// For real machine.
    //printk      ("I_x64main: [TODO]\n");
    //refresh_screen();

// System State
    if (system_state != SYSTEM_BOOTING){
        debug_print ("I_x64_initialize: system_state\n");
        //x_panic   ("I_x64_initialize: system_state\n");
        return FALSE;
    }

// System Arch
    if (current_arch != CURRENT_ARCH_X86_64){
        debug_print ("I_x64_initialize: current_arch fail\n");
        //x_panic   ("I_x64_initialize: current_arch fail\n");
        return FALSE;
    }


// ================================
// sse support.
    //PROGRESS("::(2)(?)\n"); 
    //debug_print ("I_x64_initialize: [TODO] SSE support\n");
    // x86_sse_init();

// ===================================
// I_init
// Calling the main initialization routine.
// Antes de tudo: 
// CLI, Video, runtime.
// ## BUGBUG ##
// As mensagens do abort podem não funcionarem nesse caso.
// AINDA NÃO INICIALIZAMOS O RECURSO DE MENSAGENS.
// Essa rotina só pode ser chamada 
// durante essa fase da inicialização.
// See: sysinit.c

    //PROGRESS("::(2)(?)\n"); 
    if (Initialization.current_phase != 0){
        debug_print ("I_x64_initialize: Initialization phase is NOT 0.\n");
        return FALSE;
    }

// -------------------------------
// Starting phase 1.
    //PROGRESS(":: Call\n"); 
    Initialization.current_phase = 1;

// -------------------------------
// Initialize a lot of kernel components
// + Initialize globals.
// + Initialize device manager.
// + Initialize hal components.
// + Initialize storage manager.
// + Initialize filesystem support.

    Status = (int) I_initKernelComponents(); 
    if (Status != TRUE){
        printk("I_x64_initialize: on I_initKernelComponents\n");
        return FALSE;
    }

// -------------------------------
// Starting phase 2.
    Initialization.current_phase = 2;


// ==========================
// microkernel components:
// scheduler, process, thread (intake)

    Status = keInitializeIntake();
    if (Status != TRUE){
        printk ("I_x64_initialize: keInitializeIntake fail\n");
        return FALSE;
    }
    //PROGRESS("keInitializeIntake ok\n"); 


// -------------------------------
// Starting phase 3.
    Initialization.current_phase = 3;

// ================================
// [KERNEL PROCESS] :: Creating kernel process.
// Local
// It loads the window server's image and create
// a process structure to handle the kernel base and the
// window server's control thread.

    PROGRESS("Create kernel process\n"); 
    Status = I_x64CreateKernelProcess();
    if (Status != TRUE){
        debug_print ("Couldn't create the Kernel process\n");
        return FALSE;
    }

// ================================
// [INIT PROCESS] :: Create the first ring3 process.
// INIT.BIN.

    PROGRESS("Create init process\n"); 
    Status = I_x64CreateInitialProcess();
    if (Status != TRUE){
        debug_print ("Couldn't create the Initial process\n");
        return FALSE;
    }

    return TRUE;

// ================================
// The routine fails.

fail:
    // Nothing
    //debug_print("I_x64main: Fail\n"); 
fail0:
    debug_print ("I_x64_initialize: fail\n");
    refresh_screen(); 
    return FALSE;
}

/*
// ==============================
// ::(2)
void I_x64InitializeKernel(int arch_type)
{
// We don't have any print support yet.

    //#hack
    // current_arch = CURRENT_ARCH_X86_64;
// see:
// kernel/init.c
    I_kmain(arch_type);
}
*/

