// socklib.h
// Library for socket support.
// Created by Fred Nora.

#ifndef __NET_SOCKLIB_H
#define __NET_SOCKLIB_H    1

//#define SOCKET_MAX_BACKLOG  32
#define SOCKET_MAX_PENDING_CONNECTIONS   32

// Socket state
#define SS_NULL           0
#define SS_UNCONNECTED    1
#define SS_CONNECTING     2
#define SS_CONNECTED      3
#define SS_DISCONNECTING  4

// Ports
#define PORTS_WS  4040  // Window server
#define PORTS_NS  4041  // Network server
#define PORTS_FS  4042  // Filesystem server
#define PORTS_WM  4043  // Window manager service
//#test
#define PORTS_WE  4044  // Web server
#define PORTS_FT  4045  // FTP server
#define PORTS_TN  4046  // Telnet server
// ...
// Alias
#define __PORTS_DISPLAY_SERVER  PORTS_WS
#define __PORTS_NETWORK_SERVER  PORTS_NS

//=====================================================
//++
// Gramado ports:
// ports for local servers.
// What pid is on each port.
// It can be used to setup the main components of the system,
// or maybe only the services in the gramado_ports.
// AF_GRAMADO
// ... gramado_ports[]
// ...
#define GRAMADO_PORT_WS  11  // Window server
#define GRAMADO_PORT_WM  12  // Window manager
#define GRAMADO_PORT_NS  14  // Network server
#define GRAMADO_PORT_FS  16  // File system
// #test
#define GRAMADO_PORT_WE  17  // Web server
#define GRAMADO_PORT_FT  18  // FTP server
#define GRAMADO_PORT_TN  19  // Telnet server
// ...
#define GRAMADO_PORT_BR  20  // Gramado browser.

#define GRAMADO_PORT_MAX  32
//--
//=====================================================


/*
#ifndef sa_family_t
typedef __sa_family_t	sa_family_t;
#define sa_family_t	__sa_family_t
#endif
*/

/*
#ifndef socklen_t
typedef __socklen_t	socklen_t;
#define socklen_t	__socklen_t
#endif
*/

/*
#ifdef	_BSD_SIZE_T_
typedef	_BSD_SIZE_T_	size_t;
#undef	_BSD_SIZE_T_
#endif
*/

/*
#ifdef	_BSD_SSIZE_T_
typedef	_BSD_SSIZE_T_	ssize_t;
#undef	_BSD_SSIZE_T_
#endif
*/

/*
    // Socket types
    // bsd-like and linux-like ?
    // #todo: do the same on ring3.

    SOCK_STREAM     Provides sequenced, reliable, two-way, connection-
                    based byte streams.  An out-of-band data transmission
                    mechanism may be supported.

    SOCK_DGRAM      Supports datagrams (connectionless, unreliable
                    messages of a fixed maximum length).

    SOCK_SEQPACKET  Provides a sequenced, reliable, two-way connection-
                    based data transmission path for datagrams of fixed
                    maximum length; a consumer is required to read an
                    entire packet with each input system call.

    SOCK_RAW        Provides raw network protocol access.

    SOCK_RDM        Provides a reliable datagram layer that does not
                    guarantee ordering.

    SOCK_PACKET     Obsolete and should not be used in new programs;
*/

#define SOCK_STREAM     1    /* stream (connection) socket	*/
#define SOCK_DGRAM      2    /* datagram (conn.less) socket	*/
#define SOCK_RAW        3    /* raw socket			        */
#define SOCK_RDM        4    /* reliably-delivered message	*/
#define SOCK_SEQPACKET  5    /* sequential packet socket	*/ /* sequenced packet stream */
#define SOCK_DCCP       6
// ...
#define SOCK_PACKET    10    /* linux specified 		    */
#define SOCK_GRAMADO_MSG    8000    /* window, msg, long1, long2 */
#define SOCK_MAX            (SOCK_GRAMADO_MSG+1)


