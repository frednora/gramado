// x64cont.c
// Context support for x86_64 arch.
// Created by Fred Nora.

#include <kernel.h>


// Global size of the last stack frame.
int gszLastStackFrame=5;

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

// arch_save_context:
// Saving the context of the thread interrupted by timer IRQ0.
// The context of the interrupted task was saved in variables by the timer isr.
// Here this context is placed in the structure that organizes the threads.
// Facility's unit 1: (Intake) 
// + Capture the context
void arch_save_context(int lapic_info_id)
{
    struct thread_d  *t;
    int __lapic_info_id = lapic_info_id;

// #debug:
// Only the BSP processor can save the context for now
    if (__lapic_info_id != 0){
        panic("arch_save_context: #todo Not BSP\n");
    }

// The current thread for this core
    tid_t CurrentTID = lapic_info[__lapic_info_id].current_thread;

    register int i=0;

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


// See: ts.c where we are using it
// before calling this routine.

// Structure ~ Colocando o contexto na estrutura.

    if ( CurrentTID < 0 || CurrentTID >= THREAD_COUNT_MAX )
    {
        printk("arch_save_context: CurrentTID=%d\n", CurrentTID );
        goto fail0;
    }

// The thread structure
    t = (void *) threadList[CurrentTID];
    if ((void *) t == NULL){
        printk ("arch_save_context: [ERROR] struct for CurrentTID={%d}\n",
            CurrentTID );
        goto fail1;
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        printk ("arch_save_context: [ERROR] Validation CurrentTID={%d}\n",
            CurrentTID );
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
        panic("arch_save_context: cpl != t->cpl\n");

// Ja temos o valor do current process nesse momento?
    //pid_t current_process = (pid_t) get_current_process();
    
    if ( cpl != 0 && cpl != 1 && cpl != 2 && cpl != 3 )
    {
        panic("arch_save_context: cpl\n");
    }

// It doesn't run ring 0 threads yet.
// That’s why ring 0 threads aren’t fully supported yet: 
// the code explicitly refuses to save them.

// Isso significa que um thread que estava rodando em ring0,
// foi interrompida e teve seu contexto salvo por essa rotina.
    if (cpl == 0)
    {
        if (CONFIG_ALLOW_RING0_CONTEXT_SAVE == 1)
        {
            //panic("arch_save_context: cpl 0 #test\n");
            //printk("step %d\n", t->step);
        }
        if (CONFIG_ALLOW_RING0_CONTEXT_SAVE != 1){
            panic("arch_save_context: cpl 0\n");
        }
        t->transition_counter.to_supervisor++;

        // #ps: We are also doing this in hw1.asm
        gszLastStackFrame = 3;
    }
    if (cpl == 1){
        panic("arch_save_context: cpl 1\n");
    }
    if (cpl == 2){
        panic("arch_save_context: cpl 2\n");
    }
// It means that a thread that was running in ring3, 
// was interrupted and had its context saved by this routine.
    if (cpl == 3)
    {

        // #bugbug: Not accurate
        // If a Ring 3 thread is interrupted by IRQ0, the CPU switches 
        // privilege levels (Ring 3 → Ring 0). That’s a transition into 
        // supervisor mode.
        t->transition_counter.to_supervisor++;  // (save path)  

        // #ps: We are also doing this in hw1.asm
        gszLastStackFrame = 5;
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

    return;

fail1:
    show_process_information();
fail0:
    die();
}

// arch_restore_context:
// Setting up the context of the next thread to be executed.
// The context is loaded from the thread structure and placed in the variables
// that will be used by the assembly file to load the values into the registers.
// It also updates cr3. 
// Facility's unit 3: (Burgundy)
// Release the context
void arch_restore_context(int lapic_info_id)
{
    struct thread_d  *t;

    int __lapic_info_id = lapic_info_id;

// #debug:
// Only the BSP processor can save the context for now
    if (__lapic_info_id != 0){
        panic("arch_restore_context: #todo Not BSP\n");
    }

// The current thread for this core
    tid_t CurrentTID = lapic_info[__lapic_info_id].current_thread;

    register int i=0;

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

    if ( CurrentTID < 0 || CurrentTID >= THREAD_COUNT_MAX )
    {
        printk ("arch_restore_context: CurrentTID=%d\n", CurrentTID );
        goto fail0;
    }

    t = (void *) threadList[CurrentTID]; 
    if ((void *) t == NULL){
        printk("arch_restore_context error: t\n");
        goto fail1;
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        printk("arch_restore_context error: t validation\n");
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
    for (i=0; i<512; i++){
        context_fpu_buffer[i] = (unsigned char) t->context.fpu_buffer[i];
    };


// It doesn't run ring 0 threads yet.
// That’s why ring 0 threads aren’t fully supported yet: 
// the code explicitly refuses to restore them.

/*
// Is ring3?
    if (t->cpl != 3){
    }
*/

// Transition counter
    if (t->cpl == 0)
    {
        if (CONFIG_ALLOW_RING0_CONTEXT_RESTORE != 1){
            panic ("arch_restore_context: t->cpl 0\n");
        }
        t->transition_counter.to_supervisor++;
    }
// Transition counter
    if (t->cpl == 1){
       panic ("arch_restore_context: t->cpl 1\n");
    }
// Transition counter
    if (t->cpl == 2){
       panic ("arch_restore_context: t->cpl 2\n");
    }
// Transition counter
    if (t->cpl == 3){
        t->transition_counter.to_user++;  // (restore path)  
    }

// Restore CR3 and flush TLB
    load_pml4_table((unsigned long) t->pml4_PA);
    asm ("movq %cr3, %rax");
    asm ("movq %rax, %cr3");
    return;

fail1:
    show_process_information(); 
fail0:
    die();
}

