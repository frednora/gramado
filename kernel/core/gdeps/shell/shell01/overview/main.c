// main.c
// Main file for taskbar.bin application. (Overview/Explorer)
// Created by Fred Nora.

/*
 * File: main.c
 *    Client side application for Gramado Window Server.
 *    Using socket to connect with gws.
 *    AF_GRAMADO family.
 *    #warning:
 *    This application is using floating point.
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

#include "taskbar.h"



// network ports.
#define PORTS_WS  4040
#define PORTS_NS  4041
#define PORTS_FS  4042
// ...

#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)



//
// Colors
//

#define HONEY_COLOR_TASKBAR  0x00C3C3C3 
#define MYGREEN  0x0b6623

// #bugbug
// The display server doesn't know the value we're setting here.
// So We need to synchronize the value here and the value 
// used by the server. 
// #todo:
// Maybe we can make a request to get this value,
// and use it to make our window.
// And maybe we can make a request to setup the 
// taskbar values in the display server.
// Remember: 
// The display server also controls the size of the 'working area'.
// This area represents the screen size less the taskbar size.
// >> For now 28 is smaller then the value in the server.

#define TASKBAR_HEIGHT  28
//#define TASKBAR_HEIGHT  100

// Device Info.
struct device_info_d 
{
    unsigned long width;
    unsigned long height;
};
struct device_info_d  DeviceInfo;

// Desktop info
struct desktop_info_d
{
    int initialized;
    int inUse;
    int style;
    // ...
};
struct desktop_info_d  DesktopInfo;

// #todo
// Taskbar info?
struct taskbar_info_d
{
    int initialized;
    int inUse;
    int style;
    // ...
};
struct taskbar_info_d  TaskbarInfo;

// This application is the OS Shell
struct os_shell_info_d 
{
    int initialized;
    // ...
    //struct desktop_info_d  DesktopInfo;
    //struct taskbar_info_d  TaskbarInfo;

};
struct os_shell_info_d  OSShellInfo;


// Clients
// Creating 32 clients.
// It represents the order in the taskbar.
// If an entry is not used, it will not be
// shown in the taskbar.
struct tb_client_d clientList[CLIENT_COUNT_MAX];

//
// Windows
//

static int main_window = -1;
static int desktop_window = -1;

// Main buttons
// (Navigation bar)
// #todo: Maybe we can create an structure and 
// allow customization.

struct navigation_info_d
{
    int initialized;

// Buttons
    int button00_window;
    int button01_window;
    int button02_window;
// Separator
    int useSeparator;

// Taskbar icons
// #todo
// Create and initialize a structure for this.
    //int icon_counter = 0;
    //int icon_left_limit = 80; // Limit in pixels.
    // Icon list
    //int iconList[32];
};
struct navigation_info_d  NavigationInfo;

//
// Taskbar icons.
// Not the start menu.
//

// #todo
// Create and initialize a structure for this.
static int icon_counter = 0;
static int icon_left_limit = 80; // Limit in pixels.
// Icon list
int iconList[32];

//
// Strings
//

const char *program_name = "Taskbar";

//const char *start_menu_button_label = "Gramado";
//const char *start_menu_button_label = "Start";
//const char *start_menu_button_label = "|||";

// Main labels
const char *buttom00_label = "|||";
const char *buttom01_label = "0";
const char *buttom02_label = "<";

const char *app1_name = "#terminal.bin";
const char *app2_name = "#editor.bin";

//static const char *cmdline1 = "gramland -1 -2 -3 --tb";

//
// == Private functions: prototypes ====================
//

static void __test_gr(int fd);

static int __initialization(void);
static void doPrompt(int fd);
static void compareStrings(int fd);
static void testASCIITable(int fd,unsigned long w, unsigned long h);
static void print_ascii_table(int fd);

static void do_launch_app(int app_number);

static int 
tbProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 );

static void 
updateStatusBar(
    int fd,
    unsigned long w, 
    unsigned long h, 
    int first_number, 
    int second_number);


static void __initialize_client_list(void);
void pump(int fd, int wid);

static int 
create_bar_icon(
    int fd,
    int parent,
    int icon_id,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    char *label );

static int draw_separator(int fd);

static int create_icons_on_desktop(int fd);
static int create_desktop_area(int fd);

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
    gws_debug_print("taskbar.bin: Initializing\n");
    //printf       ("taskbar.bin: Initializing ...\n");

// Socket:
// Create a socket. 
// AF_GRAMADO = 8000

    // #debug
    //printf ("gws: Creating socket\n");

    client_fd = 
        (int) socket( 
            AF_INET,   // Remote or local connections
            SOCK_RAW,  // Type
            0 );       // Protocol

    if (client_fd < 0)
    {
       gws_debug_print("taskbar.bin: on socket()\n");
       printf         ("taskbar.bin: on socket()\n");
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
            debug_print("taskbar.bin: Connection Failed\n"); 
            printf     ("taskbar.bin: Connection Failed\n"); 
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
    //printf("taskbar.bin: Type something\n");
    printf("$ ");
    fflush(stdout);

    //if (main_window<0){
    //    return;
    //}
    //gws_refresh_window(fd,main_window);
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
    gws_refresh_window(fd,main_window);
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
        gws_redraw_window(fd,main_window,TRUE);
        //#define SYSTEMCALL_SETCURSOR  34
        sc80 ( 34, 2, 2, 0 );
        goto exit_cmp;
    }

    if ( strncmp(prompt,"about",5) == 0 )
    {
        printf("taskbar.bin: Command line application\n");
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
            //gws_draw_char ( 
            //    fd, status_window, 
            //    (w/64) * j, (i*8), 
            //    COLOR_YELLOW, c );

            c++;
        };
    };
}

static void do_launch_app(int app_number)
{
    int ret_val=-1;

    char filename[16];
    size_t StringSize=0;
    memset(filename,0,16);

    //do_clear_console();
    //printf ("Launching GUI\n");

// #bugbug
// Sending cmdline via stdin
    //rewind(stdin);
    //write( fileno(stdin), cmdline1, strlen(cmdline1) );

// Launch new process.

    if (app_number == 1){
        sprintf(filename,app1_name);
        StringSize = (size_t) strlen(app1_name);
    }else if (app_number == 2){
        sprintf(filename,app2_name);
        StringSize = (size_t) strlen(app2_name);
    }else{
        return;
    }

    filename[StringSize] = 0;
    ret_val = (int) rtl_clone_and_execute(filename);
    //ret_val = (int) rtl_clone_and_execute(app1_name);
    if (ret_val <= 0){
        printf("Couldn't clone\n");
        return;
    }

// Sleep (Good!)
    //sc82( 266, 8000, 8000, 8000 );

    //printf("pid=%d\n",ret_val);

// Quit the command line.
    //isTimeToQuit = TRUE;
}

static void print_ascii_table(int fd)
{
    register int i=0;
    int line=0;

    if (fd<0)
        return;

    printf("ascii: :)\n");

    gws_redraw_window(fd,main_window,TRUE);
    //#define SYSTEMCALL_SETCURSOR  34
    sc80 ( 34, 2, 2, 0 );

    for(i=0; i<256; i++)
    {
        gws_draw_char ( 
            fd, 
            main_window, 
            i*8,  //x 
            (8*line),  //y
            COLOR_YELLOW, i );
        
        if (i%10)
            line++;
    };
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
    //gws_draw_char ( fd, status_window, (w/30)  * 2, (8), COLOR_BLUE, 127 );
// second box
    //gws_draw_char ( fd, status_window, (w/30)  * 3, (8), COLOR_BLUE, 127 );
// first number
    //gws_draw_char ( fd, status_window, (w/30)  * 2, (8), COLOR_YELLOW, first_number );
// second number
   // gws_draw_char ( fd, status_window, (w/30)  * 3, (8), COLOR_YELLOW, second_number );
}

// Process event that came from the server.
static int 
tbProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int f12Status = -1;
    int tmpNewWID = -1;

// Parameters:
    if (fd < 0){
        goto fail;
    }
    if (event_type <= 0){
        goto fail;
    }

// Process the event.
    switch (event_type){

        //#todo
        // Update the bar and the list of clients.
        case MSG_PAINT:
            //printf("task.bin: MSG_PAINT\n");
            // #todo
            // We need to update all the clients
            // Create update_clients()
            gws_redraw_window(fd, main_window, TRUE);
            gws_redraw_window(fd, NavigationInfo.button00_window, TRUE);
            gws_redraw_window(fd, NavigationInfo.button01_window, TRUE);
            gws_redraw_window(fd, NavigationInfo.button02_window, TRUE);
            draw_separator(fd);
            //#test
            //#todo
            //gws_redraw_window(fd, iconList[0], TRUE);
            //gws_redraw_window(fd, iconList[1], TRUE);
            //gws_redraw_window(fd, iconList[2], TRUE);
            //gws_redraw_window(fd, iconList[3], TRUE);

            // #test (good)
            // Async with 4 data
            // Redraw and show.
            //gws_async_command2( fd, 2000, 0,
                //main_window,
                //NavigationInfo.button00_window,
                //NavigationInfo.button01_window,
                //NavigationInfo.button02_window );
            //draw_separator(fd);

            break;

        // One button was clicked
        case GWS_MouseClicked:
            //#debug
            //printf("taskbar: GWS_MouseClicked\n");

            // #debug 
            //if (long1 == main_window)
                //printf("taskbar: GWS_MouseClicked\n");

            //if (event_window == NavigationInfo.button00_window)
            // #ps: Probably the event window is the main window.

            // # Display apps. (recent apps?), maximize.
            if (long1 == NavigationInfo.button00_window)
            {
                gws_async_command(fd,30,0,0);  // TILE (Update desktop)
                //gws_async_command(fd,15,0,0); //SET ACTIVE WINDOW BY WID
                //gws_async_command(fd,1014,0,0); //maximize active window

                /*
                // #todo: It is working. 
                // We got to initialize the variables first.
            
                //#test: Testing a new worker.
                tmpNewWID = (int) create_bar_icon(
                    fd, 
                    main_window,
                    0,
                    (icon_left_limit + icon_counter),  // Left
                    2,  // Top
                    32, // Width
                    28, // Height
                    "NEW" );

                if (tmpNewWID < 0)
                    goto fail;
                gws_refresh_window(fd,tmpNewWID);
                if (icon_counter<0)
                    goto fail;
                if (icon_counter>=32)
                    goto fail;
                iconList[icon_counter] = (int) tmpNewWID;
                icon_counter++;

                printf("done %d\n",tmpNewWID);
                */
            }
            // #todo: Minimize apps and Show desktop.
            if (long1 == NavigationInfo.button01_window)
                gws_async_command(fd,1011,0,0);
            // #todo: Back
            if (long1 == NavigationInfo.button02_window)
                gws_async_command(fd,30,0,0);
            break;

        // Add new client. Given the wid.
        // The server created a client.
        case 99440:
            printf("taskbar: [99440]\n");
            break;

        // Remove client. Given the wid.
        // The server removed a client.
        case 99441:
            printf("taskbar: [99441]\n");
            break;
        
        // Update client info.
        // The server send data about the client.
        case 99443:
            printf("taskbar: [99443]\n");
            break;

        // #test:
        // ds sent us a message to create an iconic window for an app.
        case 99500:
            tmpNewWID = (int) create_bar_icon(
                fd, 
                main_window,
                2,  // Icon ID
                8,8,28,28,
                "NEW" );
            if (tmpNewWID < 0)
                goto fail;
            gws_refresh_window(fd,tmpNewWID);
            break;

        case MSG_CLOSE:
            printf("taskbar: Closing...\n");
            exit(0);
            break;
        
        case MSG_COMMAND:
            /*
            printf("taskbar.bin: MSG_COMMAND %d \n",long1);
            switch(long1){
            case 4001:  //app1
            printf("taskbar.bin: 4001\n");
            gws_clone_and_execute("#browser.bin");  break;
            case 4002:  //app2
            printf("taskbar.bin: 4002\n");
            gws_clone_and_execute("#editor.bin");  break;
            case 4003:  //app3
            printf("taskbar.bin: 4003\n");
            gws_clone_and_execute("#terminal.bin");  break;
            };
            */
            break;


        // 20 = MSG_KEYDOWN
        case MSG_KEYDOWN:
            /*
            switch(long1){
                // keyboard arrows
                case 0x48: 
                    goto done; 
                    break;
                case 0x4B: 
                    goto done; 
                    break;
                case 0x4D: 
                    goto done; 
                    break;
                case 0x50: 
                    goto done; 
                    break;
                
                case '1':
                    goto done;
                    break;
 
                case '2': 
                    goto done;
                    break;
                
                case VK_RETURN:
                    return 0;
                    break;
                
                // input
                default:                
                    break;
            }
            */
            break;

        // 22 = MSG_SYSKEYDOWN
        case MSG_SYSKEYDOWN:
            //printf("taskbar: MSG_SYSKEYDOWN\n");
            switch (long1){
                case VK_F1:  do_launch_app(1);  break;
                case VK_F2:  do_launch_app(2);  break;
                // ...
                //case VK_F5: gws_async_command(fd,30,0,0);    break;
                //case VK_F6: gws_async_command(fd,1011,0,0);  break;
                //case VK_F7: gws_async_command(fd,30,0,0);    break;
                default:
                    break;
            };
            break;

        default:
            goto fail;
            break;
    };

    // ok
    // retorna TRUE quando o diálogo chamado 
    // consumiu o evento passado à ele.

