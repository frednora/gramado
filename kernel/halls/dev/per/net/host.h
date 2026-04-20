// host.h
// Host manager support.
// 2016 - Created by Fred Nora.

#ifndef __NET_HOST_H
#define __NET_HOST_H    1

/*
   exemplo
   
char *h_name

The real canonical host name.

char **h_aliases

A list of aliases that can be accessed with arrays the last element is NULL

int h_addrtype

The result's address type, which really should be AF_INET for our purposes.

int length

The length of the addresses in bytes, which is 4 for IP (version 4) addresses.

char **h_addr_list

A list of IP addresses for this host. Although this is a char**, 
it's really an array of struct in_addr*s in disguise. The last array element is NULL.

h_addr

A commonly defined alias for h_addr_list[0]. If you just want any old IP address
 for this host (yeah, they can have more than one) just use this field.
   
*/


/*
   exemplo
The C data structure used to represent addresses and hostnames within the networking API is the following:

 struct addrinfo {
    int     ai_flags;
    int     ai_family;
    int     ai_socktype;
    int     ai_protocol;
    size_t  ai_addrlen;
    struct  sockaddr* ai_addr;
    char*   ai_canonname;     // canonical name 
    struct  addrinfo* ai_next; // this struct can form a linked list 
};


  struct hostent {
            char   *h_name;
            char   **h_aliases;
            int    h_addrtype;
            int    h_length;
            char   **h_addr_list;
          };

struct addrinfo {
          int              ai_flags;  
          int              ai_family;
          int              ai_socktype; 
          int              ai_protocol;
          size_t           ai_addrlen;
          struct sockaddr *ai_addr;
          char            *ai_canonname;
          struct addrinfo *ai_next;
      };

struct sockaddr_in {
    short            sin_family;   //address family, set to AF_INET
    unsigned short   sin_port;     //the port in network byte order
    struct in_addr   sin_addr;     //the inet address
};	  
*/

//#test
#define HOST_DEFAULTNAME    "gramadox" 
#define HOST_NAME_MAX  64

//static char hostname_buffer[HOSTNAME_BUFFER_SIZE];

/*
 * host_info_d:
 *     The host structure.
 */

struct host_info_d
{
    object_type_t  objectType;
    object_class_t objectClass;

    int id;
    int used;
    int magic;

// N�mero identificador desse host dentro da rede interna.
    int hostIdentifier;

    char __hostname[HOST_NAME_MAX];
    size_t hostName_len;

    char *hostVersion;   // string mostrando a vers�o. ex: (1.1.1234)
    unsigned short hostVersionMajor;
    unsigned short hostVersionMinor; 
    unsigned short hostVersionRevision;
    int hostArchitecture; //identificador de tipo de arquitetura. ex: (x86)

// IP
    char hostIP[3];
    char *hostIPString;

// MAC
    char hostMAC[6];
    char *hostMACString;

    // ...
};

extern struct host_info_d *HostInfo;
//...

#endif    



//
// End.
//

