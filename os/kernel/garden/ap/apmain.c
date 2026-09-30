// apmain.c
// Main file when the kernel starts up some AP processor.
// Created by Fred Nora

#include <kernel.h>


static void __ap_DPC_loop(int lapic_id);
static void __ap_ANIMATION_loop(void);
static void __ap_kmain_imp(void);

// ========================================


// #test:
// The AP is operating as a DPC dispatcher.
// It reads a message from the BSP's structure.

static void __ap_DPC_loop(int lapic_id)
{
    static int BSP_ID = 0;
    int i=0;

// Parameter:
    if (lapic_id < 0 || lapic_id >= NR_CPUS)
    {
        panic("__ap_DPC_loop: lapic_id\n");
    }

    // #todo
    // Probably the BSP can't use this routine.
    //if (lapic_id == BSP_ID){
        //panic("__ap_DPC_loop: core is BSP\n");
    //}


//
// Animation and QF experiment
//

    // #test
    // Drawing rectangles
    unsigned int Color = COLOR_BLACK;
    int Counter = 0;

    // #test: 
    // Using DPC via AP, not via zero gravity.
    // Turn it on
    if (CONFIG_USE_DPC_VIA_AP == 1)
    {
        lapic_info[ BSP_ID ].DPC_QUEUE.on = TRUE;

        // This AP is working as a DPC dispatcher.
        lapic_info[lapic_id].is_dpc_dispatcher = TRUE;
        // So, it can't be used by the scheduler for loading balance.
        lapic_info[lapic_id].dedicated_service = TRUE;
    }


    // i/o channel:
    
    // #ps:
    // This is the idea of Defered Procedure Call (DPC) in Windows.
    // The AP is doing the work that belongs to the handlers of the IRQs.
    while (1){

        //while (apic_SPINLOCK == TRUE){ asm ("pause \n"); };

        // Do we have a new message?
        if (lapic_info[ BSP_ID ].DPC_QUEUE.on == TRUE)
        {
            int slot = qf_get_message();
            if (slot != (-1))
            {
                if (slot >=0 && slot < 32)
                {
                    int msgcode = lapic_info[ BSP_ID ].DPC_QUEUE.cache_msg[0];

                    switch (msgcode)
                    {
                        case 1000:
                            //x_panic("1000: DPC Via AP");
                            wmRawKeyEvent (
                                lapic_info[BSP_ID].DPC_QUEUE.cache_chars[0], 
                                lapic_info[BSP_ID].DPC_QUEUE.cache_chars[1], 
                                lapic_info[BSP_ID].DPC_QUEUE.cache_chars[2], 
                                lapic_info[BSP_ID].DPC_QUEUE.cache_chars[3]
                            );
                            break;

                        case 2000:
                            // x_panic("2000: DPC Via AP");
                            int mouse_event_id = 
                                ( lapic_info[BSP_ID].DPC_QUEUE.cache_longs[0] & 0xFFFFFFFF); 
                            wmMouseEvent( 
                                mouse_event_id,  //event_id, 
                                lapic_info[BSP_ID].DPC_QUEUE.cache_longs[1],  //long1, 
                                lapic_info[BSP_ID].DPC_QUEUE.cache_longs[2]  //long2 
                            );
                            break;

                        case 3000:
                            break;

                        case 4000:
                            break;

                        // #ps: hang this processor
                        case 8000:
                            asm ("hlt \n");
                            break;
                    };
                }
            }

            /*
            // Check destination
            switch (DPC_QUEUE.destination)
            {

                case 1000: // Raw keyboard
                    x_panic("i/o channel");
                    wmRawKeyEvent (
                        DPC_QUEUE.char1, DPC_QUEUE.char2, DPC_QUEUE.char3, DPC_QUEUE.char4
                    );
                    break;
            }
            //DPC_QUEUE.busy = FALSE;  // We are not busy anmore
            */
        }


        Counter++;
        Color = COLOR_YELLOW;
        if (Counter % 2 == 0)
            Color = COLOR_RED;

        for (i=0; i<100; i++){
            frontbuffer_draw_rectangle( 0, i, 4, 4, Color, 0 );
        };

        //wproxy_ap_test();


        /*
        // #test
        // Process request
        int n = WelcomeAP.msg_code;
        switch (n){
            case 1000:
                asm ("hlt");
                break;
            //case 2000:
            default:
                asm ("pause");
                break;
        };
        WelcomeAP.msg_code = 0;  // Erease
        */


        // #test
        // Delay
        // With this delay we can have performance enough 
        // in both cores to make tests.
        // #todo: Do not change it for now.
        asm ("xorl %%eax, %%eax" ::);
        asm ("pause \n");
        asm ("outb %%al, $0x80"  ::);


        // Syscall
        // It needs only to be called from ring 3 actually
        // #test: The handler was called.
        // #bugbug:
        // We can't do that because the AP still do not a 
        // TSS or a ring 0 stack.

        //if (system_state == SYSTEM_RUNNING)
            //asm ("int $0x80 \n");

        // spurious
        // #bugbug
        // It's not working. The system crashes.
        // Probably still because the lack of tss and ring 0 stack.
        //if (system_state == SYSTEM_RUNNING)
            //asm ("int $0xFF \n");

        // #bugbug
        // Testing the handlers for taskswitching
        // if (system_state == SYSTEM_RUNNING)
            //asm ("int $32 \n");
    };

    // Turn the QF off
    lapic_info[BSP_ID].DPC_QUEUE.on = FALSE;
}


