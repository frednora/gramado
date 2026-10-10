// hv.c
// Hypervisor support.
// The main goal here is ahndle the tables and 
// provide communication between the root partition and
// baremetal hv.
// Provide information about the hv if we're running on top of one,
// even if it is a type 1 or type 2 hv.
// see:
// virt/hv.h
// https://nixhacker.com/developing-hypervisior-from-scratch-part-1
// https://www.codeproject.com/Articles/215458/Virtualization-for-System-Programmers

/*
Common Instructions

VMXON    - Enable VMX
VMXOFF   - Disable VMX
VMLAUNCH - Start/enter VM
VMRESUME - Re-enter VM
VMCLEAR  - Null out/reinitialize VMCS
VMPTRLD  - Load the current VMCS
VMPTRST  - Store the current VMCS
VMREAD   - Read values from VMCS
VMWRITE  - Write values to VMCS
VMCALL   - Exit virtual machine to VMM
VMFUNC   - Invoke a VM function in VMM without exiting guest operation
*/

#include <kernel.h>


// see: virt/hv.h
struct hv_d  HVInfo;


// ===================================

static int __detect_hv(void);

// ===================================

// #todo: Used during the initialization.
static int __detect_hv(void)
{
    register int i=0;

    HVInfo.initialized = FALSE;
    HVInfo.type = HV_TYPE_UNDEFINED;

// #todo #bugbug
// This structure is for the current processor?
// on Intel only?

    if ((void *) processor == NULL){
        x_panic("detect_hv: processor struct\n");
    }

//
// Memory limits
//

    HVInfo.Physical_Address_Size = 
        (unsigned int) processor->Physical_Address_Size; 
    HVInfo.Virtual_Address_Size = 
        (unsigned int) processor->Virtual_Address_Size;

//
// name string
//

    // Copy 4 integers
    for (i=0; i<4; i++){
        HVInfo.hvName[i] = (unsigned int) processor->hvName[i];
    };

//
// type
//

    // #bugbug
    // This structure needs to be initialized.
    if ((void*) processor == NULL)
        panic("detect_hv: processor\n");
    if (processor->magic != 1234)
        panic("detect_hv: processor validation\n");

// TCG
    if ( processor->hvName[0] == HV_STRING_TCG_PART1 &&
         processor->hvName[1] == HV_STRING_TCG_PART2 &&
         processor->hvName[2] == HV_STRING_TCG_PART3 )
    {
        HVInfo.type = HV_TYPE_TCG;
        //panic ("HV_TYPE_TCG");
        goto done;
    }


// KVM
    if ( processor->hvName[0] == HV_STRING_KVM_PART1 &&
         processor->hvName[1] == HV_STRING_KVM_PART2 &&
         processor->hvName[2] == HV_STRING_KVM_PART3 )
    {
        HVInfo.type = HV_TYPE_KVM;
        //panic ("HV_TYPE_KVM");
        goto done;
    }


// bhyve?
    if ( processor->hvName[0] == HV_STRING_BHYVE_PART1 &&
         processor->hvName[1] == HV_STRING_BHYVE_PART2 &&
         processor->hvName[2] == HV_STRING_BHYVE_PART3 )
    {
        HVInfo.type = HV_TYPE_BHYVE;
        //panic ("HV_TYPE_BHYVE");
        goto done;
    }
// qnx?
    if ( processor->hvName[0] == HV_STRING_QNX_PART1 &&
         processor->hvName[1] == HV_STRING_QNX_PART2 &&
         processor->hvName[2] == HV_STRING_QNX_PART3 )
    {
        HVInfo.type = HV_TYPE_QNX;
        //panic ("HV_TYPE_QNX");
        goto done;
    }
// acrn?
    if ( processor->hvName[0] == HV_STRING_ACRN_PART1 &&
         processor->hvName[1] == HV_STRING_ACRN_PART2 &&
         processor->hvName[2] == HV_STRING_ACRN_PART3 )
    {
        HVInfo.type = HV_TYPE_ACRN;
        //panic ("HV_TYPE_ACRN");
        goto done;
    }

fail:
    HVInfo.initialized = FALSE;
    return FALSE;
done:
    HVInfo.initialized = TRUE;
    return TRUE;
}

// #todo: make this static and local. __isQEMU.
int isQEMU(void)
{
    if (HVInfo.initialized == TRUE)
    {
        if (HVInfo.type == HV_TYPE_TCG){
            return TRUE;
        }
    }
    return FALSE;
}

// Wrapper
int hv_is_qemu(void){
    return (int) isQEMU();
}

