// simple_echo_server.c
// simple server created by Copilot for Gramado OS.
// Not tested yet.

/*
Given that your hobby OS supports creating simple servers 
using Unix-like functions for sockets with the family AF_GRAMADO, 
let's create a simple server that listens for incoming connections and 
echoes back any received messages.
Here is an example of a simple echo server:
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

// Define the buffer size and port number
#define BUFFER_SIZE 1024
#define PORT 12345

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Create a socket
    server_fd = socket(AF_GRAMADO, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup the server address
    server_addr.sin_family = AF_GRAMADO;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections and handle them
    while (1) {
        addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("Client connected...\n");

        // Receive and echo back messages
        while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) {
            buffer[bytes_received] = '\0';
            printf("Received: %s\n", buffer);
            send(client_fd, buffer, bytes_received, 0);
        }

        if (bytes_received < 0) {
            perror("recv");
        }

        printf("Client disconnected...\n");
        close(client_fd);
    }

    // Close the server socket
    close(server_fd);
    return 0;
}

