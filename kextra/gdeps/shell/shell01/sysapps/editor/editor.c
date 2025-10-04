// editor.c
// Text editor for Gramado OS.
// This is a client-side GUI application connected 
// with the display server.
// Created by Fred Nora.

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
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>
// The client-side library.
#include <gws.h>
// Internal
#include <packet.h>

#include "globals.h"
#include <editor.h>


/*
#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port   = 7548, 
    .sin_addr   = IP(192, 168, 1, 79),
};
*/

static int isTimeToQuit = FALSE;
static int file_status=FALSE;

// #test
// The file buffer.
//char file_buffer[512];
char file_buffer[1024];

//
// Windows
//

// private
static int main_window = -1;
static int addressbar_window = -1;
static int savebutton_window = -1;
static int client_window = -1; // #bugbug: Sometimes we can't delete this window.
// ...

struct child_window_d
{
    unsigned long l;
    unsigned long t;
    unsigned long w;
    unsigned long h;
};
struct child_window_d cwAddressBar;
struct child_window_d cwButton;
struct child_window_d cwText;

// #todo
// int button_list[8];

// cursor
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

// Program name
static const char *program_name = "EDITOR";
// Addressbar title
static const char *bar1_string = "bar1";
// Button label.
static const char *b1_string = "Save";
// Client window title.
static const char *cw_string = "cw";

// text
static unsigned long text1_l = 0;
static unsigned long text1_t = 0;
static unsigned int text1_color = 0;
static const char *text1_string = "Name:";
static const char *text2_string = "TEXT.TXT";

// =====================================
// Prototypes

static void editorShutdown(int fd);

static void update_clients(int fd);

static int editor_init_windows(void);
static int editor_init_globals(void);

static void __test_text(int fd, int wid);
static void __test_load_file(int socket, int wid);

static int 
editorProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 );

void pump(int fd, int wid);

// =====================================
// Functions

static void editorShutdown(int fd)
{
    if (fd<0)
        return;

    gws_destroy_window(fd,client_window);   // #bugbug: sometimes we can't delete this window.
    gws_destroy_window(fd,savebutton_window);
    gws_destroy_window(fd,addressbar_window);
    gws_destroy_window(fd,main_window);
}

static void update_clients(int fd)
{
    struct gws_window_info_d lWi;

// Parameter
    if (fd<0){
        return;
    }

// Get info about the main window.
// IN: fd, wid, window info structure.
    gws_get_window_info(
        fd, 
        main_window,   // The app window.
        (struct gws_window_info_d *) &lWi );

// ------------------------
// Text
// Let's print the text, before the address bar.

    text1_l = 2;
    text1_t = 4 + (24/3);
    text1_color = COLOR_BLACK;
    gws_draw_text (
        (int) fd,
        (int) main_window,
        (unsigned long) text1_l,
        (unsigned long) text1_t,
        (unsigned long) text1_color,
        text1_string );

// ---------------------------------------------
// Address bar
// #todo: 
// '.l': It actually depends on the text befor this.
// We need to know the text width.
    cwAddressBar.l = (( lWi.cr_width/8 )*2);
    cwAddressBar.t = 4;
    cwAddressBar.w = (( lWi.cr_width/8 )*3);
    cwAddressBar.h = 24; 
    gws_change_window_position( 
        fd,
        addressbar_window,
        cwAddressBar.l,
        cwAddressBar.t );
    gws_resize_window(
        fd,
        addressbar_window,
        cwAddressBar.w,
        cwAddressBar.h );

    gws_redraw_window(fd, addressbar_window, TRUE);

//---------------------------------------------
// Save button
    cwButton.l = (( lWi.cr_width/8 )*7) -4;
    cwButton.t = 4;
    cwButton.w = (( lWi.cr_width/8 )*1);
    cwButton.h = 24;
    gws_change_window_position( 
        fd,
        savebutton_window,
        cwButton.l,
        cwButton.t );
    gws_resize_window(
        fd,
        savebutton_window,
        cwButton.w,
        cwButton.h );

    gws_redraw_window(fd, savebutton_window, TRUE);

//-----------------------
// The client window where we type the text.

    cwText.l = 0;
    cwText.t = (cwAddressBar.t + cwAddressBar.h + 2);
    cwText.w = lWi.cr_width;
    cwText.h = (lWi.cr_height - cwText.t);

    gws_change_window_position( 
        fd,
        client_window,
        cwText.l,
        cwText.t );
    gws_resize_window(
        fd,
        client_window,
        cwText.w,
        cwText.h );
    //gws_set_focus(fd,client_window);
    gws_redraw_window(fd, client_window, TRUE);
}

