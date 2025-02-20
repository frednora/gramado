/*
 * File: gws.c
 * gws - client-side library for gws window server.
 * gws is a window server running in ring3, and this is
 * a library used by the apps.
 */
// mvc:
// The user send requests to the controller
// to access the data inside the model layer.
// The user get the responses in the View layer
// where its able to visualize the information.
// " Think of the MVC pattern as a restaurant 
// with the Models being the cooks, 
// Views being the customers, and 
// Controllers being the waiters."
// #todo:
// gws_refresh_rectangle
// gws_refresh_char
// ...
// Devemos incluir o objeto gws.o nos aplicativos 
// para fazermos as chamadas ao servidor.
// #todo
// Connection support.
// O support para conexões deve ser uma bibliteca
// separada, pois assim os processos cliente podem usar essa
// biblioteca para se concetarem com outros servidores
// do mesmo tipo.

// rtl
#include <types.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <heap.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

// libgws
#include "include/gws.h"

// Library version support.
// See: gws.h
struct libgws_version_d  libgwsVersion;

// Display
struct gws_display_d *libgwsCurrentDisplay;
// Event
struct gws_event_d *CurrentEvent;

// #todo
// + draw line
// ...

// Strings
const char *title_when_no_title = "Window";

char __string_buffer[512];   // dst


// #test
// Tentando deixar o buffer aqui e aproveitar em mais funções.
// #??
// Podemos ter um buffer de request dentro da estrutura de display.
// Isso é melhor que um buffer global solto,
// Dessa forma cada display pode ter seus próprios buffers.
// #todo:
// Talvez um socket deva ter pelo menos o tamanho de um pacote de rede.
// Para assim enviarmos pacotes de rede usando a infraestrutura de socket.
// Mas por enquanto estamos usando a infraestrutura de sockets
// para transportamos mensagens de 512 bytes, contendo o protocolo
// desse sistema de janelas.

// 512 chars.
static char __gws_message_buffer[512];

//
// == prototyes (internals) =========================
//

// == Get window info ==========================
static int __gws_get_window_info_request( int fd, int wid );
static struct gws_window_info_d *__gws_get_window_info_response(
    int fd,
    struct gws_window_info_d *window_info );

// == Get next event ==========================
static int __gws_get_next_event_request(int fd,int wid);
static struct gws_event_d *__gws_get_next_event_response ( 
    int fd, 
    struct gws_event_d *event );

// == Change window position ==================
static int 
__gws_change_window_position_request ( 
    int fd, int window, unsigned long x, unsigned long y );
static int __gws_change_window_position_reponse( int fd );

// == Resize window ==========================
static int 
__gws_resize_window_request ( 
    int fd, int window, unsigned long w, unsigned long h );
static int __gws_resize_window_reponse(int fd);

// == Redraw window ==========================
static int 
__gws_redraw_window_request( int fd, int window, unsigned long flags );

// == Refresh Window ==========================
static int __gws_refresh_window_request( int fd, int window );

// == Create Window ==========================
static int 
__gws_createwindow_request (
    int fd,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    unsigned int bg_color,
    unsigned long type,
    unsigned long status,
    unsigned long view,
    unsigned long style,
    unsigned long parent,
    const char *name );
static wid_t __gws_createwindow_response(int fd);

// == refresh rectangle
static int 
__gws_refresh_rectangle_request (
    int fd,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height );
static int __gws_refresh_rectangle_response(int fd);

// == Draw Char ==========================
static int 
__gws_drawchar_request (
    int fd,
    int window_id,
    unsigned long left,
    unsigned long top,
    unsigned int color,
    unsigned int ch );

// == Draw text ==========================
static int 
__gws_drawtext_request (
    int fd,
    int window_id,
    unsigned long left,
    unsigned long top,
    unsigned int color,
    const char *string );

// == set text ==========================
static int 
__gws_settext_request (
    int fd,
    int window_id,
    unsigned long left,
    unsigned long top,
    unsigned long color,
    char *string );


// == get text ==========================
static int 
__gws_gettext_request (
    int fd,
    int window_id,
    unsigned long left,
    unsigned long top,
    unsigned long color,
    char *string );
static char *__gws_gettext_response(int fd);


// ----------------------

static int 
__gws_clone_and_execute_request (
    int fd,
    unsigned long arg1,
    unsigned long arg2,
    unsigned long arg3,
    unsigned long arg4,
    const char *string );
static int __gws_clone_and_execute_response(int fd);

// == ... ==========================

static void __gws_clear_msg_buff(void);


//
// == Functions ====================
//

// System call.
// System interrupt.
void *gws_system_call ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
/*
    unsigned long ReturnValue=0;
    asm volatile ( " int %1 \n"
                 : "=a"(ReturnValue)
                 : "i"(0x80), "a"(a), "b"(b), "c"(c), "d"(d) );
    return (void *) ReturnValue; 
*/
    return (unsigned long) sc80(a,b,c,d);
}

static void __gws_clear_msg_buff(void)
{
    register int i=0;
    for (i=0; i<512; i++){
        __gws_message_buffer[i] = 0;
    }; 
}

// Debug via serial port. (COM1)
void gws_debug_print(const char *string)
{
    if ((void*) string == NULL){
        return;
    }
    if (*string == 0){
        return;
    }
    gws_system_call ( 
        289, 
        (unsigned long) string,
        (unsigned long) string,
        (unsigned long) string );
}

//=============================================

// Initialize the library.
int gws_initialize_library(void)
{
    pid_t ws_pid = -1;    // PID do window server.

    int __ApplicationFD = -1;

    __gws_clear_msg_buff();

    ws_pid = (pid_t) gws_initialize_connection();
    if (ws_pid < 0)
    {
        gws_debug_print("gws_initialize_library: [fail] ws_pid\n");
        return (int) -1;
    }

// #todo:
// #importante
// We need to alloc memory to the CurrentEvent struct.
// So we need the libc support. 
// Check the compilation and include the libc. ???

    //CurrentEvent = (void *) gws_malloc(sizeof(struct gws_event_d));

    return 0;
    // return (int) ws_pid;
}

void *gws_malloc(size_t size)
{
    if (size <= 0){
        size = 1;
    }
    return (void*) malloc(size);
}

void gws_free(void *ptr)
{
    if ((void*) ptr == NULL){
        return;
    }
    free(ptr);
}

//
// == Helper functions ============================
//

// ==============================================================

// == get window info request ==========================
// Let's get some information about the given window.
// Send request:
// Setup the parameters and
// write the data into the file.

static int __gws_get_window_info_request( int fd, int wid )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0]; 
    int n_writes = 0;

    //gws_debug_print ("__gws_get_window_info_request: wr\n");

// window id.
    message_buffer[0] = wid;
// Message code.
    message_buffer[1] = GWS_GetWindowInfo;
// libgws signature.
    message_buffer[2] = (unsigned long) 1234;
    message_buffer[3] = (unsigned long) 5678;
    //...

    if (fd<0){
        return (int) -1;
    }
    if (wid<0){
        return (int) -1;
    }

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes <= 0){
        return (int) -1;
    }

    return (int) n_writes; 
}

// get window info response ===================
// #importante
// As informações devem ficar aqui até que o cliente pegue.
// Um ponteiro será devolvido para ele.
static struct gws_window_info_d *__gws_get_window_info_response(
    int fd,
    struct gws_window_info_d *window_info )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    ssize_t n_reads=0;
// The header.
    int wid=0;
    int msg_code=0;
    int sig1=0;
    int sig2=0;
    register int i=0;

// #todo:
// Check fd.
    //if (fd <0)
        //return NULL;

    if ((void*) window_info == NULL){
        return NULL;
    }

// fail
    window_info->used = FALSE;
    window_info->magic = 0;

    if (fd<0){
        return NULL;
    }

// read

// --------------------
// Clean the local buffer,
// and then populate with some data.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

    n_reads = 
        (ssize_t) recv ( 
                      fd, 
                      __gws_message_buffer, 
                      sizeof(__gws_message_buffer), 
                      0 );

    if (n_reads <= 0)
    { 
        //
        return (struct gws_window_info_d *) window_info;
    }

// The msg packet

    int msg = (int) message_buffer[1];
    msg = (msg & 0xFFFF);

    switch (msg){

    // ok, that is what we need.
    case SERVER_PACKET_TYPE_REPLY:
        goto process_response;
        break;

    // fail
    case SERVER_PACKET_TYPE_EVENT:
    case SERVER_PACKET_TYPE_REQUEST:
    case SERVER_PACKET_TYPE_ERROR:
    default:
        goto fail;
        break; 
    };

// Put the message info into the structure.
process_response:

// the header
    wid      = (int)           message_buffer[0];  // window id
    msg_code = (int)           message_buffer[1];  // message code: (It is an EVENT)
    sig1     = (unsigned long) message_buffer[2];  // Signature 1: 1234
    sig2     = (unsigned long) message_buffer[3];  // Signature 2: 5678

    if (sig1 != 1234){
        //debug_print ("__gws_get_window_info_response: sig1 fail\n");
        goto fail;
    }

    if (sig2 != 5678){
        //debug_print ("__gws_get_window_info_response: sig2 fail\n");
        goto fail;
    }

// data field

// OK
// The data field
    if (msg_code == SERVER_PACKET_TYPE_REPLY)
    {
        //printf("__gws_get_next_event_response: WE GOT THE DATA\n");
    
        window_info->wid   = (int) message_buffer[4];  // wid
        window_info->pwid  = (int) message_buffer[5];  // parent wid
        window_info->type  = (int) message_buffer[6];  // window type

        // window
        window_info->left   = (unsigned long) message_buffer[7];   // left 
        window_info->top    = (unsigned long) message_buffer[8];   // top
        window_info->width  = (unsigned long) message_buffer[9];   // width
        window_info->height = (unsigned long) message_buffer[10];  // height

        //#debug
        //printf("libgws: l=%d t=%d w=%d h=%d\n",
            //window_info->left, 
            //window_info->top, 
            //window_info->width, 
            //window_info->height );

        // limits
        window_info->right  = (unsigned long) message_buffer[11];  // right
        window_info->bottom = (unsigned long) message_buffer[12];  // bottom

        // client area rectangle
        window_info->cr_left   = (unsigned long) message_buffer[13];  // cr left 
        window_info->cr_top    = (unsigned long) message_buffer[14];  // cr top
        window_info->cr_width  = (unsigned long) message_buffer[15];  // cr width
        window_info->cr_height = (unsigned long) message_buffer[16];  // cr height

        // border
        window_info->border_width = (unsigned long) message_buffer[17];  // border width

        // The app will need this thing.
        window_info->used = TRUE;
        window_info->magic = 1234;

        // #debug
        //printf ("::: wid=%d msg=%d l1=%d l2=%d \n",
            //window_info->wid, window_info->msg, window_info->long1, window_info->long2 );

        // ok, we got it.
        return (struct gws_window_info_d *) window_info;
    }

// fall to fail field.
fail:
    return NULL;
}

// ==============================================================

// == get next event ==========================
// Send request:
// Setup the parameters and
// write the data into the file.

static int __gws_get_next_event_request(int fd,int wid)
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes = 0;
    register int i=0;

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

// window id.
    message_buffer[0] = (unsigned long) (wid & 0xFFFFFFFF); 
// Message code
    message_buffer[1] = GWS_GetNextEvent;
    message_buffer[2] = 0;
    message_buffer[3] = 0;
    //...

    if (fd<0){
        return (int) -1;
    }

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes <= 0){
        return (int) -1;
    }

    return (int) n_writes;
}

// __gws_get_next_event_response:
// #importante
// As informações devem ficar aqui até que o
// cliente pegue.
// Um ponteiro será devolvido para ele.
static struct gws_event_d *__gws_get_next_event_response ( 
    int fd, 
    struct gws_event_d *event )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    ssize_t n_reads=0;