// Enable ps2 keyboard and mouse.
// Initialize it only for qemu.
void hv_ps2_full_initialization(void)
{
    int status = -1;
    int fInitialize = FALSE;

    if (HVInfo.initialized != TRUE)
        return;

    //#debug
    //printk("hv type: %d\n", HVInfo.type);

// ---------------
// TCG - qemu
    if (HVInfo.type == HV_TYPE_TCG){
        fInitialize = TRUE;
    }

// ---------------
// KVM - qemu, virtualbox.
    if (HVInfo.type == HV_TYPE_KVM){
        fInitialize = TRUE;
    }

    // ...

// Do
    if (fInitialize == TRUE)
    {
        status = (int) DDINIT_ps2();
        if (status<0)
            panic("hv_ps2_full_initialization: DDINIT_ps2");
    }
        
}

// #todo: Used during the initialization.
// called by halInitialize() in hal.c
int hv_probe_info(void)
{
// Probe if we are already running on a hypervisor.
// see: detect.c
    return (int) __detect_hv();
}

// Intel (VMX)
int hv_is_vmx_supported(void)
{
    int Status = -1;

    // #bugbug
    // This structure needs to be initialized.
    if ((void*) processor == NULL)
        panic("hv_is_vmx_supported: processor\n");
    if (processor->magic != 1234)
        panic("hv_is_vmx_supported: processor validation\n");


    // Supported
    if (processor->hasVMX == 1){
        Status = TRUE;
        goto done;
    // Not supported
    }else if (processor->hasVMX != 1){
        Status = FALSE;
        goto done;
    };

    panic("hv_is_vmx_supported: Fail\n");
    // Not reached.
    return (int) Status;

done:
    return (int) Status;
}

// AMD (SVM)
int hv_is_svm_supported(void)
{
    int Status = -1;

    // #bugbug
    // This structure needs to be initialized.
    if ((void*) processor == NULL)
        panic("hv_is_svm_supported: processor\n");
    if (processor->magic != 1234)
        panic("hv_is_svm_supported: processor validation\n");


    // Supported
    if (processor->hasSVM == 1){
        Status = TRUE;
        goto done;
    // Not supported
    }else if (processor->hasSVM != 1){
        Status = FALSE;
        goto done;
    };

    panic("hv_is_svm_supported: Fail\n");
    // Not reached.
    return (int) Status;

done:
    return (int) Status;
}

// Detect if Intel VMX is available and enabled
// via IA32_FEATURE_CONTROL MSR (0x3A)
int hv_detect_vmx(void)
{
    unsigned int lo=0, hi=0;
    uint64_t feature_control=0;

    // First check CPUID capability
    //if (processor->hasVMX != 1) {
    //    return FALSE; // Not supported at all
    //}

    // Read IA32_FEATURE_CONTROL MSR (0x3A)
    cpuGetMSR(0x3A, &lo, &hi);
    feature_control = ((uint64_t)hi << 32) | lo;

    // Bit 0 = Lock, Bit 2 = VMX enabled outside SMX
    if ((feature_control & 0x1) && !(feature_control & (1 << 2))) {
        // Locked but disabled
        return FALSE;
    }

    // If we reach here, VMX is usable
    return TRUE;
}

// Detect if AMD SVM is available and enabled
// via EFER MSR (0xC0000080), bit 12 = SVME enable
int hv_detect_svm(void)
{
    unsigned int lo=0, hi=0;
    uint64_t efer=0;

    //if (processor->hasSVM != 1) {
    //    return FALSE; // Not supported
    //}

    // Read EFER MSR
    cpuGetMSR(0xC0000080, &lo, &hi);
    efer = ((uint64_t)hi << 32) | lo;

    // Bit 12 = SVME enable
    if (!(efer & (1 << 12))) {
        return FALSE; // Disabled
    }

    return TRUE;
}


/*
 * enable_Intel_VMX:
 * -----------------
 * Purpose:
 *   Enable Intel VMX (Virtual Machine Extensions) so the CPU can enter
 *   virtualization mode and execute VMX instructions (VMXON, VMLAUNCH, etc).
 *
 * Steps:
 *   1. Check CPUID.01h:ECX[bit 5] to confirm VMX capability.
 *   2. Read IA32_FEATURE_CONTROL MSR (0x3A):
 *        - Bit 0 = Lock bit (BIOS may lock this MSR).
 *        - Bit 2 = VMX enabled outside SMX.
 *      If locked and disabled, VMX cannot be enabled.
 *   3. Set CR4.VMXE (bit 13) to allow VMX instructions.
 *
 * Returns:
 *   TRUE if VMX is enabled and usable, FALSE otherwise.
 */
int enable_Intel_VMX(void)
{
    unsigned int lo=0, hi=0;
    uint64_t feature_control=0;

    // Check CPUID capability
    //if (processor->hasVMX != 1) return FALSE;

    // Read IA32_FEATURE_CONTROL MSR (0x3A)
    cpuGetMSR(0x3A, &lo, &hi);
    feature_control = ((uint64_t)hi << 32) | lo;

    // Must be locked and VMX enabled outside SMX
    if ((feature_control & 0x1) && !(feature_control & (1 << 2))) {
        return FALSE; // Disabled by BIOS
    }

    // Set CR4.VMXE (bit 13)
    unsigned long cr4;
    asm volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 13);
    asm volatile ("mov %0, %%cr4" :: "r"(cr4));

    return TRUE;
}