static int editor_init_globals(void)
{
    gScreenWidth  = (unsigned long) gws_get_system_metrics(1);
    gScreenHeight = (unsigned long) gws_get_system_metrics(2);
    //...
    return 0;
}

static int editor_init_windows(void)
{
    register int i=0;
    for (i=0; i<WINDOW_COUNT_MAX; i++){
        windowList[i] = 0;
    };
    return 0;
}

// Quem deveria fazer isso seria o window server
// escrevendo na janela com foco de entrada 
// e com as características de edição configuradas pra ela.
// Ou ainda uma biblioteca client-side.

void 
editorDrawChar( 
    int fd,
    int ch)
{
// #:
// The server is printing the char if the
// window with focus is an editbox.

    int pos_x=0;
    int pos_y=0;
    unsigned int Color = COLOR_BLACK;

// Parameter
    if (fd<0)
        return;

// Get saved value
    pos_x = (int) (cursor_x & 0xFFFF);
    pos_y = (int) (cursor_y & 0xFFFF);

    if ( pos_x < 0 ){ pos_x = 0; }
    if ( pos_y < 0 ){ pos_y = 0; }

// End of line
    if ( pos_x >= cursor_x_max )
    {
        pos_x = 0;
        pos_y++;
    }

// Last line
// #todo: scroll
    if ( pos_y >= cursor_y_max )
    {
        pos_y = cursor_y_max-1;
    }

// Save cursor
    cursor_x = pos_x;
    cursor_y = pos_y;

// Draw
// Calling the window server for drawing the char.

    gws_draw_char ( 
        fd, 
        client_window, 
        (cursor_x*8), 
        (cursor_y*8), 
        Color, 
        ch );

    // increment
    cursor_x++;
}

void
editorSetCursor( 
    int x,
    int y )
{
// #:
// The server is printing the char if the
// window with focus is an editbox.
// So, we need to tell the ws to change the cursor position.

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

static int 
editorProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
// dispatch a service

// Parameters
    if (fd<0){
        return -1;
    }
    if (event_window<0){
        return -1;
    }
    if (event_type < 0){
        return -1;
    }

// Events
    switch(event_type){

    case 0:
        //#debug
        //printf("EDITOR: Null event!\n");
        return 0;
        break;

    case MSG_KEYDOWN:
        //printf("editor: MSG_KEYDOWN\n");
        break;

    //36
    case MSG_MOUSERELEASED:
        if ( event_window == addressbar_window ||
             event_window == client_window )
        {
            //printf("editor.bin: MSG_MOUSERELEASED\n");
            //gws_redraw_window(fd, event_window, TRUE);
            return 0;
        }

        //if (event_window == savebutton_window)
            //printf("editor: Button released\n");

        return 0;
        break;

    // Mouse clicked on a button.
    case GWS_MouseClicked:
        printf("editor: GWS_MouseClicked\n");
        return 0;
        break;

    // Repainting all the client windows.
    // If the event window is the main window, so
    // redraw everyone.
    case MSG_PAINT:
        if (event_window == main_window)
        {
            __test_text(fd,addressbar_window);
            __test_text(fd,client_window);
            update_clients(fd);
            return 0;
        }
        break;

    case MSG_CLOSE:
        printf ("editor.bin: MSG_CLOSE\n");
        editorShutdown(fd);
        //isTimeToQuit = TRUE;
        // #test
        //if ((void*) Display != NULL){
            //gws_close_display(Display);
        //}
        exit(0);
        break;
    
    // After a resize event.
    //case MSG_SIZE:
        //break;

    //case MSG_CREATE: 
        // Initialize the window. 
        //return 0; 
 
    //case MSG_PAINT: 
        // Paint the window's client area. 
        //return 0; 

    // test
    //case 8888:
        //break;

    case 9191:
        printf("9191\n");
        break;

    //...
    
    default:
        goto fail;
        break;
    };

fail:
    return (int) -1;
}