// The header
    int wid=0;
    int msg_code=0;
    unsigned long sig1=0;
    unsigned long sig2=0;
    register int i=0;

    //gws_debug_print ("__gws_get_next_event_response: rd\n"); 

    if (fd<0){
        return NULL;
    }
    if ((void*) event == NULL){
        return NULL;
    }

// Read

// --------------------
// Clean the local buffer,
// and then populate with some data.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;


    n_reads = 
        (ssize_t) recv ( 
                      fd, 
                      __gws_message_buffer, 
                      sizeof(__gws_message_buffer), 
                      0 );

    if (n_reads <= 0)
    { 
        printf ("__gws_get_next_event_response: Read 0 bytes\n"); 
        event->type = 0;
        event->used = FALSE;
        event->magic = 0;
        return (struct gws_event_d *) event;
    }

// The msg packet

    int msg = (int)  message_buffer[1];
    msg = (msg & 0xFFFF);

    switch (msg){

    case SERVER_PACKET_TYPE_EVENT:
        goto process_event;
        break;

    case SERVER_PACKET_TYPE_REPLY:
    case SERVER_PACKET_TYPE_REQUEST:
    case SERVER_PACKET_TYPE_ERROR:
    default:
        //printf ("__gws_get_next_event_response: Invalid msg code\n"); 
        event->type = 0;
        event->used = FALSE;
        event->magic = 0;
        return (struct gws_event_d *) event;
        break; 
    };

// crazy fail
    printf ("__gws_get_next_event_response: crazy fail\n"); 
    event->type = 0;
    event->used = FALSE;
    event->magic = 0;

    return (struct gws_event_d *) event;

process_event:

// Get the message sent by the server.
    wid      = (int)           message_buffer[0];  // window id
    msg_code = (int)           message_buffer[1];  // message code: (It is an EVENT)
    sig1     = (unsigned long) message_buffer[2];  // Signature 1: 1234
    sig2     = (unsigned long) message_buffer[3];  // Signature 2: 5678

// #data
// A informação esta nos campos 4, 5,6 e 7.

// Checks
// #todo: 
// Check if it is a REPLY message.

    if (msg_code != SERVER_PACKET_TYPE_EVENT){
        debug_print ("__gws_get_next_event_response: msg_code fail\n");
        goto fail0;
    }
    if (sig1 != 1234){
        debug_print ("__gws_get_next_event_response: sig1 fail\n");
        goto fail0;
    }
    if (sig2 != 5678){
        debug_print ("__gws_get_next_event_response: sig2 fail\n");
        goto fail0;
    }

// The event properly.
    event->used = FALSE;
    event->magic = 0;

// OK
    if (msg_code == SERVER_PACKET_TYPE_EVENT)
    {
        //printf("__gws_get_next_event_response: WE GOT AN EVENT\n");
    
        event->window = (int) message_buffer[4];            // window id
        event->type   = (int) message_buffer[5];            // message code
        event->long1  = (unsigned long) message_buffer[6];  // long1
        event->long2  = (unsigned long) message_buffer[7];  // long2

        event->window = (int) ( event->window & 0xFFFF );
        event->type   = (int) ( event->type & 0xFFFF );

        event->used = TRUE;
        event->magic = 1234;

        // #debug
        //printf ("::: wid=%d msg=%d l1=%d l2=%d \n",
            //event->wid, event->msg, event->long1, event->long2 );

        return (struct gws_event_d *) event;
    }

fail0:
    event->type = 0;
    event->used = FALSE;
    event->magic = 0;
    return NULL;
}

//=========

static int __gws_refresh_window_request( int fd, int window )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes=0;

    //gws_debug_print ("__gws_refresh_window_request: wr\n");

    if (fd<0){
        return (int) -1;
    }
    if (window<0){
        return (int) -1;
    }

// Get info.

    message_buffer[0] = window; 
// Message code
    message_buffer[1] = GWS_RefreshWindow;
    message_buffer[2] = 0;
    message_buffer[3] = 0;
    //...

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
            return (int) -1;
    }

    return (int) n_writes;
}

static int 
__gws_redraw_window_request ( 
    int fd, 
    int window, 
    unsigned long flags )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes = 0;

    //gws_debug_print ("__gws_redraw_window_request: wr\n");

    if (fd<0){
        return (int) -1;
    }

// Window ID
    message_buffer[0] = window;
// Message code
    message_buffer[1] = GWS_RedrawWindow;
// Flags
    message_buffer[2] = (unsigned long) flags;
    message_buffer[3] = 0;
    //...

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
        return (int) -1;
    }

    return (int) n_writes;
}

static int 
__gws_change_window_position_request ( 
    int fd, 
    int window, 
    unsigned long x, 
    unsigned long y )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes = 0;
    register int i=0;

    //gws_debug_print ("__gws_change_window_position_request: wr\n");

    if (fd<0){
        return (int) -1;
    }

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

    message_buffer[0] = (unsigned long) (window & 0xFFFFFFFF);
// Message code
    message_buffer[1] = GWS_ChangeWindowPosition;
    message_buffer[2] = (unsigned long) x;
    message_buffer[3] = (unsigned long) y;
    //...

// Write

    n_writes = 
        (int) send ( 
                  fd,
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
        return (int) -1;
    }

    return (int) n_writes;
}

static int __gws_change_window_position_reponse(int fd)
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    ssize_t n_reads=0;
    register int i=0;

// Waiting for response. ==================

// Espera para ler a resposta. 
// Esperando com yield como teste.
// Isso demora, pois a resposta só será enviada depois de
// prestado o servido.
// obs: Nesse momento deveríamos estar dormindo.

    //gws_debug_print ("__gws_change_window_position_reponse: Waiting ...\n");      


// #todo
// Podemos checar antes se o fd 
// representa um objeto que permite leitura.
// Pode nem ser possível.
// Mas como sabemos que é um soquete,
// então sabemos que é possível ler.
// #caution
// Waiting for response.
// We can stay here for ever.

    if (fd<0){
        return (int) -1;
    }

// --------------------
// Clean the local buffer,
// and then populate with some data.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

response_loop:

    n_reads = 
        (ssize_t) recv( 
                      fd, 
                      __gws_message_buffer, 
                      sizeof(__gws_message_buffer), 
                      0 );

    //if (n_reads<=0){
    //    goto response_loop;
    //}
    
    // Se retornou 0, podemos tentar novamente.
    if (n_reads == 0){
        goto response_loop;
    }
    
    // Se retornou -1 é porque algo está errado com o arquivo.
    if (n_reads < 0){
        gws_debug_print ("__gws_change_window_position_reponse: recv fail.\n");
        printf          ("__gws_change_window_position_reponse: recv fail.\n");
        printf ("Something is wrong with the socket.\n");
        exit (1);
    }

// The msg index.
// Get the message sended by the server.

    int msg = (int) message_buffer[1];
    
    switch (msg){

        // Reply!
        case SERVER_PACKET_TYPE_REPLY:
            goto process_reply;
            break;

        case SERVER_PACKET_TYPE_REQUEST: 
        case SERVER_PACKET_TYPE_EVENT:
        case SERVER_PACKET_TYPE_ERROR:
        default:
            return (int) -1;
            break; 
    };

// Process reply
// A resposta tras o window id no início do buffer.

process_reply:

    // #test
    //gws_debug_print ("terminal: Testing close() ...\n"); 
    //close (fd);

    //gws_debug_print ("terminal: bye\n"); 
    //printf ("terminal: Window ID %d \n", message_buffer[0] );
    //printf ("terminal: Bye\n");
    
    // #todo
    // Podemos usar a biblioteca e testarmos
    // vários serviços da biblioteca nesse momento.

    return (int) message_buffer[0];

// Process an event.
process_event:
    //gws_debug_print ("__gws_change_window_position_reponse: We got an event\n"); 
    return 0;
}

// ======================================================

static int 
__gws_resize_window_request ( 
    int fd, 
    int window, 
    unsigned long w, 
    unsigned long h )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes = 0;
    register int i=0;

    //gws_debug_print ("__gws_resize_window_request: wr\n");

    if (fd<0){
        return (int) -1;
    }

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

    message_buffer[0] = window;
// Message code
    message_buffer[1] = GWS_ResizeWindow;
    message_buffer[2] = w;
    message_buffer[3] = h;
    // ...

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
        return (int) -1;
    }

    return (int) n_writes;
}

static int __gws_resize_window_reponse(int fd)
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    ssize_t n_reads=0;
    register int i=0;

// Waiting for response.
// Espera para ler a resposta. 
// Esperando com yield como teste.
// Isso demora, pois a resposta só será enviada depois de
// prestado o servido.
// obs: Nesse momento deveríamos estar dormindo.

    //gws_debug_print ("__gws_resize_window_reponse: Waiting ...\n");      

// #todo
// Podemos checar antes se o fd 
// representa um objeto que permite leitura.
// Pode nem ser possível.
// Mas como sabemos que é um soquete,
// então sabemos que é possível ler.

    // gws_debug_print ("__gws_resize_window_reponse: rd\n");

// #caution
// Waiting for response.
// We can stay here for ever.

    if (fd<0){
        return (int) -1;
    }

// --------------------
// Clean the local buffer,
// and then populate with some data.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

response_loop:

    n_reads = 
        (ssize_t) recv( 
                      fd, 
                      __gws_message_buffer, 
                      sizeof(__gws_message_buffer), 
                      0 );

    //if (n_reads<=0){
    //    goto response_loop;
    //}
    
    // Se retornou 0, podemos tentar novamente.
    if (n_reads == 0){
        goto response_loop;
    }
    
    // Se retornou -1 é porque algo está errado com o arquivo.
    if (n_reads < 0){
        gws_debug_print ("__gws_resize_window_reponse: recv fail.\n");
        printf          ("__gws_resize_window_reponse: recv fail.\n");
        printf ("Something is wrong with the socket.\n");
        exit (1);
    }

// The msg index.
// Get the message sended by the server.

    int msg = (int) message_buffer[1];
    
    switch (msg){

        // Reply!
        case SERVER_PACKET_TYPE_REPLY:
            goto process_reply;
            break;

        case SERVER_PACKET_TYPE_REQUEST:
        case SERVER_PACKET_TYPE_EVENT:
        case SERVER_PACKET_TYPE_ERROR:
        default:
            return (int) -1;
            break; 
    };

// Process reply.
// A resposta tras o window id no início do buffer.
process_reply:

    // #test
    //gws_debug_print ("terminal: Testing close() ...\n"); 
    //close (fd);

    //gws_debug_print ("terminal: bye\n"); 
    //printf ("terminal: Window ID %d \n", message_buffer[0] );
    //printf ("terminal: Bye\n");
    
    // #todo
    // Podemos usar a biblioteca e testarmos
    // vários serviços da biblioteca nesse momento.

    //return 0;
    return (int) message_buffer[0];

// Process an event.
process_event:
    // gws_debug_print ("__gws_resize_window_reponse: We got an event\n"); 
    return 0;
}

// =============================
// == rectangle ========

static int 
__gws_refresh_rectangle_request (
    int fd,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes = 0;

    //gws_debug_print ("__gws_refresh_rectangle_request: wr\n");

    if (fd<0){
        return (int) -1;
    }

    message_buffer[0] = (unsigned long) 0;
// Message code
    message_buffer[1] = (unsigned long) GWS_RefreshRectangle;
    message_buffer[2] = (unsigned long) 0;
    message_buffer[3] = (unsigned long) 0;

    message_buffer[4] = (unsigned long) (left   & 0xFFFF); 
    message_buffer[5] = (unsigned long) (top    & 0xFFFF); 
    message_buffer[6] = (unsigned long) (width  & 0xFFFF); 
    message_buffer[7] = (unsigned long) (height & 0xFFFF ); 

    //message_buffer[?] = (unsigned long) 0; 
    // ...

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
        return (int) -1;
    }

    return (int) n_writes;
}

