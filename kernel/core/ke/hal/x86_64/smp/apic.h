/*
 * File: apic.h
 * Descrição:
 *     Header para o driver do controlador APIC.
 *     Advanced Programmable Interrupt Controller
 *     Exemplo:
 *     82489DX.
 *     82093AA for PIIX3-based systems.
 * Variantes:
 *    xAPIC, x2APIC.
 * 2015 - Created by Fred Nora.
 */

// See:
// https://opensource.apple.com/source/xnu/xnu-2050.24.15/osfmk/i386/lapic.h.auto.html


// APICs (both local and I/O) are memory mapped devices. 
// The default location for the local APIC 
// is at 0xfee00000 in physical memory. 


#ifndef ____APIC_H
#define ____APIC_H    1


/*
    LAPIC registers. 
Software interacts with the local APIC by reading and writing its registers.
APIC registers are memory-mapped to a 4-KByte region of the 
processor’s physical address space with an initial 
starting address of FEE00000H. 
For correct APIC operation, this address space must be mapped to 
an area of memory that has been designated as strong uncacheable (UC).
(Credits: terenceli)
The default memory mapped address of the LAPIC 
is 0xFEE00000 but should be read from the MADT or MSRs. 
Each CPU has it's own LAPIC mapped at this location,
meaning CPUs cannot change each other's LAPICs.
(Credits: https://nemez.net/osdev/lapic.txt)
See:
losethos os - Adam1a.HPP.
// #todo: Vamos precisar de um endereço virtual para acessarmos isso.
*/

// BDA base address.
#define BDA_BASE  0x040E
// MP signature. "_MP_".
#define MP_SIG  0x5F504D5F

// Base physical address.
#define LAPIC_BASE  0xFEE00000

// Offsets:
#define LAPIC_APIC_ID                0x020
#define LAPIC_APIC_VERSION           0x030
#define LAPIC_TASK_PRIORITY          0x080
#define LAPIC_ARIBITRATION_PRIORITY  0x090
#define LAPIC_PROCESSOR_PRIORITY     0x0A0
#define LAPIC_EOI                    0x0B0
#define LAPIC_LOG_DST                0x0D0
#define LAPIC_LDR                    0x0D0
#define LAPIC_DFR                    0x0E0
#define LAPIC_SVR                    0x0F0

#define LAPIC_ISR          0x100
#define LAPIC_TMR          0x180

#define LAPIC_IRR          0x200

#define LAPIC_ICR_LOW      0x300
#define LAPIC_ICR_HIGH     0x310
#define LAPIC_LVT_TIMER    0x320
#define LAPIC_LVT_THERMAL  0x330
#define LAPIC_LVT_PERF     0x340
#define LAPIC_LVT_LINT0    0x350
#define LAPIC_LVT_LINT1    0x360
#define LAPIC_LVT_ERR      0x370

#define LAPIC_INITIAL_COUNT_TIMER 	0x380
#define LAPIC_CURRENT_COUNT_TIMER	0x390
#define LAPIC_DIVIDE_TIMER		    0x3e0


// ====================

//
// ICR - Interrupt Command Register
//

// Delivery Mode
#define ICR_FIXED                       0x00000000
#define ICR_LOWEST                      0x00000100
#define ICR_SMI                         0x00000200
#define ICR_NMI                         0x00000400
#define ICR_INIT                        0x00000500
#define ICR_STARTUP                     0x00000600

// Destination Mode
#define ICR_PHYSICAL                    0x00000000
#define ICR_LOGICAL                     0x00000800

// Delivery Status
#define ICR_IDLE                        0x00000000
#define ICR_SEND_PENDING                0x00001000

// Level
#define ICR_DEASSERT                    0x00000000
#define ICR_ASSERT                      0x00004000

// Trigger Mode
#define ICR_EDGE                        0x00000000
#define ICR_LEVEL                       0x00008000

// Destination Shorthand
#define ICR_NO_SHORTHAND                0x00000000
#define ICR_SELF                        0x00040000
#define ICR_ALL_INCLUDING_SELF          0x00080000
#define ICR_ALL_EXCLUDING_SELF          0x000c0000





// ====================

//#define LAPICF_APIC_ENABLED		0x100


#define MPN_VECT		0x7F
#define MP_VECT_ADDR	(MPN_VECT*0x1000)

// msr
// Model specific registers.
#define IA32F_SCE	0x001
#define IA32F_LME	0x100
#define IA32_LAPIC_BASE	0x01B
#define IA32_EFER	0xC0000080
#define IA32_FS_BASE	0xC0000100
#define IA32_GS_BASE	0xC0000101


/*
typedef enum {
	ID			= 0x02,
	VERSION			= 0x03,
	TPR			= 0x08,
	APR			= 0x09,
	PPR			= 0x0A,
	EOI			= 0x0B,
	REMOTE_READ		= 0x0C,
	LDR			= 0x0D,
	DFR			= 0x0E,
	SVR			= 0x0F,
	ISR_BASE		= 0x10,
	TMR_BASE		= 0x18,
	IRR_BASE		= 0x20,
	ERROR_STATUS		= 0x28,
	LVT_CMCI		= 0x2F,
	ICR			= 0x30,
	ICRD			= 0x31,
	LVT_TIMER		= 0x32,
	LVT_THERMAL		= 0x33,
	LVT_PERFCNT		= 0x34,
	LVT_LINT0		= 0x35,
	LVT_LINT1		= 0x36,
	LVT_ERROR		= 0x37,
	TIMER_INITIAL_COUNT	= 0x38,
	TIMER_CURRENT_COUNT	= 0x39,
	TIMER_DIVIDE_CONFIG	= 0x3E,
} lapic_register_t;
 
See: apple opensource
*/


// Initialization control.
// The base address by which each processor accesses
// its local APIC.
struct lapic_info_d
{
    int initialized;
    unsigned long lapic_va;
    unsigned long lapic_pa; 
    int entry;  // pagedirectory entry.

    int local_id;
    int local_version;
};
extern struct lapic_info_d  LAPIC;


// =======================

// Check presence of apic.
int has_apic (void);
void cpu_set_apic_base(unsigned long apic);
unsigned long cpu_get_apic_base(void); 
void lapic_initializing(unsigned long lapic_pa);


void enable_apic(void); 


void local_apic_send_init(unsigned int apic_id);
void local_apic_send_startup(unsigned int apic_id, unsigned int vector);
void Send_INIT_IPI_Once(unsigned int apic_id);
void Send_STARTUP_IPI_Twice(unsigned int apic_id);

#endif    //____APIC_H

//
// End.
//

