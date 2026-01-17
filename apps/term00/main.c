// main.c
// This is the implementation of the terminal application.
// Created by Fred Nora.

//#include <ctype.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctls.h>
#include <sys/ioctl.h>
#include <stdlib.h>
//#include <stdio.h>
//#include <unistd.h>

// Shared
#include "shared/globals.h"

// Backend
#include "core/core.h"
#include "core/shell.h"

// Frontend
#include "ui/ui.h"
#include "terminal.h"

// Client-side library.
#include <gws.h>

/*
// #test: These are sent by the system
#define __MSG_DC1  76  // ^q
#define __MSG_DC2  77  // ^r
#define __MSG_DC3  78  // ^s   (#bugbug: Same as MSG_SAVE)
#define __MSG_DC4  79  // ^t
*/

// Program name
static const char *program_name = "TERM00";
struct gws_display_d *Display;


FILE *__terminal_input_fp;

// Windows
struct gws_window_info_d *wi;  // Window info for the main window.

// Private
static int main_window=0;
static int terminal_window=0;

// color
static unsigned int bg_color = COLOR_BLACK;
static unsigned int fg_color = COLOR_WHITE;
static unsigned int prompt_color = COLOR_GREEN;

// Embedded shell
// We are using the embedded shell.
static int isUsingEmbeddedShell=TRUE;


// #todo: #maybe:
// Fazer estrutura para gerenciar a sequencia.
static int __sequence_status=0;

//const char *test_app = "#shell00.bin";
const char *test_app = "cat00.bin";


// ---------------------------------------

// CSI - Control Sequence Introducer.
// see: term0.h
char CSI_BUFFER[CSI_BUFFER_SIZE];
int __csi_buffer_tail=0;
int __csi_buffer_head=0;

// ---------------------------------------
// see: term0.h
//static CSIEscape  csiescseq;
//static STREscape  strescseq;

// ---------------------------------------
unsigned long __tmp_x=0;
unsigned long __tmp_y=0;

// ---------------------------------------
// see: term0.h
struct terminal_line  LINES[32];
// Conterá ponteiros para estruturas de linha.
unsigned long lineList[LINE_COUNT_MAX];
// Conterá ponteiros para estruturas de linha.
unsigned long screenbufferList[8];

// ---------------------------------------
// see: term0.h

// Marcador do cursor.
unsigned long screen_buffer_pos=0;    //(offset) 
unsigned long screen_buffer_x=0;      //current col 
unsigned long screen_buffer_y=0;      //current row
static unsigned long screen_buffer_saved_x=0;
static unsigned long screen_buffer_saved_y=0;

// ---------------------------------------

//
// System Metrics
//

int smScreenWidth=0;                   //1 
int smScreenHeight=0;                  //2
unsigned long smCursorWidth=0;         //3
unsigned long smCursorHeight=0;        //4
unsigned long smMousePointerWidth=0;   //5
unsigned long smMousePointerHeight=0;  //6

unsigned long smCharWidth = __CHAR_WIDTH;           //7
unsigned long smCharHeight = __CHAR_HEIGHT;          //8
// ...

//
// Window limits
//

// Full screen support
unsigned long wlFullScreenLeft=0;
unsigned long wlFullScreenTop=0;
unsigned long wlFullScreenWidth=0;
unsigned long wlFullScreenHeight=0;

// Limite de tamanho da janela.
unsigned long wlMinWindowWidth=0;
unsigned long wlMinWindowHeight=0;
unsigned long wlMaxWindowWidth=0;
unsigned long wlMaxWindowHeight=0;


//
// Linhas
//

// Quantidade de linhas e colunas na área de cliente.
int wlMinColumns=0;
int wlMinRows=0;
int __wlMaxColumns=0;
int __wlMaxRows=0;

//
//  ## Window size ##
//

unsigned long wsWindowWidth=0;
unsigned long wsWindowHeight=0;
//...

//
//  ## Window position ##
//

unsigned long wpWindowLeft=0;
unsigned long wpWindowTop=0;
//..


//#importante:
//Linhas visíveis.
//número da linha
//isso será atualizado na hora do scroll.
int textTopRow=0;  //Top nem sempre será '0'.
int textBottomRow=0;

int textSavedRow=0;
int textSavedCol=0;

int textWheelDelta=0;     //delta para rolagem do texto.
int textMinWheelDelta=0;  //mínimo que se pode rolar o texto
int textMaxWheelDelta=0;  //máximo que se pode rolar o texto
//...

//
// Bg window
// 
 
unsigned long __bgleft=0;
unsigned long __bgtop=0;
unsigned long __bgwidth=0;
unsigned long __bgheight=0;
 
unsigned long __barleft=0;
unsigned long __bartop=0;
unsigned long __barwidth=0;
unsigned long __barheight=0;

// Client window title
static const char *cw_string = "Client";


//
// == Private functions: Prototypes ==============
//

static void __initialize_basics(void);

static void __initializeTerminalComponents(void);
static void terminalInitWindowPosition(void);
static void terminalInitWindowSizes(void);
static void terminalInitWindowLimits(void);
static void terminalInitSystemMetrics(void);