// Response
// A sincronização nos diz que já temos um reply.
static int __gws_refresh_rectangle_response(int fd)
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    ssize_t n_reads=0;
    register int i=0;

    //gws_debug_print ("__gws_refresh_rectangle_response: rd\n");      

    if (fd<0){
        return (int) -1;
    }

// Read

// --------------------
// Clean the local buffer,
// and then populate with some data.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

    n_reads = 
        (ssize_t) recv( 
                      fd, 
                      __gws_message_buffer, 
                      sizeof(__gws_message_buffer), 
                      0 );

    if (n_reads <= 0){
        return (int) -1;
    }

//
// The msg index.
//
    // Get the message sended by the server.

    int msg   = (int) message_buffer[1];
    int value = (int) message_buffer[2];

    switch (msg){

        // Reply
        case SERVER_PACKET_TYPE_REPLY:
            return (int) value;
            break;

        case SERVER_PACKET_TYPE_REQUEST:
        case SERVER_PACKET_TYPE_EVENT:
        case SERVER_PACKET_TYPE_ERROR:
        default:
            return (int) -1;
            break; 
    };

//fail:
    return (int) -1;
}

// ================================================

static int 
__gws_drawchar_request (
    int fd,
    int window_id,
    unsigned long left,
    unsigned long top,
    unsigned int color,
    unsigned int ch )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes = 0;

    //gws_debug_print ("__gws_drawchar_request: wr\n");      

    if (fd<0){
        return (int) -1;
    }

    message_buffer[0] = (unsigned long) 0;
// Message code
    message_buffer[1] = (unsigned long) GWS_DrawChar;
    message_buffer[2] = (unsigned long) 0;
    message_buffer[3] = (unsigned long) 0;

    message_buffer[4] = (unsigned long) window_id;
    message_buffer[5] = (unsigned long) (left & 0xFFFF); 
    message_buffer[6] = (unsigned long) (top  & 0xFFFF); 
    message_buffer[7] = (unsigned long) (color & 0xFFFFFFFF); 

// The 'char'.
    message_buffer[8] = (unsigned long) (ch & 0xFF);
    // ...

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
        return (int) -1;
    }

    return (int) n_writes;
}

//------------------------------------------------

// Draw text
static int 
__gws_drawtext_request (
    int fd,
    int window_id,
    unsigned long left,
    unsigned long top,
    unsigned int color,
    const char *string )
{
// OUT:

    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    //unsigned long *string_buffer = (unsigned long *) &__gws_message_buffer[128];
    int n_writes = 0;
    register int i=0;
    char LocalString[256];

    //gws_debug_print ("gws_drawtext_request: wr\n");

    if (fd<0){
        goto fail;
    }

//--------------------------
// String validation
    if ((void*) string == NULL)
        goto fail;
    if (*string == 0)
        goto fail;

//--------------------------
// String size
    size_t StringSize = 0;
    StringSize = (size_t) strlen(string);
    if (StringSize <= 0)
        goto fail;
    if (StringSize > 256)
        goto fail;

    memset(LocalString, 0, 256);
    sprintf(LocalString,string);

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

    message_buffer[0] = 0;
// Message code
    message_buffer[1] = GWS_DrawText;
    message_buffer[2] = 0;
    message_buffer[3] = 0;

    message_buffer[4] = window_id;
    message_buffer[5] = left; 
    message_buffer[6] = top; 
    message_buffer[7] = (unsigned long) (color & 0xFFFFFFFF);

//--------------------------
// String support
// Fill the string buffer
    int string_off=8;
    
    char *target_base = (char *) &message_buffer[string_off];
    // Clean the target buffer.
    memset(target_base, 0, 256);
    if (StringSize > 256)
        goto fail;
    // Copy the string, given it's size.
    for (i=0; i<StringSize; i++)
    {
        //*target_base = *string;   // Put a char.
        // Increment both
        //target_base++;
        //string++; 
        *target_base = LocalString[i];
        target_base++;
    };
    // Finalize the string
    *target_base = 0;


// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes <= 0){
        goto fail;
    }
       
    return (int) n_writes;
fail:
    return (int) -1;
}

//-----------------------------------------

static int 
__gws_settext_request (
    int fd,
    int window_id,
    unsigned long left,
    unsigned long top,
    unsigned long color,
    char *string )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    //unsigned long *string_buffer = (unsigned long *) &__gws_message_buffer[128];
    int n_writes = 0;
    register int i=0;

    //gws_debug_print ("gws_drawtext_request: wr\n");

    if (fd<0){
        return (int) -1;
    }

    if ((void*) string == NULL)
        return -1;
    if (*string == 0)
        return -1;


    message_buffer[0] = 0;
// Message code
    message_buffer[1] = GWS_SetText;
    message_buffer[2] = 0;
    message_buffer[3] = 0;

    message_buffer[4] = window_id;
    message_buffer[5] = left; 
    message_buffer[6] = top; 
    message_buffer[7] = color;

// String support
// Fill the string buffer
    int string_off=8;
    char *p = (char *) &message_buffer[string_off];

    for (i=0; i<250; i++)
    {
        *p = *string;   // Put a char.
        // Increment both
        p++;
        string++; 
    };
    *p = 0;  // finalize the string

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
            return (int) -1;
    }
       
    return (int) n_writes;
}

//-----------------------------------------

static int 
__gws_gettext_request (
    int fd,
    int window_id,
    unsigned long left,
    unsigned long top,
    unsigned long color,
    char *string )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    //unsigned long *string_buffer = (unsigned long *) &__gws_message_buffer[128];
    int n_writes = 0;

    //gws_debug_print ("gws_drawtext_request: wr\n");

    if (fd<0){
        return (int) -1;
    }
    if ((void*) string == NULL)
        return -1;
    if (*string == 0)
        return -1;

    message_buffer[0] = 0;
// Message code
    message_buffer[1] = GWS_GetText;
    message_buffer[2] = 0;
    message_buffer[3] = 0;

    message_buffer[4] = window_id;
    message_buffer[5] = left; 
    message_buffer[6] = top; 
    message_buffer[7] = color;

/*
// String support
// Fill the string buffer
    register int i=0;
    int string_off=8;
    char *p = (char *) &message_buffer[string_off];
    for (i=0; i<250; i++)
    {
        *p = *string;   // Put a char.
        // Increment both
        p++;
        string++; 
    };
    *p = 0;  // finalize the string
*/

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
            return (int) -1;
    }
       
    return (int) n_writes;
}

// Draw text - response.
// Waiting for response.
// Espera para ler a resposta. 
// Esperando com yield como teste.
// Isso demora, pois a resposta só será enviada depois de
// prestado o servido.
// obs: Nesse momento deveríamos estar dormindo.
// #todo
// Podemos checar antes se o fd 
// representa um objeto que permite leitura.
// Pode nem ser possível.
// Mas como sabemos que é um soquete,
// então sabemos que é possível ler.

static char *__gws_gettext_response(int fd)
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    ssize_t n_reads=0;
    char *p = (char *) &message_buffer[8];  // src
    register int i=0;

    if (fd<0){
        printf ("__gws_gettext_response: fd\n");
        return NULL;  
    }

// #caution
// Waiting for response.
// We can stay here for ever.

response_loop:

    n_reads = 
        (ssize_t) recv(
                      fd, 
                      __gws_message_buffer, 
                      sizeof(__gws_message_buffer), 
                      0 );

    //if (n_reads<=0){
    //    goto response_loop;
    //}
    
    // #bugbug: Loop infinito.
    // Se retornou 0, podemos tentar novamente.
    if (n_reads == 0){
        goto response_loop;
    }
    
    // Se retornou -1 é porque algo está errado com o arquivo.
    if (n_reads < 0)
    {
        gws_debug_print ("__gws_gettext_response: recv fail.\n");
        printf          ("__gws_gettext_response: recv fail.\n");
        printf ("Something is wrong with the socket.\n");
        return NULL;
        //exit (1);
    }

// The msg index.
// Get the message sended by the server.

    int msg = (int) message_buffer[1];
    
    switch (msg){

        // Reply!
        case SERVER_PACKET_TYPE_REPLY:
            goto process_reply;
            break;

        case SERVER_PACKET_TYPE_REQUEST:
        case SERVER_PACKET_TYPE_EVENT:            
        case SERVER_PACKET_TYPE_ERROR:
        default:
            printf ("__gws_gettext_response: Not a reply\n");
            return NULL;
            break; 
    };

// Process reply
// A resposta tras o window id no início do buffer. 
process_reply:

    // #test
    //gws_debug_print ("terminal: Testing close() ...\n"); 
    //close (fd);

    //gws_debug_print ("terminal: bye\n"); 
    //printf ("terminal: Window ID %d \n", message_buffer[0] );
    //printf ("__gws_gettext_response: process_reply\n");

    // #todo
    // Podemos usar a biblioteca e testarmos
    // vários serviços da biblioteca nesse momento.
    memset(__string_buffer, 0 ,512);
// Get from message buffer
// and put it into the local buffer.
    for (i=0; i<256; i++)
    {
        __string_buffer[i] = *p; 
        p++;
    };
    __string_buffer[i+1] = 0; // finalize the local buffer.

   //printf ("__gws_gettext_response: __string_buffer[] %s\n",__string_buffer);
   //while(1){}

// Return the address of a local buffer.
    return (char*) __string_buffer;
}


//----------------------------------------------------

// Clone and execute - request.
static int 
__gws_clone_and_execute_request (
    int fd,
    unsigned long arg1,
    unsigned long arg2,
    unsigned long arg3,
    unsigned long arg4,
    const char *string )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    //unsigned long *string_buffer = (unsigned long *) &__gws_message_buffer[128];
    int n_writes = 0;
    register int i=0;

    //gws_debug_print ("__gws_clone_and_execute_request: wr\n");

    if (fd<0){
        return (int) -1;
    }

// path
    if ((void*) string == NULL)
        return -1;
    if (*string == 0)
        return -1;


    size_t StringSize = 0;
    StringSize = (size_t) strlen(string);
    if (StringSize <= 0)
        return -1;
    if (StringSize > 250)
        return -1;

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

    message_buffer[0] = 0;
// Message code
    message_buffer[1] = 9099;
    message_buffer[2] = 0;
    message_buffer[3] = 0;

    message_buffer[4] = (unsigned long) arg1;
    message_buffer[5] = (unsigned long) arg2; 
    message_buffer[6] = (unsigned long) arg3; 
    message_buffer[7] = (unsigned long) arg4; 

// String support
// Fill the string buffer.
    int string_off=8;
    char *p = (char *) &message_buffer[string_off];
    memset(p, 0, 250);
    for (i=0; i<StringSize; i++)
    {
        *p = *string;
        p++;
        string++;
    };
    // finalize
    *p = 0;
    p++;
    // finalize again.
    *p = 0;

// Write

    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes<=0){
            return (int) -1;
    }

    return (int) n_writes;
}

// clone and execute - response.
// Waiting for response.
// Espera para ler a resposta. 
// Esperando com yield como teste.
// Isso demora, pois a resposta só será enviada depois de
// prestado o servido.
// obs: Nesse momento deveríamos estar dormindo.
// #todo
// Podemos checar antes se o fd 
// representa um objeto que permite leitura.
// Pode nem ser possível.
// Mas como sabemos que é um soquete,
// então sabemos que é possível ler.
static int __gws_clone_and_execute_response(int fd)
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    ssize_t n_reads=0;

    // gws_debug_print ("__gws_clone_and_execute_response: rd\n");

    if (fd<0){
        return (int) -1;
    }

// #caution
// Waiting for response.
// We can stay here for ever.

response_loop:

// Read

    n_reads = 
        (ssize_t) recv(
                      fd, 
                      __gws_message_buffer, 
                      sizeof(__gws_message_buffer), 
                      0 );

    //if (n_reads<=0){
    //    goto response_loop;
    //}
    
    // Se retornou 0, podemos tentar novamente.
    if (n_reads == 0){
        goto response_loop;
    }
    
    // Se retornou -1 é porque algo está errado com o arquivo.
    if (n_reads < 0){
        gws_debug_print ("__gws_clone_and_execute_response: recv fail.\n");
        printf          ("__gws_clone_and_execute_response: recv fail.\n");
        printf ("Something is wrong with the socket.\n");
        return (int) -1;
        //exit (1);
    }

