
// tss.h

#ifndef __TSS_H
#define __TSS_H    1


// See:
// https://wiki.osdev.org/Task_State_Segment

typedef struct tss_d {

    unsigned int reserved;
    
    unsigned long rsp0; 
    unsigned long rsp1; 
    unsigned long rsp2;

    unsigned int reserved2[2];

    unsigned long ist1; 
    unsigned long ist2; 
    unsigned long ist3; 
    unsigned long ist4; 
    unsigned long ist5; 
    unsigned long ist6; 
    unsigned long ist7;

    unsigned int reserved3[2];

    unsigned short reserved4;
    unsigned short IOPB_offset;

} __attribute__((packed)) tagTSS;

// #todo
// Put this in tss.c and put here an external reference.
struct tss_d  *CurrentTSS;

/*
 * I/O bitmap offset beyond TSS's segment limit means no bitmaps.
 * (i.e. any I/O attempt generates an exception.)
 */

#define IOMAP_INVALOFF  0xffffu

/*
 * If we have an I/O bitmap, there is only one valid offset.
 */
#define IOMAP_VALIDOFF  sizeof(struct tss_d)


//
// == prototypes ===========================
//

void 
tss_init( 
    struct tss_d *tss, 
    void *stack_address );

void x64_load_ltr(int tr);

#endif    

