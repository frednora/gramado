// main.c
// GDM - Gramado Display Manager.
// 2022 - Created by Fred Nora.

// Connecting via AF_INET.
// tutorial example taken from. 
// https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
/*
    To make a process a TCP server, you need to follow the steps 
    given below −
    Create a socket with the socket() system call.
    Bind the socket to an address using the bind() system call. 
    For a server socket on the Internet, an address consists of a 
    port number on the host machine.
    Listen for connections with the listen() system call.
    Accept a connection with the accept() system call. 
    This call typically blocks until a client connects with the server.
    Send and receive data using the read() and write() system calls.
*/ 
// See:
// https://wiki.osdev.org/Message_Passing_Tutorial
// https://wiki.osdev.org/Synchronization_Primitives
// ...

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>
// The client-side library.
#include <gws.h>

// Internal
#include "globals.h"
#include <packet.h>
#include <gdm.h>

// Ports
#define PORTS_WS  4040
//#define PORTS_NS  4041
//#define PORTS_FS  4042
// ...

#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)

/*
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port   = 7548, 
    .sin_addr   = IP(192, 168, 1, 79),
};
*/



// Screen
unsigned long screen_width=0;
unsigned long screen_height=0;
// gdm
unsigned long gdm_x=0;
unsigned long gdm_y=0;
unsigned long gdm_width=0;
unsigned long gdm_height=0;

//
// Windows
//

static int main_window = 0;
// bar1 | button1
static int bar1_window = 0;
static int button1_window = 0;
// bar2 | button2
static int bar2_window = 0;
static int button2_window = 0;

//-------------

struct child_window_d
{
    unsigned long l;
    unsigned long t;
    unsigned long w;
    unsigned long h;
};
struct child_window_d cwBar1;
struct child_window_d cwButton1;
struct child_window_d cwBar2;
struct child_window_d cwButton2;


// #todo
// int button_list[8];

// Cursor
static int cursor_x = 0;
static int cursor_y = 0;
static int cursor_x_max = 0;
static int cursor_y_max = 0;

static int blink_status=FALSE;

// tmp input pointer.
// #todo
// we will copy all the iput support from the other editor.
// for now we will use this tmp right here.
//int tmp_ip_x=8;
//int tmp_ip_y=8;

//char *hello = "Hello there!\n";

// ========


static void pump(int fd);
static void __event_loop(int fd);

static void do_done(int fd);

//prototype
static int 
gdmProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 );

static void update_clients(int fd);
static void destroy_windows(int fd);

static int __init_globals(void);
//int __init_windows(void);

// ========

static void update_clients(int fd)
{
    // Local
    struct gws_window_info_d lWi;

    if (fd<0){
        return;
    }

// Get info about the main window.
// IN: fd, wid, window info structure.
    gws_get_window_info(
        fd, 
        main_window,   // The app window.
        (struct gws_window_info_d *) &lWi );


// ---------------------------------------------
// cwBar1
    cwBar1.l = (( lWi.cr_width/8 )*2) +20; 
    cwBar1.t = 4;
    cwBar1.w = (( lWi.cr_width/8 )*2);
    cwBar1.h = 24;
    gws_change_window_position( 
        fd,
        bar1_window,
        cwBar1.l,
        cwBar1.t );
    gws_resize_window(
        fd,
        bar1_window,
        cwBar1.w,
        cwBar1.h );

// ---------------------------------------------
// cwButton1
    cwButton1.l = (( lWi.cr_width/8 )*5);
    cwButton1.t = 4;
    cwButton1.w = (( lWi.cr_width/8 )*2);
    cwButton1.h = 24;
    gws_change_window_position( 
        fd,
        button1_window,
        cwButton1.l,
        cwButton1.t );
    gws_resize_window(
        fd,
        button1_window,
        cwButton1.w,
        cwButton1.h );

// ---------------------------------------------
// cwBar2
    cwBar2.l = (( lWi.cr_width/8 )*2) + 20;
    cwBar2.t = (4 +(24) +4);
    cwBar2.w = (( lWi.cr_width/8 )*2);
    cwBar2.h = 24;
    gws_change_window_position( 
        fd,
        bar2_window,
        cwBar2.l,
        cwBar2.t );
    gws_resize_window(
        fd,
        bar2_window,
        cwBar2.w,
        cwBar2.h );

// ---------------------------------------------
// cwButton2
    cwButton2.l = (( lWi.cr_width/8 )*5);
    cwButton2.t = (4 +(24) +4);
    cwButton2.w = (( lWi.cr_width/8 )*2);
    cwButton2.h = 24;
    gws_change_window_position( 
        fd,
        button2_window,
        cwButton2.l,
        cwButton2.t );
    gws_resize_window(
        fd,
        button2_window,
        cwButton2.w,
        cwButton2.h );

// ----------

    //Original
    gws_set_focus(fd,bar2_window);

    //#test
    //gws_set_focus(fd,button2_window);

    gws_redraw_window(fd, bar1_window, TRUE);
    gws_redraw_window(fd, button1_window, TRUE);
    gws_redraw_window(fd, bar2_window, TRUE);
    gws_redraw_window(fd, button2_window, TRUE);
}

