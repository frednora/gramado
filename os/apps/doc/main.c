// main.c
// c-like interpreter.
// Ported from Gramado 32bit. 
// 2022 - Fred Nora

#include "gramcnf.h"

// rtl 
// #bugbug: redundante. Parte disso ja foi incluido logo acima.
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <packet.h>
// The client-side library.
#include <gws.h>



// network ports.
#define PORTS_WS  4040
#define PORTS_NS  4041
#define PORTS_FS  4042
// ...

#define IP(a, b, c, d)  (a << 24 | b << 16 | c << 8 | d)



static int __main_window = -1;
static int __addressbar_window = -1;
static int __button_window = -1;
static int __client_window = -1;

struct child_window_d
{
    unsigned long l;
    unsigned long t;
    unsigned long w;
    unsigned long h;
};
struct child_window_d cwAddressBar;
struct child_window_d cwButton;
struct child_window_d cwClientWindow;
// ...




//#define __VERSION__ "0.1"
//const char copyright[] = "Copyright (c) Fred Nora";

//default name.
char program_name[] = "[Default program name]";
char *compiler_name;
//static int running = 1;
int running = 1;
//Para o caso de não precisarmos produzir 
//nenhum arquivo de output. 
int no_output;

/* While POSIX defines isblank(), it's not ANSI C. */
//#define IS_BLANK(c) ((c) == ' ' || (c) == '\t')
// #important
// Specification for gramc.
//char *standard_spec = "%{%{CC} -c %{I} -o %{O}}";


// =====================================================
static int gramcnf_initialize(void);
static void usage(char **argv);
static void debugShowStat(void);
static void update_clients(int fd);
// =====================================================

//
// == Private functions: prototypes ================
//

static int 
docvProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 );

static int do_event_loop(int fd);

// ====================================


/*
int is_letter(char c);
int is_letter(char c) 
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
*/


// gramcnf_initialize:
// Initialize global variables.
static int gramcnf_initialize(void)
{
    int Status = 0;
    register int i=0;

    //printf ("gramcnf_initialize:\n");

// Clear buffers

// Clear infile and outfile buffers.
    for ( i=0; i<INFILE_SIZE; i++ ){
        infile[i] = '\0';
    };
    sprintf (infile, "; ======================== \n");
    strcat (infile,  "; Initializing infile ...  \n\n");
    for ( i=0; i<OUTFILE_SIZE; i++ ){
        outfile[i] = '\0';
    };
    sprintf (outfile, "; ========================\n");
    strcat (outfile,  ";Initializing outfile ... \n\n");

// Clear text, data, bss buffers.
    sprintf (TEXT, "; ======================== \n");
    strcat  (TEXT, "; Initializing TEXT buffer \n");
    strcat  (TEXT, "segment .text              \n");
    sprintf (DATA, "; ======================== \n");
    strcat  (DATA, "; Initializing DATA buffer \n");
    strcat  (DATA, "segment .data              \n");
    sprintf (BSS,  "; ======================== \n");
    strcat  (BSS,  "; Initializing BSS buffer  \n");
    strcat  (BSS,  "segment .bss               \n");

// Table.

// Contador para não estourar a lista. 
    keyword_count = 0;  
    identifier_count = 0; 
    keyword_count = 0; 
    constant_count = 0; 
    string_count = 0; 
    separator_count = 0; 
    special_count = 0;
    // ...

// Usado pelo lexar pra saber 
// qual lugar na lista colocar o lexeme.

    current_keyword = 0; 
    current_identifier = 0; 
    current_keyword = 0; 
    current_constant = 0; 
    current_string = 0; 
    current_separator = 0; 
    current_special = 0;

// The 'program' structure.

    program.name = program_name;
    program.function_count;
    program.function_list = NULL;
    //...

    return (int) Status;
}

// Mostra as estatísticas para o desenvolvedor.
static void debugShowStat(void)
{
    printf("\n");
    printf("==========================================\n");

//#ifdef LEXER_VERBOSE
    printf("number of liner: {%d}\n",lexer_lineno);
    printf("first line:      {%d}\n",lexer_firstline);
    printf("last line:       {%d}\n",lexer_lastline);
    printf("token count:     {%d}\n",lexer_token_count);
//#endif

//#ifdef PARSER_VERBOSE
    printf("infile_size:     {%d bytes}\n",infile_size);
    printf("outfile_size:    {%d bytes}\n",outfile_size);
//#endif
}

//
// main:
//

static
int doc_viewer( int argc, char *argv[] );