static int 
terminalProcedure ( 
    int fd,
    int window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

//
// Event loop
//

// System messages.
static void __get_system_event(int fd, int wid);
static void __get_ds_event(int fd, int event_wid);

static int __input_STDIN(int fd);
static int __input_from_connector(int fd);

static int embedded_shell_run(int fd);
static int terminal_run(int fd);

static void compareStrings(int fd);
static void doPrompt(int fd);
static void __on_return_key_pressed(int fd);

static void doHelp(int fd);
static void doAbout(int fd);
static void __libc_test(int fd);

static void clear_terminal_client_window(int fd);
static void __test_winfo(int fd, int wid);

static void __winmax(int fd);
static void __winmin(int fd);

//#test
static void update_clients(int fd);


// Global or inside Terminal struct
int ptym_fd = -1;
int ptys_fd = -1;

static void terminal_notify_child_close(void);

//====================================================

// Quick and dirty kill: send ETX and EOT to child
static void terminal_notify_child_close(void)
{
    char etx = 0x03;  // ETX (Ctrl +C)
    char eot = 0x04;  // EOT (Ctrl +D)

    if (ptym_fd < 0)
        return;
    write(ptym_fd, &etx, 1);
    write(ptym_fd, &eot, 1);
}


//static int term00_gettid(void);
static int term00_gettid(void)
{
    unsigned long ul_value=0;
    ul_value = (unsigned long) sc80(87, 0, 0, 0);
// 32bit value.
    return (pid_t) (ul_value & 0xFFFFFFFF);
};

// Open PTYM
static void terminal_initialize_pty(void)
{
    ptym_fd = open("/DEV/PTYM", 0, "a+");
    if (ptym_fd < 0) {
        //printf("term00: Failed to open PTYM\n");
    } else {
        //printf("term00: PTYM opened, fd=%d\n", ptym_fd);
    }
}

// Open PTYS
static void terminal_initialize_ptys_for_the_shell(void)
{
    ptys_fd = open("/DEV/PTYS", 0, "a+");
    if (ptys_fd < 0) {
        printf("shell: failed to open PTYS\n");
        exit(1);
    }
    // #todo: redirect stdin, stdout, stderr
    //dup2(ptys_fd, 0);
    //dup2(ptys_fd, 1);
    //dup2(ptys_fd, 2);

    //dup2(ptys_fd, STDIN_FILENO);
    //dup2(ptys_fd, STDOUT_FILENO);
    //dup2(ptys_fd, STDERR_FILENO);
    // The shell will use standard stream ... the same the terminal has now

    //if (ptys_fd > STDERR_FILENO)
        //close(ptys_fd);
}


static void update_clients(int fd)
{
    // Terminal window
    int wid = Terminal.client_window_id;
    // Local
    struct gws_window_info_d lWi;

    if (fd<0){
        return;
    }

    // No
    // Activate the main window.
    // gws_set_active(fd,main_window);

// Get info about the main window.
// IN: fd, wid, window info structure.
    gws_get_window_info(
        fd, 
        main_window,   // The app window.
        (struct gws_window_info_d *) &lWi );

    unsigned long l = 0;
    unsigned long t = 0;
    unsigned long w = lWi.cr_width;
    unsigned long h = lWi.cr_height;

    if (wid < 0)
        return;

    gws_change_window_position( 
        fd,
        wid,  // Terminal window
        l,
        t );
    gws_resize_window(
        fd,
        wid,  // Terminal window
        w,
        h );

// #todo: 
// We need a list o clients. maybe clients[i]

    // No
    //gws_set_focus(fd,wid);

    gws_redraw_window(fd, wid, TRUE);

// ------------------------------------------------
// Update some font info based on our new viewport.

// Update information in Terminal structure

// Font info again
// Based on our corrent viewport
// In chars.

// Terminal

    if (Terminal.initialized != TRUE)
        return;
    Terminal.width = w;
    Terminal.height = h;

// Font
// #todo: We need the font information in the window structure.

    if (FontInfo.initialized != TRUE)
        return;

    if ( FontInfo.width > 0 && 
         FontInfo.width < Terminal.width )
    {
        Terminal.width_in_chars = 
            (unsigned long)((Terminal.width/FontInfo.width) & 0xFFFF);
    }

    if ( FontInfo.height > 0 && 
         FontInfo.height < Terminal.height )
    {
        Terminal.height_in_chars = 
            (unsigned long)((Terminal.height/FontInfo.height) & 0xFFFF);
    }
}

// Redraw and refresh the client window.
// Setup the cursor position.
// #todo: Maybe we need to get the window info again.
static void clear_terminal_client_window(int fd)
{
    int wid = Terminal.client_window_id;

    if (fd<0){
        return;
    }
// Redraw and refresh the window.
    //gws_redraw_window( fd, wid, TRUE );   //Slower?
// Clear the window
// Repaint it using the default background color.
    gws_clear_window(fd,wid);  // Faster?
// Update cursor.
    cursor_x = Terminal.left;
    cursor_y = Terminal.top;
}

// Maximize application window.
// #bugbug: Covering the taskbar.
// #todo: Isso pode virar uma função na biblioteca.
// mas podemos deixar o display server fazer isso.
static void __winmax(int fd)
{
// #bugbug
// Esse tipo de torina nao eh atribuiçao do terminal.
// Talvez seja atribuiçao do display server.
// Talvez uma biblioteca client side tambem possa tratar disso.
// Talvez um wm client-side tambem possa tratar isso.

    int wid        = (int) Terminal.main_window_id;
    int client_wid = (int) Terminal.client_window_id;
    unsigned long w=rtl_get_system_metrics(1);
    unsigned long h=rtl_get_system_metrics(2);
                  // #bugbug
                  // The server needs to respect the working area.
                  h = (h -40);

    if(fd<0){
        return;
    }
// Change position, resize and redraw the window.
    gws_change_window_position(fd,wid,0,0);
    gws_resize_window(fd, wid, w, h );
    gws_redraw_window(fd, wid, TRUE );

//---------------

// get the info for the main window.
// change the position of the terminal window.
// its because the client are also changed.
// Get window info:
// IN: fd, wid, window info structure.
    gws_get_window_info(
        fd, 
        wid,
        (struct gws_window_info_d *) wi );
    if (wi->used != TRUE){ return; }
    if (wi->magic!=1234) { return; }

// Show info:
// Frame: l,t,w,h
    //printf("Frame info: l=%d t=%d w=%d h=%d\n",
    //    wi->left, wi->top, wi->width, wi->height );
// Client rectangle: l,t,w,h
    //printf("Client rectangle info: l=%d t=%d w=%d h=%d\n",
    //    wi->cr_left, wi->cr_top, wi->cr_width, wi->cr_height );

// The terminal window. (client area)
// Change position, resize and redraw the window.
    
    //gws_change_window_position(fd,client_wid,wi->cr_left,wi->cr_top);
    // Always in 0,0
    gws_change_window_position(
        fd, client_wid, 
        0,
        0 );

    gws_resize_window(fd, client_wid, wi->cr_width, wi->cr_height );
    gws_redraw_window(fd, client_wid, TRUE );
}

// Minimize application window.
// #bugbug: Covering the taskbar.
// #todo: Isso pode virar uma função na biblioteca.
// mas podemos deixar o display server fazer isso.
static void __winmin(int fd)
{
// #bugbug
// Esse tipo de torina nao eh atribuiçao do terminal.
// Talvez seja atribuiçao do display server.
// Talvez uma biblioteca client side tambem possa tratar disso.
// Talvez um wm client-side tambem possa tratar isso.

    int wid        = (int) Terminal.main_window_id;
    int client_wid = (int) Terminal.client_window_id;

// #bugbug
// Estamos chamando o kernel pra pegar informações sobre tela.
// Devemos considerar as dimensões da área de trabalho e
// não as dimensões da tela.
// #todo: Devemos fazer requests ao servidor para pegar essas informações.
// #todo: Criar requests para pegar os valores da área de trabalho.
    unsigned long w=rtl_get_system_metrics(1);
    unsigned long h=rtl_get_system_metrics(2);
                  // h=h-40;

// resize
    //unsigned long w_width=100;
    //unsigned long w_height=100;
    //if(w>200){w_width=200;}
    //if(h>100){w_height=100;}

    unsigned long w_width  = (w>>1);
    unsigned long w_height = (h>>1);

    if (fd<0){
        return;
    }
// Change position, resize and redraw the window.
    gws_change_window_position(fd,wid,0,0);
    gws_resize_window( fd, wid, w_width, w_height );
    gws_redraw_window( fd, wid, TRUE );

//---------------

// get the info for the main window.
// change the position of the terminal window.
// its because the client are also changed.
// Get window info:
// IN: fd, wid, window info structure.

    gws_get_window_info( fd, wid, (struct gws_window_info_d *) wi );
    if (wi->used != TRUE){ 
        return; 
    }
    if (wi->magic != 1234) { return; }

// Show info:
// Frame: l,t,w,h
    //printf("Frame info: l=%d t=%d w=%d h=%d\n",
    //    wi->left, wi->top, wi->width, wi->height );
// Client rectangle: l,t,w,h
    //printf("Client rectangle info: l=%d t=%d w=%d h=%d\n",
    //    wi->cr_left, wi->cr_top, wi->cr_width, wi->cr_height );

// The terminal window. (client area)
// Change position, resize and redraw the window.

    // Always in 0,0
    //gws_change_window_position(fd,client_wid,wi->cr_left,wi->cr_top);
    gws_change_window_position(
        fd, client_wid, 
        0,
        0 );
    gws_resize_window(fd, client_wid, wi->cr_width, wi->cr_height );
    gws_redraw_window(fd, client_wid, TRUE );
}

// command "window"
// Testando serviços variados.
void __test_gws(int fd)
{
    int Window = Terminal.main_window_id;
    //int Window = Terminal.client_window_id;

// Parameters:
    if (fd<0){
        return;
    }

    gws_change_window_position(fd,Window,0,0);
    gws_resize_window( fd, Window, 400, 400);
    //gws_refresh_window(fd,Window); //#bugbug
    //text
    //gws_draw_text(fd,Window,0,0,COLOR_RED,"This is a string");
//redraw and refresh.
    gws_redraw_window( fd, Window, TRUE );
//redraw and not refresh
    //gws_redraw_window( fd, Window, FALSE );
    //text
    //gws_draw_text(fd,Window,0,0,COLOR_RED,"This is a string");
}

// Comand 'w-main'.
static void __test_winfo(int fd, int wid)
{
    struct gws_window_info_d *Info;

// Parameters:
    if (fd<0) { return; }
    if (wid<0){ return; }

    Info = (void*) malloc( sizeof(struct gws_window_info_d) );
    if ((void*) Info == NULL){
        return;
    }
    memset ( Info, 0, sizeof(struct gws_window_info_d) );

// Get window info:
// IN: fd, wid, window info structure.
    gws_get_window_info( fd, wid, (struct gws_window_info_d *) Info );
    if (Info->used != TRUE){ 
        return; 
    }
    if (Info->magic != 1234){ return; }

// Show info:
// Frame: l,t,w,h
    printf("Frame info: l=%d t=%d w=%d h=%d\n",
        Info->left, Info->top, Info->width, Info->height );
// Client rectangle: l,t,w,h
    printf("Client rectangle info: l=%d t=%d w=%d h=%d\n",
        Info->cr_left, Info->cr_top, Info->cr_width, Info->cr_height );
}

/*
static void __test_rand(void);
static void __test_rand(void)
{
   int i, n;
   time_t t;
  
   n = 5;
   
   //printf ("M=%d\n",rtl_get_system_metrics(118) ); //jiffies
    
   //Intializes random number generator
   //srand((unsigned) time(&t));

   //Print 5 random numbers from 0 to 49 
   for( i = 0 ; i < n ; i++ ) {
      printf("%d\n", rand() % 50);
   }
   
   return(0);
}
*/

static inline void do_int3(void)
{
    asm ("int $3");
}

static inline void do_cli(void)
{
    asm ("cli");
}

// Testing libc functionalities.
static void __libc_test(int fd)
{
    static int NumberOfFilesToCreate = 8;
    int file_index=0;
    char tmp_file_name[64];
    char index_string_buffer[64];

    if (fd < 0)
        return;

    //close(0); 
    //close(1); 
    //close(2);
    //#remember: stderr was redirected to stdout.
    //fclose(stdin); 
    //fclose(stdout); 
    //fclose(stderr); 

    //creat( "newfile.txt", 0666 );  // fcntl.c
    //mkdir( "newdir", 0666 );       // unistd.c

// #test: Cria n files.
// stress test:
// O rootdir tem 512 entradas,
// vai acabar as entradas ou o heap do kernel.
// # OK. It is working.

    //printf ("Creating {%d} files ...\n",NumberOfFilesToCreate);

    cr();
    lf();
    tputstring(fd,"Creating 8 files\n");

    for ( file_index = 0; 
          file_index < NumberOfFilesToCreate; 
          file_index++ )
    {
        // #debug
        //printf ("Creating file number {%d}\n",file_index);

        // Crear the buffer for the next name.
        memset(tmp_file_name,0,64);

        // Set up a custom filename.
        sprintf( tmp_file_name, "new" );
        itoa( (int) file_index, index_string_buffer );
        strcat(tmp_file_name,index_string_buffer);
        strcat(tmp_file_name,".txt");

        // Create next file using libc.
        // call open() with the flag O_CREAT.
        // see: fcntl.c
        creat( tmp_file_name, 0666 );
    };

    // ...
}

// Compare the string typed into the terminal.
// Remember, we have an embedded command interpreter.
static void compareStrings(int fd)
{

// c
// TRUE: Create a process for the command 
// not found in the embedded list.
// FALSE: Send the command line to the application.
    //int CloneAndExecuteMethod1 = FALSE;  // #test: Execute a non-embeded command in a different way 
    int CloneAndExecuteMethod1 = TRUE;  // Execute a non-embeded command.

    if (fd<0){
        return;
    }


//
// Internal command
//

// First we check if the first chars are an internal command.
// If it's not an internal command we try to launch it as 
// a external program.
// There is no parser for command line.

// Test kernel module
    unsigned long mod_ret=0;
    if ( strncmp(prompt,"mod0",4) == 0 )
    {
        // ---------------------------------------
        // Reason 1000: Initialize the module.
        mod_ret = 
            sc81 ( 
                1000,    // Reason 1000: Initialize the module. 
                   0,    // l2 
                   0,    // l3
                   0 );  // l4
        printf("RETURN: >>> {%d}\n",mod_ret);
        if (mod_ret == 4321)
            printf("terminal.bin: Access denied!\n");

        // ---------------------------------------
        // Reason 1001: Test printf function.
        mod_ret = 
            sc81 ( 
                1001,    // Reason 1001: Test printf function. 
                   0,    // l2 
                   0,    // l3
                   0 );  // l4
        printf("RETURN: >>> {%d}\n",mod_ret);
        if (mod_ret == 4321)
            printf("terminal.bin: Access denied!\n");

        goto exit_cmp;
    }


// Unlock network
    if ( strncmp(prompt,"net-on",6) == 0 )
    {
        sc82 ( 22001, 
        1,  // ON 
        0, 0 );

        // #test
        // Also make the DORA DHCP dialog.
        sc82( 22003, 3, 0, 0 );
        goto exit_cmp;
    }
// Lock network
    if ( strncmp(prompt,"net-off",7) == 0 )
    {
        sc82 ( 22001, 
        0,  // OFF
        0, 0 );
        goto exit_cmp;
    }


//
// start-xxx section
//

// Quit embedded shell, 
// launch #shell00.bin
// and start listening to stderr.
    if ( strncmp(prompt, "start-shell", 11) == 0 )
    {
        // #todo: Create a worker for that.
        //printf("Quit embedded shell.\n");
        //printf("Start listening to stderr.\n");
        cr();
        lf();
        tputstring(fd,"Quit embedded shell\n");
        tputstring(fd,"Start listening to stderr\n");
        cr();
        lf();
        isUsingEmbeddedShell = FALSE;
        goto exit_cmp;
    }
// Start the network server.
// Maybe we're gonna connect to this server to get information
// about our network.
    int netd_res = -1;
    if ( strncmp(prompt,"start-netd", 10) == 0 )
    {
        netd_res = 
            (int) gws_clone_and_execute2(
                      fd, 0,0,0,0,
                      "@netd.bin" );
        goto exit_cmp;
    }

// #libc
// Testing libc components.
    if ( strncmp(prompt,"libc",4) == 0 )
    {
        __libc_test(fd);
        goto exit_cmp; 
    }

// Create file using rtl implementation, not posix.
    if ( strncmp(prompt,"create-file",11) == 0 )
    {
        rtl_create_empty_file("newfil.txt");
        goto exit_cmp; 
    }
// Create directory using rtl implementation, not posix.
    if ( strncmp(prompt,"create-dir",10) == 0 )
    {
        rtl_create_empty_directory("newdir");
        goto exit_cmp; 
    }

// exit: Exit the terminal application.
    if ( strncmp(prompt,"exit",4) == 0 )
    {
        cr();
        lf();
        tputstring(fd,"Exit the terminal application\n");
        //cr();
        //lf();
        //rtl_sleep(2000);
        gws_destroy_window(fd,main_window);
        exit(0);
        goto exit_cmp;
    }

// Send systen message to init.bin and
// do NOT wait for response.
// But it will send us a message back.
    if ( strncmp(prompt,"msg1",4) == 0 )
    {
        __test_post_async_hello();
        goto exit_cmp;
    }

// Send system message to init.bin and
// do NOT wait for response.
// But it will send us a message back.
    if ( strncmp(prompt,"msg2",4) == 0 )
    {
        // IN: PID for init process, msgcode, sig, sig.
        rtl_post_to_tid( 0, 44888, 1234, 5678 );
        goto exit_cmp;
    }

// Sleep until
// IN: ms.
    if ( strncmp(prompt,"sleep",5) == 0 ){
        rtl_sleep(2000);
        goto exit_cmp;
    }

// Network tests

// arp
    if ( strncmp(prompt,"n1", 2) == 0 ){
        sc82( 22003, 1, 0, 0 );
        goto exit_cmp;
    }
// udp
    if( strncmp(prompt,"n2", 2) == 0 ){
        sc82 ( 22003, 2, 0, 0 );
        goto exit_cmp;
    }
// dhcp
    if ( strncmp(prompt,"n3", 2) == 0 ){
        sc82( 22003, 3, 0, 0 );
        goto exit_cmp;
    }

// Enable mouse.
// Begging the display server to initialize
// the mouse support. The kernel part and the ws part.
    if ( strncmp(prompt,"ps2-qemu", 8) == 0 )
    {
        // #todo
        // Create a wrapper for that request.
        gws_async_command(fd,44,0,0);
        goto exit_cmp;
    }


// yes or no.
// see: stdio.c
    static int yn_result = -1;
    if ( strncmp(prompt,"yn",2) == 0 )
    {
        yn_result = (int) rtl_y_or_n();
        if (yn_result == TRUE){
            printf("Returned TRUE\n");
        }
        if (yn_result == FALSE){
            printf("Returned FALSE\n");
        }
        if ( yn_result != TRUE && yn_result != FALSE ){
            printf("Returned Invalid result\n");
        }
        goto exit_cmp;
    }

/*
// open1
// Test open() function.
    if ( strncmp(prompt,"open1",5) == 0 )
    {
        // #test: ok, found.
        open("/DEV/TTY0",          0, "a+"); 
        open("/DEV/TTY1",          0, "a+"); 
        open("/DEV/PS2KBD",        0, "a+");
        open("/DEV/MOUSEKBD",      0, "a+");
        open("/DEV/DEV_1234_1111", 0, "a+");
        open("/DEV/DEV_8086_100E", 0, "a+");
        // ...
        goto exit_cmp;
    }
*/

    if (strncmp(prompt, "hishell", 7) == 0)
    {
        if (ptym_fd<0) 
            goto exit_cmp;
        write(ptym_fd, "Hi shell\n", 8);
        goto exit_cmp;
    }

    if (strncmp(prompt, "ptym", 4) == 0)
    {
        int fd = open("/DEV/PTYM", 0, "a+");
        printf("open(/DEV/PTYM) returned fd=%d\n", fd);
        write(fd, "Hello PTYM\n", 12);
        goto exit_cmp;
    }

    char ptys_buffer[64];
    if (strncmp(prompt, "ptys", 4) == 0)
    {
        int fd = open("/DEV/PTYS", 0, "a+");
        printf("open(/DEV/PTYS) returned fd=%d\n", fd);
        memset(ptys_buffer, 0, 64);
        read(fd, ptys_buffer, 64);
        printf("read: %s\n", ptys_buffer);
        goto exit_cmp;
    }

    if ( strncmp(prompt,"int3",4) == 0 ){
        do_int3();
        goto exit_cmp;
    }

    // GP fault
    if ( strncmp(prompt,"cli",3) == 0 ){
        do_cli();
        goto exit_cmp;
    }

// Poweroff via ds.
    if ( strncmp(prompt,"poweroff",8) == 0 )
    {
        terminal_poweroff_machine(fd);
        goto exit_cmp;
    }

// Get window info: main window
// IN: fd, wid
    if ( strncmp(prompt,"w-main",6) == 0 ){
        __test_winfo( fd, main_window );
        goto exit_cmp;
    }

// Get window info: terminal window
// Terminal.client_window_id
// IN: fd, wid
    if ( strncmp(prompt,"w-terminal",10) == 0 ){
        __test_winfo( fd, terminal_window );
        goto exit_cmp;
    }

    if ( strncmp(prompt,"tputc",5) == 0 ){
        tputc(fd, Terminal.client_window_id, 'x', 1);
        goto exit_cmp;
    }

// Print a string inside the client window?
    if ( strncmp(prompt, "string", 6) == 0 )
    {
        cr(); 
        lf();
        tputstring(fd, "This is a string!\n");
        goto exit_cmp;
    }

// Testing escape sequence in the 'kernel console'.
// Test escape sequence do console no kernel.
    if ( strncmp(prompt,"esc-k",5) == 0 )
    {
        // Moving the cursor:
        printf("\033[8Cm Fred\n");         // right
        printf("\033[8Dm Fred\n");         // left
        printf("\033[4Bm cursor down\n");  // down
        printf("\033[8Am cursor up\n");    // up
        // ...
        goto exit_cmp;
    }

// Testing escape sequence inside the client window.
// Test escape sequence do terminal.
    if ( strncmp(prompt,"esc-t",5) == 0 ){
        __test_escapesequence(fd);
        goto exit_cmp;
    }

// Quit 'ds'.
    if ( strncmp(prompt,"ds-quit",7) == 0 ){
        //gws_async_command(fd,88,0,0);  //ok
        goto exit_cmp;
    }

// Testing ioctl
    if ( strncmp(prompt,"ioctl",5) == 0 ){
        __test_ioctl();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"winmax",6) == 0 ){
        __winmax(fd);
        clear_terminal_client_window(fd);
        goto exit_cmp;
    }

    if ( strncmp(prompt,"winmin",6) == 0 ){
        __winmin(fd);
        //clear_terminal_client_window(fd);
        goto exit_cmp;
    }

// #test: 
// Testando serviços diversos.
    if ( strncmp(prompt,"window",6) == 0 ){
        __test_gws(fd);
        goto exit_cmp;
    }

// #test: 
// Update all the windows in the desktop.
    if ( strncmp(prompt,"desktop",7) == 0 ){
        gws_update_desktop(fd);
        goto exit_cmp;
    }

// 'help'
    if ( strncmp(prompt,"help",4) == 0 ){
        doHelp(fd);
        goto exit_cmp;
    }

// 'about'
    if ( strncmp(prompt,"about",5) == 0 ){
        doAbout(fd);
        goto exit_cmp;
    }

// 'console'
    int fg_console = -1;
    if ( strncmp(prompt,"console",7) == 0 )
    {
        fg_console = (int) rtl_get_system_metrics(400);
        fg_console = (int) (fg_console & 0xFF);
        printf("The current fg_console is {%d}\n",fg_console);
        goto exit_cmp;
    }

// 'reboot'
// reboot via ws.
    if ( strncmp(prompt,"reboot",6) == 0 ){
        gws_reboot(fd);
        goto exit_cmp;
    }

// 'cls'
    if ( strncmp(prompt,"cls",3) == 0 ){
        clear_terminal_client_window(fd);
        goto exit_cmp;
    }

// 'clear'
    if ( strncmp(prompt,"clear",5) == 0 ){
        clear_terminal_client_window(fd);
        goto exit_cmp;
    }

/*
// =============
// 't1'
    if ( strncmp(prompt,"t1",2) == 0 ){
        goto exit_cmp;
    }
*/

//
// Not an internal command. Launch program.
//

// #todo
// The kernel is gonna crash if the file was no found.
// see: libs/libgws/gws.c

// Execute the command line found in prompt[]
    int child_tid = -1;
    child_tid = (int) terminal_core_launch_from_cmdline(fd, prompt);
    if (child_tid < 0){
        //printf("term00.bin: on terminal_core_launch_from_cmdline()\n");
        return;
    }

// It uses gws_clone_and_execute2(),
// it means that the display server is clonning himself
// to create a child, not the terminal.
// So, the clone_process function in kernel mode will not
// create the connectors.
// see: libgws.

/*
    //int target_fd = stderr->_file;
    int target_fd = stdin->_file;

    // Create a processs given the command line 
    // saved in input[].
    if (CloneAndExecuteMethod1 == TRUE){
        gws_clone_and_execute_from_prompt(fd);
    
    // Sent the command line to the application
    // via stderr for now.
    // Let's test it with a modified version o cat, cat00.bin
    } else {

        //lseek( fileno(stdin), 0, 1000);
        //rewind(stdin);
        //write ( fileno(stdin), prompt, 40 );
        //rtl_clone_and_execute(test_app);

        //gws_clone_and_execute_from_prompt(fd); 

        rtl_sleep(3000);
    };
*/

exit_cmp:
    return;
}

