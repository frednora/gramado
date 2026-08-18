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

// http://example.com
//#define TARGET_IP  "93.184.215.14"
//#define HTTP_PORT 80

static void do_request(int sockfd);

// ===============================================

static void do_request(int sockfd)
{
// Bigger buffer so we can pull a decent chunk at a time

    const char *request =
    "GET /html HTTP/1.1\r\n"
    "Host: httpbin.org\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
    "Accept: text/html\r\n"
    "Connection: close\r\n"
    "\r\n";

    int r_number = (int) write(sockfd, request, strlen(request));
    if (r_number < 0) 
    {
        perror(":: write failed\n");
        if (r_number == (-107))  //ENOTCONN
        {
            printf("client: ENOTCONN Exited\n");
            exit(1);
        }
        return;
    }
    printf("HTTP_CLIENT.BIN: Sent %d bytes\n", r_number);

// ------------------------

    //while (1) {
    
        // Wait until kernel signals data ready
    while (rtl_get_file_sync(sockfd, SYNC_REQUEST_GET_ACTION) != ACTION_REPLY) {
        rtl_yield(); // yield CPU until reply arrives
    }

    // Read available data
    char buffer[1024];
    int n = read(sockfd, buffer, sizeof(buffer)-1);
    //if (n <= 0) 
        //break; // EOF or error

    if (n>0)
    {
        buffer[n] = '\0';
        printf("%s", buffer);
    }

    // Reset sync state so kernel can send more
    rtl_set_file_sync(sockfd, SYNC_REQUEST_SET_ACTION, ACTION_NULL);

    // }

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

    int Try = 8;

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


    while (Try > 1){

    printf("HTTP_CLIENT.BIN [%d]: Connecting to %s:%d\n", 
        Try, target_ip, HTTP_PORT );

    if (sockfd < 0){
       printf("HTTP.BIN: sockfd exit()\n");
       exit(1);
    }

// Connect
    int ConnectStatus = -1;
    ConnectStatus = connect(
        sockfd,
        (struct sockaddr *) &addr,
        sizeof(struct sockaddr_in)
    );

    if (ConnectStatus < 0){
        printf("HTTP_CLIENT.BIN: connect failed\n");
        //exit(0);
    }

    printf("HTTP_CLIENT.BIN: Connected! fd={%d}\n", sockfd);

// Wait for ever
    // while(1){}

    //while (1){
    int i=0;
    for (i=0; i<100; i++)
        rtl_yield;
    //rtl_sleep(5*1000);  // #bubug: Not working

        do_request(sockfd);

    //}

        Try--;

    };  // end of while

    // close(sockfd);

    printf("HTTP_CLIENT.BIN: done\n");
    return EXIT_SUCCESS;
}
