//CLIENT
// Creadits:
// https://mcalabprogram.blogspot.com/2012/01/udp-sockets-chat-application-server.html

#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>


//const char *ip = "127.0.0.1";
//#define PORT 43454
const char *ip = "192.168.1.4";
#define PORT  11888

#define MAX 80
#define SA  struct sockaddr


int main(int argc, char **argv)
{
    char buff[MAX];
    int sockfd,len,n;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd==-1){
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

    for (;;)
    {
        printf("\nEnter string : ");
        n=0;
        while ( (buff[n++]=getchar()) != '\n')
        {
        };

        if (n < MAX)
        {
            // Remove End Of Line.
            if (buff[n-1] == '\n')
                buff[n-1] = 0;
        }

        sendto (
            sockfd,
            buff,
            sizeof(buff), 
            0,
            (SA *)&servaddr,
            len );
        bzero(buff,sizeof(buff));
        recvfrom(sockfd,buff,sizeof(buff),0,(SA *)&servaddr,&len);
        printf("From Server : %s\n",buff);
        if (strncmp("exit",buff,4) == 0){
            printf("Client Exit...\n");
            break;
        }
    };

    close(sockfd);
}
