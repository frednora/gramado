
// s1.c - socket test 1
// tutorial example taken from. 
// https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
 
/*
To make a process a TCP server, you need to follow the steps given below −
Create a socket with the socket() system call.
Bind the socket to an address using the bind() system call. 
* For a server socket on the Internet, an address consists of a 
* port number on the host machine.
Listen for connections with the listen() system call.
Accept a connection with the accept() system call. 
* This call typically blocks until a client connects with the server.
Send and receive data using the read() and write() system calls.
*/ 
 
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

int main( int argc, char *argv[] )
{
   int sockfd=0;
   int newsockfd=0; 
   int portno,=0;
   int clilen=0;
   char buffer[512];
   int n=0;

   struct sockaddr_in cli_addr;   
   printf("s1: Testing socket functions ...\n");

// Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        printf ("s1: ERROR opening socket \n");
        exit(1);
    }

// Address structure.
   struct sockaddr_in serv_addr; 

/* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;
     
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(portno);

// bind.
/* Now bind the host address using bind() call.*/

   if ( bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
   {
      printf ("ERROR on binding\n");
      exit(1);
   }

// listen.

/* 
 * Now start listening for the clients, here process will
 * go in sleep mode and will wait for the incoming connection
 */

   listen (sockfd,5);
   clilen = sizeof(cli_addr);

// accept
/* Accept actual connection from the client */

   newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }

// read.
/* If connection is established then start communicating */

   bzero(buffer,256);
   n = read( newsockfd,buffer,255 );
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   
   printf("Here is the message: %s\n",buffer);

// write. (response)
/* Write a response to the client */

   n = write(newsockfd,"I got your message",18);
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }

   printf("s1: done\n");
   return 0;
}

