// blmain.c

#include "kstdarg.h"

#define UART_BASE   0x09000000UL
#define UARTDR      0x00
#define UARTFR      0x18
#define UARTFR_TXFF 0x20

// Write one character to UART
void printchar(char c) {
    volatile unsigned int *uartfr = (unsigned int *)(UART_BASE + UARTFR);
    volatile unsigned int *uartdr = (unsigned int *)(UART_BASE + UARTDR);

    // Wait until TX FIFO not full
    while (*uartfr & UARTFR_TXFF) {
        // spin until ready
    }

    // Write character
    *uartdr = (unsigned int)c;
}

// Write a null-terminated string to UART
void printstring(const char *s) {
    while (*s) {
        printchar(*s++);
    }
}


// Print decimal integer
void printint(int value) {
    char buf[16];
    int i = 0;

    if (value == 0) {
        printchar('0');
        return;
    }

    if (value < 0) {
        printchar('-');
        value = -value;
    }

    while (value > 0 && i < 15) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0) {
        printchar(buf[--i]);
    }
}

// Print hexadecimal integer
void printhex(unsigned int value) {
    const char *digits = "0123456789ABCDEF";
    printstring("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        printchar(digits[(value >> shift) & 0xF]);
    }
}


void printf_s(const char *fmt, const char *s) {
    while (*fmt) {
        if (*fmt == '%' && *(fmt+1) == 's') {
            printstring(s);
            fmt += 2;
        } else {
            printchar(*fmt++);
        }
    }
}

void printf_d(const char *fmt, int d) {
    while (*fmt) {
        if (*fmt == '%' && *(fmt+1) == 'd') {
            printint(d);
            fmt += 2;
        } else {
            printchar(*fmt++);
        }
    }
}

void printf_x(const char *fmt, unsigned int x) {
    while (*fmt) {
        if (*fmt == '%' && *(fmt+1) == 'x') {
            printhex(x);
            fmt += 2;
        } else {
            printchar(*fmt++);
        }
    }
}

void printf_c(const char *fmt, char c) {
    while (*fmt) {
        if (*fmt == '%' && *(fmt+1) == 'c') {
            printchar(c);
            fmt += 2;
        } else {
            printchar(*fmt++);
        }
    }
}



void tiny_printf2(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's': {
                    char *s = va_arg(args, char *);
                    printstring(s);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    printchar(c);
                    break;
                }
                case 'd': {
                    int d = va_arg(args, int);
                    printint(d);
                    break;
                }
                case 'x': {
                    unsigned int x = va_arg(args, unsigned int);
                    printhex(x);
                    break;
                }
                default:
                    printchar('%');
                    printchar(*fmt);
                    break;
            }
        } else {
            printchar(*fmt);
        }
        fmt++;
    }

    va_end(args);
}

void tiny_printf3(const char *fmt, unsigned int *args) {
    int i = 0;
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': printint((int)args[i++]); break;
                case 'x': printhex(args[i++]); break;
                case 's': printstring((char *)args[i++]); break;
                case 'c': printchar((char)args[i++]); break;
            }
        } else {
            printchar(*fmt);
        }
        fmt++;
    }
}

void tiny_printf_dispatch(const char *fmt, void *arg) {
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's': printf_s("%s", (const char *)arg); break;
                case 'd': printf_d("%d", (int)(long)arg); break;
                case 'x': printf_x("%x", (unsigned int)(long)arg); break;
                case 'c': printf_c("%c", (char)(long)arg); break;
                default:  printchar('%'); printchar(*fmt); break;
            }
        } else {
            printchar(*fmt);
        }
        fmt++;
    }
}

void bl_main(void)
{

    // These functions are working fine.
    printstring("Hello from C!\n");
    printchar('X');
    printchar('\n');

    // These functions are working fine.
    printf_s("Hello %s!\n", "Fred");
    printf_d("Value=%d\n", 1234);
    printf_x("Hex=%x\n", 0xDEADBEEF);
    printf_c("Char=%c\n", 'A');

    tiny_printf_dispatch("Hello %s!\n", "Fred");
    tiny_printf_dispatch("Value=%d\n", (void *)1234);
    tiny_printf_dispatch("Hex=%x\n", (void *)0xDEADBEEF);
    tiny_printf_dispatch("Char=%c\n", (void *)'A');

/*
    tiny_printf("Hello %s!\n", "Fred");
    tiny_printf("Char: %c\n", 'A');
    tiny_printf("Decimal: %d\n", 1234);
    tiny_printf("Hex: %x\n", 0xDEADBEEF);
*/

    return;
}

