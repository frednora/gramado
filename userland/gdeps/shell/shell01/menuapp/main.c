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

int main(int argc, char *argv[])
{
    int client_fd=-1;

    printf("MENUAPP.BIN: Hello\n");

    client_fd = (int) __initialize_connection();
    if (client_fd<0)
        return EXIT_FAILURE;


//
// Menu structure
//
    struct gws_menu_d *menu00;

//
// Creating the menu
//

    menu00 = 
        (struct gws_menu_d *) gws_create_menu(
            client_fd,
            0,
            TRUE,
            4,
            10,
            10,
            40,
            40,
            0xffffffff 
        );

    if ((void*) menu00 == NULL)
        return EXIT_FAILURE;


//
// Menu item
//

/*
struct gws_menu_item_d *gws_create_menu_item (
    int fd,
    const char *label,
    int id,
    struct gws_menu_d *menu)
*/

    printf("Test done\n");

    return EXIT_SUCCESS;
}
