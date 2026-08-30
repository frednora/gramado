// socket.h
// Header for socket implementation.
// Created by Fred Nora.

#ifndef __NET_SOCKET_H
#define __NET_SOCKET_H    1

// Read using getsockopt() with SOL_SOCKET, SO_PEERCRED 
struct sockpeercred 
{
    uid_t uid;  // effective user id 
    gid_t gid;  // effective group id 
    pid_t pid;
};

typedef unsigned  socklen_t;
//typedef unsigned int socklen_t;

// Structure used for manipulating linger option.
// #ps: BSD-style
struct linger 
{
    int l_onoff;   // option on/off
    int l_linger;  // linger time in seconds
};

//bsd
/*
struct	accept_filter_arg {
	char	af_name[16];
	char	af_arg[256-16];
};
*/

// Level number for (get/set)sockopt() to apply to socket itself.
// Options for socket level
//#define SOL_SOCKET    0xffff

struct sockaddr 
{
    unsigned short sa_family;
    char sa_data[14];
};

// #test
struct sockaddr_gram  
{
    unsigned short sa_family;
    char           sa_data[14];
};


/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 * bsd style
 */
struct sockproto 
{
    unsigned short sp_family;    // address family 
    unsigned short sp_protocol;  // protocol 
};


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

/*
//#bugbug
//Device structure 
struct socket_context 
{
    struct socket_context *next; 
    struct socket_context *prev;
    unsigned int fd;
    int family;
    int type;
    int protocol;
};
typedef struct socket_context  socket_t;
*/

/*
 //#bugbug
struct hostent_d 
{
    char *h_addr;
    unsigned int h_length;
};
typedef struct hostent_d  hostent;
*/

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
struct sockcred 
{
    pid_t sc_pid;        // process id 
    uid_t sc_uid;        // real user id 
    uid_t sc_euid;       // effective user id 
    gid_t sc_gid;        // real group id 
    gid_t sc_egid;       // effective group id 
    int   sc_ngroups;    // number of supplemental groups 
    gid_t sc_groups[1];  // variable length 
};
*/

// This is the Gramado Kernel implementation for socke structure
struct socket_d
{
    struct kobject_d  kobj;
    int used;
    int magic;
    int id;

    // Basic socket parameters
    int family;    // e.g. AF_INET, AF_INET6
    int type;      // e.g. SOCK_STREAM, SOCK_DGRAM
    int protocol;  // e.g. IPPROTO_TCP, IPPROTO_UDP

// Ownership information.
// process, user, group.
    pid_t pid;
    uid_t uid; 
    gid_t gid;

    // maybe
    //struct sockpeercred  peercred;

//
// == Connections ========
//

// 1=LOCAL | 2=REMOTE
    int connection_type;
    int is_client_connecting_with_remote_server;
    int is_remote_client_connecting_with_local_server;

    struct connection_d *conn;  // Belongs to this connection
    struct endpoint_d *ep;      // Belongs to this ep

// Local
// ip and port.
    unsigned long ip_ipv6;
    unsigned int ip_ipv4;
    unsigned short port;

// Remote
// ip and port.
    //unsigned int remote_ip_ipv4;
    //unsigned long remote_ip_ipv6;
    //unsigned short remote_port;

//
// Protocol flags
//

// #test
// In the case of TCP connections, 
//  we're gonna handle the control bits
    // Data Offset (4bits) | Reserved (6bits) | Control bits (6bits).
    uint16_t tcp__do_res_flags;

//
// Socket flags
//

// The flags that describe the state of this socket.
    unsigned short flags; 

//
// Backlog
//

// Server‑side child sockets
    unsigned long pending_server_endpoints[32];
    int pending_server_count;

// Client‑side child sockets (for local connections)
    unsigned long pending_client_endpoints[32];
    int pending_client_count;

// List of sockets.
    int backlog_max;
    int backlog_head;

// It indicates that this socket is currently
// accepting new connections.
// Updated by listen().
    int isAcceptingConnections;
// Link to the current connection.
    struct socket_d *link;

// See: sockint.h
    int state;

// TRUE = available for reuse, FALSE = occupied.
    int free;

// FALSE = normal, TRUE = verbose debug.
// We can set it via ioctl.
    int debug;

// flag
// write() copy the data to the connected socket.
    int conn_copy; 
// The server finds a place in the server_process->Objects[i].
    int clientfd_on_server;
// ====================================

// Is this a socket file?
// Associated File Data (if sockets are file objects in your system)
    file *private_file;
// Debugging magic string
    char magic_string[8];

// Local address storage (if needed as a fallback)
    struct sockaddr  addr;
    struct sockaddr_in  addr_in; 

// Navigation
// If you need to chain sockets in a list.
    struct socket_d *next;
};

// see: socket.c
extern struct socket_d  *CurrentSocket;
extern struct socket_d  *LocalHostHTTPSocket;
// ...

//
// == prototypes =========================
//

//
// ------------------------------------------
//

// Belongs to socket.c

int 
sys_getsockname ( 
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen );

int sys_socket_shutdown (int socket, int how);

// https://linux.die.net/man/2/sendto
ssize_t 
sys_sendto ( 
    int sockfd, 
    const void *ubuf, 
    size_t len, 
    int flags,
    const struct sockaddr *dest_addr, 
    socklen_t addrlen );

ssize_t sys_sendmsg (int sockfd, const struct msghdr *msg, int flags);

//
// ============================================================
// Well know libc methods for socket support
//

int sys_socket(int family, int type, int protocol);

int 
sys_bind ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen );

int sys_listen(int sockfd, int backlog);

int 
sys_accept (
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen );

int 
sys_gramado_accept (
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen );

int 
sys_connect ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen );

#endif    