static void doHelp(int fd)
{
    const char *String = 
        "term00.bin: This is the terminal application\n";

    if (fd<0){
        return;
    }
    cr();
    lf();
    tputstring(fd, String);
    tputstring(fd,"You can type some commands\n");
    tputstring(fd,"cls, help ...\n");
    tputstring(fd,"reboot, shutdown, cat, uname ...\n");
    //cr();
    //lf();
}

// This is the terminal application, 
// the goal is receiving data from other programs via stdout.
// But for now we are using a embedded shell.
static void doAbout(int fd)
{
    const char *String =  "term00.bin: This is the terminal application\n";
    if (fd < 0){
        return;
    }
    cr(); 
    lf();
    tputstring(fd, String);
}

// Draw the prompt
static void doPrompt(int fd)
{
    register int i=0;
    unsigned long CharWidth = __CHAR_WIDTH;
    unsigned long CharHeight = __CHAR_HEIGHT;

    if(fd<0){
        return;
    }
// Clean prompt buffer and setup it again.
    for ( i=0; i<PROMPT_MAX_DEFAULT; i++ ){ prompt[i] = (char) '\0'; };
    prompt[0] = (char) '\0';
    prompt_pos    = 0;
    prompt_status = 0;
    prompt_max    = PROMPT_MAX_DEFAULT;  

// Escrevia no console.
    // Prompt
    //printf("\n");
    //printf("cmdline: Type something\n");
    //printf("$ ");
    //fflush(stdout);

// Cursor do terminal.
    cursor_x = 0;

// linha
    cursor_y++;
    if (cursor_y >= Terminal.height_in_chars)
    {
        // #bugbug #todo #provisório
        clear_terminal_client_window(fd);
    }

// Refresh client window.
    int wid = Terminal.client_window_id;
    if (wid < 0){
        return;
    }

//--------------------------------------
//
// Focus
//

// #important
// Use the right input authority. (display server)

// #test:
// As the display server is the authority for 
// selecting the foreground thread, let's call the display server 
// to set the focus and select the foreground thread 
// associated with the window.

// #test
// Setup this thread as foreground thread,
// this way the app can receive input via stdin.
    //rtl_focus_on_this_thread();

// #test
// Using display server as the input authority.
// ok. It is working.
// But we need the prompt string.
    gws_set_focus(fd,wid);

// -------------------------------

    if (FontInfo.initialized == TRUE)
    {
        CharWidth = FontInfo.width;
        CharHeight = FontInfo.height;
    }

// Draw prompt symbol.
    gws_draw_char ( 
        fd, 
        wid, 
        (cursor_x*CharWidth), 
        (cursor_y*CharHeight), 
        prompt_color, 
        '>' ); 

// Increment x.
    cursor_x++;

// #bugbug
// Refreshing the whole window is too much.
// Refresh only the rectangle of the size of a char or line.

    gws_refresh_retangle(
        fd,
        (cursor_x*CharWidth),
        (cursor_y*CharHeight),
        CharWidth,
        CharHeight );

    // it works
    //gws_refresh_window(fd,wid);
}

// Testing the standard stream.
// Write something in the standard stream and call shell.bin.
void test_standard_stream(int fd)
{
    char buffer[4096];  //#bugbug: 4KB
    int nread = 0;

    // gws_debug_print("test_standard_stream:\n");  

    //FILE *f;
    //f = fopen("syscalls.txt", "r+"); 
    //f = fopen("gramado.txt", "r+"); 
    //f = fopen("kstderr.txt", "r+");
    //f = fopen("g.txt", "r+");

    // Testar   
    //sc80 ( 900, 
        //(unsigned long) "tprintf.bin", 0, 0 );

   //sc80 ( 900, 
     //  (unsigned long) "tprintf.bin", 0, 0 );

   //sc80 ( 900, 
     //  (unsigned long) "tprintf.bin", 0, 0 );
     
    //fseek(f, 0, SEEK_END);   // seek to end of file
    //size_t size = ftell(f);  // get current file pointer
    //fseek(f, 0, SEEK_SET);   // seek back to beginning of file
    //printf (">>>> size %d \n",size);  

    fseek(stdin, 0, SEEK_SET); 
    fseek(stdout, 0, SEEK_SET); 
    fseek(stderr, 0, SEEK_SET); 

    input('\n');
    input('\0');
    //input(-1);

    //#bugbug
    //Não podemos escrever usando o tamanho do buffer
    //porque o arquivo é menor que isso.
    write(fileno(stdin), prompt, 20);//#bugbug sizeof(prompt));    
    write(fileno(stdout),prompt, 20);//#bugbug sizeof(prompt)); 
    write(fileno(stderr),prompt, 20);//#bugbug sizeof(prompt)); 

    //fseek(stdin, 0, SEEK_SET); 
    //fseek(stdout, 0, SEEK_SET); 
    //fseek(stderr, 0, SEEK_SET); 

    int ii=0;
    prompt_pos = 0;
    for(ii=0;ii<32;ii++) {prompt[ii]=0;}

    //sc80 ( 900, 
      //(unsigned long) "tprintf.bin", 0, 0 );

// Clone and execute.
    sc80 ( 
      900, 
      (unsigned long) "shell.bin", 
      0, 
      0 );

    return;
    //while(1);
    

    /*

    int i=0;
    while(1){

        //nread = read ( fileno(f), buffer, sizeof(buffer) ); 
        nread = read ( fileno(f), buffer, size ); 
        //nread = read ( fileno(stdin), buffer, size ); 
        //nread = read ( fileno(stdout), buffer, size ); 

        if( nread>0){
            
            for(i=0;i< size ;i++){

                if( buffer[i] == 0){ printf("FIM0\n"); return; }
                
                //eof
                if( buffer[i] == EOF){ printf("FIM1\n"); return; }
                
                if( buffer[i] != 0){
                    //terminal_write_char(fd, buffer[i]);
                    tputc ((int) fd, window, (int) buffer[i], (int) 1); //com tratamento de escape sequence.
                }
            };
            printf("FIM2\n");
            return;
        }
    };
    */
}

// Launch a child.
void
test_child_message(void)
{
    // #todo: use clone_and_execute.
    sc80 ( 900, 
       (unsigned long) "sh1.bin", 0, 0 );

}

//
// =======================
//

// Write a char into the terminal client window.
// This is the main function to write a char into the window.
// It is called by tputc() and input().
// fd: File descriptor.
// window: The terminal client window id.

void 
terminal_write_char (
    int fd, 
    int window, 
    int c )
{
    static char prev=0;
    unsigned long CharWidth = __CHAR_WIDTH;
    unsigned long CharHeight = __CHAR_HEIGHT;

    if (FontInfo.initialized == TRUE)
    {
        CharWidth = FontInfo.width;
        CharHeight = FontInfo.height;
    }

    unsigned long x = (cursor_x * CharWidth);
    unsigned long y = (cursor_y * CharHeight);

    if (fd<0)    {return;}
    if (window<0){return;}

    // #test: Suppresing this if statement.
    //if (c<0)     {return;}

    // #todo
    // #todo TAB
    /*
    if (c == '\t')
    {
        printf("TAB\n");
        return;
    }
    */


    if (c == '\r')
    {
        cursor_x=0;
        prev = c;
        return;
    }

    //if ( c == '\n' && prev == '\r' ) 
    if (c == '\n')
    {
        //printf("NEWLINE\n");
        cursor_x=0; // começo da linha ...(desnecessário)
        cursor_y++;  //linha de baixo
        // #test
        // #todo: scroll
        if ( cursor_y >= Terminal.height_in_chars )
        {
            clear_terminal_client_window(fd);  //#provisório
        }

        //começo da linha
        prev = c; 
        return;
    }

// Draw!
// Draw the char into the given window.
// Vamos pintar o char na janela usando o display server.
// White on black
// IN: fd, wid, l, t, color, ch.

// Draw and refresh?
    gws_draw_char (
        (int) fd,
        (int) window,
        (unsigned long) (x & 0xFFFF),
        (unsigned long) (y & 0xFFFF),
        (unsigned long) fg_color,
        (unsigned long) c );

// Coloca no buffer de linhas e colunas.
    terminalInsertNextChar((char) c); 

// Circula
// próxima linha.
// começo da linha
    cursor_x++;
    
    //if (cursor_x > __wlMaxColumns)
    if (cursor_x >= Terminal.width_in_chars)
    {
        cursor_y++;
        cursor_x=0;
    }
}

/*
 * terminalInsertNextChar:
 *     Coloca um char na próxima posição do buffer.
 *     Memória de vídeo virtual, semelhante a vga.
 *     #todo: Esse buffer poderia ser um arquivo que o kernel
 * pudesse usar, ou o servidor de recursos gráficos pudesse usar.
 */
//#importante:
//o refresh é chamado no default do procedimento de janela

// Insert char into the line buffer.
void terminalInsertNextChar(char c)
{
	// #todo
	// para alguns caracteres temos que efetuar o flush.
	// \n \r ... ??
	// Coloca no buffer.

    if (cursor_x < 0 || cursor_y < 0)
        return;

    LINES[cursor_y].CHARS[cursor_x] = (char) c;
}

// Insert null terminator
void terminalInsertNullTerminator (void)
{
    terminalInsertNextChar ( (char) '\0' );
}

// Insert LF
void terminalInsertLF (void)
{
    terminalInsertNextChar ( (char) '\n' );
}

// Insert CR
void terminalInsertCR (void)
{
    terminalInsertNextChar ( (char) '\r' );
}

// Line feed
void lf(void)
{
    //#todo
    //terminalInsertLF();

// Next line
    cursor_y++;

// Clear
    if (cursor_y >= Terminal.height_in_chars){
        clear_terminal_client_window(Terminal.client_window_id);
    }
}

// Carriege return
void cr(void)
{
    //#todo
    //terminalInsertCR();
    cursor_x = 0;
}

