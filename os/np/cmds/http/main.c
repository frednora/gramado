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

#include <rtl/gramado.h>


// http://httpbin.org
#define TARGET_IP  "100.60.124.177"
#define HTTP_PORT 80

static void do_request(int sockfd);

// ===============================================

static void do_request(int sockfd)
{
    char buffer[512];
    int n;

    const char *request =
    "GET /html HTTP/1.1\r\n"
    "Host: httpbin.org\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
    "Accept: text/html\r\n"
    "Connection: close\r\n"
    "\r\n";

    // Clear
    memset(buffer, 0, sizeof(buffer));    

    // Send the request
    n = (int) write(sockfd, request, strlen(request));
    if (n < 0) {
        perror(":: write failed");
        return;
    }

    printf("HTTP_CLIENT.BIN: Sent %d bytes\n", n);

    //#provisory
    //return;

    // Clear buffer
    bzero(buffer, sizeof(buffer));


    
// #test
    int Value=0;

    //rtl_set_file_sync( sockfd, SYNC_REQUEST_SET_ACTION, ACTION_REQUEST );

// Response
// Waiting to read the response.
    //gws_debug_print("gws_draw_char: response\n");
    while (1){
        rtl_yield(); 
        rtl_yield(); 
        rtl_yield(); 
        rtl_yield(); 
        Value = rtl_get_file_sync( sockfd, SYNC_REQUEST_GET_ACTION );
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_REPLY ) { break; }
        if (Value == ACTION_ERROR ) { goto done; }
        if (Value == ACTION_NULL )  { goto done; }  // No reponse
        // REMOVED: "if (Value == ACTION_NULL) { goto done; }"
        // Instead of giving up, we let the CPU loop/yield until ACTION_REPLY is set by the driver
    };


    // Read the response
    n = (int) read(sockfd, buffer, sizeof(buffer)-1);
    if (n < 0) {
        perror("read failed");
        return;
    }

    // Null-terminate and print
    buffer[n] = '\0';
    printf("HTTP_CLIENT.BIN: Received response:\n%s\n", buffer);

done:
    rtl_set_file_sync( sockfd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
fail:
    rtl_set_file_sync( sockfd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return;
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
    // while(1){}

    int i=0;
    while (1){
    for (i=0; i<100; i++)
        rtl_yield;
    rtl_sleep(5*1000);  // #bubug: Not working
    do_request(sockfd);
    }

    // close(sockfd);

    printf("HTTP_CLIENT.BIN: done\n");
    return EXIT_SUCCESS;
}
