/*
 * File: main.c
 *    Client side application for Gramado Window Server.
 *    Using socket to connect with gws.
 *    AF_GRAMADO family.
 *
 *    #warning:
 *    This application is using floating point.
 *
 * History:
 *     2020 - Created by Fred Nora.
 */

// ##
// This is a test.
// This code is a mess.

// #todo
// This is gonna be a command line interpreter application.
// It will looks like the old gdeshell application.
// #todo: See the 32bit version of gdeshell.
// #todo
// We can have a custom status bar in this client.
// goal: Identity purpose.
// tutorial example taken from. 
// https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
 
/*
    To make a process a TCP server, you need to follow the steps given below −
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
#include <unistd.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>
// libgws - The client-side library.
#include <gws.h>


// network ports.
#define PORTS_WS  4040
#define PORTS_NS  4041
#define PORTS_FS  4042
// ...

#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)


#define MYGREEN  0x0b6623

unsigned long savedW=0;
unsigned long savedH=0;



int game_status;

// area de jogo
int game_window;
int game_width;
int game_height;
int player_x;
int player_y;
int prize_x;
int prize_y;


// barra de status
int status_window;


//
// == Private functions: prototypes ====================
//

static void __test_gr(int fd);

static int __initialization(void);
static void doPrompt(int fd);
static void compareStrings(int fd);
static void testASCIITable(int fd,unsigned long w, unsigned long h);
static void print_ascii_table(int fd);

static int 
cmdlineProcedure ( 
    int fd,
    void *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

static void 
updateStatusBar(
    int fd,
    unsigned long w, 
    unsigned long h, 
    int first_number, 
    int second_number);

//==================================

// initialize via AF_GRAMADO.
// Ainda nao podemos mudar isso para a lib, pois precisamos
// do suporte as rotinas de socket e as definiçoes.
// tem que incluir mais coisa na lib.

static int __initialization(void)
{

//==============================
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = IP(127,0,0,1);
    addr_in.sin_port = PORTS_WS;
//==============================

    int client_fd = -1;

    //gws_debug_print ("-------------------------\n"); 
    //printf          ("-------------------------\n"); 
    gws_debug_print("cmdline: Initializing\n");
    //printf          ("gws.bin: Initializing ...\n");


// Socket:
// Create a socket. 
// AF_GRAMADO = 8000

    // #debug
    //printf ("gws: Creating socket\n");

    //client_fd = (int) socket( AF_INET, SOCK_STREAM, 0 );
    client_fd = (int) socket( AF_INET, SOCK_RAW, 0 );
    if (client_fd < 0)
    {
       gws_debug_print("cmdline: on socket()\n");
       printf         ("cmdline: on socket()\n");
       exit(1);  //#bugbug Cuidado.
    }

// Connect
// Nessa hora colocamos no accept um fd.
// então o servidor escreverá em nosso arquivo.
// Tentando nos conectar ao endereço indicado na estrutura
// Como o domínio é AF_GRAMADO, então o endereço é "w","s".

    //printf ("gws: Trying to connect to the address 'ws' ...\n");      

    while (TRUE){
        if (connect(client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){ 
            debug_print("cmdline: Connection Failed \n"); 
            printf     ("cmdline: Connection Failed \n"); 
        }else{ break; }; 
    };

    return (int) client_fd;
}

static void doPrompt(int fd)
{
    register int i=0;

    if (fd<0){
        return;
    }

// Clean prompt buffer.    
    for (i=0; i<PROMPT_MAX_DEFAULT; i++){
        prompt[i] = (char) '\0';
    };

    prompt[0] = (char) '\0';
    prompt_pos    = 0;
    prompt_status = 0;
    prompt_max    = PROMPT_MAX_DEFAULT;  

//
// Usando o console do kernel
//

    // Prompt
    printf("\n");
    //printf("cmdline: Type something\n");
    printf("$ ");
    fflush(stdout);

    //if (game_window<0){
    //    return;
    //}
    //gws_refresh_window(fd,game_window);
}

static void __test_gr(int fd)
{
    if (fd<0)
        return;

    gr_initialize();

// Plot a 2d point using int.
//see: gr.c
    gws_plot0Int2D( 
        fd,
        10,         //x
        10,         //y
        COLOR_YELLOW,  //color
        0 );        //rop

// Plot a 3d point using int.
    gws_plot0Int3D( 
        fd,
        0,         //x
        0,         //y
        0,
        COLOR_YELLOW,  //color
        0 );        //rop

// Plot a 3d point using int.
    gws_plot0Int3D( 
        fd,
        10,         //x
        10,         //y
        0,
        COLOR_YELLOW,  //color
        0 );        //rop

//Bresenham
    gr_plotLine3d( 
        fd,
        -10, 10, 0,
        10, -10, 0,
        COLOR_GREEN,
        0 );

    plotCircleZ ( 
        fd,
        -10,   //x 
        20,    //y
        10,   //r 
        COLOR_WHITE,  //color 
        0,   // z
        0 ); // rop

// refresh window.
    gws_refresh_window(fd,game_window);
}

static void compareStrings(int fd)
{
    unsigned long message_buffer[8];
    int init_tid=-1;

    if (fd<0)
        return;

    printf("\n");

    if ( strncmp(prompt,"close",5) == 0 )
    {
         printf("~close()\n");
         close(fd);
         //#test
         //gws_draw_char ( 
         //    fd, status_window, 3 * 2, (8), COLOR_BLUE, 127 );
         goto exit_cmp;
    }

    if ( strncmp(prompt,"exit",4) == 0 )
    {
         printf("~exit()\n");
         exit(0);
         goto exit_cmp;
    }

    if ( strncmp(prompt,"test",4) == 0 )
    {
        //get init tid.
        init_tid = (int) sc82(10021,0,0,0);
        message_buffer[0] = 0;
        message_buffer[1] = MSG_COMMAND;  //msg code
        message_buffer[2] = 4002;  //fileman
        message_buffer[3] = 4002;  //fileman
        rtl_post_system_message(init_tid,message_buffer);
        goto exit_cmp;
    }

    if ( strncmp(prompt,"ascii",5) == 0 )
    {
         //not working
         //print_ascii_table(fd);
         goto exit_cmp;
    }

    if ( strncmp(prompt,"reboot",6) == 0 )
    {
         rtl_reboot();
         goto exit_cmp;
    }

    if ( strncmp(prompt,"cls",3) == 0 )
    {
         gws_redraw_window(fd,game_window,TRUE);
         //#define SYSTEMCALL_SETCURSOR  34
         sc80 ( 34, 2, 2, 0 );
         goto exit_cmp;
    }

    if ( strncmp(prompt,"about",5) == 0 )
    {
         printf("cmdline: Command line application\n");
         goto exit_cmp;
    }

    if ( strncmp(prompt,"x",1) == 0 ){
         __test_gr(fd);
         goto exit_cmp;
    }

    printf("Command not found\n");

exit_cmp:
    doPrompt(fd);
}

static void testASCIITable(int fd,unsigned long w, unsigned long h)
{
    int i=0;
    register int j=0;
    int c=0;

    if (fd<0)
        return;

    for (i=0; i<4; i++)
    {
        for (j=0; j<64; j++)
        {
            gws_draw_char ( 
                fd, status_window, 
                (w/64) * j, (i*8), 
                COLOR_YELLOW, c );
            c++;
        };
    };
}

static int 
cmdlineProcedure ( 
    int fd,
    void *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
    int f12Status = -1;

    if (fd<0)
        return -1;

    if(msg<=0){
        return (-1);
    }

    switch (msg){

        case MSG_CLOSE:
            printf("cmdline.bin: Closing...\n");
            exit(0);
            break;
        
        case MSG_COMMAND:
            printf("cmdline.bin: MSG_COMMAND %d \n",long1);
            switch(long1){
            case 4001:  //app1
            printf("cmdline.bin: 4001\n");
            gws_clone_and_execute("browser.bin");  break;
            case 4002:  //app2
            printf("cmdline.bin: 4002\n");
            gws_clone_and_execute("editor.bin");  break;
            case 4003:  //app3
            printf("cmdline.bin: 4003\n");
            gws_clone_and_execute("terminal.bin");  break;
            };
            break;


        // 20 = MSG_KEYDOWN
        case MSG_KEYDOWN:
            switch(long1){
                // keyboard arrows
                case 0x48: 
                    //printf ("UP   \n"); 
                    //player_y = (player_y - 8);
                    //if( player_y <= 0){ player_y = 0; }
                    //gws_draw_char ( fd, game_window, player_x, player_y, COLOR_YELLOW, 'G' );
                    //updateStatusBar(fd,savedW,savedH,0, 'U');
                    goto done; 
                    break;
                case 0x4B: 
                    //printf ("LEFT \n"); 
                    //player_x = (player_x - 8);
                    //if ( player_x <= 0){ player_x = 0;}
                    //gws_draw_char ( fd, game_window, player_x, player_y, COLOR_YELLOW, 'G' );
                    //updateStatusBar(fd,savedW,savedH,0, 'L');
                    goto done; 
                    break;
                case 0x4D: 
                    //printf ("RIGHT\n"); 
                    //player_x = (player_x + 8);
                    //if ( player_x >= game_width){ player_x = (game_width - 8);}
                    //gws_draw_char ( fd, game_window, player_x, player_y, COLOR_YELLOW, 'G' );
                    //updateStatusBar(fd,savedW,savedH,0, 'R');
                    goto done; 
                    break;
                case 0x50: 
                    //printf ("DOWN \n"); 
                    //player_y = (player_y + 8);
                    //if( player_y >= game_height){ player_y = (game_height-8); }
                    //gws_draw_char ( fd, game_window, player_x, player_y, COLOR_YELLOW, 'G' );
                    //updateStatusBar(fd,savedW,savedH,0, 'D');
                    goto done; 
                    break;
                
                case '1':
                    //updateStatusBar(fd,savedW,savedH,'x','x');
                    goto done;
                    break;
 
                case '2': 
                    //game_redraw(fd);
                    goto done;
                    break;
                
                case VK_RETURN:
                    input('\0');
                    compareStrings(fd);
                    return 0;
                    break;
                
                // input
                default:                
                    input(long1);
                    printf("%c",long1);
                    fflush(stdout);
                    break;
            }
            break;

        // 22 = MSG_SYSKEYDOWN
        case MSG_SYSKEYDOWN:
            switch (long1){

                // 1~4
                case VK_F1: gws_clone_and_execute("editor.bin");   break;
                case VK_F2: gws_clone_and_execute("gwm.bin");      break;
                case VK_F3: gws_clone_and_execute("fileman.bin");  break;
                case VK_F4: gws_clone_and_execute("terminal.bin"); break;

                // 4~8
                //case VK_F5: gws_clone_and_execute("browser.bin"); break;
                //case VK_F6: gws_clone_and_execute("browser.bin"); break;
                //case VK_F7: gws_clone_and_execute("browser.bin"); break;
                case VK_F8: 
                    // #test
                    // Setup the flag to show or not the fps window.
                    // Request number 6.
                    //gws_async_command(fd,6,TRUE,0);
                    break;


                // 9~12
                case VK_F9: 
                    //gws_async_command(fd,4,1,0);
                    //gws_async_command(fd,1,0,0);
                    //gws_async_command(fd,4,9,0);  // cat
                    //gws_async_command(fd,4,1,0);
                    //gws_async_command(fd,4,2,0);
                    //gws_clone_and_execute("browser.bin"); 
                    break;
                case VK_F10: 
                    //gws_async_command(fd,4,6,0);
                    //gws_clone_and_execute("browser.bin"); 
                    break;
                case VK_F11: 
                    //gws_async_command(fd,4,7,0);
                    //gws_clone_and_execute("browser.bin"); 
                    break;

                // #test
                case VK_F12: 
                    //gws_async_command(fd,4,9,0);
                    //gws_async_command(fd,4,10,0);   //triangle
                    //gws_async_command(fd,4,11,0); //polygon
                    //gws_async_command(fd,4,12,0); //lines
                    //gws_clone_and_execute("browser.bin"); 
                    //printf("gws.bin: Shutting down ...\n");
                    //f12Status = (int) gws_clone_and_execute("shutdown.bin");
                    //if (f12Status<0){ break; } // fail
                    //exit(0);
                    break;
 
                // ...
            };
            break;
    };

    // ok
    // retorna TRUE quando o diálogo chamado 
    // consumiu o evento passado à ele.

done:
    //check_victory(fd);
    return 0;
    //return (int) gws_default_procedure(fd,0,msg,long1,long2);
}

void init_cursor(int fd)
{
    if (fd<0)
        return;

    player_x = game_width >> 1;
    player_y = game_height >> 1;

    gws_draw_char ( 
        fd, 
        game_window, 
        player_x, 
        player_y, 
        COLOR_RED, 
        'g' );
}

void init_prize(int fd)
{
    if (fd<0)
        return;

    prize_x = (rand() % 50) << 3;
    prize_y = (rand() % 50) << 3;

    gws_draw_char ( 
        fd, 
        game_window, 
        prize_x, 
        prize_y, 
        COLOR_RED, 
        '+' );
}

//
// initialize 'game' support.
//
int gameInitialize(int fd,unsigned long w, unsigned long h)
{
    if (fd<0)
        return -1;

    game_status = FALSE;

    init_cursor(fd);
    init_prize(fd);

    // Test
    // O refresh da tela faz à cada letra, 
    // faz as letras aparecerem lentamente.

    gws_draw_char ( fd, status_window, (w/30)  * 2, (8), COLOR_BLUE, 127 );
    gws_draw_char ( fd, status_window, (w/30)  * 3, (8), COLOR_BLUE, 127 );
    
    gws_draw_char ( fd, status_window, (w/30)  * 2, (8), COLOR_YELLOW, '0' );
    gws_draw_char ( fd, status_window, (w/30)  * 3, (8), COLOR_YELLOW, '0' );


    gws_draw_text(
        fd, status_window,
        (w/30)  * 6, 8, COLOR_YELLOW, "cmdline:");

    /*
    gws_draw_char ( fd, status_window, (w/30)  * 6, (8), COLOR_YELLOW, 'G' );
    gws_draw_char ( fd, status_window, (w/30)  * 7, (8), COLOR_YELLOW, 'R' );
    gws_draw_char ( fd, status_window, (w/30)  * 8, (8), COLOR_YELLOW, 'A' );
    gws_draw_char ( fd, status_window, (w/30)  * 9, (8), COLOR_YELLOW, 'M' );
    gws_draw_char ( fd, status_window, (w/30) * 10, (8), COLOR_YELLOW, 'A' );
    gws_draw_char ( fd, status_window, (w/30) * 11, (8), COLOR_YELLOW, 'D' );
    gws_draw_char ( fd, status_window, (w/30) * 12, (8), COLOR_YELLOW, 'O' );
    */

    //gws_draw_char ( fd, status_window, (w/30)  * 12, (8), COLOR_YELLOW, 127 );
    
    //...

    game_status = TRUE;

    return 0;
}