done:
    //check_victory(fd);
    return 0;
    //return (int) gws_default_procedure(fd,0,msg,long1,long2);
fail:
    return (int) (-1);
}

// Pump event
// + Request next event with the server.
// + Process the event.
void pump(int fd, int wid)
{
    struct gws_event_d  lEvent;
    lEvent.used = FALSE;
    lEvent.magic = 0;
    lEvent.type = 0;
    //lEvent.long1 = 0;
    //lEvent.long2 = 0;

    struct gws_event_d *e;

    if (fd<0)
        return;
    if (wid<0)
        return;

// Request event with the server.
    e = 
        (struct gws_event_d *) gws_get_next_event(
                                   fd, 
                                   wid,
                                   (struct gws_event_d *) &lEvent );

    if ((void *) e == NULL)
        return;
    if (e->magic != 1234){
        return;
    }
    if (e->type < 0)
        return;

// Process event.
    int Status = -1;
    Status = 
        tbProcedure( 
            fd, 
            e->window, e->type, e->long1, e->long2 );

    // ...
}

static void __initialize_client_list(void)
{
    register int i=0;
    for (i=0; i<CLIENT_COUNT_MAX; i++)
    {
        clientList[i].used = FALSE;
        clientList[i].magic = 0;
        clientList[i].client_id = -1;
        
        clientList[i].icon_info.wid = -1;
        clientList[i].icon_info.icon_id = -1;
        clientList[i].icon_info.state = 0;
    };
}

