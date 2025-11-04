// main.c

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


#include "menuapp.h"

// Network ports
#define PORTS_WS  4040
#define PORTS_NS  4041
#define PORTS_FS  4042
// ...

#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)


// ====================================

static int __initialize_connection(void);

// ====================================

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
       gws_debug_print("taskbar.bin: on socket()\n");
       printf         ("taskbar.bin: on socket()\n");
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
            debug_print("taskbar.bin: Connection Failed\n"); 
            printf     ("taskbar.bin: Connection Failed\n"); 
        }else{ break; }; 
    };

    return (int) client_fd;
}

int main(int argc, char *argv[])
{
    int client_fd=-1;

    //printf("MENUAPP.BIN: Hello\n");

    client_fd = (int) __initialize_connection();
    if (client_fd<0){
        printf("on connection\n");
        return EXIT_FAILURE;
    }

//
// Create main window
//
    //printf("MENUAPP.BIN: Create window\n");

    int main_window;
    const char *program_name = "LAUNCH";

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

    //printf("MENUAPP.BIN: Create menu\n");

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

    tmp = 
        (struct gws_menu_item_d *) gws_create_menu_item (
            client_fd,
            tmp_label1,
            1,  // index
            menu00 );

    tmp = 
        (struct gws_menu_item_d *) gws_create_menu_item (
            client_fd,
            tmp_label2,
            2,  // index
            menu00 );

    tmp = 
        (struct gws_menu_item_d *) gws_create_menu_item (
            client_fd,
            tmp_label3,
            3,  // index
            menu00 );


// Refresh only the menu window
    int menu_window = menu00->window;
    gws_refresh_window(client_fd,menu_window);

// Refresh the whole application window
    //gws_refresh_window(client_fd,main_window);



//
// Event loop
//

    // #todo


// Done
    printf("Test done\n");
    return EXIT_SUCCESS;
}