// The msg index.
// Get the message sended by the server.

    int msg = (int) message_buffer[1];
    
    switch (msg){

        // Reply!
        case SERVER_PACKET_TYPE_REPLY:
            goto process_reply;
            break;

        case SERVER_PACKET_TYPE_REQUEST:
        case SERVER_PACKET_TYPE_EVENT:            
        case SERVER_PACKET_TYPE_ERROR:
        default:
            return (int) -1;
            break; 
    };

// Process reply.
// A resposta tras o window id no início do buffer.
process_reply:

    // #test
    //gws_debug_print ("terminal: Testing close() ...\n"); 
    //close (fd);

    //gws_debug_print ("terminal: bye\n"); 
    //printf ("terminal: Window ID %d \n", message_buffer[0] );
    //printf ("terminal: Bye\n");

    // #todo
    // Podemos usar a biblioteca e testarmos
    // vários serviços da biblioteca nesse momento.

    return (int) message_buffer[0];

// Process an event.
process_event:
    // gws_debug_print ("__gws_clone_and_execute_response: We got an event\n"); 
    return 0;
}

// Create window - request.
// #todo
// Describe all the parameter.
static int 
__gws_createwindow_request (
    int fd,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    unsigned int bg_color,
    unsigned long type,
    unsigned long status,
    unsigned long view,
    unsigned long style,
    unsigned long parent,
    const char *name )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes=0;
    register int i=0;
    char *Name;
    char LocalName[256];
    int client_pid = (int) rtl_current_process();
    int client_tid = (int) rtl_current_thread();

    //gws_debug_print ("__gws_createwindow_request: wr\n");

    if (fd<0){
       return (int) -1;
    }

// name
    if ((void*) name == NULL)
        return -1;
    if (*name == 0)
        return -1;

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

// wid, message code, long1, long2.
    message_buffer[0] = 0;
    message_buffer[1] = GWS_CreateWindow;
    message_buffer[2] = status;  // #test Status
    message_buffer[3] = view;    // #test view
// l,t,w,h
// These are the outer values.
// Including the border if it has one.
    message_buffer[4] = (unsigned long)(left   & 0xFFFF);
    message_buffer[5] = (unsigned long)(top    & 0xFFFF);
    message_buffer[6] = (unsigned long)(width  & 0xFFFF);
    message_buffer[7] = (unsigned long)(height & 0xFFFF);
// background color
    message_buffer[8] = (unsigned long)(bg_color & 0xFFFFFFFF);
// type
    message_buffer[9] = (unsigned long)(type & 0xFFFF);
// parent window id.
    message_buffer[10] = parent;
// #test
// style
// 0x0001 maximized
// 0x0002 minimized
// 0x0004 fullscreen
// ...
    message_buffer[11] = style;
// Client pid
    message_buffer[12] = client_pid;
// Client tid
    message_buffer[13] = client_tid;

// ------

// Local pointer
    Name = name;
    if ((void*) Name == NULL){
        Name = title_when_no_title;
    }


    size_t StringSize = 0;
    StringSize = (size_t) strlen(Name);
    if (StringSize <= 0)
        return -1;
    if (StringSize > 250)
        return -1;

    // Local name
    memset(LocalName, 0, 256);
    sprintf(LocalName,Name);

// String support
// Set up the string starting in the offset '14'.
    //int max=250;
    int string_off=14;
    char *p = (char *) &message_buffer[string_off];
    memset(p, 0, 250);
    for (i=0; i<StringSize; i++)
    {
        //*p = *Name;   // Put the char into the message buffer.
        //p++;
        //Name++;
        *p = LocalName[i];
        p++;
    };
    // Finalize
    *p = 0;
// ------


// Write
    n_writes = -1;
    n_writes = 
        (int) send ( 
                  fd, 
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes <= 0){
        return (int) -1;
    }

    return (int) n_writes;

fail:
    return (int) (-1);
}

// Create window - response.
// A sincronização nos diz que já temos um reply.
// Waiting for response.
// Espera para ler a resposta. 
// Esperando com yield como teste.
// Isso demora, pois a resposta só será enviada depois de
// prestado o servido.
// obs: Nesse momento deveríamos estar dormindo.
// #todo
// Podemos checar antes se o fd 
// representa um objeto que permite leitura.
// Pode nem ser possível.
// Mas como sabemos que é um soquete,
// então sabemos que é possível ler.

static wid_t __gws_createwindow_response(int fd)
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    ssize_t n_reads=0;
    register int i=0;

    //gws_debug_print ("__gws_createwindow_response: rd\n");

    if (fd<0){
        return (wid_t) -1;
    }

//
// Read
//

// --------------------
// Clean the local buffer,
// and then populate with some data.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

    n_reads = 
        (ssize_t) recv ( 
                      fd, 
                      __gws_message_buffer, 
                      sizeof(__gws_message_buffer), 
                      0 );

// #bugbug
// If we do not read the file, so the flag will not switch
// and we will not be able to write into the socket.

    if (n_reads <= 0){
        return (wid_t) -1;
    }

// The response message
// Recebemos alguma coisa.
// A mesagem pode ser de vários tipos.
// A mensagem que esperamos nesse caso é SERVER_PACKET_TYPE_REPLY.
// Porque estamos esperando resposta de um request.

    wid_t wid = (int) message_buffer[0];
    int msg = (int) message_buffer[1];

    switch (msg){
    // reply
    case SERVER_PACKET_TYPE_REPLY: 
        return (wid_t) wid;
        break;
    // error
    case SERVER_PACKET_TYPE_REQUEST:
    case SERVER_PACKET_TYPE_EVENT:
    case SERVER_PACKET_TYPE_ERROR:
    default:
        return (wid_t) -1;
        break;
    };

fail:
    return (wid_t) -1;
}

//
// == Functions ===================================
//

