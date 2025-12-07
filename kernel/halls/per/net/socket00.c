#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>

/* These types might come from your object system. */
typedef enum {
    OBJECT_TYPE_SOCKET = 1,
    // ...
} object_type_t;

typedef enum {
    OBJECT_CLASS_NETWORK = 1,
    // ...
} object_class_t;

/* Enum for connection types (local vs. remote or client vs. server). */
typedef enum {
    CONNECTION_TYPE_NONE = 0,
    CONNECTION_TYPE_LOCAL,
    CONNECTION_TYPE_REMOTE
} connection_type_t;

/* Enum for socket state. */
typedef enum {
    SOCKET_STATE_CLOSED = 0,
    SOCKET_STATE_LISTENING,
    SOCKET_STATE_CONNECTED,
    SOCKET_STATE_SHUTDOWN
} socket_state_t;


/* Forward declaration for TCP control block if needed.
   This would hold TCP-specific state (sequence numbers, windows, timers, etc.). */
struct tcp_control_block;
typedef struct tcp_control_block tcp_control_block_t;


/* Improved socket structure. */
struct socket_d {
    /* Core object information */
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;
    int id;
    
    /* Basic socket parameters */
    int family;    // e.g. AF_INET, AF_INET6
    int type;      // e.g. SOCK_STREAM, SOCK_DGRAM
    int protocol;  // e.g. IPPROTO_TCP, IPPROTO_UDP
    
    /* Ownership information. */
    pid_t pid;
    uid_t uid;
    gid_t gid;
    
    /* Connection information */
    connection_type_t connection_type;
    
    /* Addressing: Using a union to cover IPv4 and IPv6. */
    union {
        struct sockaddr_in ipv4;
        struct sockaddr_in6 ipv6;
    } local_addr;
    
    /* For a connected socket, the remote address. */
    union {
        struct sockaddr_in ipv4;
        struct sockaddr_in6 ipv6;
    } remote_addr;
    
    /* Port information is available through the addresses,
       but you might also keep them separately for quick access. */
    uint16_t local_port;
    uint16_t remote_port;
    
    /* For TCP: protocol control or TCP-specific flags.
       Rather than mixing TCP control bits into a generic socket,
       you can isolate them. */
    union {
        uint16_t tcp_doff_res_flags; // TCP data offset/reserved/flags in one field
        // Extend this union later for other protocols.
    } proto_specific;
    
    /* Generic socket flags and options (e.g. non-blocking, reuse addr) */
    unsigned short flags;
    
    /* Backlog (used when listening). It's better to store pointers to pending sockets
       rather than raw numbers. */
    int connections_count;
    int backlog_max;
    int backlog_head;
    int backlog_tail;
    struct socket_d *pending_connections[32]; // Array of pointers to pending connection sockets.
    int client_backlog_pos;  // Position pointer if needed.
    bool isAcceptingConnections;
    
    /* Link for association with another socket (for a connected pair)
       or for linking into a list of established connections.
       You might also have a separate pointer to the shared TCP control block. */
    struct socket_d *link;
    
    socket_state_t state;  // e.g. SOCKET_CONNECTED, SOCKET_LISTENING, etc.
    
    /* If data from write() must be copied to the connected end. */
    int conn_copy;
    
    /* For the server: which index/descriptor the client is associated with.
       This might be used to map a client socket from a server’s internal list. */
    int clientfd_on_server;
    
    /* Associated File Data (if sockets are file objects in your system) */
    struct file *private_file;
    
    /* Debugging magic string */
    char magic_string[8];
    
    /* Local address storage (if needed as a fallback) */
    struct sockaddr addr;
    struct sockaddr_in addr_in;
    
    /* If you need to chain sockets in a list. */
    struct socket_d *next;
    
    /* Pointer to a TCP control block if the socket is TCP.
       Both ends of an active TCP connection can share the same TCB. */
    tcp_control_block_t *tcb;
};