// chamado para interpretar o documento.
static int doc_viewer ( int argc, char *argv[] )
{
// Input
    FILE *fp;
// Output file for compiler.
    FILE *____O;
    register int i=0;
    char *filename;
// Output string.
    char *o;
// Switches 
    int flagA = 0;
    int flagB = 0;
    int flagC = 0;
    int flagD = 0;
    int flagString1 = 0;
    int flagString2 = 0;
    int flagString3 = 0;
    int flagString4 = 0;
    int flagX = 0;
    int flagY = 0;
    int flagZ = 0;
    int flagR = 0;
    int flagS = 0;
    int flagT = 0;

    int fShowStats = FALSE;  //#bugbug
    int fDumpOutput = FALSE;  // Dump output file?

// Carregamos o arquivo num buffer em ring0.
// getc() precisa ler os dados em stdin
// #bugbug: 
// Se o buffer for maior que isso, read() falha.
    char __buf[1024];
    int nreads=0;





//
// ===========================================
//





//
// ===========================================
//



// Initializing
    //debug_print ("gramcnf: Initializing ...\n");  
    //printf ("\n");
    //printf ("main: Initializing ..\n");

// Inicializa variáveis globais.
    gramcnf_initialize();

    //printf ("*breakpoint");
    //while (1){}

//
// ## Args ##
//

// #todo
// O nome do programa é o primeiro comando da linha.
// compiler_name = argv[0];
 
// #debug 
// Mostrando os argumentos. 

    //printf ("argc=%d \n", argc );
    //for ( i=0; i < argc; i++ ){
    //    printf("arg %d = %s \n", i, argv[i] );
    //};

// flags.
// Comparando os argumentos para acionar as flags.

    for ( i=0; i < argc; i++ )
    {
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

// # Arquivo de entrada #
// #bugbug
// lembrando que não podemos mais usar os elementos
// da estrutura em user mode.
// Então o buffer é gerenciado pelo kernel.
// podemos copiar o conteúdo do arquivo para um buffer aqui no programa
// através de fread, mas fread está disponível apenas na libc03.

// Open
    //printf ("\n");
    //printf("Calling fopen()    :)\n");
    //while(1){}

    fp = fopen((char *) argv[2], "rb");
    if ( fp == NULL ){
        printf("doc: Couldn't open the input file\n");
        usage(argv);
        exit(1);
    }

// Input file.
// para que getc leia desse arquivo que carregamos.
    stdin = fp;
    finput = fp;
 
//#debug
// Esse while está aqui para visualizarmos o arquivo carregado.

    //int c;
    //while(1)
    //{
        //c=getc(stdin);
        //if(c == EOF)
            //break;
        //printf("%c",c);
    //}
    //fflush(stdout);
    //while(1){}
//=====================================

// Compiler
// Routine:
// + Initialize the lexer.
// + Parse the tokens.
// + Return a pointer to the output file.

    // IN: dump output file?
    ____O = (FILE *) compiler(fDumpOutput);   

    //if ( (void*) ____O == NULL ){
       //
    //}

    if (fShowStats){
        debugShowStat();
    }

    printf("Done :)\n");
    return 0;
}


static void usage(char **argv)
{
    printf ("\n");
    printf ("====================\n");
    printf ("%s version %s \n", 
        argv[0], 
        __VERSION__ );
}


static void update_clients(int fd)
{
    // Local
    struct gws_window_info_d lWi;

    if (fd<0){
        return;
    }
// Get info about the main window.
//IN: fd, wid, window info structure.
    gws_get_window_info(
        fd, 
        __main_window,   // The app window.
        (struct gws_window_info_d *) &lWi );

// ---------------------------------------------
// address bar
    cwAddressBar.l = (( lWi.cr_width/8 )*2);
    cwAddressBar.t = 4;
    cwAddressBar.w = (( lWi.cr_width/8 )*3);
    cwAddressBar.h = 24;
    gws_change_window_position( 
        fd,
        __addressbar_window,
        cwAddressBar.l,
        cwAddressBar.t );
    gws_resize_window(
        fd,
        __addressbar_window,
        cwAddressBar.w,
        cwAddressBar.h );

    gws_set_focus(fd,__addressbar_window);
    gws_redraw_window(fd, __addressbar_window, TRUE);


//---------------------------------------------
// Button
    cwButton.l = (( lWi.cr_width/8 )*7) -4;
    cwButton.t = 4;
    cwButton.w = (( lWi.cr_width/8 )*1);
    cwButton.h = 24;
    gws_change_window_position( 
        fd,
        __button_window,
        cwButton.l,
        cwButton.t );
    gws_resize_window(
        fd,
        __button_window,
        cwButton.w,
        cwButton.h );

    gws_redraw_window(fd, __button_window, TRUE);

//-----------------------
// the client window

    cwClientWindow.l = 0;
    cwClientWindow.t = (cwAddressBar.t + cwAddressBar.h + 2);
    cwClientWindow.w = lWi.cr_width;
    cwClientWindow.h = (lWi.cr_height - cwClientWindow.t);

    gws_change_window_position( 
        fd,
        __client_window,
        cwClientWindow.l,
        cwClientWindow.t );
    gws_change_window_position( 
        fd,
        __client_window,
        cwClientWindow.l,
        cwClientWindow.t );
    gws_resize_window(
        fd,
        __client_window,
        cwClientWindow.w,
        cwClientWindow.h );

    gws_redraw_window(fd, __client_window, TRUE);
}

static int 
docvProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{

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

    // Evento de teste.
    case 1000:
        // If the event window is the main window, so
        // redraw client window
        if( event_window == __main_window ){
            gws_redraw_window(fd,__client_window,TRUE);
            return 0;
        }
        break;

    //36
    case MSG_MOUSERELEASED:
        
         // #test
         // We are in the browser.
        if (event_window == __client_window)
        {
            // Refresh?
            /*
            gws_draw_char (
                (int) fd,              // fd
                (int) event_window,    // wid
                (unsigned long) long1, // left
                (unsigned long) long2, // top
                (unsigned long) COLOR_BLACK,
                (unsigned long) '.' );
            */
            return 0;
        }
        
        return 0;
        break;

    case GWS_MouseClicked:
        printf("doc.bin: GWS_MouseClicked\n");
        return 0;
        break;

    // #todo;
    // Create the worker update_clients();
    case MSG_PAINT:
        if (event_window == __main_window){
            update_clients(fd);
            return 0;
        }
        //if (event_window == __main_window)
        //{
            //gws_redraw_window(fd,__addressbar_window,TRUE);
            //gws_redraw_window(fd,__button_window,TRUE);
            //gws_redraw_window(fd,__client_window,TRUE);
            //gws_set_focus(fd,__addressbar_window);
        //    return 0;
        //}
        break;

    case MSG_CLOSE:
        printf ("doc.bin: MSG_CLOSE\n");
        gws_destroy_window(fd,__button_window);
        gws_destroy_window(fd,__main_window);
        exit(0);
        break;

    //...
    
    default:
        return -1;
        break;
    };

//fail:
    return -1;
}


