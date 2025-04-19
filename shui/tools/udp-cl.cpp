// Credits:
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/
// g++ udp-cl.cpp -o udp-cl

// Client side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 


// IP:Port for the server.
const char *ServerIPString = "192.168.1.6";
#define PORT  11888

#define MAXLINE  1024 


int main(void) 
{ 
    int sockfd; 
    char buffer[MAXLINE]; 
    const char *hello = "Hello from client"; 
    struct sockaddr_in     servaddr; 
   
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    memset(&servaddr, 0, sizeof(servaddr)); 
 
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    //servaddr.sin_addr.s_addr = INADDR_ANY; 
    //servaddr.sin_addr.s_addr = inet_addr("192.168.1.4");
    servaddr.sin_addr.s_addr = inet_addr(ServerIPString);

    int n=0;
    socklen_t len; 
       
    sendto(
        sockfd, 
        (const char *)hello, strlen(hello), 
        MSG_CONFIRM, 
        (const struct sockaddr *) &servaddr,  
        sizeof(servaddr) ); 

    std::cout << "Hello message sent." << std::endl; 

    n = recvfrom(
            sockfd, 
            (char *) buffer, 
            MAXLINE,  
            MSG_WAITALL, 
            (struct sockaddr *) &servaddr, 
            &len );
  
    buffer[n] = '\0'; 
    std::cout<<"Server :"<<buffer<<std::endl; 
   
    close(sockfd); 

    return 0; 
}

