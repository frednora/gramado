// detect.c
// Created by Fred Nora.

#include <kernel.h>


int syscall_is_supported = FALSE;



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

// ------------------


// Check if SYSCALL is supported
int check_syscall_support(void)
{
    unsigned int eax=0;
    unsigned int ebx=0;
    unsigned int ecx=0;
    unsigned int edx=0;

    cpuid(0x80000001, eax, ebx, ecx, edx);  // Extended feature flags

    return (edx & (1 << 11)) ? 1 : 0;  // Check if bit 11 in EDX is set
}

int probe_if_cpu_has_support_to_syscall(void) 
{
    syscall_is_supported = FALSE;

    if (check_syscall_support()) {
        syscall_is_supported = TRUE;
        printk("SYSCALL is supported!\n");
    } else {
        syscall_is_supported = FALSE;
        printk("SYSCALL is NOT supported!\n");
    }
    return 0;
}

// ------------------

#define __IA32_EFER 0xC0000080  // MSR for enabling SYSCALL/SYSRET

void syscall_enable(void) 
{
    uint32_t lo, hi;

    // Read IA32_EFER
    cpuGetMSR(__IA32_EFER, &lo, &hi);

    // Check if SYSCALL is already enabled
    if (!(lo & 1)) {
        lo |= 1;  // Set bit 0 (SCE - SYSCALL Enable)
        cpuSetMSR(__IA32_EFER, lo, hi);  // Write back to IA32_EFER
    }
}


#define __SYSCALL_REG_LSTAR 0xC0000082  // MSR for syscall entry point
#define __SYSCALL_REG_FMASK 0xC0000084  // MSR for syscall flag mask

// Set the syscall entry point (correctly handling 64-bit addresses)
void syscall_set_entry_point(uint64_t ent) 
{
    uint32_t alpha = (uint32_t)(ent & 0xFFFFFFFF);  // Lower 32 bits
    uint32_t beta = (uint32_t)(ent >> 32);         // Upper 32 bits
    cpuSetMSR(__SYSCALL_REG_LSTAR, alpha, beta);
}

// Set the syscall mask (disables specific flags during syscall)
void syscall_set_mask(uint32_t mask) 
{
    uint32_t alpha = mask;
    uint32_t beta = 0;
    cpuSetMSR(__SYSCALL_REG_FMASK, alpha, beta);
}

// See: sw2.asm
extern unsigned long syscall_handler;
void initialize_syscall(void) 
{
    // Step 1: Check if SYSCALL is supported
    if (!check_syscall_support()) 
    {
        printk("SYSCALL is NOT supported on this CPU!\n");
        return;
    }

    // Step 2: Enable SYSCALL/SYSRET in IA32_EFER
    syscall_enable();

    // Step 3: Set the syscall entry point (handler address)
    syscall_set_entry_point((uint64_t)syscall_handler);

    // Step 4: Set the syscall mask (disable specific flags)
    syscall_set_mask(0x3F);  // Example mask (disable interrupts)

    printk("SYSCALL setup complete!\n");
}


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

/*
 * hal_hardware_detect:
 *     Detecta fabricantes específicos suportados pelo núcleo.
 * 8086, 1237  //PCI & Memory.
 * 8086, 7000  //PIIX3 PCI-to-ISA Bridge (Triton II).
 * 1022, 2000  //Advanced Micro Devices, PCnet LANCE PCI Ethernet Controller.
 * 8086, 7113  //PIIX4/4E/4M Power Management Controller.
 * 8086, 2829  //Intel(R) ICH8M SATA AHCI Controller.
 * //...
 *
 */
// Consumer Chipsets (Z87, H87, H81) Haswell LGA1150. 
// Business Chipsets (Q87, Q85, B85) Haswell LGA1150.

int hal_hardware_detect (void)
{
    debug_print ("hal_hardware_detect: [TODO]\n");
    return 0;    //#todo
}

