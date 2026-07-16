
// kal.h

#ifndef __KE_KAL_H
#define __KE_KAL_H    1

extern unsigned long *fn_table;
extern unsigned long *sci_entry;

void caller0(unsigned long function_address);

void 
caller1(
    unsigned long function_address, 
    unsigned long data0 );

unsigned long 
caller2(
    unsigned long function_address, 
    unsigned long data0 );

void *ke_sci ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d );

#endif   

