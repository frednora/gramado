


// IRQ vectors

// 0~31  - system traps and exceptions
// 32~47 - IRQs for PIC mode.
// 128   - systemcall 0x80
// 129   - systemcall 0x81
// 130   - systemcall 0x82
// 198   - Callback restorer
// 199   - Called by crt0 during the app initialization.


#define SYSCALL1_VECTOR  0x80
#define SYSCALL2_VECTOR  0x81
#define SYSCALL3_VECTOR  0x82

//#todo
//#define SPURIOUS_APIC_VECTOR		0xff
