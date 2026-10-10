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

// see:
// https://man7.org/linux/man-pages/man3/inet_pton.3.html
int inet_pton(int af, const char *src, void *dst)
{
    printf ("inet_pton: [TODO]\n");
    return -1;
}

int inet_aton (const char *cp, struct in_addr *inp)
{
    printf ("inet_aton: [TODO]\n");
    return -1;
}

// see:
// https://pubs.opengroup.org/onlinepubs/009695399/functions/inet_addr.html
// #test
// implementation
in_addr_t inet_addr(const char *cp)
{
    in_addr_t addr = 0;
    int octet;
    char ch;
    int i;
    for (i = 0; i < 4; i++) {
        octet = 0;
        while (*cp && *cp != '.') {
            ch = *cp++;
            if (ch < '0' || ch > '9') {
                return INADDR_NONE; // Invalid character
            }
            octet = octet * 10 + (ch - '0');
        }
        if (octet > 255) {
            return INADDR_NONE; // Invalid octet
        }
        addr = (addr << 8) | octet;
        if (*cp) {
            cp++;
        }
    }
    return htonl(addr);
}

in_addr_t inet_network(const char *cp)
{
    printf ("inet_network: [TODO]\n");
    return (in_addr_t) 0;
}


// inet_ntoa:
// Convert network-format internet address
// to base 256 d.d.d.d representation.

char *inet_ntoa (struct in_addr in) 
{
/*
    static char buffer[32];
    inet_ntop(AF_INET, &in.s_addr, buffer, sizeof(buffer));
    return buffer;
 */

    printf ("inet_ntoa: [TODO]\n");
    return NULL;
}


// inet_makeaddr:
// Formulate an Internet address from network + host.  Used in
// building addresses stored in the ifnet structure.

struct in_addr inet_makeaddr ( in_addr_t net, in_addr_t host )
{
    struct in_addr ret;
    in_addr_t addr;

    printf("inet_makeaddr: [TODO] Not implemented!\n");

	/*
	if (net < 128)
		addr = (net << IN_CLASSA_NSHIFT) | (host & IN_CLASSA_HOST);
	else if (net < 65536)
		addr = (net << IN_CLASSB_NSHIFT) | (host & IN_CLASSB_HOST);
	else if (net < 16777216L)
		addr = (net << IN_CLASSC_NSHIFT) | (host & IN_CLASSC_HOST);
	else
		addr = net | host;
	ret.s_addr = htonl(addr);
	*/

    return (struct in_addr) ret;
}


// inet_lnaof:
// Return the local network address portion of an
// internet address; handles class a/b/c network number formats.

in_addr_t inet_lnaof(struct in_addr in)
{
    printf("inet_lnaof: [todo] Not implemented!\n");
    return 0;

   /*
	in_addr_t i = ntohl(in.s_addr);

	if (IN_CLASSA(i))
		return ((i)&IN_CLASSA_HOST);
	else if (IN_CLASSB(i))
		return ((i)&IN_CLASSB_HOST);
	else
		return ((i)&IN_CLASSC_HOST);

    */
}


// inet_netof: 
// Return the network number from an internet
// address; handles class a/b/c network #'s.

in_addr_t inet_netof (struct in_addr in)
{
    printf("inet_netof: [BUGBUG] Not implemented!\n");
    return 0;

   /*
	in_addr_t i = ntohl(in.s_addr);

	if (IN_CLASSA(i))
		return (((i)&IN_CLASSA_NET) >> IN_CLASSA_NSHIFT);
	else if (IN_CLASSB(i))
		return (((i)&IN_CLASSB_NET) >> IN_CLASSB_NSHIFT);
	else
		return (((i)&IN_CLASSC_NET) >> IN_CLASSC_NSHIFT);
   */

}




