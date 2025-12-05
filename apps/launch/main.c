// main.c
// Created by Fred Nora.

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

#include "launch.h"

struct gws_display_d *Display;

// Network ports
#define PORTS_WS  4040
#define PORTS_NS  4041
#define PORTS_FS  4042
// ...

#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)



struct my_menu_info_d 
{
    int menu_wid;

    int item0_wid;
    int item1_wid;
    int item2_wid;
    int item3_wid;
};
struct my_menu_info_d MyMenuInfo;

static int main_window = -1;
static int isTimeToQuit=FALSE;

// ====================================

static int 
launchProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 );

void pump(int fd, int wid);
static int __initialize_connection(void);

static void __paint_worker(int fd);
static void __close_worker(int fd);

// ====================================


// Worker: redraw all children when we receive MSG_PAINT
static void __paint_worker(int fd)
{
    // Redraw the main window
    gws_redraw_window(fd, main_window, TRUE);

    // Redraw the menu container
    gws_redraw_window(fd, MyMenuInfo.menu_wid, TRUE);

    // Redraw each menu item
    gws_redraw_window(fd, MyMenuInfo.item0_wid, TRUE);
    gws_redraw_window(fd, MyMenuInfo.item1_wid, TRUE);
    gws_redraw_window(fd, MyMenuInfo.item2_wid, TRUE);
    gws_redraw_window(fd, MyMenuInfo.item3_wid, TRUE);

    gws_refresh_window (fd, main_window);

    //#todo: Lock the geometry of the main window and the menu container.

    // #debug
    //printf("Paint worker: refreshed main, menu, and items\n");
}

// Worker: destroy all windows and exit
static void __close_worker(int fd)
{
    // Destroy children first
    gws_destroy_window(fd, MyMenuInfo.item0_wid);
    gws_destroy_window(fd, MyMenuInfo.item1_wid);
    gws_destroy_window(fd, MyMenuInfo.item2_wid);
    gws_destroy_window(fd, MyMenuInfo.item3_wid);

    // Destroy the menu container
    gws_destroy_window(fd, MyMenuInfo.menu_wid);

    // Destroy the main window
    gws_destroy_window(fd, main_window);

    //printf("Close worker: destroyed all windows\n");
}


// Process event that came from the server.
static int 
launchProcedure(
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

        case 0:
            printf("msg\n");
            break;

        //#todo
        // Update the bar and the list of clients.
        case MSG_PAINT:
            __paint_worker(fd);   // redraw main, container, items
            //printf("launch: MSG_PAINT\n");

            // #todo
            // We need to update all the clients
            // Create update_clients()
            //gws_redraw_window(fd, main_window, TRUE);
            //gws_redraw_window(fd, NavigationInfo.button00_window, TRUE);
            //gws_redraw_window(fd, NavigationInfo.button01_window, TRUE);
            //gws_redraw_window(fd, NavigationInfo.button02_window, TRUE);
            //draw_separator(fd);
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
            //printf("[DEBUG EVENT] type=%d event_window=%d long1=%d long2=%d\n",
                //event_type, event_window, (int) long1, (int) long2);

            if ((int) long1 == MyMenuInfo.item0_wid)
                printf("Item 0\n");
            if ((int) long1 == MyMenuInfo.item1_wid)
                printf("Item 1\n");
            if ((int) long1 == MyMenuInfo.item2_wid)
                printf("Item 2\n");
            if ((int) long1 == MyMenuInfo.item3_wid)
                printf("Item 3\n");
            break;

        // Add new client. Given the wid.
        // The server created a client.
        case 99440:
            printf("launch: [99440]\n");
            break;

        // Remove client. Given the wid.
        // The server removed a client.
        case 99441:
            printf("launch: [99441]\n");
            break;
        
        // Update client info.
        // The server send data about the client.
        case 99443:
            printf("launch: [99443]\n");
            break;

        // #test:
        // ds sent us a message to create an iconic window for an app.
        case 99500:
            /*
            tmpNewWID = (int) create_bar_icon(
                fd, 
                main_window,
                2,  // Icon ID
                8,8,28,28,
                "NEW" );
            if (tmpNewWID < 0)
                goto fail;
            gws_refresh_window(fd,tmpNewWID);
            */
            break;

        case MSG_CLOSE:
            __close_worker(fd);
            isTimeToQuit = TRUE;  // Signal to quit the app
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
                case VK_F1:  
                    //do_launch_app(1);  
                    break;
                case VK_F2:  
                    //do_launch_app(2);  
                    break;
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

// Request event with the display server.
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
    if (e->type <= 0)
        return;

// Process event
    int Status = -1;
    Status = launchProcedure( fd, e->window, e->type, e->long1, e->long2 );

    // ...
}


// OUT: client fd.
static int __initialize_connection(void)
{

// -------------------------
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = IP(127,0,0,1);    //ok
    //addr_in.sin_addr.s_addr = IP(127,0,0,9);  //fail
    addr_in.sin_port = __PORTS_DISPLAY_SERVER;
// -------------------------


    int client_fd = -1;

    //gws_debug_print ("-------------------------\n"); 
    //printf          ("-------------------------\n"); 
    //gws_debug_print("taskbar.bin: Initializing\n");
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
       gws_debug_print("launch.bin: on socket()\n");
       printf         ("launch.bin: on socket()\n");
       exit(1);  //#bugbug Cuidado.
    }

// Connect
// Nessa hora colocamos no accept um fd.
// então o servidor escreverá em nosso arquivo.
// Tentando nos conectar ao endereço indicado na estrutura
// Como o domínio é AF_GRAMADO, então o endereço é "w","s".

    //printf ("gws: Trying to connect ..\n");      

    while (TRUE){
        if (connect(client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){ 
            debug_print("launch.bin: Connection Failed\n"); 
            printf     ("launch.bin: Connection Failed\n"); 
        }else{ break; }; 
    };

    return (int) client_fd;
}

