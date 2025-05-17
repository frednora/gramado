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


// kernel sysboltable address.
// Pointer for the table of function pointers
// exported by the base kernel.
unsigned long *kfunctions;
// Function Number
int FN_DIE=0;      //it works
int FN_PUTCHARK=1; //it works
int FN_REBOOT=2;   //it works
int FN_REFESHSCREEN=3;
int FN_PUTCHAR_FGCONSOLE=4;  //(1arg)
// #todo: Call dead thread collector, scheduler ...
// read flags
// read messages
// ...


// #todo: Move this to another place.
struct module_initialization_d  ModuleInitialization;

// =====================================================



int newm0_1001(void)
{
    if (ModuleInitialization.initialized != TRUE){
        goto fail;
    }
    newm0_print_string("newm0_1001: reason 1001\n");

    //for (i=0; i<100; i++)
    //    caller0( (unsigned long) kfunctions[FN_PUTCHARK] );

    //caller0( (unsigned long) kfunctions[FN_DIE] );
    //caller0( (unsigned long) kfunctions[FN_PUTCHARK] );
    //caller0( (unsigned long) kfunctions[FN_REBOOT] );
    //do_int3();
    //caller1( kfunctions[FN_PUTCHAR_FGCONSOLE], 'x');

// #testing printk
    long value = 1234;
    //printk("mod0.bin: Testing printk | value={%d} :)\n",
        //value);

// #testing reboot.
// ok, it's working.
    //printk("mod0.bin: Testing reboot via ports\n");
    //do_reboot();

// Done
    return TRUE;
fail:
    return FALSE;
}

// #todo: Why this name?
// Why this functions is here? Move it to another place.
// OUT: TRUE or FALSE.
int newm0_initialize(void)
{

// The kernel static entry point.
// #bugbug: It's not safe.
// We need a random address.
    unsigned char *k = (unsigned char *) 0x30001000;

// #test
// Lookup for "__GRAMADO__"
// see: head_64.asm
    register int i=0;
    int Found=0;  //FALSE
    unsigned long __function_table=0;

    ModuleInitialization.initialized = FALSE;

    for (i=0; i<100; i++)
    {
        if (k[i+0]  == '_' &&
            k[i+1]  == '_' &&
            k[i+2]  == 'G' &&
            k[i+3]  == 'R' &&
            k[i+4]  == 'A' &&
            k[i+5]  == 'M' &&
            k[i+6]  == 'A' &&
            k[i+7]  == 'D' &&
            k[i+8]  == 'O' &&
            k[i+9]  == '_' &&
            k[i+10] == '_')
        {
            Found = 1;
            // The function table starts here.
            __function_table = (unsigned long) &k[i+11];
        }
    };

// Symbol table 'exported' hehe by the kernel.
    //unsigned long *kfunctions = (unsigned long *) __function_table;
    kfunctions = (unsigned long *) __function_table;

// Reinitialize the indexes for safety
    FN_DIE=0;      //it works
    FN_PUTCHARK=1; //it works
    FN_REBOOT=2;   //it works
    FN_REFESHSCREEN=3;
    FN_PUTCHAR_FGCONSOLE=4;  //(1arg)
    // ...

// done?
    // TRUE
    if (Found==1)
    {
        ModuleInitialization.initialized = TRUE;

        //#debug
        //newm0_print_string("newm0_initialize: Initialized\n");

        return TRUE;
    }

fail:
    ModuleInitialization.initialized = FALSE;
    return FALSE;
}

// ---------------------------
// mmain:
// Called by kernel_start() in head.c
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
            // See: kstdio.c
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
            
            if (ModuleInitialization.initialized != TRUE)
            {
                // See: kstdio.c
                newm0_initialize();
            }
            if (ModuleInitialization.initialized == TRUE)
            {
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

