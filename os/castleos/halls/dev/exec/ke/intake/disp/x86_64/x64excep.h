// x64excep.h 

#ifndef __DISP_X64EXCEP_H
#define __DISP_X64EXCEP_H    1


// #todo
// Its a way to profile the exceptions and traps in a more organized way.
// Using counters and flags.
struct exception_info_d 
{
    int initialized;   // The structure is initialized
    // int counter[32];
    // ...

    struct thread_d *faulty_thread;
    struct te_d *faulty_process;
    // int isKernel;
};
extern struct exception_info_d ExceptionInfo;


// =========================================

void x64excep_all_exceptions(unsigned long number);

#endif   