void 
gws_draw_char (
    int fd, 
    int window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    unsigned int ch )
{
// Draw char.

    int Response=0;
    int Value=0;
    int req_status = -1;

    if (fd<0)    {goto fail;}
    if (window<0){goto fail;}

// Request
    req_status = __gws_drawchar_request (
        (int) fd,             // fd
        (int) window,         // window id
        (unsigned long) x,    // left
        (unsigned long) y,    // top
        (unsigned int) (color & 0xFFFFFFFF),
        (unsigned int) (ch & 0xFF) );
    if (req_status<=0){
        goto fail;
    }
    rtl_set_file_sync( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// Response
// Waiting to read the response.
    //gws_debug_print("gws_draw_char: response\n");
    while (1){
        Value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        //if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) { goto done; }
        if (Value == ACTION_NULL )  { goto done; }  //no reponse. (syncronous)
    };

done:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (int) Response;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return -1;
}

void 
gws_draw_text (
    int fd, 
    int window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string )
{
// Draw text.
// #todo: void

    int response =0;
    int Value=0;
    int req_status = -1;

    if (fd<0)    { goto fail; }
    if (window<0){ goto fail; }

// String validation.
    if ((void*) string == NULL)
        goto fail;
    if (*string == 0)
        goto fail;


// Request
// IN: fd, window, x, y, color, string
    int count=0;
    for (count=0; count<8; count++){
    req_status = 
        (int) __gws_drawtext_request (
                  (int) fd,
                  (int) window,
                  (unsigned long) x,
                  (unsigned long) y,
                  (unsigned int) color,
                  (const char *) string );

    //if (req_status <= 0){
        //goto fail;
    //}
    if (req_status > 0){
        break;
    }
    };

    if (req_status <= 0){
        goto fail;
    }

    rtl_set_file_sync( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// ---------------------------------
// No response
// Response
// Waiting to read the response.
    //gws_debug_print("gws_draw_text: response\n");
    while (1){
        Value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        //if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) { goto done; }
        if (Value == ACTION_NULL )  { goto done; }  //no reponse. (syncronous)
        
        gws_debug_print("gws_draw_text: Waiting sync flag\n"); 
    };

done:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
}

//--------------------------------------

void 
gws_set_text (
    int fd, 
    int window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string )
{
// Inject a text into the text buffer of a window.
// Editbox only
// No reply

    int response =0;
    int Value=0;
    int req_status = -1;

    if (fd<0)    {goto fail;}
    if (window<0){goto fail;}

    if ( (void*) string == NULL )
        goto fail;
    if (*string == 0)
        goto fail;


// Request
// IN: fd, window, x, y, color, string
    req_status = 
        (int) __gws_settext_request (
                  (int) fd,
                  (int) window,
                  (unsigned long) x,
                  (unsigned long) y,
                  (unsigned long) (color & 0xFFFFFFFF),
                  (char *) string );
    if (req_status<=0){
        goto fail;
    }
    rtl_set_file_sync( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// Response
// Waiting to read the response.
    //gws_debug_print("gws_draw_text: response\n");
    while (1){
        Value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        //if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) { goto done; }
        if (Value == ACTION_NULL )  { goto done; }  //no reponse. (syncronous)
    };

done:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
}

//--------------------------------------

int 
gws_get_text (
    int fd, 
    int window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string )
{
// Get a text from the text buffer of a window.
// Editbox only

    char *where;
    where = string;

    int Value=0;
    int req_status = -1;

    if (fd<0)    {goto fail;}
    if (window<0){goto fail;}


    if ( (void*) string == NULL )
        goto fail;
    if (*string == 0)
        goto fail;


// Request
// IN: fd, window, x, y, color, string
    req_status = 
        (int) __gws_gettext_request (
                  (int) fd,
                  (int) window,
                  (unsigned long) x,
                  (unsigned long) y,
                  (unsigned long) (color & 0xFFFFFFFF),
                  (char *) string );
    if (req_status<=0){
        goto fail;
    }
    rtl_set_file_sync( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// Response
// Waiting to read the response.
    //gws_debug_print("gws_draw_text: response\n");
    while (1){
        Value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) {goto fail;}
        if (Value == ACTION_NULL )  {goto fail;}  //no reponse. (syncronous)
    };

    int c=0;
    char *p;
    p = (char *) __gws_gettext_response(fd);
    if ((void*) p == NULL){
        printf("gws_get_text: Invalid p\n");
        goto fail;
    }

// From 'p' to 'where'.
    if ((void*) p != NULL)
    {
        // O ponteiro dado pelo app.
        if ( (void*) where != NULL )
        {
            // Copy
            for (c=0; c<64; c++)
            {
                *where = *p;
                where++;
                p++;
            };
        }
    }

// status OK.
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (int) 0;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return -1;
}



// ========================================================
// clone and execute
//#todo: change arguments.
int 
gws_clone_and_execute2 (
    int fd, 
    unsigned long arg1,
    unsigned long arg2,
    unsigned long arg3,
    unsigned long arg4,
    const char *string )
{
// No reply

    int response=0;
    int Value=0;
    int req_status = -1;

    if (fd<0){
        goto fail;
    }

// path
    if ((void*) string == NULL)
        goto fail;
    if (*string == 0)
        goto fail;


// Request
// IN: fd, window, x, y, color, string
    req_status = 
        (int) __gws_clone_and_execute_request (
                  (int) fd,
                  (unsigned long) arg1,
                  (unsigned long) arg2,
                  (unsigned long) arg3,
                  (unsigned long) arg4,
                  (const char *) string );
    if(req_status<=0){
        goto fail;
    }
    rtl_set_file_sync( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// Response
// Waiting to read the response.
    //gws_debug_print("gws_draw_text: response\n");
    while (1){
        Value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) { goto fail; }
        if (Value == ACTION_NULL )  { goto fail; }  //no reponse. (syncronous)
    };

    response = (int) __gws_clone_and_execute_response(fd);

    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (int) response;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return -1;
}

// ========================================================
// Try to execute the command line in the prompt[] buffer.
void gws_clone_and_execute_from_prompt(int fd)
{
    char filename_buffer[32]; //8+3+1
// Limits:
// + The prompt[] limit is BUFSIZ = 1024;
// + The limit for the write() operation is 512 for now.
    size_t WriteLimit = 512;

// Clean the buffer.
    memset(filename_buffer,0,32);

    if (fd<0){
        return;
    }
// Empty buffer
   if (*prompt == 0){
       goto fail;
   }

// Clone.
// #important:
// For now the system will crash if the
// command is not found.
// #bugbug
// We are using the whole 'command line' as an argument.
// We need to work on that routine of passing
// the arguments to the child process.
// See: rtl.c
// Stop using the embedded shell.

// rebubina o arquivo de input.
    //rewind(__terminal_input_fp);
    
// ==================================

//
// Send commandline via stdin.
//

// Write it into stdin.
// It's working
// See: crt0.c

    //rewind(stdin);
    //prompt[511]=0;
    //write(fileno(stdin), prompt, 512);

    //fail
    //fprintf(stdin,"One Two Three ...");
    //fflush(stdin);

/*
// it's working
    char *shared_buffer = (char *) 0x30E00000;  //extra heap 3.
    sprintf(shared_buffer,"One Two Three ...");
    shared_buffer[511] = 0;
*/

// ==================================

//
// Get filename
//

// #bugbug
// The command line accepts only one word
// and the command line has too many words.

//#todo
//Create a method.
//int rtl_get_first_word_in_a_string(char *buffer_pointer, char *string);

    register int ii=0;
    char *p;
    p = prompt;

    while (1)
    {
        // Se tem tamanho o suficiente ou sobra.
        // 0~7 | 8 | 9~11
        if (ii >= 12){
            filename_buffer[12] = 0;  //finalize
            break;
        }

        // Se o tamanho esta no limite.
        
        // 0, space or tab.
        // Nao pode haver espace no nome do programa.
        // Depois do nome vem os parametros.
        if ( *p == 0 || 
             *p == ' ' ||
             *p == '\t' )
        {
            // Finalize the buffer that contain the image name.
            filename_buffer[ii] = 0;
            break;
        }

        // Printable.
        // Put the char into the buffer.
        // What are these chars? 
        // It includes symbols? Or just letters?
        if ( *p >= 0x20 && *p <= 0x7F )
        {
            filename_buffer[ii] = (char) *p;
        }

        p++;    // next char in the command line.
        ii++;   // next byte into the filename buffer.
    };

//
// Parse the filename inside its local buffer.
//

    register int i=0;
// Is it a valid extension?
// Pois podemos executar sem extensão.
    int isValidExt = FALSE;
    int dotWasFound = FALSE;
    int dot_found_in = 0;

// Look up for the first occorence of '.'.
// 12345678.123 = (8+1+3) = 12
// 0~7 | 8 | 9~11
    for (i=0; i<12; i++)
    {
        // The command name can't have these chars.
        // It means that we reached the end of the command name.
        // Maybe we have parameters after the name, maybe not.
        if ( filename_buffer[i] == 0 || 
             filename_buffer[i] == ' ' ||
             filename_buffer[i] == '\t' )
        {
            break;
        }        

        if ( filename_buffer[i] == '.' )
        {
            dotWasFound = TRUE;
            dot_found_in = i;
            isValidExt = FALSE;
            break;
        }
    };

// ----------------
// '.' was NOT found, but the filename is bigger than 8 bytes.
// The limit for the '.' if the byte 8. '01234567.'
    if (dotWasFound != TRUE)
    {
        //dot_found_in
        if (i > 8){
            printf("libgws: Long command name\n");
            goto fail;
        }
    }

// ----------------
// '.' was found.
// Se temos um ponto e 
// o que segue o ponto não é 'bin' ou 'BIN',
// entao a estencao e' invalida.

    if (dotWasFound == TRUE){
    if ( filename_buffer[i] == '.' )
    {
        // Ainda nao temos uma extensao valida.
        // Encontramos um ponto,
        // mas ainda não sabemos se a extensão é valida
        // ou não.
        // isInvalidExt = TRUE;
        
        // Valida a extensao se os proximos chars forem "bin".
        if ( filename_buffer[i+1] == 'b' &&
             filename_buffer[i+2] == 'i' &&
             filename_buffer[i+3] == 'n'  )
        {
            isValidExt = TRUE;
        }

        // Valida a extensao se os proximos chars forem "BIN".
        if ( filename_buffer[i+1] == 'B' &&
             filename_buffer[i+2] == 'I' &&
             filename_buffer[i+3] == 'N'  )
        {
            isValidExt = TRUE;
        }
    }
    }

// No extension
// The dot was found, but the extension is invalid.
// Invalid extension.
    if (dotWasFound == TRUE)
    {
        if (isValidExt != TRUE){
            printf("libgws: Invalid extension in command name\n");
            goto fail;
        }
    }

//
// Clone and execute.
//


//#todo
// Tem que limpar o buffer do arquivo em ring0, 
// antes de escrever no arquivo.

// cmdline:
// Only if the name is a valid name.
    rewind(stdin);
    //off_t v=-1;
    //v=lseek( fileno(stdin), 0, SEEK_SET );
    //if (v!=0){
    //    printf("testing lseek: %d\n",v);
    //    asm("int $3");
    //}

// Finalize the command line.
// Nao pode ser maior que o buffer.
    if (WriteLimit > PROMPT_MAX_DEFAULT){
        WriteLimit = PROMPT_MAX_DEFAULT;
    }
    int __LastChar = (int) (WriteLimit-1);
    prompt[__LastChar]=0;

    // #debug
    // OK!
    //printf("promt: {%s}\n",prompt);
    //asm ("int $3");

// #bugbug: 
// A cmdline ja estava dentro do arquivo
// antes de escrevermos. Isso porque pegamos mensagens de
// teclado de dentro do sdtin.
// Tambem significa que rewind() não funcionou.
// #test
// Nao pode ser maior que o limite atual para operaçoes de escrita.
    if (WriteLimit > 512){
        WriteLimit = 512;
    }
    //write(fileno(stdin), "dirty", 5);
    write(fileno(stdin), prompt, WriteLimit);

    //rtl_clone_and_execute(filename_buffer);
    //rtl_clone_and_execute(prompt);
    //rtl_clone_and_execute("shutdown.bin");
    // while(1){}
        
    // #todo #test
    // This is a method for the whole routine above.
    // rtl_execute_cmdline(prompt);

// clone and execute via ws.
// four arguments and a string pointer.

    int res = -1;

    //gws_clone_and_execute2(
    //    fd,
    //    0,0,0,0,
    //    "reboot.bin" );

    res = 
        (int) gws_clone_and_execute2(
                  fd,
                  0,0,0,0,
                  filename_buffer );

   if (res<0){
       //#debug #todo: do not use printf.
       //printf("gws_clone_and_execute2: fail\n");
   }

// #bugbug
// breakpoint
// something is wrong when we return here.
    
    //printf("terminal: breakpoint\n");
    //while(1){}

// #bugbug: 
// Se não estamos usando então
// o terminal vai sair do loop de input e fechar o programa.
    
    //isUsingEmbeddedShell = FALSE;
    //return;

    //printf("Command not found\n");
done:
    __gws_clear_msg_buff();
    return;
fail:
    __gws_clear_msg_buff();
    return;
}


/*
// #todo
// Services.
// This is a dialog with the library.
// We can use this for services just like initialization, shutdown ...
// IN: service number, ...
*/
void *gws_services ( 
    int service,
    unsigned long arg2,
    unsigned long arg3,
    unsigned long arg4 )
{
    // #todo
    //printf("gws_services: #todo\n");
    return NULL;
}


// gws_send_message_to_process:
// Envia uma mensagem para a thread de controle de um dado processo.
// Dado o PID.
// #obs
// Dá pra criar uma função semelhante, 
// que use estrutura ao invés de buffer.

int
gws_send_message_to_process ( 
    int pid, 
    int window, 
    int message,
    unsigned long long1,
    unsigned long long2 )
{
    unsigned long message_buffer[8];
    unsigned long Value=0;


    if (pid<0){
        gws_debug_print ("gws_send_message_to_process: pid\n");
        return (int) (-1);
    }

    //if(message<0){
    //    return -1;
    //}


// wid, message code, long1, long2.
    message_buffer[0] = (unsigned long) (window & 0xFFFFFFFF);
    message_buffer[1] = (unsigned long) (message & 0xFFFFFFFF);
    message_buffer[2] = (unsigned long) long1;
    message_buffer[3] = (unsigned long) long2;

    message_buffer[4] = 0;  // this pid
    message_buffer[5] = 0;  // tid
    // ...

    Value = 
        (unsigned long) gws_system_call ( 
                            112, 
                            (unsigned long) &message_buffer[0], 
                            (unsigned long) (pid & 0xFFFFFFFF), 
                            (unsigned long) (pid & 0xFFFFFFFF) );

// #todo
// Error message
    return (int) (Value & 0xF);
}

// gws_send_message_to_thread:
// Envia uma mensagem para uma thread.

int 
gws_send_message_to_thread ( 
    int tid, 
    int window, 
    int message,
    unsigned long long1,
    unsigned long long2 )
{
    unsigned long message_buffer[8];
    unsigned long Value=0;

    if (tid<0){
        gws_debug_print ("gws_send_message_to_thread: tid\n");
        return (int) (-1);
    }

// wid, message code, long1, long2
    message_buffer[0] = (unsigned long) (window & 0xFFFFFFFF);
    message_buffer[1] = (unsigned long) (message & 0xFFFFFFFF);
    message_buffer[2] = (unsigned long) long1;
    message_buffer[3] = (unsigned long) long2;

    message_buffer[4] = 0;  // this tid
    message_buffer[5] = 0;  // tid
    // ...

    Value = 
        (unsigned long) gws_system_call ( 
                            117, 
                            (unsigned long) &message_buffer[0], 
                            (unsigned long) (tid & 0xFFFFFFFF), 
                            (unsigned long) (tid & 0xFFFFFFFF) );

// #todo
// Error message
    return (int) (Value & 0xF);
}

// Reboot via ws.
// gwssrv.bin has the permission for this job.
void gws_reboot(int fd)
{
    if (fd<0){
        return;
    }
    gws_async_command(fd,89,0,0);
}

// Poweroff via ws.
void gws_shutdown(int fd)
{
    if (fd<0){
        return;
    }
    gws_async_command(fd,22,0,0);
}

// #todo: Explain it better.
void gws_update_desktop(int fd)
{
    if (fd<0){
        return;
    }
    gws_async_command(fd,11,0,0);
}


/*
// Se o window server entregar um evento ping,
// então respondemos com um evento pong.
// #todo: precisamos nos idntificar. Talvez nossa tid, talvez
// nosso pid, talvez a main_window ou a mesma janela
// indicada no evento.
void gws_pong(int fd);
void gws_pong(int fd)
{
    if(fd<0){
        return;
    }
    gws_async_command(fd,??,0,0);
}
*/

// Load a file using a pathname as an argument.
// #todo: Explain the return value.
int 
gws_load_path ( 
    const char *path, 
    unsigned long buffer, 
    unsigned long buffer_len )
{
    unsigned long Value=0;

// string
    if ((void*) path == NULL){
        return -1;
    }
    if ( *path == 0 ){
         return -1;
    }
    if (buffer == 0)    { return -1; }
    if (buffer_len == 0){ return -1; }

// #todo
// Chame a rtl e não uma syscall.

    Value = 
        (unsigned long) gws_system_call ( 
                            4004, 
                            (unsigned long) path, 
                            (unsigned long) buffer, 
                            (unsigned long) buffer_len );

// #todo
// Error message.

    return (int) (Value & 0xF);
}


// Window position.
int 
gws_change_window_position ( 
    int fd, 
    int window, 
    unsigned long x, 
    unsigned long y )
{
    int Value=0;
    int req_status = -1;

    if (fd<0)    { goto fail; }
    if (window<0){ goto fail; }

// Request
    req_status = 
        (int) __gws_change_window_position_request(fd,window,x,y);
    if (req_status <= 0){
        goto fail;
    }
    rtl_set_file_sync ( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );    

// Response
// Waiting to read the response.
    while (1){
        Value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) { goto fail; }
    };

    __gws_change_window_position_reponse(fd);

    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return 0;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return -1;
}

// Resize window.
int 
gws_resize_window( 
    int fd, 
    int window, 
    unsigned long w, 
    unsigned long h )
{
    int Value=0;
    int req_status=-1;

    if (fd<0)    {goto fail;}
    if (window<0){goto fail;}

// request
    req_status = (int) __gws_resize_window_request(fd,window,w,h);
    if (req_status <= 0){
        goto fail;
    }
    rtl_set_file_sync ( fd, SYNC_REQUEST_SET_ACTION, ACTION_REQUEST );

// response
// Waiting to read the response.
    while (1){
        Value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) {goto fail;}
    };

    __gws_resize_window_reponse(fd);

    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return 0;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return -1;
}

// Redraw window.
void 
gws_redraw_window (
   int fd, 
   int window, 
   unsigned long flags )
{
// No reply

    unsigned long Value=0;
    int req_status = -1;

    if (fd<0)    {goto fail;}
    if (window<0){goto fail;}

// #todo
// check the return values.

// Request
    req_status = (int) __gws_redraw_window_request (fd,window,flags);
    if (req_status <= 0){
        goto fail;
    }
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REQUEST );

// Response
// Waiting to read the response.
    while (TRUE){
        Value = (unsigned long) rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        //if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) {goto done;}
        if (Value == ACTION_NULL )  {goto done;}  //no reponse. (syncronous)
    };

done:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
}

