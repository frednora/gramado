
// main.c
// Gramado Web Server.

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

// #test
#include <rtl/gramado.h>
#include "packet.h"
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>

#include "actions.h"



#define SERVER_BACKLOG  8
static int running=FALSE;

// Flag: Send or not a reoply.
static int NoReply = FALSE;


#define GNS_BUFFER_SIZE  512
#define MSG_BUFFER_SIZE  512
char __buffer[GNS_BUFFER_SIZE];

// Tmp buffer for next response's metadata.
// This is the buffer ussed for the next response.
// Marsheling ...
// Esses valores serão enviados como
// resposta ao serviço atual.
// Eles são configurados pelo dialogo na hora da
// prestação do serviço.
// No início desse array fica o header.
// see: globals.h
unsigned long next_response[32];


static void ServerInitialization(int value);
static void dispacher(int fd);

// ===============================================


void service00(void)
{
    printf("service00: Hello!\n");
}


static void dispacher(int fd)
{
    unsigned long *message_buffer = (unsigned long *) &__buffer[0];
    int n_reads = 0;     // For requests.
    int n_writes = 0;    // For responses.

    int Status = -1;
    int SendErrorResponse=FALSE;
    int SendEvent=FALSE;

    // printf("webd: dispatcher\n");

    if (fd<0)
        return;

// Check if we have a new request.
    int value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
// Not a request.
    if (value != ACTION_REQUEST){
        //message_buffer[1] = 0;
        //gnssrv_yield();
        return;
    }

// =====================
// Read the request.
    n_reads = (int) read( fd, __buffer, sizeof(__buffer) );
    if (n_reads <= 0)
        return;


// #todo
// A local client will gonna use what protocol 
// to connect with this server? HTTP? Gramado protocols?

// #todo: 
// Call the procedure instead of executing the routine bellow.

    // GET /file.html .....

    //char* f = __buffer + 5;
    //*strchr(f, ' ') = 0;
    //int opened_fd = open(f, O_RDONLY);
    //int opened_fd = open(f, O_RDONLY,0);
    
    // #todo
    // Send the content of the requested file to the caller.
    // Copy the content of one file to another.
    // Gramado still don't have the function sendfile() in the library.
    // This way the caller will have the cantent of the file.

    // sendfile(fd, opened_fd, 0, 256);

    // Close stuff
    //close(opened_fd);
    //close(client_fd);
    //close(s);


    if (NoReply == TRUE){
        rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
        //message_buffer[0] = 0;
        return;
        //goto exit0;
    }

}




static void ServerInitialization(int value)
{

    struct sockaddr server_address;
    socklen_t addrlen;
    server_address.sa_family = AF_GRAMADO;
    server_address.sa_data[0] = 'w';  // 'we' - Web server?
    server_address.sa_data[1] = 'e';
    addrlen = sizeof(server_address);

    int server_fd = -1;
    int bind_status = -1;
    int newconn = -1;

// Create socket
    server_fd = (int) socket(AF_GRAMADO, SOCK_STREAM, 0);
    if (server_fd<0){
        printf("On creating socket\n");
        exit(0);
    }

// Bind
    bind_status = 
        (int) bind( server_fd, (struct sockaddr *) &server_address, addrlen );
    if (server_fd<0){
        printf("On binding\n");
        exit(0);
    }

// Listen
    listen(server_fd,SERVER_BACKLOG);

// Accepting connections.
    running = TRUE;
    while (running == TRUE)
    {
        newconn = 
            (int) accept( 
                server_fd, 
                (struct sockaddr *) &server_address, 
                (socklen_t) addrlen);

        if (newconn == 31){
            dispacher(newconn);
        }
    };
}

int main(int argc, char *argv[])
{
    printf("webd.bin: Initializing ...\n");
    ServerInitialization(0);

    return (int) EXIT_SUCCESS;
}


