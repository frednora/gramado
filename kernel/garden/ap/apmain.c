// apmain.c

#include <kernel.h>


static void __ap_animation_experiment(void);
static void __ap_DPC_experiment(int lapic_id);
static void AP_kmain2(void);

// ========================================


static void __ap_animation_experiment(void)
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


// #test:
// The AP is operating as a DPC dispatcher.
static void __ap_DPC_experiment(int lapic_id)
{
    int i=0;

// Parameter:
    if (lapic_id < 0 || lapic_id >= NR_CPUS)
    {
        panic("__ap_DPC_experiment: lapic_id\n");
    }

//
// Animation and QF experiment
//

    // #test
    // Drawing rectangles
    unsigned int Color = COLOR_BLACK;
    int Counter = 0;

    // #test: Using DPC via AP, not via zero gravity.
    // Turn it on
    if (CONFIG_USE_DPC_VIA_AP == 1)
    {
        lapic_info[0].DPC_QUEUE.on = TRUE;

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
        if (lapic_info[0].DPC_QUEUE.on == TRUE)
        {
            int slot = qf_get_message();
            if (slot != (-1))
            {
                if (slot >=0 && slot < 32)
                {
                    int msgcode = lapic_info[0].DPC_QUEUE.cache_msg[0];
                    switch (msgcode)
                    {
                        case 1000:
                            //x_panic("1000: DPC Via AP");
                            wmRawKeyEvent (
                                lapic_info[0].DPC_QUEUE.cache_chars[0], 
                                lapic_info[0].DPC_QUEUE.cache_chars[1], 
                                lapic_info[0].DPC_QUEUE.cache_chars[2], 
                                lapic_info[0].DPC_QUEUE.cache_chars[3]
                            );
                            break;

                        case 2000:
                            // x_panic("2000: DPC Via AP");
                            int mouse_event_id = 
                                ( lapic_info[0].DPC_QUEUE.cache_longs[0] & 0xFFFFFFFF); 
                            wmMouseEvent( 
                                mouse_event_id,  //event_id, 
                                lapic_info[0].DPC_QUEUE.cache_longs[1],  //long1, 
                                lapic_info[0].DPC_QUEUE.cache_longs[2]  //long2 
                            );
                            break;

                        case 3000:
                            break;

                        case 4000:
                            break;

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
    lapic_info[0].DPC_QUEUE.on = FALSE;
}


//
// $
// AP INITIALIZATION
//

// #todo
// We need One GDT per-core AND 
// One IDT per-core (in x86-64 SMP)
// #ps: We have support for GDT in C.

static void AP_kmain2(void)
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

// Talk with the BSP in order to identify the current AP.
// #ps: return the lapic info id, not the real hw cpu id.
    lapic_id = (int) __AP_BSP_handshake();

/*
    if (lapic_id <0 || lapic_id>NR_CPUS)
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
    ProcessorNumber = id;  // ID
    Status = (int) I_initialize_kernel(arch_type, ProcessorNumber);
    if (Status == FALSE){
        PROGRESS("on I_initialize_kernel()\n");
    }
    if (system_state == SYSTEM_ABORTED){
        PROGRESS("SYSTEM_ABORTED\n");
    }
*/


//
//
//

    // #test: Using DPC via AP.
    if (CONFIG_USE_DPC_VIA_AP == 1){
        __ap_DPC_experiment(lapic_id);
    }

///
//
//

    __ap_animation_experiment();

// Something went wrong with this AP.
// #todo:
// Call a system routine in order to report this.
AP_die:
    while (1){
        asm (" cli ");
        asm (" hlt ");
    };
}




void AP_kmain(void)
{
    // See kmain.c
    AP_kmain2();
}




