// wproxy.h
// Window proxy. This is a lighteight proxy for the window structure 
// that lives in the display server in the user space.
// It can accelerate some operations that uses the window structure.

#ifndef __GDI_WPROXY_H
#define __GDI_WPROXY_H    1

// Structure for window proxy. 
// This is a lighteight proxy for the window structure 
// that lives in the display server in the user space.
// It can accelerate some operations that uses the window structure.
// O kernel só usa o wproxy para decisões rápidas (hit-test, ownership, bounds).  
// Essa estrutura vai abrir portas para a criação de janelas offscreen ... 
// pois é mais fácil alocar memoria estando dentro do kernel.
// Podemos criar o arquivo gdimm.c para gerenciar memória usada pelo 
// sistema gráfico.

// #todo: #important
// We gotta associate this structure with the window structure 
// in the display server. Probably at the same moment we create 
// the window in the display server, we create the wproxy structure.


struct wproxy_d 
{
    int used;
    int magic;

// The same id used in the display server. 
// It is used to identify the window in the display server.
    int wid;

    unsigned long l;
    unsigned long t;
    unsigned long w;
    unsigned long h;

    unsigned int color;

// The data buffer for the window. 
// It is used to store the pixel data of the window in the case of 
// off-screen rendering. 
// It can be used to accelerate the rendering of the window 
// by avoiding the need to read the pixel data 
// from the display server every time.   
    char *data;

// The size of the data buffer.
    int data_size; 

// Navigation
    struct wproxy_d *next;
    // ...
};


// ======================

struct wproxy_d *wproxyCreateObject(void);
char *wproxy_create_data_buffer(struct wproxy_d *wproxy, int size);
char *wproxy_get_data_buffer(struct wproxy_d *wproxy);
int wproxy_draw(struct wproxy_d *wproxy, int back_or_front);
int wproxy_redraw(struct wproxy_d *wproxy, int back_or_front);

#endif    