// voltando uma linha.
void ri (void)
{
	//if ( screen_buffer_y > top ){
		
		// Volta uma linha.
	//	screen_buffer_y--;
	//	screen_buffer_pos = (screen_buffer_pos - columns); 
	//	return;
	//}
	
	//@todo:
	//scrdown();
}

// Delete the char at the cursor position.
void del (void)
{
    if (cursor_x < 0 || cursor_y < 0)
        return;
    
    LINES[cursor_y].CHARS[cursor_x]      = (char) '\0';
    LINES[cursor_y].ATTRIBUTES[cursor_x] = 7;
}

// Testing escape sequence inside the client window.
void __test_escapesequence(int fd)
{
    if (fd<0)
        return;

    tputstring(fd,"\n");
    tputstring(fd, "Testing escape sequence:\n");
    //tputstring(fd, "One: \033[m");          //uma sequencia.
    //tputstring(fd, "Two: \033[m \033[m");   //duas sequencias.
    tputstring(fd, "~Before \033[m ~Inside \033[m ~ After"); 
    //tputstring(fd, "\033[1Am"); //ok
    //tputstring(fd, "\033[2Am X");  // cursor move up.
    tputstring(fd, "\033[4Bm cursor down!\n");  // cursor move down.
    
    //tputstring(fd, "\033[sm");     // save cursor position
    //tputstring(fd, "\033[um");     // restore cursor position
    
    // apagar N bytes na mesma linha.
    //tputstring(fd, "Before --|\033[0Km|-- After \n");  
    tputstring(fd, "Before --|\033[1Km|-- After \n");  
    tputstring(fd, "Before --|\033[2Km|-- After \n");  
    tputstring(fd, "Before --|\033[3Km|-- After \n");  
    tputstring(fd, "Before --|\033[4Km|-- After \n");  
    //tputstring(fd, "Before --|\033[5Km|-- After \n");  
    //tputstring(fd, "Before --|\033[6Km|-- After \n");  
    //tputstring(fd, "Before --|\033[7Km|-- After \n");  
    //tputstring(fd, "Before --|\033[8Km|-- After \n");  

    //move cursor right
    tputstring(fd, "\033[8Cm Fred\n");
    //move cursor left
    tputstring(fd, "\033[8Dm Fred\n");

    tputstring(fd, "done :)\n");
}

// 
void tputstring(int fd, char *s)
{
// #test
// Print a string inside the client window?

    size_t StringSize=0;
    register int i=0;
    char *b = (char *) s;

// Pointer validation
    if ( (void*) s == NULL )
        return;
    if ( *s == 0 )
        return;

// String size
    StringSize = (size_t) strlen(s);
    if (StringSize <= 0){
        return;
    }

// Limits: 
// #test
    //if(StringSize>=32)
        //return;

// Initialize escape sequence steps.
    __sequence_status=0;
    //__csi_buffer_tail = 0;

    for (i=0; i<StringSize; i++){
        tputc(fd, Terminal.client_window_id, b[i], 1);
    }

// Initialize escape sequence steps.
    __sequence_status=0;
    //__csi_buffer_tail = 0;
}


/*
 * tputc:
 *     Draw a char into the client window.
 */
// #todo
// See-github: tlvince/st.c
// #Atenção: 
// A libc do app foi configurada dinamicamente
// para que printf coloque chars no arquivo. Mas 
// a libc desse terminal ainda não foi. Então a printf
// desse terminal escreve no backbuffer e exibe na tela.
// #bugbug: 
// O problema é que dessa forma nada foi colocado no buffer de arquivo.
// #todo
// fazer essa função colocar os chars no buffer de arquivo. 
// Usaremos no scroll.

/*
Black       0;30     Dark Gray     1;30  
Blue        0;34     Light Blue    1;34  
Green       0;32     Light Green   1;32  
Cyan        0;36     Light Cyan    1;36  
Red         0;31     Light Red     1;31  
Purple      0;35     Light Purple  1;35  
Brown       0;33     Yellow        1;33  
Light Gray  0;37     White         1;37   
*/

/*
Position the Cursor:
puts the cursor at line L and column C.
\033[<L>;<C>H 
Or
\033[<L>;<C>f
Move the cursor up N lines:          \033[<N>A
Move the cursor down N lines:        \033[<N>B
Move the cursor forward N columns:   \033[<N>C
Move the cursor backward N columns:  \033[<N>D
Clear the screen, move to (0,0):     \033[2J
Erase to end of line:     \033[K
Save cursor position:     \033[s
Restore cursor position:  \033[u
*/

/*
ESC [ 4 A             move cursor 4 lines up (4 can be any number)
ESC [ 5 B             move cursor 5 lines down
ESC [ 2 K             erase current line 
ESC [ 30;46 m         set black text (30) on cyan background (46)
ESC [ 0 m             reset color and attributes
*/

/*
DEC	HEX	CHARACTER
0	0	NULL
1	1	START OF HEADING (SOH)
2	2	START OF TEXT (STX)
3	3	END OF TEXT (ETX)
4	4	END OF TRANSMISSION (EOT)
5	5	END OF QUERY (ENQ)
6	6	ACKNOWLEDGE (ACK)
7	7	BEEP (BEL)
8	8	BACKSPACE (BS)
9	9	HORIZONTAL TAB (HT)
10	A	LINE FEED (LF)
11	B	VERTICAL TAB (VT)
12	C	FF (FORM FEED)
13	D	CR (CARRIAGE RETURN)
14	E	SO (SHIFT OUT)
15	F	SI (SHIFT IN)
16	10	DATA LINK ESCAPE (DLE)
17	11	DEVICE CONTROL 1 (DC1)
18	12	DEVICE CONTROL 2 (DC2)
19	13	DEVICE CONTROL 3 (DC3)
20	14	DEVICE CONTROL 4 (DC4)
21	15	NEGATIVE ACKNOWLEDGEMENT (NAK)
22	16	SYNCHRONIZE (SYN)
23	17	END OF TRANSMISSION BLOCK (ETB)
24	18	CANCEL (CAN)
25	19	END OF MEDIUM (EM)
26	1A	SUBSTITUTE (SUB)
27	1B	ESCAPE (ESC)
28	1C	FILE SEPARATOR (FS) RIGHT ARROW
29	1D	GROUP SEPARATOR (GS) LEFT ARROW
30	1E	RECORD SEPARATOR (RS) UP ARROW
31	1F	UNIT SEPARATOR (US) DOWN ARROW
*/

