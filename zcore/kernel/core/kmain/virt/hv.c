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
int hv_is_qemu(void)
{
    return (int) isQEMU();
}

// Enable ps2 keyboard and mouse.
void hv_ps2_full_initialization(void)
{
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
    int status = -1;
    if (fInitialize == TRUE)
    {
        status = (int) DDINIT_ps2();
        if (status<0)
            panic("hv_ps2_full_initialization: DDINIT_ps2");
    }
        
}

// #todo: Used during the initialization.
// called by I_init() in x64init.c
int hv_probe_info(void)
{
// Probe if we are already running on a hypervisor.
// see: detect.c
    return (int) __detect_hv();
}

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

