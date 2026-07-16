// udp_directed_broadcast.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 11888
#define MAX  256

int main(void)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buff[MAX];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Use subnet broadcast instead of 255.255.255.255
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.255"); // directed broadcast

    strcpy(buff, "g:ip");

    if (sendto(sockfd, buff, strlen(buff), 0,
               (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("sendto failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Sent: %s\n", buff);

    socklen_t len = sizeof(servaddr);
    memset(buff, 0, sizeof(buff));
    if (recvfrom(sockfd, buff, sizeof(buff)-1, 0,
                 (struct sockaddr*)&servaddr, &len) < 0) {
        perror("recvfrom failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Reply: %s\n", buff);

    close(sockfd);
    return 0;
}
