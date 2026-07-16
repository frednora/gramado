// Broadcast message asking for the server's IP.

/*
Below is an example of such a worker function. This function sends a 
broadcast request using your gprot command "g:ip?" and waits for a reply. 
It then parses the reply (expected to be in the form "g:ip <server_ip>"), 
stores the discovered IP in the caller’s variables, and also updates 
the server address so that subsequent communications go to the correct destination.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

// Function: obtain_server_ip
// Description:
//   - Configures the socket for broadcasting.
//   - Sends a broadcast message that asks for the server's IP ("g:ip?").
//   - Waits for the server's response and extracts the IP address from it.
//   - Updates 'server_ip' (a provided buffer) and the 'servaddr' structure accordingly.
//
// Parameters:
//   sockfd     - the UDP socket descriptor
//   servaddr   - pointer to the server address structure (used for port, etc.)
//   server_ip  - output buffer to store the discovered server IP (as a string)
//   ip_len     - length of the server_ip buffer
//
// Returns:
//   0 on success, -1 on failure.
int obtain_server_ip(int sockfd, struct sockaddr_in *servaddr, char *server_ip, size_t ip_len) {
    int ret;
    
    // Enable broadcast option on the socket.
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("setsockopt (SO_BROADCAST) failed");
        return -1;
    }
    
    // Prepare broadcast address structure.
    // Using the generic broadcast address: 255.255.255.255.
    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = servaddr->sin_port;  // Use the same port as defined in servaddr.
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    // Prepare the broadcast message.
    const char *broadcast_msg = "g:ip?";

    // Send the broadcast message.
    ret = sendto(sockfd, broadcast_msg, strlen(broadcast_msg), 0,
                 (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));
    if (ret < 0) {
        perror("sendto (broadcast) failed");
        return -1;
    }
    
    // Wait for a response.
    // Note: In a full implementation, you might want to add timeouts and retries.
    char recv_buf[128];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    ret = recvfrom(sockfd, recv_buf, sizeof(recv_buf) - 1, 0,
                   (struct sockaddr *)&sender_addr, &addr_len);
    if (ret < 0) {
        perror("recvfrom (waiting for IP response) failed");
        return -1;
    }
    recv_buf[ret] = '\0';  // Ensure we have a null-terminated string.

    // Expected response format: "g:ip <server_ip>"
    char prefix[6];  // Enough to hold "g:ip" plus null.
    char ip_received[64];
    if (sscanf(recv_buf, "%5s %63s", prefix, ip_received) != 2 || strcmp(prefix, "g:ip") != 0) {
        fprintf(stderr, "Invalid response received: %s\n", recv_buf);
        return -1;
    }
    
    // Copy the discovered IP address into the provided buffer.
    strncpy(server_ip, ip_received, ip_len);
    server_ip[ip_len - 1] = '\0';  // Ensure null termination.

    // Update your server address structure with the discovered IP.
    if (inet_aton(server_ip, &servaddr->sin_addr) == 0) {
        fprintf(stderr, "inet_aton conversion failed for IP: %s\n", server_ip);
        return -1;
    }

    return 0;
}

/*
How to Use This Function in Your main()
Before entering your communication loop, call the worker function:
*/

int main(int argc, char **argv) {
    int sockfd, n;
    struct sockaddr_in servaddr;
    
    // Create a UDP socket.
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created.\n");
    
    // Initialize the server address structure.
    memset(&servaddr, 0, sizeof(servaddr)); // Zero out the structure to be safe.
    servaddr.sin_family = AF_INET;
    // Initially, any IP works; it will be updated by our worker function.
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);  // Use your defined port.

    // Worker function call to obtain the server's IP.
    char server_ip[64];
    if (obtain_server_ip(sockfd, &servaddr, server_ip, sizeof(server_ip)) == 0) {
        printf("Server discovered at IP: %s\n", server_ip);
    } else {
        fprintf(stderr, "Failed to obtain server IP.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Proceed with the rest of your client communication loop...
    
    // Remember to close the socket when done.
    close(sockfd);
    return EXIT_SUCCESS;
}

/*
Some Additional Thoughts
Timeouts and Retries: 
Since UDP doesn’t guarantee delivery, you may want to set a timeout on the socket for recvfrom() (using the SO_RCVTIMEO socket option) and implement a retry mechanism if no response is received within the expected time limit.
Security Considerations: 
Even in a hobby project, consider protecting the broadcast (for example by expecting a known token in responses) to ensure you’re processing responses from your intended OS only.
Extensibility: 
With a discovery mechanism in place, you can extend the same logic for finding other services or managing additional dynamic configurations, much like SSDP or Zeroconf services.
*/


