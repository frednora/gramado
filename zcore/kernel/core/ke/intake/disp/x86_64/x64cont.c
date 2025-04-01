// x64cont.c
// Context support for x86_64 arch.
// Created by Fred Nora.

#include <kernel.h>

// globals.
// Context:
// Variáveis para salvar o contexto para x86_64.
// Essas variáveis devem permanecer encapsuladas nesse arquivo 
// do módulo. 
// Somente esse arquivo terá acesso direto à essas informações.
// #importante
// Algumas poucas rotinas vão importar essas variáveis.

unsigned long contextSS=0;        // User mode.
unsigned long contextRSP=0;       // User mode.
unsigned long contextRFLAGS=0; 
unsigned long contextCS=0;
unsigned long contextRIP=0;

unsigned long contextDS=0;
unsigned long contextES=0;
unsigned long contextFS=0;  // ?
unsigned long contextGS=0;  // ?

unsigned long contextRAX=0;
unsigned long contextRBX=0;
unsigned long contextRCX=0;
unsigned long contextRDX=0;

unsigned long contextRSI=0;
unsigned long contextRDI=0;

unsigned long contextRBP=0;

unsigned long contextR8=0;
unsigned long contextR9=0;
unsigned long contextR10=0;
unsigned long contextR11=0;
unsigned long contextR12=0;
unsigned long contextR13=0;
unsigned long contextR14=0;
unsigned long contextR15=0;

// #todo
// Debug registers

// The cpl of the thread.
// Updated by irq0.
unsigned long contextCPL=0;

// fpu buffer
// Defined in unit1hw.asm
unsigned char context_fpu_buffer[512];
// char fxsave_region[512] __attribute__((aligned(16)));

// ===============================================

/*
 * save_current_context: 
 *    Salvando o contexto da thread interrompida pelo timer IRQ0.
 *    O contexto da tarefa interrompida foi salvo em vari�veis pela 
 * isr do timer. Aqui esse contexto � colocado na estrutura que 
 * organiza as threads.
 * @todo: 
 *     Est�o faltando vari�veis nesse contexto, como registradores de 
 * debug e float point por exemplo.
 *     Mudar nome para contextSaveCurrent();.
 */
// unit 1: Capture context.
// Intake
void save_current_context (void)
{
    struct thread_d  *t;

    // Context
    unsigned long *contextss  = (unsigned long *) &contextSS;
    unsigned long *contextrsp = (unsigned long *) &contextRSP;
    unsigned long *contextrflags = (unsigned long *) &contextRFLAGS;
    unsigned long *contextcs  = (unsigned long *) &contextCS;
    unsigned long *contextrip = (unsigned long *) &contextRIP;
    unsigned long *contextds  = (unsigned long *) &contextDS;
    unsigned long *contextes  = (unsigned long *) &contextES;
    unsigned long *contextfs  = (unsigned long *) &contextFS;
    unsigned long *contextgs  = (unsigned long *) &contextGS;
    unsigned long *contextrax = (unsigned long *) &contextRAX;
    unsigned long *contextrbx = (unsigned long *) &contextRBX;
    unsigned long *contextrcx = (unsigned long *) &contextRCX;
    unsigned long *contextrdx = (unsigned long *) &contextRDX;
    unsigned long *contextrsi = (unsigned long *) &contextRSI;
    unsigned long *contextrdi = (unsigned long *) &contextRDI;
    unsigned long *contextrbp = (unsigned long *) &contextRBP;

    unsigned long *contextr8 = (unsigned long *) &contextR8;
    unsigned long *contextr9 = (unsigned long *) &contextR9;
    unsigned long *contextr10 = (unsigned long *) &contextR10;
    unsigned long *contextr11 = (unsigned long *) &contextR11;
    unsigned long *contextr12 = (unsigned long *) &contextR12;
    unsigned long *contextr13 = (unsigned long *) &contextR13;
    unsigned long *contextr14 = (unsigned long *) &contextR14;
    unsigned long *contextr15 = (unsigned long *) &contextR15;

    unsigned long *context_cpl = (unsigned long *) &contextCPL;


// See: ts.c where we are using current_thread
// before calling this routine.

// Structure ~ Colocando o contexto na estrutura.

    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX )
    {
        printk("save_current_context: TID=%d\n", current_thread );
        goto fail0;
    }

// The thread structure.
    t = (void *) threadList[current_thread];
    if ((void *) t == NULL){
        printk ("save_current_context: [ERROR] Struct Thread={%d}\n",
            current_thread );
        goto fail1;
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        printk ("save_current_context: [ERROR] Validation Thread={%d}\n",
            current_thread );
        goto fail1;
    }

// Fill the structure.

