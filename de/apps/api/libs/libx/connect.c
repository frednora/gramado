// connect.c
// Use this code to connect with the display server.
// Created by Fred Nora.

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

//#bugbug
//#include <netdb.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

// libgws
#include "include/libx.h"  

int __gws__desktop__id=0;
int __ws__pid=0;
int ____gws_connected = 0;

// For read and write functions.
int ____gws_client_fd=0;      // socket fd
char ____gws_io_buffer[512];  // buffer


// gws_connect:
// #todo
// Return the fd.
// OUT: newfd = Ok.  0 = fail.

int gws_connect(void)
{
// Using AF_GRAMADO and 'ws' address.
// maybe we can accept some parameters.

    int fd=0;
    int Status=0;

    struct sockaddr addr;
    addr.sa_family = 8000;  // AF_GRAMADO
    addr.sa_data[0] = 'w';
    addr.sa_data[1] = 's';

// socket
// Create the socket.

    // #debug
    printf ("libgws-gws_connect: Creating socket\n");
    fd = socket( 8000, SOCK_STREAM, 0 );
    if ( fd < 0 ){
       gws_debug_print ("libgws-gws_connect: Couldn't create socket\n");
       goto fail;
    }

// connect

    Status = 
        connect ( 
            fd, 
            (struct sockaddr *) &addr, 
            sizeof(addr) );

    if (Status < 0){ 
        gws_debug_print ("libgws-gws_connect: Connection Failed\n"); 
        close(fd);
        goto fail;
    }    

// done
// Saving
// Return fd.
    ____gws_connected = Status;
    ____gws_client_fd = fd;
    return (int) fd;

fail:
    ____gws_connected = 0;  // Disconnected
    ____gws_client_fd = 0;  // fail
    return (int)(-1);       // Error. fd.
}


//#todo
/*
// Disconnect from server.
void gws_disconnect (int fd);
void gws_disconnect (int fd)
{
    close(fd);
}
*/

int gws_get_connect_status(void)
{
    return (int) ____gws_connected;
}

void gws_set_connect_status(int status)
{
    ____gws_connected = (int) status;
}

int gws_get_client_fd(void)
{
    return (int) ____gws_client_fd;
}

void gws_set_client_fd(int client_id)
{
    ____gws_client_fd = (int) client_id;
}

int gws_get_desktop_id(void)
{
    return (int) __gws__desktop__id;
}

void gws_set_desktop_id(int desktop_id)
{
    __gws__desktop__id = (int) desktop_id;
}

// gws_initialize_connection:
int gws_initialize_connection(void)
{
// Called by gws_initialize_library() in gws.c

    unsigned long Value=0;

// Pega o id do desktop atual.
// ?? Ou o qual o processo pertence?

    Value = (unsigned long) gws_system_call (519,0,0,0);
    __gws__desktop__id = (int) (Value & 0xFFFFFFFF); 

    if (__gws__desktop__id < 0){
    // #todo        
    }

// Get ws PID for a given desktop.

     Value = 
         (unsigned long) gws_system_call ( 
             512,
             (unsigned long) __gws__desktop__id,
             (unsigned long) __gws__desktop__id,
             (unsigned long) __gws__desktop__id );    

    __ws__pid = (int) (Value & 0xFFFFFFFF);

    if (__ws__pid < 0)
    {
        // Message?
        return (int) (-1);
    }

// #bugbug
// O window server nao esta processando mensgens do sistema.
// Ele apenas pega as mensagens de sistema quando solicitadas
// pelos clientes.

// Send message.
// Envia uma mensagem pedindo para o ws emitir um hello!
// msg = 1000;

// vamos criar uma chamada semelhante a essa.

    // IN: PID, window, msg, long1, long2
    //gws_send_message_to_process ( __ws__pid, 
    //    0, 1000, 0, 0 );  
    
    // put some pixel in this position.
    //gws_send_message_to_process ( __ws__pid, 
    //    0, 2000, 40, 40 );  

    // refresh screen
    //gws_send_message_to_process ( __ws__pid, 
    //    0, 2020, 0, 0 );     

    return (int) __ws__pid;
}

// #todo: maybe a pointer.
unsigned long gws_get_message_buffer (void)
{
    return (unsigned long) &____gws_io_buffer[0];
    //return (unsigned long) ____gws_io_buffer;
}