// See: 
// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
void 
tputc ( 
    int fd, 
    int window,
    int c, 
    int len )
{
// Pinta o char na janela, estando ou não
// no shell embutido.

    int ivalue=0;
    unsigned char ascii = (unsigned char) c;
    //unsigned char ascii = *c;

// ------------
// Control codes?
// (0x00~0x1F) and 0x7F.

    int is_control=FALSE;
    if (ascii <= 0x1F || ascii == 0x7F)
    {
        is_control = TRUE;
    }

// Invalid socket
    if (fd<0)
        return;
// Invalid target window
    if (window < 0)
        return;
// Invalid char len.
// #bugbug: Isso nem precisa.
     //if(len!=1)
         //len=1;
    //??
    //if(iofd != -1) {}

// #importante
// Se não é controle é string ou escape sequence.

/*
    //string normal
    //if(Terminal.esc & ESC_STR) 
    if (__sequence_status == 0)
    {
        switch (ascii){
        
        // [Esc]
        // Deixou de ser string normal e entramos em uma sequência.
        // Logo abaixo esse char será tratado novamente.
        case '\033':
            printf("FOUND {033}. Start of sequence\n");
            Terminal.esc = ESC_START;
            __sequence_status = 1;
            break;

        // #importante
        // Imprimindo caracteres normais.
        // #todo: talvez possamos usar a API para isso.
        // como acontece nos caracteres digitados no shell interno.
        // #importante
        // Isso vai exibir o caractere mas também
        // na colocar ele no buffer da posição atual.
        default:
            //printf ("%c",ascii);  //debug
                 
            // It's not a control code.
            if(is_control==FALSE){
                terminal_write_char ( fd, window, (int) ascii ); 
            }
            return;
        };
    }
*/

//==============================
// Se uma sequencia foi finalizada, ou nunca foi inicializada.
// Vamos imprimir quando
// uma sequencia nao esta inicializada e
// temos um char que nao eh controle.

    if (__sequence_status == 0)
    {
        if (is_control == FALSE){
            terminal_write_char( fd, window, (int) ascii ); 
            return;
        }
    }

// ===========================
// Se o char eh um control code.
// Control codes. 
// (dentro de um range)

    if (is_control == TRUE){

        switch (ascii)
        {
            //case '\v':    /* VT */
            //case '\a':    /* BEL */    
            case '\t':      /* HT */
            case '\b':      /* BS */
            case '\r':      /* CR */
            case '\f':      /* LF */
            case '\n':      /* LF */

                //if (ascii == '\t')
                    //exit(0); //debug
                terminal_write_char (fd, window, (int) ascii);
                //printf ("%c",ascii); //debug
                return;
                break;

            //(Esc)
            // ESC 27 033  0x1B  \e*  ^[  (Escape character)
            // ESC - sequence starting with ESC (\x1B).
            //case TERMINAL_ESCAPE:
            //case '\e':
            //case '\033':
            //case '\x1b':
            case 0x1B:
                //printf("FOUND {033}. Start of sequence\n");
                __sequence_status = 1;
                Terminal.esc = ESC_START;
                //terminal_write_char ( fd, window, (int) '$');  //debug
                //printf (" {ESCAPE} ");  //debug
                return;
                break;

            case 0x0E:  // SO - Shift Out
            case 0x0F:  // SI - Shift In 
                return;
                break;

            case 0x1A:  // SUB - Substitute
            case 0x18:  // CAN - Cancel
                //csireset ();
                //terminal_write_char ( fd, window, (int) '$'); //debug
                //printf (" {reset?} "); //debug
                return;
                break;

            case 0x00:  // NUL - (IGNORED)
            case 0x05:  // ENQ - Enquiry (IGNORED)
            
            case 0x11:  // DC1 - device control 1
            case 0x12:  // DC2 - device control 2
            case 0x13:  // DC3 - device control 3
            case 0x14:  // DC4 - device control 4
                // Nothing
                return;

            // DEL - Ignored for now.
            case 0x7F:
                //Nothing
                return;

            // ...
        };

        // ... 

// ===========================
// Se encontramos um char logo apos encontrarmos um \033.
// Um 1b já foi encontrado.
// Um \033 foi encontrado.

    } else if (Terminal.esc & ESC_START){

        // Depois de encontrarmos o '[', entramos no ESC_CSI.
        // Vamos analisar a sequencia depois de '['
        // A sequencia vai terminar com um 'm'
        // #todo parse csi
        // CSI - Control Sequence Introducer
        if (Terminal.esc & ESC_CSI){

            switch (ascii)
            {
                // Quando acaba a sequência.
                //'\033[0m'       #Reset text
                case 'm':
                    //printf("FOUND {m}. End of sequence\n");
                    __sequence_status = 0;   // essa sequencia terminou.
                    // agora o buffer esta vazio.
                    // #todo: usarloop para de fato esvaziar o buffer.
                    __csi_buffer_tail = 0;
                    Terminal.esc = 0;  //??
                    //terminal_write_char (fd, window, (int) '$'); //debug
                    //printf (" {m} "); //debug
                    return;
                    break;

                //save cursor position
                case 's':
                    //printf("FOUND {Save cursor position}\n");
                     save_cur();
                    return;
                    break;

                // restore cursor position
                case 'u':
                    //printf("FOUND {Restore cursor position}\n");
                    restore_cur();
                    return;
                    break;

                // UP
                // Move cursor N lines up 
                // (N can be any number)
                // N is inside the buffer
                case 'A':
                    //printf("FOUND {A}\n");
                    ivalue = (int) CSI_BUFFER[0];
                    ivalue = (int) (ivalue & 0xFF); //only the first byte.
                    ivalue = atoi(&ivalue); 
                    //printf("ivalue {%d}\n",ivalue);
                    cursor_y = (cursor_y - ivalue);
                    if (cursor_y<0){ cursor_y=0; }
                    return;
                    break;
                 
                // DOWN
                // Move cursor N lines down
                // N is inside the buffer
                case 'B':
                    //printf("FOUND {B}\n");
                    ivalue = (int) CSI_BUFFER[0];
                    ivalue = (int) (ivalue & 0xFF); //only the first byte.
                    ivalue = atoi(&ivalue); 
                    //printf("ivalue {%d}\n",ivalue);
                    cursor_y = (cursor_y + ivalue);
                    if(cursor_y>24){ cursor_y=24; }
                    return;
                    break;

                // Cursor right.
                // Pegamos o valor que vem antes disso,
                // pra sabermos quando devemos mudar o cursor.
                case 'C':
                    //printf("FOUND {C}\n");
                    ivalue = (int) CSI_BUFFER[0];
                    ivalue = (int) (ivalue & 0xFF); //only the first byte.
                    ivalue = atoi(&ivalue); 
                    //printf("ivalue {%d}\n",ivalue);
                    cursor_x = (cursor_x + ivalue);
                    if (cursor_x >= 80){
                        cursor_x=79; 
                    }
                    return;
                    break;

                // Cursor left.
                // Pegamos o valor que vem antes disso,
                // pra sabermos quando devemos mudar o cursor.
                case 'D':
                    //printf("FOUND {D}\n");
                    ivalue = (int) CSI_BUFFER[0];
                    ivalue = (int) (ivalue & 0xFF); //only the first byte.
                    ivalue = atoi(&ivalue); 
                    //printf("ivalue {%d}\n",ivalue);
                    if (cursor_x >= ivalue)
                    {
                        cursor_x = (cursor_x - ivalue);
                    }
                    if (cursor_x < 0){
                        cursor_x=0;
                    }
                    return;
                    break;

                // 2K   erase 2 bytes in the current line 
                case 'K':
                    //printf("FOUND {K}\n");
                    ivalue = (int) CSI_BUFFER[0];
                    ivalue = (int) (ivalue & 0xFF); //only the first byte.
                    ivalue = atoi(&ivalue); 
                    //printf("ivalue {%d}\n",ivalue);
                    if ( (cursor_x+ivalue) < 80 )
                    {
                        while (ivalue > 0)
                        {
                            terminal_write_char(fd, window, (int) ' ');
                            ivalue--;
                        }
                    }
                    return;
                    break;

                // Estilo de texto.
                // Quando aparece o ';' temos que mudar o estilo.
                // No buffer tem o valor do novo estilo.
                //case TERMINAL_PARAMETER_SEPARATOR:
                case ';':
                    //printf("FOUND {;}\n");
                    ivalue = (int) CSI_BUFFER[0];
                    ivalue = (int) (ivalue & 0xFF); //only the first byte.
                    ivalue = atoi(&ivalue); 
                    if(ivalue==0){}; //reset all modes (styles and colors)
                    if(ivalue==1){}; //set bold mode.
                    if(ivalue==2){}; //set dim/faint mode.
                    if(ivalue==3){}; //set italic mode.
                    if(ivalue==4){}; //set underline mode.
                    if(ivalue==5){}; //set blinking mode
                    if(ivalue==6){}; //
                    if(ivalue==7){}; //
                    if(ivalue==8){}; //set hidden/invisible mode
                    if(ivalue==9){}; //set strikethrough mode.
                    return;
                    break;

                // Vamos apenas colocar no buffer
                // para analizarmos depois.
                // Colocamos no tail e retiramos no head.
                default:
                    //printf ("FOUND {value}\n"); //debug
                    
                    //#test: 
                    // Using only the first offset for now.
                    
                    // Nesse caso estamos colocando números 
                    // depois de encontrarmos o '['.
                    // Estamos em ESC_CSI e continuaremos nele.
                    CSI_BUFFER[0] = (char) ascii;
                    
                    //#bugbug: 'PF'
                    //CSI_BUFFER[__csi_buffer_tail] = (char) ascii;
                    //__csi_buffer_tail++;
                    //if ( __csi_buffer_tail >= CSI_BUFFER_SIZE )
                    //{
                    //    __csi_buffer_tail = 0;
                    //}
                    //printf("value done\n");
                    return;
                    break;
            };


        } else if (Terminal.esc & ESC_STR_END){ 
 
            // ...

        } else if (Terminal.esc & ESC_ALTCHARSET){

            switch (ascii)
            {
                case 'A':  /* UK (IGNORED) */
                case '<':  /* multinational charset (IGNORED) */
                case '5':  /* Finnish (IGNORED) */
                case 'C':  /* Finnish (IGNORED) */
                case 'K':  /* German (IGNORED) */
                    break;
            };


        } else if (Terminal.esc & ESC_TEST) {

            // ...
 
        // Valido para apos ESC_START tambem.
        }else{

            switch (ascii){

            // CSI - Control Sequence Introducer: 
            // sequence starting with ESC [ or CSI (\x9B)
            // ESC [ -  CSI Control sequence introducer
            // Estavamos no ESC_START e encontramos o '['.
            // Encontramos o '[' depois de \033.
            // Entao vamos entrar em ESC_CSI?
            // see: https://man7.org/linux/man-pages/man4/console_codes.4.html
            //case TERMINAL_INTRODUCER:
            case '[':
                //printf ("FOUND {[}\n"); //debug
                Terminal.esc |= ESC_CSI;
                //terminal_write_char ( fd, window, (int) '['); //debug
                return;
                break; 
   
            case '#':
                 //printf ("FOUND {#}\n"); //debug
                 Terminal.esc |= ESC_TEST;
                 break;

            //  ESC P - DCS   Device control string (ended by ESC \)
            case 'P':  /* DCS -- Device Control String */
            case '_':  /* APC -- Application Program Command */
            // ESC ^ - PM    Privacy message (ended by ESC \)
            case '^':  /* PM -- Privacy Message */
            case ']':  /* OSC -- Operating System Command */
            case 'k':  /* old title set compatibility */
                Terminal.esc |= ESC_STR;
                break; 

            /* Set primary charset G0 */ 
            case '(': 
                Terminal.esc |= ESC_ALTCHARSET;
                break;    

            // ESC ( - Start sequence defining G0 character set
            // (followed by one of B, 0, U, K, as below)
            case ')':  /* set secondary charset G1 (IGNORED) */
            case '*':  /* set tertiary charset G2 (IGNORED) */
            case '+':  /* set quaternary charset G3 (IGNORED) */
                Terminal.esc = 0;
                __sequence_status = 0;
                break;  

            // #test
            // #todo: A=LINEFEED D=CARRIEGE RETURN.
            case 'A':
                Terminal.esc = 0;
                tputstring(fd,"\n");
                break;
             
            // ESC D - IND 
            /* IND -- Linefeed */
            // #todo: A=LINEFEED D=CARRIEGE RETURN.
            case 'D': 
                Terminal.esc = 0;
                //terminal_write_char ( fd, window, (int) '$');  //debug
                //printf (" {IND} ");  //debug
                //tputstring(fd,"\r");
                tputstring(fd,"\n");
                break;


            // ESC E - NEL  Newline.
            /* NEL -- Next line */ 
            case 'E': 
                Terminal.esc = 0;
                terminal_write_char ( fd, window, (int) '$'); //debug
                //printf (" {NEL} "); //debug
                break;

            // ESC H - HTS Set tab stop at current column.
            /* HTS -- Horizontal tab stop */
            case 'H':   
                Terminal.esc = 0;
                terminal_write_char ( fd, window, (int) '$'); //debug
                 //printf (" {HTS} "); //debug
                break;

            // ESC M - RI Reverse linefeed.
            /* RI -- Reverse index */
            case 'M':     
                Terminal.esc = 0;
                terminal_write_char ( fd, window, (int) '$'); //debug
                //printf (" {RI} "); //debug
                break;

            // ESC Z - DECID  DEC private identification.
            // The kernel returns the string  ESC[?6c, 
            // claiming that it is a VT102.
            /* DECID -- Identify Terminal */
            case 'Z':  
                 Terminal.esc = 0;
                 terminal_write_char (fd, window, (int) '$'); //debug
                 //printf (" {DECID} "); //debug
                 break;

            // ESC c - RIS  Reset.
            /* RIS -- Reset to inital state */
            case 'c': 
                 Terminal.esc = 0;
                 terminal_write_char ( fd, window, (int) '$'); //debug
                 //printf (" {reset?} "); //debug
                 break; 

            // ESC = - DECPAM   Set application keypad mode
            /* DECPAM -- Application keypad */
            case '=': 
                 Terminal.esc = 0;
                 terminal_write_char ( fd, window, (int) '$'); //debug
                 //printf (" {=} "); //debug
                 break;

            // ESC > - DECPNM   Set numeric keypad mode
            /* DECPNM -- Normal keypad */
            case '>': 
                Terminal.esc = 0;
                terminal_write_char (fd, window, (int) '$'); //debug
                //printf (" {>} "); //debug
                break;

            // ESC 7 - DECSC    Save current state (cursor coordinates,
            //         attributes, character sets pointed at by G0, G1).
            /* DECSC -- Save Cursor */ 
            //case '7':
               //  Terminal.esc = 0;
               //  break;

            // ESC 8 - DECRC    Restore state most recently saved by ESC 7.
            /* DECRC -- Restore Cursor */ 
            //case '8': 
               //  Terminal.esc = 0;
               //  break;

            /* ST -- Stop */
            // ESC \  ST    String terminator
            //0x9C ST String Terminator ???
            //case '\\':   
                 //Terminal.esc = 0;
                 //break;
  
            //erro    
            //default:
                //break; 
            };
        };
        
        // ...

        return;
    };
 
    // ...
}

// # terminal stuff
// Insere um caractere sentro do buffer.

char 
terminalGetCharXY ( 
    unsigned long x, 
    unsigned long y )
{
    if ( x >= __wlMaxColumns || y >= __wlMaxRows )
    {
        // #bugbug
        return 0;
    }

    return (char) LINES[y].CHARS[x];
}


// # terminal stuff
// Insere um caractere dentro do buffer.

void 
terminalInsertCharXY ( 
    unsigned long x, 
    unsigned long y, 
    char c )
{
    if ( x >= __wlMaxColumns || y >= __wlMaxRows )
    {
        return;
    }

    LINES[y].CHARS[x]      = (char) c;
    LINES[y].ATTRIBUTES[x] = 7;
}

// # terminal stuff
static void save_cur (void)
{
    textSavedCol = cursor_x;
    textSavedRow = cursor_y;
}

// # terminal stuff
static void restore_cur (void)
{
    cursor_x = textSavedCol;
    cursor_y = textSavedRow;
}

// terminalClearBuffer:
// Limpa o buffer da tela.
// Inicializamos com espaços.

void terminalClearBuffer (void)
{
    register int i=0;
    int j=0;
    for ( i=0; i<32; i++ )
    {
        for ( j=0; j<80; j++ ){
            LINES[i].CHARS[j]      = (char) ' ';
            LINES[i].ATTRIBUTES[j] = (char) 7;
        };
        LINES[i].left = 0;
        LINES[i].right = 0;
        LINES[i].pos = 0;
    };
}


/*
//#test
void
__testPrintBuffer(void)
{
    register int i=0;
    int j=0;
    for ( i=0; i<32; i++ )
    {
        for ( j=0; j<80; j++ )
        {
            if ( LINES[i].CHARS[j] != 0 )
            {
            }
            
            //LINES[i].CHARS[j]      = (char) ' ';
            //LINES[i].ATTRIBUTES[j] = (char) 7;
        };
        //LINES[i].left = 0;
        //LINES[i].right = 0;
        //LINES[i].pos = 0;
    };
}
*/

// Qual será a linha que estará no topo da janela.
void textSetTopRow (int number)
{
    if (number < 0)
        return;
    textTopRow = (int) number; 
}

int textGetTopRow (void)
{
    return (int) textTopRow;
}

// Qual será a linha que estará na parte de baixo da janela.
void textSetBottomRow (int number)
{
    if (number < 0)
        return;
    textBottomRow = (int) number; 
}

int textGetBottomRow (void)
{
    return (int) textBottomRow; 
}

void textSetCurrentRow (int number)
{
    if (number < 0)
        return;
    cursor_y = (int) number; 
}

int textGetCurrentRow (void)
{
    return (int) cursor_y;
}

void textSetCurrentCol (int number)
{
    if (number < 0)
        return;
    cursor_x = (int) number; 
}

int textGetCurrentCol (void)
{
    return (int) cursor_x; 
}

// Move cursor position
// #bugbug: Is this still valid?
void move_to ( unsigned long x, unsigned long y )
{
    if ( x > __wlMaxColumns || y > __wlMaxRows )
        return;

	//screen_buffer_x = x;
	//screen_buffer_y = y;
    cursor_x = x;
    cursor_y = y;

	//screen_buffer_pos = ( screen_buffer_y * __wlMaxColumns + screen_buffer_x ) ;
}


/* credits: bsd */
/* Pad STRING to COUNT characters by inserting blanks. */

int pad_to (int count, char *string)
{
    register int i=0;

//#todo
//Check string validation?

    i = strlen(string);
    if (i >= count){
        string[i++] = ' ';
    }else{
        while (i < count)
            string[i++] = ' ';
    };
    string[i] = '\0';

    return (int) (i);
}


// Local. Not in use.
int __terminal_clone_and_execute (char *name)
{
    //if( (void*) name == NULL )
    //    return -1;
    //if(*name == 0)
    //    return -1;
    return (int) sc80 ( 900, (unsigned long) name, 0, 0 );
}

void _draw(int fd, int c)
{

    unsigned long CharWidth = __CHAR_WIDTH;
    unsigned long CharHeight = __CHAR_HEIGHT;

    if (FontInfo.initialized == TRUE)
    {
        CharWidth = FontInfo.width;
        CharHeight = FontInfo.height;
    }


   //unsigned long x;
   //x=0x65666768; //last
   

    //printf ("%c",c);
    //fflush(stdout);
    //return;
   
   
                  /*
                    terminal_drawchar_request (
                        (int) fd,//fd,
                        (int) 0, //__response_wid, //window_id,
                        (unsigned long) __tmp_x,//left,
                        (unsigned long) __tmp_y,//top,
                        (unsigned long) COLOR_RED,
                        (unsigned long) x ); 
                        */
                    
                  gws_draw_char (
                      (int) fd,             // fd,
                      (int) 0,              // window id,
                      (unsigned long) __tmp_x,    // left,
                      (unsigned long) __tmp_y,    // top,
                      (unsigned long) fg_color,
                      (unsigned long) c );
      
                    
                        
                 __tmp_x = __tmp_x + CharWidth;
                 
                 //if ( __tmp_x > (8*80) )
                 //{
                 //    __tmp_y = __tmp_y + 8;
                 //    __tmp_x = 0;
                 //}
                 
                //terminal_drawchar_response((int) fd);
}


