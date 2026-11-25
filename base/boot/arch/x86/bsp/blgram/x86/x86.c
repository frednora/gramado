// x86.c
// Low level routines for x86 boot loader.
// 2005 - Created by Fred Nora.

#include "../bl.h"


// Enable interrupts
void enable()
{
    asm (" sti ");
}

// Disable interrupts
void disable()
{
    asm (" cli ");
}

// CLI/HLT
void stopCpu()
{
    asm ( " cli \n\t" 
          " hlt ");
}

void intReturn()
{
    asm (" iret ");
}

void farReturn()
{
    asm (" lret ");
}

// #todo: 
// Rever isso.
int getFlags (int variable) 
{
    asm (" pushfl \n\t"   
         " popl %0"       
         : "=r" (variable) );
}

// #todo: 
// Rever isso.
int setFlags (int variable)
{
    asm (" pushl %0 \n\t"   
         " popfl"           
          : : "r" (variable) );
}

int Push (int value)
{
    //__asm ("pushl %0" : : "r" (value) : "%esp" );
}

int Pop (int variable)
{
    //__asm ("popl %0" : "=r" (variable) : : "%esp" );
}

void pushRegs()
{
    //__asm ("pushal" : : : "%esp");
}

void popRegs()
{
    //__asm ("popal" : : : "%esp");
}

void pushFlags()
{
    //__asm ("pushfl" : : : "%esp" );
}

void popFlags()
{
    //__asm ("popfl" : : : "%esp" );
}

int getStackPointer (int addr)
{
    asm (" movl %%esp, %0" : "=r" (addr) );
}

int setStackPointer (int addr)
{
    //__asm ("movl %0, %%esp" : : "r" (addr) : "%esp" );
}


// MSR
// Accessing Model Specific Registers 
//Each MSR that is accessed by the RDMSR and WRMSR group of instructions 
//is identified by a 32-bit integer. MSRs are 64-bit wide. 
//The presence of MSRs on your processor is 
//indicated by CPUID.01h:EDX[bit 5].

const unsigned long CPUID_FLAG_MSR = (1 << 5);
 
int cpuHasMSR (void)
{
    unsigned long a=0;
    unsigned long b=0;
    unsigned long c=0;
    unsigned long d=0;

    cpuid(1, a, b, c, d);
    return (int) (d & CPUID_FLAG_MSR);
}

void 
cpuGetMSR ( 
    unsigned long msr, 
    unsigned long *lo, 
    unsigned long *hi )
{
    asm volatile ("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void 
cpuSetMSR ( 
    unsigned long msr, 
    unsigned long lo, 
    unsigned long hi )
{
    asm volatile ("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

//
// End
//