static void __ap_ANIMATION_loop(void)
{
    int i=0;

//
// Animation
//

    // #test
    // Drawing rectangles
    unsigned int Color = COLOR_BLACK;
    int Counter = 0;

    while (1){

        //while (apic_SPINLOCK == TRUE){ asm ("pause \n"); };

        Counter++;
        Color = COLOR_YELLOW;
        if (Counter % 2 == 0)
            Color = COLOR_RED;

        for (i=0; i<100; i++){
            frontbuffer_draw_rectangle( 0, i, 4, 4, Color, 0 );
        };

        //wproxy_ap_test();

        // #test
        // Delay
        // With this delay we can have performance enough 
        // in both cores to make tests.
        // #todo: Do not change it for now.
        asm ("xorl %%eax, %%eax" ::);
        asm ("pause \n");
        asm ("outb %%al, $0x80"  ::);


        // Syscall
        // It needs only to be called from ring 3 actually
        // #test: The handler was called.
        // #bugbug:
        // We can't do that because the AP still do not a 
        // TSS or a ring 0 stack.

        //if (system_state == SYSTEM_RUNNING)
            //asm ("int $0x80 \n");

        // spurious
        // #bugbug
        // It's not working. The system crashes.
        // Probably still because the lack of tss and ring 0 stack.
        //if (system_state == SYSTEM_RUNNING)
            //asm ("int $0xFF \n");

        // #bugbug
        // Testing the handlers for taskswitching
        // if (system_state == SYSTEM_RUNNING)
            //asm ("int $32 \n");
    };
}

//
// $
// AP INITIALIZATION
//

// #todo
// We need One GDT per-core AND 
// One IDT per-core (in x86-64 SMP)
// #ps: We have support for GDT in C.