//=========
/* Supported address families. */

#define AF_UNSPEC     0
#define AF_UNIX       1     /* Unix domain sockets 		*/
#define AF_LOCAL      1     /* POSIX name for AF_UNIX	*/
#define AF_INET       2     /* Internet IP Protocol 	*/
#define AF_AX25       3     /* Amateur Radio AX.25 		*/
#define AF_IPX        4     /* Novell IPX 			*/
#define AF_APPLETALK  5     /* AppleTalk DDP 		*/
#define AF_NETROM     6     /* Amateur Radio NET/ROM 	*/
#define AF_BRIDGE     7     /* Multiprotocol bridge 	*/
#define AF_ATMPVC     8     /* ATM PVCs			*/
#define AF_X25        9     /* Reserved for X.25 project 	*/
#define AF_INET6      10    /* IP version 6			*/
#define AF_ROSE       11    /* Amateur Radio X.25 PLP	*/
#define AF_DECnet     12    /* Reserved for DECnet project	*/
#define AF_NETBEUI    13    /* Reserved for 802.2LLC project*/
#define AF_SECURITY   14    /* Security callback pseudo AF */
#define AF_KEY        15    /* PF_KEY key management API */
#define AF_NETLINK    16
#define AF_ROUTE      AF_NETLINK    /* Alias to emulate 4.4BSD */
#define AF_PACKET     17            /* Packet family		*/
#define AF_ASH        18            /* Ash				*/
#define AF_ECONET     19            /* Acorn Econet			*/
#define AF_ATMSVC     20            /* ATM SVCs			*/
#define AF_SNA        22            /* Linux SNA Project (nutters!) */
#define AF_IRDA       23            /* IRDA sockets			*/
#define AF_PPPOX      24            /* PPPoX sockets		*/
#define AF_WANPIPE    25            /* Wanpipe API Sockets */
#define AF_LLC        26            /* Linux LLC			*/
#define AF_TIPC       30            /* TIPC sockets			*/
#define AF_BLUETOOTH  31            /* Bluetooth sockets 		*/
#define AF_IUCV       32            /* IUCV sockets			*/
#define AF_RXRPC      33            /* RxRPC sockets 		*/
#define AF_RS232      35            /* Serial socket (NEW!) */
// ...
#define AF_GRAMADO    8000
// Max
#define AF_MAX     AF_GRAMADO
#define NET_MAXID  AF_MAX
#define NPROTO     AF_MAX
// -------------------------------------------------------

/* Protocol families, same as address families. */
#define PF_UNSPEC	AF_UNSPEC
#define PF_UNIX		AF_UNIX
#define PF_LOCAL	AF_LOCAL
#define PF_INET		AF_INET
#define PF_AX25		AF_AX25
#define PF_IPX		AF_IPX
#define PF_APPLETALK	AF_APPLETALK
#define	PF_NETROM	AF_NETROM
#define PF_BRIDGE	AF_BRIDGE
#define PF_ATMPVC	AF_ATMPVC
#define PF_X25		AF_X25
#define PF_INET6	AF_INET6
#define PF_ROSE		AF_ROSE
#define PF_DECnet	AF_DECnet
#define PF_NETBEUI	AF_NETBEUI
#define PF_SECURITY	AF_SECURITY
#define PF_KEY		AF_KEY
#define PF_NETLINK	AF_NETLINK
#define PF_ROUTE	AF_ROUTE
#define PF_PACKET	AF_PACKET
#define PF_ASH		AF_ASH
#define PF_ECONET	AF_ECONET
#define PF_ATMSVC	AF_ATMSVC
#define PF_SNA		AF_SNA
#define PF_IRDA		AF_IRDA
#define PF_PPPOX	AF_PPPOX
#define PF_WANPIPE	AF_WANPIPE
#define PF_LLC		AF_LLC
#define PF_TIPC		AF_TIPC
#define PF_BLUETOOTH	AF_BLUETOOTH
#define PF_IUCV		AF_IUCV
#define PF_RXRPC	AF_RXRPC
#define PF_RS232	AF_RS232
#define PF_MAX		AF_MAX

