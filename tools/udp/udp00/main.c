// CLIENT
// Creadits:
// https://mcalabprogram.blogspot.com/2012/01/udp-sockets-chat-application-server.html

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define TRUE  1
#define FALSE  0

//const char *ip = "127.0.0.1";
//#define PORT 43454

const char *ip = "192.168.1.4";
#define PORT  11888

#define SA  struct sockaddr

// A single line with 80 chars long.
#define MAX  80
static char buff[MAX];

static int isTimeToQuit = FALSE;

FILE *file_saved;
char file_buffer[512];

// ==============================================

int process_command(char *cmdline);

// ==============================================

int process_command(char *cmdline)
{
    char *p;

    if ((void*)cmdline == NULL)
        goto fail;
    if (*cmdline == 0)
        goto fail;

    // Show
    // printf("From Server : %s\n",buff);

    // ACK: Ignore it for now.
    if (strncmp("g:a",cmdline,3) == 0)
        goto done;

    // reply
    if (strncmp("g:1", cmdline,3) == 0)
    {
        p = (cmdline + 4); // Address where the response starts.
        //printf("%s\n",p);
        // Saving reply
        memset(file_buffer,0,512);
        sprintf(file_buffer,p);
        fwrite(file_buffer, sizeof(char), sizeof(file_buffer),file_saved);
        printf("%s\n",p);
        goto done;
    }

    // error
    if (strncmp("g:3", cmdline,3) == 0){
        printf("~ERROR\n");
        goto done;
    }

    // Process command
    if (strncmp("g:0 exit", cmdline,8) == 0){
        printf("exit: Client Exit ...\n");
        isTimeToQuit = TRUE;
        goto done;
    }
    goto fail;
done:
    return 0;
fail: 
    return (int) -1;
}

int main(int argc, char **argv)
{
    int sockfd, len, n;
    struct sockaddr_in  servaddr;

// Socket
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd == -1){
        printf("socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    };

    bzero(&servaddr,sizeof(len));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port=htons(PORT);
    len = sizeof(servaddr);

    FILE *file_r = fopen("received.txt", "w");
    file_saved = file_r;

// ---------------------
// Loop
    for (;;){

        if (isTimeToQuit == TRUE)
            break;

        printf("\n");
        printf("Enter string: ");
        bzero(buff,sizeof(buff));

        n=0;
        while ( (buff[n++] = getchar()) != '\n')
        {
        };

        // Remove End Of Line and finalize the string.
        if (n < MAX)
        {
            if (buff[n-1] == '\n')
            {
                buff[n-1] = 0;
            }
        }

        // Send
        sendto (
            sockfd,
            buff,
            sizeof(buff), 
            0,
            (SA *) &servaddr,
            len );

        // Receive
        bzero(buff,sizeof(buff));
        recvfrom(
            sockfd,
            buff,
            sizeof(buff),
            0, 
            (SA *) &servaddr, 
            &len );

        process_command(buff);
    };

//
    if (isTimeToQuit != TRUE){
        return EXIT_FAILURE;
    }

// 
    close(sockfd);
    return EXIT_SUCCESS;
}