static int do_event_loop(int fd)
{
    if(fd<0)
        return -1;

// #test
// pegando um evento com o ws.
// See: libgws/

    struct gws_event_d lEvent;
    lEvent.used = FALSE;
    lEvent.magic = 0;
    lEvent.type = 0;
    lEvent.long1 = 0;
    lEvent.long2 = 0;

    struct gws_event_d *e;

// loop
// Call the local window procedure 
// if a valid event was found.

    while (1)
    {
        //if (isTimeToQuit == TRUE)
            //break;

        e = (struct gws_event_d *) gws_get_next_event(
                fd, 
                __main_window,
                (struct gws_event_d *) &lEvent );

        if ( (void *) e != NULL )
        {
            //if( e->used == TRUE && e->magic == 1234 )
            if (e->magic == 1234){
                docvProcedure( 
                    fd, e->window, e->type, e->long1, e->long2 );
            }
        }
    };

// Exit application withou error.
    return 0;
}


int main ( int argc, char *argv[] )
{

//test
    // doc_viewer(argc,argv);

// -------------------------------------
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = IP(127,0,0,1);
    addr_in.sin_port = PORTS_WS;   
// -------------------------------------

    int client_fd = -1;

    //#debug
    //printf("doc.bin:\n");

// Device info.
    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);
    if ( w == 0 || h == 0 ){
        printf ("doc: w h \n");
        exit(1);
    }

// socket
    //client_fd = socket( AF_INET, SOCK_STREAM, 0 );
    client_fd = socket( AF_INET, SOCK_RAW, 0 );
    if (client_fd<0)
    {
       printf ("doc: Couldn't create socket\n");
       exit(1);
    }

