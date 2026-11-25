// tss.c


#include <kernel.h>

struct tss_d TSS[NR_CPUS];


// ==================================


// ======================

void
tss_init ( 
    struct tss_d *tss, 
    void *stack_address )
{
    if ((void *) tss == NULL){
        debug_print ("[x64] tss_init:\n");
        panic       ("[x64] tss_init:\n");
    }

// Clean
    memset ( tss, 0, sizeof *tss );
    //memset ( tss, 0, sizeof(struct tts_d) ); //#todo

// ring 0 stack
    //#todo
    //if ( stack_address == 0 )
    //    panic("tss_init: stack_address\n");
    tss->rsp0 = (unsigned long) stack_address;  // va?? 


    tss->IOPB_offset = sizeof(struct tss_d);

    //#debug
    //printk ("Stack %x\n", stack_address);
    //refresh_screen();
    //while(1){}
}


