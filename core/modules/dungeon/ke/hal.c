
// hal.c
// Created by Fred Nora.

#include <kernel.h>

void do_hlt(void)
{
    asm ("hlt");
}

void do_int3(void)
{
    asm ("int $3");
}

void do_reboot(void)
{
    unsigned char good=0;

    printk ("newm0: do_reboot\n");

// Wait
    good = 0x02;
    while (good & 0x02){
        good = in8(0x64);
    };

    printk ("newm0: Go!\n");
    out8(0x64, 0xFE);

//fail:
    printk ("newm0: Fail\n");
    while (1){
        asm (" cli ");
        do_hlt();
    };
}