static void __ap_kmain_imp(void)
{
    int i=0;
    int lapic_id = -1;

    asm ("cli");  // For safety

// CLTS — Clear Task-Switched Flag in CR0
// The processor sets the TS flag every time a task switch occurs. 
// For taskswitching via hardware i guess.
// see:
// https://www.felixcloutier.com/x86/clts

    asm volatile ("clts \n");

    //PROGRESS("AP_kmain: \n")

    printk("AP_kmain: [DEBUG] Calling __AP_BSP_handshake\n");

// Talk with the BSP in order to identify the current AP.
// #ps: return the lapic info id, not the real hw cpu id.
// see: kmain.c

    lapic_id = (int) __AP_BSP_handshake();

// #bugbug
// #important:
// The handshake is fully working only for the first AP we launch.
// for anyone of them, but not for more than one.

    printk("AP_kmain: [DEBUG] handshake ok for core=%d\n", lapic_id);


// #test
/*
    if (lapic_id > 1)
    {
        //panic("AP_kmain: id\n", lapic_id);
        asm ("hlt");
    }
*/


/*
    if (lapic_id <0 || lapic_id >= NR_CPUS)
    {
        panic("AP_kmain: id\n");
    }
*/

//
// Compare
//

/*
    int MyHardwareID = (int) apic_get_id_00();
    if (lapic_info[id].local_id == MyHardwareID)
    {
        panic("AP_kmain: MyHardwareID\n");
    }
*/

/*

// #ps:
// Maybe we do not need to call this routine and 
// have an initialization path for the AP different from the BSP.

    ProcessorNumber = id;  // ID
    Status = (int) I_initialize_kernel(arch_type, ProcessorNumber);
    if (Status == FALSE){
        PROGRESS("on I_initialize_kernel()\n");
    }
    if (system_state == SYSTEM_ABORTED){
        PROGRESS("SYSTEM_ABORTED\n");
    }
*/


// =================================================================

// Initialize the GDT and the proper TSS for this core
// IN: 
// + index for lapic_info[i] table
// + ring 0 stack base for the TSS that belongs to this core
// + gdt
// + gdtr

// We cannot change the same GDT used by the BSP core.
// Maybe we can simply load it for the AP.
// We need a new gdt address for the GDT the belongs to the AP.

    static struct segment_descriptor_d ap_gdt[DESCRIPTOR_COUNT_MAX];
    static struct gdt_ptr_d ap_gdtr;

    void *r0_stack;
    r0_stack = kmalloc(4096);
    if (!r0_stack)
        panic("AP: r0_stack");

    x64_init_ap_gdt(
        lapic_id,
        (unsigned long)r0_stack + 4096,
        ap_gdt,
        &ap_gdtr );

// ======================================================================


//
// Thread
//

// #test
// Create the first thread for this code.
// + ring 0
// + not preemptable (for now)
// + ...
// #ps: For now we are simply creating it, not dispatching it.

    unsigned long stack_base =
        (unsigned long) kmalloc(4096);
    unsigned long entry_point =
        (unsigned long) &__ap_ANIMATION_loop;
    ppid_t OwnerPID = GRAMADO_PID_KERNEL;

    struct thread_d *t =
        create_thread(
            THREAD_TYPE_SYSTEM,
            NULL,
            entry_point,
            stack_base + 4096,
            OwnerPID,
            "ap-thread-r0",
            RING0 
        );

    // #debug
    if ((void*) t == NULL)
        x_panic("__ap_kmain_imp: t");

// Link it to this lapic id
    t->current_processor = lapic_id;

// This is what i am gonna do after the thread creation 
// just as a safe measure ... 
// in the case this thread eventually reaches the task swtiching

    t->is_preemptable = UNPREEMPTABLE;


/*
This is the current situation

BSP
  Timer IRQ
  Preemption
  Scheduler

AP
  No timer scheduling yet
  Dedicated thread
  Runs forever
*/

// ===============================

//
// DPC
//

    // #test: Using DPC via AP
    if (CONFIG_USE_DPC_VIA_AP == 1){
        __ap_DPC_loop(lapic_id);
    }

//
// Animation
//

    __ap_ANIMATION_loop();

// Something went wrong with this AP.
// #todo:
// Call a system routine in order to report this.

AP_die:
    while (1){
        asm (" cli ");
        asm (" hlt ");
    };
}


//
// #
// Main function (For AP processor)
//

void AP_kmain(void)
{
    __ap_kmain_imp();
}




