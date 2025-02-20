/*
 * File: main.c
 *    Client side application for Gramado Window Server.
 *    Using socket to connect with gws.
 *    AF_GRAMADO family.
 * History:
 *     2020 - Created by Fred Nora.
 */

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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>
// libgws - The client-side library.
#include <gws.h>


#define MYGREEN  0x0b6623


unsigned long savedW=0;
unsigned long savedH=0;

int game_status=0;

// area de jogo
int game_window=0;
int game_width=0;
int game_height=0;
int player_x=0;
int player_y=0;
int prize_x=0;
int prize_y=0;


// barra de status
int status_window;


    
//
// == prototypes =============
//

int gws(void);
int gameInitialize(int fd,unsigned long w, unsigned long h);

// ----------------------------

void init_cursor(int fd)
{
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

void gameTestASCIITable(int fd,unsigned long w, unsigned long h)
{
    int i=0;
    int j=0;
    int c=0;
    
    for(i=0; i<4; i++)
    {
        for(j=0; j<64; j++)
        {
            gws_draw_char ( 
                fd, status_window, 
                (w/64) * j, (i*8), 
                COLOR_YELLOW, c );
            c++;
        };
    };
}


void you_win(int fd)
{
    printf("You win!\n");
    gws_clone_and_execute("fileman.bin");
    game_status = FALSE;
}


void check_victory(int fd)
{
    int p1 = prize_x - 10;
    int p2 = prize_x + 10;

    int p3 = prize_y - 10;
    int p4 = prize_y + 10;

    if (game_status != TRUE)
        return;

    if ( player_x > p1 && player_x < p2 && 
         player_y > p3 && player_y < p4 )
    {
        you_win(fd);
    }
}

void game_redraw(int fd)
{
    gws_redraw_window(fd, game_window, TRUE); 
    gws_redraw_window(fd, status_window, TRUE);
    gws_refresh_window(fd, game_window);
    init_cursor(fd);
    init_prize(fd);
}


void game_reinitialize(int fd)
{
    int status=0;
    status=gameInitialize(fd,game_width,game_height);
    if(status<0)
        printf("game_reinitialize: fail\n");
}


//
// initialize 'game' support.
//

int gameInitialize(int fd,unsigned long w, unsigned long h)
{

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
        (w/30)  * 6, 8, COLOR_YELLOW, "GRAMADO");

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


void 
updateStatusBar(
    int fd,
    unsigned long w, 
    unsigned long h, 
    int first_number, 
    int second_number)
{

// first box
    gws_draw_char ( fd, status_window, (w/30)  * 2, (8), COLOR_BLUE, 127 );
// second box
    gws_draw_char ( fd, status_window, (w/30)  * 3, (8), COLOR_BLUE, 127 );
// first number
    gws_draw_char ( fd, status_window, (w/30)  * 2, (8), COLOR_YELLOW, first_number );
// second number
    gws_draw_char ( fd, status_window, (w/30)  * 3, (8), COLOR_YELLOW, second_number );
}


// initialize via AF_GRAMADO.
// Ainda nao podemos mudar isso para a lib, pois precisamos
// do suporte as rotinas de socket e as definiçoes.
// tem que incluir mais coisa na lib.

int gws(void)
{

    // Vamos nos concetar com o processo identificado 
    // com o nome 'ws'
    // The port name is 'port:/ws'

    //==============================
    struct sockaddr addr; 
    int addrlen;
    
    addr.sa_family = AF_GRAMADO; 
    addr.sa_data[0] = 'w';
    addr.sa_data[1] = 's';  

    addrlen = sizeof(addr);
    //==============================
    
    int client_fd = -1;
    
    gws_debug_print ("-------------------------\n"); 
    //printf          ("-------------------------\n"); 
    gws_debug_print ("gws.bin: Initializing ...\n");
    //printf          ("gws.bin: Initializing ...\n");


    //
    // Socket
    // 

    // #debug
    //printf ("gws: Creating socket\n");

    // Create a socket. 
    // AF_GRAMADO = 8000
    client_fd = socket ( AF_GRAMADO, SOCK_STREAM, 0 );
    
    if ( client_fd < 0 ){
       gws_debug_print ("gws: [FAIL] Couldn't create socket\n");
       printf          ("gws: [FAIL] Couldn't create socket\n");
       exit(1);  //#bugbug Cuidado.
    }

    //
    // Connect
    //

    // Nessa hora colocamos no accept um fd.
    // então o servidor escreverá em nosso arquivo.
    // Tentando nos conectar ao endereço indicado na estrutura
    // Como o domínio é AF_GRAMADO, então o endereço é "w","s".

    //printf ("gws: Trying to connect to the address 'ws' ...\n");      

    while (TRUE){
        if ( connect (client_fd, (struct sockaddr *) &addr, addrlen ) < 0 )
        { 
            gws_debug_print ("gws: Connection Failed\n");
            //printf          ("gws: Connection Failed \n"); 
            //exit(1);
        }else{ break; };
    };

    return (int) client_fd;
}


// local
int 
gwsProcedure ( 
    int fd,
    void *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{

    int f12Status = -1;


    if(msg<=0){
        return (-1);
    }

    switch (msg){

        case MSG_CLOSE:
            printf("gws.bin: Closing...\n");
            exit(0);
            break;
        
        case MSG_COMMAND:
            printf("gws.bin: MSG_COMMAND %d \n",long1);
            switch(long1){
            case 4001:  //app1
            printf("gws.bin: 4001\n");
            gws_clone_and_execute("browser.bin");  break;
            case 4002:  //app2
            printf("gws.bin: 4002\n");
            gws_clone_and_execute("editor.bin");  break;
            case 4003:  //app3
            printf("gws.bin: 4003\n");
            gws_clone_and_execute("terminal.bin");  break;
            };
            break;


        // 20 = MSG_KEYDOWN
        case MSG_KEYDOWN:
            switch(long1){
                // keyboard arrows
                case 0x48: 
                    //printf ("UP   \n"); 
                    player_y = (player_y - 8);
                    if( player_y <= 0){ player_y = 0; }
                    gws_draw_char ( fd, game_window, player_x, player_y, COLOR_YELLOW, 'G' );
                    updateStatusBar(fd,savedW,savedH,0, 'U');
                    goto done; 
                    break;
                case 0x4B: 
                    //printf ("LEFT \n"); 
                    player_x = (player_x - 8);
                    if ( player_x <= 0){ player_x = 0;}
                    gws_draw_char ( fd, game_window, player_x, player_y, COLOR_YELLOW, 'G' );
                    updateStatusBar(fd,savedW,savedH,0, 'L');
                    goto done; 
                    break;
                case 0x4D: 
                    //printf ("RIGHT\n"); 
                    player_x = (player_x + 8);
                    if ( player_x >= game_width){ player_x = (game_width - 8);}
                    gws_draw_char ( fd, game_window, player_x, player_y, COLOR_YELLOW, 'G' );
                    updateStatusBar(fd,savedW,savedH,0, 'R');
                    goto done; 
                    break;
                case 0x50: 
                    //printf ("DOWN \n"); 
                    player_y = (player_y + 8);
                    if( player_y >= game_height){ player_y = (game_height-8); }
                    gws_draw_char ( fd, game_window, player_x, player_y, COLOR_YELLOW, 'G' );
                    updateStatusBar(fd,savedW,savedH,0, 'D');
                    goto done; 
                    break;
                
                case '1':
                    updateStatusBar(fd,savedW,savedH,'x','x');
                    goto done;
                    break;
 
                case '2': 
                    game_redraw(fd);
                    goto done;
                    break;
                
                default:
                    printf("%c",long1); fflush(stdout);
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
                case VK_F5: gws_clone_and_execute("browser.bin"); break;
                case VK_F6: gws_clone_and_execute("browser.bin"); break;
                case VK_F7: gws_clone_and_execute("browser.bin"); break;
                case VK_F8: 
                    // #test
                    // Setup the flag to show or not the fps window.
                    // Request number 6.
                    gws_async_command(fd,6,TRUE,0);
                    break;


                // 9~12
                case VK_F9: 
                    gws_async_command(fd,4,1,0);
                    //gws_async_command(fd,1,0,0);
                    //gws_async_command(fd,4,9,0);  // cat
                    //gws_async_command(fd,4,1,0);
                    //gws_async_command(fd,4,2,0);
                    //gws_clone_and_execute("browser.bin"); 
                    break;
                case VK_F10: 
                    gws_async_command(fd,4,6,0);
                    //gws_clone_and_execute("browser.bin"); 
                    break;
                case VK_F11: 
                    gws_async_command(fd,4,7,0);
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

    check_victory(fd);

    return 0;
    //return (int) gws_default_procedure(fd,0,msg,long1,long2);
}


//==========================================
// Main

int main ( int argc, char *argv[] )
{

// #config

    int ShowCube = FALSE;
    int launchChild = TRUE;
    // ...


    int client_fd = -1;
    int main_window = -1;
    

// hello
    gws_debug_print ("setup.bin: Hello world \n");
    printf          ("setup.bin: Hello world \n");


// interrupts
    gws_debug_print ("setup.bin: Enable interrupts \n");
    printf          ("setup.bin: Enable interrupts \n");

    asm ("int $199 \n");


// interrupts
// Unlock the taskswitching support.
// Unlock the scheduler embedded into the base kernel.
// Only the init process is able to do this.

    gws_debug_print ("setup.bin: Unlock taskswitching and scheduler \n");
    printf          ("setup.bin: Unlock taskswitching and scheduler \n");

    sc80 (641,0,0,0);
    sc80 (643,0,0,0);

// Create the rectangle
    gws_debug_print ("setup.bin: Create rectangle \n");
    printf          ("setup.bin: Create rectangle \n");

    sc80(897,0,0,0);

//
// hang
//
    //while (1){
    //    sc80(897,0,0,0);
    //}

//================================
   
// connection.
// Only connect. Nothing more.

    client_fd = gws();

    if ( client_fd < 0 ){
         gws_debug_print ("setup.bin: gws initialization fail \n");
         printf          ("setup.bin: gws initialization fail \n");
         exit(1);
    }


//========================================
    
    // Waiting ...
    // Wait for the moment where the server says: 'yes'

    gws_debug_print ("setup.bin:  \n");
    //         printf ("setup.bin:  \n");


    /*
    rtl_set_file_sync( client_fd, SYNC_REQUEST_SET_ACTION, ACTION_ERROR );
    int value = rtl_get_file_sync( client_fd, SYNC_REQUEST_GET_ACTION );
    
    printf ("VALUE {%d} \n", value);
    
    if( value == ACTION_ERROR )
        printf("OK\n");

    rtl_set_file_sync( client_fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    //close(client_fd);
    //exit(0);
    //while(1){}
    */ 
        
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
        printf ("setup.bin: w h \n");
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

    //===============================
    gws_debug_print ("setup.bin: 1 Creating main window \n");
    //printf          ("setup.bin: Creating main window \n");

    main_window = gws_create_window (client_fd,
                      WT_SIMPLE, 1, 1, "setup-main",
                      0, 0, w, h-40,
                      0, 0, MYGREEN, MYGREEN);

    if (main_window<0){
        printf ("setup.bin: main_window\n");
        exit(1);
    }
    game_window = main_window;
    //========================

    //printf("gws.bin: [2] after create simple green window :)\n");

    //asm ("int $3");
    
    // #debug
    //gws_refresh_window (client_fd, main_window);

    //while(1){}
    //asm ("int $3");


    //===============================
    gws_debug_print ("setup.bin:  Creating  window \n");
    //printf          ("gws.bin: Creating main window \n");
    int tmp1;
    tmp1 = gws_create_window (client_fd,
                      WT_SIMPLE, 1, 1, "setup-status",
                      0, 0, w, 40,
                      0, 0, COLOR_GRAY, COLOR_GRAY);

    if (tmp1<0){
        printf ("setup.bin: tmp1\n");
        exit(1);
    }
    status_window = tmp1;
    //========================


    //printf("gws.bin: [2] after create simple gray bar window :)\n");

    // #debug
    //gws_refresh_window (client_fd, tmp1);
    //asm ("int $3");

    // Drawing a char just for fun,not for profit.

    //===================
    gws_debug_print ("setup.bin: 2 Drawing a char \n");
    //printf          ("setup.bin: Drawing a char \n");
    if(main_window>0){
        gws_draw_char ( 
            client_fd, 
            main_window, 0, 0, COLOR_YELLOW, 'G' );
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

    gws_debug_print ("setup.bin: 4 Testing Plot cube \n");
    //printf        ("setup.bin: 4 Testing Plot cube \n");


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

    struct gr_cube_d  *cube;
    cube = (void *) malloc( sizeof( struct gr_cube_d ) );

    //int Count=0;
    //for (Count=0; Count<100; Count = Count+10 ){
    //zTest = zTest + 10;
    if ( (void*) cube != NULL )
    {
        // =========
        // south (frente) 
        cube->p[0].x = frontLeft; 
        cube->p[0].y = frontTop; 
        cube->p[0].z = zTest;
        cube->p[0].color = south_color;
        
        cube->p[1].x = frontRight; 
        cube->p[1].y = frontTop;
        cube->p[1].z = zTest;
        cube->p[1].color = south_color;
        
        cube->p[2].x = frontRight;
        cube->p[2].y = frontBottom;
        cube->p[2].z = zTest;
        cube->p[2].color = south_color;
        
        cube->p[3].x = frontLeft;
        cube->p[3].y = frontBottom; 
        cube->p[3].z = zTest;
        cube->p[3].color = south_color;

        // ===========
        // north (trás)
        cube->p[4].x = backLeft;
        cube->p[4].y = backTop;
        cube->p[4].z = zTest;
        cube->p[4].color = north_color;
        
        cube->p[5].x = backRight; //140;
        cube->p[5].y = backTop; //-90;
        cube->p[5].z = zTest;
        cube->p[5].color = north_color;
        
        cube->p[6].x = backRight;
        cube->p[6].y = backBottom;
        cube->p[6].z = zTest;
        cube->p[6].color = north_color;
        
        cube->p[7].x = backLeft;
        cube->p[7].y = backBottom;
        cube->p[7].z = zTest;
        cube->p[7].color = north_color;
 
        // plot cube 
        if (ShowCube==TRUE){
            //gws_plotcube ( client_fd, (struct gr_cube_d *) cube );
        }
    }
    //}; //for


    //
    // == rectangle ==================================
    //

    /*
    gws_debug_print ("gws.bin: 5 Testing Plot rect \n");
    printf          ("gws.bin: 5 Testing Plot rect \n");

    
    struct gr_rectangle_d *rect;
    rect = (void *) malloc( sizeof( struct gr_rectangle_d ) );
    if ( (void*) rect != NULL )
    {

        rect->p[0].x = -20;
        rect->p[0].y = 20;
        rect->p[0].z = 0;
        rect->p[0].color = COLOR_BLUE;
        
        rect->p[1].x = 10;
        rect->p[1].y = 20;
        rect->p[1].z = 0;
        rect->p[1].color = COLOR_BLUE;
        
        rect->p[2].x = 10;
        rect->p[2].y = 0;
        rect->p[2].z = 0;
        rect->p[2].color = COLOR_BLUE;
        
        rect->p[3].x = -20;
        rect->p[3].y = 0;
        rect->p[3].z = 0;
        rect->p[3].color = COLOR_BLUE;
        
        
        // plot rectangle 
        gws_plotrectangle ( client_fd, (struct gr_rectangle_d *) rect );
    }
    */
   
    gws_debug_print("LOOP:\n");
    //printf ("LOOP:\n");

    // #debug
    //while (1){

    if( main_window > 0 ){
        gws_draw_char ( 
            client_fd, 
            main_window, 8, 8, COLOR_YELLOW, 'x' );
    }

        // ...

        // Saving the fd into the libgws.
        // it's gonna be used by the payment routine.
        // See: connect.c
        //gws_set_client_fd(client_fd);
        //gws_payment();
    //}

    // #test: Chamando um demo.
    // gws_async_command(client_fd,4,9,0); //cat

/*
//============================================================

    int i=0;
    int x=0;  int y=0;  int z=0;
    int offset=0;
    int T=0;

    //no translation
    offset=0;
    for(i=0; i<50; i++){
    gws_plot0(client_fd, x+offset,        0,        0, COLOR_RED   );
    gws_plot0(client_fd,        0, y+offset,        0, COLOR_GREEN );
    gws_plot0(client_fd,        0,        0, z+offset, COLOR_BLUE  );
    offset++;
    };

    // x translation
    T=8;
    offset=0;
    for(i=0; i<50; i++){
    gws_plot0(client_fd, x+offset+T,        0,        0, COLOR_RED   );
    gws_plot0(client_fd,        0+T, y+offset,        0, COLOR_GREEN );
    gws_plot0(client_fd,        0+T,        0, z+offset, COLOR_BLUE  );
    offset++;
    };

    
    // y translation
    T=8;
    offset=0;
    for(i=0; i<50; i++){
    gws_plot0(client_fd, x+offset,        0+T,        0, COLOR_RED   );
    gws_plot0(client_fd,        0, y+offset+T,        0, COLOR_GREEN );
    gws_plot0(client_fd,        0,        0+T, z+offset, COLOR_BLUE  );
    offset++;
    };
    
    // z translation
    T=8;
    offset=0;
    for(i=0; i<50; i++){
    gws_plot0(client_fd, x+offset,        0,        0+T, COLOR_RED   );
    gws_plot0(client_fd,        0, y+offset,        0+T, COLOR_GREEN );
    gws_plot0(client_fd,        0,        0, z+offset+T, COLOR_BLUE  );
    offset++;
    };

//============================================================
    */

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
    //gameTestASCIITable(client_fd,w,h);


// #test
// Setup the flag to show or not the fps window.
// Request number 6.

    gws_async_command(client_fd,6,FALSE,0);


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

    if(launchChild == TRUE){

// Interface 1: File manager.
    gws_clone_and_execute("fileman.bin");

// Interface 1: Test app.
    //gws_clone_and_execute("editor.bin");

    }

//
// Input
//


    //=================================
    
    // get current thread
    // set foreground thread.
    // #todo: We need to create a rtl function for this.
    // See: rtl.c
    //int cThread = (int) pthread_self();
    //sc82 (10011,cThread,cThread,cThread);

    // rtl_focus_on_this_thread();
    
    // Enable input method number 1.
    // Event queue in the current thread.

    //gws_enable_input_method(1);

    //=================================

    // Podemos chamar mais de um diálogo
    // Retorna TRUE quando o diálogo chamado 
    // consumiu o evento passado à ele.
    // Nesse caso chamados 'continue;'
    // Caso contrário podemos chamar outros diálogos.

    while (1){
        if ( rtl_get_event() == TRUE )
        {
            //if( RTLEventBuffer[1] == MSG_QUIT ){ break; }

            gwsProcedure ( 
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
    gws_debug_print ("setup: bye :) \n");
    exit(0);
    
    return 0;
}



//
// End.
//