// #test
// Set text and Get text into an editbox window.
static void __test_text(int fd, int wid)
{
    char string_buffer[256];
    char *p;
    int Status=0;

    if (fd<0)
        return;
    if (wid<0)
        return;

// Setup the local buffer.
    memset(string_buffer,0,256);
    sprintf(string_buffer,"dirty");

//
// Inject
//

    const char *short_text = "Short text";
    //const char *long_text = "This is a long text, a really long text";
    const char *long_text = 
        "This is a long text, a really, really, really, really, really long text";
    char *target_text;

    if (wid == addressbar_window)
        target_text = short_text;
    if (wid == client_window)
        target_text = long_text;

    // #bugbug: What is the size?
    // The limit is 256 chars.
    gws_set_text (
        (int) fd,      // fd,
        (int) wid,     // window id,
        (unsigned long)  1, 
        (unsigned long)  1, 
        (unsigned long) COLOR_BLACK,
        target_text );

/*
// ------------------
// Get back
    Status = 
    gws_get_text (
        (int) fd,     // fd,
        (int) wid,    // window id,
        (unsigned long)  1, 
        (unsigned long)  1, 
        (unsigned long) COLOR_BLACK,
        (char *) string_buffer );
*/

    //if ( (void*) p == NULL ){
    //    printf("editor.bin: Invalid text buffer\n");
    //    return;
    //}

    //#debug
    //printf("__test_text: {%s}\n",string_buffer);
    //while(1){}

/*
// ------------------
// Print into the window.
    p = string_buffer;
    gws_draw_text (
        (int) fd,      // fd
        (int) wid,     // window id
        (unsigned long)  8, 
        (unsigned long)  8, 
        (unsigned long) COLOR_RED,
        p );
*/
}

// #test
// Working on routine to load a file
// into the client area of the application window.
static void __test_load_file(int socket, int wid)
{
// #
// This is a work in progress!

    int fd = -1;
    char *name = "init.ini";

    file_status = FALSE;

    fd = open( (char*) name, 0, "a+" );
    //lseek(fd,0,SEEK_SET);
    int nreads=0;
    nreads = read(fd,file_buffer,511);
    if (nreads>0)
        file_status = TRUE;

// socket
    if(socket<0)
        return;

    if(wid<0)
        return;

    unsigned long x=0;
    unsigned long y=0;
    register int i=0;
    // Draw and refresh chars.
    for (i=0; i<nreads; i++)
    {
        if ( isalnum( file_buffer[i] ) )
        {
            gws_draw_char (
                (int) socket,      // socket fd
                (int) wid,         // wid
                (unsigned long) x, // left
                (unsigned long) y, // top
                (unsigned long) COLOR_BLACK,
                (unsigned long) file_buffer[i] );  // char.
        }

        x += 8;  // Next column.
        if (x > (8*40))
        {
           x=0;
           y += 8;  // Next row.
        }

        if (file_buffer[i] == '\n')
        {
            x=0;
            y += 8;
        }
    };
}

