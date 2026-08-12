// Experimental http client
// Connects to the http.bin server, sends a GET, prints the response.

// rtl
#include <types.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/utsname.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


//#define HTTP_PORT     11888
#define HTTP_PORT 80

// #test: change to a remote IP to test tcp_client_connect()
//#define TARGET_IP     "127.0.0.1"
// Google
//#define TARGET_IP     "8.8.8.8"
#define TARGET_IP     "142.250.190.46"



static void do_request(int sockfd);

// ===============================================

static void do_request(int sockfd)
{
    char buffer[1024];
    int n;

    const char *request =
        "GET / HTTP/1.1\r\n"
        "Host: gramados\r\n"
        "Connection: close\r\n"
        "\r\n";

    // Send the request
    n = (int) write(sockfd, request, strlen(request));
    if (n < 0) {
        perror("write failed");
        return;
    }
    printf("HTTP_CLIENT.BIN: Sent %d bytes\n", n);

    // Clear buffer
    bzero(buffer, sizeof(buffer));

    // Read the response
    n = (int) read(sockfd, buffer, sizeof(buffer)-1);
    if (n < 0) {
        perror("read failed");
        return;
    }

    // Null-terminate and print
    buffer[n] = '\0';
    printf("HTTP_CLIENT.BIN: Received response:\n%s\n", buffer);
}


int main(int argc, char *argv[])
{
    struct sockaddr_in addr;
    int sockfd;
    const char *target_ip = TARGET_IP;

    // Allow overriding the target IP from the command line:
    // http_client.bin 203.0.113.7
    if (argc > 1){
        target_ip = argv[1];
    }

// Setup structure
    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    // IP:PORT
    addr.sin_addr.s_addr = inet_addr(target_ip);
    addr.sin_port = htons(HTTP_PORT);

// Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0){
        printf("HTTP_CLIENT.BIN: socket creation failed...\n");
        exit(0);
    }

    printf("HTTP_CLIENT.BIN: Connecting to %s:%d\n", target_ip, HTTP_PORT);

// Connect
    int ConnectStatus = -1;
    ConnectStatus = connect(
        sockfd,
        (struct sockaddr *) &addr,
        sizeof(struct sockaddr_in)
    );

    if (ConnectStatus < 0){
        printf("HTTP_CLIENT.BIN: connect failed\n");
        exit(0);
    }

    printf("HTTP_CLIENT.BIN: Connected! fd={%d}\n", sockfd);

// Wait for ever
    while(1){}

    // do_request(sockfd);

    // close(sockfd);

    printf("HTTP_CLIENT.BIN: done\n");
    return EXIT_SUCCESS;
}
