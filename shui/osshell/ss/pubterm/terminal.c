// terminal.c
// Created by Fred Nora.

//#include <ctype.h>
// #todo:
// We need to change the name of this document??

// #test:
// Testing ioctl()
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctls.h>
#include <sys/ioctl.h>
#include <stdlib.h>
//#include <stdio.h>
//#include <unistd.h>

#include "include/terminal.h"

// In DE/
const char *child_image_name = "#pubsh.bin";

// network ports.
#define PORTS_WS  4040
#define PORTS_NS  4041
#define PORTS_FS  4042
// ...
#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)


// The main structure.
// see: terminal.h
struct terminal_d  Terminal;
FILE *__terminal_input_fp;

// Windows
struct gws_window_info_d *wi;  // Window info for the main window.
static int main_window=0;
static int terminal_window=0;

// color
static unsigned int bg_color = COLOR_BLACK;
static unsigned int fg_color = COLOR_WHITE;
static unsigned int prompt_color = COLOR_GREEN;
// cursor
static int cursor_x=0;
static int cursor_y=0;
#define __CHAR_WIDTH  8
#define __CHAR_HEIGHT  16

// Embedded shell
// We are using the embedded shell.
static int isUsingEmbeddedShell=TRUE;

// #todo: #maybe:
// Fazer estrutura para gerenciar a sequencia.
static int __sequence_status=0;

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

// System Metrics
int smScreenWidth=0;                   //1 
int smScreenHeight=0;                  //2
unsigned long smCursorWidth=0;         //3
unsigned long smCursorHeight=0;        //4
unsigned long smMousePointerWidth=0;   //5
unsigned long smMousePointerHeight=0;  //6
unsigned long smCharWidth=0;           //7
unsigned long smCharHeight=0;          //8
//...

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


// Lines
// Quantidade de linhas e colunas na área de cliente.
int wlMinColumns=0;
int wlMinRows=0;
int __wlMaxColumns=0;
int __wlMaxRows=0;

//  ## Window size ##
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

// Program name
static const char *program_name = "PUBTERM";
// Client window title
static const char *cw_string = "Client";

//
// == Private functions: Prototypes ==============
//

static void __initialize_basics(void);

static void terminalTerminal(void);
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
static void __try_execute(int fd);

static void doHelp(int fd);
static void doAbout(int fd);
static void libc_test(int fd);

static void clear_terminal_client_window(int fd);
static void __send_to_child (void);
static void __test_winfo(int fd, int wid);

static void __test_ioctl(void);

static void __test_post_async_hello(void);

static void __winmax(int fd);
static void __winmin(int fd);

//#test
static void update_clients(int fd);

//====================================================

//#test
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

/*
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
 */
}


static void __test_post_async_hello(void)
{
// Send async hello. 44888.

// The tid of init.bin is '0', i guess. :)
    int InitProcessControlTID = 0;
// Response support.
    //int __src_tid = -1;
    //int __dst_tid = -1;

    unsigned long message_buffer[32];
    unsigned long MessageCode = 44888;

// The hello message
    message_buffer[0] = 0; //window
    message_buffer[1] = (unsigned long) MessageCode;
    message_buffer[2] = (unsigned long) 1234;   // Signature
    message_buffer[3] = (unsigned long) 5678;   // Signature
    message_buffer[4] = 0;  // Receiver
    message_buffer[5] = 0;  // Sender

// ---------------------------------
// Post
// Add the message into the queue. In tail.
// IN: tid, message buffer address
    rtl_post_system_message( 
        (int) InitProcessControlTID, 
        (unsigned long) message_buffer );
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
// mas podemos deixar o window server fazer isso.
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
    gws_change_window_position(fd,client_wid,wi->cr_left,wi->cr_top);
    gws_resize_window(fd, client_wid, wi->cr_width, wi->cr_height );
    gws_redraw_window(fd, client_wid, TRUE );
}

// Minimize application window.
// #bugbug: Covering the taskbar.
// #todo: Isso pode virar uma função na biblioteca.
// mas podemos deixar o window server fazer isso.
static void __winmin(int fd)
{
// #bugbug
// Esse tipo de torina nao eh atribuiçao do terminal.
// Talvez seja atribuiçao do display server.
// Talvez uma biblioteca client side tambem possa tratar disso.
// Talvez um wm client-side tambem possa tratar isso.

    //if (fd<0){
    //    return;
    //}

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
    gws_change_window_position(fd,client_wid,wi->cr_left,wi->cr_top);
    gws_resize_window(fd, client_wid, wi->cr_width, wi->cr_height );
    gws_redraw_window(fd, client_wid, TRUE );
}

// local
// command "window"
// Testando serviços variados.
void __test_gws(int fd)
{
    int Window = Terminal.main_window_id;
    //int Window = Terminal.client_window_id;

    if(fd<0){
        return;
    }
    gws_change_window_position(fd,Window,0,0);
    gws_resize_window(fd, Window, 400, 400);
    //gws_refresh_window(fd,Window); //#bugbug
    //text
    //gws_draw_text(fd,Window,0,0,COLOR_RED,"This is a string");
//redraw and refresh.
    gws_redraw_window(fd, Window, TRUE);
//redraw and not refresh.
    //gws_redraw_window(
         //fd, Window, FALSE );
    //text
    //gws_draw_text(fd,Window,0,0,COLOR_RED,"This is a string");
}

