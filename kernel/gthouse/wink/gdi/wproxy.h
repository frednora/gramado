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

#define HIT_NONE       0
#define HIT_FRAME      1   // Non-client area (chrome, borders, titlebar)
#define HIT_CLIENT     2   // Client area

struct wproxy_d 
{
    int used;
    int magic;

// This is the thread id of the thread that owns the window.
// This is the target thread for sending messages when the 
// hit-test is successful.
    tid_t tid;

// The same id used in the display server. 
// It is used to identify the window in the display server.
    int wid;

//  Window frame
    unsigned long l;
    unsigned long t;
    unsigned long w;
    unsigned long h;

// Client area
// This is the client area of a window.
// If we're not inside a client area, 
// we send message to the display server to do the hit-test.
// This way the server check agains the frame/chrome area.
    unsigned long ca_l;
    unsigned long ca_t;
    unsigned long ca_w;
    unsigned long ca_h;

// Hit area
// What was the area reached by the hit-test?
    int hit_area;

// Has frame/chrome?
    int has_frame;

// Expanded non-client area.
// When set, the app does not receive mouse events.
// All mouse events (even inside the client area) are sent to the display server
// for hit-testing. This preserves the old "server authority" style of apps,
// similar to X11, but can be disabled for modern client-side drawing.
    int expanded_nonclient_area;


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

extern struct wproxy_d *wproxy_head;  // List of window proxy objects.
extern struct wproxy_d *wproxy_hover;  // mouse hover
extern struct wproxy_d *wproxy_shell;  // The shell window proxy. The taskbar is the shell. 
// ...

// ======================

void wproxy_hit_test00(unsigned long x, unsigned long y);

struct wproxy_d *wproxyCreateObject(void);


int wproxy_set_shell(tid_t tid);

int wproxy_set_expanded_nonclient_area(tid_t tid);

struct wproxy_d *wproxy_create0(
    tid_t tid,
    unsigned long l, 
    unsigned long t, 
    unsigned long w, 
    unsigned long h, 
    unsigned int color);


char *wproxy_create_data_buffer(struct wproxy_d *wproxy, int size);
char *wproxy_get_data_buffer(struct wproxy_d *wproxy);

// Draw frame
int wproxy_drawframe(struct wproxy_d *wproxy, int back_or_front);
int wproxy_redrawframe(struct wproxy_d *wproxy, int back_or_front);

// Is it inside the frame?
int wproxy_is_inside_frame(struct wproxy_d *wproxy, unsigned long x, unsigned long y);

// Is it inside the client area?
int 
wproxy_is_inside_client_area(
    struct wproxy_d *wproxy, 
    unsigned long x, 
    unsigned long y );

void wproxy_test0(unsigned long x, unsigned long y);
void wproxy_test2(unsigned long x, unsigned long y);

// Update the values for wproxy given the owner's tid.
void 
wproxy_set_parameters_given_tid(
    tid_t tid, 
    unsigned long l, 
    unsigned long t,
    unsigned long w,
    unsigned long h,
    unsigned long ca_l, 
    unsigned long ca_t,
    unsigned long ca_w,
    unsigned long ca_h );

#endif    

