
// socket.h
// suporte a socket dentro do kernel.


#ifndef ____SOCKET_H
#define ____SOCKET_H




//socket state.
#define SOCKET_NOT_CONNECTED    0
#define SOCKET_CONNECTED        1 
#define SOCKET_CONNECTING       2 
#define SOCKET_PENDING          3
// ...




//
// gramado ports
//


//++
// ports for local servers
// What pid is on each port.
// AF_GRAMADO

// ... gramado_ports[]

#define GRAMADO_WS_PORT  11  // window server.
#define GRAMADO_NS_PORT  14  // network server.
#define GRAMADO_FS_PORT  16  // file system

// ...
int gramado_ports[32];
//--


//#atenção. Variável global;
int current_socket;


/*
 * create_socket:
 * 
 */
struct socket_d *create_socket ( unsigned long ip, unsigned short port );


unsigned long getSocketIP ( struct socket_d *socket );
unsigned long getSocketPort ( struct socket_d *socket );


int 
update_socket ( 
    struct socket_d *socket, 
    unsigned long ip, 
    unsigned short port );



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
#define AF_MAX        34            /* For now.. ??? */
#define AF_RS232      35            /* Serial socket (NEW!) */
#define AF_GRAMADO    8000
//#define AF_MAX        AF_GRAMADO  //#todo



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
#define PF_MAX		AF_MAX
#define PF_RS232	AF_RS232


/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	128



/* Flags we can use with send/ and recv. 
   Added those for 1003.1g not all are supported yet */
 
#define MSG_OOB        1
#define MSG_PEEK       2
#define MSG_DONTROUTE  4
#define MSG_TRYHARD    4       /* Synonym for MSG_DONTROUTE for DECnet */
#define MSG_CTRUNC     8
#define MSG_PROBE      0x10  /* Do not send. Only probe path f.e. for MTU */
#define MSG_TRUNC      0x20
#define MSG_DONTWAIT   0x40  /* Nonblocking io		 */
#define MSG_EOR        0x80  /* End of record */
#define MSG_WAITALL    0x100  /* Wait for a full request */
#define MSG_FIN        0x200
#define MSG_SYN        0x400
#define MSG_CONFIRM    0x800  /* Confirm path validity */
#define MSG_RST		0x1000
#define MSG_ERRQUEUE	0x2000	/* Fetch message from error queue */
#define MSG_NOSIGNAL	0x4000	/* Do not generate SIGPIPE */
#define MSG_MORE	0x8000	/* Sender will send more */

#define MSG_EOF         MSG_FIN

#if defined(CONFIG_COMPAT)
#define MSG_CMSG_COMPAT	0x80000000	/* This message needs 32 bit fixups */
#else
#define MSG_CMSG_COMPAT	0		/* We never have 32 bit fixups */
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

/* IPX options */
#define IPX_TYPE	1


/* 
 *  ** Sock types  ** 
 */
 
/*
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
#define SOCK_SEQPACKET  5    /* sequential packet socket	*/
#define SOCK_PACKET    10    /* linux specified 		    */

//#test
#define SOCK_GRAMADO_MSG    8000    /* window, msg, long1, long2 */




#define  _NETINET_IN_H_
#define  IPPROTO_IP      0  // Dummy for IP.
#define  IPPROTO_ICMP    1  // Control message protocol.
#define  IPPROTO_IGMP    2  // Group management protocol.
#define  IPPROTO_TCP     6  // Transmission control protocol.
#define  IPPROTO_UDP     17   // User datagram protocol.
#define  IPPROTO_RAW    255          // Raw IP packet.
#define  IPPROTO_MAX    256          // Maximum protocol identifier.
#define  IPPORT_RESERVED 1024        // Last reserved port number.
#define  IPPORT_USERRESERVED   5000  // User reserved port number.
#define  INADDR_ANY   (unsigned long)0x00000000 // Any IP address.
#define  INADDR_BROADCAST   (unsigned long)0xffffffff // Broadcast IP address.



/*
 * Types of socket shutdown(2).
 */
 
#define  SHUT_RD      0    /* Disallow further receives. */
#define  SHUT_WR      1    /* Disallow further sends. */
#define  SHUT_RDWR    2    /* Disallow further sends/receives. */



typedef unsigned socklen_t;



//bsd
/*
 * Structure used for manipulating linger option.
 */
struct	linger {
	int	l_onoff;		/* option on/off */
	int	l_linger;		/* linger time in seconds */
};


//bsd
/*
struct	accept_filter_arg {
	char	af_name[16];
	char	af_arg[256-16];
};
*/


/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
//#define	SOL_SOCKET	0xffff		/* options for socket level */



//bsd
/*
 * Structure used by kernel to store most
 * addresses.
 */
/* 
struct sockaddr {
	__uint8_t	sa_len;         // total length 
	sa_family_t	sa_family;      //address family 
	char		sa_data[14];    //actually longer; address value 
};
*/



