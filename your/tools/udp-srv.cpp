// Credits:
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/
// g++ udp-srv.cpp -o udp-srv

// Server side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
   
//#define PORT  8080 
#define PORT  11888
#define MAXLINE  1024 


int main(int argc, char **argv)
{ 
    int sockfd=0;
    char buffer[MAXLINE]; 
    const char *hello = "Hello from server"; 
    struct sockaddr_in servaddr, cliaddr; 

    socklen_t len;
    int n=0;


// Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
       
// Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 

// Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
    {
        perror("bind failed"); 
        goto fail;
    } 

    len = sizeof(cliaddr);  //len is value/result 

// Receive
    n = recvfrom( 
            sockfd, 
            (char *)buffer, 
            MAXLINE,  
            MSG_WAITALL, 
            (struct sockaddr *) &cliaddr, 
            &len );
    buffer[n] = '\0'; 
    printf("Client : %s\n", buffer);

// Send
    sendto(
        sockfd, 
        (const char *)hello, 
        strlen(hello),  
        MSG_CONFIRM, 
        (const struct sockaddr *) &cliaddr, 
        len ); 
    std::cout<<"Hello message sent."<<std::endl;  
       
    return 0;

fail:
    return EXIT_FAILURE;
}

