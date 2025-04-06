// main.c
// Main file for xtb.bin
// This application uses libx, a library
// that works as a X11-like wrapper to create 
// Gramado OS client-side GUI applications.
// See: api/libs/libx/


/*
 * File: main.c
 *    Client side application for Gramado Window Server.
 *    Using socket to connect with gws.
 *    AF_GRAMADO family.
 *    #warning:
 *    This application is using floating point.
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

// libx - The client-side library.
#include <libx.h>

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

struct device_info_d 
{
    unsigned long width;
    unsigned long height;
};
struct device_info_d DeviceInfo;

// Creating 32 clients.
// It represents the order in the taskbar.
// If an entry is not used, it will not be
// shown in the taskbar.
struct tb_client_d clientList[CLIENT_COUNT_MAX];

//
// Windows
//

int main_window = -1;
int startmenu_window = -1;

//
// Strings
//

const char *program_name = "XTB";
//const char *start_menu_button_label = "Gramado";
const char *start_menu_button_label = "XTB";

const char *app1_name = "terminal.bin";
const char *app2_name = "editor.bin";

//static const char *cmdline1 = "gramland -1 -2 -3 --tb";


struct gws_display_d *Display;
const char *display_name = "display:name.0";

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

static void pump(int fd, int wid);
static void pump2(void);

static int 
create_start_menu(
    int fd,
    int parent,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height );

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
    gws_debug_print("xtb.bin: Initializing\n");
    //printf       ("xtb.bin: Initializing ...\n");

// Socket:
// Create a socket. 
// AF_GRAMADO = 8000

    // #debug
    //printf ("xtb: Creating socket\n");

    client_fd = 
        (int) socket( 
            AF_INET,   // Remote or local connections
            SOCK_RAW,  // Type
            0 );       // Protocol

    if (client_fd < 0){
       printf ("xtb.bin: on socket()\n");
       goto FatalError;
    }

// Connect
// Nessa hora colocamos no accept um fd.
// então o servidor escreverá em nosso arquivo.
// Tentando nos conectar ao endereço indicado na estrutura
// Como o domínio é AF_GRAMADO, então o endereço é "w","s".

    //printf ("xtb: Trying to connect to the address 'ws' ...\n");      

    while (TRUE){
        if (connect(client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){ 
            printf("xtb.bin: Connection Failed\n"); 
        }else{ 
            break;
        }; 
    };

    return (int) client_fd;

FatalError:
    exit(1);
    return (int) -1;
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
    //printf("xtb.bin: Type something\n");
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
        printf("xtb.bin: Command line application\n");
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
    size_t string_size=0;
    memset(filename,0,16);

    //do_clear_console();
    //printf ("Launching GUI\n");

// #bugbug
// Sending cmdline via stdin
    rewind(stdin);
    //write( fileno(stdin), cmdline1, strlen(cmdline1) );

// Launch new process.

    if (app_number == 1){
        sprintf(filename,app1_name);
        string_size = strlen(app1_name);
    }else if (app_number == 2){
        sprintf(filename,app2_name);
        string_size = strlen(app2_name);
    }else{
        return;
    }

    filename[string_size] = 0;
    ret_val = (int) rtl_clone_and_execute(filename);
    //ret_val = (int) rtl_clone_and_execute(app1_name);
    if (ret_val<=0){
        printf("Couldn't clone\n");
        return;
    }

// Sleep (Good!)
    //sc82( 266, 8000, 8000, 8000 );

    //printf("pid=%d\n",ret_val);

// Quit the command line.
    //isTimeToQuit = TRUE;
}


static int 
tbProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int f12Status = -1;

// Parameters:
    if (fd<0)
        return -1;
    if(event_type<=0){
        return (-1);
    }

    switch (event_type){

        //#todo
        // Update the bar and the list of clients.
        case MSG_PAINT:
            //printf("xtb: MSG_PAINT\n");
            // #todo
            // We need to update all the clients
            // Create update_clients()
            gws_redraw_window(fd, main_window, TRUE);
            gws_redraw_window(fd, startmenu_window, TRUE);
            break;

        // One button was clicked
        case GWS_MouseClicked:
            printf("xtb: GWS_MouseClicked\n");
            
            // #test: Clear main window when clicking the button.
            XClearWindow(Display,Display->main_wid);

            // #test Create simple window when clicking the button.        
            int new_wid = (int) XCreateSimpleWindow(
                Display, 
                Display->main_wid,  // parent wid 
                2,    //x 
                2,    //y 
                40,   //width 
                 4,   //height 
                1,    //border_width 
                1,    //border, 
                1 );  //background
            // Show the window new window.
            gws_refresh_window(fd, new_wid);

            break;
       
        // Add new client. Given the wid.
        // The server created a client.
        case 99440:
            printf("xtb: [99440]\n");
            break;

        // Remove client. Given the wid.
        // The server removed a client.
        case 99441:
            printf("xtb: [99441]\n");
            break;
        
        // Update client info.
        // The server send data about the client.
        case 99443:
            printf("xtb: [99443]\n");
            break;

        case MSG_CLOSE:
            printf("xtb: Closing...\n");
            exit(0);
            break;
        
        case MSG_COMMAND:
            /*
            printf("xtb.bin: MSG_COMMAND %d \n",long1);
            switch(long1){
            case 4001:  //app1
            printf("xtb.bin: 4001\n");
            gws_clone_and_execute("browser.bin");  break;
            case 4002:  //app2
            printf("xtb.bin: 4002\n");
            gws_clone_and_execute("editor.bin");  break;
            case 4003:  //app3
            printf("xtb.bin: 4003\n");
            gws_clone_and_execute("terminal.bin");  break;
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
            //printf("xtb: MSG_SYSKEYDOWN\n");
            switch (long1){
                case VK_F1:
                    //gws_clone_and_execute(app1_name);
                    do_launch_app(1);
                    break;
                case VK_F2:
                    //gws_clone_and_execute(app2_name);
                    do_launch_app(2);
                    break;
                default:
                    break;
            };

            /*
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
                    //printf("xtb: Shutting down ...\n");
                    //f12Status = (int) gws_clone_and_execute("shutdown.bin");
                    //if (f12Status<0){ break; } // fail
                    //exit(0);
                    break;
 
                // ...
            };
            */
            break;
    };

