// gconsole.c
// Gramado Protocol Console
// Linux UDP client

#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVER_IP   "192.168.1.3"
#define SERVER_PORT 11888

#define BUFFER_SIZE 512

int main(void)
{
    int sockfd;
    socklen_t addrlen;

    struct sockaddr_in server;
    struct sockaddr_in from;

    char tx[BUFFER_SIZE];
    char rx[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    memset(&server,0,sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port   = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    addrlen = sizeof(from);

    printf("\n");
    printf("Gramado Protocol Console\n");
    printf("Server %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("\n");

    for (;;)
    {
        printf("gprot> ");
        fflush(stdout);

        memset(tx,0,sizeof(tx));

        if (fgets(tx,sizeof(tx),stdin) == NULL)
            break;

        /* remove '\n' */
        size_t len = strlen(tx);

        if (len > 0 && tx[len-1] == '\n')
        {
            tx[len-1] = 0;
            len--;
        }

        if (strcmp(tx,"quit") == 0)
            break;

        if (len == 0)
            continue;

        if (sendto(
                sockfd,
                tx,
                len + 1,
                0,
                (struct sockaddr *) &server,
                sizeof(server)) < 0)
        {
            perror("sendto");
            continue;
        }

        memset(rx,0,sizeof(rx));

        int n = recvfrom(
                    sockfd,
                    rx,
                    sizeof(rx)-1,
                    0,
                    (struct sockaddr *) &from,
                    &addrlen);

        if (n < 0)
        {
            perror("recvfrom");
            continue;
        }

        rx[n] = 0;

        printf("\n");
        printf("Reply from %s:%d\n",
            inet_ntoa(from.sin_addr),
            ntohs(from.sin_port));

        printf("%s\n",rx);
        printf("\n");
    }

    close(sockfd);

    return 0;
}
