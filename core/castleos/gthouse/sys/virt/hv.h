// hv.h
// Hypervisor support.
// Created by Fred Nora.

// #remember: 
// Little endian?

#ifndef __VIRT_HV_H
#define __VIRT_HV_H    1


struct vmx_info_d
{
// The structure is fully initialized
    int struct_initialized;

    int support_confirmed_via_cpuid; // CPUID.01h:ECX[5]
    int support_confirmed_via_msr;   // IA32_FEATURE_CONTROL MSR (0x3A)

    // Final verdict: usable or not
    // Enabled (both CPUID and MSR say yes).
    int enabled;


// If cpuid=0 → unsupported.
// If cpuid=1 and msr=0 → disabled by BIOS.
// If cpuid=1 and msr=1 → enabled.

    // ...
};

struct svm_info_d
{
// The structure is fully initialized
    int struct_initialized;

    int support_confirmed_via_cpuid; // CPUID.80000001h:ECX[2]
    int support_confirmed_via_msr;   // IA32_EFER MSR (0xC0000080), bit 12

    // Final verdict: usable or not
    // Enabled (both CPUID and MSR say yes).
    int enabled;

// If cpuid=0 → unsupported.
// If cpuid=1 and msr=0 → disabled by BIOS.
// If cpuid=1 and msr=1 → enabled.

    // ...
};


// ----------------------------
// "ACRNACRNACRN"
// A Big Little Hypervisor for IoT Development.
// 0x4E524341, 0x4E524341, 0x4E524341
#define HV_STRING_ACRN_PART1  0x4E524341
#define HV_STRING_ACRN_PART2  0x4E524341
#define HV_STRING_ACRN_PART3  0x4E524341

// -------------------
// apple
// "GenuineIntel" – Apple Rosetta 2[8]
// "VirtualApple" – Newer versions of Apple Rosetta 2


// ----------------------------
// "bhyve bhyve "
// The BSD hypervisor.
// 0x76796862, 0x68622065, 0x20657679
#define HV_STRING_BHYVE_PART1  0x76796862
#define HV_STRING_BHYVE_PART2  0x68622065
#define HV_STRING_BHYVE_PART3  0x20657679

// ----------------------------
// "Microsoft Hv" – Microsoft Hyper-V or Windows Virtual PC
// #todo
//#define HV_STRING_HYPERV_PART1  0x00000000
//#define HV_STRING_HYPERV_PART2  0x00000000
//#define HV_STRING_HYPERV_PART3  0x00000000


// ----------------------------
// "KVMKVMKVM   "  – KVM 
#define HV_STRING_KVM_PART1  0x4B4D564B  // KMVK
#define HV_STRING_KVM_PART2  0x564B4D56  // VKMV
#define HV_STRING_KVM_PART3  0x0000004D  //    M

// ----------------------------
// QEMU hypervisor (Emulator)
// Tiny Code Generator (TCG).
// "TCGTCGTCGTCG" – QEMU
#define HV_STRING_TCG_PART1  0x54474354
#define HV_STRING_TCG_PART2  0x43544743
#define HV_STRING_TCG_PART3  0x47435447

// ----------------------------
// QNX Hypervisor
// "QNXQVMBSQG  "
// 0x51584E51, 0x53424D56, 0x00004751
#define HV_STRING_QNX_PART1  0x51584E51
#define HV_STRING_QNX_PART2  0x53424D56
#define HV_STRING_QNX_PART3  0x00004751

// ----------------------------
// "VMwareVMware" – VMware
// #todo
//#define HV_STRING_VMWARE_PART1  0x00000000
//#define HV_STRING_VMWARE_PART2  0x00000000
//#define HV_STRING_VMWARE_PART3  0x00000000


// ----------------------------
// "XenVMMXenVMM" – Xen HVM
// #todo
//#define HV_STRING_XEN_PART1  0x00000000
//#define HV_STRING_XEN_PART2  0x00000000
//#define HV_STRING_XEN_PART3  0x00000000

// =============================================

#define HV_TYPE_UNDEFINED  0
#define HV_TYPE_TCG        1000  // qemu, emulated mode.
#define HV_TYPE_KVM        1001  // qemu, virtualbox.
#define HV_TYPE_BHYVE      1002
#define HV_TYPE_QNX        1003
#define HV_TYPE_ACRN       1004
// ...

struct hv_d
{
// The structure is fully initializzed
    int initialized;

// Hypervisor type
    int type;

// from cpuid:
// Hypervisor name string.
    unsigned int hvName[4];

// from cpuid:
// The memory limits in this machine.
    unsigned int Physical_Address_Size;
    unsigned int Virtual_Address_Size;
};

// see: detect.c
extern struct hv_d  HVInfo;

// -------------------------------

int hv_probe_info(void);
int isQEMU(void);
int hv_is_qemu(void);

void hv_ps2_full_initialization(void);

// via cpuid. See: x64.c
int hv_is_vmx_supported(void);
int hv_is_svm_supported(void);

// via MSR. See: hv.c
int hv_detect_vmx(void);
int hv_detect_svm(void);

int enable_Intel_VMX(void);
int disable_Intel_VMX(void);
int enable_AMD_SVM(void);
int disable_AMD_SVM(void);


#endif   

