// pic.h
// 8259A PIC controller device driver.
// 2015 - Created by Fred Nora.


#ifndef ____PIC_H
#define ____PIC_H    1

/* i8259A PIC registers */
/*
#define PIC_MASTER_CMD		0x20
#define PIC_MASTER_IMR		0x21
#define PIC_SLAVE_CMD		0xa0
#define PIC_SLAVE_IMR		0xa1
*/

/* 8259A interrupt controller ports. */
//#define INT_CTL         0x20	/* I/O port for interrupt controller */
//#define INT_CTLMASK     0x21	/* setting bits in this port disables ints */
//#define INT2_CTL        0xA0	/* I/O port for second interrupt controller */
//#define INT2_CTLMASK    0xA1	/* setting bits in this port disables ints */


// Estrutura para gerenciar as irqs do PIC.
struct irq_d
{
    //object_type_t objectType;
    //object_class_t objectClass;
    int used;
    int magic;
    int id;

    unsigned long irq_address;      //(hardware.s)
    unsigned long handler_address;  
    //...

    char *name; //description.

    struct irq_d  *next;
};


struct pic_d
{
    object_type_t objectType;
    object_class_t objectClass;
    int dummy;
    // 8 entries for pic controller.
};
// #todo
// Put this in pic.c and create an external refrence here.
//struct pic_d *Pic;

//
// == IRQ Handlers =======================================
//

// See: x86/pit.c
__VOID_IRQ 
irq0_TIMER(void);

// See: keyboard.c
__VOID_IRQ 
irq1_KEYBOARD(void);

// See: x86/rtc.c
__VOID_IRQ 
irq8_RTC(void); 

// See: e1000.c
__VOID_IRQ 
irq_E1000(void);

// See: mouse.c
__VOID_IRQ 
irq12_MOUSE(void);

// See: pci.c
// Each one will be shared by up to 4 devices.
__VOID_IRQ 
irq_SHARED0(void);
__VOID_IRQ 
irq_SHARED1(void);
__VOID_IRQ 
irq_SHARED2(void);
__VOID_IRQ 
irq_SHARED3(void);

// See: atairq.c
__VOID_IRQ 
irq14_PRIMARY_IDE(void);
__VOID_IRQ 
irq15_SECONDARY_IDE(void);

#endif    

