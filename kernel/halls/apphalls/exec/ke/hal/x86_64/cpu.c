//  cpu.c
// Created by Fred Nora.


#include <kernel.h>


// ====================================
// MSR
// Accessing 'Model Specific Registers'
// Each MSR that is accessed by the 
// RDMSR and WRMSR group of instructions 
// is identified by a 32-bit integer. 
// MSRs are 64-bit wide. 
// The presence of MSRs on your processor is indicated 
// by CPUID.01h:EDX[bit 5].
// See: cpu.h
const unsigned int CPUID_FLAG_MSR = (1 << 5);


int cpuHasMSR(void)
{
    unsigned int a=0;
    unsigned int b=0;
    unsigned int c=0;
    unsigned int d=0;

    cpuid(1,a,b,c,d);
    return (int) (d & CPUID_FLAG_MSR);
}

void 
cpuGetMSR ( 
    unsigned int msr, 
    unsigned int *lo, 
    unsigned int *hi )
{
    asm volatile ("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void 
cpuSetMSR ( 
    unsigned int msr, 
    unsigned int lo, 
    unsigned int hi )
{
    asm volatile ("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

