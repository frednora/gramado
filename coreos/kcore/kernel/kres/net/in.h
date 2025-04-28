// in.h 
// ring0. socket address for internet style.
// Created by Fred Nora.

#ifndef __NET_IN_H
#define __NET_IN_H    1

#define  _NETINET_IN_H_

// TCP and UDP
#define  __IPPROTO_TCP     6    // Transmission control protocol.
#define  __IPPROTO_UDP    17    // User datagram protocol.

#define  __IPPROTO_RAW    255   // Raw IP packet.

#define FACILITY_TCP_PORT  __IPPROTO_TCP
#define FACILITY_UDP_PORT  __IPPROTO_UDP

// Protocols

#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_HOPOPTS		0		/* IP6 hop-by-hop options */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_IGMP		2		/* group mgmt protocol */
#define	IPPROTO_GGP		3		/* gateway^2 (deprecated) */
#define	IPPROTO_IPV4		4 		/* IP header */
#define	IPPROTO_IPIP		4		/* IP inside IP */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_IDP		22		/* xns idp */
#define	IPPROTO_TP		29 		/* tp-4 w/ class negotiation */
#define	IPPROTO_DCCP		33		/* DCCP */
#define	IPPROTO_IPV6		41		/* IP6 header */
#define	IPPROTO_ROUTING		43		/* IP6 routing header */
#define	IPPROTO_FRAGMENT	44		/* IP6 fragmentation header */
#define	IPPROTO_RSVP		46		/* resource reservation */
#define	IPPROTO_GRE		47		/* GRE encaps RFC 1701 */
#define	IPPROTO_ESP		50 		/* encap. security payload */
#define	IPPROTO_AH		51 		/* authentication header */
#define	IPPROTO_MOBILE		55		/* IP Mobility RFC 2004 */
#define	IPPROTO_IPV6_ICMP	58		/* IPv6 ICMP */
#define	IPPROTO_ICMPV6		58		/* ICMP6 */
#define	IPPROTO_NONE		59		/* IP6 no next header */
#define	IPPROTO_DSTOPTS		60		/* IP6 destination option */
#define	IPPROTO_EON		80		/* ISO cnlp */
#define	IPPROTO_ETHERIP		97		/* Ethernet-in-IP */
#define	IPPROTO_ENCAP		98		/* encapsulation header */
#define	IPPROTO_PIM		103		/* Protocol indep. multicast */
#define	IPPROTO_IPCOMP		108		/* IP Payload Comp. Protocol */
#define	IPPROTO_VRRP		112		/* VRRP RFC 2338 */
#define	IPPROTO_CARP		112		/* Common Address Resolution Protocol */
#define	IPPROTO_L2TP		115		/* L2TPv3 */
#define	IPPROTO_SCTP		132		/* SCTP */
#define IPPROTO_PFSYNC      240     /* PFSYNC */
#define	IPPROTO_RAW		255		/* raw IP packet */
#define	IPPROTO_MAX		256

/* last return value of *_input(), meaning "all job for this pkt is done".  */
#define	IPPROTO_DONE		257

/* sysctl placeholder for (FAST_)IPSEC */
#define CTL_IPPROTO_IPSEC	258


/*
 * Local port number conventions:
 *
 * Ports < IPPORT_RESERVED are reserved for privileged processes (e.g. root),
 * unless a kernel is compiled with IPNOPRIVPORTS defined.
 *
 * When a user does a bind(2) or connect(2) with a port number of zero,
 * a non-conflicting local port address is chosen.
 *
 * The default range is IPPORT_ANONMIN to IPPORT_ANONMAX, although
 * that is settable by sysctl(3); net.inet.ip.anonportmin and
 * net.inet.ip.anonportmax respectively.
 *
 * A user may set the IPPROTO_IP option IP_PORTRANGE to change this
 * default assignment range.
 *
 * The value IP_PORTRANGE_DEFAULT causes the default behavior.
 *
 * The value IP_PORTRANGE_HIGH is the same as IP_PORTRANGE_DEFAULT,
 * and exists only for FreeBSD compatibility purposes.
 *
 * The value IP_PORTRANGE_LOW changes the range to the "low" are
 * that is (by convention) restricted to privileged processes.
 * This convention is based on "vouchsafe" principles only.
 * It is only secure if you trust the remote host to restrict these ports.
 * The range is IPPORT_RESERVEDMIN to IPPORT_RESERVEDMAX.
 */

#define	IPPORT_RESERVED		1024
#define IPPORT_USERRESERVED  5000  // User reserved port number.
#define	IPPORT_ANONMIN		49152
#define	IPPORT_ANONMAX		65535
#define	IPPORT_RESERVEDMIN	600
#define	IPPORT_RESERVEDMAX	(IPPORT_RESERVED-1)
// =============================================

//#test
typedef __uint32_t  __in_addr_t;  /* IP(v4) address */
typedef __uint16_t  __in_port_t;  /* "Internet" port number */


#ifndef in_addr_t
typedef __in_addr_t  in_addr_t;
#define in_addr_t  __in_addr_t
#endif

#ifndef in_port_t
typedef __in_port_t  in_port_t;
#define in_port_t  __in_port_t
#endif

typedef __uint8_t  __sa_family_t;  /* socket address family */


// #bugbug: 
// It needs to be 16bit.
#ifndef sa_family_t
typedef __sa_family_t  sa_family_t;
#define sa_family_t  __sa_family_t
#endif


// Internet address (a structure for historical reasons)
// #todo: checar a tipagem dos elementos 
/* address in network byte order */
struct in_addr 
{
    //in_addr_t  s_addr;
    uint32_t s_addr;
};


//#todo: checar a tipagem dos elementos  
// Socket address, internet style.
struct sockaddr_in {
    
    // len + family = 16bit
    uint8_t         sin_len;      // 8

    sa_family_t     sin_family;   /* 8 address family: AF_INET */
    in_port_t       sin_port;     /* port in network byte order */ 
    struct in_addr  sin_addr;     /* internet address */

    __int8_t        sin_zero[8];
};

#define  INET_ADDRSTRLEN    16

// internat address 'unsigned long' stuff

#define IN_CLASSA(i)         ((((unsigned long)(i))&0x80000000)==0)
#define IN_CLASSA_NET        0xff000000
#define IN_CLASSA_NSHIFT     24
#define IN_CLASSA_HOST       0x00ffffff

#define IN_CLASSB(i)         ((((unsigned long)(i))&0xc0000000)==0x80000000)
#define IN_CLASSB_NET        0xffff0000
#define IN_CLASSB_NSHIFT     16
#define IN_CLASSB_HOST       0x0000ffff

#define IN_CLASSC(i)         ((((unsigned long)(i))&0xc0000000)==0xc0000000)
#define IN_CLASSC_NET        0xffffff00
#define IN_CLASSC_NSHIFT     8
#define IN_CLASSC_HOST       0x000000ff

#define INADDR_ANY           0x00000000
#define INADDR_BROADCAST     0xffffffff

#endif    



