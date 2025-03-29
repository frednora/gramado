
// head.h

#ifndef __HEAD_H
#define __HEAD_H  1

// Entry point
unsigned long  
kernel_start(
    unsigned char sc_id,  // syscall id
    unsigned long l1,   // Reason
    unsigned long l2,   // data 1
    unsigned long l3,   // data 2
    unsigned long l4 );  // data 3

#endif  


