/*
 * File: main.c
 *    This is the InitProcess.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>
// libgws - The client-side library.
#include <gws.h>

// Ports
#define PORTS_WS  4040
#define PORTS_NS  4041
#define PORTS_FS  4042
// ...
#define IP(a, b, c, d)  (a << 24 | b << 16 | c << 8 | d)


// == Private functions: prototypes =============
static int __gws_initialization(void);
//====================

// initialize via AF_GRAMADO.
// Ainda nao podemos mudar isso para a lib, pois precisamos
// do suporte as rotinas de socket e as definiçoes.
// tem que incluir mais coisa na lib.

static int __gws_initialization(void)
{

// Vamos nos concetar com o processo identificado 
// com o nome 'ws'
// The port name is 'port:/ws'

//==============================
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = IP(127,0,0,1);
    addr_in.sin_port = PORTS_WS;  //4040
//==============================

    int client_fd = -1;
 
    gws_debug_print("gws.bin: Initializing\n");
    //printf       ("gws.bin: Initializing\n");

// Socket
    //client_fd = (int) socket( AF_INET, SOCK_STREAM, 0 );
    client_fd = (int) socket( AF_INET, SOCK_RAW, 0 );
    if (client_fd < 0){
       gws_debug_print("gws: on socket()\n");
       printf         ("gws: on socket()\n");
       exit(1);  //#bugbug Cuidado.
    }

// Connect
// Nessa hora colocamos no accept um fd.
// então o servidor escreverá em nosso arquivo.
    while (1){
        if (connect(client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){
            gws_debug_print("gws: Connection Failed\n");
            printf         ("gws: Connection Failed\n");
        }else{ break; }; 
    };

    return (int) client_fd;
}

// Main
int main(int argc, char *argv[])
{
// This is a program to setup the display server.
// ps: The display server need to be is already running.

// #todo
// We need to grab the parameters,
// and send messages to the display server 
// at the end of the main routine.

    int client_fd = -1;

    printf("GWS.BIN: argc{%d}\n",argc);

/*
    if (argc >= 2){
        printf ("argv[0]: %s \n",argv[0]);
        printf ("argv[1]: %s \n",argv[1]);
    }
*/

/*
// Check signature.
    if( strncmp(argv[0],"GWS.BIN",7) == 0 )
    {
        fSignature=TRUE;
        printf("signature ok\n");
    }
*/

//================================
   
// Connection.
// Trying to connect with the window server.
// Only connect. Nothing more.
// #bugbug: 
// Is the window server already up and running?
// Will we stay in a loop until the connection is stablished?

    client_fd = (int) __gws_initialization();
    if (client_fd < 0){
         gws_debug_print("gws.bin: Initialization fail\n");
         printf         ("gws.bin: Initialization fail\n");
         exit(1);
    }

//========================================

//
// Setup window server based on flags.
//


// Kill server.
// Isso eh estranho... 
// um cliente remoto nao deve poder fazer isso.
    //gws_async_command(client_fd,1,0,0);

// Ping!
    //gws_async_command(client_fd,2,0,0);
// Hello!
    //gws_async_command(client_fd,3,0,0);

// Chamando um demo. (cat?)
// Only on (GRAMADO_JAIL).
    //gws_async_command(client_fd,4,9,0);

// Setup if we will show or not the 'fps window'.
    //gws_async_command(client_fd,6,TRUE,0);
    //gws_async_command(client_fd,6,FALSE,0);

// Shutdown the system
    //gws_async_command(client_fd,22,0,0);

// Quit the window server.
    //gws_async_command(client_fd,88,0,0);

// Reboot the system
    //gws_async_command(client_fd,89,0,0);


    printf ("Closing socket\n");
    close(client_fd);

// exit
    gws_debug_print("gws: bye :)\n");
    printf         ("gws: bye :)\n");

    return 0;
}

//
// End
//
