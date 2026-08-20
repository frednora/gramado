// inet.h
// Created by Fred Nora.

#ifndef __NET_INET_H
#define __NET_INET_H    1

//
// Macros
//

#define ToNetByteOrder16(v)  \
    ((v >> 8) | (v << 8))

#define ToNetByteOrder32(v)  \
    (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))

#define FromNetByteOrder16(v)  \
    ((v >> 8) | (v << 8))

#define FromNetByteOrder32(v)  \
    (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))

//===============================

// See: net/inet.c
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

#endif   