static int 
create_bar_icon(
    int fd,
    int parent,
    int icon_id,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    char *label )
{
    int tmp1 = -1;
    unsigned long style = 0;

    if (fd<0)
        goto fail;
    if (parent<0)
        goto fail;
    if ((void*) label == NULL)
        goto fail;
    if (*label == 0)
        goto fail;

    tmp1 = 
        (int) gws_create_window (
                     fd,
                     WT_BUTTON, 
                     BS_DEFAULT, 
                     1,
                     label,
                     left, top, width, height,
                     parent, 
                     style, 
                     COLOR_GRAY, 
                     COLOR_GRAY );
    if (tmp1<0){
        printf("taskbar.bin: tmp1\n");
        exit(1);
    }

    //gws_refresh_window(fd, tmp1);
    //NavigationInfo.button00_window = tmp1;
    //return 0;
    return (int) tmp1;

fail:
    return (int) -1;
}

static int draw_separator(int fd)
{
// ========================
// Create separator

    if (fd<0)
        return -1;

    if (main_window<0)
        return -1;

    if (NavigationInfo.useSeparator != TRUE)
        return -1;

    gws_draw_text (
        (int) fd,
        (int) main_window,
        (unsigned long) 2 +84 +84 +84 +2,
        (unsigned long) 8,
        (unsigned long) COLOR_GRAY,
        "|" );

    return 0;
}

