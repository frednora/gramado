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

/*
 * Structure used for manipulating linger option.
 * BSD-style.
 */
struct linger 
{
    int l_onoff;     /* option on/off */
    int l_linger;    /* linger time in seconds */
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
 * bsd
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

// The socket structure.
struct socket_d
{
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;
    int id;

    int family;
    int type;
    int protocol;

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

// ip and port.
    unsigned int ip_ipv4;
    unsigned long ip_ipv6;
    unsigned short port;

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

// The list of pending connections.
// Updated by listen().
    int connections_count;
// List of sockets.
    int backlog_max;
    int backlog_head;
    int backlog_tail;
    unsigned long pending_connections[32];
// Em que posiçao o ponteiro do socket de cliente esta
// dentro da fila de conecxoes pendentes no socket do servidor.
    int client_backlog_pos;

// It indicates that this socket is currently
// accepting new connections.
// Updated by listen().
    int isAcceptingConnections;
// Link to the current connection.
    struct socket_d *link;
// States: SOCKET_CONNECTED, SOCKET_NOT_CONNECTED.
    int state;
// flag
// write() copy the data to the connected socket.
    int conn_copy; 
// The server finds a place in the server_process->Objects[i].
    int clientfd_on_server;
// ====================================

// Nosso arquivo.
// Eh o objeto socket??
    file *private_file;
// testing
    char magic_string[8];

// Local structures for address.
    struct sockaddr     addr;
    struct sockaddr_in  addr_in; 

// #todo
// Navegation
    //struct socket_d *next;
};

// see: socket.c
extern struct socket_d  *CurrentSocket;
extern struct socket_d  *LocalHostHTTPSocket;
// ...

// #todo:
// Refazer esse limite proviório.
#define SOCKET_COUNT_MAX  32
extern unsigned long socketList[SOCKET_COUNT_MAX];

//
// == prototypes =========================
//

struct socket_d *create_socket_object(void);

// Get the socket structure in the process list given the port number.
struct socket_d *get_socket_in_process_list(unsigned short target_port);

unsigned int getSocketIPV4(struct socket_d *socket);
unsigned long getSocketIPV6(struct socket_d *socket);
unsigned short getSocketPort(struct socket_d *socket);
struct socket_d *get_socket_from_fd(int fd);
void show_socket_for_a_process(pid_t pid);

//
// sys_socket() support.
//

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
socket_inet ( 
    struct socket_d *sock, 
    int family,
    int type, 
    int protocol );

int socket_init (void);
int socket_ioctl ( int fd, unsigned long request, unsigned long arg );

int socket_read ( int fd, char *buf, int count );
int socket_write ( int fd, char *buf, int count );

// gramado ports.
pid_t socket_get_gramado_port (int port);
int socket_set_gramado_port (int port, pid_t pid);

int 
update_socket ( 
    struct socket_d *socket, 
    unsigned int ip_ipv4, 
    unsigned short port );

int
sock_socketpair ( 
    int family, 
    int type, 
    int protocol, 
    int usockvec[2] );

unsigned long 
socket_dialog ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

//
// ------------------------------------------
//

// Belongs to socket.c

int 
sys_accept (
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen );

int 
sys_bind ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen );

int 
sys_connect ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen );

int 
sys_getsockname ( 
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen );

int sys_listen (int sockfd, int backlog);
int sys_socket (int family, int type, int protocol);
int sys_socket_shutdown (int socket, int how);

#endif    