static void 
updateStatusBar(
    int fd,
    unsigned long w, 
    unsigned long h, 
    int first_number, 
    int second_number)
{
    if (fd<0)
        return;

// first box
    gws_draw_char ( fd, status_window, (w/30)  * 2, (8), COLOR_BLUE, 127 );
// second box
    gws_draw_char ( fd, status_window, (w/30)  * 3, (8), COLOR_BLUE, 127 );
// first number
    gws_draw_char ( fd, status_window, (w/30)  * 2, (8), COLOR_YELLOW, first_number );
// second number
    gws_draw_char ( fd, status_window, (w/30)  * 3, (8), COLOR_YELLOW, second_number );
}


static void print_ascii_table(int fd)
{
    register int i=0;
    int line=0;

    if (fd<0)
        return;

    printf("ascii: :)\n");

    gws_redraw_window(fd,game_window,TRUE);
    //#define SYSTEMCALL_SETCURSOR  34
    sc80 ( 34, 2, 2, 0 );

    for(i=0; i<256; i++)
    {
        gws_draw_char ( 
            fd, 
            game_window, 
            i*8,  //x 
            (8*line),  //y
            COLOR_YELLOW, i );
        
        if (i%10)
            line++;
    };
}

//==========================================
// Main
int main(int argc, char *argv[])
{
// #test
// Testing floating point.

// #config

    int ShowCube = FALSE;
    int launchChild = TRUE;
    // ...
    int client_fd = -1;
    int main_window = -1;

// hello
    //gws_debug_print ("cmdline.bin: Hello world \n");
    //printf          ("cmdline.bin: Hello world \n");

// interrupts
    //gws_debug_print ("cmdline.bin: Enable interrupts \n");
    //printf          ("cmdline.bin: Enable interrupts \n");
    //asm ("int $199 \n");


// interrupts
// Unlock the taskswitching support.
// Unlock the scheduler embedded into the base kernel.
// Only the init process is able to do this.

    //gws_debug_print ("cmdline.bin: Unlock taskswitching and scheduler \n");
    //printf          ("cmdline.bin: Unlock taskswitching and scheduler \n");

    //sc80 (641,0,0,0);
    //sc80 (643,0,0,0);

// Create the rectangle
    //gws_debug_print ("cmdline.bin: Create rectangle \n");
    //printf          ("cmdline.bin: Create rectangle \n");
    //sc80(897,0,0,0);

//
// hang
//
    //while (1){
    //    sc80(897,0,0,0);
    //}

//================================
// Connection.
// Only connect. Nothing more.
// Create socket and call connect().
    client_fd = (int) __initialization();
    if (client_fd < 0)
    {
         gws_debug_print("cmdline: __initialization fail\n");
         printf         ("cmdline: __initialization fail\n");
         exit(1);
    }

// #debug
    //printf(":: Entering CMDLINE.BIN pid{%d} fd{%d}\n",
        //getpid(), client_fd);
    //while(1){}


//========================================

    /*
    char buf[32];
    while (1)
    {
        read (client_fd, buf, 4);
        
        // Not yes
        if( buf[0] != 'y')
        { 
            buf[4] = 0;
            printf ("%s",buf); 
            fflush(stdout);
        }
        
        // yes!
        if( buf[0] == 'y')
        {
            printf ("YES babe!\n");
            break;
            //exit(0);
        }
        
        //gws_draw_char ( client_fd, 
        //    main_window, 
        //    w/3, 8, COLOR_RED, 'C' );
    }
    //================
    */


//========================================

// Device info

    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);

    if ( w == 0 || h == 0 ){
        printf ("cmdline.bin: w h \n");
        exit(1);
    }

    game_width  = (w & 0xFFFF);
    game_height = (h & 0xFFFF);
    savedW      = (w & 0xFFFF);
    savedH      = (h & 0xFFFF);


    //ok
    //rtl_show_heap_info();

    //while(1){
    //gws_async_command(client_fd,3,0,0);  // Hello
    //gws_async_command(client_fd,5,0,0);  // Draw black rectangle.
    //}