// ok
// retorna TRUE quando o diálogo chamado 
// consumiu o evento passado à ele.
done:
    return 0;
    //return (int) gws_default_procedure(fd,0,msg,long1,long2);
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

// Good old method.
// see: pump2() for method using X function.
static void pump(int fd, int wid)
{
    struct gws_event_d lEvent;
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
    if (e->type <0)
        return;

    tbProcedure( 
        fd, 
        e->window, e->type, e->long1, e->long2 );
}

// Method using X function.
static void pump2(void)
{
    struct _XDisplay *dpy;
    struct _XEvent e;
    register int fd = -1;
    register int wid = -1;

// Check our display structure.
    dpy = (struct _XDisplay *) Display;
    if ((void*)dpy == NULL){
        printf("pump2: dpy\n");
        goto FatalError;
    }
    if (dpy->magic != 1234){
        printf("pump2: dpy->magic\n");
        goto FatalError;
    }

// Get parameters we need.
    fd = (int) dpy->fd ;
    wid = (int) dpy->main_wid;
// Validation
    if (fd<0){
        printf("pump2: fd\n");
        goto FatalError;
    }
    if (wid<0){
        printf("pump2: wid\n");
        goto FatalError;
    }

// Request next event and process it.
    int Status = (int) XNextEvent( (struct _XDisplay *) dpy, (struct _XEvent *) &e );
    if (Status < 0){
        return;
    }
    if (e.magic != 1234){
        return;
    }
    if (e.type < 0){
        return;
    }
    tbProcedure( fd, e.window, e.type, e.long1, e.long2 );
    return;

FatalError:
    printf("pump2: FatalError\n");
    exit(1);
    return;
}