// worker
static void __on_return_key_pressed(int fd)
{
    unsigned long jiffie_start=0;
    unsigned long jiffie_end=0;
    unsigned long jiffie_delta=0;

// Finalize the command line.
    input('\0');

    //jiffie_start = (unsigned long) rtl_get_system_metrics(118);

    if (fd<0){
        return;
    }
    compareStrings(fd);

    //jiffie_end = (unsigned long) rtl_get_system_metrics(118);
    //jiffie_delta = (jiffie_end-jiffie_start);

// #bugbug: 
// We do not have a function to print strings
// into the terminal's client window.

    //printf("speed: %d ms\n",jiffie_delta);

// Clear prompt.
    doPrompt(fd);
}

static int 
terminalProcedure ( 
    int fd,
    int window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
    if (fd<0)    {return (int) -1;}
    if (window<0){return (int) -1;}  // Event window
    if (msg<0)   {return (int) -1;}  // Event type


    // Ignore KEYUP events
    if (msg == MSG_KEYUP || msg == MSG_SYSKEYUP)
        return 0;

    //if (msg == MSG_KEYDOWN)
        //tputstring(fd, "MSG_KEYDOWN\n");
// ==================

    switch (msg){

    case MSG_DC1:
        tputstring(fd, "MSG_DC1\n");
        break;
    case MSG_DC2:
        tputstring(fd, "MSG_DC2\n");
        break;
    case MSG_DC3:
        tputstring(fd, "MSG_DC3\n");
        break;
    case MSG_DC4:
        tputstring(fd, "MSG_DC4\n");
        break;
        

    //case MSG_QUIT:
    //case 4080:
        //exit(0);
        //break;

    case MSG_KEYDOWN:
        switch (long1)
        {
            //tputstring(fd, "MSG_KEYDOWN\n");

            //case 0:
                //break;

            // [ Enter ]
            case VK_RETURN:
                
                // When using the embedded shell.
                // Compare strings.
                if (isUsingEmbeddedShell == TRUE){
                    //printf ("terminalProcedure; VK_RETURN\n");
                    __on_return_key_pressed(fd);
                    return 0;
                }

                // When not using the embedded shell.
                // Goto next line.
                if (isUsingEmbeddedShell == FALSE)
                {
                    cursor_x++;
                    if (cursor_x >= Terminal.width_in_chars)
                    {
                        cursor_x = Terminal.left;
                        cursor_y++;
                    }
                }

                if (isUsingEmbeddedShell == FALSE)
                {
                    // Actually we need to convert VK into ascii.
                    char toshellBuffer2[4];
                    toshellBuffer2[0] = (char) '\n';  //10
                    write(ptym_fd, toshellBuffer2, 1);
                    return 0;
                }

                return 0;
                break;

            // Draw the char using the display server.
            // tputc() uses escape sequence.
            default:
                // Coloca na cmdline
                if (isUsingEmbeddedShell == TRUE){
                    input(long1);
                }
                
                /*
                // #test: Stop printing
                // Exibe na área de cliente.
                // Estando ou não no shell embutido.
                // Tem suporte a escape sequence.
                tputc(
                    (int) fd, 
                    (int) Terminal.client_window_id, 
                    (int) long1, 
                    (int) 1 );
                */

                // Actually we need to convert VK into ascii.
                if (isUsingEmbeddedShell == FALSE)
                {
                    char toshellBuffer[4];
                    toshellBuffer[0] = (char) long1;
                    write(ptym_fd, toshellBuffer, 1);
                }

                return 0;
                break;
        };
        break;

    //case MSG_KEYUP:
        //return 0;
        //break;

    case MSG_SYSKEYDOWN:
        switch(long1)
        {
            case VK_F1:
                tputstring(fd, "VK_F1\n");
                break;
            case VK_F11:
                tputstring(fd, "VK_F11\n"); // Not expected
                break;
            case VK_F12:
                tputstring(fd, "VK_F12\n");
                break;
            // ...
        };
        return 0;
        break;

    case MSG_SYSKEYUP:
        return 0;
        break;

    // ok. It's working.
    case MSG_PAINT:
        //printf ("terminal.bin: MSG_PAINT\n");
        //clear_terminal_client_window(fd);
        //doPrompt(fd);
       
        // #test
        // Trying to redraw the terminal window inside the
        // client are of the application window.
        // #todo: Take the editor.bin routine as an example for this.
        if (window == main_window)
        {
            // #debug
            //printf("terminal.bin: [MSG_PAINT]\n");

            // Updating the terminal window
            update_clients(fd);
            if (Terminal._mode == TERMINAL_MODE_EMBEDDED)
                doPrompt(fd);
            return 0;
        }
        break;

    case MSG_CLOSE:
        //printf("term00.bin: MSG_CLOSE\n");
        tputstring(fd, "term00.bin: MSG_CLOSE\n");

        // Notify the child app with ETX/EOT
        terminal_notify_child_close();

        // Tear down the terminal windows
        gws_destroy_window(fd, terminal_window);
        gws_destroy_window(fd, main_window);

        // Exit the terminal process itself. (quick and dirty)
        exit(0);
        break;


    //case GWS_Undo:  // [control + z] (Undo)
        //break;
    //case GWS_Cut:   // [control + x] (Cut)
        //break;

    // #test
    // [Control + c] can be used to kill the child,
    // we alread have the child's tid.
    //case GWS_Copy:  // [control + c] (Copy)
        //break;

    //case GWS_Paste:  // [control + v] (Paste)
        //break;
    //case GWS_SelectAll:  // [control + a] (Select all)
        //break;
    //case GWS_Find:  // [control + f] (Find)
        //printf("term00.bin: GWS_Find\n");
        //break;
    //case GWS_Save:  // [control + s] (Save?)
        //break;

    default:
        return 0;
        break;
    };

// done
    return 0;
}

// Get events from stdin, kernel and ws.
// Pegando o input de 'stdin'.
// #importante:
// Esse event loop pega dados de um arquivo.
static int __input_STDIN(int fd)
{
// + Get bytes from stdin.
// + Get system messages from the queue in control thread.
// + Get events from the server.

    FILE *new_stdin;
    int client_fd = fd;
    int window_id = Terminal.client_window_id;
    int C=0;
    int fGetSystemEvents = TRUE;  // from kernel.
    int fGetWSEvents = TRUE;  // from display server.

    //new_stdin = (FILE *) fopen("gramado.txt","a+");
    new_stdin = stdin;

    if ((void*) new_stdin == NULL){
        printf ("__input_STDIN: new_stdin\n");
        goto fail;
    }

/*
// We don't need this
// O kernel seleciona qual será 
// o arquivo para teclado ps2.
    sc80(
        8002,
        fileno(new_stdin),
        0,
        0 );
*/

// Poisiona no início do arquivo.
// #bugbug: Se fizermos isso, 
// então leremos novamente o que ja foi colocado.

    // not standard.
    // volta ao inicio do arquivo em ring0, depois de ter apagado
    // o arquivo.
    // GRAMADO_SEEK_CLEAR
    lseek( fileno(new_stdin), 0, 1000);
    // Atualiza as coisas em ring3 e ring0.
    rewind(new_stdin);

    char chBuffer[4];

    while (1){
        //if (isUsingEmbeddedShell == FALSE){
            //break;
        //}

        // Terminal is reading output from the child.
        // When set to TRUE, the terminal stops acting as the stdin consumer and 
        // instead waits for output from the child process.
        if (isWaitingForOutput == TRUE){

            // Read input from stdin
            // and send it to the pty master fd.
            C = fgetc(stdin);
            if (C > 0)
            {
                // Print the char we get.
                chBuffer[0] = (char) C;
                //tputstring(fd, chBuffer);
                // Send it to the shell.
                write(ptym_fd, &C, 1);

                chBuffer[0] = 0; // clear buffer
            }
            // Read what comes from the shell. And print it
            int ch_read = read(ptym_fd, chBuffer, 1);
            if ( ch_read > 0 )
                tputstring(fd, chBuffer);
            // #todo: Read the output that comes from the child
            // Cant read from stderr anymore
            //C = fgetc(stderr);
            //if (C > 0){
                // socket, wid, msg, ascii, ascii
                //terminalProcedure( client_fd, window_id, MSG_KEYDOWN, C, C );
            //}

        // Terminal is reading keyboard input from stdin
        } else {

        // + Get bytes from stdin.
        // #bubug
        // Logo apos lermos um ENTER, o terminal vai colocar
        // alguma coisa em stdin. Provavelmente estamos lendo
        // alguma coisa da linha de comandos usada pelo processo filho.
        // #bubug
        // Estamos lendo dois ENTER seguidos.
           // C = fgetc(new_stdin);
            //if (C > 0){
                // socket, wid, msg, ascii, ascii
               // terminalProcedure( client_fd, window_id, MSG_KEYDOWN, C, C );
           // }
        };
      
        // + Get system messages from the queue in control thread.
        // System events.
        if (fGetSystemEvents == TRUE){
            __get_system_event( client_fd, window_id );
        }

        // + Get events from the display server.
        if (fGetWSEvents == TRUE){
            // #todo: Change function name to __get_ds_event.
            __get_ds_event( client_fd, main_window );
        }
    };

    //printf ("__input_STDIN: Stop listening stdin\n");
    cr();
    lf();
    tputstring(fd,"__input_STDIN: exit this loop");
    return 0;

fail:
    return (int) -1;
}

// local
// Pegando o input de 'stderr'.
static int __input_from_connector(int fd)
{
// + Get bytes from stderr.
// + Get system messages from the queue in control thread.
// + Get events from the server.

// #importante:
// Esse event loop pega dados de um arquivo.
    int client_fd = fd;
    int window_id = Terminal.client_window_id;
    int C=0;
    int fGetSystemEvents = TRUE;  // from kernel.
    int fGetWSEvents = TRUE;  // from display server.

    //printf ("__input_from_connector: #todo\n");
    cr();
    lf();
    tputstring(fd,"__input_from_connector:\n");

RelaunchShell:

//-------------------------------------------
// The terminal is clonning himself and launching the child.
// It can't be rtl_clone_and_execute2(), where the server
// will clone and launch it.

    memset(prompt,0,sizeof(prompt));
    sprintf(prompt,"cat00 gramado.txt");
    lseek( fileno(stderr), 0, 1000);
    rewind(stderr);
    write ( fileno(stderr), prompt, 40 );
    rtl_clone_and_execute(test_app);

    //gws_clone_and_execute_from_prompt(fd); 

// ------------------------------
// New stdin.
// Reaproveitando a estrutura em ring3 do stderr.
    //new_stdin = (FILE *) fopen("gramado.txt","a+");
    //new_stdin = stderr;
   // __terminal_input_fp = stdin;   //save global.
   // if ((void*) __terminal_input_fp == NULL){
   //     printf("__input_from_connector: __terminal_input_fp\n");
   //     return -1;
   // }

// --------------------------------------
// #test
// Let's get the fd for the connector0.
// We already told to the kernel that we're a terminal.
// We did that in main().

    //int connector0_fd = -1;
    //connector0_fd = (int) sc82(902,0,0,0);

    //if (connector0_fd < 0)
        //goto fail;

// The terminal is reading from connector 0.
    //__terminal_input_fp->_file = (int) connector0_fd;

// -----------------------
// Loop
// Reading from stderr, with a new fd.

    while (1){

        isUsingEmbeddedShell = TRUE;

        // VT Interactivity
        // + Get bytes from stdin.
        C = fgetc(stdin);
        if (C > 0)
        {
            // VT Renderer
            // Process the char.
            terminalProcedure( 
                client_fd,    // socket
                window_id,    // window ID
                MSG_KEYDOWN,  // message code
                C,            // long1 (ascii)
                C );          // long2 (ascii)
            //continue;
        }

        // #todo
        // We need to send the data to the application
        // right after pressing enter.
        // See the procedure for that.

        // EOT - End Of Transmission.
        //if (C == 4){
        //    goto done;
        //}

        // Get system events.
        if (fGetSystemEvents == TRUE){
            __get_system_event( client_fd, window_id );
        }

        // Get events from the server.
        if (fGetWSEvents == TRUE){
            __get_ds_event( client_fd, main_window );
        }

        // Read what the application sent to us. stderr
        while (1){
            isUsingEmbeddedShell = FALSE;
            C = fgetc(stderr);
            //if (C <= 0)
                //break;
            if (C > 0){
                tputc(fd, Terminal.client_window_id, C, 1);
            }
            if (C == '\n')
               break;
            if (C == EOF)
               break;
        }

    };

    goto RelaunchShell;

done:
    printf ("__input_from_connector: Stop listening\n");
    return 0;
fail:
    return -1;
}

// Embedded mode
// Reading from stdin
static int embedded_shell_run(int fd)
{
    isUsingEmbeddedShell = TRUE;
    isWaitingForOutput=FALSE;
    Terminal._mode = TERMINAL_MODE_EMBEDDED;

    if (fd<0)
        goto fail;
    while (1)
    {
        if (isUsingEmbeddedShell != TRUE)
            break;
        __input_STDIN(fd);
    };
    return 0;
fail:
    return (int) -1;
}

// Normal mode
// Reading from connector
static int terminal_run(int fd)
{
    int InputStatus= -1;
    isUsingEmbeddedShell = FALSE;
    Terminal._mode = TERMINAL_MODE_NORMAL;

    while (1){
        InputStatus = __input_from_connector(fd);
        if(InputStatus == 0)
            break;
    };
    return 0;
fail:
    return -1;
}

