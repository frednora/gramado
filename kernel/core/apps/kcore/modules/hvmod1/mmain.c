// mmain.c
// Generic kernel side component for
// the ring3 main subsystem.
// The main file for the ring 0 kernel module.
// The entry point is called by head.c
// #warning: 
// Do not use 'interrupts'. Do done make syscalls.
// Call the kernel symbols directly.
// Created by Fred Nora.

#include "kernel.h"

// ---------------------------
// mmain:
unsigned long 
mmain (
    unsigned char sc_id,   // system call id.
    unsigned long param1,  // reason
    unsigned long param2,  // long1
    unsigned long param3,  // long2
    unsigned long param4 ) // long3
{
// Called by kernel_start() in head.c

    unsigned long reason = (unsigned long) (param1 & 0xFFFF);
    int Status = -1;
    unsigned long ReturnValue;

// #todo
// sc_id means the id of the syscall used to
// request services of this module.
// Just some few values are allowed,
// because we only have some few syscall interrupts.
// But we can have some special values indicating that
// the module was called by the kernel or by another module.

    //#debug
    //if (sc_id == 0xFF)
        //printk ("[0xFF]: Called by kernel\n");

// Invalid reason
    if (reason < 0){
        goto fail;
    }

    switch (reason){

        // Initializing the module.
        case 1000:
            // Initialize the server functions.
            Status = (int) newm0_initialize();
            if (Status == TRUE){
                printk("Initialization OK\n");
                return 1234;
            }
            return (unsigned long) 0;
            break;

        // Testing printk function.
        case 1001:
            Status = (int) newm0_1001();
            if (Status == TRUE){
                return 1234;
            }
            return (unsigned long) 0;
            break;

        // The function table exported by the base kernel.
        case 1002:
            fn_table = (unsigned long *) param4;
            printk("fn_table {%x}\n", fn_table);
            return (unsigned long) 0;
            //while(1){}
            break;

        // The syscall entrypoint exported by the kernel.
        // This way we can call routines inside the kernel.
        case 1003:
            printk("mod_sci: {%x}\n", param4);
            sci_entry = param4;
            
            // Calling the kernel service.
            // see: kal.c
            // ok, it is working!
            //ReturnValue = (unsigned long) ke_sci(0,0,0,0);
            //printk("mmain.c: ReturnValue={%d} \n", ReturnValue);
            
            //printk("mmain.c: #breakpoint :)\n");
            //while (1){};
            return (unsigned long) 0;
            break;

        // :: Step1
        // Initialize display information
        // Called only by the display server.
        // see: display.c
        case 2001:
            return (unsigned long) display_initialization_phase1( 
                                       param1, 
                                       param2, 
                                       param3, 
                                       param4 );
            break;

        // :: Step2
        // Initialize displey information
        // Called only by the display server.
        // see: display.c
        case 2002:
            return (unsigned long) display_initialization_phase2( 
                                       param1, 
                                       param2, 
                                       param3, 
                                       param4 );
            break;

        // Testing the parameter list.
        case 8888:
            if (ModuleInitialization.initialized != TRUE){
                newm0_initialize();
            }
            if (ModuleInitialization.initialized == TRUE){
                printk("Parameters: %d | %d | %d | %d\n",
                    param1, param2, param3, param4 );
                return (unsigned long) 1234;
            }
            return 0;
            break;

        // Invalid reason.
        default:
            goto fail;
            break;
    };

fail:
    return (unsigned long) 0;
}

