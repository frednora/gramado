
// detect.c

#include <kernel.h>

/*
 * hal_probe_cpu:
 *     Detectar qual é o tipo de processador. 
 *     Salva o tipo na estrutura.
 * #todo: Estamos usando cpuid para testar os 2 tipos de arquitetura.
 * nao sei qual ha instruções diferentes para arquiteturas diferentes.
 */
int hal_probe_cpu(void)
{
    unsigned int eax=0;
    unsigned int ebx=0;
    unsigned int ecx=0;
    unsigned int edx=0;

    debug_print ("hal_probe_cpu:\n");

// Check processor structure.
    if ((void *) processor == NULL){
        x_panic("hal_probe_cpu: [FAIL] processor\n");
    }

// Unknown processor type.
    processor->Type = Processor_NULL;

//
// Check vendor
//

    cpuid( 0, eax, ebx, ecx, edx ); 

// TestIntel: Confere se é intel.
    if ( ebx == CPUID_VENDOR_INTEL_1 && 
         edx == CPUID_VENDOR_INTEL_2 && 
         ecx == CPUID_VENDOR_INTEL_3 )
    {
        processor->Type = Processor_INTEL;
        return 0;
    }

// TestAmd: Confere se é AMD.
    if ( ebx == CPUID_VENDOR_AMD_1 && 
         edx == CPUID_VENDOR_AMD_2 && 
         ecx == CPUID_VENDOR_AMD_3 )
    {
        processor->Type = Processor_AMD;
        return 0;
    }
    
// fail:
    x_panic("hal_probe_cpu: [FAIL] Processor not supported\n");
    return (int) (-1);
}

/*
 * hal_probe_processor_type:
 *     Sonda pra ver apenas qual é a empresa do processador.
 */
// Called by I_initKernelComponents() in x64init.c.c
int hal_probe_processor_type (void)
{
    unsigned int eax=0;
    unsigned int ebx=0;
    unsigned int ecx=0;
    unsigned int edx=0;
    unsigned int name[32];
    int MASK_LSB_8 = 0xFF;  

    //debug.
    debug_print("hal_probe_processor_type:\n");
    //printk("Scaning x86 CPU ...\n");

// Check processor structure.
    if ((void *) processor == NULL){
        x_panic("hal_probe_processor_type: processor\n");
    }

// Unknown processor type.
    // processor->Type = Processor_NULL;

// vendor
// This is the same for intel and amd processors.
    cpuid( 0, eax, ebx, ecx, edx ); 
    name[0] = ebx;
    name[1] = edx;
    name[2] = ecx;
    name[3] = 0;

// Salva na estrutura.
    processor->Vendor[0] = ebx;
    processor->Vendor[1] = edx;
    processor->Vendor[2] = ecx;
    processor->Vendor[3] = 0;

// #hackhack
// #fixme
// Na verdade quando estamos rodando no qemu, é amd.
// #todo: Precisamos do nome certo usado pelo qemu.

    return Processor_AMD;

    /*
    // Confere se é Intel.
    if ( ebx == CPUID_VENDOR_INTEL_1 && 
         edx == CPUID_VENDOR_INTEL_2 && 
         ecx == CPUID_VENDOR_INTEL_3 )
    {
        return (int) Processor_INTEL; 
    }

    // Confere se é AMD
    if ( ebx == CPUID_VENDOR_AMD_1 && 
         edx == CPUID_VENDOR_AMD_2 && 
         ecx == CPUID_VENDOR_AMD_3 )
    {
        return (int) Processor_AMD; 
    }
    */

// Continua...

    return (int) Processor_NULL;
}

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

/* Example: Get CPU's model number */
/*
static int get_model(void);
static int get_model(void)
{
    int ebx, unused;
    cpuid(0, unused, ebx, unused, unused);
    return ebx;
}
*/

/* Example: Check for builtin local APIC. */
/*
//  CPUID_FEAT_EDX_APIC = 1 << 9,  
static int check_apic(void);
static int check_apic(void)
{
    unsigned int eax, unused, edx;
    cpuid(1, &eax, &unused, &unused, %edx);
    return edx & CPUID_FEAT_EDX_APIC;
}
*/