static int create_icons_on_desktop(int fd)
{
    register int i=0;
    static int Max = 4;

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

    if (fd<0)
        goto fail;
    if (desktop_window<0)
        goto fail;

    int ParentWID = desktop_window;


    // grid item
    unsigned long w0 = (screen_w/8);
    unsigned long h0 = ((screen_h-TASKBAR_HEIGHT)/4);
    // icon
    unsigned long l = 10;
    unsigned long t = 10;
    unsigned long w = (w0 -4);
    unsigned long h = (h0 -4);

    // #todo
    // Create the style WS_DESKTOPICON
    unsigned long style = WS_TITLEBARICON;

    // #test (Local for now)
    int iconID = -1;

// #todo
// We need a routine to create and populate the grid.

    for (i=0; i<Max; i++){
    t = (h0*i);
    iconID = 
        (int) gws_create_window (
                fd,
                WT_ICON, 1, 1, "IconXX",
                l, t, w, h,
                ParentWID, 
                style, 
                COLOR_WHITE, COLOR_WHITE );

    if (iconID < 0){
        printf ("taskbar.bin: iconID\n");
        exit(1);
    } 
    };

    return 0;
fail:
    return (int) -1;
}


static int create_desktop_area(int fd)
{
// This window will have the same size of the working area,
// managed by the display server.

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

    if (fd<0)
        goto fail;

//
// Window
//

    // The taskbar window.
    unsigned long l = 0;
    unsigned long t = 0;
    unsigned long w = screen_w;
    unsigned long h = screen_h - TASKBAR_HEIGHT;

    unsigned long style = WS_CHILD;

    unsigned int Color01 = COLOR_DESKTOP;
    unsigned int Color02 = COLOR_DESKTOP;

    desktop_window = 
        (int) gws_create_window (
                fd,
                WT_SIMPLE, 1, 1, "DesktopWin",
                l, t, w, h,
                0, 
                style, 
                Color01, Color02 );

    if (desktop_window < 0){
        printf ("taskbar.bin: desktop_window\n");
        exit(1);
    } 

    create_icons_on_desktop(fd);
    gws_refresh_window(fd, desktop_window);
    return 0;

fail:
    //exit(1);
    return (int) -1;
}