// #todo
// use t->x64_context.ss  ...

    // Stack frame
    t->context.ss     = (unsigned long) (contextss[0] & 0xFFFF);  // usermode
    t->context.rsp    = (unsigned long) contextrsp[0];            // usermode
    t->context.rflags = (unsigned long) contextrflags[0];
    t->context.cs     = (unsigned long) (contextcs[0]  & 0xFFFF);
    t->context.rip    = (unsigned long) contextrip[0];

    // Segments
    t->context.ds = (unsigned long) (contextds[0] & 0xFFFF);
    t->context.es = (unsigned long) (contextes[0] & 0xFFFF);
    t->context.fs = (unsigned long) (contextfs[0] & 0xFFFF);
    t->context.gs = (unsigned long) (contextgs[0] & 0xFFFF);

// General purpose

    t->context.rax = (unsigned long) contextrax[0];
    t->context.rbx = (unsigned long) contextrbx[0];
    t->context.rcx = (unsigned long) contextrcx[0];
    t->context.rdx = (unsigned long) contextrdx[0];
    t->context.rsi = (unsigned long) contextrsi[0];
    t->context.rdi = (unsigned long) contextrdi[0];
    t->context.rbp = (unsigned long) contextrbp[0];

    t->context.r8 = (unsigned long) contextr8[0];
    t->context.r9 = (unsigned long) contextr9[0];
    t->context.r10 = (unsigned long) contextr10[0];
    t->context.r11 = (unsigned long) contextr11[0];
    t->context.r12 = (unsigned long) contextr12[0];
    t->context.r13 = (unsigned long) contextr13[0];
    t->context.r14 = (unsigned long) contextr14[0];
    t->context.r15 = (unsigned long) contextr15[0];

// Save fpu stuff.
// #ps 
// Byte by byte. We can do that in a better way.
    register int i=0;
    for (i=0; i<512; i++){
        t->context.fpu_buffer[i] = (unsigned char) context_fpu_buffer[i];
    };


// 
// -- cpl --------
//

// This cpl came from the cs register found in the iretq stack frame,
// right after the timer interrupt.
// #bugbug:
// Maybe it is not the same in the actual cs register
// in the moment of the irq handler routine.

    int cpl = -1;
    unsigned long tmp_cpl = (unsigned long) context_cpl[0];

// 2 bits
// The first 2 bits of cs.
    cpl = (int) (tmp_cpl & 3);

    if (cpl != t->cpl)
        panic("save_current_context: cpl != t->cpl\n");


// Ja temos o valor do current process nesse momento?
    //pid_t current_process = (pid_t) get_current_process();
    
    if( cpl != 0 && cpl != 1 && cpl != 2 && cpl != 3 )
    {
        panic("save_current_context: cpl\n");
    }

// Isso significa que um thread que estava rodando em ring0,
// foi interrompida e teve seu contexto salvo por essa rotina.
    if (cpl == 0){
        panic("save_current_context: cpl 0\n");
    }
    if (cpl == 1){
        panic("save_current_context: cpl 1\n");
    }
    if (cpl == 2){
        panic("save_current_context: cpl 2\n");
    }
// Significa que uma thread que estava rodando em ring3,
// foi interrompida e teve seu context salvo por essa rotina.
    if (cpl == 3){
        t->transition_counter.to_supervisor++;
    }

// rflags
// ok, it's working
    unsigned long rflags = (unsigned long) t->context.rflags;
    int rflags_iopl = (int) (rflags & 0x200);

// changing it on the fly
    t->rflags_current_iopl = (unsigned int) rflags_iopl;

// #todo
// Vamos analisar o que acabamos de capturar e
// configurar seu destino daqui pra frente.

//done:
    return;
fail1:
    show_process_information();
fail0:
    die();
}

/*
 * restore_current_context: 
 *     Carregando o contexto da pr�xima thread a ser executada.
 *     Pegando os valores na estrutura e colocando nas vari�veis 
 * que ser�o usadas pelo arquivo em assembly para carregar os valores 
 * dentro dos registradores.
 * @todo:   
 *     Mudar nome para contextRestoreCurrent();.
 */
// unit 3: Release the context.
// Burgundy?

