// net.h
// Created by Fred Nora.

#ifndef __NET_NETWORK_H
#define __NET_NETWORK_H    1


struct remote_endpoint_d 
{
    struct sockaddr_in addr;

    int protocol;

//
// tcp
//
    int tcp_state;
    // future: seq numbers, ack numbers, flags, etc.
};


struct endpoint_d
{
// An endpoint belongs to a side, inside a corner of a square.
    int side_id;     // LEFT or RIGHT (0 or 1)
    int case_id;     // which of the 4 square cases (LCLS, LCRS, LSLC, LSRC)

    int is_remote;   // ENDPOINT_LOCAL or ENDPOINT_REMOTE
    struct socket_d *socket;              // valid only if type == LOCAL
    struct remote_endpoint_d *remote;     // valid only if type == REMOTE
};


struct endpoint_pair_d 
{
    int used;
    int magic;

// An endpoint pair belongs to a case.
//  One of the 4 corners of a square.
    int case_id;
    struct endpoint_d *left;
    struct endpoint_d *right;
};


struct udp_connection_d 
{
    int used;
    int magic;
};


/*
What belongs in tcp_connection_d
This structure represents the local TCP state machine 
for a single connection. It contains everything your kernel needs to:
track sequence numbers
manage windows
handle retransmissions
maintain timers
process ACKs
implement congestion control (later)
*/
struct tcp_connection_d 
{
    int used;
    int magic;

// TCP_LISTEN, TCP_SYN_SENT, TCP_ESTABLISHED, etc.
    int state;

// Sequence Numbers

    uint32_t snd_una;   // oldest unacknowledged sequence
    uint32_t snd_nxt;   // next sequence to send
    uint32_t snd_wnd;   // send window
    uint32_t iss;       // initial send sequence

    uint32_t rcv_nxt;   // next expected sequence
    uint32_t rcv_wnd;   // receive window
    uint32_t irs;       // initial receive sequence

// Buffers
    void *send_buffer;
    size_t send_buffer_len;
    void *recv_buffer;
    size_t recv_buffer_len;

// Timers
    uint32_t rto;       // retransmission timeout
    uint32_t srtt;      // smoothed RTT
    uint32_t rttvar;    // RTT variance
    uint32_t last_ack;  // timestamp of last ACK
    uint32_t last_sent; // timestamp of last sent segment

// Flags and Options
// TCP options negotiated during handshake:
    int sack_enabled;
    int window_scaling;
    int timestamp_enabled;
    uint16_t mss;       // maximum segment size

// Congestion Control
    uint32_t cwnd;      // congestion window
    uint32_t ssthresh;  // slow start threshold

// Retransmission Tracking
    int retransmit_count;
    uint32_t last_retransmit;
};

// ...

// connection_d
// Represents a communication link.
// Local connections: only local_conn is used.
// Remote connections: server_endpoint + remote_endpoint describe
// the local socket and the remote endpoint.

struct connection_d 
{
    int used;
    int magic;

    struct endpoint_pair_d *ep_pair;

    int type;  // LOCAL, UDP, TCP
    struct udp_connection_d *udp_conn;
    struct tcp_connection_d *tcp_conn;
};

#define MAX_CONNECTIONS  256
// See: net.c
extern unsigned long connectionList[MAX_CONNECTIONS];

// ================================================


// Used for fast responses
struct network_saved_d
{
// Gateway info
    unsigned char gateway_ipv4[4];
    unsigned char gateway_mac[6];
// Our info.
// We have another structure for our information.
    //unsigned char our_ipv4[4];
    //unsigned char our_mac[6];
// Caller's info.
    unsigned char caller_ipv4[4];
    unsigned char caller_mac[6];
};
extern struct network_saved_d  NetworkSaved;


// 8192
#define NETWORK_DEFAULT_BUFFER_SIZE  \
    E1000_DEFAULT_BUFFER_SIZE


// >> The register at offset 0x00 is the "IOADDR" window. 
// >> The register at offset 0x04 is the "IODATA" window. 

// packet format
// Ethernet IPv4 TCP/UDP DATA FCS

/*
// Each NIC device needs to be represented here.
// But each NIC device has its own internal structure
// with elements specific for their brand.
struct nic_device_d
{
    int used;
    int magic;
    int id;

    char *dev_name;
    size_t name_size;

    int initialized;

// Opaque pointer to the archtecture specific structure.
    void *priv;
    int brand;  // Intel, Realteck ...

    unsigned char mac_address[6];
    unsigned char ipv4_address[4];
    unsigned char ipv6_address[6];

// Navigation
    struct nic_device_d *next;
};
extern struct nic_device_d *CurrentNICDevice;
*/

// =================================================