int main(int argc, char *argv[])
{
    const char *display_name_string = "display:name.0";
    int client_fd=-1;

    //printf("launch.BIN: Hello\n");

// ============================
// Open display.
// IN: hostname:number.screen_number
    Display = (struct gws_display_d *) gws_open_display(display_name_string);
    if ((void*) Display == NULL){
        printf("launch.bin: Display\n");
        goto fail;
    }
// Get client socket.
    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        printf("launch.bin: fd\n");
        goto fail;
    }


//
// Create main window
//
    //printf("launch.BIN: Create window\n");

    const char *program_name = "Launch";

    main_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_OVERLAPPED, 
                  WINDOW_STATUS_ACTIVE,  // status
                  VIEW_NULL,             // view
                  program_name,
                  20, 20, 200, 300,
                  0,
                  0x0000,   // style?
                  COLOR_WINDOW, 
                  COLOR_WINDOW );

    if (main_window < 0){
        printf("on create window\n");
        return EXIT_FAILURE;
    }

    gws_refresh_window(client_fd,main_window);


//
// Menu structure
//

    //printf("launch.bin: Create menu\n");

    struct gws_menu_d *menu00;
    struct gws_window_info_d lWi;

// Get info about the main window.
// IN: fd, wid, window info structure.
    gws_get_window_info(
        client_fd, 
        main_window,   // The app window.
        (struct gws_window_info_d *) &lWi );

//
// Creating the menu
//

    menu00 = 
        (struct gws_menu_d *) gws_create_menu(
            client_fd,
            main_window,  // Parent
            TRUE,         // Highlight
            4,            // n of itens
            0, 0, lWi.cr_width, lWi.cr_height,   // Relative to the client area rectangle.
            COLOR_GRAY
        );

    if ((void*) menu00 == NULL){
        printf("on create menu\n");
        return EXIT_FAILURE;
    }

    MyMenuInfo.menu_wid = (int) menu00->window;

//
// Menu item
//

    struct gws_menu_item_d *tmp;
    const char *tmp_label0 = "Menu item 0";
    const char *tmp_label1 = "Menu item 1";
    const char *tmp_label2 = "Menu item 2";
    const char *tmp_label3 = "Menu item 3";

    tmp = 
        (struct gws_menu_item_d *) gws_create_menu_item (
            client_fd,
            tmp_label0,
            0,  // index
            menu00 );
    if (tmp->window < 0)
        printf("menuitem window fail\n");
    MyMenuInfo.item0_wid = tmp->window;

    tmp = 
        (struct gws_menu_item_d *) gws_create_menu_item (
            client_fd,
            tmp_label1,
            1,  // index
            menu00 );
    if (tmp->window < 0)
        printf("menuitem window fail\n");
    MyMenuInfo.item1_wid = tmp->window;

    tmp = 
        (struct gws_menu_item_d *) gws_create_menu_item (
            client_fd,
            tmp_label2,
            2,  // index
            menu00 );
    if (tmp->window < 0)
        printf("menuitem window fail\n");
    MyMenuInfo.item2_wid = tmp->window;

    tmp = 
        (struct gws_menu_item_d *) gws_create_menu_item (
            client_fd,
            tmp_label3,
            3,  // index
            menu00 );
    if (tmp->window < 0)
        printf("menuitem window fail\n");
    MyMenuInfo.item3_wid = tmp->window;

// Refresh only the menu window
    gws_refresh_window(client_fd,MyMenuInfo.menu_wid);

// Refresh the whole application window
    //gws_refresh_window(client_fd,main_window);


/*
// The IDs
    printf("[DEBUG CREATE] main_window=%d\n", main_window);
    printf("[DEBUG CREATE] menu_wid=%d\n", MyMenuInfo.menu_wid);
    printf("[DEBUG CREATE] item0_wid=%d\n", MyMenuInfo.item0_wid);
    printf("[DEBUG CREATE] item1_wid=%d\n", MyMenuInfo.item1_wid);
    printf("[DEBUG CREATE] item2_wid=%d\n", MyMenuInfo.item2_wid);
    printf("[DEBUG CREATE] item3_wid=%d\n", MyMenuInfo.item3_wid);
*/

//
// Event loop
//

// =======================
// Event loop
// Getting input events from the system.

    unsigned long start_jiffie=0;
    unsigned long end_jiffie=0;
    unsigned long delta_jiffie=0;
    int UseSleep = TRUE;

// #ps: We will sleep if a round was less than 16 ms, (60fps).
// The thread wait until complete the 16 ms.
// #bugbug: Valid only if the timer fires 1000 times a second.
// It gives the opportunities for other threads to run a bit more.

    isTimeToQuit =  FALSE;

    while (1){

        if (isTimeToQuit == TRUE)
            break;

        start_jiffie = (unsigned long) rtl_jiffies();
        // Get and process event.
        pump(client_fd,main_window);
        end_jiffie = rtl_jiffies();

        if (end_jiffie > start_jiffie)
        {
            delta_jiffie = (unsigned long) (end_jiffie - start_jiffie);
            // Let's sleep if the round was less than 16 ms.
            if (delta_jiffie < 16){
                if (UseSleep == TRUE)
                    rtl_sleep(16 - delta_jiffie);
            }    
        }
    };

// Done
    //printf("launch.bin: Test done\n");
    return EXIT_SUCCESS;

fail:
    printf("launch.bin: fail\n");
    return EXIT_FAILURE;
}
