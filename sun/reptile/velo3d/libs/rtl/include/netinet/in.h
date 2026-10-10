/*
 * File: in.h
 * 
 *   Socket support for internet style. 
 */

// Internet address family
// AF_INET and AF_INET6 address families and 
// their corresponding protocol families, 
// PF_INET and PF_INET6. 
// These include standard IP addresses and TCP and UDP port numbers.
// See:
// https://en.wikipedia.org/wiki/Berkeley_sockets

/*	$NetBSD: in.h,v 1.108 2018/11/09 11:46:28 maya Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)in.h	8.3 (Berkeley) 1/3/94
 */

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981, and numerous additions.
 */


#ifndef _NETINET_IN_H_
#define _NETINET_IN_H_


#include <sys/ansi.h> 


#define  _NETINET_IN_H_



//======================

// TCP and UDP
#define  __IPPROTO_TCP     6    // Transmission control protocol.
#define  __IPPROTO_UDP    17    // User datagram protocol.

#define  __IPPROTO_RAW          255   // Raw IP packet.

#define FACILITY_TCP_PORT  __IPPROTO_TCP
#define FACILITY_UDP_PORT  __IPPROTO_UDP



// =============================================
/*
 * Protocols
 */
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

/*
#define MACPY(s, d) memcpy(d, s, ETHER_ADDR_LEN)

#define MAXTMO 20	//seconds 
#define MINTMO 2	//seconds 

#define FNAME_SIZE 128
#define	IFNAME_SIZE 16
#define RECV_SIZE 1536	//XXX delete this 
*/




#ifndef _BSD_UINT8_T_
typedef __uint8_t  uint8_t;
#define _BSD_UINT8_T_
#endif


#ifndef _BSD_UINT32_T_
typedef __uint32_t  uint32_t;
#define _BSD_UINT32_T_
#endif


#ifndef in_addr_t
typedef __in_addr_t in_addr_t;
#define in_addr_t  __in_addr_t
#endif


#ifndef in_port_t
typedef __in_port_t in_port_t;
#define in_port_t  __in_port_t
#endif


#ifndef sa_family_t
typedef __sa_family_t  sa_family_t;
#define sa_family_t  __sa_family_t
#endif



// Internet address (a structure for historical reasons)
struct in_addr {
    in_addr_t s_addr;    /* address in network byte order */
} __packed;

// sockaddr_in:
//     Socket address, internet style.
struct sockaddr_in {

    // len + family = 16bit
    uint8_t           sin_len;      // 8

    sa_family_t       sin_family;   /* 8 address family: AF_INET */
    in_port_t         sin_port;     /* port in network byte order */ 
    struct in_addr    sin_addr;     /* internet address */

    __int8_t          sin_zero[8];
};

#define  INET_ADDRSTRLEN    16

// internat address 'unsigned long' stuff

#define   IN_CLASSA(i)         ((((unsigned long)(i))&0x80000000)==0)
#define   IN_CLASSA_NET        0xff000000
#define   IN_CLASSA_NSHIFT     24
#define   IN_CLASSA_HOST       0x00ffffff

#define   IN_CLASSB(i)         ((((unsigned long)(i))&0xc0000000)==0x80000000)
#define   IN_CLASSB_NET        0xffff0000
#define   IN_CLASSB_NSHIFT     16
#define   IN_CLASSB_HOST       0x0000ffff

#define   IN_CLASSC(i)         ((((unsigned long)(i))&0xc0000000)==0xc0000000)
#define   IN_CLASSC_NET        0xffffff00
#define   IN_CLASSC_NSHIFT     8
#define   IN_CLASSC_HOST       0x000000ff

#define   INADDR_ANY           0x00000000
#define   INADDR_BROADCAST     0xffffffff


/*
 * Structure used to describe IP options.
 * Used to store options internally, to pass them to a process,
 * or to restore options retrieved earlier.
 * The ip_dst is used for the first-hop gateway when using a source route
 * (this gets put into the header proper).
 */

/*
struct ip_opts {

    struct in_addr  ip_dst;       // first hop, 0 w/o src rt 
    __int8_t        ip_opts[40];  // actually variable in size 
};
*/



struct ip_opts {
    struct  in_addr ip_dst;     //first hop, 0 w/o src rt 
    char    ip_opts[40];        //actually variable in size 
};




/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
 