// #debug
// ok

    //printf("gws.bin: [1] calling create window\n");

    //while(1){}
    //asm ("int $3");

//
// Window
//


// ===========================
    gws_debug_print ("cmdline.bin: 1 Creating main window \n");
    //printf          ("cmdline.bin: Creating main window \n");

    main_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_SIMPLE, 1, 1, "cmdline",
                  0, 0, w, h-40,
                  0, 0, COLOR_GRAY, COLOR_GRAY);

    if (main_window<0)
    {
        printf ("cmdline.bin: main_window\n");
        exit(1);
    }
    game_window = main_window;
// ========================

    //printf ("CMDLINE.BIN: main_window created\n");
    //while(1){}


    //printf("gws.bin: [2] after create simple green window :)\n");

    //asm ("int $3");
    
    // #debug
    //gws_refresh_window (client_fd, main_window);

    //while(1){}
    //asm ("int $3");


// barra azul no topo.
//===============================
    gws_debug_print ("cmdline.bin:  Creating  window \n");
    //printf          ("cmdline.bin: Creating main window \n");
    int tmp1 = -1;
    tmp1 = (int) gws_create_window (
                     client_fd,
                     WT_SIMPLE, 1, 1, "status",
                     0, 0, w, 24,
                     0, 0, COLOR_BLUE, COLOR_BLUE );
    if (tmp1<0)
    {
        printf ("cmdline.bin: tmp1\n");
        exit(1);
    }
    status_window = tmp1;
