// printk.h

#ifndef __PRINTK_PRINTK_H
#define __PRINTK_PRINTK_H    1

// #suspensa
// Essa implementação foi feita para 32bit e não funciona
// por inteiro em long mode.
// Usaremos kinguio_printf por enquanto.
int printk_old( const char *format, ... );

int kinguio_printf(const char *fmt, ...);


// ===========================================

// -----------------------------------
// printf() in ring0.
#define printk  kinguio_printf


#endif   

