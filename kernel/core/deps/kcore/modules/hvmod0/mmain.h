// mmain.h

#ifndef __MMAIN_H
#define __MMAIN_H    1


// kernel sysboltable address.
// Pointer for the table of function pointers
// exported by the base kernel.
extern unsigned long *kfunctions;
extern int FN_DIE;      //it works
extern int FN_PUTCHARK; //it works
extern int FN_REBOOT;   //it works
extern int FN_REFESHSCREEN;
extern int FN_PUTCHAR_FGCONSOLE;  //(1arg)
// #todo: Call dead thread collector, scheduler ...
// read flags
// read messages
// ...


struct module_initialization_d
{
    int initialized;
    //unsigned long ksysmboltable_address;
};
extern struct module_initialization_d  ModuleInitialization;


// ============================

int newm0_1001(void);
int newm0_initialize(void);

#endif   