static int 
create_start_menu(
    int fd,
    int parent,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height )
{
    int tmp1 = -1;
    unsigned long style = 0;

    if (fd<0)
        return -1;
    if (parent<0)
        return -1;

    tmp1 = 
        (int) gws_create_window (
                     fd,
                     WT_BUTTON, 
                     BS_DEFAULT, 
                     1, 
                     start_menu_button_label,
                     left, top, width, height,
                     parent, 
                     style, 
                     COLOR_GRAY, 
                     COLOR_GRAY );
    if (tmp1<0){
        printf("xtb.bin: tmp1\n");
        exit(1);
    }

    //gws_refresh_window(fd, tmp1);
    
    startmenu_window = tmp1;
    return 0;
}


//==========================================
// #test: Testing this function
// Custom fscanf-like function to read a float
int my_fscanf(FILE *stream, const char *format, void *ptr) {
    if (stream == NULL || format == NULL || ptr == NULL) {
        return -1; // Error: invalid arguments
    }

    // For simplicity, we only handle "%f" format
    if (format[0] != '%' || format[1] != 'f' || format[2] != '\0') {
        return -1; // Unsupported format
    }

    float *result = (float *)ptr; // Cast the void pointer to float pointer
    float value = 0.0f;
    int sign = 1;
    int c;

    // Skip leading whitespace
    while ((c = fgetc(stream)) != EOF && (c == ' ' || c == '\t' || c == '\n'));

    if (c == EOF) {
        return 0; // No items read
    }

    // Handle sign
    if (c == '-') {
        sign = -1;
        c = fgetc(stream);
    } else if (c == '+') {
        c = fgetc(stream);
    }

    // Read integer part
    int has_digits = 0;
    while (c >= '0' && c <= '9') {
        value = value * 10.0f + (c - '0');
        has_digits = 1;
        c = fgetc(stream);
    }

    // Read decimal part if present
    if (c == '.') {
        float fraction = 0.1f;
        c = fgetc(stream);
        while (c >= '0' && c <= '9') {
            value += (c - '0') * fraction;
            fraction *= 0.1f;
            has_digits = 1;
            c = fgetc(stream);
        }
    }

    // If no valid digits were read, return failure
    if (!has_digits) {
        ungetc(c, stream); // Put back the last character
        return 0;
    }

    // Apply sign and store the result
    *result = value * sign;

    // Put back the last non-number character
    if (c != EOF) {
        ungetc(c, stream);
    }

    return 1; // Successfully read one item
}


// Example usage
int test_fun00(void)
{

    printf("test_fun00:\n");

    FILE *file = fopen("test.txt", "r");
    if (!file) {
        printf("Error opening file!\n");
        return 1;
    }

    float value;
    int result = my_fscanf(file, "%f", &value);
    if (result == 1) {
        //printf("ok\n");
        //printf("Read float: %f\n", value);
        if (value == 3.14f)
            printf("It's PI\n");
    } else {
        printf("Failed to read float, result: %d\n", result);
    }

    //fclose(file);
    return 0;
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

// hello
    //printf          ("xtb.bin: Hello world \n");

// interrupts
    //printf          ("xtb.bin: Enable interrupts \n");
    //asm ("int $199 \n");

// interrupts
// Unlock the taskswitching support.
// Unlock the scheduler embedded into the base kernel.
// Only the init process is able to do this.

    //printf          ("xtb.bin: Unlock taskswitching and scheduler\n");

    //sc80 (641,0,0,0);
    //sc80 (643,0,0,0);

// Create the rectangle
    //printf          ("xtb.bin: Create rectangle\n");
    //sc80(897,0,0,0);

//
// hang
//
    //while (1){
    //    sc80(897,0,0,0);
    //}




// ================================================
// # testing 
    //printf("xtb:\n");
    //test_fun00();
    //while(1){}

// ================================================

/*
//================================
// Connection
// Only connect. Nothing more.
// Create socket and call connect().
    client_fd = (int) __initialization();
    if (client_fd < 0)
    {
        gws_debug_print("xtb.bin: __initialization fail\n");
        printf         ("xtb.bin: __initialization fail\n");
        exit(1);
    }
*/

    Display = (struct gws_display_d *) gws_open_display(display_name);
    if ((void*) Display == NULL){
        printf ("xtb: Couldn't open display\n");
        exit(1);
    }
    if (Display->magic != 1234){
        printf ("xtb: Display->magic\n");
        exit(1);
    }

// Socket fd.
    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        debug_print("xtb: bad fd\n");
        printf ("xtb: bad fd\n");
        exit(1);
    }

