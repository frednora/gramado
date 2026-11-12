
// arpa/inet.h
// Functions for manipulating numeric IP addresses.
// See:
// https://en.wikipedia.org/wiki/Berkeley_sockets

#ifndef _ARPA_INET_H
#define _ARPA_INET_H    1

/*
Description
The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
The ntohl() function converts the unsigned integer netlong from network byte order to host byte order.
The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order.  
 */

// little endian
//#define ToNetByteOrder16(v) ((v >> 8) | (v << 8))
//#define ToNetByteOrder32(v) (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))
//#define FromNetByteOrder16(v) ((v >> 8) | (v << 8))
//#define FromNetByteOrder32(v) (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))

// See: inet.c
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);


#endif    