void pump(int fd, int wid)
{
    struct gws_event_d lEvent;
    lEvent.used = FALSE;
    lEvent.magic = 0;
    lEvent.type = 0;
    //lEvent.long1 = 0;
    //lEvent.long2 = 0;

    struct gws_event_d *e;

    int target_wid = wid;

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

// Pump
    e = 
        (struct gws_event_d *) gws_get_next_event(
                                   fd, 
                                   target_wid,
                                   (struct gws_event_d *) &lEvent );

    if ((void *) e == NULL)
        return;
    if (e->magic != 1234){
        return;
    }
    if (e->type < 0)
        return;
// Dispatch
    editorProcedure( fd, e->window, e->type, e->long1, e->long2 );
}

// Called by main() in main.c.
int editor_initialize(int argc, char *argv[])
{
    const char *display_name_string = "display:name.0";
    int client_fd = -1;
    unsigned int client_area_color = COLOR_RED;  // Not implemented 
    unsigned int frame_color = COLOR_GRAY;

// Main window.
// w_width, w_height, w_left, w_top
    unsigned long w_width = 0;
    unsigned long w_height = 0;
    unsigned long w_left = 0;
    unsigned long w_top = 0;

    isTimeToQuit = FALSE;
// Screen
    gScreenWidth=0;
    gScreenHeight=0;
// Initialize WIDs.
    main_window = -1;
    addressbar_window = -1;
    savebutton_window = -1;
    client_window = -1;
// Cursor
    cursor_x = 0;
    cursor_y = 0;
    cursor_x_max = 0;
    cursor_y_max = 0;
// Blink.
    blink_status=FALSE;

    //if (argc < 0)
        //return 1;

/*
// #test
// OK!
    int i=0;
    for (i = 1; i < argc; i++)
        if (strcmp("--test--", argv[i]) == 0)
            printf("TEST\n");
*/

// ============================
// Open display.
// IN: 
// hostname:number.screen_number

    Display = (struct gws_display_d *) gws_open_display(display_name_string);
    if ((void*) Display == NULL){
        printf("editor.bin: Display\n");
        goto fail;
    }

    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        printf("editor.bin: fd\n");
        goto fail;
    }

// =====================================================

    // editor_init_globals();

// Device info
    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);
    if ( w == 0 || h == 0 )
    {
        printf("editor.bin: w h \n");
        goto fail;
    }

// -----------------------
// Width
    w_width = (w>>1);
    // #hack for 800
    if (w == 800){ w_width = 640; }
    // #hack for 640
    if (w == 640){ w_width = 480; }
    // #hack for 640
    if (w == 320){ w_width = w;   }

// -----------------------
// Height
    w_height = (h - 100);  //(h>>1);
    if (h == 200){ w_height = h; }

// -----------------------
// Left
    //w_left = ( ( w - w_width ) >> 1 );
    w_left = 10;
    if (w == 320)
        w_left = 0;

// -----------------------
// Top
    //w_top = ( ( h - w_height) >> 1 ); 
    w_top = 10;
    if (w == 320)
        w_top = 0;

// -----------------------
// Position and dimensions for 320x200. (Again)

    if (w == 320)
    {
        w_left = 0; 
        w_top = 0; 
        w_width = w; 
        w_height = h -32 -32;  //#todo: Get this offset.
    }

// Cursor limits based on the window size.
    cursor_x = 0;
    cursor_y = 0;
    cursor_x_max = ((w_width/8)  -1);
    cursor_y_max = ((w_height/8) -1);

// Create main window.
    main_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_OVERLAPPED,  // type 
                  WINDOW_STATUS_ACTIVE,  // status 
                  VIEW_NULL,             // view
                  program_name, 
                  w_left, w_top, w_width, w_height,
                  0, 
                  0x0000, 
                  client_area_color,
                  frame_color );

    if (main_window < 0){
        printf("editor.bin: main_window failed\n");
        goto fail;
    }

    //#debug
    gws_refresh_window(client_fd, main_window);


