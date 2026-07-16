// gprot_client.c
// Simple UDP sender/receiver for testing Gramado gprot

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define LOCAL_PORT 50000
#define TARGET_PORT 11888

int main()
{
    int sock;
    struct sockaddr_in local_addr;
    struct sockaddr_in target_addr;
    char buffer[512];

    // 1. Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 2. Bind to a fixed local port (IMPORTANT)
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(LOCAL_PORT);

    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind");
        return 1;
    }

    // 3. Setup target (Gramado OS)
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(TARGET_PORT);
    target_addr.sin_addr.s_addr = inet_addr("192.168.0.10"); // change this

    // 4. Send gprot request
    //strcpy(buffer, "g:0 hello from client");
    strcpy(buffer, "g:3");

    sendto(sock,
           buffer,
           strlen(buffer),
           0,
           (struct sockaddr*)&target_addr,
           sizeof(target_addr));

    printf("sent: %s\n", buffer);

    // 5. Wait for reply
    socklen_t addrlen = sizeof(target_addr);
    memset(buffer, 0, sizeof(buffer));

    int n = recvfrom(sock,
                     buffer,
                     sizeof(buffer)-1,
                     0,
                     (struct sockaddr*)&target_addr,
                     &addrlen);

    if (n > 0) {
        buffer[n] = 0;
        printf("received: %s\n", buffer);
    } else {
        printf("no response\n");
    }

    close(sock);
    return 0;
}