//========================

    //printf ("CMDLINE.BIN: status_window created\n");
    //while(1){}
    

    //printf("gws.bin: [2] after create simple gray bar window :)\n");

    // #debug
    //gws_refresh_window (client_fd, tmp1);
    //asm ("int $3");


//===================
// Drawing a char just for fun,not for profit.

    gws_debug_print ("cmdline.bin: 2 Drawing a char \n");
    //printf          ("cmdline.bin: Drawing a char \n");
    if(status_window>0)
    {
        gws_draw_char ( 
            client_fd, 
            status_window, 
            0, 0, COLOR_YELLOW, 'G' );
    }
//====================   


    // #debug
    //gws_refresh_window (client_fd, tmp1);
    //asm ("int $3");

    
    /*
    //
    // == stdin ===================================================
    //
    char evBuf[32];
    int ev_nreads=0;
    unsigned long lMessage[8];
    //struct
    while(TRUE){
        //read from keyboard tty
        ev_nreads = read(0,evBuf,16);
        //if (ev_nreads>0){ printf ("E\n"); }  //funcionou.
        if(ev_nreads>0)
        {
            memcpy( (void*) &lMessage[0],(const void*) &evBuf[0], 16); //16 bytes 
            if( lMessage[1] != 0 )
            {
                 printf( "%c", lMessage[2] ); //long1
                 fflush(stdout);
            }
        }  
        gws_draw_char ( client_fd, main_window, 
        32, 8, COLOR_RED, 'I' );
    };
    // ============================================================
    */
    
    

    // Create a little window in the top left corner.
    //gws_create_window (client_fd,
        //WT_SIMPLE,1,1,"gws-client",
        //2, 2, 8, 8,
        //0, 0, COLOR_RED, COLOR_RED);

    // Draw a char.
    // IN: fd, window id, x, y, color, char.
    //gws_draw_char ( client_fd, 0, 
        //16, 8, COLOR_RED, 'C' );


    /*
    gws_debug_print ("gws.bin: 3 Testing Plot0 4x\n");
    printf          ("gws.bin: 3 Testing Plot0 4x\n");

    //test: plot point;
    //um ponto em cada quadrante.
    gws_plot0 ( client_fd, -50,  50, 0, COLOR_RED );
    gws_plot0 ( client_fd,  50,  50, 0, COLOR_GREEN );
    gws_plot0 ( client_fd,  50, -50, 0, COLOR_BLUE );
    gws_plot0 ( client_fd, -50, -50, 0, COLOR_YELLOW );
    */