// The server will return an event 
// from the its client's event queue.

struct gws_event_d *gws_get_next_event(
    int fd,
    int wid,
    struct gws_event_d *event )
{
    struct gws_event_d *e;
    unsigned long Value=0;
    int req_status = -1;

    if (fd<0){
        debug_print("gws_get_next_event: fd\n");
        goto fail;
    }
    if ( (void*) event == NULL )
        goto fail;

// #todo: 
// Check event pointer validation.
    /*
    if (wid<0){
        return NULL;
    }
    */

// Request
    req_status = (int) __gws_get_next_event_request(fd,wid);
    if (req_status <= 0){
        goto fail;
    }
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REQUEST );

// Response
// Waiting to read the response.
    while (TRUE){
        Value = (unsigned long) rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_REPLY){ break; }
        if (Value == ACTION_ERROR){
            goto fail;
        }
    };

    e = (struct gws_event_d *) __gws_get_next_event_response (fd,event);
    if ((void*) e == NULL){
        debug_print("gws_get_next_event: fail\n");
        // #todo: goto fail;
    }

// #ok
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (struct gws_event_d *) e;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return NULL;
}

// gws_get_window_info:
// The server will return the info about one given window.
struct gws_window_info_d *gws_get_window_info(
    int fd,
    int wid,
    struct gws_window_info_d *window_info ) 
{
    struct gws_window_info_d *wi;
    unsigned long Value=0;
    int req_status = -1;

    if (fd<0){
        //debug_print("gws_get_window_info: fd\n");
        goto fail;
    }
    if (wid<0){
        goto fail;
    }
    if ((void*)window_info == NULL)
        goto fail;

// Request
    req_status = (int) __gws_get_window_info_request(fd,wid);
    if (req_status <= 0){
        goto fail;
    }
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REQUEST );

// Response
// Waiting to read the response.
    while (TRUE){
        Value = (unsigned long) rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_REPLY){ break; }
        if (Value == ACTION_ERROR){
            goto fail;
        }
    };

    wi = 
        (struct gws_window_info_d *) __gws_get_window_info_response( 
                                         fd, 
                                         window_info );

    if ((void*) wi == NULL){
        debug_print("gws_get_window_info: fail\n");
        goto fail;
    }

// ok
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (struct gws_window_info_d *) wi;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return NULL;
}

// gws_query_window:
// The server will return the info about one given window.
struct gws_window_info_d *gws_query_window(
    int fd,
    int wid,
    struct gws_window_info_d *window_info )
{
    if (fd<0){
        goto fail;
    }
    if (wid<0){
        goto fail;
    }
    if ((void*) window_info == NULL){
        goto fail;
    }
// ok?
    return (struct gws_window_info_d *) gws_get_window_info(fd,wid,window_info);
fail:
    //__gws_clear_msg_buff();
    return NULL;
} 

// gws_refresh_window:
// Refresh window.
// #bugbug
// It is not working.
// The window server can not get the window number.

void 
gws_refresh_window(int fd, wid_t wid)
{
// no reply

    int value=0;
    int req_status=-1;

    if (fd<0) {goto fail;}
    if (wid<0){goto fail;}

// Request
    req_status = (int) __gws_refresh_window_request(fd,wid);
    if (req_status<=0){
        goto fail;
    }
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REQUEST );

// Response
// Waiting to read the response.
    while (TRUE){
        value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (value == ACTION_REPLY ) { break; }
        if (value == ACTION_ERROR ) {goto done;}
        if (value == ACTION_NULL )  {goto done;}  //no reponse. (syncronous)
    };

done:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
}

int
gws_refresh_retangle ( 
    int fd,
    unsigned long left, 
    unsigned long top, 
    unsigned long width, 
    unsigned long height )
{
    int Response=0;
    int Value=0;
    int req_status=-1;

    if (fd<0){
        goto fail;
    }

// Request
    req_status = 
        (int) __gws_refresh_rectangle_request (
                  (int) fd,
                  (unsigned long) left, 
                  (unsigned long) top,
                  (unsigned long) width, 
                  (unsigned long) height );

    if (req_status <= 0){
        goto fail;
    }
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REQUEST );

// Response
// Waiting to read the response.
    //gws_debug_print("gws_refresh_retangle: response\n");
    while (1){
        Value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_REPLY) { break; }
        if (Value == ACTION_ERROR) { goto fail; }
        if (Value == ACTION_NULL)  { goto fail; }  //no reponse. (syncronous)
    };

// A sincronização nos diz que já temos um reply.
    Response = (int) __gws_refresh_rectangle_response (fd);  
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (int) Response;
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (int) -1;
}

// Atualiza o retângulo da surface da thread.
void 
setup_surface_retangle ( 
    unsigned long left, 
    unsigned long top, 
    unsigned long width, 
    unsigned long height )
{
    unsigned long buf[5];
    buf[0] = (unsigned long) left;
    buf[1] = (unsigned long) top;
    buf[2] = (unsigned long) (width  & 0xFFFF);
    buf[3] = (unsigned long) (height & 0xFFFF);
    buf[4] = 0;
    sc80 ( 892, (unsigned long) &buf[0], 0, 0 );
}

void libgws_invalidate_surface_rectangle(void)
{
    sc80( 893, 0, 0, 0 );
}

// Invalidate window.
// Async.
void gws_invalidate_window(int fd,int wid)
{
    if (fd<0){
        return;
    }
    if (wid<0){
        return;
    }
    gws_async_command(fd,13,0,wid);
}


/*
 * gws_create_window: 
 *     Create a window.
 *     Given it's type.
 */
// OUT: wid

