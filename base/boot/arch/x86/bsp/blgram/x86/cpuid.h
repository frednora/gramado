
// cpuid.h 
// Created by Fred Nora.
// See:
// https://en.wikipedia.org/wiki/CPUID
// https://wiki.osdev.org/CPUID


#ifndef ____CPUID_H
#define ____CPUID_H    1

//cpuid.
#define cpuid( in, a, b, c, d)\
    asm ("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

// Vendor Strings. 
#define CPUID_VENDOR_INTEL         "GenuineIntel"  // Intel.
#define CPUID_VENDOR_AMD           "AuthenticAMD"  // AMD.
#define CPUID_VENDOR_OLDAMD        "AMDisbetter!"  // AMD.
#define CPUID_VENDOR_VIA           "CentaurHauls"  // VIA.
#define CPUID_VENDOR_SIS           "SiS SiS SiS "  // SIS.
#define CPUID_VENDOR_OLDTRANSMETA  "TransmetaCPU"
#define CPUID_VENDOR_TRANSMETA     "GenuineTMx86"
#define CPUID_VENDOR_CYRIX         "CyrixInstead"
#define CPUID_VENDOR_CENTAUR       "CentaurHauls"
#define CPUID_VENDOR_NEXGEN        "NexGenDriven"
#define CPUID_VENDOR_UMC           "UMC UMC UMC "
#define CPUID_VENDOR_NSC           "Geode by NSC"
#define CPUID_VENDOR_RISE          "RiseRiseRise"
//...

//Intel.
#define CPUID_VENDOR_INTEL_1  0x756e6547    //"Genu" 
#define CPUID_VENDOR_INTEL_2  0x49656e69    //"ineI" 
#define CPUID_VENDOR_INTEL_3  0x6c65746e    //"ntel" 

//AMD.
#define CPUID_VENDOR_AMD_1    0x68747541    //"Auth" 
#define CPUID_VENDOR_AMD_2    0x69746e65    //"enti" 
#define CPUID_VENDOR_AMD_3    0x444d4163    //"cAMD"


//" KVMKVMKVM " – KVM
//"TCGTCGTCGTCG" – QEMU
//"Microsoft Hv" – Microsoft Hyper-V or Windows Virtual PC
//"VMwareVMware" – VMware
// ...

// QEMU hypervisor
// invertido
// "TCGTCGTCGTCG"
#define CPUID_HV_QEMU_1  0x54474354  // 0x54434754  TCGT
#define CPUID_HV_QEMU_2  0x43544743  // 0x43475443  CGTC
#define CPUID_HV_QEMU_3  0x47435447  // 0x47544347  GTCG



//
// CPU Features
// When called with EAX = 1 (CPUID_GETFEATURES), CPUID returns a bit field 
// in EDX containing the following values. (osdev)
//

/*

enum {
    CPUID_FEAT_ECX_SSE3         = 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,  
    CPUID_FEAT_ECX_VMX          = 1 << 5,  
    CPUID_FEAT_ECX_SMX          = 1 << 6,  
    CPUID_FEAT_ECX_EST          = 1 << 7,  
    CPUID_FEAT_ECX_TM2          = 1 << 8,  
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,  
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13, 
    CPUID_FEAT_ECX_ETPRD        = 1 << 14, 
    CPUID_FEAT_ECX_PDCM         = 1 << 15, 
    CPUID_FEAT_ECX_DCA          = 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20, 
    CPUID_FEAT_ECX_x2APIC       = 1 << 21, 
    CPUID_FEAT_ECX_MOVBE        = 1 << 22, 
    CPUID_FEAT_ECX_POPCNT       = 1 << 23, 
    CPUID_FEAT_ECX_AES          = 1 << 25, 
    CPUID_FEAT_ECX_XSAVE        = 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27, 
    CPUID_FEAT_ECX_AVX          = 1 << 28,
 
    CPUID_FEAT_EDX_FPU          = 1 << 0,  
    CPUID_FEAT_EDX_VME          = 1 << 1,  
    CPUID_FEAT_EDX_DE           = 1 << 2,  
    CPUID_FEAT_EDX_PSE          = 1 << 3,  
    CPUID_FEAT_EDX_TSC          = 1 << 4,  
    CPUID_FEAT_EDX_MSR          = 1 << 5,  
    CPUID_FEAT_EDX_PAE          = 1 << 6,  
    CPUID_FEAT_EDX_MCE          = 1 << 7,  
    CPUID_FEAT_EDX_CX8          = 1 << 8,  
    CPUID_FEAT_EDX_APIC         = 1 << 9,  
    CPUID_FEAT_EDX_SEP          = 1 << 11, 
    CPUID_FEAT_EDX_MTRR         = 1 << 12, 
    CPUID_FEAT_EDX_PGE          = 1 << 13, 
    CPUID_FEAT_EDX_MCA          = 1 << 14, 
    CPUID_FEAT_EDX_CMOV         = 1 << 15, 
    CPUID_FEAT_EDX_PAT          = 1 << 16, 
    CPUID_FEAT_EDX_PSE36        = 1 << 17, 
    CPUID_FEAT_EDX_PSN          = 1 << 18, 
    CPUID_FEAT_EDX_CLF          = 1 << 19, 
    CPUID_FEAT_EDX_DTES         = 1 << 21, 
    CPUID_FEAT_EDX_ACPI         = 1 << 22, 
    CPUID_FEAT_EDX_MMX          = 1 << 23, 
    CPUID_FEAT_EDX_FXSR         = 1 << 24, 
    CPUID_FEAT_EDX_SSE          = 1 << 25, 
    CPUID_FEAT_EDX_SSE2         = 1 << 26, 
    CPUID_FEAT_EDX_SS           = 1 << 27, 
    CPUID_FEAT_EDX_HTT          = 1 << 28, 
    CPUID_FEAT_EDX_TM1          = 1 << 29, 
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31
};
*/


// ecx
#define  CPUID_FEAT_ECX_SSE3         (1 << 0)
#define  CPUID_FEAT_ECX_PCLMUL       (1 << 1)
#define  CPUID_FEAT_ECX_DTES64       (1 << 2)
#define  CPUID_FEAT_ECX_MONITOR      (1 << 3)
#define  CPUID_FEAT_ECX_DS_CPL       (1 << 4)
#define  CPUID_FEAT_ECX_VMX          (1 << 5)
#define  CPUID_FEAT_ECX_SMX          (1 << 6)
#define  CPUID_FEAT_ECX_EST          (1 << 7)
#define  CPUID_FEAT_ECX_TM2          (1 << 8)
#define  CPUID_FEAT_ECX_SSSE3        (1 << 9)
#define  CPUID_FEAT_ECX_CID          (1 << 10)
#define  CPUID_FEAT_ECX_FMA          (1 << 12)
#define  CPUID_FEAT_ECX_CX16         (1 << 13)
#define  CPUID_FEAT_ECX_ETPRD        (1 << 14)
#define  CPUID_FEAT_ECX_PDCM         (1 << 15)
#define  CPUID_FEAT_ECX_DCA          (1 << 18)
#define  CPUID_FEAT_ECX_SSE4_1       (1 << 19)
#define  CPUID_FEAT_ECX_SSE4_2       (1 << 20)
#define  CPUID_FEAT_ECX_x2APIC       (1 << 21)
#define  CPUID_FEAT_ECX_MOVBE        (1 << 22)
#define  CPUID_FEAT_ECX_POPCNT       (1 << 23)
#define  CPUID_FEAT_ECX_AES          (1 << 25)
#define  CPUID_FEAT_ECX_XSAVE        (1 << 26)
#define  CPUID_FEAT_ECX_OSXSAVE      (1 << 27)
#define  CPUID_FEAT_ECX_AVX          (1 << 28)

//edx
#define  CPUID_FEAT_EDX_FPU          (1 << 0)
#define  CPUID_FEAT_EDX_VME          (1 << 1)
#define  CPUID_FEAT_EDX_DE           (1 << 2)
#define  CPUID_FEAT_EDX_PSE          (1 << 3)
#define  CPUID_FEAT_EDX_TSC          (1 << 4)
#define  CPUID_FEAT_EDX_MSR          (1 << 5)
#define  CPUID_FEAT_EDX_PAE          (1 << 6)
#define  CPUID_FEAT_EDX_MCE          (1 << 7)
#define  CPUID_FEAT_EDX_CX8          (1 << 8)
#define  CPUID_FEAT_EDX_APIC         (1 << 9)
#define  CPUID_FEAT_EDX_SEP          (1 << 11)
#define  CPUID_FEAT_EDX_MTRR         (1 << 12)
#define  CPUID_FEAT_EDX_PGE          (1 << 13)
#define  CPUID_FEAT_EDX_MCA          (1 << 14)
#define  CPUID_FEAT_EDX_CMOV         (1 << 15)
#define  CPUID_FEAT_EDX_PAT          (1 << 16)
#define  CPUID_FEAT_EDX_PSE36        (1 << 17)
#define  CPUID_FEAT_EDX_PSN          (1 << 18)
#define  CPUID_FEAT_EDX_CLF          (1 << 19)
#define  CPUID_FEAT_EDX_DTES         (1 << 21)
#define  CPUID_FEAT_EDX_ACPI         (1 << 22)
#define  CPUID_FEAT_EDX_MMX          (1 << 23)
#define  CPUID_FEAT_EDX_FXSR         (1 << 24)
#define  CPUID_FEAT_EDX_SSE          (1 << 25)
#define  CPUID_FEAT_EDX_SSE2         (1 << 26)
#define  CPUID_FEAT_EDX_SS           (1 << 27)
#define  CPUID_FEAT_EDX_HTT          (1 << 28)
#define  CPUID_FEAT_EDX_TM1          (1 << 29)
#define  CPUID_FEAT_EDX_IA64         (1 << 30)
#define  CPUID_FEAT_EDX_PBE          (1 << 31)

#endif    // ____CPUID_H 