//bsd
/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
/*
struct sockproto {
	u_short	sp_family;		    // address family 
	u_short	sp_protocol;		// protocol 
};
*/



//bsd
/*
 * we make the entire struct at least UCHAR_MAX + 1 in size since existing
 * use of sockaddr_un permits a path up to 253 bytes + '\0'.
 * sizeof(sb_len) + sizeof(sb_family) + 253 + '\0'
 */
/*
#define _SB_DATASIZE	254
struct sockaddr_big {
    union {
	struct {
	    __uint8_t	sb_len;
	    sa_family_t	sb_family;
	    char	sb_data[_SB_DATASIZE];
	};
	uint64_t dummy; //  solicit natural alignment 
    };
};
*/



/* Device structure */
typedef struct socket_context {

    struct socket_context *next, *prev;
    unsigned fd;
    int family;
    int type;
    int protocol;

}socket_t;



// See:
// http://alas.matf.bg.ac.rs/manuals/lspe/snode=25.html
struct sockaddr
{
    unsigned short sa_family;
    char sa_data[14];
};


//#todo
//isso deve ficar em in.h
typedef struct
{
    int sin_family;
    int sin_port;
    char *sin_addr;

}sockaddr_in;


typedef struct
{
    char *h_addr;
    unsigned h_length;

}hostent;


//=========



/*
#ifndef pid_t
typedef __pid_t		pid_t;		// process id 
#define pid_t		__pid_t
#endif
*/

/*
#ifndef	gid_t
typedef	__gid_t		gid_t;		// group id 
#define	gid_t		__gid_t
#endif
*/

/*
#ifndef	uid_t
typedef	__uid_t		uid_t;		// user id 
#define	uid_t		__uid_t
#endif
*/


//bsd
/*
 * Socket credentials.
 */
/* 
struct sockcred {
	pid_t	sc_pid;			// process id 
	uid_t	sc_uid;			// real user id 
	uid_t	sc_euid;		// effective user id 
	gid_t	sc_gid;			// real group id 
	gid_t	sc_egid;		// effective group id 
	int	sc_ngroups;		    // number of supplemental groups 
	gid_t	sc_groups[1];	// variable length 
};
*/


/*
 **********************
 * socket_d:
 *     Socket strutuct.
 */

struct socket_d
{
    object_type_t objectType;
    object_class_t objectClass;


    int used;
    int magic;

    // #test.
    int family;
    int type;
    int protocol;
    
    pid_t pid;  // Process
    uid_t uid;  // User 
    gid_t gid;  // Group


    unsigned long ip;
    unsigned short port;

    //
    // Connection
    //
    
    
    struct socket_d *conn;
    int state;   // SOCKET_CONNECTED, SOCKET_NOT_CONNECTED


    // Nosso arquivo, nosso buffer ?
    file *private_file;



    //se ele está ou não aceitando conexões. ...
    //...
    unsigned short flags; 


    // usada em endereços AF_GRAMADO
    struct sockaddr addr;

    // usada em endereços AF_INET
    struct sockaddr_in addr_in; 
};
struct socket_d *CurrentSocket;
struct socket_d *LocalHostHTTPSocket;
//...


// #todo:
// Refazer esse limite proviório.
#define SOCKET_COUNT_MAX 32

unsigned long socketList[SOCKET_COUNT_MAX];


//
// prototipes;
//

int socket_init(void);

int 
socket_gramado ( 
    struct socket_d *sock,
    int family, 
    int type, 
    int protocol );


int 
socket_unix ( 
    struct socket_d *sock, 
    int family, 
    int type, 
    int protocol );


int 
socket_inet ( struct socket_d *sock, 
              int family, 
              int type, 
              int protocol );



int sys_socket ( int family, int type, int protocol );

struct socket_d *get_socket_from_fd (int fd);


int
sock_socketpair ( 
    int family, 
    int type, 
    int protocol, 
    int usockvec[2] );


int socket_ioctl ( int fd, unsigned long request, unsigned long arg );

// Essa rotina sera' chamada se os serviços 
// forem superios a 7000 ou inferiores a 8000
unsigned long 
socket_dialog ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );



int 
sys_connect ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen );
              
              
int sys_accept (int sockfd, struct sockaddr *addr, socklen_t *addrlen);             


int 
sys_bind ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen );

int 
sys_getsockname ( 
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen );

int sys_listen (int sockfd, int backlog);          
               

int socket_read (unsigned int fd, char *buf, int count);
int socket_write (unsigned int fd,char *buf,int count);


// #test
// vamos pegar um descritor que aponta para
// um arquivo do tipo soquete que deseja se concetar 
// em o processo
int sys_accept_sender(int n);



int socket_set_gramado_port(int port, int pid);


void show_socket_for_a_process(int pid);


int is_socket (file *f);
int is_virtual_console (file *f);
//...


#endif   



//
// End.
//