wid_t
gws_create_window ( 
    int fd,
    unsigned long type,        //1, Tipo de janela (popup,normal,...)
    unsigned long status,      //2, Estado da janela.
    unsigned long view,        //3, (min, max ...)
    const char *windowname,          //4, Título.                          
    unsigned long x,           //5, Deslocamento em relação às margens do Desktop.                           
    unsigned long y,           //6, Deslocamento em relação às margens do Desktop.
    unsigned long width,       //7, Largura da janela.
    unsigned long height,      //8, Altura da janela.
    int parentwindow,          //9, Endereço da estrutura da janela mãe.
    unsigned long style,       //10. style
    unsigned int clientcolor,  //11, Cor da área de cliente
    unsigned int color )       //12, Color (bg) (para janela simples).
{
    int value=0;
    wid_t wid = -1;
    //char *Name;
    int req_status=-1;

    //gws_debug_print("gws_create_window:\n");

    if (fd<0){
        goto fail;
    }

// name
    if ((void*) windowname == NULL){
        goto fail;
    }
    if ( *windowname == 0 )
        goto fail;

//#bugbug: parentwindow?

    //#todo
    //Um argumento passa o display usado
    //o display aponta para o socket a ser usado
    //display->fd

    //#todo
    // use more arguments.


// Local pointer

    //Name = windowname;
    //if ((void*) Name == NULL){
    //    Name = title_when_no_title;
    //}

// Request
    req_status = 
        (int) __gws_createwindow_request ( 
                  fd, 
                  x, y, width, height, 
                  color, 
                  type, 
                  status,
                  view,
                  style, 
                  parentwindow, 
                  (const char *) windowname );

    if (req_status <= 0){
        goto fail;
    }

    //ok set the sync
    rtl_set_file_sync( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// Response
// Waiting to read the response.
// Return the index returned by the window server.
// ??
// E se o arquivo for fechado pelo sistema?
// Ficaremos aqui para sempre?

    while (1){
        value = 
            (int) rtl_get_file_sync( 
                      fd, 
                      SYNC_REQUEST_GET_ACTION );

        //if (value == ACTION_REQUEST){}
        if (value == ACTION_REPLY){ break; }
        if (value == ACTION_ERROR){
            goto fail;
        }
        //if (value == ACTION_NULL){ goto fail; }  // no reply
    };

// A sincronização nos diz que já temos um reply.
// Simply read the file.
    wid = (wid_t) __gws_createwindow_response(fd); 
    
// #test
// Probably this is the root window.
    if (wid == 0)
        goto fail;

    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (wid_t) wid;

fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (wid_t) -1;
}

// Wrapper
wid_t 
gws_create_application_window(
    int fd,
    const char *windowname,         // Título. #todo maybe const char.
    unsigned long x,          // Deslocamento em relação às margens do Desktop. 
    unsigned long y,          // Deslocamento em relação às margens do Desktop.
    unsigned long width,      // Largura da janela.
    unsigned long height )    // Altura da janela.
{
// style: 
// 0x0001=maximized | 0x0002=minimized | 0x0004=fullscreen | 0x0008 statusbar

    wid_t wid = -1;
    unsigned long type = WT_OVERLAPPED;
    unsigned long status = WINDOW_STATUS_ACTIVE;
    unsigned long view = VIEW_NULL;
    wid_t parent_wid = 0;
    unsigned long style = 0;
    unsigned int client_color = COLOR_GRAY;
    unsigned int frame_color = COLOR_GRAY;

    if (fd<0)
        goto fail;

    if ((void*) windowname == NULL)
        goto fail;
    if (*windowname == 0)
        goto fail;

    wid = 
        (wid_t) gws_create_window ( 
                  fd,
                  type,
                  status,
                  view,
                  windowname,
                  x, y, width, height,
                  parent_wid,   // No parent
                  style, 
                  client_color, 
                  frame_color );

    return (wid_t) wid;
fail:
    return (wid_t) -1;
}






// Yield current thread.
void gws_yield(void)
{
    sc82(265,0,0,0);
}

// Refresh the background and yield the current thread.
void gws_refresh_yield (int fd)
{
    if (fd<0){
        return;
    }
    gws_refresh_window (fd, -4);  //??: fd,window
    gws_yield();
}

// Refresh a given window and yield the current thread.
void gws_refresh_yield2 (int fd, int window)
{
    if (fd<0)    { return; }
    if (window<0){ return; }
    gws_refresh_window (fd, window);
    gws_yield();
}

void gws_yield_n_times(unsigned long n)
{
    long it=0;
    if (n == 0){
        n=1;
    }
    for (it=0; it<n; it++){
        gws_yield();
    };
}

/*
 * gws_create_thread:
 *     Create a thread.
 *     #todo: 
 *     Precisamos uma função que envie mais argumentos.
 *     Essa será uma rotina de baixo nível para pthreads.
 *     Use const char
 */
// OUT:
// Is it a pointer to the ring0 thread structure?

void *gws_create_thread ( 
    unsigned long init_eip, 
    unsigned long init_stack, 
    char *name )
{
    //#define	SYSTEMCALL_CREATETHREAD     72
    debug_print ("gws_create_thread:\n");

    if (init_eip == 0){
        debug_print ("init_eip\n");
        goto fail;
    }
    if (init_stack == 0){
        debug_print ("init_stack\n");
        goto fail;
    }
    if ((void*) name == NULL){
        debug_print ("name\n");
        goto fail;
    }
    if (*name == 0){
        debug_print ("*name\n");
        goto fail;
    }

// OUT:
// Is it a pointer to the ring0 thread structure?

    //SYSTEMCALL_CREATETHREAD,
    return (void *) gws_system_call ( 
                        72,  
                        init_eip, 
                        init_stack, 
                        (unsigned long) name );
fail:
    // Message?
    return NULL;
}

/*
 * gws_start_thread:
 *     Coloca no estado standby para executar pela primeira vez
 */

void gws_start_thread (void *thread)
{
    //unsigned long ret_value=0;
    
    // #define	SYSTEMCALL_STARTTHREAD  94 

    debug_print ("gws_start_thread:\n");

    // Is it a pointer to the ring0 thread structure?

    if ((void*) thread == NULL){
        debug_print ("thread\n");
        return;
    }

    //SYSTEMCALL_STARTTHREAD,
    sc80 ( 
        94, 
        (unsigned long) thread, 
        (unsigned long) thread, 
        (unsigned long) thread );
}


// Clone the current process and execute the clone.
// #todo: Use 'const char*'.
// OUT: ??

int gws_clone_and_execute(const char *name)
{
    unsigned long Value=0;

    if ((void*) name == NULL){ 
        debug_print("gws_clone_and_execute: name\n");
        goto fail;
    }
    if ( *name == 0 ){ 
        debug_print("gws_clone_and_execute: *name\n");
        goto fail; 
    }

    // #todo
    // Ret = (int) rtl_clone_and_execute(name);

    Value = 
        (unsigned long) sc82( 900, (unsigned long) name, 0, 0 );

// #todo
// Error message
    return (int) (Value & 0xF);
fail:
    return (int) -1;
}

// Get system metrics.
unsigned long gws_get_system_metrics (int index)
{
    if (index<0){
        return 0;
    }
    return (unsigned long) rtl_get_system_metrics( (int) index );
}


// Enter critical section
// close the gate
// Pega o valor do spinlock principal.
// Se deixou de ser 0 então posso entrar.
// Se ainda for 0, continuo no while.
// Depois de entrar, fecha o portão.
// TRUE = OPEN.
// FALSE = CLOSED.
// yield thread if closed.

void gws_enter_critical_section (void)
{
    unsigned long GateStatus=0;  // Closed.
// Waiting the value 1.
    while (TRUE)
    {
        // Get the gate status.
        GateStatus = (unsigned long) gws_system_call ( 226, 0, 0, 0 );
        // Enter if the gate is open.
        if (GateStatus == 1){
            goto done;
        }
    };
// Close the gate. turn FALSE.
done:
    gws_system_call ( 227, 0, 0, 0 );
    return;
}

// exit critical section
// open the gate.
void gws_exit_critical_section (void)
{
    gws_system_call ( 228, 0, 0, 0 );
}


/*
//refresh raw rectangle
//refresh raw rectangle
int 
gws_refresh_rectangle(
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height);
int 
gws_refresh_rectangle(
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height)
{

   //todo
   //call request function (it uses 2021 service)
   //call response function
   return -1;
}
*/


// constructor
// Create the structure, given only a type.
/*
int gws_window ( int fd, int type );
int gws_window ( int fd, int type )
{
    int wid;
    wid = gws_create_window_socket (client_fd,
        WT_SIMPLE,1,1,"Browser",
        40, 40, 640, 480,
        0,0,COLOR_GRAY, COLOR_GRAY);

    return wid;
}
*/


// Create menu and return a pointer to a menu structure.
// This a good thing when 
// creating a window manager as a client application.
// The synchronization is made when we call gws_create_window.

struct gws_menu_d *gws_create_menu (
    int fd,
    int parent,
    int highlight,
    int count,
    unsigned long x,
    unsigned long y,
    unsigned long width,
    unsigned long height,
    unsigned int color )
{
    struct gws_menu_d  *menu;
    int window=0;

    if (fd<0){
        debug_print("gws_create_menu: fd\n");
        goto fail;
    }

// #todo: Check parent validation
    if (parent<0){
    }

// #todo: 'count' limits.

// Menu
    menu = 
        (struct gws_menu_d *) gws_malloc( sizeof(struct gws_menu_d) );
    if ( (void *) menu == NULL ){
        debug_print("gws_create_menu: [FAIL] menu\n");
        goto fail;
    }
// Offset related to the window
    menu->x = x;
    menu->y = y;
// Dimensions
    menu->width  = width;
    menu->height = height;
    menu->color = (unsigned int) color;
// ??
// Are we selection am item
    menu->highlight = highlight;
// Number of itens.
    menu->itens_count = count;

// Create menu window

    window = 
        (int) gws_create_window ( 
                  fd,
                  WT_SIMPLE, 1, 1, "Menu",
                  menu->x,  //Deslocamento em relação a janela mãe. 
                  menu->y,  //Deslocamento em relação a janela mãe. 
                  width, 
                  height,
                  parent, 0, color, color );

    if (window <= 0){
        debug_print("gws_create_menu: [FAIL] window\n");
        //gws_free(menu);
        menu->window = 0;  //#bugbug !!!!
        goto fail;
    }

// Saving 
// #bugbug
    menu->window = (int) window;
    menu->parent = (int) parent;

// Return the pointer.
    return (struct gws_menu_d *) menu;
fail:
    return NULL;
}

// Create a menu item for a given valid menu.
// This a good thing when 
// creating a window manager as a client application.
// The synchronization is made when we call gws_create_window.
struct gws_menu_item_d *gws_create_menu_item (
    int fd,
    const char *label,
    int id,
    struct gws_menu_d *menu)
{
    struct gws_menu_item_d  *item;
    int window=0;    //menu item window

    if (fd<0){
        debug_print("gws_create_menu_item: fd\n");
        return (struct gws_menu_item_d *) 0;
    }
    if ((void*)label == NULL)
        return NULL;
    if ( (void *) menu == NULL ){
        debug_print("gws_create_menu_item: menu\n");
        return (struct gws_menu_item_d *) 0;
    }

// Create menu item.
    item = 
        (struct gws_menu_item_d *) gws_malloc( sizeof(struct gws_menu_item_d) );

    if ( (void *) item == NULL ){
        debug_print("gws_create_menu_item: item\n");
        return (struct gws_menu_item_d *) 0;
    }

// ID
// Provisório

    if (id>5 || id>menu->itens_count)
    {
        // ? msg ?
        return (struct gws_menu_item_d *) 0;
    }
    item->id = id;

    item->width  = (menu->width -8);
    item->height = (menu->height / menu->itens_count);
    item->x = 4;
    item->y = (item->height*id);
    
    //item->color = COLOR_GRAY;

// Create a window for a menu item.

    if ( menu->window > 0 )
    {
        window = 
            (int) gws_create_window ( 
                      fd,
                      WT_BUTTON,1,1, label,
                      item->x, 
                      item->y, 
                      item->width, 
                      item->height,
                      menu->window,  // #bugbug 
                      0, COLOR_GRAY, COLOR_GRAY );

        //debug
        //gws_draw_char (
            //fd, 
             //menu->window, // #bugbug
              //0,
              //0,
              //COLOR_RED,
              //'x');

         item->window = window;
    }

// Return the pointer fot the menu item.
    return (struct gws_menu_item_d *) item;
}

// Expand a byte all over the long.
// #todo: This is valid only for 32bit 'unsigned int'
// We need to create another one for 8bytes long.
unsigned int gws_explode_byte_32(unsigned char data)
{
    return (unsigned int) (data << 24 | data << 16 | data << 8 | data);
}

// Create empty file.
int gws_create_empty_file(const char *file_name)
{

// #todo
// Use rtl, not a systemcall.

    unsigned long Value=0;

// name
    if ((void*) file_name == NULL){
        debug_print("gws_create_empty_file: [FAIL] file_name\n");
        return (int) -1;
    }
    if ( *file_name == 0 ){
        debug_print("gws_create_empty_file: [FAIL] *file_name\n");
        return (int) -1;
    }

    Value = 
        (unsigned long) sc80 ( 
                            43, 
                            (unsigned long) file_name, 
                            0, 
                            0 );

// #todo
// Error message.

    return (int) (Value & 0xF);
}

// Create empty directory.
int gws_create_empty_directory(const char *dir_name)
{

// #todo
// Use rtl, not a systemcall.

    unsigned long Value=0;

// name
    if ((void*) dir_name == NULL){
        debug_print("gws_create_empty_directory: [FAIL] dir_name\n");
        return (int)(-1);
    }
    if ( *dir_name == 0 ){
        debug_print("gws_create_empty_directory: [FAIL] *dir_name\n");
        return (int)(-1);
    }

// #todo
// Quais são os valores de retorno.
// TRUE or FALSE ?

    Value = 
        (unsigned long) sc80 ( 
                            44, 
                            (unsigned long) dir_name, 
                            0, 
                            0 );

// #todo
// Error message

    return (int) (Value & 0xF);
}

// Destroy overlapped window.
// (Application window).
void gws_destroy_window(int fd, wid_t wid)
{
    if (fd<0){
        return;
    }
    if (wid<0){
        return;
    }
// IN: fd, request, sub-request, data.
    gws_async_command( fd, 90, 0, wid );
}

void gws_set_active(int fd, wid_t wid)
{
    if (fd<0) { return; }
    if (wid<0){ return; }
// IN: fd, request, sub-request, data.
    gws_async_command( fd, 15, 0, wid );
}

// Set focus
// async request.
void gws_set_focus(int fd, wid_t wid)
{
    if (fd<0) { return; }
    if (wid<0){ return; }
// IN: fd, request, sub-request, data.
    gws_async_command( fd, 9, 0, wid );
}

// Clear the window
// Repaint it using the default background color.
// Only valid for WT_SIMPLE.
// async request.
void gws_clear_window(int fd, wid_t wid)
{
    if (fd<0) { return; }
    if (wid<0){ return; }
// IN: fd, request, sub-request, data.
    gws_async_command( fd, 14, 0, wid );
}

// Send async request.
// No response.
// #todo
// List some standard commands accepted by the ws.
// IN: 
// fd, request number, subrequest.
// #todo: We need a list of services we can all with this function.
void
gws_async_command ( 
    int fd, 
    unsigned long request,
    unsigned long sub_request,
    unsigned long data )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes=0;
    int Value=0;
    register int i=0;

// Enviamos um request para o servidor.
// Precisamos mesmo de um loop para isso?
// Write!
// Se foi possível enviar, então saimos do loop.  
// Nesse caso, corremos o risco de ficarmos presos
// caso não seja possível escrever.

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

// wid, message code, request, subrequest, data1
    message_buffer[0] = 0;
    message_buffer[1] = GWS_AsyncCommand;
// #todo: We need a list of services we can all with this function.
    message_buffer[2] = request;           // request
    message_buffer[3] = sub_request;       // sub request
// data
// #todo: We can deliver more data if we want.
    message_buffer[4] = data;  // data1
    // ...

// Parameters
    if (fd<0){
        debug_print("gws_async_command: fd\n");
        goto fail;
    }
// ...

// Sending ...
    n_writes = 
        (int) send ( 
                  fd,
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes <= 0){
        goto fail;
    }

    rtl_set_file_sync ( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// No return.
    while (1){
        Value = 
            (int) rtl_get_file_sync( 
                      fd, 
                      SYNC_REQUEST_GET_ACTION );
        // Essa é a sincronização esperada.
        // Não teremos uma resposta, mas precisamos
        // conferir a sincronização.
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_NULL )  { goto done; }
        if (Value == ACTION_ERROR ) { goto done; }
        //#debug
        gws_debug_print ("gws_async_command: Waiting sync flag\n"); 
    };

done:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
}