// Label.
// Text inside the main window.
// Right below the title bar.
// Right above the client window.

    text1_l = 2;
    text1_t = 4 + (24/3);
    text1_color = COLOR_BLACK;

    gws_draw_text (
        (int) client_fd,
        (int) main_window,
        (unsigned long) text1_l,
        (unsigned long) text1_t,
        (unsigned long) text1_color,
        text1_string );

    //#debug
    gws_refresh_window(client_fd, main_window);
// -----------------------------

    // Local.
    struct gws_window_info_d lWi;

// Get info about the main window.
//IN: fd, wid, window info structure.
    gws_get_window_info(
        client_fd, 
        main_window,   // The app window.
        (struct gws_window_info_d *) &lWi );


// Address bar - (edit box)
// Inside the main window.
// se a janela mae é overlapped, 
// então estamos relativos à sua área de cliente.

// Create address bar window.
    addressbar_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_EDITBOX, 1, 1, bar1_string,
                  (( lWi.cr_width/8 )*2),  //l
                  4,                       //t
                  (( lWi.cr_width/8 )*3), 
                  24,    
                  main_window, 
                  0, 
                  COLOR_WHITE, COLOR_WHITE );

    if (addressbar_window < 0){
        printf("editor.bin: addressbar_window failed\n");
        goto fail;
    }

    //#debug
    gws_refresh_window(client_fd, addressbar_window);


// Text inside the address bar.
    if (addressbar_window > 0)
    {
        gws_draw_text (
            (int) client_fd,            // fd
            (int) addressbar_window,    // window id
            (unsigned long) 8,          // left
            (unsigned long) 8,          // top
            (unsigned long) COLOR_BLACK,
            text2_string );
    }
    //#debug
    gws_refresh_window (client_fd, addressbar_window);

// Save
    cwAddressBar.l = (( lWi.cr_width/8 )*2);
    cwAddressBar.t = 4;
    cwAddressBar.w = (( lWi.cr_width/8 )*3);
    cwAddressBar.h = 24;

// The [Save] button.
// inside the main window.

    // #test
    // The 'button state' is the same of window status.

// Create save button window.
    savebutton_window = 
        (int) gws_create_window ( 
                  client_fd,
                  WT_BUTTON,
                  BS_DEFAULT,  // window status or button state
                  1,
                  b1_string,
                  (( lWi.cr_width/8 )*6),  //l 
                  4,                       //t
                  (( lWi.cr_width/8 )*1), 
                  24,
                  main_window, 0, COLOR_GRAY, COLOR_GRAY );

    if (savebutton_window < 0){
        printf("editor.bin: savebutton_window failed\n");
        goto fail;
    }
    //#debug
    gws_refresh_window (client_fd, savebutton_window);

// Save button
    cwButton.l = (( lWi.cr_width/8 )*7) -4;
    cwButton.t = 4;
    cwButton.w = (( lWi.cr_width/8 )*1);
    cwButton.h = 24;


//
// == Client window =======================
//

// #todo
// Here is the moment where we're gonna 
// draw the content of the input file into the 
// client window.
// >> We're gonna save the file content into a local buffer,
// large enough for the whole file.
// >> We're gonna send parts of this file to the display server,
// and the server will save it into the text buffer 
// in the window structure.


/*

 // #todo: Get the client window's info.
 // see: the same in terminal.bin appication.

    struct gws_window_info_d *wi;
    wi = (void*) malloc( sizeof( struct gws_window_info_d ) );
    if( (void*) wi == NULL )
    {
        printf("terminal: wi\n");
        while(1){}
    }
    //IN: fd, wid, window info structure.
    gws_get_window_info(
        client_fd, 
        main_window,   // The app window.
        (struct gws_window_info_d *) wi );
*/

// (Editbox)
// Client window (White)
// Inside the mainwindow.
// Lembre-se que temos uma status bar.

// left:
    unsigned long cw_left = 0;
// top: pad | address bar | pad
    unsigned long cw_top =  (cwAddressBar.t + cwAddressBar.h + 2);