//==========================================
// Main
int main(int argc, char *argv[])
{
// #test
// Testing floating point.

    /*
    // #debug: Let's crash this application
    int *pf_ptr = NULL;
    // Dereferencing the NULL pointer triggers a page fault.
    int pf_value = *pf_ptr;
    printf("Value: %d\n", pf_value);
    */


// #config

    int ShowCube = FALSE;
    int launchChild = TRUE;
    int useDesktop = FALSE; //TRUE;
    // ...
    int client_fd = -1;


// Initialize navigation info structure.
    NavigationInfo.button00_window = -1;
    NavigationInfo.button01_window = -1;
    NavigationInfo.button02_window = -1;
    NavigationInfo.useSeparator = TRUE;
    NavigationInfo.initialized = TRUE;

// Initialization
    icon_left_limit = 80;
    icon_counter = 0;
    int ic=0;
    for (ic=0; ic<32; ic++)
        iconList[0] = 0;

// hello
    //gws_debug_print ("taskbar.bin: Hello world \n");
    //printf          ("taskbar.bin: Hello world \n");

// interrupts
    //gws_debug_print ("taskbar.bin: Enable interrupts \n");
    //printf          ("taskbar.bin: Enable interrupts \n");
    //asm ("int $199 \n");


// interrupts
// Unlock the taskswitching support.
// Unlock the scheduler embedded into the base kernel.
// Only the init process is able to do this.

    //gws_debug_print ("taskbar.bin: Unlock taskswitching and scheduler\n");
    //printf          ("taskbar.bin: Unlock taskswitching and scheduler\n");

    //sc80 (641,0,0,0);
    //sc80 (643,0,0,0);

// Create the rectangle
    //gws_debug_print ("taskbar.bin: Create rectangle\n");
    //printf          ("taskbar.bin: Create rectangle\n");
    //sc80(897,0,0,0);

//
// hang
//
    //while (1){
    //    sc80(897,0,0,0);
    //}

//================================
// Connection
// Only connect. Nothing more.
// Create socket and call connect().
    client_fd = (int) __initialization();
    if (client_fd < 0)
    {
        gws_debug_print("taskbar.bin: __initialization fail\n");
        printf         ("taskbar.bin: __initialization fail\n");
        exit(1);
    }

// #debug
    //printf(":: Entering taskbar.bin pid{%d} fd{%d}\n",
        //getpid(), client_fd);
    //while(1){}

// ==================================================
// Create the desktop area and one icon for test.

    if (useDesktop == TRUE)
        create_desktop_area(client_fd);

// ==================================================


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
        printf ("taskbar.bin: w h\n");
        exit(1);
    }
    // Saving display properties.
    DeviceInfo.width  = (unsigned long) (w & 0xFFFF);
    DeviceInfo.height = (unsigned long) (h & 0xFFFF);

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

