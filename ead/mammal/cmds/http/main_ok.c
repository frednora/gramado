// Experimental http client for Gramado OS.
// Connects to the http.bin server, sends a GET, prints the response.
// Environment:
//     Ring 3.
// Created by Fred Nora

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

#include <rtl/gramado.h>

//
// Definitions and prototypes
//

// http://httpbin.org
#define __DEFAULT_TARGET_IP  "100.60.124.177"

// http://wttr.in/
//#define __DEFAULT_TARGET_IP  "5.9.243.187"

#define HTTP_PORT 80

static char __http_response_buffer[4096];

static void do_request(int sockfd);

// ===============================================

// We are already connected.
// Send a request and wait for a response.
static void do_request(int sockfd)
{
    size_t BufferSize = sizeof(__http_response_buffer);
    int fd_output = fileno(stdout);

// HTTP request
/*
    const char *request =
    "GET /html HTTP/1.1\r\n"
    "Host: httpbin.org\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
    "Accept: text/html\r\n"
    "Connection: close\r\n"
    "\r\n";
*/
    char request[1024];
    memset(request, 0, sizeof(request));

// Building the request:

    //strcat(request, "GET /index.html HTTP/1.1\r\n");
    strcat(request, "GET /status/500 HTTP/1.1\r\n");

    strcat(request, "Host: httpbin.org\r\n");
    //strcat(request, "Host: wttr.in\r\n");

    strcat(request,
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
    );
    strcat(request, "Accept: text/html\r\n");
    strcat(request, "Connection: close\r\n");
    strcat(request, "\r\n");

    // strcat(request, body);  // No body

    size_t RequestSize = strlen(request);

// ------------------------
// Send

    int w_count;
    w_count = (int) write(sockfd, request, RequestSize);
    if (w_count < 0) 
    {
        perror(":: write failed\n");
        if (w_count == (-107))  //ENOTCONN
        {
            printf("client: ENOTCONN Exited\n");
            exit(1);
        }
        return;
    }
    printf("HTTP_CLIENT.BIN: Sent %d bytes\n", w_count);


// ------------------------
// Receive

    memset( __http_response_buffer, 0, sizeof(__http_response_buffer) );

// #ps:
// We will hang we untill we get some data.
// This is temporary.
    int r_count;
    while (1) {
        rtl_sleep_until(40000);
        r_count = read(
                sockfd, 
                __http_response_buffer,
                sizeof(__http_response_buffer) );
        //if (r_count <= 0) 
            //break;   // EOF after FIN
        if (r_count > 0) 
            break;
    };
    __http_response_buffer[BufferSize -1] = '\0';


// ------------------------
// Show received message

    printf("Show message:\n");

// #ps: This is because maybe we still we have a limitation of 1KB 
// in the write() implementation for now.

    int total = 0;
    char *tmp_buf;
    while (total < BufferSize) 
    {
        int chunk = (BufferSize - total > 1024) ? 1024 : (BufferSize - total);
        tmp_buf = (__http_response_buffer + total); 
        int nw = write( fd_output, tmp_buf, chunk );
        if (nw <= 0) 
            break;
        total += nw;
    };

done:
    // Reset sync state so kernel can send more
    rtl_set_file_sync( sockfd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;

fail:
    // Reset sync state so kernel can send more
    rtl_set_file_sync( sockfd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
}


// IN: target IP (string format)
int main(int argc, char *argv[])
{
    struct sockaddr_in addr;
    int sockfd;
    const char *target_ip = __DEFAULT_TARGET_IP;
    int Try = 4;

// Allow overriding the target IP from the command line:
    if (argc > 1){
        target_ip = argv[1];
    }

// Setup structure
    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    // IP:PORT
    addr.sin_addr.s_addr = inet_addr(target_ip);
    addr.sin_port = htons(HTTP_PORT);

//
// Connection loop
//

    while (Try > 1)
    {
        // Create socket
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd < 0){
            printf("HTTP_CLIENT.BIN: socket creation failed...\n");
            exit(1);
        }
        // Reset the sync state
        rtl_set_file_sync(sockfd, SYNC_REQUEST_SET_ACTION, ACTION_NULL);

        printf("\n");
        printf("SOCKET fd={ %d } <<<< \n", sockfd);
        printf("HTTP_CLIENT.BIN [%d]: Connecting to %s:%d\n", 
            Try, target_ip, HTTP_PORT );

        if (sockfd < 0){
            printf("HTTP.BIN: sockfd exit()\n");
            exit(1);
        }

        // Connect
        // sync state: #test ACTION_CONNECTING. This is a new one.
        rtl_set_file_sync(sockfd, SYNC_REQUEST_SET_ACTION, 10000);
        int ConnectStatus = -1;
        ConnectStatus = connect(
            sockfd,
            (struct sockaddr *) &addr,
            sizeof(struct sockaddr_in) );

        if (ConnectStatus == 0){

            // Waiting the connection happens
            int ActionState = -1;
            while (1)
            {
                //rtl_yield(); // yield CPU
                ActionState = rtl_get_file_sync(sockfd, SYNC_REQUEST_GET_ACTION);
                // We are not connecting anymore
                if (ActionState != 10000)
                    break;
            };
            printf("HTTP_CLIENT.BIN: Connected! fd={%d} :)\n", sockfd);

            // We are fully connected now.
            // Lets call the request and get a response.
            do_request(sockfd);
            rtl_set_file_sync(sockfd, SYNC_REQUEST_SET_ACTION, ACTION_NULL);

            // #test:
            // It's simply closing the connection based on its status.
            close(sockfd);

        } else if (ConnectStatus < 0) {
            printf("HTTP_CLIENT.BIN: connect failed\n");
        }

        Try--;
    };  // end of while


    printf("HTTP_CLIENT.BIN: EXIT_SUCCESS\n");
    return EXIT_SUCCESS;
}
