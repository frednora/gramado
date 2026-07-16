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

// #hackhack
#define TRUE  1
#define FALSE  0

#define SA   struct sockaddr

const char *TARGET_IP = "192.168.1.10";
#define TARGET_PORT  11888

#define MAX 80

static int isTimeToQuit = FALSE;

int main(int argc, char **argv)
{
    static char buff[MAX];
    int sockfd=0; 
    int len=0; 
    int n;
    struct sockaddr_in  servaddr;

    sockfd = (int) socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd < 0){
        printf("on socket()\n");
        goto fail;
    } else {
        printf("Socket successfully created..\n");
    }

    bzero(&servaddr,sizeof(len));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(TARGET_IP);
    servaddr.sin_port = htons(TARGET_PORT);
    len = sizeof(servaddr);

    isTimeToQuit = FALSE;
    for (;;){
	if (isTimeToQuit == TRUE)
	    break;
        printf("\nEnter string : ");
        // Clean buffer
        memset(buff,0,MAX);
        // Get string.
        n=0;
        while ( (buff[n++] = getchar()) != '\n')
        {
        };

        if (n < MAX)
        {

            // Remove the EOL.
            if (buff[n-1] == '\n')
                buff[n-1] = 0;

            // Send
            sendto (
                sockfd,
                buff,
                sizeof(buff), 
                0,
                (SA *)&servaddr,
                len );
        }

        bzero(buff,sizeof(buff));

        // Receive
        recvfrom(sockfd,buff,sizeof(buff),0,(SA *)&servaddr,&len);
        printf("From Server : %s\n",buff);
        
        // Compare
	// g:0 means (REQUEST)
        if (strncmp("g:0 exit",buff,8) == 0)
	{
            isTimeToQuit = TRUE;
            printf("Client Exit...\n");
            break;
        }
	// ...
    };

    if (isTimeToQuit == TRUE){
        close(sockfd);
        return EXIT_SUCCESS;
    }

fail:
    return EXIT_FAILURE;
}