//
// == cube ==================================
//

    // #maybe
    // The custon status bar?
    // Maybe the custon status bar can be a window.

    gws_debug_print ("cmdline.bin: 4 Testing Plot cube \n");
    //printf        ("cmdline.bin: 4 Testing Plot cube \n");


// back
    int backLeft   = (-(w/8)); 
    int backRight  =   (w/8);
    int backTop    = (60);
    int backBottom = (10);

// front
    int frontLeft   = (-(w/8)); 
    int frontRight  =   (w/8);
    int frontTop    = -(10);
    int frontBottom = -(60);

// z ?
    int zTest = 0;
    
    int north_color = COLOR_RED;
    int south_color = COLOR_BLUE;


    gws_debug_print("LOOP:\n");
    //printf ("LOOP:\n");


    //
    // Loop
    //

    // #test
    //gws_refresh_window (client_fd, main_window);
        

//
// Game
//

// ??
// What is this?
// Is this a prototype, a test?

    gameInitialize(client_fd,w,h);
    //testASCIITable(client_fd,w,h);


// #test
// Setup the flag to show or not the fps window.
// Request number 6.

    //gws_async_command(client_fd,6,FALSE,0);


//
// Refresh
//

// #test
// nem precisa ja que todas as rotinas que criam as janelas 
// estao mostrando as janelas.

    gws_refresh_window (client_fd, main_window);


