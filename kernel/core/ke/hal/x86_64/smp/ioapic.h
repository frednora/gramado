
// ioapic.h

#ifndef __HAL_IOAPIC_H
#define __HAL_IOAPIC_H    1

/*
// ioapic
//I/O APIC Memory mapped window
#define IOAPIC_REG		0xFEC00000 //U8
#define IOAPIC_DATA		0xFEC00010 //U32
//I/O APIC Registers
#define IOAPICID		0x00
#define IOAPICVER		0x01
#define IOAPICARB		0x02
#define IOREDTAB		0x10
*/

// #warning: Hard coded.
#define IO_APIC_BASE    0xFEC00000

#define IO_APIC_IND  0
#define IO_APIC_DAT   (IO_APIC_IND +0x10)
#define IO_APIC_EOIR  (IO_APIC_IND +0x40)

#define IO_APIC_ID   0x0
#define IO_APIC_VER  0x1
#define IO_APIC_ARB  0x2
#define IO_APIC_REDIR_TBL(n)  \
    (0x10 + 2 * n)  //REDIR_TBL[0:23] 64 bits 



// Initialization control.
// The base address to accesses IOAPIC.
struct ioapic_info_d
{
    int initialized;
    unsigned long ioapic_va;
    unsigned long ioapic_pa; 
    int entry;  // pagedirectory entry.
};
extern struct ioapic_info_d  IOAPIC;



struct __IOAPIC_REDIR_TBL 
{

// 64 bit?
    unsigned long vector        : 8;
    unsigned long delv_mode     : 3;
    unsigned long dest_mode     : 1;
    unsigned long delv_status   : 1;
    unsigned long pin_polarity  : 1;
    unsigned long remote_IRR    : 1;
    unsigned long trigger_mode  : 1;
    unsigned long mask          : 1;
    unsigned long rsv           : 39;
    unsigned long destination   : 8;

}__attribute__((packed));



int ioapic_masked(int n);
int ioapic_umasked(int n);

void enable_ioapic(void);

#endif   