// Get system events.
// The events came from kernel or other processes.
static void __get_system_event(int fd, int wid)
{
    int msg_code = 0;

// Validate parameters.
    //if (fd<0 || wid<0){
        //printf("__get_system_event: Invalid parameters\n");
        //return;
    //}

// Get one single event.
    if ( rtl_get_event() != TRUE )
        return;

// Dispatch
    msg_code = (int) (RTLEventBuffer[1] & 0xFFFFFFFF);

// Valid range for system events [0~99]
    if (msg_code >= 100)
        return;

    switch (msg_code){

    case MSG_KEYDOWN:
        terminalProcedure ( 
            fd,   // socket 
            wid,  // wid 
            (int) msg_code, 
            (unsigned long) RTLEventBuffer[2],
            (unsigned long) RTLEventBuffer[3] );
        RTLEventBuffer[1] = 0;
        return;
        break;

    case MSG_KEYUP:
        return;
        break;

    case MSG_SYSKEYDOWN:
        terminalProcedure ( 
            fd,   // socket 
            wid,  // wid 
            (int) msg_code, 
            (unsigned long) RTLEventBuffer[2],
            (unsigned long) RTLEventBuffer[3] );
        RTLEventBuffer[1] = 0;
        return;
        break;

    case MSG_SYSKEYUP:
        return;
        break;


    case MSG_DC1:
    case MSG_DC2:
    case MSG_DC3:
    case MSG_DC4:
        terminalProcedure ( 
            fd,   // socket 
            wid,  // wid 
            (int) msg_code, 
            (unsigned long) RTLEventBuffer[2],
            (unsigned long) RTLEventBuffer[3] );
        RTLEventBuffer[1] = 0;
        return;
        break;

    // Accepting only these messages.
    case MSG_CLOSE:
    case MSG_PAINT:
    // ...
        terminalProcedure ( 
            fd,  // socket 
            wid,  // wid 
            (int) msg_code, 
            (unsigned long) RTLEventBuffer[2],
            (unsigned long) RTLEventBuffer[3] );
        RTLEventBuffer[1] = 0;
        break;
    
    // #test
    // The parent (we) was notified when
    // some important event happened with the child.
    // MSG_NOTIFY_PARENT
    case 89:
        cr();
        lf();
        tputstring(fd,"terminal.bin: #test Notify parent");    
        break;

    // #deprecated: Out of range for system messages.
    // #test
    // In this test, some routine sent a message to the 
    // init process and then the init process responded it.
    //case 44888:
       //cr();
       //lf();
       //tputstring(fd,"terminal.bin: 44888 Received");   
       //break;

    default:
        break;
    };
}

// #todo: Change function name to __get_ds_event.
static void __get_ds_event(int fd, int event_wid)
{
// Get only one event from the display server.

    struct gws_event_d lEvent;
    lEvent.used = FALSE;
    lEvent.magic = 0;
    lEvent.type = 0;
    lEvent.long1 = 0;
    lEvent.long2 = 0;

    struct gws_event_d *e;
    e = 
        (struct gws_event_d *) gws_get_next_event(
                                   fd, 
                                   event_wid,
                                   (struct gws_event_d *) &lEvent );
// Invalid event.
    if ((void *) e == NULL)
        return;
    if (e->magic != 1234)
        return;

// Dispatch event.
    int event_type = (int) (e->type & 0xFFFFFFFF);
    if (event_type<0)
        return;
    switch (event_type){
    // ...
    case MSG_PAINT:
    case MSG_CLOSE:
        terminalProcedure( 
            fd,          // socket
            e->window,   // window ID
            e->type,     // message code
            e->long1,    // long1 (ascii)
            e->long2 );  // long2 (ascii)
        break;
    };
}

static void terminalInitSystemMetrics(void)
{
// Screen width and height.
    smScreenWidth = gws_get_system_metrics(1);
    smScreenHeight = gws_get_system_metrics(2);
// Cursor width and height.
    smCursorWidth = gws_get_system_metrics(3);
    smCursorHeight = gws_get_system_metrics(4);
// Mouse pointer width and height.
    smMousePointerWidth = gws_get_system_metrics(5);
    smMousePointerHeight = gws_get_system_metrics(6);

// Char width and height. (from kernel)
    //smCharWidth = gws_get_system_metrics(7);
    //smCharHeight = gws_get_system_metrics(8);
    smCharWidth = __CHAR_WIDTH;
    smCharHeight = __CHAR_HEIGHT;


// Initialize font info based on system metrics,
// maybe we're gonna change it later,
// when we get information from the server.
    // __CHAR_WIDTH __CHAR_HEIGHT 
    FontInfo.width = (unsigned long) __CHAR_WIDTH; //smCharWidth;
    FontInfo.height = (unsigned long) __CHAR_HEIGHT; //smCharHeight;

    FontInfo.id = 0;
    FontInfo.initialized = TRUE;


//#todo:
//vertical scroll size
//horizontal scroll size.

//#importante
//#todo: pegar mais.

    //...

// #todo: 
// Temos que criar essa variável.

    //InitSystemMetricsStatus = TRUE;
} 

static void terminalInitWindowLimits(void)
{

// #todo
// Tem variáveis aqui que não podem ser '0'.

// #todo: 
// Temos que criar essa variável.
/*
    if (InitSystemMetricsStatus == 0){
        terminalInitSystemMetrics();
    }
 */

//
// ## Window limits ##
//

// problemas; 
    //if ( smScreenWidth == 0 || smScreenHeight )
    //{
    //    printf ...
    //}

// Fullscreen support.
    wlFullScreenLeft = 0;
    wlFullScreenTop  = 0;
    wlFullScreenWidth  = smScreenWidth;
    wlFullScreenHeight = smScreenHeight;
// Limite de tamanho da janela.
    wlMinWindowWidth  = (__CHAR_WIDTH * 80); 
    wlMinWindowHeight = (__CHAR_HEIGHT * 25);  //#bugbug ?
    wlMaxWindowWidth  = wlFullScreenWidth;
    wlMaxWindowHeight = wlFullScreenHeight;
// Quantidade de linhas e colunas na área de cliente.
    wlMinColumns = 80;
    wlMinRows = 1;
// Dado em quantidade de linhas.
    textMinWheelDelta = 1;  //mínimo que se pode rolar o texto
    textMaxWheelDelta = 4;  //máximo que se pode rolar o texto	
    textWheelDelta = textMinWheelDelta;
    //...
}

static void terminalInitWindowSizes(void)
{
    if (Terminal.initialized != TRUE){
        printf("terminalInitWindowSizes: Terminal.initialized\n");
        exit(1);
    }

//  ## Window size ##
    //wsWindowWidth = wlMinWindowWidth;
    //wsWindowHeight = wlMinWindowHeight;

// Tamanho da janela do shell com base nos limites 
// que ja foram configurados.
    wsWindowWidth  = Terminal.width;
    wsWindowHeight = Terminal.height;
    if ( wsWindowWidth < wlMinWindowWidth ){
        wsWindowWidth = wlMinWindowWidth;
    }
    if ( wsWindowHeight < wlMinWindowHeight ){
        wsWindowHeight = wlMinWindowHeight;
    }
}

static void terminalInitWindowPosition(void)
{
    if (Terminal.initialized != TRUE){
        printf("terminalInitWindowPosition: Terminal.initialized\n");
        exit(1);
    }
// Window position
    wpWindowLeft = Terminal.left;
    wpWindowTop  = Terminal.top;
    //wpWindowLeft = (unsigned long) ( (smScreenWidth - wsWindowWidth)/2 );
    //wpWindowTop = (unsigned long) ( (smScreenHeight - wsWindowHeight)/2 );  	
}

// __initializeTerminalComponents:
// Não emite mensagens.
// #bugbug
// essas configurações são configurações de janela,
// então estão mais para terminal do que para shell.

static void __initializeTerminalComponents(void)
{
    int i=0;
    int j=0;

    bg_color = COLOR_BLACK;
    fg_color = COLOR_WHITE;
    cursor_x=0;
    cursor_y=0;
    prompt_color = COLOR_GREEN;
    //shellStatus = 0;
    //shellError = 0;

// Inicializando as estruturas de linha.
// Inicializamos com espaços.
// Limpa o buffer de linhas onde os caracteres são colocados.
    terminalClearBuffer();
// Deve ser pequena, clara e centralizada.
// Para ficar mais rápido.
// #importante:
// O aplicativo tem que confiar nas informações 
// retornadas pelo sistema.
// Usar o get system metrics para pegar o 
// tamanho da tela.
//inicializa as metricas do sistema.
    terminalInitSystemMetrics();
//inicializa os limites da janela.
    terminalInitWindowLimits();
//inicia o tamanho da janela.
    terminalInitWindowSizes();
//inicializar a posição da janela.
    terminalInitWindowPosition();
// initialize visible area.
// #todo: criar função para isso
// É melhor que seja pequena por enquanto pra não ativar
// o scroll do kernel e só usar o scroll desse terminal.
    //textTopRow = 0;
    //textBottomRow = 24;
    //terminalNewVisibleArea ( 0, 19);
    //...
// Obs:
// prompt[] - Aqui ficam as digitações. 
    //shellBufferMaxColumns = DEFAULT_BUFFER_MAX_COLUMNS;
    //shellBufferMaxRows    = DEFAULT_BUFFER_MAX_ROWS;
    //buffersize = (shellBufferMaxColumns * shellBufferMaxRows);
// #todo: 
// E o fluxo padrão. Quem configurou os arquivos ???
// o kernel configuroru???
    //...

	//for ( i=0; i<WORKINGDIRECTORY_STRING_MAX; i++ ){
	//	current_workingdiretory_string[i] = (char) '\0';
	//};

    //sprintf ( current_workingdiretory_string, 
    //    SHELL_UNKNOWNWORKINGDIRECTORY_STRING );    

	//...

//done:

    //ShellFlag = SHELLFLAG_COMMANDLINE;

// #bugbug
// Nossa referência é a moldura e não a área de cliente.
// #todo:usar a área de cliente como referência
    //terminalSetCursor(0,0);
    //terminalSetCursor(0,4);

// #todo
// Tentando posicionar o cursor dentro da janela
    //terminalSetCursor( (shell_info.main_window->left/8) , (shell_info.main_window->top/8));	

/*
// #todo:
// Getting info from the server to setup our font info.
// Or maybe tell the server what font we want to use.
// the server has a limited number of embedded fonts for now.
    FontInfo.width = (unsigned long) ?;
    FontInfo.height = (unsigned long) ?;
    FontInfo.id = 0;
    FontInfo.initialized = TRUE;
*/

    //shellPrompt();
}

// Initializing basic variables.
static void __initialize_basics(void)
{
    register int i=0;

// Windows
    main_window=0;
    terminal_window=0;

// Cursor
    cursor_x=0;
    cursor_y=0;

// Font info
    FontInfo.initialized = FALSE;
    FontInfo.width = __CHAR_WIDTH;
    FontInfo.height = __CHAR_HEIGHT;
    FontInfo.id = 0;  // Fail

    __sequence_status=0;

// CSI - Control Sequence Introducer
    for (i=0; i<CSI_BUFFER_SIZE; i++){
        CSI_BUFFER[i] = 0;
    };
    __csi_buffer_tail=0;
    __csi_buffer_head=0;

    __tmp_x=0;
    __tmp_y=0;

// Window limits
    wlMinWindowWidth=0;
    wlMinWindowHeight=0;
    wlMaxWindowWidth=0;
    wlMaxWindowHeight=0;
// ...
}

//
// $
// INITIALIZATION
//

// This routine will initialize the terminal variables, 
// create the socket for the application, connect with the display server, 
// create the main window, create the terminal window and fall into a loop.
// Called by main() in main.c
int terminal_init(unsigned short flags)
{
    const char *display_name_string = "display:name.0";
    int client_fd = -1;
    unsigned long w=0;
    unsigned long h=0;

    //debug_print ("terminal: Initializing\n");

// Initializing basic variables
    __initialize_basics();

// Device info
// #todo: Check for 'zero'.
    w = gws_get_system_metrics(1);
    h = gws_get_system_metrics(2);

    //...

    // pid=2 fd=4
    //printf ("TERMINAL.BIN: pid{%d} fd{%d}\n",
    //    Terminal.pid, Terminal.client_fd );

// Open display.
// IN: hostname:number.screen_number
    Display = (struct gws_display_d *) gws_open_display(display_name_string);
    if ((void*) Display == NULL){
        printf("term00.bin: Display\n");
        goto fail;
    }
// Get client socket
    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        printf("term00.bin: fd\n");
        goto fail;
    }
    Terminal.client_fd = (int) client_fd;

// Windows: it's global now.
    //int main_window = 0;
    //int terminal_window = 0;

// --------------------------------------
// main window
    unsigned long mwWidth  = (w >> 1);
    unsigned long mwHeight = (h >> 1); 
    // #hack
    if (w == 800)
        mwWidth = 640;
    // #hack
    if (w == 640)
        mwWidth = 480;
    // #hack
    if (w == 320)
        mwWidth = 240;
    unsigned long mwLeft = (( w - mwWidth ) >> 1);
    unsigned long mwTop  = (( h - mwHeight) >> 1);

    unsigned int mwColor = COLOR_WINDOW;

// The surface of this thread.
// It has the same values of the main window.
    setup_surface_retangle ( 
        (unsigned long) mwLeft, (unsigned long) mwTop, 
        (unsigned long) mwWidth, (unsigned long) mwHeight );

// ===================================================
// main window

    main_window = 
        (int) gws_create_window (
                client_fd,
                WT_OVERLAPPED, 
                WINDOW_STATUS_ACTIVE,  // status
                WINDOW_STATE_NULL,     // state
                program_name,
                mwLeft, mwTop, mwWidth, mwHeight,
                0, 
                WS_APP | WS_TERMINAL,  // Style 
                mwColor, mwColor );

    if (main_window < 0){
        printf("terminal.c: fail on main_window\n");
        exit(1);
    }
    Terminal.main_window_id = main_window;

    // #test: Show the window early.
    gws_refresh_window(client_fd, main_window);

