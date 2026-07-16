// udp_broadcast_client.c
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

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Enable broadcast option
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
                   &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error enabling broadcast option");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Server address (broadcast)
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("255.255.255.255"); // broadcast

    // Prepare message
    strcpy(buff, "g:ip");

    // Send broadcast
    if (sendto(sockfd, buff, strlen(buff), 0,
               (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("sendto failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Broadcast sent: %s\n", buff);

    // Receive reply
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
