// CLIENT
// Creadits:
// https://mcalabprogram.blogspot.com/2012/01/udp-sockets-chat-application-server.html

#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

// #hack
#define TRUE (1)
#define FALSE (0)

//const char *ip = "127.0.0.1";
//#define PORT 43454
const char *ip = "192.168.1.10";
#define PORT  11888

#define MAX  256  //80
static char buff[MAX];

#define SA struct sockaddr

int main(int argc, char **argv)
{
    int sockfd, len, n;
    struct sockaddr_in servaddr;
    int IsTimeToQuit = FALSE;

    memset(buff, 0, sizeof(buff));

    // It means UDP.
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd == -1){
        printf("socket creation failed...\n");
        goto fail;
    } else {
        printf("Socket successfully created..\n");
    };
    bzero(&servaddr,sizeof(len));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port=htons(PORT);
    len = sizeof(servaddr);

// Loop
    for (;;)
    {
        if (IsTimeToQuit)
            break;

        printf("\n");
        printf("Enter string : ");
        n=0;
        
        // Get string
        memset(buff, 0, sizeof(buff));
        while ( (buff[n++] = getchar()) != '\n')
        {
        };

        if (n < MAX)
        {
            // Remove End Of Line.
            if (buff[n-1] == '\n')
                buff[n-1] = 0;
        }

        // Send request
        sendto (
            sockfd,
            buff,
            sizeof(buff), 
            0,
            (SA *)&servaddr,
            len );
        
        // Read response
        bzero(buff,sizeof(buff));
        recvfrom(
            sockfd,
            buff,
            sizeof(buff),
            0,
            (SA *) &servaddr,
            &len );

        // Print response
        printf("From Server : %s\n",buff);

        // Compare response

        // g:0 = Request.
        if (buff[0] == 'g' &&
            buff[1] == ':' &&
            buff[2] == '0' )
        {
            if (strncmp("quit",(buff+4),4) == 0)
            {
                printf("udp01: ~quit\n");
                IsTimeToQuit = TRUE;
                break;
            }
            if (strncmp("exit",(buff+4),4) == 0)
            {
                printf("udp01: ~exit\n");
                IsTimeToQuit = TRUE;
                break;
            }
            // ...
        }
    };

    close(sockfd);
    return EXIT_SUCCESS;
fail:
    return EXIT_FAILURE;
}