static int __init_globals(void)
{
    screen_width  = (unsigned long) gws_get_system_metrics(1);
    screen_height = (unsigned long) gws_get_system_metrics(2);
    return 0;
}

/*
int __init_windows(void)
{
    register int i=0;
    for (i=0; i<WINDOW_COUNT_MAX; i++){
        windowList[i] = 0;
    };
    return 0;
}
*/

// Quem deveria fazer isso seria o window server
// escrevendo na janela com foco de entrada 
// e com as características de edição configuradas pra ela.
// Ou ainda uma biblioteca client-side.

void editorDrawChar(int fd, int ch)
{
    int pos_x=0;
    int pos_y=0;

    if (fd<0)
        return;

// Get saved value
    pos_x = (int) (cursor_x & 0xFFFF);
    pos_y = (int) (cursor_y & 0xFFFF);
    if (pos_x < 0){
        pos_x = 0;
    }
    if (pos_y < 0){
        pos_y = 0;
    }
// End of line
    if (pos_x >= cursor_x_max){
        pos_x = 0;
        pos_y++;
    }
// Last line
// #todo: scroll
    if (pos_y >= cursor_y_max){
        pos_y = (cursor_y_max - 1);
    }
// Save cursor
    cursor_x = pos_x;
    cursor_y = pos_y;
// Draw
// Calling the window server for drawing the char.
    gws_draw_char ( 
        fd, 
        bar2_window, 
        (cursor_x*8), 
        (cursor_y*8), 
        COLOR_BLACK, 
        ch );
// Increment
    cursor_x++;
}

void editorSetCursor( int x, int y )
{
    if (cursor_x >= 0 && 
        cursor_x < cursor_x_max)
    {
        cursor_x = x;
    }

    if (cursor_y >= 0 && 
        cursor_y < cursor_y_max)
    {
        cursor_y = y;
    }
}

static void destroy_windows(int fd)
{
    if (fd<0)
        return;
    gws_destroy_window(fd,button1_window);
    gws_destroy_window(fd,button2_window);
// #todo: 
// The window server need to destroy all the child windows
// when we close the application window.
    gws_destroy_window(fd,main_window);
}


// do_done:
// >> Launch the taskbar application.
// >> Close the gdm application.
static void do_done(int fd)
{
    if (fd<0){
        printf("do_done: fd\n");
    }

// #test
// #todo
// Get the return. it can NOT fail.
    //rtl_clone_and_execute(tb_image_name);

    destroy_windows(fd);
    exit(0);
}

static int 
gdmProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
// Dispatch service.

// Parameters
    if (fd<0){
        return -1;
    }
    if (event_window<0){
        return -1;
    }
    if (event_type<0){
        return -1;
    }