static void __test_ioctl(void)
{
// Testing the 'foreground console' configuration.
// It's working.

    //printf ("~ioctl: Tests...\n");

// #test
// Changing the output console tty.
// A implementaçao que trata do fd=1 eh console_ioctl
// e nao tty_ioctl.
    //printf("\n");
    printf("Changing the output console\n");
    // IN: fd, request, arg.
    int ioctl_return;
    ioctl_return = (int) ioctl( STDOUT_FILENO, TIOCCONS, 0 );
    printf("ioctl_return: {%d}\n",ioctl_return);
    //printf("Done\n");


// Setup cursor position.
    //ioctl(1, 1001, 10);  // Cursor x
    //ioctl(1, 1002, 10);  // Cursor y
    //ioctl(1, 1003,  2);  //switch to the virtual console 2. 

// Setup cursor position.
    //ioctl( STDOUT_FILENO, 1001, 0 );  // Cursor x 
    //ioctl( STDOUT_FILENO, 1002, 0 );  // Cursor y
    //printf("| Test: Cursor position at 0:0\n");

/*
// Indentation
    ioctl(1, 1010, 8);
    printf ("| Starting at column 8\n");
*/
//-----------

    int maxcol = ioctl( STDOUT_FILENO, 512, 0 );  //Get max col

//-----------

// Goto first line, position 0.
    ioctl( STDOUT_FILENO, 1008, 0 );
    printf("A"); fflush(stdout);

// Goto first line. position 70
    ioctl( STDOUT_FILENO, 1008, maxcol -2); //Set
    printf("a"); fflush(stdout);

//-----------

// Goto last line, position 0.
    ioctl( STDOUT_FILENO, 1009, 0 );
    printf("Z"); fflush(stdout);

// Goto last line. position 70
    ioctl( STDOUT_FILENO, 1009, maxcol-2 );
    printf("z"); fflush(stdout);

// Scroll forever.
    //while(1){
    //    printf("%d\n",rtl_jiffies());
    //   ioctl(1,999,0);  //scroll
    //};

// Flush?
// It's not about flushing the ring3 buffer into the file.
    //ioctl ( STDIN_FILENO,  TCIFLUSH, 0 ); // input
    //ioctl ( STDOUT_FILENO, TCIFLUSH, 0 ); // console
    //ioctl ( STDERR_FILENO, TCIFLUSH, 0 ); // regular file
    //ioctl ( 4,             TCIFLUSH, 0 ); // invalid?

// Invalid limits
    //ioctl ( -1, -1, 0 );
    //ioctl ( 33, -1, 0 );

// Changing the color.
// #deprecated.
// The application will not change the colors anymore.
    //ioctl(1, 1000,COLOR_CYAN);

    //printf ("done\n");

}