//-------------

// -------------------------------------
// AF_UNIX:  Unix domain socket (UDS)
// Valid socket types in the UNIX domain are:
// see: 
// https://en.wikipedia.org/wiki/Unix_domain_socket

/*
 > SOCK_STREAM    (compare to TCP)  – 
   for a stream-oriented socket

 > SOCK_DGRAM     (compare to UDP)  – 
   for a datagram-oriented socket that preserves message boundaries 
   (as on most UNIX implementations, UNIX domain datagram sockets are 
   always reliable and don't reorder datagrams)

 > SOCK_SEQPACKET (compare to SCTP) – 
   for a sequenced-packet socket that is connection-oriented, 
   preserves message boundaries, and 
   delivers messages in the order that they were sent
*/

// -------------------------------------
// Types of network protocol
// Datagram sockets (Connectionless sockets)      (UDP)
// Stream sockets   (Connection-oriented sockets) (TCP) (SCTP) (DCCP)
// Raw sockets      (No protocols?)
//                  The application must build the entire IP datagram, 
//                  including the IP header. 

//see:
// https://en.wikipedia.org/wiki/Network_socket


// Maximum queue length specifiable by listen().
//#define SOMAXCONN	128
#define SOMAXCONN    32

#define MAXHOSTNAMELEN    256

//-----------------

/* 
 * Flags we can use with send/ and recv. 
 * Added those for 1003.1g not all are supported yet 
 */

// #bugbug:
// Isso se confunde com as 'message codes' in msgcode.h 
// Usadas pelo Gramado.
// Mudando o prefixo de MSG_ para SOCKET_MSG_


#define SOCKET_MSG_OOB        1
#define SOCKET_MSG_PEEK       2
#define SOCKET_MSG_DONTROUTE  4
#define SOCKET_MSG_TRYHARD    4       //Synonym for MSG_DONTROUTE for DECnet 
#define SOCKET_MSG_CTRUNC     8
#define SOCKET_MSG_PROBE      0x10  //Do not send. Only probe path f.e. for MTU 
#define SOCKET_MSG_TRUNC      0x20
#define SOCKET_MSG_DONTWAIT   0x40  //Nonblocking io		 
#define SOCKET_MSG_EOR        0x80  //End of record 
#define SOCKET_MSG_WAITALL    0x100  //Wait for a full request 
#define SOCKET_MSG_FIN        0x200
#define SOCKET_MSG_SYN        0x400
#define SOCKET_MSG_CONFIRM    0x800  //Confirm path validity 
#define SOCKET_MSG_RST		0x1000
#define SOCKET_MSG_ERRQUEUE	0x2000  //Fetch message from error queue 
#define SOCKET_MSG_NOSIGNAL	0x4000  //Do not generate SIGPIPE 
#define SOCKET_MSG_MORE	0x8000   //Sender will send more 

#define SOCKET_MSG_EOF  SOCKET_MSG_FIN

#if defined(CONFIG_COMPAT)
#define SOCKET_MSG_CMSG_COMPAT  0x80000000  // This message needs 32 bit fixups
#else
#define SOCKET_MSG_CMSG_COMPAT  0  //We never have 32 bit fixups
#endif



//bsd
/*
 * Message header for recvmsg and sendmsg calls.
 * Used value-result for recvmsg, value only for sendmsg.
 */
/* 
struct msghdr {
	void		*msg_name;	     // optional address 
	socklen_t	msg_namelen;	 // size of address 
	struct iovec	*msg_iov;	 // scatter/gather array 
	int		msg_iovlen;	         // # elements in msg_iov 
	void		*msg_control;	 // ancillary data, see below 
	socklen_t	msg_controllen;  // ancillary data buffer len 
	int		msg_flags;	         // flags on received message 
};
*/