// Events
    switch (event_type){

    case 0:
        //#debug
        //printf("gdm: Null event!\n");
        return 0;
        break;

    //36
    case MSG_MOUSERELEASED:
        if ( event_window == bar1_window ||
             event_window == bar2_window )
        {
            //gws_redraw_window(fd, event_window, TRUE);
            // IN: fd, wid, left, top, color, char.
            if (event_window == bar2_window){
                gws_draw_char (
                    (int) fd,
                    (int) event_window,
                    (unsigned long) long1,
                    (unsigned long) long2,
                    (unsigned long) COLOR_BLACK,
                    (unsigned long) '.' );
            }
            return 0;
        }
        if (event_window == button1_window){
            printf("~ button1 released\n");
        }
        if (event_window == button2_window){
            printf("~ button2 released\n");
        }
        return 0;
        break;


    // #todo
    // We can create the message button clicked.
    // This msg will be send to the main window
    // and the child window was passed via parameter.
    case GWS_MouseClicked:
        printf("gdm: GWS_MouseClicked\n");
        return 0;
        break;

    // Redraw all the child windows.
    case MSG_PAINT:
        if (event_window == main_window)
        {
            update_clients(fd);
            return 0;
        }
        break;

    //case MSG_KEYDOWN:
    //        printf("MSG_KEYDOWN\n");
    //    break;

    // Sent by the window server.
    case MSG_SYSKEYDOWN:
        if (long1 == VK_F1)
        {
            printf ("gdm.bin: MSG_SYSKEYDOWN VK_F1\n");
            do_done(fd);
            return 0;
        }
        if (long1 == VK_F2)
        {
            printf ("gdm.bin: MSG_SYSKEYDOWN VK_F2\n");
            do_done(fd);
            return 0;
        }
        if (long1 == VK_F3)
        {
            printf ("gdm.bin: MSG_SYSKEYDOWN VK_F3\n");
            do_done(fd);
            return 0;
        }
        if (long1 == VK_F4)
        {
            printf ("gdm.bin: MSG_SYSKEYDOWN VK_F4\n");
            do_done(fd);
            return 0;
        }
        break;

    case MSG_CLOSE:
        printf ("gdm.bin: MSG_CLOSE\n");
        do_done(fd);
        //destroy_windows(fd);
        //exit(0);
        break;

    //...
    
    default:
        return -1;
        break;
    };

    return -1;
}

static void pump(int fd)
{
    struct gws_event_d *e;

    struct gws_event_d lEvent;
    lEvent.used = FALSE;
    lEvent.magic = 0;
    //lEvent.type = 0;
    //lEvent.long1 = 0;
    //lEvent.long2 = 0;

    int target_wid = main_window;
    int status = -1;

// Parameter
    if (fd<0){
        printf("pump: fd\n");
        return;
    }

// Target window
// The main window?
    if (target_wid<0){
        printf("pump: target_wid\n");
        return;
    }

// The server will return an event 
// from the its client's event queue.
// Call the local window procedure 
// if a valid event was found.

    // Get events from the display server.
    e = 
        (struct gws_event_d *) gws_get_next_event(
                                   fd, 
                                   target_wid,
                                   (struct gws_event_d *) &lEvent );

    if ((void *) e == NULL)
        return;
    if (e->used != TRUE)
        return;
    if (e->magic != 1234)
        return;

// Dispatch
    status = 
        (int) gdmProcedure(
            fd, 
            e->window,   // The window affected by the event. 
            e->type,     // Message code.
            e->long1, 
            e->long2 );

    //if (status < 0){
    //}
}

static void __event_loop(int fd)
{
    if (fd<0){
        printf("__event_loop: fd\n");
        return;
    }

    while (1){
        pump(fd);
    };
}

int main( int argc, char *argv[] )
{
    int client_fd = -1;
    register int i=0;
// Flags
    int fTest=FALSE;
    int fAll=FALSE;

// #test
// OK!
    for (i=1; i<argc; i++)
    {
        if ( strncmp("-t", argv[i], 2) == 0 ){
            printf("flag: TEST\n");
            fTest=TRUE;
        }
        if ( strncmp("-a", argv[i], 2) == 0 ){
            printf("flag: All\n");
            fAll=TRUE;
        }
        //...
    };



// Screen
    screen_width=0;
    screen_height=0;
// gdm
    gdm_x=0;
    gdm_y=0;
    gdm_width=0;
    gdm_height=0;

// Globals.
// Reinitialize.
    main_window = 0;
// bar1 | button1
    bar1_window = 0;
    button1_window = 0;
// bar2 | button2
    bar2_window = 0;
    button2_window = 0;


// Cursor
    cursor_x = 0;
    cursor_y = 0;
    cursor_x_max = 0;
    cursor_y_max = 0;

    blink_status=FALSE;


// ============================
// Open display.
// IN: 
// hostname:number.screen_number

    Display = (struct gws_display_d *) gws_open_display(display_name);
    if ((void*) Display == NULL)
    {
        debug_print("gdm: Couldn't open display\n");
        printf     ("gdm: Couldn't open display\n");
        exit(1);
    }

// Socket fd.
    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        debug_print("gdm: bad fd\n");
        printf     ("gdm: bad fd\n");
        exit(1);
    }

    __init_globals();