//
// Client
//

// #todo
// Podemos nesse momento ler alguma configuração
// que nos diga qual interface devemos inicializar.

    if(launchChild == TRUE)
    {
        gws_redraw_window(client_fd,game_window,0);
        
        // Interface 1: File manager.
        //gws_clone_and_execute("fileman.bin");

        // Interface 1: Test app.
        //gws_clone_and_execute("editor.bin");
    }

//
// Input
//

    
    // Enable input method number 1.
    // Event queue in the current thread.

    //gws_enable_input_method(1);

    //=================================


// =================================
// Focus


// set focus
    rtl_focus_on_this_thread();

// set focus
    gws_async_command(
         client_fd,
         9,             // set focus
         game_window,
         game_window );

//
// Banner
//

// Set cursor position.
    sc80 ( 34, 2, 2, 0 );

    printf ("cmdline.bin: Gramado OS\n");

/*
//#tests
    printf ("#test s null: %s\n",NULL);
    printf ("#test S null: %S\n",NULL);
    printf ("#test u:  %u  \n",12345678);         //ok
    printf ("#test lu: %lu \n",1234567887654321); //ok
    printf ("#test x:  %x  \n",0x1234ABCD);         //
    printf ("#test lx: %lx \n",0x1234ABCDDCBA4321); //
    printf ("#test X:  %X  \n",0x1000ABCD);         //
    printf ("#test lX: %lX \n",0x1000ABCDDCBA0001); //
    // ...
*/





