// Experimental http server

// rtl
#include <types.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/utsname.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


#define HTTP_PORT    22888
static void handle_connection(int connfd);

// ===============================================

static void handle_connection(int connfd)
{
    char buffer[1024];
    int n;

    // Clear buffer
    bzero(buffer, sizeof(buffer));

    // Read from the socket
    while (1){
        rtl_sleep_until(40000);
        n = read(connfd, buffer, sizeof(buffer)-1);
        //if (n < 0) {
        //    perror("read failed");
        //    return;
        //}
        if (n>0)
            break;
    }

    // Null-terminate and print
    buffer[n] = '\0';
    printf("HTTP.BIN: Received request:\n%s\n", buffer);

    // Optionally send a response
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello World!";
    write(connfd, response, strlen(response));

    // Close connection
    // close(connfd);
}


int main( int argc, char *argv[])
{
    struct sockaddr_in addr;
    socklen_t addrlen=0;
    int sockfd;

    printf("HTTPSRV: Hello from Gramado OS ring 3 server. Port=%d\n", 
        HTTP_PORT );

// Setup structure
    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    // IP:PORT
    //addr.sin_addr.s_addr = inet_addr("127.0.0.1")
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    //addr.sin_addr.s_addr = htonl(INADDR_ANY); 

    addr.sin_port = htons(HTTP_PORT);

    addrlen = sizeof(addr);

// Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0){
        printf("socket creation failed...\n");
        exit(0);
    } 

// Bind it
    int BindStatus = -1;
    BindStatus = bind(
        sockfd, 
        (struct sockaddr *) &addr, 
        sizeof(struct sockaddr_in) 
    );
    if (BindStatus < 0)
        exit(0);

// Listen
    listen(sockfd, 1);

// Accept
    printf("HTTPSRV: accepting ...\n");
    int IsTimeToQuit = FALSE;
    int newconn = -1;
    while (1){
        newconn = (int) accept( 
            sockfd, 
            (struct sockaddr *) &addr, 
            (socklen_t *) addrlen 
        );

        if (newconn > 0){
            printf("HTTP.BIN: newconn={%d} Accepted\n", newconn);
            handle_connection(newconn);
        }
    };

    printf("HTTP.BIN: done\n");
    return EXIT_SUCCESS;
}
