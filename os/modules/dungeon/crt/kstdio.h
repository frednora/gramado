// kstdio.h
// Created by Fred Nora.

#ifndef __CRT_KSTDIO_H
#define __CRT_KSTDIO_H    1

void newm0_print_string (char *s);


char *kinguio_itoa (int val, char *str);

void 
kinguio_i2hex( 
    unsigned int val, 
    char *dest, 
    int len );

void kinguio_puts(const char* str);

int 
kinguio_vsprintf(
    char *str, 
    const char *fmt, 
    va_list ap );

int kinguio_printf(const char *fmt, ...);

int mysprintf(char *buf, const char *fmt, ...);

// --------------------
// Aliases
#define printk   kinguio_printf
#define ksprintf  mysprintf

#endif   


