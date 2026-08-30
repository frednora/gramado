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


#define RESPONSE_BUFFER_SIZE 1024

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
        rtl_sleep_until(20000);
        n = read(connfd, buffer, sizeof(buffer)-1);
        if (n <= 0) {
            //perror("HTTPSRV.BIN: read failed\n");
            //printf("HTTPSRV.BIN: read failed\n");
            //return;
        }
        if (n>0)
            break;
    }

    // Null-terminate and print
    buffer[n] = '\0';
    printf("HTTPSRV.BIN: Received request:\n%s\n", buffer);

    /*
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello World!";
    */

/*
    const char *response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 85\r\n"
    "\r\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head><title>Gramado Server</title></head>\n"
    "<body><h1>Hello from Gramado OS!</h1></body>\n"
    "</html>\n";
*/

// --- HTML body ---
    const char *body =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>Gramado Server</title></head>\n"
        "<body>\n"
        "<h1>Hello from Gramado OS!</h1>\n"
        "<p>This is a tiny HTML page served from ring 3.</p>\n"
        "</body>\n"
        "</html>\n";
    int body_len = strlen(body);

    // --- Response buffer ---
    char response[RESPONSE_BUFFER_SIZE];
    bzero(response, sizeof(response));

    // Headers
    strcat(response, "HTTP/1.1 200 OK\r\n");
    strcat(response, "Server: Gramado/0.1\r\n");
    strcat(response, "Content-Type: text/html; charset=utf-8\r\n");
    char lenbuf[32];
    sprintf(lenbuf, "%d", body_len);
    strcat(response, "Content-Length: ");
    strcat(response, lenbuf);
    strcat(response, "\r\n");
    strcat(response, "Connection: close\r\n");
    strcat(response, "\r\n");

    // Body
    strcat(response, body);

// Send
    write(connfd, response, strlen(response));

    // Close connection
    // close(connfd);
}


int main( int argc, char *argv[])
{
    struct sockaddr_in addr;
    socklen_t addrlen=0;
    int sockfd;

    printf("HTTPSRV.BIN: Hello from Gramado OS ring 3 server. Port=%d\n", 
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
    printf("HTTPSRV.BIN: accepting ...\n");
    int IsTimeToQuit = FALSE;
    int newconn = -1;
    while (1){
        newconn = (int) accept( 
            sockfd, 
            (struct sockaddr *) &addr, 
            (socklen_t *) addrlen 
        );

        if (newconn > 0){
            //printf("HTTPSRV.BIN: newconn={%d} Accepted\n", newconn);
            handle_connection(newconn);
        }
    };

    printf("HTTPSRV.BIN: done\n");
    return EXIT_SUCCESS;
}