// Comand 'w-main'.
static void __test_winfo(int fd, int wid)
{
    struct gws_window_info_d *Info;

    if (fd<0) { return; }
    if (wid<0){ return; }

    Info = (void*) malloc( sizeof(struct gws_window_info_d) );
    if ((void*) Info == NULL){
        return;
    }
    memset ( Info, 0, sizeof(struct gws_window_info_d) );

// Get window info:
// IN: fd, wid, window info structure.
    gws_get_window_info(
        fd, 
        wid,
        (struct gws_window_info_d *) Info );
    if (Info->used != TRUE){ return; }
    if (Info->magic!=1234) { return; }
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

// Try to execute the command line in the prompt[].
static void __try_execute(int fd)
{
    register int i=0;
    register int ii=0;

// Limits:
// + The prompt[] limit is BUFSIZ = 1024;
// + The limit for the write() operation is 512 for now.
    size_t WriteLimit = 512;

// Parameter
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

    char filename_buffer[12]; //8+3+1
    char *p;
    p = prompt;

    while (1)
    {
        // Se tem tamanho o suficiente ou sobra.
        if (ii >= 12){
            filename_buffer[ii] = 0;  //finalize
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
        // What are these chars? It includes symbols? Or just letters?
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

// Is it a valid extension?
// Pois podemos executar sem extensão.
    int isInvalidExt = FALSE;
    int dotWasFound = FALSE;

// Look up for the first occorence of '.'.
// 12345678.123 = (8+1+3) = 12
    for (i=0; i<=12; i++)
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
            isInvalidExt = FALSE;
            break;
        }
    };

// ----------------
// '.' was NOT found, but the filename is bigger than 8 bytes.
    if (dotWasFound != TRUE)
    {
        if (i > 8){
            printf("terminal: Long command name\n");
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
            isInvalidExt = TRUE;
        }

        // Valida a extensao se os proximos chars forem "BIN".
        if ( filename_buffer[i+1] == 'B' &&
             filename_buffer[i+2] == 'I' &&
             filename_buffer[i+3] == 'N'  )
        {
            isInvalidExt = TRUE;
        }
    }
    }

// No extension
// The dot was found, but the extension is invalid.
// Invalid extension.
    if (dotWasFound == TRUE)
    {
        if (isInvalidExt == FALSE){
            printf("terminal: Invalid extension in command name\n");
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
    rewind(stderr);
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
    write(fileno(stderr), prompt, WriteLimit);

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
    return;
fail:
    return;
}

static void libc_test(int fd)
{
    int NumberOfFilesToCreate = 8;
    int file_index=0;
    char tmp_file_name[64];
    char index_string_buffer[64];

// Parameter
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

    printf ("Creating {%d} files ...\n",NumberOfFilesToCreate);

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
}

// Compare the string typed into the terminal.
// Remember, we have an embedded command interpreter.
static void compareStrings(int fd)
{

// Parameter
    if (fd < 0){
        return;
    }

// Unlock network
    if ( strncmp(prompt,"net-on",6) == 0 ){
        sc82 ( 22001, 
        1,  // ON 
        0, 0 );
        goto exit_cmp;
    }
// Lock network
    if ( strncmp(prompt,"net-off",7) == 0 ){
        sc82 ( 22001, 
        0,  // OFF
         0, 0 );
        goto exit_cmp;
    }

//
// start-xxx section
//

// Quit embedded shell, 
// launch #shell.bin
// and start listening to stderr.
    if ( strncmp(prompt, "start-shell", 11) == 0 )
    {
        // #todo: Create a worker for that.
        printf("Quit embedded shell.\n");
        printf("Start listening to stderr.\n");
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
                      fd, 0,0,0,0, "gnssrv.bin" );
        goto exit_cmp;
    }


// #libc
// Testing libc components.
    if ( strncmp(prompt,"libc",4) == 0 ){
        libc_test(fd);        
        goto exit_cmp; 
    }

// Create file using rtl implementation, not posix.
    if ( strncmp(prompt,"create-file",11) == 0 ){
        rtl_create_empty_file("newfil.txt");
        goto exit_cmp; 
    }
// Create directory using rtl implementation, not posix.
    if ( strncmp(prompt,"create-dir",10) == 0 ){
        rtl_create_empty_directory("newdir");
        goto exit_cmp; 
    }

// exit: Exit the terminal application.
    if ( strncmp(prompt,"exit",4) == 0 ){
        printf("~exit: Exit the terminal application\n");
        gws_destroy_window(fd,main_window);
        exit(0);
        goto exit_cmp;
    }

// Sending message to init.bin and
// do NOT wait for response.
// But it will send us a message back.
    if ( strncmp(prompt,"msg1",4) == 0 )
    {
        //while(1)
            __test_post_async_hello();
        goto exit_cmp;
    }

// Sending message to init.bin and
// do NOT wait for response.
// But it will send us a message back.
    if ( strncmp(prompt,"msg2",4) == 0 )
    {
        rtl_post_to_tid(
            0, // init process tid.
            44888, // message code
            1234,
            5678 );
            
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
// Begging the window server to initialize
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
    if ( strncmp(prompt,"open1",5) == 0 )
    {
        open("/DEV/KBD0", 0,"a+"); 
        open("/TTY0", 0, "a+");
        open("/TTY1", 0, "a+");
        open("/DEV_1234_1111", 0, "a+");  // Bochs Graphics Adapter.
        // ...
        goto exit_cmp;
    }
    */

    // int 3
    if ( strncmp(prompt,"int3",4) == 0 ){
        do_int3();
        goto exit_cmp;
    }
    // GP fault
    if ( strncmp(prompt,"cli",3) == 0 ){
        do_cli();
        goto exit_cmp;
    }
    // Poweroff via ws.
    if ( strncmp(prompt,"poweroff",8) == 0 ){
        gws_shutdown(fd);
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

// #test
// Print a string inside the client window?
    if ( strncmp(prompt, "string", 6) == 0 )
    {
        cr();
        lf();
        tputstring(fd, "This is a string!\n");
        cr();
        lf();
        goto exit_cmp;
    }

// Test escape sequence do terminal.
    if ( strncmp(prompt,"esc",3) == 0 ){
        __test_escapesequence(fd);
        goto exit_cmp;
    }

// Test escape sequence do console no kernel.
    if ( strncmp(prompt,"esc2",4) == 0 )
    {
        // Move cursor right
        printf("\033[8Cm Fred\n");
        // Move cursor left
        printf("\033[8Dm Fred\n");
        printf("\033[4Bm cursor down!\n");
        printf("\033[8Am cursor up!\n");
        // ...
        goto exit_cmp;
    }

// Quit 'ws'.
    if ( strncmp(prompt,"ws-quit",7) == 0 ){
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
    if ( strncmp(prompt,"console",7) == 0 ){
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

// =============
// 't1'
    if ( strncmp(prompt,"t1",2) == 0 )
    {
        //printf("One Two \b\b\b\b\b\b\b\b\b\bX\n");
        
        // pixel: ok
        //gws_plot0(fd, 20, 20, 0, COLOR_RED );
        //gws_plot0(fd, 30, 30, 0, COLOR_GREEN );
        //gws_plot0(fd, 40, 40, 0, COLOR_BLUE );

        // char: #testing
        //gws_draw_char ( 
        //    fd, 
        //    Terminal.client_window_id, 
        //    (cursor_x*8), 
        //    (cursor_y*8), 
        //    fg_color, 
        //    '/' ); 
        
        // #bugbug
        // Esta travando.
        // #fixed: Essa chamada esta funcionando agora.
        //gws_async_command(fd,2,0,0);    // PING
        //gws_async_command(fd,5,0,0);  //rectangle ring3
        //gws_async_command(fd,10,0,0); //ramdom tests
        //gws_async_command(fd,6,0,0);  // show fps bar.

        //demos: IN: fd,service,demo_index,0.
        //gws_async_command(fd,4,1,0); // points (works, but sometimes it fails)
        //gws_async_command(fd,4,5,0);  //cube1 (ok)
        //gws_async_command(fd,4,6,0);  //cube2 (ok)
        //gws_async_command(fd,4,7,0);  //curve (ok)
        //gws_async_command(fd,4,9,0);  //cat   (ok)
        //gws_async_command(fd,4,10,0);  // triangle (ok)
        //gws_async_command(fd,4,11,0); //mesh1 (ok)
        //gws_async_command(fd,4,14,0);  //polygon (ok)
        //gws_async_command(fd,4,15,0);  //polygon2 (ok)
        
        //gws_async_command(fd,22,0,0);  // clone a process.
        
        // #bugbug
        // Não podemos pegar esse falor corretamente.
        //printf ("M=%d\n",rtl_get_system_metrics(118) ); //jiffies
        //printf ("M=%d\n",rtl_get_system_metrics(120) ); // variável de time
        //printf ("M=%d\n",rtl_get_system_metrics(1) );
        //printf ("M=%d\n",rtl_get_system_metrics(2) );

        // Clear the window
        // Repaint it using the default background color.
        //gws_clear_window(fd,Terminal.client_window_id);

        goto exit_cmp;
    }

//
// Not a reserved word.
//

// #todo
// The kernel is gonna crash if the file was no found.
// see: libs/libgws/gws.c

    //__try_execute(fd);  // Local worker

// It uses gws_clone_and_execute2(),
// it means that the display server is clonning himself
// to create a child, not the terminal.
// So, the clone_process function in kernel mode will not
// create the connectors.
// libgws
    gws_clone_and_execute_from_prompt(fd);

exit_cmp:
    return;
}

static void doHelp(int fd)
{

// Parameter
    if (fd<0){
        printf("doHelp: Invalid fd\n");
        return;
    }

// Print message inside the terminal.
    cr();
    lf();
    tputstring(fd,"pubterm:");
    cr();
    lf();
    tputstring(fd,"This is the terminal application");
    cr();
    lf();
    tputstring(fd,"pubterm: You can type some commands");
    cr();
    lf();
    tputstring(fd,"cls, reboot, shutdown, uname ...");
    cr();
    lf();

/*
 //# oldstuff
        cursor_y++;

        cursor_x=0;   
        gws_draw_char ( 
            fd, 
            Terminal.client_window_id, 
            (cursor_x*8), 
            (cursor_y*8), 
            fg_color, 
            '\\' ); 

        cursor_x=1;
        gws_draw_char ( 
            fd, 
            Terminal.client_window_id, 
            (cursor_x*8), 
            (cursor_y*8), 
            fg_color, 
            'o' ); 

        cursor_x=2;
        gws_draw_char ( 
            fd, 
            Terminal.client_window_id, 
            (cursor_x*8), 
            (cursor_y*8), 
            fg_color, 
            '/' ); 
 */
}

// This is the terminal application, 
// the goal is receiving data from other programs via stdout.
// But for now we are using a embedded shell.
static void doAbout(int fd)
{

// Parameter
    if (fd < 0)
        return;

    cr();
    lf();
    tputstring(fd,"pubterm: This is the terminal application");
    cr();
    lf();
}

static void doPrompt(int fd)
{
    register int i=0;

// Parameter
    if (fd<0){
        return;
    }

// Clean prompt buffer and setup it again.
    for ( i=0; i<PROMPT_MAX_DEFAULT; i++ ){
        prompt[i] = (char) '\0';
    };
    prompt[0] = (char) '\0';
    prompt_pos = 0;
    prompt_status = 0;
    prompt_max = PROMPT_MAX_DEFAULT;  

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

// Draw prompt symbol.
    gws_draw_char ( 
        fd, 
        wid, 
        (cursor_x*__CHAR_WIDTH), 
        (cursor_y*__CHAR_HEIGHT), 
        prompt_color, 
        '>' ); 

// Increment x.
    cursor_x++;

// #bugbug
// Refreshing the whole window is too much.
// Refresh only the rectangle of the size of a char or line.

    gws_refresh_retangle(
        fd,
        (cursor_x*__CHAR_WIDTH),
        (cursor_y*__CHAR_HEIGHT),
        __CHAR_WIDTH,
        __CHAR_HEIGHT);

    // it works
    //gws_refresh_window(fd,wid);

// #test
// Setup this thread as foreground thread,
// this way the app can receive input via stdin.
    rtl_focus_on_this_thread();
}


// interna
// Isso chama o aplicativo true.bin
// que se conecta a esse via tty e nos envia uma mensagem.
void test_tty_support(int fd)
{
    gws_debug_print("test_tty_support: [FIXME] undefined reference\n");
    return;
    
    /*
    char buffer[32];
    int nread = 0;



    gws_debug_print("test_tty_support:\n");

    int ____this_tty_id = (int) sc80( 266, getpid(), 0, 0 );


   // lançando um processo filho.  
   sc80 ( 900, 
       (unsigned long) "true.bin", 0, 0 );

    int i=0;
    while(1){

        nread = read_ttyList ( ____this_tty_id, buffer, 32 ); 
        
        if( nread>0){
            
            for(i=0;i<32;i++){
                if( buffer[i] != 0)
                    terminal_write_char(fd, buffer[i]);
            }
            return;
        }

        //i++;
        //if(i>20) i=0;
    }
    */
   
    /*
    int nwrite = -1; // bytes escritos.
    size_t __w_size2=0;
    while(1)
    {
        // Escrevendo na tty desse processo e na tty slave pra leitura.
        nwrite = write_ttyList ( ____this_tty_id, 
                     buffer, 
                     __w_size2 = sprintf (buffer,"THIS IS A MAGIC STRING\n")  );
    
        if (nwrite > 0)
           return 0;//goto __ok;
    }
    */
}


// f4
// Send the content of prompt[] to stdin.
// The child process will read this.
// This is why the stdin needs to be a tty.
// (Canonical) 
// If the child is a shell it will read a line.
// (Raw) 
// If the child is a text editor it will read a single char.
// Maybe the shell can change stdin for it's child.
// For now, the shell will need a line.

static void __send_to_child (void)
{
    char *shared_flag   = (char *) (0xC0800000 -0x210);   // flag
    char *shared_memory = (char *) (0xC0800000 -0x200);   // input
    //char *shared_memory = (char *) (0xC0800000 -0x100); // output

    register int i=0;   
   
    // There is a '\n' terminated line in prompt[].
    // #bugbug: Não podemos mandar uma linha sem '\n'.
    fseek(stderr, 0, SEEK_SET); 
    write ( fileno(stderr), prompt, 80);
    
    //copy to shared memory
    //send a system message.(maybe)
    //flag?

    // Send the command line to the shared memory.
    for(i=0; i<80; i++){ shared_memory[i] = prompt[i]; }
    // Clear prompt.
    for(i=0; i<80; i++){ prompt[i]=0; }
    prompt_pos = 0; 
    
    // Notify the child that it has a message in the shared memory.
    shared_flag[0] = 1; 
}


// Write something in the standard stream and call shell.bin.
void test_standard_stream(int fd)
{
    char buffer[4096];  //#bugbug: 4KB
    int nread = 0;

// Parameter
    if (fd < 0)
        return;

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

// lançando um processo filho.  
// #todo: use clone_and_execute.
void test_child_message(void)
{
   sc80 ( 900, (unsigned long) "sh7.bin", 0, 0 );
}

//
// =======================
//

// Called by tputc.
void 
terminal_write_char (
    int fd, 
    int window, 
    int c )
{
// worker
// Print the char into the window.

    static char prev=0;
    unsigned long x = (cursor_x*__CHAR_WIDTH);
    unsigned long y = (cursor_y*__CHAR_HEIGHT);

    if (fd<0)    {return;}
    if (window<0){return;}
    if (c<0)     {return;}

// #todo
// Ver no kernel esse tipo de rotina
// tab;

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
        cursor_x=0;  // começo da linha ...(desnecessário)
        cursor_y++;  // linha de baixo
        // #test
        // #todo: scroll
        if ( cursor_y >= Terminal.height_in_chars ){
            clear_terminal_client_window(fd);  //#provisório
        }

        // começo da linha
        prev = c; 
        return;
    }

// Draw!
// Draw the char into the given window.
// Vamos pintar o char na janela usando o window server.
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
void terminalInsertNextChar(char c)
{
	// #todo
	// para alguns caracteres temos que efetuar o flush.
	// \n \r ... ??
	// Coloca no buffer.

    LINES[cursor_y].CHARS[cursor_x] = (char) c;
}

// # terminal stuff
void terminalInsertNullTerminator(void)
{
    terminalInsertNextChar((char) '\0');
}

// # terminal stuff
void terminalInsertLF (void)
{
    terminalInsertNextChar ( (char) '\n' );
}

// # terminal stuff
void terminalInsertCR (void)
{
    terminalInsertNextChar ( (char) '\r' );
}

// # terminal stuff
//line feed
void lf(void)
{
    //#todo
    //terminalInsertLF();
    
    cursor_y++;
    if (cursor_y >= Terminal.height_in_chars){
        clear_terminal_client_window(Terminal.client_window_id);
    }
}

// # terminal stuff
//carriege return
void cr(void)
{
    //#todo
    //terminalInsertCR();
    cursor_x = 0;
}

// # terminal stuff
// ??
//voltando uma linha.
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

// # terminal stuff
void del (void)
{
    //if(cursor_x<0){cursor_x=0;}
    //if(cursor_y<0){cursor_y=0;}
    
    LINES[cursor_y].CHARS[cursor_x] = (char) '\0';
    LINES[cursor_y].ATTRIBUTES[cursor_x] = 7;
}

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

// Print a string inside the client window
void tputstring(int fd, char *s)
{
    size_t StringSize=0;
    register int i=0;
    char *b = (char *) s;

// Parameters
    if (fd<0)
        return;
    if ((void*) s == NULL)
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

// Parameters
    if (fd<0)
        return;
    if (window<0)
        return;

// ------------
// Control codes?
// (0x00~0x1F) and 0x7F.

    int is_control=FALSE;
    if ( ascii <= 0x1F || ascii == 0x7F )
    {
        is_control = TRUE;
    }

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
                    if ( (cursor_x + ivalue) < 80 )
                    {
                        while (ivalue > 0){
                            terminal_write_char(fd, window, (int) ' ');
                            ivalue--;
                        };
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
void textSetTopRow ( int number )
{
    textTopRow = (int) number; 
}

int textGetTopRow (void)
{
    return (int) textTopRow;
}

// Qual será a linha que estará na parte de baixo da janela.
void textSetBottomRow (int number)
{
    textBottomRow = (int) number; 
}

int textGetBottomRow (void)
{
    return (int) textBottomRow; 
}

void textSetCurrentRow (int number)
{
    cursor_y = (int) number; 
}

int textGetCurrentRow (void)
{
    return (int) cursor_y;
}

void textSetCurrentCol (int number)
{
    cursor_x = (int) number; 
}

int textGetCurrentCol (void)
{
    return (int) cursor_x; 
}

// move_to:
// Move o cursor de posição.
// Assim o próximo char será em outro lugar da janela.
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

// #todo
// Check string validation?

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

//char *hello = "Hello there!\n";
/*
#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port   = 7548, 
    .sin_addr   = IP(192, 168, 1, 79),
};
*/

// interna
// #todo: Use 'const char*'
int __terminal_clone_and_execute (char *name)
{
// Parameter:
    if ((void*) name == NULL)
        goto fail;
    if (*name == 0)
        goto fail;

    return (int) sc80 ( 900, (unsigned long) name, 0, 0 );
fail:
    return (int) -1;
}

void _draw(int fd, int c)
{

    //unsigned long x;
    //x=0x65666768; //last

    if (fd < 0)
        return;

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

// Draw char

    gws_draw_char (
        (int) fd,             // fd,
        (int) 0,              // window id,
        (unsigned long) __tmp_x,    // left,
        (unsigned long) __tmp_y,    // top,
        (unsigned long) fg_color,
        (unsigned long) c );
  
    __tmp_x = __tmp_x + 8;

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

// Parameter
    if (fd <0){
        return;
    }

    //jiffie_start = (unsigned long) rtl_get_system_metrics(118);

// Finalize the command line.
    input('\0');

    compareStrings(fd);

    //jiffie_end = (unsigned long) rtl_get_system_metrics(118);
    //jiffie_delta = (jiffie_end-jiffie_start);

// #bugbug: 
// We do not have a function to print strings
// into the terminal's client window.

    //printf("speed: %d ms\n",jiffie_delta);

// Clear prompt.
    if (Terminal._mode == TERMINAL_MODE_EMBEDDED)
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
    if (fd<0)    {return -1;}
    if (window<0){return -1;}
    if (msg<0)   {return -1;}

// ==================

    switch (msg){

    //case MSG_QUIT:
    //case 4080:
        //exit(0);
        //break;

    case MSG_KEYDOWN:
        switch(long1)
        {
            //case 0:
                //break;

            // [ Enter ]
            case VK_RETURN:
                
                // When using the embedded shell.
                // Compare strings.
                if (isUsingEmbeddedShell == TRUE)
                {
                    //printf ("terminalProcedure; VK_RETURN\n");
                    __on_return_key_pressed(fd);
                    return 0;
                }

                // When not using the embedded shell.
                // Goto next line.
                if (isUsingEmbeddedShell == FALSE){
                    cursor_x++;
                    if (cursor_x >= Terminal.width_in_chars)
                    {
                        cursor_x = Terminal.left;
                        cursor_y++;
                    }
                }

                return 0;
                break;

            // Draw the char using the window server.
            // tputc() uses escape sequence.
            default:
                // Coloca na cmdline
                if (isUsingEmbeddedShell == TRUE){
                    input(long1);
                }
                // Exibe na área de cliente.
                // Estando ou não no shell embutido.
                // Tem suporte a escape sequence.
                tputc(
                    (int) fd, 
                    (int) Terminal.client_window_id, 
                    (int) long1, 
                    (int) 1 );

                return 0;
                break;
        };
        break;


    // #bugbug: Not working
    // It's because the terminal is getting input
    // from file, not from the control thread.
    //case MSG_SYSKEYDOWN:
    //    switch(long1){
    //    case VK_F1: gws_clone_and_execute("browser.bin");  break;
    //    case VK_F2: gws_clone_and_execute("editor.bin");   break;
    //    case VK_F3: gws_clone_and_execute("fileman.bin");  break;
    //    case VK_F4: gws_clone_and_execute("shutdown.bin"); break;
    //    };
    //    return 0;
    //    break;

    // ok. It's working.
    case MSG_PAINT:
        //printf ("terminal.bin: MSG_PAINT\n");
        //clear_terminal_client_window(fd);

        update_clients(fd);
        if (Terminal._mode == TERMINAL_MODE_EMBEDDED)
            doPrompt(fd);
        return 0;
        break;

    case MSG_CLOSE:
        printf("terminal.bin: MSG_CLOSE\n");
        gws_destroy_window(fd,terminal_window);
        gws_destroy_window(fd,main_window);
        exit(0);
        break;

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
    register int C=0;
    FILE *new_stdin;
    int client_fd = fd;
    int window_id = Terminal.client_window_id;

    int fGetSystemEvents = TRUE;  // from kernel.
    int fGetWSEvents = TRUE;      // from display server.

// Parameter
   if (fd < 0)
       goto fail;

// stdin:
    //new_stdin = (FILE *) fopen("gramado.txt","a+");
    new_stdin = stdin;
    if ((void*) new_stdin == NULL){
        printf ("__input_STDIN: new_stdin\n");
        goto fail;
    }

// O kernel seleciona qual será 
// o arquivo para teclado ps2.
    sc80(
        8002,
        fileno(new_stdin),
        0,
        0 );

// Poisiona no início do arquivo.
// #bugbug: Se fizermos isso, 
// então leremos novamente o que ja foi colocado.

    // not standard.
    // volta ao inicio do arquivo em ring0, depois de ter apagado
    // o arquivo.
    // GRAMADO_SEEK_CLEAR
    lseek( fileno(new_stdin), 0, 1000);
    // atualiza as coisas em ring3 e ring0.
    rewind(new_stdin);

// --------------------
// #bubug
// Logo apos lermos um ENTER, o terminal vai colocar
// alguma coisa em stdin. Provavelmente estamos lendo
// alguma coisa da linha de comandos usada pelo processo filho.
// #bubug
// Estamos lendo dois ENTER seguidos.
// Process the char.
// IN: socket, wid, msg, data1, data2

    while (1){

        if (isUsingEmbeddedShell == FALSE){
            break;
        }

        // VT Interactivity
        C = (int) fgetc(new_stdin);
        if (C > 0)
        {
            // VT Renderer
            terminalProcedure( 
                client_fd,
                window_id,
                MSG_KEYDOWN,
                C,
                C );
        }

        // Get system events
        if (fGetSystemEvents == TRUE){
            __get_system_event( client_fd, window_id );
        }

        // Get events from the display server
        if (fGetWSEvents == TRUE){
            __get_ds_event( client_fd, main_window );
        }
    };

    printf ("__input_STDIN: Stop listening stdin\n");
    return 0;
fail:
    return (int) -1;
}

// local
// Pegando o input de 'stderr'.
// It's working
static int __input_from_connector(int fd)
{
// #importante:
// Esse event loop pega dados de um arquivo.

    register int C=0;
    int client_fd = fd;
    int window_id = Terminal.client_window_id;
    int launch_status = -1;

    int fGetSystemEvents = TRUE;  // from kernel.
    int fGetWSEvents = TRUE;      // from display server.

// Parameter
    if (fd < 0)
        goto fail;

    //printf ("__input_from_connector: #todo\n");

RelaunchShell:

//-------------------------------------------
// The terminal is clonning himself and launching the child.
// It can't be rtl_clone_and_execute2(), where the server
// will clone and launch it.
    if ((void*) child_image_name == NULL){
        goto fail;
    }
    launch_status = (int) rtl_clone_and_execute(child_image_name);
    //if (launch_status<0)
        //goto fail;
    rtl_sleep(1000);

// ------------------------------
// New stdin.
// Reaproveitando a estrutura em ring3 do stderr.
// Save it globally.

    __terminal_input_fp = stderr;
    if ((void*) __terminal_input_fp == NULL){
        printf("__input_from_connector: __terminal_input_fp\n");
        goto fail;
    }

// --------------------------------------
// #test
// Let's get the fd for the connector0.
// We already told to the kernel that we're a terminal 
// when we called syscall 901 in main(), so this way 
// we're able to get out connector.
// Our connector is gonna be our new stdin.
// We need to call this routine after the rtl_clone_and_execute().

    int connector0_fd = -1;
    connector0_fd = (int) sc82(902,0,0,0);
    if (connector0_fd < 0){
        goto fail;
    }
// The terminal is reading from connector 0.
    __terminal_input_fp->_file = (int) connector0_fd;

// -----------------------
// Loop
// Reading from your connector, it's our new stdin.
// Probably this is the output file of our connected client.
// Process the char.
// IN: socket, wid, msg, data1, data2

    const int MessageCode = MSG_KEYDOWN;

    while (1){
        // VT Interactivity
        C = (int) fgetc(__terminal_input_fp);
        if (C > 0)
        {
            // VT Renderer
            terminalProcedure( 
                client_fd,
                window_id,
                MessageCode,
                C,
                C );
        }
        // EOT - End Of Transmission.
        //if (C == 4){
        //    goto done;
        //}

        // Get system events
        if (fGetSystemEvents == TRUE){
            __get_system_event( client_fd, window_id );
        }

        // Get events from the display server
        if (fGetWSEvents == TRUE){
            __get_ds_event( client_fd, main_window );
        }

    };

    goto RelaunchShell;

done:
    printf ("__input_from_connector: Stop listening\n");
    return 0;
fail:
    return (int) -1;
}

// Embedded mode
static int embedded_shell_run(int fd)
{
    isUsingEmbeddedShell = TRUE;
    Terminal._mode = TERMINAL_MODE_EMBEDDED;

// Parameter
    if (fd < 0){
        goto fail;
    }
// Loop
    while (1)
    {
        //if (Terminal._mode != TERMINAL_MODE_EMBEDDED){
        if (isUsingEmbeddedShell != TRUE){
            break;
        }
        __input_STDIN(fd);
    };
    return 0;
fail:
    return (int) -1;
}

// Normal mode
static int terminal_run(int fd)
{
    int InputStatus = -1;

    isUsingEmbeddedShell = FALSE;
    Terminal._mode = TERMINAL_MODE_NORMAL;

// Parameter
    if (fd<0){
        goto fail;
    }
// Loop
    while (1)
    {
        InputStatus = (int) __input_from_connector(fd);
        if (InputStatus == 0)
            break;
    };
    return 0;
fail:
    return (int) -1;
}

static void __get_system_event(int fd, int wid)
{
    int msg_code = 0;

// Parameters
    if (fd < 0)
        return;
    //if (wid < 0)
        //return;

// Get one single event.
    if ( rtl_get_event() != TRUE ){
        return;
    }

// Dispatch
    msg_code = (int) (RTLEventBuffer[1] & 0xFFFFFFFF);

// Accepting only these messages.
    switch (msg_code){
    case MSG_CLOSE:
    case MSG_PAINT:
    // ...
        // IN: socket, wid, msgcode, data1, data2
        terminalProcedure ( 
            fd, 
            wid,
            (int) msg_code, 
            (unsigned long) RTLEventBuffer[2],
            (unsigned long) RTLEventBuffer[3] );
        break;
    //#test
    case 44888:
       printf("pubterm: 44888 Received\n");
       break;
    default:
        break;
    };
}

// Get only one event from the window server.
static void __get_ds_event(int fd, int event_wid)
{
    int event_type = -1;

// Parameters
    if (fd < 0)
        return;
    if (event_wid < 0)
        return;

// ------------
    struct gws_event_d lEvent;
    lEvent.used = FALSE;
    lEvent.magic = 0;
    lEvent.type = 0;
    lEvent.long1 = 0;
    lEvent.long2 = 0;

// ------------
    struct gws_event_d *e;
    e = 
        (struct gws_event_d *) gws_get_next_event(
                                   fd, 
                                   event_wid,
                                   (struct gws_event_d *) &lEvent );
// Invalid event
    if ((void *) e == NULL){
        return;
    }
    if (e->magic != 1234){
        return;
    }

// Dispatch event
    event_type = (int) (e->type & 0xFFFFFFFF);
    if (event_type < 0){
        return;
    }
    switch (event_type){
    // ...
    case MSG_PAINT:
    case MSG_CLOSE:
        // socket, wid, msg code, data1, data2
        terminalProcedure( fd, e->window, e->type, e->long1, e->long2 );
        break;
    };
}

static void __initialize_basics(void)
{
    register int i=0;

// Windows
    main_window=0;
    terminal_window=0;
// Cursor
    cursor_x=0;
    cursor_y=0;
    __sequence_status=0;

// CSI - Control Sequence Introducer.
// see: term0.h
    for (i=0; i<CSI_BUFFER_SIZE; i++){
        CSI_BUFFER[i] = 0;
    };
    __csi_buffer_tail=0;
    __csi_buffer_head=0;

    __tmp_x=0;
    __tmp_y=0;

// Limite de tamanho da janela.
    wlMinWindowWidth=0;
    wlMinWindowHeight=0;
    wlMaxWindowWidth=0;
    wlMaxWindowHeight=0;

// ...

}

// terminalTerminal:
// Não emite mensagens.
// #bugbug
// essas configurações são configurações de janela,
// então estão mais para terminal do que para shell.

static void terminalTerminal(void)
{
    //int i=0;
    //int j=0;

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

    //shellPrompt();
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
// Char width and height.
    smCharWidth = gws_get_system_metrics(7);
    smCharHeight = gws_get_system_metrics(8);

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
    wlMinWindowWidth  = (smCharWidth * 80);
    wlMinWindowHeight = (smCharWidth * 25);
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

// Initialization.
// IN: Flags
int terminal_init(unsigned short flags)
{
// Called by main() in main.c

// Socket address
    struct sockaddr_in  addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = PORTS_WS;
    addr_in.sin_addr.s_addr = IP(127,0,0,1);    //ok
    //addr_in.sin_addr.s_addr = IP(127,0,0,9);  //fail

    int client_fd = -1;
    unsigned long w=0;
    unsigned long h=0;

    debug_print ("pubterm:\n");

    __initialize_basics();

// Device info
// #todo: Check for 'zero'.
    w = gws_get_system_metrics(1);
    h = gws_get_system_metrics(2);

// --------------
// Create the socket and save the fd into the terminal structure.
    //client_fd = (int) socket( AF_INET, SOCK_STREAM, 0 );
    client_fd = (int) socket( AF_INET, SOCK_RAW, 0 );
    if (client_fd < 0){
       debug_print("pubterm: on socket()\n");
       printf     ("pubterm: on socket()\n");
       goto fail;
    }
    Terminal.client_fd = (int) client_fd;

    //...

    // pid=2 fd=4
    //printf ("TERMINAL.BIN: pid{%d} fd{%d}\n",
    //    Terminal.pid, Terminal.client_fd );

    //while(1){}

// connect
// Nessa hora colocamos no accept um fd.
// então o servidor escreverá em nosso arquivo.
    //printf ("terminal: Connecting to ws via inet ...\n");

    int con_status = -1;

    while (1){

        con_status = 
            (int) connect(client_fd, (void *) &addr_in, sizeof(addr_in));

        if (con_status < 0){
            debug_print ("terminal: Connection Failed\n");
            printf      ("terminal: Connection Failed\n");
            // Nesse caso a conexao pode ter sido recusada 
            // porque o servidor tem clentes demais.
            // Vamos esperar para sempre?
            if (con_status == ECONNREFUSED){
            }
        }else{
            break; 
        };
    };

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
    unsigned long mwLeft   = ( ( w - mwWidth ) >> 1 );
    unsigned long mwTop    = ( ( h - mwHeight) >> 1 );
    //unsigned long mwLeft   = 0; //( ( w - mwWidth ) >> 1 );
    //unsigned long mwTop    = 0; // ( ( h - mwHeight) >> 1 );

    unsigned int mwColor = COLOR_WINDOW;

// --------------------------------------
// Client area window

// Set default
    unsigned long wLeft   = 2;   // por causa da borda.
    unsigned long wTop    = 34;  // por causa da title bar
    unsigned long wWidth  =  mwWidth  -2 -2;
    unsigned long wHeight =  mwHeight -2 -34;

    unsigned int wColor = (unsigned int) bg_color;

// The surface of this thread.
    setup_surface_retangle ( 
        (unsigned long) mwLeft, 
        (unsigned long) mwTop, 
        (unsigned long) mwWidth, 
        (unsigned long) mwHeight );

// ===================================================
// main window
// style: 
// 0x0001=maximized | 0x0002=minimized | 0x0004=fullscreen

    main_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_OVERLAPPED, 
                  WINDOW_STATUS_ACTIVE,  // status
                  VIEW_NULL,             // view
                  program_name,
                  mwLeft, mwTop, mwWidth, mwHeight,
                  0,
                  WS_TERMINAL,
                  mwColor, 
                  mwColor );

    if (main_window < 0){
        printf("pubterm: on main_window\n");
        goto fail;
    }
    Terminal.main_window_id = main_window;

    //#debug
    gws_refresh_window(client_fd, main_window);

// ===================================================
// Client area window
// Let's get some values.
// Remember: Maybe the window server changed
// the window size and position.
// We need to get these new values.

    // #test: Now it's a global thing.
    //struct gws_window_info_d *wi;
    
    wi = (void*) malloc( sizeof(struct gws_window_info_d) );
    if ((void*) wi == NULL){
        printf("pubterm: wi\n");
        goto fail;
    }
    //IN: fd, wid, window info structure.
    gws_get_window_info(
        client_fd, 
        main_window,   // The app window.
        (struct gws_window_info_d *) wi );

    if (wi->used != TRUE){
        printf("pubterm: wi->used\n");
        goto fail;
    }
    if (wi->magic != 1234){
        printf("pubterm: wi->magic\n");
        goto fail;
    }

// Setting new values for the client window.

// Não pode ser maior que o dispositivo.
    if (wi->cr_left >= w){
        printf("pubterm: wi->cr_left\n");
        goto fail;
    }

// Não pode ser maior que o dispositivo.
    if (wi->cr_top >= h){
        printf("pubterm: wi->cr_top\n");
        goto fail;
    }

// Não pode ser maior que o dispositivo.
    if (wi->cr_width == 0 || wi->cr_width > w){
        printf("pubterm: wi->cr_width\n");
        goto fail;
    }

// Não pode ser maior que o dispositivo.
    if (wi->cr_height == 0 || wi->cr_height > h){
        printf("pubterm: wi->height\n");
        goto fail;
    }

// #danger
// Let's get the values for the client area.
// #
// Quando a janela mãe é overlapped,
// então o deslocamento é relativo à
// área de cliente da janela mãe.
// # 
// Obtivemos as dimensões da área de cliente.

    wLeft   = 0;
    wTop    = 0;
    wWidth  = wi->cr_width;
    wHeight = wi->cr_height;

    terminal_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_SIMPLE, 1, 1, cw_string,
                  wLeft, wTop, wWidth, wHeight,
                  main_window,
                  0x0000,
                  COLOR_BLACK,
                  COLOR_BLACK );

    if (terminal_window < 0){
        printf("pubterm: on terminal_window\n");
        goto fail;
    }
    Terminal.client_window_id = terminal_window;
    //#debug
    gws_refresh_window(client_fd, terminal_window);


    Terminal._mode = 0;

// #bugbug
// Something is wrong here.
// is it in pixel or in chars?
    Terminal.left = 0;
    Terminal.top  = 0;
    //Terminal.left = wLeft;  //0;
    //Terminal.top  = wTop;   //0;

//
// Width and height
//

// In pixels.
    Terminal.width = wWidth;
    Terminal.height = wHeight;

    // In chars.
    Terminal.width_in_chars = 
        (unsigned long)((wWidth  / __CHAR_WIDTH ) & 0xFFFF);
    Terminal.height_in_chars = 
        (unsigned long)((wHeight / __CHAR_HEIGHT) & 0xFFFF);

// Done
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
    terminalTerminal();

// Inicializando prompt[].
    //input('\n');
    input('\0');

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

    // Set focus
    // gws_async_command( client_fd, 9, terminal_window, terminal_window );

    //rtl_focus_on_this_thread();

/*
//================
// cls
     gws_redraw_window(client_fd,Terminal.client_window_id,TRUE);
     //#define SYSTEMCALL_SETCURSOR  34
     sc80 ( 34, 2, 2, 0 );
//=================
*/

    clear_terminal_client_window(client_fd);
    if (Terminal._mode == TERMINAL_MODE_EMBEDDED)
        doPrompt(client_fd);

    // Set active window
    //gws_async_command( client_fd, 15, main_window, main_window );

    gws_set_active( client_fd, main_window );

    //#debug
    gws_refresh_window(client_fd, main_window);

//
// Be nice
// 

    // #test (777)
    //rtl_broken_vessels();

// Input loop!
// local routine.
    int InputStatus = -1;

// -------------------------
// Reading from the child.
// Reading from connector.
    InputStatus = terminal_run(client_fd);
    if (InputStatus < 0){
        goto fail;
    }

// -------------------------
// Embedded shell
    InputStatus = (int) embedded_shell_run(client_fd);
    if (InputStatus < 0){
        goto fail;
    }

done:
    debug_print("pubterm: Done\n"); 
    printf     ("pubterm: Done\n");
    return EXIT_SUCCESS;

fail:
    debug_print("pubterm: Fail\n"); 
    printf     ("pubterm: Fail\n");
    return (int) -1;
}

//
// End
//

