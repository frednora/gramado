// public.c
// Cross Pointers
// These functions are exported to the ring0 module.
// See: head_64.asm and mod.c
// Created by Fred Nora.

#include <kernel.h>


// 0
void xp_die(void)
{
    printk("xp_die:\n");
    keDie();
}

// 1
void xp_putchar_test(void)
{
    const int c = 'x';
    gramk_putchar_in_fgconsole(c);
}

// 2
void xp_reboot(void)
{
    keReboot();
}

// 3
void xp_refresh_screen(void)
{
    gramk_refresh_screen();
}

// 4
void xp_putchar_in_fgconsole(unsigned long _char)
{
    gramk_putchar_in_fgconsole(_char);
}

