// head.c
// crt0 file for a ring0 kernel module.
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
module_crt0 (
    unsigned char sc_id,  // syscall id
    unsigned long l1,   // Reason
    unsigned long l2,   // data 1
    unsigned long l3,   // data 2
    unsigned long l4 )  // data 3
{
    unsigned char id = (unsigned char) sc_id;
    unsigned long param1 = (unsigned long) l1; // Reason
    unsigned long param2 = (unsigned long) l2; // long1
    unsigned long param3 = (unsigned long) l3; // long2
    unsigned long param4 = (unsigned long) l4; // long3
    unsigned long return_value = 0;


// If it was not called by the kernel
// 0xFF is the kernel ID.

    if (id != 0xFF)
        return 0;

//
// Call module procedure
//

    return_value = 
        (unsigned long) mmain( id, param1, param2, param3, param4 );

    return (unsigned long) return_value;
}