#define	IP_OPTIONS      1  /* buf/ip_opts; set/get IP options */
#define	IP_HDRINCL      2  /* int; header is included with data */
#define	IP_TOS          3  /* int; IP type of service and preced. */
#define	IP_TTL          4  /* int; IP time to live */
#define	IP_RECVOPTS     5  /* bool; receive all IP opts w/dgram */
#define	IP_RECVRETOPTS  6  /* bool; receive IP opts for response */
#define	IP_RECVDSTADDR  7  /* bool; receive IP dst addr w/dgram */
#define	IP_RETOPTS      8  /* ip_opts; set/get IP options */
#define	IP_MULTICAST_IF 9  /* in_addr; set/get IP multicast i/f  */
#define	IP_MULTICAST_TTL    10  /* u_char; set/get IP multicast ttl */
#define	IP_MULTICAST_LOOP   11  /* u_char; set/get IP multicast loopback */
/* The add and drop membership option numbers need to match with the v6 ones */
#define	IP_ADD_MEMBERSHIP   12  /* ip_mreq; add an IP group membership */
#define	IP_DROP_MEMBERSHIP  13  /* ip_mreq; drop an IP group membership */
#define	IP_PORTALGO         18  /* int; port selection algo (rfc6056) */
#define	IP_PORTRANGE        19  /* int; range to use for ephemeral port */
#define	IP_RECVIF        20  /* bool; receive reception if w/dgram */
#define	IP_ERRORMTU      21  /* int; get MTU of last xmit = EMSGSIZE */
#define	IP_IPSEC_POLICY  22  /* struct; get/set security policy */
#define	IP_RECVTTL       23  /* bool; receive IP TTL w/dgram */
#define	IP_MINTTL        24  /* minimum TTL for packet or drop */
#define	IP_PKTINFO       25  /* struct; set default src if/addr */
#define	IP_RECVPKTINFO   26  /* int; receive dst if/addr w/dgram */

#define IP_SENDSRCADDR IP_RECVDSTADDR /* FreeBSD compatibility */ 
 
 
/*
 * Information sent in the control message of a datagram socket for
 * IP_PKTINFO and IP_RECVPKTINFO.
 */


struct in_pktinfo {

    struct in_addr  ipi_addr;     //src/dst address 
    unsigned int    ipi_ifindex;  //interface index 
};



/*
 * Defaults and limits for options
 */
 
#define	IP_DEFAULT_MULTICAST_TTL   1   /* normally limit m'casts to 1 hop  */
#define	IP_DEFAULT_MULTICAST_LOOP  1   /* normally hear sends if a member  */
#define	IP_MAX_MEMBERSHIPS         20  /* per socket; must fit in one mbuf */


/*
 * Argument structure for IP_ADD_MEMBERSHIP and IP_DROP_MEMBERSHIP.
 */
 

struct ip_mreq {
    struct in_addr  imr_multiaddr;  // IP multicast address of group 
    struct in_addr  imr_interface;  // local IP address of interface 
};



/*
 * Argument for IP_PORTRANGE:
 * - which range to search when port is unspecified 
 *   at bind() or connect()
 */

#define IP_PORTRANGE_DEFAULT  0  /* default range */
#define IP_PORTRANGE_HIGH     1  /* same as DEFAULT (FreeBSD compat) */
#define IP_PORTRANGE_LOW      2  /* use privileged range */



#if defined(_NETBSD_SOURCE)
/*
 * Definitions for inet sysctl operations.
 *
 * Third level is protocol number.
 * Fourth level is desired variable within that protocol.
 */

/*
 * Names for IP sysctl objects
 */
#define	IPCTL_FORWARDING	1	/* act as router */
#define	IPCTL_SENDREDIRECTS	2	/* may send redirects when forwarding */
#define	IPCTL_DEFTTL		3	/* default TTL */
/* IPCTL_DEFMTU=4, never implemented */
#define	IPCTL_FORWSRCRT		5	/* forward source-routed packets */
#define	IPCTL_DIRECTEDBCAST	6	/* default broadcast behavior */
#define	IPCTL_ALLOWSRCRT	7	/* allow/drop all source-routed pkts */
#define	IPCTL_SUBNETSARELOCAL	8	/* treat subnets as local addresses */
#define	IPCTL_MTUDISC		9	/* allow path MTU discovery */
#define	IPCTL_ANONPORTMIN      10	/* minimum ephemeral port */
#define	IPCTL_ANONPORTMAX      11	/* maximum ephemeral port */
#define	IPCTL_MTUDISCTIMEOUT   12	/* allow path MTU discovery */
#define	IPCTL_MAXFLOWS         13	/* maximum ip flows allowed */
#define	IPCTL_HOSTZEROBROADCAST 14	/* is host zero a broadcast addr? */
#define	IPCTL_GIF_TTL 	       15	/* default TTL for gif encap packet */
#define	IPCTL_LOWPORTMIN       16	/* minimum reserved port */
#define	IPCTL_LOWPORTMAX       17	/* maximum reserved port */
#define	IPCTL_MAXFRAGPACKETS   18	/* max packets reassembly queue */
#define	IPCTL_GRE_TTL          19	/* default TTL for gre encap packet */
#define	IPCTL_CHECKINTERFACE   20	/* drop pkts in from 'wrong' iface */
#define	IPCTL_IFQ	       21	/* IP packet input queue */
#define	IPCTL_RANDOMID	       22	/* use random IP ids (if configured) */
#define	IPCTL_LOOPBACKCKSUM    23	/* do IP checksum on loopback */
#define	IPCTL_STATS		24	/* IP statistics */
#define	IPCTL_DAD_COUNT        25	/* DAD packets to send */

#endif /* _NETBSD_SOURCE */

 
#endif /* !_NETINET_IN_H_ */


 