// width: Width - borders.
    unsigned long cw_width = (lWi.cr_width);
// height:
// #bugbug:
// We gotta get the client window values.
    unsigned long cw_height = (lWi.cr_height - cw_top);

// Create client window.
    client_window = 
        (int) gws_create_window ( 
                client_fd,
                WT_EDITBOX_MULTIPLE_LINES, 1, 1, cw_string,
                cw_left, cw_top, cw_width, cw_height,
                main_window, 0, COLOR_WHITE, COLOR_WHITE );

    if (client_window < 0){
        printf("editor.bin: client_window failed\n");
        goto fail;
    }
    //#debug
    //gws_refresh_window (client_fd, client_window);

// Save
    cwText.l = 0;
    cwText.t = (cwAddressBar.t + cwAddressBar.h + 2);
    cwText.w = lWi.cr_width;
    cwText.h = (lWi.cr_height - cwText.t);

    gws_set_active( client_fd, main_window );
    gws_set_focus( client_fd, client_window );

// Show main window. (Again)
    gws_refresh_window (client_fd, main_window);

// ============================================

//
// Event loop
//

// ===========================================
// #test
// Get input from stdin

    int C=0;

    rtl_focus_on_this_thread();
    // GRAMADO_SEEK_CLEAR
    lseek( fileno(stdin), 0, 1000);
    // Atualiza as coisas em ring3 e ring0.
    rewind(stdin);

    while (1){
        if (isTimeToQuit == TRUE)
            break;

        // It needs to be the main window for now.
        pump( client_fd, main_window );

        C = fgetc(stdin);
        if (C > 0)
        {
            editorProcedure ( 
                client_fd,    // socket
                client_window,    // window ID
                MSG_KEYDOWN,  // message code
                C,            // long1 (ascii)
                C );          // long2 (ascii)
        }
    };

// ===========================================

// loop
// The server will return an event from the client's event queue.
// Call the local window procedure if a valid event was found.
// #todo: 
// Por enquanto, a rotina no servidor somente lida com 
// eventos na janela com foco de entrada.
// Talvez a ideia é lidar com eventos em todas as janelas
// do processo cliente.

    //Display->running = TRUE;

// Getting the asynchronous events 
// from the window server via socket.
// Processing this events.

/*
    while (1)
    {
        //if ( Display->running != TRUE )
            //break;
        if (isTimeToQuit == TRUE)
            break;

        // It needs to be the main window for now.
        pump( client_fd, main_window );
    };
*/

// ok
    if (isTimeToQuit == TRUE){
        printf("editor.bin: isTimeToQuit\n");
        editorShutdown(client_fd);
        return EXIT_SUCCESS;
    }

// Hang
    printf("editor.bin: main loop failedn");
    while (1){
    };


/*
    int C=0;
    //char data[2];
    //int nread=0;

    //fputc('A',stdin);
    //fputs("This is a string in stdin",stdin);

    rewind(stdin);

    while (1){
        C=fgetc(stdin);
        if(C>0){
            editorProcedure( 
                client_fd,     // socket
                NULL,          // opaque window object
                MSG_KEYDOWN,   // message code
                C,             // long1 (ascii)
                C );           // long2 (ascii)
        }
    };
*/

//==============================================


//
// loop
//

/*
//=================================
// Set foreground thread.
// Get events scanning a queue in the foreground queue.
    rtl_focus_on_this_thread();
    
    while (1){
        if ( rtl_get_event() == TRUE )
        {  
            editorProcedure( 
                client_fd,
                (void*) RTLEventBuffer[0], 
                RTLEventBuffer[1], 
                RTLEventBuffer[2], 
                RTLEventBuffer[3] );
        }
    };

//=================================
*/


// exit
    //close (client_fd);
    debug_print("editor: bye\n"); 
    printf     ("editor: bye\n");

    return EXIT_SUCCESS;
fail:
    return EXIT_FAILURE;
}

//
// End
//