// Device info
    if ( screen_width == 0 || screen_height == 0 ){
        printf("gdm: screen_width screen_height\n");
        exit(1);
    }

// ---------------------------------
// Main window

// Tamanho da janela.
// #
// Isso depende do modo em que o window manager esta operando.
// No modo 1, janelas do tipo overlapped
// terão as dimensões da área de trabalho.
// #todo: precisamos de um request que selecione
// o modo de operação do window manager.


// Posicionamento e demensoes da main window.
    gdm_width  = (unsigned long) (screen_width  >> 1);
    gdm_height = (unsigned long) (screen_height >> 1);
    gdm_x = 
        (unsigned long) ( ( screen_width  - gdm_width ) >> 1 );
    gdm_y = 
        (unsigned long) ( ( screen_height - gdm_height) >> 1 ); 

    // #hack
    if (screen_width == 320)
    {
        gdm_x = 0;
        gdm_y = 0;
        gdm_width  = (unsigned long) screen_width;
        gdm_height = (unsigned long) screen_height;
    }

// Cursor limits based on the window size.
    cursor_x = 0;
    cursor_y = 0;
    cursor_x_max = (int) (((gdm_width/8)  -1) & 0xFFFFFFFF);
    cursor_y_max = (int) (((gdm_height/8) -1) & 0xFFFFFFFF);

// Creating the main window.
// style: 
// 0x0001=maximized 
// 0x0002=minimized 
// 0x0004=fullscreen
// 0x0008 = use statusbar
// ...

    main_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_OVERLAPPED, 
                  WINDOW_STATUS_ACTIVE,  // status 
                  VIEW_NULL,             // view
                  program_name, 
                  gdm_x, gdm_y, gdm_width, gdm_height,
                  0, 
                  0x0000,  
                  COLOR_RED,   // #todo: client bg. Not implemented. 
                  COLOR_GRAY );

    if (main_window <= 0)
    {
        debug_print("gdm: main_window\n");
        printf     ("gdm: main_window\n");
        exit(1);
    }
    //#debug
    //gws_refresh_window(client_fd, main_window);

// -------------------------------
//  The first line of elements.

// 'username:' text
// Text inside the main window.
    if (main_window > 0)
    {
        gws_draw_text (
            (int) client_fd,      // fd,
            (int) main_window,    // window id,
            (unsigned long) 2, 
            (unsigned long) 4 +(24/3), 
            (unsigned int) COLOR_BLACK,
           t1_string );
    }

    //#debug
    //gws_refresh_window(client_fd, main_window);

// ---------------------------------
// bar1_window: (username)
// Inside the main window.

    unsigned long bar1_l = (( gdm_width/8 )*2) +20;
    unsigned long bar1_t = 4;
    unsigned long bar1_w = (( gdm_width/8 )*2);
    unsigned long bar1_h = 24;

    bar1_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_EDITBOX, 1, 1, bar1_string,
                  bar1_l, bar1_t, bar1_w, bar1_h,
                  main_window, 0, COLOR_WHITE, COLOR_WHITE );

    if (bar1_window <= 0)
    {
        debug_print("gdm: bar1_window fail\n");
        printf     ("gdm: bar1_window\n");
        exit(1);
    }
    //#debug
    //gws_refresh_window(client_fd, main_window);

// Text inside the address bar.

    if (bar1_window>0)
    {
        gws_draw_text (
            (int) client_fd,            // fd
            (int) bar1_window,          // window id
            (unsigned long) 8,          // left
            (unsigned long) 8,          // top
            (unsigned int) COLOR_BLACK,
            bar1_text_string);
     }
    //#debug
    //gws_refresh_window(client_fd, main_window);

// ---------------------------------
// button1_window: (reboot button window)
// The [Reboot] button.
// inside the main window.
    unsigned long bu1_l = (( gdm_width/8 )*5);
    unsigned long bu1_t = 4;
    unsigned long bu1_w = (( gdm_width/8 )*2);
    unsigned long bu1_h = 24;

    button1_window = 
        (int) gws_create_window ( 
                  client_fd,
                  WT_BUTTON, 
                  BS_DEFAULT, 
                  1, 
                  b1_string,
                  bu1_l, bu1_t, bu1_w, bu1_h,
                  main_window, 0, COLOR_GRAY, COLOR_GRAY );

    if (button1_window <= 0)
    {
        debug_print("gdm: button1_window fail\n");
        printf     ("gdm: button1_window\n");
        exit(1);
    }
    //#debug
    //gws_refresh_window(client_fd, main_window);

