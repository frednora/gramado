
// kal.c
// Kernel Abstraction Layer.

#include <kernel.h>


// Pointer for the table of function pointers
// exported by the base kernel.
unsigned long *fn_table;
unsigned long *sci_entry;


// Call a function inside the base kernel
// with no parameters.
void caller0(unsigned long function_address)
{
    asm volatile ("call *%0" : : "r"(function_address));
}

// Call a function inside the base kernel
// with only one parameter.
void 
caller1(
    unsigned long function_address, 
    unsigned long data0 )
{
// #test
// Using 'unsigned long' for everything.

    unsigned long x = (unsigned long ) (data0 & 0xFF);
    unsigned long y = (unsigned long ) (data0 & 0xFF);

    asm volatile ( "movq %1, %%rax;"
                   "movq %%rax, %0;"
                   "movq %%rax, %%rdi;"
                   :"=r"(y)  // output
                   :"r"(x)   // input
                   :"%rax"   // clobbered register
    );

    asm volatile ("call *%0" : : "r"(function_address));
}


// Call a function inside the base kernel
// with only one parameter.
unsigned long 
caller2(
    unsigned long function_address, 
    unsigned long data0 )
{
// #test
// Using 'unsigned long' for everything.

    //unsigned long x = (unsigned long ) (data0 & 0xFF);
    //unsigned long y = (unsigned long ) (data0 & 0xFF);
    unsigned long x = (unsigned long ) data0;
    unsigned long __Return=0;

    asm volatile ( "movq %1, %%rax;"
                   "movq %%rax, %0;"
                   "movq %%rax, %%rdi;"
                   :"=r"(__Return)  // output
                   :"r"(x)   // input
                   :"%rax"   // clobbered register
    );
    asm volatile ( "call *%0" 
                   : "=r"(__Return)  // output
                   : "r"(function_address));

    return (unsigned long) __Return;
}

// #warning
// We have a set of rules for the 
// services we can call.
void *ke_sci ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// 'Function calls'.

    unsigned long Lmessage_buffer[8];

    if ((void*) sci_entry == NULL)
        goto fail;
    unsigned long address = (unsigned long) sci_entry;

// Populate the message buffer.
    Lmessage_buffer[0] = (unsigned long) a;  // Number
    Lmessage_buffer[1] = (unsigned long) b;  // param
    Lmessage_buffer[2] = (unsigned long) c;  // param
    Lmessage_buffer[3] = (unsigned long) d;  // param

// #todo:
// We need a return for this function.
// IN: 
//   + The pointer for the sci interface inside the base kernel.
//   + The pointer for the message buffer here inside the module.

    unsigned long ret_val;

    ret_val = 
      (unsigned long) caller2( 
        address, 
        &Lmessage_buffer[0] ); 

// #todo:
// We need a return for this function.
    return (void*) ret_val;

fail:
    return NULL;
}