//#define DEFAULT_NUMBER_OF_NETWORK_BUFFERS  32

struct network_buffer_d
{
    int initialized;
// Receive
    int receive_tail;
    int receive_head;
//  pointers.
    unsigned long buffers[32];
// O status de cada buffer, se ele está vazio ou não.
    int is_full[32];
};
// See: network.c
extern struct network_buffer_d  NETWORK_BUFFER;

//========================================================

// Initialized?
#define NETWORK_INITIALIZED  TRUE
#define NETWORK_NOT_INITIALIZED  FALSE

// Online?
#define NETWORK_ONLINE   TRUE
#define NETWORK_OFFLINE  FALSE
#define ONLINE   NETWORK_ONLINE
#define OFFLINE  NETWORK_OFFLINE

struct network_initialization_d
{

// Status do driver de network
// 0 - uninitialized
// 1 - initialized
    int initialized;

// Are we online?
// Do we already have an valid IP?
// We are online when we already have a valid IP.
    int is_online;

    int locked;

// ...

};
extern struct network_initialization_d  NetworkInitialization;

// =================================================

// Describe our network
// #todo
// There is a lot of fields here,
// let's include pointers to another structures.
struct network_info_d
{
    int used;
    int magic;
    int initialized;  // This structure was initialied

// Número identificador da rede.
    int id;

// Strings

// Nome da rede.
    char *name_string;
    size_t name_string_size;
// String mostrando a versão. ex: (1.1.1234)
    char *version_string;
    size_t version_string_size;

// Values
// #bugbug
// We don't need this.
    //unsigned short version_major;
    //unsigned short version_minor;
    //unsigned short version_revision;

// Network status
    //int status;
    //int is_online;

//adaptador de rede.
    //struct intel_nic_info_d *nic_info;

    // struct user_info_d *user_info;
    struct host_info_d *host_info;
    //..

// Gateway support
    uint8_t gateway_mac[6];
    uint8_t gateway_ipv4[4];
    //uint8_t gateway_ipv6[6];
    int gateway_initialized;


// Number of nic devices in the machine.
    int nic_counter;
// The interface is using this nic device.
    //int nic_in_use;

// ...

// Stats
// For all the NICs?
    unsigned long tx_counter;
    unsigned long rx_counter;

// The domain controller.
    //struct domain_d *domain_controller;

// This network interface belongs to this cgroup.
// If a process belongs to another cgroup i will be unable to 
// accress this interface.
// This way we can create multiple network interface and 
// each container/vm can use his own network interface.
    //struct cgroup_d  *cgroup;

    struct network_info_d  *next;
};
extern struct network_info_d *NetworkInfo;

// =================================================


//
// == Prototypes ====================
//

// Network interface for keyboard input.
int 
network_keyboard_event( 
    int event_id,
    long long1, 
    long long2 );

// Network interface for mouse input.
int 
network_mouse_event( 
    int event_id, 
    long data1, 
    long data2 );

int 
network_register_ring3_osshell(
    struct cgroup_d *cg,
    pid_t caller_pid );
    
int 
network_register_ring3_network_server(
    struct cgroup_d *cg,
    pid_t caller_pid );
    
// Register display servver into a given valid cgroup.
int 
network_register_ring3_display_server(
    struct cgroup_d *cg,
    pid_t caller_pid );

// Register browser into a given valid cgroup.
int 
network_register_ring3_browser(
    struct cgroup_d *cg,
    pid_t caller_pid );

void 
network_fill_mac(
    unsigned char *to, 
    unsigned char *from );
void 
network_fill_ipv4(
    unsigned char *to,
    unsigned char *from );
void 
network_fill_ipv6(
    unsigned char *to,
    unsigned char *from );

unsigned short 
net_checksum(
    const unsigned char *phdr, 
    int phdr_len, 
    const unsigned char *start, 
    const unsigned char *end);

void network_test_NIC(void);

void 
network_send_raw_packet (
    struct intel_nic_info_d *dev, 
    size_t frame_len, 
    const char *frame_address );

//
// Handling packets
//

// Called when a packet came in from a device driver.
int 
network_on_receiving ( 
    const unsigned char *frame, 
    ssize_t frame_size );

// Called when sending some raw packet.
// #ps: We do NOT send, we're called by the sending routines.
int network_on_sending (void);

// Push and Pop data.
int network_push_packet( void *src_buffer, int len );
int network_pop_packet( void *u_buffer, int len );

void networkSetStatus(int status);
int networkGetStatus(void);

void networkLock (void);
void networkUnlock (void);
int networkIsLocked (void);

void networkSetOnlineStatus(int status);
int networkGetOnlineStatus(void);

void networkUpdateCounter(int the_counter);

int netInitialize(void);

#endif    