void
gws_async_command2 ( 
    int fd, 
    unsigned long request,
    unsigned long sub_request,
    unsigned long data1,
    unsigned long data2,
    unsigned long data3,
    unsigned long data4 )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gws_message_buffer[0];
    int n_writes=0;
    int Value=0;
    register int i=0;

    // #debug
    // gws_debug_print ("gws_async_command2: send...\n"); 

// Enviamos um request para o servidor.
// Precisamos mesmo de um loop para isso?
// Write!
// Se foi possível enviar, então saimos do loop.  
// Nesse caso, corremos o risco de ficarmos presos
// caso não seja possível escrever.

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gws_message_buffer[i] = 0;

// Window ID
    message_buffer[0] = 0;
// Message code. (2222?)
    message_buffer[1] = GWS_AsyncCommand;
// #todo: We need a list of services we can all with this function.
    message_buffer[2] = request;           // request
    message_buffer[3] = sub_request;       // sub request
// data
// #todo: We can deliver more data if we want.
    message_buffer[4] = (unsigned long) data1;  // data1
    message_buffer[5] = (unsigned long) data2;  // data2
    message_buffer[6] = (unsigned long) data3;  // data3
    message_buffer[7] = (unsigned long) data4;  // data4
    // ...

    if (fd<0){
        debug_print("gws_async_command2: fd\n");
        goto fail;
    }

    n_writes = 
        (int) send ( 
                  fd,
                  __gws_message_buffer, 
                  sizeof(__gws_message_buffer), 
                  0 );

    if (n_writes <= 0){
         goto fail;
    }

    rtl_set_file_sync ( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// No return.
    while (1){
        Value = 
            (int) rtl_get_file_sync( 
                      fd, 
                      SYNC_REQUEST_GET_ACTION );
        // Essa é a sincronização esperada.
        // Não teremos uma resposta, mas precisamos
        // conferir a sincronização.
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_NULL )  { goto done; }
        if (Value == ACTION_ERROR ) { goto done; }
        //#debug
        gws_debug_print ("gws_async_command2: Waiting sync flag\n"); 
    };

done:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
fail:
    __gws_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
}

// #test
// No response.
// Sending the wm's pid to the ws.
// Don't trust in this method. There is no response.
// We can use this to broadcast.
void gws_send_wm_magic( int fd, int pid )
{
    if(fd<0) { return; }
    if(pid<0){ return; }
    gws_async_command(fd,7,0,pid);
}

// gws_open_display:
// + (1) Create the display structure.
// + (2) Create the socket file.
// + (3) Connect to the window server.
// IN: #todo: Explain it better.
// hostname:number.screen_number
// #todo
// It opens a display device ... 
// A device to control the screen access.
// 
struct gws_display_d *gws_open_display(const char *display_name)
{
    struct gws_display_d *Display;
    int DisplayID = 0; //?
    int client_fd = -1;

// ---------------------------------
    struct sockaddr_in  addr_in;
    addr_in.sin_family      = AF_INET;
    addr_in.sin_addr.s_addr = __IP(127,0,0,1);
    addr_in.sin_port        = __PORTS_DISPLAY_SERVER;
    int addrlen=0;
    addrlen = sizeof(addr_in);
// ---------------------------------

// #todo
// Open the display device
// and set the fd for the device for future 
// display configurations.
    // Display->_device_fd = ?

    if ((void*)display_name == NULL)
        goto fail;
    if (*display_name == 0)
        goto fail;

// (1)
// Create the display structure.

    Display = 
        (struct gws_display_d *) gws_malloc( sizeof(struct gws_display_d) );

    if ((void*) Display == NULL){
        printf("gws_open_display: Couldn't create display\n");
        goto fail;
    }

// (2)
// Create the socket file.
    //client_fd = socket( AF_INET, SOCK_STREAM, 0 );
    client_fd = socket( AF_INET, SOCK_RAW, 0 );
    if (client_fd<0){
       printf ("gws_open_display: Couldn't create socket\n");
       goto fail;
    }
// Network socket.
    Display->fd = (int) client_fd;
    Display->connected = FALSE;

    Display->lock      = FALSE;
    // ...

// Display name.
// #todo:
// Use a default name if we do not have a given one.

    if ((void*) display_name == NULL){
        printf ("gws_open_display: [FAIL] display_name\n");
        goto fail;
    }
    if (*display_name == 0){
        printf ("gws_open_display: [FAIL] *display_name\n");
        goto fail;
    }

// (3)
// Connect to the window server.

    while (TRUE){
        if (connect (client_fd, (void *) &addr_in, addrlen ) < 0){
            gws_debug_print("gws_open_display: Connection Failed\n");
            printf         ("gws_open_display: Connection Failed\n");
        }else{
            // Connected!
            break;
        };
    };


//
// Screens
//

// #todo: 
// This is a work in progress.
    Display->default_screen = 0;  // Current screen.
    Display->nscreens = 1;  // Number of screens.
    Display->screens = NULL;  // Screen list.

// ID
    Display->id = (int) DisplayID;

// Not running yet.
// The application sets this flag 
// when the server enters in a llop for requests.
    Display->running = FALSE;

// Flags
    Display->connected = TRUE;
    Display->used = TRUE;
    Display->magic = 1234;
// Current display
    gws_set_current_display(Display);

// Navigation.
    Display->next = NULL;

// Done
// Return the display structure pointer.
    __gws_clear_msg_buff();
    //rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return (struct gws_display_d *) Display;
fail:
    return NULL;
}

void gws_close_display(struct gws_display_d *display)
{
    // #todo
 
// Parameter
    if ((void*) display == NULL){
        gws_debug_print("gws_close_display: display\n");
        return;
    }
    if (display->used != TRUE)
        return;
    if (display->magic != 1234)
        return;

    display->connected = FALSE;
    display->running = FALSE;
    //display->lock=0;
    // ...

// #todo:
// Maybe, destroy the screen structure.
// ...

// Show display name.
    if ((void*) display->display_name != NULL)
    {
        // #bugbug: string size?
        printf("Closing the %s display for GWS\n",
            display->display_name );
    }

// #todo
// Close network socket for this application.
    // close(display->fd);

// ...

// Invalidate structure.
    display->used = FALSE;
    display->magic = 0;
    display = NULL;
}

// Change the current display for this library.
int gws_set_current_display(struct gws_display_d *display)
{
    if ((void*) display == NULL){
        goto fail;
    }
    if ( display->used != TRUE || display->magic != 1234 )
    {
        goto fail;
    }
// ok
    libgwsCurrentDisplay = (struct gws_display_d *) display;
// Done
    return 0;
fail:
    return (int) -1;
}

struct gws_display_d *gws_get_current_display(void)
{
    return (struct gws_display_d *) libgwsCurrentDisplay;
}

void gws_display_exit(struct gws_display_d *display)
{
    if ((void*) display == NULL){
        return;
    }
    if (display->magic != 1234){
        return;
    }
    display->running = FALSE;
}

/*
  // #todo
  // Send some pending requests.
void gws_flush_display ( struct gws_display_d *display );
void gws_flush_display ( struct gws_display_d *display )
{
}
*/

//test
struct gws_display_d *__ApplicationDisplay;
static int __ApplicationFD = -1;

int application_start(void)
{
// #todo: Explain this routine.
// + Open display.

    struct gws_display_d *Display;

// ============================
// Open display.
// IN: 
// hostname:number.screen_number

    Display = 
        (struct gws_display_d *) gws_open_display("display:name.0");

    if ((void*) Display == NULL)
    {
        debug_print ("application_start: Couldn't open display\n");
        printf      ("application_start: Couldn't open display\n");
        //exit(1);
        goto fail;
    }

    if (Display->fd <= 0)
    {
        debug_print ("application_start: bad Display->fd\n");
        printf      ("application_start: bad Display->fd\n");
        //exit(1);
        goto fail;
    }

    __ApplicationDisplay = (struct gws_display_d *) Display;
    __ApplicationFD = (int) Display->fd;

// Return the socket for a
// connected client application

    //return (int) Display->fd;
    return (int) __ApplicationFD;

fail:
    __ApplicationDisplay = NULL;
    __ApplicationFD = -1;
    exit(1);
    return -1;
}

// #todo: gws_application_end()
void application_end(void)
{
    // #todo
    // Close the current display.
    exit(0);
}

int gws_enable_input_method(int method)
{
    if (method<0){
        return (int) -1;
    }

    switch (method){
    // Event queue in the current thread.
    case 1:
        gws_debug_print ("gws_enable_input_method: [1] \n");
        rtl_focus_on_this_thread();
        return 0;
        break;
    // ...
    default:
        gws_debug_print ("gws_enable_input_method: [FAIL] Invalid method\n");
        break;
    };

fail:
    return (int) -1;
}

// Default procedure.
// Call the window server.
int 
gws_default_procedure (
    int fd, 
    int window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
    gws_debug_print ("gws_default_procedure:\n");

    if (fd<0){
        return (int) (-1);
    }
    if (msg<0){
        return 0;
    }

// Messages
// Vamos mandar para o servidor algumas
// mensagens que interessam ao window manager 
// dentro do window server.
// #todo
// Algumas chamadas de sistema podem ficar aqui,
// como client shutdown e coisas do tipo.
// Some menu stuff?
// Changes in the title bar... new text... redraw the bar.
// + close window.
// + F1: caso o aplicativo n~ao interceptou f1, mostraremos ajuda.
// foco na janela ou ativar.
// window position, window size.
// Set cursor. (that text cursor.)
// show window. (refresh)
// sysmenu, control menu
// copy global data.
// Change UI state.
    
    switch (msg){

    case MSG_SETFOCUS:
        return 0;
        break;

    // Close process
    // Para que o diálogo default feche o aplicativo,
    // esse parâmetro não pode ser '0'.
    // Se o kernel quiser fechar a aplicação, então tem que
    // colocar algum valor aqui.
    case MSG_CLOSE:
        if (long1 != 0){
            exit(0);
        }
        return 0;
        break;

    // #bugbug
    // Nesse caso podemos ter duplicidade de tratamento,
    // com o aplicativo fazendo um tratamento antes desse.
    case MSG_SYSKEYDOWN:
        switch (long1){
            case VK_F1:
                return 0;
                break;
            case VK_F12:
                return 0;
                break;
        };
        break;
        
        
    // Essa mensagem foi enviada pelo kernel.
    //case 11316:
        //printf("gsw_default_procedure: 11316\n");
        //exit(0);
        //gws_async_command(fd,8,1,1);
        //break;
    
    default:
        // printf("gsw_default_procedure: msg={%d}\n",msg);
        break;
    };
    
    // ...

    return 0;
}

/*
// Get thread info given the widnow id.
struct gws_thread_info_d *thread_info_from_wid(int fd, int wid);
struct gws_thread_info_d *thread_info_from_wid(int fd, int wid)
{
    //#todo
    return NULL;
}
*/


// #todo: Not tested yet.
unsigned int gws_rgb(int r, int g, int b)
{
    unsigned int Color = 
        (unsigned int) (r<<16 &0xff0000)|(g<<8 &0xff00)|(b &0xff); 
    return (unsigned int) Color;
}

// #todo: Not tested yet.
unsigned int gws_argb(int a, int r, int g, int b)
{
    unsigned int Color = 
        (unsigned int) (a<<24 &0xff000000)|(r<<16 &0xff0000)|(g<<8 &0xff00)|(b &0xff); 
    return (unsigned int) Color;
}



//
// End
//

