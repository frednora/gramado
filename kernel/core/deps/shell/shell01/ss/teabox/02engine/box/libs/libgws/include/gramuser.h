// gramuser.h
// User interactive components, not system components.
// Created by Fred Nora.

#ifndef __LIBGWS_GRAMUSER_H_
#define __LIBGWS_GRAMUSER_H_    1


// Explode byte.
unsigned int gws_explode_byte_32(unsigned char data);

unsigned int gws_rgb(int r, int g, int b);
unsigned int gws_argb(int a, int r, int g, int b);

//
// == 2d ==============================================
//

// Draw a char.
void 
gws_draw_char (
    int fd, 
    int window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    unsigned int ch );

//
// Text support
//

// Draw a text.
void 
gws_draw_text (
    int fd, 
    int window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string );

void 
gws_set_text (
    int fd, 
    int window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string );

int 
gws_get_text (
    int fd, 
    int window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string );


//
// Rectangle support
//

int
gws_refresh_retangle ( 
    int fd,
    unsigned long left, 
    unsigned long top, 
    unsigned long width, 
    unsigned long height );

//
// Surface support
//

// Atualiza o retângulo da surface da thread.
void 
setup_surface_retangle ( 
    unsigned long left, 
    unsigned long top, 
    unsigned long width, 
    unsigned long height );

// #todo: 
// change to gws_invalidate_surface_rectangle,
// or gws_invalidate-surface.
void libgws_invalidate_surface_rectangle (void);

// ==========================

//
// Window support
//

// Redraw a window.
void 
gws_redraw_window (
    int fd, 
    int window, 
    unsigned long flags );

// The server will return the info about one given window.
struct gws_window_info_d *gws_query_window(
    int fd,
    int wid,
    struct gws_window_info_d *window_info );

// The server will return the info about one given window.
struct gws_window_info_d *gws_get_window_info(
    int fd,
    int wid,
    struct gws_window_info_d *window_info );

// Refresh a window.
void 
gws_refresh_window (int fd, wid_t wid);

// Change window position.
int 
gws_change_window_position (
    int fd, 
    int window, 
    unsigned long x, 
    unsigned long y );

// Resize a window.
int 
gws_resize_window ( 
    int fd, 
    int window, 
    unsigned long w, 
    unsigned long h );



void gws_invalidate_window(int fd,int wid);

// Create a window.
wid_t
gws_create_window ( 
    int fd,
    unsigned long type,        //1, Tipo de janela (popup,normal,...)
    unsigned long status,      //2, Estado da janela (ativa ou nao) (button state)
    unsigned long view,        //3, (min, max ...)
    const char *windowname,    //4, Título. #todo maybe const char.
    unsigned long x,           //5, Deslocamento em relação às margens do Desktop. 
    unsigned long y,           //6, Deslocamento em relação às margens do Desktop.
    unsigned long width,       //7, Largura da janela.
    unsigned long height,      //8, Altura da janela.
    int parentwindow,          //9, Endereço da estrutura da janela mãe.
    unsigned long style,       //10, Ambiente.( Está no desktop, barra, cliente ...)
    unsigned int clientcolor,  //11, Cor da área de cliente
    unsigned int color );      //12, Color (bg) (para janela simples).

wid_t 
gws_create_application_window(
    int fd,
    const char *windowname,         // Título. #todo maybe const char.
    unsigned long x,          // Deslocamento em relação às margens do Desktop. 
    unsigned long y,          // Deslocamento em relação às margens do Desktop.
    unsigned long width,      // Largura da janela.
    unsigned long height );    // Altura da janela.


void gws_destroy_window(int fd, wid_t wid);
void gws_set_active(int fd, wid_t wid);
void gws_set_focus(int fd, wid_t wid);
void gws_clear_window(int fd, wid_t wid);

// wm initialization
void gws_send_wm_magic( int fd, int pid );



//
// Menu support.
//

// Menu
struct gws_menu_d *gws_create_menu (
    int fd,
    int parent,
    int highlight,
    int count,
    unsigned long x,
    unsigned long y,
    unsigned long width,
    unsigned long height,
    unsigned int color );

// Menu item
struct gws_menu_item_d *gws_create_menu_item (
    int fd,
    const char *label,
    int id,
    struct gws_menu_d *menu);

// ==========================

// #todo: Explain it better.
void gws_update_desktop(int fd);


// ==========================
//
// Event support
//

// The server will return an event 
// from the its client's event queue.
struct gws_event_d *gws_get_next_event(
    int fd, 
    int wid,
    struct gws_event_d *event );

// Send event.
// It is gonna be used by the window manager to send 
// events to the window server.
// Remember: The window manager gets the system messages
// end sends window events to the window server.
void 
gws_send_event ( 
    int fd, 
    int window, 
    struct gws_event_d *event );

// Dialog.
int 
gws_default_procedure ( 
    int fd,
    int window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

//
// Async request support.
//

void
gws_async_command ( 
    int fd, 
    unsigned long request,
    unsigned long sub_request,
    unsigned long data );

void
gws_async_command2 ( 
    int fd, 
    unsigned long request,
    unsigned long sub_request,
    unsigned long data1,
    unsigned long data2,
    unsigned long data3,
    unsigned long data4 );


#endif  



