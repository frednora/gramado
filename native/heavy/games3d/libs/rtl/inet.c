
// inet.c
// Header in arpha/inet.h

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <rtl/gramado.h> 
#include <arpa/inet.h>

static uint32_t __htonl_imp(uint32_t hostlong);
static uint16_t __htons_imp(uint16_t hostshort);
static uint32_t __ntohl_imp(uint32_t netlong);
static uint16_t __ntohs_imp(uint16_t netshort);

// =====================================================

// Host to net long.
static uint32_t __htonl_imp(uint32_t hostlong)
{
    return (uint32_t) (((hostlong >> 24) & 0xFF) | 
                       ((hostlong << 8)  & 0xFF0000) | 
                       ((hostlong >> 8)  & 0xFF00) | 
                       ((hostlong << 24) & 0xFF000000));
}

// Host to net short.
static uint16_t __htons_imp(uint16_t hostshort)
{
    return (uint16_t) ((hostshort >> 8) | (hostshort << 8));
}

// Net to host long.
static uint32_t __ntohl_imp(uint32_t netlong)
{
    return (uint32_t) (((netlong >> 24) & 0xFF) | 
                       ((netlong << 8)  & 0xFF0000) | 
                       ((netlong >> 8)  & 0xFF00) | 
                       ((netlong << 24) & 0xFF000000));
}

// Net to host short.
static uint16_t __ntohs_imp(uint16_t netshort)
{
    return (uint16_t) ((netshort >> 8) | (netshort << 8));
}

// =====================================================

// Host to net long.
uint32_t htonl(uint32_t hostlong)
{
    return (uint32_t) __htonl_imp(hostlong);
}

// Host to net short.
uint16_t htons(uint16_t hostshort)
{
    return (uint16_t) __htons_imp(hostshort);
}

// Net to host long.
uint32_t ntohl(uint32_t netlong)
{
    return (uint32_t) __ntohl_imp(netlong);
}

// Net to host short.
uint16_t ntohs(uint16_t netshort)
{
    return (uint16_t) __ntohs_imp(netshort);
}