void restore_current_context (void)
{
    struct thread_d  *t;

    // Context
    unsigned long *contextss  = (unsigned long *) &contextSS;
    unsigned long *contextrsp = (unsigned long *) &contextRSP;
    unsigned long *contextrflags = (unsigned long *) &contextRFLAGS;
    unsigned long *contextcs  = (unsigned long *) &contextCS;
    unsigned long *contextrip = (unsigned long *) &contextRIP;
    unsigned long *contextds  = (unsigned long *) &contextDS;
    unsigned long *contextes  = (unsigned long *) &contextES;
    unsigned long *contextfs  = (unsigned long *) &contextFS;
    unsigned long *contextgs  = (unsigned long *) &contextGS;
    unsigned long *contextrax = (unsigned long *) &contextRAX;
    unsigned long *contextrbx = (unsigned long *) &contextRBX;
    unsigned long *contextrcx = (unsigned long *) &contextRCX;
    unsigned long *contextrdx = (unsigned long *) &contextRDX;
    unsigned long *contextrsi = (unsigned long *) &contextRSI;
    unsigned long *contextrdi = (unsigned long *) &contextRDI;
    unsigned long *contextrbp = (unsigned long *) &contextRBP;

    unsigned long *contextr8 = (unsigned long *) &contextR8;
    unsigned long *contextr9 = (unsigned long *) &contextR9;
    unsigned long *contextr10 = (unsigned long *) &contextR10;
    unsigned long *contextr11 = (unsigned long *) &contextR11;
    unsigned long *contextr12 = (unsigned long *) &contextR12;
    unsigned long *contextr13 = (unsigned long *) &contextR13;
    unsigned long *contextr14 = (unsigned long *) &contextR14;
    unsigned long *contextr15 = (unsigned long *) &contextR15;

    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
        printk ("restore_current_context: TID=%d\n", current_thread );
        goto fail0;
    }

    t = (void *) threadList[current_thread]; 
    if ((void *) t == NULL){
        printk("restore_current_context error: t\n");
        goto fail1;
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        printk("restore_current_context error: t validation\n");
        goto fail1;
    }

//
// Restore
//

    // Stack frame
    contextss[0]     = (unsigned long) t->context.ss & 0xffff;  // usermode
    contextrsp[0]    = (unsigned long) t->context.rsp;          // usermode 
    contextrflags[0] = (unsigned long) t->context.rflags;
    contextcs[0]     = (unsigned long) t->context.cs & 0xffff;  
    contextrip[0]    = (unsigned long) t->context.rip;

    // Segments
    contextds[0] = (unsigned long) t->context.ds & 0xffff;
    contextes[0] = (unsigned long) t->context.es & 0xffff; 
    contextfs[0] = (unsigned long) t->context.fs & 0xffff; 
    contextgs[0] = (unsigned long) t->context.gs & 0xffff; 

// General purpose.

    contextrax[0] = (unsigned long) t->context.rax;  
    contextrbx[0] = (unsigned long) t->context.rbx; 
    contextrcx[0] = (unsigned long) t->context.rcx;  
    contextrdx[0] = (unsigned long) t->context.rdx; 
    contextrsi[0] = (unsigned long) t->context.rsi;  
    contextrdi[0] = (unsigned long) t->context.rdi; 
    contextrbp[0] = (unsigned long) t->context.rbp;  
    // Continua...

    contextr8[0]  = (unsigned long) t->context.r8;  
    contextr9[0]  = (unsigned long) t->context.r9;  
    contextr10[0] = (unsigned long) t->context.r10;  
    contextr11[0] = (unsigned long) t->context.r11;  
    contextr12[0] = (unsigned long) t->context.r12;  
    contextr13[0] = (unsigned long) t->context.r13;  
    contextr14[0] = (unsigned long) t->context.r14;  
    contextr15[0] = (unsigned long) t->context.r15; 

// Restore
// #ps 
// Byte by byte. We can do that in a better way.
    register int i=0;
    for (i=0; i<512; i++){
        context_fpu_buffer[i] = (unsigned char) t->context.fpu_buffer[i];
    };

// Is ring3?
    if (t->cpl != 3){
       panic ("restore_current_context: t->cpl\n");
    }
// Transition counter
    if (t->cpl == 3){
        t->transition_counter.to_user++;
    }

// Restore CR3 and flush TLB.
    load_pml4_table((unsigned long) t->pml4_PA);
// #bugbug: rever isso.
    asm ("movq %cr3, %rax");
    asm ("movq %rax, %cr3");
//done:
    return;
fail1:
    show_process_information(); 
fail0:
    die();
}

// Checar um contexto valido para threads em ring 3. 
// #bugbug: 
// Nao usaremos mais isso.
int contextCheckThreadRing3Context(int tid)
{
    struct thread_d  *t;

    if ( tid < 0 || tid >= THREAD_COUNT_MAX ){
        return FALSE;
    }
    t = (void *) threadList[tid]; 
    if ((void *) t == NULL){
        return FALSE;
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        debug_print("contextCheckThreadRing3Context: validation\n");
        return FALSE;
    }

//
// cpl
//

    if (t->cpl != RING3)
        panic("contextCheckThreadRing3Context: cpl\n");

//
// iopl
//

// For now we only accept ring 3 threads with weak protection

    if (t->rflags_initial_iopl != 3){
        debug_print("contextCheckThreadRing3Context: rflags_initial_iopl\n");
        return FALSE;
    }

    if (t->rflags_current_iopl != 3){
        debug_print("contextCheckThreadRing3Context: rflags_current_iopl\n");
        return FALSE;
    }

// Segment
    if (t->context.cs != 0x1B) {
        debug_print("contextCheckThreadRing3Context: segments fail\n");
        return FALSE; 
    }

    // ...

// OK: This is a valid thread for ring3.
    return TRUE;
}

