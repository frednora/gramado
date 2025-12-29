
// ioapic.c
// Document created by Fred Nora - (2023)
// Credits:
//   + Ported from Sirius-x86-64, created by Nelson Cole.

#include <kernel.h>

// Initialization control.
struct ioapic_info_d  IOAPIC;

#define IOAPIC_NULL  0

// #todo
// I/O APIC
// see: ioapic.h
struct __IOAPIC_REDIR_TBL ioapic_redir_table[24];

// #todo
static unsigned long ioapic_base=0;

// ----------------------------------------

static int 
__set_ioapic_redir_table(
    int n,
    unsigned char vector,
    unsigned char delv_mode,
    unsigned char dest_mode,
    unsigned char delv_status,
    unsigned char pin_polarity,
    unsigned char remote_IRR,
    unsigned char trigger_mode,
    unsigned char mask,
    unsigned char destination );

static unsigned int read_ioapic_register(int reg);
static void write_ioapic_register(int reg, unsigned int val);
static int __ioapic_initialize_redirection_table(int maximum_redirection);
static int __setup_ioapic(void);

// ----------------------------------------

// read
static unsigned int read_ioapic_register(int reg)
{
    if (IOAPIC.initialized != TRUE)
        panic("read_ioapic_register: depends on IOAPIC.initialized\n");


// The base is a local pointer.
    *(volatile unsigned char*)(ioapic_base + IO_APIC_IND) = reg;

    return ( *(volatile unsigned int*)(ioapic_base + IO_APIC_DAT));
}

// write
static void write_ioapic_register(int reg, unsigned int val)
{
    if (IOAPIC.initialized != TRUE)
        panic("write_ioapic_register: depends on IOAPIC.initialized\n");

// The base is a local pointer.
    *(volatile unsigned char*)(ioapic_base + IO_APIC_IND) = reg;
    *(volatile unsigned int*) (ioapic_base + IO_APIC_DAT) = val;
}



int ioapic_masked(int n)
{
// This pointer is define into this document.
    unsigned long pointer = (unsigned long) &ioapic_redir_table;

// Chech the range.
    if (n<0 || n>=24)
        panic("ioapic_masked: n\n");

    ioapic_redir_table[n].mask = 1;

    unsigned int *data = (unsigned int*) pointer;

    write_ioapic_register(
        IO_APIC_REDIR_TBL(n),
        *(unsigned int*)(data + (n *2)) );

    return 0;
}

int ioapic_umasked(int n)
{
// This pointer is define into this document.
    unsigned long pointer = (unsigned long) &ioapic_redir_table;

// Chech the range.
    if (n<0 || n>=24)
        panic("ioapic_umasked: n\n");

    ioapic_redir_table[n].mask = 0;

    unsigned int *data = (unsigned int*) pointer;
    write_ioapic_register(
        IO_APIC_REDIR_TBL(n),
        *(unsigned int*)(data + (n *2)) );

    return 0;
}

// Setup a given entry in the redirection table.
static int 
__set_ioapic_redir_table(
    int n,
    unsigned char vector,
    unsigned char delv_mode,
    unsigned char dest_mode,
    unsigned char delv_status,
    unsigned char pin_polarity,
    unsigned char remote_IRR,
    unsigned char trigger_mode,
    unsigned char mask,
    unsigned char destination)
{

// #warning
// The redirection table is define here in this document.
    unsigned long pointer = (unsigned long) &ioapic_redir_table;

    unsigned int *data = (unsigned int*) pointer;


// Invalid entry
// range: (0~23)
    if (n >= 24)
        panic("__set_ioapic_redir_table: n\n");


// Fill the entry.

// These are the fields for each entry.
// Each redirection entry is made of the following fields: 


// #todo
// Talvez devamos começar com a mesma interrupçao
// que começamos a configuraçao do pic. 'int 32' e nao 0x40 como no exemplo.

// Vector 0-7 
// The Interrupt vector that will be raised on the specified CPU(s). 
    ioapic_redir_table[n].vector = 
        vector;

// Delivery Mode 8 - 10 - How the interrupt will be sent to the CPU(s). 
//  It can be 000 (Fixed*), 
// 001 (Lowest Priority), 010 (SMI), 100 (NMI), 101 (INIT) and 111 (ExtINT).  
    ioapic_redir_table[n].delv_mode = 
        delv_mode;

// Destination Mode 11 
// Specify how the Destination field shall be interpreted. 
// 0: Physical Destination, 1: Logical Destination 
    ioapic_redir_table[n].dest_mode = 
        dest_mode;

// Delivery Status 12 
    ioapic_redir_table[n].delv_status = 
        delv_status;

// Pin Polarity 13 
// 0: Active high, 1: Active low.
    ioapic_redir_table[n].pin_polarity = 
        pin_polarity;

// Remote IRR 14 
    ioapic_redir_table[n].remote_IRR = 
        remote_IRR;

// Trigger Mode 15
// 0: Edge, 1: Level.  
    ioapic_redir_table[n].trigger_mode = 
        trigger_mode;

// Mask 16 
// Just like in the old PIC, you can temporary disable this IRQ by setting this bit, and 
// reenable it by clearing the bit. 
    ioapic_redir_table[n].mask = 
        mask;

// Destination 	56 - 63 
// You put here the APIC ID of the CPU that you want to receive the interrupt.
    ioapic_redir_table[n].destination = 
        destination;

// ------------------------------------
// see this macro in: ioapic.h
// #todo:
// Explain it better.
// IOREDTBL 
// There are two 32-bit register for each IRQ. 
// >> In the first of the two registers you access to the LOW uint32_t / bits 31:0, and 
// >> the second for the high uint32_t / 63:32. 


// bits 31:0
    write_ioapic_register(
        IO_APIC_REDIR_TBL(n), 
        *(unsigned int*)(data + (n *2)) );

// bits 63:32
    write_ioapic_register(
        IO_APIC_REDIR_TBL(n) + 1,
        *(unsigned int*)(data + (n *2) + 1) );

// done
    return 0;
}

