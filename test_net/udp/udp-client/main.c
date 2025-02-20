// CLIENT
// Creadits:
// https://mcalabprogram.blogspot.com/2012/01/udp-sockets-chat-application-server.html


#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>


#define SA   struct sockaddr


const char *TARGET_IP = "192.168.1.4";
#define TARGET_PORT  11888

#define MAX 80

int main(int argc, char **argv)
{
    char buff[MAX];
    int sockfd, len, n;
    struct sockaddr_in  servaddr;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd < 0){
        printf("socket creation failed...\n");
        goto fail;
    } else {
        printf("Socket successfully created..\n");
    }

    bzero(&servaddr,sizeof(len));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(TARGET_IP);
    servaddr.sin_port = htons(TARGET_PORT);
    len = sizeof(servaddr);

    for (;;)
    {
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
        if (strncmp("exit",buff,4) == 0){
            printf("Client Exit...\n");
            break;
        }
    };

    close(sockfd);
    return 0;

fail:
    return 0;
}
