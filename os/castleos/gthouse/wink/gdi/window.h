// window.h
// Window. This is a lighteight proxy for the window structure 
// that lives in the display server in the user space.
// It can accelerate some operations that uses the window structure.

#ifndef __GDI_WINDOW_H
#define __GDI_WINDOW_H    1

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
#define HIT_DESKTOP    3   // Not inside a window


// Thread/Desktop related with this window.
struct WND_OWNER_d 
{
    struct process_d *process;
    struct thread_d  *thread;
    // ...

// This is the thread id for the thread that owns the window.
// This is the target thread for sending messages when the 
// hit-test is successful.

    tid_t tid;   // #bugbug: It sounds redundant now

};

// Header for information about the structure itself.
struct WND_HEADER_d 
{
    int dummy;
    // ...
};


// #bugbug
// The values we have here normally are not sync with
// the values inside the compositor in ring 3.
// #todo: 
// We need to figure out how to deal with this situation.
// It affects the hit testing.
// When is it acceptable? When it isn't?

//struct wproxy_d (#ps: This name was deprecated)
struct WND_d 
{

// #todo
// The first main fields here will be two substructures:
// One for the Thread/Desktop related with this window and 
// the Header for information about the struture itself.

    int used;
    int magic;

// Thread/Desktop related with this window.
    struct WND_OWNER_d  owner;

// Header for information about the structure itself.
    struct WND_HEADER_d  header;

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
    //int has_frame;

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

// The address for the wproxy procedure and 
// if it is a ring 3 procedure or not.
// For the ring 3 procedure it is inside the client, not the server.
    unsigned long procedure_va;
    int is_ring3_procedure;

// Navigation
    struct WND_d *next;
    // ...
};

extern struct WND_d *window_head;  // List of window proxy objects.
extern struct WND_d *window_hover;  // mouse hover
extern struct WND_d *window_shell;  // The shell window proxy. The taskbar is the shell. 
extern struct WND_d *window_desktop;  // The desktop area.
// ...

// ======================

void wproxy_hit_test00(unsigned long x, unsigned long y);

struct WND_d *windowCreateObject(void);

int wproxy_set_shell(tid_t tid);

struct WND_d *window_create0(
    tid_t tid,
    unsigned long l, 
    unsigned long t, 
    unsigned long w, 
    unsigned long h, 
    unsigned int color);

void wproxy_ap_test(void);

char *wproxy_create_data_buffer(struct WND_d *wproxy, int size);
char *wproxy_get_data_buffer(struct WND_d *wproxy);

// Draw frame
int wproxy_drawframe(struct WND_d *wproxy, int back_or_front);
int wproxy_redrawframe(struct WND_d *wproxy, int back_or_front);

// Is it inside the frame?
int 
wproxy_is_inside_frame(
    struct WND_d *wproxy, 
    unsigned long x, 
    unsigned long y );

// Is it inside the client area?
int 
wproxy_is_inside_client_area(
    struct WND_d *wproxy, 
    unsigned long x, 
    unsigned long y );

// Update the values for wproxy given the owner's tid.
void 
window_set_parameters_given_tid(
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