// The taskbar window.
    unsigned long tb_l = 0;
    unsigned long tb_t = h - TASKBAR_HEIGHT;
    unsigned long tb_w = w;
    unsigned long tb_h = TASKBAR_HEIGHT;

    unsigned long style = WS_TASKBAR;

    main_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_SIMPLE, 1, 1, program_name,
                  tb_l, tb_t, tb_w, tb_h,
                  0, 
                  style, 
                  HONEY_COLOR_TASKBAR, HONEY_COLOR_TASKBAR );

    if (main_window < 0){
        printf ("taskbar.bin: main_window\n");
        exit(1);
    }
    gws_set_active( client_fd, main_window );

// ========================
// Create button based on the taskbar dimensions.

    NavigationInfo.button00_window = 
    (int) create_bar_icon (
        client_fd,
        main_window,
        0, // Icon ID
        2, 
        2, 
        (8*10),   // 8 chars width. 
        tb_h -2,
        buttom00_label );

// ========================
// Create button based on the taskbar dimensions.

    NavigationInfo.button01_window = 
    (int) create_bar_icon (
        client_fd,
        main_window,
        0, // Icon ID
        2 +84, 
        2, 
        (8*10),   // 8 chars width. 
        tb_h -2,
        buttom01_label );

// ========================
// Create button based on the taskbar dimensions.

    NavigationInfo.button02_window = 
    (int) create_bar_icon (
        client_fd,
        main_window,
        0, // Icon ID
        2 +84 +84, 
        2, 
        (8*10),   // 8 chars width. 
        tb_h -2,
        buttom02_label );


// ========================
// Create separator

    draw_separator(client_fd);

/*
    gws_draw_text (
        (int) client_fd,
        (int) main_window,
        (unsigned long) 2 +84 +84 +84 +2,
        (unsigned long) 2,
        (unsigned long) COLOR_GRAY,
        "|" );
*/


    //printf ("taskbar.bin: main_window created\n");
    //while(1){}

    //printf("gws.bin: [2] after create simple green window :)\n");
    //asm ("int $3");
    
    // #debug
    //gws_refresh_window (client_fd, main_window);

    //while(1){}
    //asm ("int $3");


/*
// barra azul no topo.
//===============================
    gws_debug_print ("taskbar.bin:  Creating  window \n");
    //printf        ("taskbar.bin: Creating main window \n");
    int tmp1 = -1;
    tmp1 = (int) gws_create_window (
                     client_fd,
                     WT_SIMPLE, 1, 1, "status",
                     0, 0, w, 24,
                     0, 0, COLOR_BLUE, COLOR_BLUE );
    if (tmp1<0){
        printf ("taskbar.bin: tmp1\n");
        exit(1);
    }
    //status_window = tmp1;
//========================
*/

    //printf ("taskbar.bin: status_window created\n");
    //while(1){}

    //printf("gws.bin: [2] after create simple gray bar window :)\n");

    // #debug
    //gws_refresh_window (client_fd, tmp1);
    //asm ("int $3");

/*
//===================
// Drawing a char just for fun,not for profit.

    gws_debug_print ("taskbar.bin: 2 Drawing a char \n");
    //printf        ("taskbar.bin: Drawing a char \n");
    if (main_window > 0)
    {
        gws_draw_char ( 
            client_fd, 
            main_window, 
            2, 2, COLOR_RED, 'G' );
         
         // Símbolo para somatoria.
         // #ps: The extended ascii chars are working.
         gws_draw_char ( 
            client_fd, 
            main_window, 
            2 + 8 + 2, 2, COLOR_RED, 228 );
    }
//====================   
*/

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

    //gws_debug_print ("taskbar.bin: 4 Testing Plot cube\n");
    //printf        ("taskbar.bin: 4 Testing Plot cube\n");

/*

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
*/


    //gws_debug_print("LOOP:\n");
    //printf ("LOOP:\n");


    //
    // Loop
    //

    // #test
    //gws_refresh_window (client_fd, main_window);
        

    //??
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

    gws_refresh_window(client_fd, main_window);


