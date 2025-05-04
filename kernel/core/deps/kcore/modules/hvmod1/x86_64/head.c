// head.c
// Created by Fred Nora.

#include <kernel.h>


// crt0 file for a ring0 kernel module.
extern unsigned long 
mmain (
    unsigned char sc_id,   // system call id.
    unsigned long param1,  // reason
    unsigned long param2,  // long1
    unsigned long param3,  // long2
    unsigned long param4 ); // long3

const char *crt0_args[] = {
    "one",
    "two",
    NULL
};

// ----------------------------------
// Entry point
unsigned long  
kernel_start(
    unsigned char sc_id,  // syscall id
    unsigned long l1,   // Reason
    unsigned long l2,   // data 1
    unsigned long l3,   // data 2
    unsigned long l4 )  // data 3
{
    unsigned char id = (unsigned char) sc_id;
    unsigned long param1 = (unsigned long) l1;
    unsigned long param2 = (unsigned long) l2;
    unsigned long param3 = (unsigned long) l3;
    unsigned long param4 = (unsigned long) l4;
    unsigned long return_value=0;

// #todo
// sc_id means the id of the syscall used to
// request services of this module.
// Just some few values are allowed,
// because we only have some few syscall interrupts.
// But we can have some special values indicating that
// the module was called by the kernel or by another module.


// sc81 and sc83
    if ( id == 0x81 ||
         id == 0x83 )
    {
        return_value = 
            (unsigned long) mmain(
                                id, 
                                param1, param2, param3, param4 );
    }

// #test #todo
// Other options
// FF - Called by kernel itself.
    if ( id == 0xFF )
    {
        return_value = 
            (unsigned long) mmain(
                                id, 
                                param1, param2, param3, param4 );
    }

/*
// Undefined
    if ( id == 0x00 )
    {
        return_value = 
            (unsigned long) mmain(
                                id, 
                                param1, param2, param3, param4 );
    }
*/

    return (unsigned long) return_value;
}