// ===================================================
// Client area window
// Let's get some values.
// Remember: Maybe the window server changed
// the window size and position.
// We need to get these new values.

// #todo
// #bugbug
// Here need to fit the client are window with the 
// client-area in the main window.

    // Set default values.
    // We're getting the information about the client area
    // right after creating the main window.
    // >> The terminal window needs to fit into the 
    // client are of the main window,
    // So, we simply need to know the width and height,
    // cause a client will be drawed inside the client area.
    unsigned long wLeft   = 0;
    unsigned long wTop    = 0;
    unsigned long wWidth  = mwWidth >> 1;
    unsigned long wHeight =  mwHeight >> 1;

    unsigned int wColor = (unsigned int) bg_color;

// Getting information about the main window.
// We're gonna need this to fit the terminal window
// into the client area of the main window.

    /*
    // #test: Now it's a global thing.
    //struct gws_window_info_d *wi;
    wi = (void*) malloc( sizeof(struct gws_window_info_d) );
    if ((void*) wi == NULL){
        printf("terminal: wi\n");
        exit (1);
    }
    */

    struct gws_window_info_d lWi;

    // IN: fd, wid, window info structure
    gws_get_window_info(
        client_fd, 
        main_window,   // The app window
        (struct gws_window_info_d *) &lWi );

    if (lWi.used != TRUE){
        printf("terminal: lWi.used\n");
        exit (1);
    }
    if (lWi.magic != 1234){
        printf("terminal: lWi.magic\n");
        exit (1);
    }

// Setting new values for the client window.

// #ps:
// Left/top has values, but we use 0,0 for client area.

// Não pode ser maior que o dispositivo.
    if (lWi.cr_left >= w){
        printf("terminal: lWi.cr_left\n");
        exit (1);
    }

// Não pode ser maior que o dispositivo.
    if (lWi.cr_top >= h){
        printf("terminal: lWi.cr_top\n");
        exit (1);
    }

// Não pode ser maior que o dispositivo.
    if (lWi.cr_width == 0 || lWi.cr_width > w){
        printf("terminal: lWi.cr_width\n");
        exit (1);
    }

// Não pode ser maior que o dispositivo.
    if (lWi.cr_height == 0 || lWi.cr_height > h){
        printf("terminal: lWi.height\n");
        exit (1);
    }

// #danger
// Let's get the values for the client area.
// #
// Quando a janela mãe é overlapped,
// então o deslocamento é relativo à
// área de cliente da janela mãe.
// # 
// Obtivemos as dimensões da área de cliente.

// The terminal window needs to fit into the 
// client are of the main window,
// So, we simply need to know the width and height,
// cause a client will be drawed inside the client area.

// Left/top always in 0,0 for client area.
    wLeft   = 0;
    wTop    = 0;
    wWidth  = lWi.cr_width;
    wHeight = lWi.cr_height;

// Create terminal window
    terminal_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_SIMPLE, 1, 1, cw_string,
                  wLeft, wTop, wWidth, wHeight,
                  main_window,
                  WS_CHILD,
                  COLOR_BLACK, COLOR_BLACK );

    if (terminal_window < 0){
        printf("terminal: fail on terminal_window\n");
        exit(1);
    }
    Terminal.client_window_id = terminal_window;

    // #debug
    //gws_draw_rectangle(client_fd, main_window,
        //0, 0, wWidth, wHeight,
        //COLOR_WHITE, TRUE, 0);

    //#debug
    gws_refresh_window(client_fd, terminal_window);

    // #bugbug
    // Its not returning the right client area values.
    //while(1){}

    Terminal._mode = 0;

// #bugbug
// Something is wrong here.
// is it in pixel or in chars?
    Terminal.left = 0;
    Terminal.top  = 0;
    //Terminal.left = wLeft;  //0;
    //Terminal.top  = wTop;   //0;
// Width and height in pixels.
    Terminal.width = wWidth;
    Terminal.height = wHeight;
// Width and height in chars.
    Terminal.width_in_chars = 
        (unsigned long)((wWidth/__CHAR_WIDTH) & 0xFFFF);
    Terminal.height_in_chars = 
        (unsigned long)((wHeight/__CHAR_HEIGHT) & 0xFFFF);

    Terminal.initialized = TRUE;

// Set window with focus
    //gws_async_command(client_fd,9,0,terminal_window);

// Invalidate surface.
    invalidate_surface_retangle();

    //while(1){}

//
// Test 3
//

/*
    __tmp_x = 40;
    __tmp_y = 40;
    // Testing draw a char in a window.
    terminal_drawchar_request (
        (int) client_fd,          //fd,
        (int) __response_wid,     //window_id,
        (unsigned long) __tmp_x,  //left,
        (unsigned long) __tmp_y,  //top,
        (unsigned long) COLOR_RED,
        (unsigned long) 'X' );
    terminal_drawchar_response((int) client_fd);
 */

    //#debug
    //hanging
    //while(1){}

// Initialize globals.
// #importante: 
// Isso será definido somente uma vez.

    __wlMaxColumns = DEFAULT_MAX_COLUMNS;
    __wlMaxRows    = DEFAULT_MAX_ROWS;

// Initializations
// #important:
// We will call this function
// only after having the Terminal structure initialized.
    __initializeTerminalComponents();


    // No embedded shell by default.
    //isUsingEmbeddedShell = FALSE;

// Font info again
// Based on our corrent viewport
// In chars.

    if (Terminal.initialized == TRUE)
    {
        if (FontInfo.initialized == TRUE)
        {
            if (FontInfo.width > 0 && FontInfo.width < Terminal.width)
            {
                Terminal.width_in_chars = 
                    (unsigned long)((Terminal.width/FontInfo.width) & 0xFFFF);
            }
            if (FontInfo.height > 0 && FontInfo.height < Terminal.height)
            {
                Terminal.height_in_chars = 
                    (unsigned long)((Terminal.height/FontInfo.height) & 0xFFFF);
            }
        }
    }

//
// Client
//

// #todo
// Vamos fazer isso outra hora.
// por hora vamos apenas usar o terminal,
// com o input no terminal
// Write something in the standard stream and call shell.bin.

    // test_standard_stream(client_fd);

// ============================================
// focus
// #bugbug
// It needs to be an 'editbox' for typing messages.

/*
    gws_async_command(
         client_fd,
         9,             // set focus
         terminal_window,
         terminal_window );
*/

    //rtl_focus_on_this_thread();

/*
//================
// cls
     gws_redraw_window(client_fd,Terminal.client_window_id,TRUE);
     //#define SYSTEMCALL_SETCURSOR  34
     sc80 ( 34, 2, 2, 0 );
//=================
*/

// Inicialize prompt[]
// #bugbug: Maybe its not good.
    input('\0');
// Clear the terminal window
    clear_terminal_client_window(client_fd);

// #todo
// + Maybe we need to print the banner instead of the prompt.
// + Maybe we need to start the terminal runnig an application
//   or a script.

// Draw the prompt string
    //doPrompt(client_fd);

// Set active window
    gws_set_active( client_fd, main_window );

    //#debug
    gws_refresh_window(client_fd, main_window);


// Open PTYM
    terminal_initialize_pty();

// Open PTYS
    terminal_initialize_ptys_for_the_shell();
// Duplicate it to the standard streams the shell will use it later.
    dup2(ptys_fd,  STDIN_FILENO);  // replaces the shell’s stdin with the PTY slave.
    dup2(ptys_fd, STDOUT_FILENO);  // replaces the shell’s stdout with the PTY slave.
    dup2(ptys_fd, STDERR_FILENO);  // replaces the shell’s stderr with the PTY slave.

    //write(STDOUT_FILENO, "[dup2 done]\n", 12);

/*
    lseek( fileno(stdin), 0, 1000);
    lseek( fileno(stdout), 0, 1000);
    lseek( fileno(stderr), 0, 1000);
    rewind(stdin);
    rewind(stdout);
    rewind(stderr);
*/

// Launch our shell child
// It sets the flag isWaitingForOutput to TRUE
// so the terminal starts reading from the child process.

    //const char *fake_cmdline = "#shell.bin";
    //terminal_core_launch_from_cmdline(client_fd,fake_cmdline);

// Launch the child, get the tid and delegate the 
// hability to read from stdin without been the foreground thread.
    const char *filename = "#shell.bin";
    int tid = (int) rtl_clone_and_execute_return_tid(filename);
    if (tid < 0){
        tputstring(client_fd, "term00: shell failed\n");
        exit(0);
    }
    //rtl_sleep(4000);
// Delegate foreground to the child
// Delegate a second stdin reader for the foreground thread.
// Only the foreground thread can change this.
    sc82(10013, tid, tid, tid);

/*
    char buf[128];
    ssize_t n = read(ptym_fd, buf, sizeof(buf)-1);
    if (n > 0) {
        buf[n] = 0;
        tputstring(client_fd,"[TERM READ]: ");
        tputstring(client_fd,buf);
        tputstring(client_fd,"\n");
    }
*/

/*
    Ah i got it ... after the dup2  the terminal operated with the 
    standard streams 0,1,2 and now all the rules that we created 
    for tty files do not applies anymore ... 
    the rules now are the rules for 0,1,2 that were console rules.
*/

    // #bugbug
    // maybe the terminal is printing garbage before the loop

    isUsingEmbeddedShell = FALSE;

    char coolCharBuffer[4];
    int ch_read=0;
    while (1){

        // 1. Pump events from Input Broker (system events)
        __get_system_event(client_fd, Terminal.client_window_id);

        // 2. Read what comes from the shell and print it
        coolCharBuffer[0] = 0;
        coolCharBuffer[1] = 0;
        while (1){
            ch_read = (int) read(ptym_fd, coolCharBuffer, 1);
            if (ch_read <= 0)
                break;
            tputstring(client_fd, coolCharBuffer);
        };

        // 3. Pump events from Display Server
        __get_ds_event( client_fd, main_window );
    };


/*
    char coolCharBuffer[256];
    int ch_read=0;
    while (1){

         // #bugbug
         // maybe the terminal is printing garbage before the 
         //loop

        // Get input from kernel and send it to the shell
        __get_system_event(client_fd, Terminal.client_window_id);

        // Read what comes from the shell. And print it
        coolCharBuffer[0] = 0;
        coolCharBuffer[1] = 0;
        while(1){
            //memset(coolCharBuffer,0,256);
            ch_read = read(ptym_fd, coolCharBuffer, 256);
            if (ch_read <= 0)
                break;
            // Null-terminate for safety if you want to treat it as a string 
            coolCharBuffer[ch_read] = '\0';
            tputstring(client_fd, coolCharBuffer);
        };

        __get_ds_event( client_fd, main_window );
    };
*/

// Input loop!
// local routine.
    //int InputStatus=-1;

// -------------------------
// Embedded shell
// Reading from stdin
/*
    InputStatus = (int) embedded_shell_run(client_fd);
    if (InputStatus < 0)
        goto fail;
*/

/*
// -------------------------
// Reading from the child.
// Reading from connector.
    InputStatus = terminal_run(client_fd);
    if (InputStatus < 0)
        goto fail;
*/

done:
    printf("term00.bin: Bye\n");
    return 0;

fail:
    // #bugbug: This code is running. We're simply avoiding the noise.
    printf("term00.bin: Fail :)\n");
    return (int) -1;
}

// Main function for the terminal application
int main(int argc, char *argv[])
{ 
    int Status = -1;

    //debug_print ("terminal.bin:\n");
    //printf("TERM: main() started in thread %d\n", term00_gettid());

    saved_argc = argc;
    saved_argv = argv;

// #todo: Parse parameters.

    if (argc < 1){
    }
/*
    for (i=0; i<argc; i++)
    {
        // #todo
        // Create useful flags for this application.

        if ( strncmp( argv[i], "-a", 2) == 0 ){
        }
        if ( strncmp( argv[i], "-b", 2) == 0 ){
        }
        if ( strncmp( argv[i], "-s", 2) == 0 ){
            asm_flag = 1;
        }
        if ( strncmp( argv[i], "--stats", 7) == 0 ){
            fShowStats = TRUE;
        }
        if ( strncmp( argv[i], "--dumpo", 7) == 0 ){
            fDumpOutput = TRUE;
        }
        //...
    };
*/

// Initializing the structure.
    Terminal.initialized = FALSE;
    Terminal.client_fd = -1;
    Terminal.pid = (pid_t) getpid();
    Terminal.uid = (uid_t) getuid();
    Terminal.gid = (gid_t) getgid();

    //setreuid(-1, -1);
    //setpgrp(0, getpid());

    Terminal.esc = 0;
 
    Terminal.main_window_id = -1;
    Terminal.client_window_id = -1;

    Terminal.left = 0;
    Terminal.top = 0;
    Terminal.width = 50;
    Terminal.height = 50;

    Terminal.width_in_chars = 0;
    Terminal.height_in_chars = 0;

    // ...

// --------------------------------
// #test
// Telling to the kernel that we are a terminal.
// This way the kernel will create connectors where we clone ourself.
// #todo: Create an API for this.
// IN: syscall number,  ... signature.

    sc82( 
        901,
        1234, 1234, 1234 );

// --------------------------------
// Initialization.
// This routine will initialize the terminal variables, 
// create the socket for the application, connect with the display server, 
// create the main window, create the terminal window and fall into a loop.
// See: terminal.c
// IN: flags

    const unsigned short INIT_FLAGS = 0;

    Status = (int) terminal_init(INIT_FLAGS);
    if (Status != 0)
    {
        printf("term00 main(): Something is wrong\n");
        //printf("TERM: main() is returning in thread %d\n", 
            //term00_gettid() );
        
        // #bugbug: This code is running. We're simply avoiding the noise.
    }

    return 0;
}

//
// End
//

