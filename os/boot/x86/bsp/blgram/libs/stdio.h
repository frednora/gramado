// stdio.h
// i/o operations. (C99)
// Created by Fred Nora.

#ifndef __STDIO_H
#define __STDIO_H    1

 
#define SCREEN_START 0x000B8000
#define ScreenStart SCREEN_START

#define COLUMNS  80
#define ROWS     25

#define SCREEN_WIDTH COLUMNS
#define REVERSE_ATTRIB 0x70

#define PAD_RIGHT  1
#define PAD_ZERO   2

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

//#define BUFSIZ	512
//#define EOF	(-1)
// typedef char *	va_list;


//
// == prototypes ============================================
//


//printf
int printf(const char *format, ...);
int putchar(int ch);
static void printchar(char **str, int c);
static int printi(
    char **out, 
    int i, 
    int b, 
    int sg, 
    int width, 
    int pad, 
    int letbase );
static int print(char **out, int *varg);
static int prints(char **out, const char *string, int width, int pad);
int sprintf(char *out, const char *format, ...);
int printf_main(void); //teste ...

void scroll(void);

int bl_clear(int color);
int kprintf(char *message, unsigned int line, int color);

//@todo: N�o faz parte da libc.
void vsync();
char gui_inb(int port);
void my_buffer_put_pixel( unsigned long ax, unsigned long bx, unsigned long cx, unsigned long dx);
void my_buffer_horizontal_line( unsigned long  x1,unsigned long  y, unsigned long  x2,  unsigned long color );
void my_buffer_char_blt( unsigned long x, unsigned long y, unsigned long color, unsigned long c);
void carrega_bitmap_16x16( unsigned long ax, unsigned long bx, unsigned long cx, unsigned long dx );

void clear_backbuffer(void);

#endif    


//
// End.
//