// #debug
    //printf(":: Entering xtb.bin pid{%d} fd{%d}\n",
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
        printf ("xtb.bin: w h\n");
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

    //printf("xtb: [1] calling create window\n");

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
        printf ("xtb.bin: main_window\n");
        exit(1);
    }
    gws_set_active( client_fd, main_window );

// Save into the display structure
    Display->main_wid = (int) main_window;

// ========================
// Create th start menu button 
// based on the taskbar dimensions.
    create_start_menu(
        client_fd,
        main_window,
        2, 
        2, 
        (8*10),   // 8 chars width. 
        tb_h -4 );

    //printf ("xtb.bin: main_window created\n");
    //while(1){}


    //printf("xtb: [2] after create simple green window :)\n");

    //asm ("int $3");
    
    // #debug
    //gws_refresh_window (client_fd, main_window);

    //while(1){}
    //asm ("int $3");


/*
// barra azul no topo.
//===============================
    gws_debug_print ("xtb.bin:  Creating  window \n");
    //printf        ("xtb.bin: Creating main window \n");
    int tmp1 = -1;
    tmp1 = (int) gws_create_window (
                     client_fd,
                     WT_SIMPLE, 1, 1, "status",
                     0, 0, w, 24,
                     0, 0, COLOR_BLUE, COLOR_BLUE );
    if (tmp1<0){
        printf ("xtb.bin: tmp1\n");
        exit(1);
    }
    //status_window = tmp1;
//========================
*/

    //printf ("xtb.bin: status_window created\n");
    //while(1){}

    //printf("xtb: [2] after create simple gray bar window :)\n");

    // #debug
    //gws_refresh_window (client_fd, tmp1);
    //asm ("int $3");

/*
//===================
// Drawing a char just for fun,not for profit.

    gws_debug_print ("xtb.bin: 2 Drawing a char \n");
    //printf        ("xtb.bin: Drawing a char \n");
    if(status_window>0)
    {
        gws_draw_char ( 
            client_fd, 
            status_window, 
            0, 0, COLOR_YELLOW, 'G' );
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
    gws_debug_print ("xtb: 3 Testing Plot0 4x\n");
    printf          (": 3 Testing Plot0 4x\n");

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

    gws_debug_print ("xtb.bin: 4 Testing Plot cube\n");
    //printf        ("xtb.bin: 4 Testing Plot cube\n");

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
        //gws_clone_and_execute("fileman.bin");

        // Interface 1: Test app.
        //gws_clone_and_execute("editor.bin");
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


// set focus
    rtl_focus_on_this_thread();

// set focus
    gws_async_command(
         client_fd,
         9,             // set focus
         main_window,
         main_window );

//
// Banner
//

// Set cursor position.
    sc80 ( 34, 2, 2, 0 );

    //printf ("xtb.bin: Gramado OS\n");

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

    //#breakpoint
    //printf("xtb.bin: Breakpoint :)\n");
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
// Event loop:
// Pumping events and processing them all.

    while (1)
    {
        //if (isTimeToQuit == TRUE)
            //break;

        // Method using the libx function.
        pump2();

        // Method using the libgws function.
        //pump(client_fd,main_window);
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
    //gws_debug_print ("xtb: Sending command to close the server. \n");
    //gws_async_command(client_fd,1,0,0);
    //exit(0);

    // Asking to server to send me an notification
    // telling me to close myself
    
    //gws_debug_print ("xtb: Pinging\n");
    //gws_async_command(client_fd,2,0,0);


    //while(1){}
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
        //    gws_debug_print ("xtb: [EVENT] We got the event 12344321\n \n");
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
    printf("xtb.bin: EXIT_SUCCESS\n");
    return EXIT_SUCCESS;
}

//
// End
//
