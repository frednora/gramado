// x64.h
// Created by Fred Nora.

#ifndef ____X64_H
#define ____X64_H    1

// Interrupt vectors for x64.

/*
#define X64_INTERRUPT_VECTOR_TIMER      32
#define X64_INTERRUPT_VECTOR_KEYBOARD   33
#define X64_INTERRUPT_VECTOR_RTC        40
#define X64_INTERRUPT_VECTOR_MOUSE      44
#define X64_INTERRUPT_VECTOR_IDE0       46
#define X64_INTERRUPT_VECTOR_IDE1       47
#define X64_INTERRUPT_VECTOR_SYSTEM0   128   //0x80
#define X64_INTERRUPT_VECTOR_SYSTEM1   129 
#define X64_INTERRUPT_VECTOR_SYSTEM2   133 
#define X64_INTERRUPT_VECTOR_SYSTEM3   213 
#define X64_INTERRUPT_VECTOR_SYSTEM4   216 
// ...
*/

// CR0 bits
#define CPUX64_CR0_PG  (1 << 31)
#define CPUX64_CR0_CD  (1 << 30)
#define CPUX64_CR0_NW  (1 << 29)
//...

// CR4 bits
#define CPUX64_CR4_MPE  (1 << 11)
#define CPUX64_CR4_PCE  (1 <<  8)
#define CPUX64_CR4_PGE  (1 <<  7)
#define CPUX64_CR4_PAE  (1 <<  5)
#define CPUX64_CR4_PSE  (1 <<  4)
//...


/*
 * 386 processor status longword.
 * credits: netBSD
 */

#define PSL_C		0x00000001	/* carry flag */
#define PSL_PF		0x00000004	/* parity flag */
#define PSL_AF		0x00000010	/* auxiliary carry flag */
#define PSL_Z		0x00000040	/* zero flag */
#define PSL_N		0x00000080	/* sign flag */
#define PSL_T		0x00000100	/* trap flag */
#define PSL_I		0x00000200	/* interrupt enable flag */
#define PSL_D		0x00000400	/* direction flag */
#define PSL_V		0x00000800	/* overflow flag */
#define PSL_IOPL	0x00003000	/* i/o privilege level */
#define PSL_NT		0x00004000	/* nested task */
#define PSL_RF		0x00010000	/* resume flag */
#define PSL_VM		0x00020000	/* virtual 8086 mode */
#define PSL_AC		0x00040000	/* alignment check flag */
#define PSL_VIF		0x00080000	/* virtual interrupt enable flag */
#define PSL_VIP		0x00100000	/* virtual interrupt pending flag */
#define PSL_ID		0x00200000	/* identification flag */

#define PSL_MBO		0x00000002	/* must be one bits */
#define PSL_MBZ		0xffc08028	/* must be zero bits */

#define PSL_USERSET  \
    (PSL_MBO | PSL_I)

#define PSL_USERSTATIC  \
    (PSL_MBO | PSL_MBZ | PSL_I | PSL_IOPL | PSL_NT | PSL_VM | PSL_VIF | PSL_VIP)

#define PSL_USER  \
    (PSL_C | PSL_PF | PSL_AF | PSL_Z | PSL_N | PSL_T | PSL_V | PSL_D | PSL_AC)

#define PSL_CLEARSIG  \
    (PSL_T | PSL_VM | PSL_AC | PSL_D)

//
// == prototypes ============================================
//


//
// FPU
//

void x64_init_fpu(void);
int x64_init_fpu_support(void);

void fpu_load_control_word(const uint16_t control);

void x64_disable_interrupts(void);
void x64_enable_interrupts(void);

void x64_iretq(void);
void x64_iret(void);
void x64_lret(void);

void x64_cli_hlt(void);
void x64_stop_cpu(void);

//
// Cache
//

void x64_enable_cache(void);
int x64_init_intel(void);

// Show info
void x64_info(void);

void x64_load_pml4_table(unsigned long phy_addr);

// see: x64nmi.c 
void x64_all_faults(unsigned long number);


#endif    