// connect
// Nessa hora colocamos no accept um fd.
// Então o servidor escreverá em nosso arquivo.

    while (TRUE){
        if (connect(client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){ 
            debug_print("doc: Connection Failed\n"); 
            printf     ("doc: Connection Failed\n"); 
        }else{ break; }; 
    };

// ==============================================

// #todo: 
// Salvar em global.
// Por enquanto aqui.

    int main_window=0;
    int addressbar_window=0;    
    int client_window=0;
    int button=0;

// A janela é a metade da tela.
    unsigned long w_width  = (w/2);
    unsigned long w_height = (h/2);
    unsigned long viewwindowx = ( ( w - w_width ) >> 1 );
    unsigned long viewwindowy = ( ( h - w_height) >> 1 );

    if (w == 320)
    {
        // dimensoes
        w_width  = w;
        w_height = h;
        // posicionamento
        viewwindowx = 0;
        viewwindowy = 0;
    }

// ===================
// main window

    const char *app_name = "DOC";

// style: 
// 0x0001=maximized | 0x0002=minimized | 0x0004=fullscreen | 0x0008 statusbar

    main_window = 
        (int) gws_create_window ( 
                  client_fd,
                  WT_OVERLAPPED, 
                  WINDOW_STATUS_ACTIVE,  // status
                  VIEW_NULL,             // view
                  app_name,
                  viewwindowx, viewwindowy, w_width, w_height,
                  0, 
                  0x0000,  
                  COLOR_GRAY, COLOR_GRAY );

    if (main_window < 0){
        debug_print("doc: main_window fail\n"); 
        exit(1);
    }
// Save globally
    if (main_window > 0){
        __main_window = main_window;
    }

    // #debug
    gws_refresh_window( client_fd, main_window );

// ===================
// address bar
// #todo: set focus.
// Se a janela mae for overlapped,
// entao seremos relativos à sua áre de cliente.
    addressbar_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_EDITBOX, 1, 1, "AddressBar",
                  4, 
                  4, 
                  (w_width -4 -4 -24 -4), 
                  24,//32,
                  main_window,  
                  0, COLOR_WHITE, COLOR_WHITE );

    if (addressbar_window < 0){
        debug_print("doc: addressbar_window fail\n"); 
        exit(1);
    }
    // #debug
    gws_refresh_window( client_fd, main_window );

// IN: 
// fd, window id, left, top, color, string
    if( addressbar_window > 0 ){
        __addressbar_window = addressbar_window;
        gws_draw_text (
            (int) client_fd,
            (int) addressbar_window,
             8, 8, (unsigned long) COLOR_BLACK,
            "file:///test1.doc");
    }

// ===================
// button

    cwButton.l = (w_width -24) -4;
    cwButton.t = 4;
    cwButton.w = 24;
    cwButton.h = 24;

    button = 
        (int) gws_create_window (
                  client_fd,
                  WT_BUTTON, 
                  BS_DEFAULT, 
                  1, 
                  ">",  // #todo: Use global variable.
                  cwButton.l, cwButton.t, cwButton.w, cwButton.h,
                  main_window, 0, COLOR_GRAY, COLOR_GRAY );

    if (button < 0){
        debug_print("doc: button fail\n"); 
        exit(1);
    }
    // #debug
    gws_refresh_window( client_fd, main_window );
    
    if(button>0)
        __button_window=button;

// ===================
// client window (White)

    cwClientWindow.l = 0;
    cwClientWindow.t = 4 +24 +4;
    cwClientWindow.w = w_width >> 1;
    cwClientWindow.h = w_height >> 1;

    client_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_SIMPLE,1,1,"client",
                  cwClientWindow.l,
                  cwClientWindow.t,  
                  cwClientWindow.w, 
                  cwClientWindow.h,
                  main_window, 0, COLOR_WHITE, COLOR_WHITE );

    if (client_window < 0){
        debug_print("doc: client_window fail\n"); 
        exit(1);
    }
    // #debug
    gws_refresh_window( client_fd, main_window );
    if (client_window > 0){
        // Save globally.
        __client_window = client_window;
        gws_draw_text (
            (int) client_fd,      // fd
            (int) client_window,  // window id
            (unsigned long) 8,    // left
            (unsigned long) 8,    // top
            (unsigned long) COLOR_BLACK,
            "DOCVIEWER");
    }

// Refresh
    //gws_refresh_window( client_fd, main_window );

// ============================================
// focus
// Set focus on addressbar window.
//    gws_async_command(
//        client_fd, 9, addressbar_window, addressbar_window );
// Set focus on client window.
    //gws_async_command(
    //    client_fd, 9, client_window, client_window );
// =======================================================

// set active window.
    //gws_async_command(
    //     client_fd,
    //     15, 
    //     main_window,
    //     main_window );

    gws_set_focus( client_fd, client_window );
    gws_set_active( client_fd, main_window );

// Refresh
    gws_refresh_window( client_fd, main_window );

// Call the event loop.

    return (int) do_event_loop(client_fd);
    //return 0;
}


//
// End.
//