// ===============================
// Testing fpu

    printf("\n");
    printf("Testing math:\n");
    printf("\n");

// -------------------
// float. ok on qemu.
    float float_var = 1.5;
    unsigned int float_res = (unsigned int) (float_var + float_var);
    printf("float_res={%d}\n",(unsigned int)float_res);

// -------------------
// double. ok on qemu.
    double double_var = 2.5000;
    unsigned int double_res = (unsigned int) (double_var + double_var);
    printf("double_res={%d}\n",(unsigned int)double_res);

// -------------------
// #test
// See: math.c in rtl/
    //unsigned int square_root = (unsigned int) sqrt(81);
    //printf("sqrt of 81 = {%d}\n",(unsigned int)square_root);
    unsigned long square_root = (unsigned long) sqrt(81);
    printf("sqrt of 81 = {%d}\n",(unsigned long)square_root);

// -------------------
    // 9 ao cubo.
    //long r9 = (long) power0(9,3);
    //long r9 = (long) power1(9,3);
    //long r9 = (long) power2(9,3);
    //long r9 = (long) power3(9,3);
    long r9 = (long) power4(9,3);
    printf("9^3 = {%d}\n", (long) r9);

// ===============================

//#breakpoint
    //printf("cmdline.bin: Breakpoint :)\n");
    //while(1){}


// #test
// Getting 2mb shared memory surface.
// ring3.

    void *ptr;
    ptr = (void*) rtl_shm_get_2mb_surface();
    if( (void*) ptr != NULL )
        printf("surface address: %x\n",ptr);


// ===============================



// Show prompt.
    doPrompt(client_fd);

    // Podemos chamar mais de um diálogo
    // Retorna TRUE quando o diálogo chamado 
    // consumiu o evento passado à ele.
    // Nesse caso chamados 'continue;'
    // Caso contrário podemos chamar outros diálogos.

    while (1){
        if ( rtl_get_event() == TRUE )
        {
            //if( RTLEventBuffer[1] == MSG_QUIT ){ break; }

            cmdlineProcedure ( 
                client_fd,
                (void*) RTLEventBuffer[0], 
                RTLEventBuffer[1], 
                RTLEventBuffer[2], 
                RTLEventBuffer[3] );
        }
    };
//=================================

    // Isso eh estranho ... um cliente remoto nao deve poder fazer isso.
    //gws_debug_print ("gws: Sending command to close the server. \n");
    //gws_async_command(client_fd,1,0,0);
    //exit(0);

    // Asking to server to send me an notification
    // telling me to close myself
    
    //gws_debug_print ("gws: Pinging\n");
    //gws_async_command(client_fd,2,0,0);


    while(1){}
    // ...

    /*
    unsigned long event_buffer[8];
    // Event loop
    while (TRUE)
    {
        // Get next event.
        read ( client_fd, event_buffer, sizeof(event_buffer) );
        
        //event: Close my self
        //if ( event_buffer[1] == 12344321 )
        //{
        //    gws_debug_print ("gws: [EVENT] We got the event 12344321\n \n");
        //    break;
        //}
        
        if ( event_buffer[0] == 'p' &&
             event_buffer[1] == 'o' &&
             event_buffer[2] == 'n' &&
             event_buffer[3] == 'g' )
        {
            printf("PONG\n");
            gws_async_command(client_fd,1,0,0);
        }
    };
    */
 
    // exit
    gws_debug_print ("cmdline: bye :) \n");
    exit(0);
    
    return 0;
}

//
// End
//