/*
struct mmsghdr {
	struct msghdr msg_hdr;
	unsigned int msg_len;
};
*/

//------------


/* Setsockoptions(2) level. Thanks to BSD these must match IPPROTO_xxx */
#define SOL_IP		0
/* #define SOL_ICMP	1	No-no-no! Due to Linux :-) we cannot use SOL_ICMP=1 */
#define SOL_TCP		6
#define SOL_UDP		17
#define SOL_IPV6	41
#define SOL_ICMPV6	58
#define SOL_SCTP	132
#define SOL_UDPLITE	136     /* UDP-Lite (RFC 3828) */
#define SOL_RAW		255
#define SOL_IPX		256
#define SOL_AX25	257
#define SOL_ATALK	258
#define SOL_NETROM	259
#define SOL_ROSE	260
#define SOL_DECNET	261
#define	SOL_X25		262
#define SOL_PACKET	263
#define SOL_ATM		264	/* ATM layer (cell level) */
#define SOL_AAL		265	/* ATM Adaption Layer (packet level) */
#define SOL_IRDA        266
#define SOL_NETBEUI	267
#define SOL_LLC		268
#define SOL_DCCP	269
#define SOL_NETLINK	270
#define SOL_TIPC	271
#define SOL_RXRPC	272


// IPX options
#define IPX_TYPE	1


//-----------------


/*
 * Option flags per-socket.
 */
// credits: openbsd
#define	SO_DEBUG	0x0001		/* turn on debugging info recording */
#define	SO_ACCEPTCONN	0x0002		/* socket has had listen() */
#define	SO_REUSEADDR	0x0004		/* allow local address reuse */
#define	SO_KEEPALIVE	0x0008		/* keep connections alive */
#define	SO_DONTROUTE	0x0010		/* just use interface addresses */
#define	SO_BROADCAST	0x0020		/* permit sending of broadcast msgs */
#define	SO_USELOOPBACK	0x0040		/* bypass hardware when possible */
#define	SO_LINGER	0x0080		/* linger on close if data present */
// #define SO_DONTLINGER	~SO_LINGER
#define	SO_OOBINLINE	0x0100		/* leave received OOB data in line */
#define	SO_REUSEPORT	0x0200		/* allow local address & port reuse */
#define SO_TIMESTAMP	0x0800		/* timestamp received dgram traffic */
#define SO_BINDANY	0x1000		/* allow bind to any address */
#define SO_ZEROIZE	0x2000		/* zero out all mbufs sent over socket */

/*
 * Additional options, not kept in so_options.
 */
// credits: openbsd
#define	SO_SNDBUF	0x1001		/* send buffer size */
#define	SO_RCVBUF	0x1002		/* receive buffer size */
#define	SO_SNDLOWAT	0x1003		/* send low-water mark */
#define	SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define	SO_SNDTIMEO	0x1005		/* send timeout */
#define	SO_RCVTIMEO	0x1006		/* receive timeout */
#define	SO_ERROR	0x1007		/* get error status and clear */
#define	SO_TYPE		0x1008		/* get socket type */
#define	SO_NETPROC	0x1020		/* multiplex; network processing */
#define	SO_RTABLE	0x1021		/* routing table to be used */
#define	SO_PEERCRED	0x1022		/* get connect-time credentials */
#define	SO_SPLICE	0x1023		/* splice data to other socket */
#define	SO_DOMAIN	0x1024		/* get socket domain */
#define	SO_PROTOCOL	0x1025		/* get socket protocol */

/*
 * Types of socket shutdown(2).
 */
 
#define SHUT_RD    0    /* Disallow further receives. */
#define SHUT_WR    1    /* Disallow further sends. */
#define SHUT_RDWR  2    /* Disallow further sends/receives. */

//-----------

#endif   

