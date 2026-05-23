// printk.h
// Created by Fred Nora.

#ifndef __PRINTK_PRINTK_H
#define __PRINTK_PRINTK_H    1

// #suspensa
// Essa implementação foi feita para 32bit e não funciona
// por inteiro em long mode.
// Usaremos kinguio_printf por enquanto.
int printk_old(const char *format, ...);

// The ring 0 implementation of printf().
int kinguio_printf(const char *fmt, ...);

// ===========================================

// --------------------------------------
// The ring 0 implementation of printf().
#define printk  kinguio_printf

#endif   