/*
 * disable_Intel_VMX:
 * ------------------
 * Purpose:
 *   Disable Intel VMX operation and return CPU to normal mode.
 *
 * Steps:
 *   1. Clear CR4.VMXE (bit 13).
 *   2. Execute VMXOFF instruction to exit VMX root operation.
 *
 * Notes:
 *   VMXOFF must be executed before clearing CR4.VMXE.
 *   If VMXON was never executed, VMXOFF will #UD fault.
 *
 * Returns:
 *   TRUE once VMX is disabled.
 */
int disable_Intel_VMX(void)
{
    unsigned long cr4;
    asm volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 &= ~(1 << 13); // Clear VMXE
    asm volatile ("mov %0, %%cr4" :: "r"(cr4));

    // Execute VMXOFF to leave VMX operation
    asm volatile ("vmxoff");

    return TRUE;
}

/*
 * enable_AMD_SVM:
 * ----------------
 * Purpose:
 *   Enable AMD SVM (Secure Virtual Machine) so the CPU can run virtualization
 *   instructions (VMRUN, etc).
 *
 * Steps:
 *   1. Check CPUID.80000001h:ECX[bit 2] to confirm SVM capability.
 *   2. Read IA32_EFER MSR (0xC0000080).
 *   3. Set SVME bit (bit 12) in EFER to enable SVM.
 *
 * Returns:
 *   TRUE if SVM is enabled and usable, FALSE otherwise.
 */
int enable_AMD_SVM(void)
{
    unsigned int lo=0, hi=0;
    uint64_t efer=0;

    if (processor->hasSVM != 1) return FALSE;

    // Read EFER MSR (0xC0000080)
    cpuGetMSR(0xC0000080, &lo, &hi);
    efer = ((uint64_t)hi << 32) | lo;

    // Set SVME bit (bit 12)
    efer |= (1 << 12);
    lo = (unsigned int)(efer & 0xFFFFFFFF);
    hi = (unsigned int)(efer >> 32);
    cpuSetMSR(0xC0000080, lo, hi);

    return TRUE;
}

/*
 * disable_AMD_SVM:
 * -----------------
 * Purpose:
 *   Disable AMD SVM operation and return CPU to normal mode.
 *
 * Steps:
 *   1. Read IA32_EFER MSR (0xC0000080).
 *   2. Clear SVME bit (bit 12).
 *
 * Notes:
 *   Once cleared, SVM instructions (VMRUN, etc) will #UD fault.
 *
 * Returns:
 *   TRUE once SVM is disabled.
 */
int disable_AMD_SVM(void)
{
    unsigned int lo=0, hi=0;
    uint64_t efer=0;

    cpuGetMSR(0xC0000080, &lo, &hi);
    efer = ((uint64_t)hi << 32) | lo;

    // Clear SVME bit (bit 12)
    efer &= ~(1 << 12);
    lo = (unsigned int)(efer & 0xFFFFFFFF);
    hi = (unsigned int)(efer >> 32);
    cpuSetMSR(0xC0000080, lo, hi);

    return TRUE;
}






/*
// Looking for CPUID.1:ECX.VMX[bit 5] = 1
int vmxSupport(void)
{
    unsigned int getVmxSupport=0;
    int vmxBit=0;

    asm volatile ("mov $1, %rax");
    asm volatile ("cpuid");
    asm volatile ("mov %%ecx, %0 \n\t" : "=r"(getVmxSupport) );

    // Get bit
    vmxBit = (getVmxSupport >> 5) & 1;

    if (vmxBit == 1){
        return TRUE;
    } else {
        return FALSE;
    }
}
*/

/*
// Before entering vmxon we need to set few bits in CRX registers 
// for different purposes. The first bit we need to set is bit 13 in CR4 register.
#define X86_CR4_VMXE_BIT  13  //enable VMX virtualization 
#define X86_CR4_VMXE  _BITUL(X86_CR4_VMXE_BIT)
int getVmxOperation(void)
{
    unsigned long cr4=0;
    // setting CR4.VMXE[bit 13] = 1
    asm volatile ("mov %%cr4, %0" : "=r"(cr4) : : "memory");
    cr4 |= X86_CR4_VMXE;
    asm volatile ("mov %0, %%cr4" : : "r"(cr4) : "memory");
}
*/

/*
VMXON can also be controlled by IA32_FEATURE_CONTROL in MSR. 
* It maybe disabled which can stop processor from entering vmx mode. 
* We need to extract the value of IA32_FEATURE_CONTROL and set it 
* accordingly to support vmxon.
*/

// To execute VMXON we need to set the Bit 0(Lock bit) and Bit 2(VMX outside SMX).

