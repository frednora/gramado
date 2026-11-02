
// main.c
// Gramado FTP Server.

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#define SERVER_BACKLOG  8
static int running=FALSE;

static void ServerInitialization(int value);
static void dispacher(int fd);

static void dispacher(int fd)
{
    if (fd<0)
        return;
    printf("ftpd: dispatcher\n");
}

static void ServerInitialization(int value)
{

    struct sockaddr server_address;
    socklen_t addrlen;
    server_address.sa_family = AF_GRAMADO;
    server_address.sa_data[0] = 'f';  // 'ft' - FTp server?
    server_address.sa_data[1] = 't';
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
    printf("ftpd.bin: Initializing ...\n");
    ServerInitialization(0);

    return (int) EXIT_SUCCESS;
}