// -------------------------------
//  The second line of elements.

// 'username:' text
// Text inside the main window.
    if (main_window > 0){
     gws_draw_text (
        (int) client_fd,      // fd
        (int) main_window,    // window id
        (unsigned long)  2,
        (unsigned long)  4 +(24) +4 +(24/3), 
        (unsigned int) COLOR_BLACK,
        t2_string);
    }
    //#debug
    //gws_refresh_window(client_fd, main_window);

// ---------------------------------
// bar2_window: password
    unsigned long bar2_l = (( gdm_width/8 )*2) + 20;
    unsigned long bar2_t = (4 +(24) +4);
    unsigned long bar2_w = (( gdm_width/8 )*2);
    unsigned long bar2_h = 24;

    bar2_window = 
        (int) gws_create_window ( 
                  client_fd,
                  WT_EDITBOX, 1, 1, bar2_string,
                  bar2_l, bar2_t, bar2_w, bar2_h,
                  main_window, 0, COLOR_WHITE, COLOR_WHITE );

    if (bar2_window <= 0)
    {
        debug_print("gdm: bar2_window fail\n");
        printf     ("gdm: bar2_window\n");
        exit(1);
    }
    //#debug
    //gws_refresh_window(client_fd, main_window);

// ---------------------------------
// button2_window: (confirm button)
// The [Enter] button.
// inside the main window.

    unsigned long bu2_l = (( gdm_width/8 )*5);
    unsigned long bu2_t = (4 +(24) +4);
    unsigned long bu2_w = (( gdm_width/8 )*2);
    unsigned long bu2_h = 24;

    button2_window = 
        (int) gws_create_window ( 
                  client_fd,
                  WT_BUTTON, 
                  BS_DEFAULT, 
                  1, 
                  b2_string,
                  bu2_l, bu2_t, bu2_w, bu2_h,
                  main_window, 0, COLOR_GRAY, COLOR_GRAY );

    if (button2_window <= 0)
    {
        debug_print("gdm: button2_window fail\n");
        printf     ("gdm: button2_window\n");
        exit(1);
    }
    //#debug
    //gws_refresh_window(client_fd, main_window);

// ---------------------
// Tip
    if (main_window > 0){
    gws_draw_text (
        (int) client_fd,      // fd
        (int) main_window,    // window id
        (unsigned long)  2,
        (unsigned long)  4 +(24) +4 +(24/3) +(24), 
        (unsigned int) COLOR_BLACK,
        t3_string );
    }

/*
    int t=0;
    for (t=1; t<8; t++){
    gws_draw_text (
        (int) client_fd,        // fd,
        (int) bar2_window,    // window id,
        (unsigned long) 4,      // left,
        (unsigned long) t*8,    // top,
        (unsigned long) COLOR_BLACK,
        "Hello, this is a text editor!");
    } 
*/

/*
    int t=0;
    for (t=0; t<80; t++){
        gws_draw_char (
            (int) client_fd,        // fd,
            (int) bar2_window,    // window id,
            (unsigned long) t*8,    // left,
            (unsigned long) 8,      // top,
            (unsigned long) COLOR_BLUE,
            (unsigned long) t );
    } 
*/

// Show main window.
    //gws_refresh_window(client_fd, main_window);

// test: ok
    //gws_change_window_position(client_fd,main_window,0,0);
    //gws_resize_window ( client_fd, main_window, 400, 400);
    //gws_redraw_window(client_fd,main_window,1);

// mudando o tamanho de uma janela que esta no top
// vai deixar ela suja e o compositor vai repintar.

// ============================================
// focus
// editbox
// Setting the input focus on a given window.
// Input
// #focus
// Well, the gdm.bin application is not receiving
// the input ... so, i guess the window server
// is printing the chars into the window with focus.

    //gws_async_command(
    //     client_fd,
    //     9,             // set focus
    //     bar2_window,
    //     bar2_window );


// set active window.
    //gws_async_command(
    //     client_fd,
    //     15, 
    //     main_window,
    //     main_window );

    gws_set_active( client_fd, main_window );
    gws_set_focus( client_fd, bar2_window );

// Show main window.
    gws_refresh_window(client_fd, main_window);

// Event loop
    __event_loop(client_fd);

// Close socket.
    // close(client_fd);
    return 0;
}

//
// End
//