static int __ioapic_initialize_redirection_table(int maximum_redirection)
{
// Initialize the redirection table.
// Called by __setup_ioapic().

    // 24
    int Max = (int) maximum_redirection;

    if (Max < 0)
       panic("__ioapic_initialize_redirection_table: Max\n");
// Invalid range
    if (Max > 24)
        panic("__ioapic_initialize_redirection_table: Max\n");


    if (LAPIC.initialized != TRUE)
        panic("__ioapic_initialize_redirection_table: It depends on LAPIC.initialized\n");

    unsigned int id = (unsigned int) LAPIC.local_id;

// --------------------------
// Install 24 entries
// starting at '32'. Why? 
// maybe 32, that is where the IRQs starts in Gramado.
// The purpose here is only setup the redirection table.
// changing the values for the 24 entries.

// 0x00 --> 32
// 0x01 --> 33
// 0x02 --> 34
// ...

    unsigned char First = 32;

    register int i=0;
    for (i=0; i<Max; i++) 
    {
            //if (i == 11){
            //}
            // else
            
            // Escrene na tabela e configura registradores.
	        __set_ioapic_redir_table(
	            i,         // IRQn
	            (First + i),  // vector
	            0,         // Delivery Mode
	            0,         // Destination Mode
	            0,         // RO
	            0,         // Interrupt Input Pin Polarity
	            0,         // RO
	            0,
	            1,         // masked 
	            id );      // lapic id.

    };

    return 0;
}


static int __setup_ioapic(void)
{
// Called by enable_ioapic().

    if (IOAPIC.initialized != TRUE)
        panic("__setup_ioapic: It depends on IOAPIC.initialized\n");


// -------------------------
// Save the virtual address of the base of the registers 
// into a local pointer.
    ioapic_base = (unsigned long) IOAPIC.ioapic_va;
    if (ioapic_base == 0)
        panic("__setup_ioapic: ioapic_base\n");


// -------------------------
// Get the 'Maximum redirection' entry.
// We're gonna use it bellow.
// IOAPICVER 
// The Max Redirection Entry which is "how many IRQs can this I/O APIC handle - 1". 
// It is encoded in bits 16 - 23. 

    int MaximumRedirection = 
        ((read_ioapic_register(IO_APIC_VER) >> 16) & 0xff) + 1;

// -------------------------
// Set the id of the lapic.
    if (LAPIC.initialized != TRUE)
        panic("__setup_ioapic: It depends on LAPIC.initialized\n");
    unsigned int id = (unsigned int) LAPIC.local_id;
    write_ioapic_register(IO_APIC_ID,id);

// -------------------------
// Fill the table.
// Out of range
    if (MaximumRedirection > 24)
        panic("__setup_ioapic: MaximumRedirection\n");

// Initialize the redirection table.
    __ioapic_initialize_redirection_table(MaximumRedirection);

    return 0;
}

void enable_ioapic(void)
{
// Called by I_kmain() in kmain.c.

    printk("enable_ioapic:\n");

// It depends on LAPIC.
    if (LAPIC.initialized != TRUE)
        panic ("enable_ioapic: It depends on LAPIC.initialized\n");

// Not initialized yet.
    IOAPIC.initialized = FALSE;

// ===================
// Mapping

// page table
// Isso serve pra pegarmos um endereço físico
// que servira de base para criarmos uma pagetable.
// Mas endereço físico e virtual são iguais nessa região.
// Identidade 1:1.
    unsigned long *pt_ioapic = 
        (unsigned long *) get_table_pointer_va();

// -------------
// pa
// #warning: Hard coded.
// see: ioapic.h
    IOAPIC.ioapic_pa = (unsigned long) (IO_APIC_BASE & 0xFFFFFFFF);
// -------------
// va
// see: x64gva.h
    IOAPIC.ioapic_va = (unsigned long) IOAPIC_VA;

// -------------------------------------
// Mapping area for registers.

    int map_status = -1;
    map_status = 
        (int) mm_map_2mb_region(
            IOAPIC.ioapic_pa,
            IOAPIC.ioapic_va );
    if (map_status != 0)
        panic("enable_ioapic: on mm_map_2mb_region()\n");

// OK, the structure is initialized,
// now this module can use the values in it.
    IOAPIC.initialized = TRUE;

//==========================================

//
// Initialize the interrupts.
//
    __setup_ioapic();
}