//
// Client
//

// #todo
// Podemos nesse momento ler alguma configuração
// que nos diga qual interface devemos inicializar.

    /*
    if(launchChild == TRUE)
    {
        gws_redraw_window(client_fd,main_window,0);
        
        // Interface 1: File manager.
        //gws_clone_and_execute("#fileman.bin");

        // Interface 1: Test app.
        //gws_clone_and_execute("#editor.bin");
    }
    */

//
// Input
//
    
    // Enable input method number 1.
    // Event queue in the current thread.

    //gws_enable_input_method(1);

    //=================================


// =================================
// Focus

// Set focus on current thread.
    //rtl_focus_on_this_thread();

// Set focus on main window.
// #bugbug
// Maybe it can switch the foreground thread.
/*
    gws_async_command(
         client_fd,
         9,             // 9 = set focus
         main_window,
         main_window );
*/

//
// Banner
//

// Set cursor position on top of the raw window.
// #bugbug
// Sometimes we use printf directly on screen for debug purpose.
    sc80 ( 34, 2, 2, 0 );

/*
//#tests
    printf ("taskbar.bin: Gramado OS\n");
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

    //printf("\n");
    //printf("Testing math:\n");
    //printf("\n");

/*
// -------------------
// float. ok on qemu.
    float float_var = 1.5;
    unsigned int float_res = (unsigned int) (float_var + float_var);
    printf("float_res={%d}\n",(unsigned int)float_res);
*/


/*
// -------------------
// double. ok on qemu.
    double double_var = 2.5000;
    unsigned int double_res = (unsigned int) (double_var + double_var);
    printf("double_res={%d}\n",(unsigned int)double_res);
*/

/*
// -------------------
// #test
// See: math.c in rtl/
    //unsigned int square_root = (unsigned int) sqrt(81);
    //printf("sqrt of 81 = {%d}\n",(unsigned int)square_root);
    unsigned long square_root = (unsigned long) sqrt(81);
    printf("sqrt of 81 = {%d}\n",(unsigned long)square_root);
*/

/*
// -------------------
    // 9 ao cubo.
    //long r9 = (long) power0(9,3);
    //long r9 = (long) power1(9,3);
    //long r9 = (long) power2(9,3);
    //long r9 = (long) power3(9,3);
    long r9 = (long) power4(9,3);
    printf("9^3 = {%d}\n", (long) r9);
*/

// ===============================

//#breakpoint
    //printf("taskbar.bin: Breakpoint :)\n");
    //while(1){}

/*
// #test
// Getting 2mb shared memory surface.
// ring3.
    void *ptr;
    ptr = (void*) rtl_shm_get_2mb_surface();
    if( (void*) ptr != NULL )
        printf("surface address: %x\n",ptr);
*/

// ===============================
// Show prompt.
    //doPrompt(client_fd);

// ===============================
//
    __initialize_client_list();

// =======================
// Loop

    unsigned long start_jiffie=0;
    unsigned long end_jiffie=0;
    unsigned long delta_jiffie=0;
    int UseSleep = TRUE;

// #ps: We will sleep if a round was less than 16 ms, (60fps).
// The thread wait until complete the 16 ms.
// #bugbug: Valid only if the timer fires 1000 times a second.
// It gives the opportunities for other threads to run a
// a bit more.

    while (1){
        start_jiffie = (unsigned long) rtl_jiffies();
        
        //if (isTimeToQuit == TRUE)
            //break;
        pump(client_fd,main_window);
        
        end_jiffie = rtl_jiffies();
        if (end_jiffie > start_jiffie)
        {
            delta_jiffie = (unsigned long) (end_jiffie - start_jiffie);
            if (delta_jiffie < 16)
            {
                // #test
                // This function is still in test phase.
                if (UseSleep == TRUE)
                    rtl_sleep(16 - delta_jiffie);
            }    
        }
    };

/*
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

            cmdlineProcedure ( 
                client_fd,
                (void*) RTLEventBuffer[0], 
                RTLEventBuffer[1], 
                RTLEventBuffer[2], 
                RTLEventBuffer[3] );
        }
    };
//=================================
*/
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
 
// Done:
    gws_debug_print("taskbar.bin: bye :)\n");
    return EXIT_SUCCESS;
}

//
// End
//

