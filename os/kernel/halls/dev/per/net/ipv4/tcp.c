// tcp.c
// Created by Fred Nora.

//
// The 3-Step Handshake Process:
//
// (1) Client to Server (SYN=1, ACK=0): 
//     The client initiates the connection by sending this segment.
//
// (2) Server to Client (SYN=1, ACK=1): 
//     The server replies with its own synchronization and 
//     acknowledges the client's packet .
//
// (3) Client to Server (SYN=0, ACK=1): 
//     The client sends a final acknowledgment. The connection is 
//     now "ESTABLISHED" and data transfer can begin .
//

#include <kernel.h>


// Temporary definition for testing flow control
//#define TCP_WINDOW_TEST 1024   // advertise 1 KB receive window
#define __TEMPORARY_TCP_WINDOW_TEST  512


// For testing only: track the last connection we created.
// Later this will be replaced with endpoint-based lookup.
static struct connection_d *cur_conn = NULL;

const unsigned short __first_ephemeral_port = 32768;
const unsigned short __last_ephemeral_port = 32999;
static unsigned short __new_client_port_number = 32768; // Inicial

//static char __tcp_payload[1024];
static char __tcp_payload[1400];   // or 1460

static unsigned int __get_random_32bit(void);
static unsigned int __generate_ISN(void);

static uint16_t 
__tcp_checksum(
    uint8_t src_ip[4],
    uint8_t dst_ip[4],
    const uint8_t *tcp_segment,
    size_t tcp_len );

static void 
__kd_handle_tcp( 
    const unsigned char *buffer, 
    ssize_t size,
    unsigned int s_ipv4_int,
    unsigned int d_ipv4_int );

// ===================================================


static unsigned int __get_random_32bit(void)
{
    unsigned int rv;
    unsigned int Seed = (unsigned int) (jiffies & 0xFFFFFFFF);
    Seed = (Seed + foreground_thread);
    srand(Seed);
    rv = (unsigned int) rand();

    return (unsigned int) rv;
}

// Pseudo‑Random Number Generator.
// Generate a 32bit random Initial Sequence Number (ISN).
// RFC 6528-style ISN generation
static uint32_t __generate_ISN(void)
{
    // Monotonic counter (ticks since boot, scaled)
    static uint32_t isn_counter = 0;
    isn_counter += 64000;   // ~64k per second, ensures steady growth

    // Random salt (from PRNG or entropy source)
    uint32_t salt = __get_random_32bit();  // kernel PRNG, not libc rand()

    // Mix counter and salt
    uint32_t isn = isn_counter ^ salt;

    return isn;
}


/*
static uint16_t 
__tcp_checksum(
    uint8_t src_ip[4],
    uint8_t dst_ip[4],
    const uint8_t *tcp_segment,
    size_t tcp_len )
{
    uint32_t sum = 0;
    size_t i=0;

    // Pseudo-header
    sum += (src_ip[0] << 8) | src_ip[1];
    sum += (src_ip[2] << 8) | src_ip[3];
    sum += (dst_ip[0] << 8) | dst_ip[1];
    sum += (dst_ip[2] << 8) | dst_ip[3];
    sum += 6;               // Protocol = TCP
    sum += tcp_len;

    // TCP header + payload
    for (i=0; i < tcp_len; i += 2) {
        uint16_t word = tcp_segment[i] << 8;
        if (i+1 < tcp_len) word |= tcp_segment[i+1];
        sum += word;
    }

    // Fold 32-bit sum to 16 bits
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);

    return (uint16_t)(~sum);
}
*/

static uint16_t 
__tcp_checksum(
    uint8_t src_ip[4],
    uint8_t dst_ip[4],
    const uint8_t *tcp_segment,   // TCP header + data, with checksum field = 0
    size_t tcp_len)
{
    uint32_t sum = 0;
    size_t i;

    // Pseudo-header
    sum += (src_ip[0] << 8) | src_ip[1];
    sum += (src_ip[2] << 8) | src_ip[3];
    sum += (dst_ip[0] << 8) | dst_ip[1];
    sum += (dst_ip[2] << 8) | dst_ip[3];
    sum += 6;                    // TCP protocol
    sum += (uint16_t)tcp_len;    // TCP length

    // TCP header + data (checksum field must be 0)
    for (i = 0; i < tcp_len; i += 2) {
        uint16_t word = (tcp_segment[i] << 8);
        if (i + 1 < tcp_len)
            word |= tcp_segment[i + 1];
        sum += word;
    }

    // If odd length, pad with zero (already handled by the loop above)

    // Fold 32-bit sum
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return (uint16_t)(~sum);
}

void test_sending_tcp(void)
{
    unsigned short SourcePort = 11888;
    char payload[4];
    payload[0]=0;
    payload[1]=0;
    size_t payload_len = 0;  // 1

    printk("test_sending_tcp: sending SYNs to external targets\n");

    // Example sequence/ack numbers
    tcp_seq our_seq = __generate_ISN();  // initial sequence number
    tcp_ack ack = 0;  //_seq_number + 1;   // acknowledge client’s ISN

    uint8_t google_ip[4] = {142, 250, 190, 46};
    network_send_tcp(
        dhcp_info.your_ipv4,
        google_ip,
        NetworkSaved.gateway_mac,
        SourcePort,   // source port
        80,           // dest port
        our_seq,      // seq
        ack,          // ack
        TH_SYN,       // flags
        payload,      // no payload
        payload_len
    );

    // Cloudflare Web (HTTPS)
    uint8_t cf_ip[4] = {104, 16, 132, 229};
    network_send_tcp(
        dhcp_info.your_ipv4,
        cf_ip,
        NetworkSaved.gateway_mac,
        SourcePort,
        443,
        our_seq,   // our seq
        ack,       // ack
        TH_SYN,
        payload,
        payload_len
    );

    // Microsoft Azure (HTTP)
    uint8_t ms_ip[4] = {20, 112, 52, 29};
    network_send_tcp(
        dhcp_info.your_ipv4,
        ms_ip,
        NetworkSaved.gateway_mac,
        SourcePort,
        80,
        0x3000,
        0,
        TH_SYN,
        payload,
        payload_len
    );

    // Wikipedia (HTTPS)
    uint8_t wiki_ip[4] = {208, 80, 154, 224};
    network_send_tcp(
        dhcp_info.your_ipv4,
        wiki_ip,
        NetworkSaved.gateway_mac,
        SourcePort,
        443,
        0x4000,
        0,
        TH_SYN,
        payload,
        payload_len
    );
}

// Low level worker
int
network_send_tcp ( 
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    uint8_t target_mac[6], 
    unsigned short source_port,
    unsigned short target_port,
    tcp_seq seq,
    tcp_ack ack,
    uint16_t flags,
    char *data_buffer, 
    size_t data_lenght )
{
// Buffers: [ethernet, ipv4, tcp, data]

    register int i=0;
    int j=0;
    char *data = (char *) data_buffer;  // TCP payload

//==============================================

// #todo
// NIC Intel device structure.

    if ((void *) currentNIC == NULL){
        printk("network_send_tcp: currentNIC\n");
        //goto fail;
        return -1;
    }

// #ps:
// Saving the sender IP into the NIC structure.
// Why are we doing this?
    currentNIC->ip_address[0] = source_ip[0];
    currentNIC->ip_address[1] = source_ip[1];
    currentNIC->ip_address[2] = source_ip[2];
    currentNIC->ip_address[3] = source_ip[3];

//==============================================
    if ((void*) data == NULL){
        printk ("network_send_tcp: Invalid data buffer\n");
        //goto fail;
        return -1;
    }

    // #debug
    printk("TCP SEND: %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d flags=%x seq=%u ack=%u\n",
       source_ip[0],source_ip[1],source_ip[2],source_ip[3],source_port,
       target_ip[0],target_ip[1],target_ip[2],target_ip[3],target_port,
       flags, seq, ack );

// ==============================================
// ethernet header:

    struct ether_header  Leh;

    for (i=0; i<6; i++){
        Leh.mac_dst[i] = (uint8_t) target_mac[i];               // dest
        Leh.mac_src[i] = (uint8_t) currentNIC->mac_address[i];  // source 
    };
    Leh.type = (uint16_t) ToNetByteOrder16(ETHERTYPE_IPV4);

// ==============================================
// ipv4 header:

    struct ip_d  Lipv4;

    Lipv4.v_hl = 0x45;    // Version (8bits)

    // Type of service (8bits)
    // - Differentiated Services Code Point (6bits)
    // - Explicit Congestion Notification (2bits)
    Lipv4.ip_tos = 0x00;  // 8 bit (0=Normal)

    // IPV4 Total lenght (16bits)
    // (IP + (TCP + data)) given in bytes.
    // 20~65535
    // This 16-bit field defines the entire packet size in bytes, 
    // including header and data. 
    // The minimum size is 20 bytes (header without data) and 
    // the maximum is 65,535 bytes.

    uint16_t ip_total_len = IP_HEADER_LENGHT + TCP_HEADER_LENGHT + data_lenght;
    Lipv4.ip_len = ToNetByteOrder16(ip_total_len);

    // Identification (16bits)
    // When the message is large and we have a lot of packets.
    // Actually they are 'fragments' of a packet.

    //uint16_t ipv4count = 1; 
    //ipv4->ip_id = 0x0100;
    //ipv4->ip_id = (uint16_t) ToNetByteOrder16(ipv4count);

    Lipv4.ip_id = 0;

    // Fragments (16bits)
    // Flags (3bits) (Do we have fragments?)
    // Fragment offset (13bits) (fragment position)
    // Don't fragment for now.
    Lipv4.ip_off = ToNetByteOrder16(0x4000);  //DF bit 

    Lipv4.ip_ttl = 255;  // Time to live (8bits)
    Lipv4.ip_p = 0x06;   // Protocol is TCP (8bit)

// Addresses

    unsigned char *spa = (unsigned char *) &Lipv4.ip_src.s_addr;
    unsigned char *tpa = (unsigned char *) &Lipv4.ip_dst.s_addr;
    register int it=0;
    for (it=0; it<4; it++){
        spa[it] = (uint8_t) source_ip[it]; 
        tpa[it] = (uint8_t) target_ip[it]; 
    };

//
// Checksum
//

    Lipv4.ip_sum = 0;
    Lipv4.ip_sum =
         (uint16_t)  net_checksum(
              0, 
              0,
              (const unsigned char *) &Lipv4, 
              (const unsigned char *) &Lipv4 + sizeof(struct ip_d) );
    // #ps: Change byte order
    Lipv4.ip_sum = (uint16_t) ToNetByteOrder16(Lipv4.ip_sum);


    // #debug
    // printk("ip_sum={%x} \n",Lipv4.ip_sum);
    // printk ("size %d\n", sizeof (struct ip_d) );
    // refresh_screen();
    // while(1){}

// ==============================================
// tcp header:

    struct tcp_d  Ltcp;

    // Ports (16,16)
    Ltcp.th_sport = (uint16_t) ToNetByteOrder16(source_port);
    Ltcp.th_dport = (uint16_t) ToNetByteOrder16(target_port);

    // Sequence number and Acknowledgement number. (32,32)
    Ltcp.th_seq = ToNetByteOrder32(seq);
    Ltcp.th_ack = ToNetByteOrder32(ack);

    // Flags (16bit)
    // 4,6,6
    // data offset (4) | reserved (6) | control_flags (6)
    // -----------------------------
    // data offset (4): 
    //  + Specifies the size of the TCP header in 32-bit words. 
    // reserved (6): 
    // + ?
    // Control bits (6):  
    //  + We use them to establish connections, 
    //  + send data and 
    //  + terminate connections.

    //Ltcp.do_res_flags = ToNetByteOrder16(flags | (5 << 12));
    Ltcp.do_res_flags = 0;  // clear first
    Ltcp.do_res_flags = (5 << 12) | flags;   // data offset = 5 (20 bytes), no options
    Ltcp.do_res_flags = ToNetByteOrder16(Ltcp.do_res_flags);

// Window size (16 bits)
// Telling the peer: 
// “I have this much buffer space available for incoming data.”
// FIN flag → “I’m done sending data from my side.”
// Window size = 0 → “I have no buffer space left, don’t send me more data.”

    //Ltcp.window_size = ToNetByteOrder16(TCP_WINDOW_SIZE);  // max window
    Ltcp.window_size = ToNetByteOrder16(__TEMPORARY_TCP_WINDOW_TEST);  // max window

/*
    if (flags & TH_FIN) {
        Ltcp.window_size = ToNetByteOrder16(0);
    } else {
        Ltcp.window_size = ToNetByteOrder16(TCP_WINDOW_SIZE);  // max window
        //Ltcp.window_size = ToNetByteOrder16(cur_conn->tcp_conn->rcv_wnd);
    }
*/

    // Checksum (16 bits)
    // We will calculate at the end of the routine.
    Ltcp.checksum = 0;

    Ltcp.urgent_pointer = 0;  // Urgent pointer (16 bits)

//
// Checksum
//

    Ltcp.checksum = 0;  //#todo
    uint8_t tcp_segment[TCP_HEADER_LENGHT + data_lenght];
    // Inject the tcp header
    memcpy(tcp_segment, &Ltcp, TCP_HEADER_LENGHT);
    // Inject the tcp payload
    if (data_lenght > 0 && data_buffer != NULL)
    {
        memcpy(tcp_segment + TCP_HEADER_LENGHT, data_buffer, data_lenght);
    }
    // Calculate the checksum
    Ltcp.checksum = 
        __tcp_checksum( 
            source_ip, 
            target_ip, 
            tcp_segment, 
            sizeof(tcp_segment) 
        );
    
    // Byte swapping
    // This is necessary because the checksum must be 
    // transmitted in network byte order.
    Ltcp.checksum = (uint16_t) ToNetByteOrder16(Ltcp.checksum);

    //printk ("size %d\n", sizeof (struct  udp_d) );
    //refresh_screen();
    //while(1){}

// ----------------------------------------------------

//
// Buffer
//

// Let's call it 'frame'.
// Because we're sending a 'frame'.

// ??
// Quem?
// Estamos pegando o offset que nos levara ao endereço do buffer.
// Usaremos esse offset logo abaixo.
// Pegamos esse offset na estrutura do controlador nic intel.
// Copiando o pacote no buffer.
// Pegando o endereco virtual do buffer na estrutura do controlador 
// nic intel. Para isso usamos o offset obtido logo acima.

    uint16_t buffer_index = (uint16_t) currentNIC->tx_cur;

// Get the buffer address based on its offset
    unsigned char *frame = 
        (unsigned char *) currentNIC->tx_buffers_virt[buffer_index];

    //#debug
    //printk ("buffer_index {%d}\n",buffer_index);

// Prepare pointer to inject structure into the buffer
    unsigned char *src_ethernet = (unsigned char *) &Leh;    // eh
    unsigned char *src_ipv4     = (unsigned char *) &Lipv4;  // ipv4 
    unsigned char *src_tcp      = (unsigned char *) &Ltcp;   // tcp 

//
// Copy
//

    // #debug: Actuall we don't need panic here
    if ((void*) frame == NULL)
    {
        panic("network_send_tcp: frame\n");
        //printk("network_send_tcp: frame\n");
        //goto fail;
    }

// Inject data structure into the buffer
// Step1: Inject ethernet header
// Step2: Inject ipv4 header
// Step3: Inject tcp header
// Step4: Inject tcp payload

// Step1: Inject ethernet header
    int eth_offset=0;
    for ( j=0; j<ETHERNET_HEADER_LENGHT; j++ ){
        frame[eth_offset +j] = src_ethernet[j];
    };

// Step2: Inject ipv4 header
    int ipv4_offset = ETHERNET_HEADER_LENGHT;
    for ( j=0; j<IP_HEADER_LENGHT; j++ ){
        frame[ipv4_offset +j] = src_ipv4[j];
    };

// Step3: Inject tcp header
    int tcp_offset = ETHERNET_HEADER_LENGHT + IP_HEADER_LENGHT;
    for ( j=0; j<TCP_HEADER_LENGHT; j++ ){
        frame[tcp_offset +j] = src_tcp[j];
    };

// Step4: Inject tcp payload
    int data_offset = 
            ( ETHERNET_HEADER_LENGHT +
              IP_HEADER_LENGHT +
              TCP_HEADER_LENGHT );
    for ( j=0; j<data_lenght; j++ ){
        frame[data_offset +j] = data[j];
    };


// ---------------------------------------
// send
// lenght:
// Vamos configurar na estrutura do nic intel o tamanho do pacote.
// Lenght de um pacote ipv4.
// ethernet header, ipv4 header, udp header, data.
// 14 + 20 + 6 + 512 = 552.
    size_t FRAME_SIZE = 
               ( ETHERNET_HEADER_LENGHT +\
                 IP_HEADER_LENGHT +\
                 TCP_HEADER_LENGHT +\
                 data_lenght );

//
// Check
//

    // #todo
    //if ((void*) frame == NULL)
    //    goto fail;
    //if (FRAME_SIZE == 0)
    //    goto fail;

//
// Send
//

// #bugbug
// I guess we don't need the routine above.
// It's because ethernet_send() will put the given data into 
// the right place.

// Send frame via current NIC
    int rv = -1;
    rv = (int) ethernet_send(FRAME_SIZE, frame);
    if (rv < 0){
        printk("network_send_tcp: on ethernet_send()\n");
        goto fail;
    }

// #debug
// Send frame to myself.
    //network_on_receiving(frame,FRAME_SIZE);
    //refresh_screen();
    //while(1){}

// #test
    //kfree(eh);
    //kfree(udp);
    //kfree(udp);

    printk("TCP: Done\n");  // #debug
    return 0;

fail:
    printk ("TCP: Fail\n");  // #debug
    return -1;
}


/*
 * tcp_socket_send:
 *   Send application data over an established AF_INET TCP socket.
 *   IN:  sk   - local client socket (must be SS_CONNECTED)
 *        buf  - user payload
 *        len  - payload length
 *   OUT: bytes sent (>=0) or negative errno
 */
// Send data to a REMOTE peer.
// We get the connection information based on the client's socket.

int 
tcp_socket_send(
    struct socket_d *sk, 
    const char *buf, 
    size_t len )
{
    struct connection_d *conn;
    struct socket_d *local_sk;
    struct socket_d *remote_sk;
    uint8_t target_ip[4];
    tcp_seq seq;
    tcp_ack ack;
    uint16_t Flags = 0;
    int rv;

    local_sk = sk;
    remote_sk = NULL;

    if ((void *) local_sk == NULL)
        return -EINVAL;
    if (local_sk->magic != 1234)
        return -EINVAL;
    if ((void *) buf == NULL)
        return -EINVAL;
    if (len == 0)
        return 0;

    // Only AF_INET stream sockets that finished the handshake
    if (local_sk->family != AF_INET)
        return -EAFNOSUPPORT;
    if (local_sk->state != SS_CONNECTED){
        printk("tcp_socket_send: Cant send. socket unconnected\n");
        return -ENOTCONN;
    }

// Connection:
// Find the connection that owns this local socket
    conn = tcp_find_connection_by_local_socket(sk);

    if ((void *) conn == NULL)
        return -ENOTCONN;
    if (conn->magic != 1234)
        return -ENOTCONN;
    if (conn->status == CONN_STATUS_CLOSED){
        printk("tcp_socket_send: Cant send. Connection is closed\n");
        return -ENOTCONN;
    }
    if (conn->status != CONN_STATUS_ESTABLISHED)
        return -ENOTCONN;

    if ((void *) conn->tcp_conn == NULL)
        return -ENOTCONN;
    if ((void *) conn->ep_pair == NULL)
        return -ENOTCONN;
    if ((void *) conn->ep_pair->s_ep == NULL)
        return -ENOTCONN;

    remote_sk = conn->ep_pair->s_ep->socket;
    if ((void *) remote_sk == NULL)
        return -ENOTCONN;
    if (remote_sk->magic != 1234)
        return -ENOTCONN;

// Target ip (array)
// Host-order IP → dotted octets
    target_ip[0] = (uint8_t) ((remote_sk->ip_ipv4 >> 24) & 0xFF);
    target_ip[1] = (uint8_t) ((remote_sk->ip_ipv4 >> 16) & 0xFF);
    target_ip[2] = (uint8_t) ((remote_sk->ip_ipv4 >>  8) & 0xFF);
    target_ip[3] = (uint8_t) ( remote_sk->ip_ipv4        & 0xFF);

    seq = conn->tcp_conn->snd_nxt;  // (next sequence number to send)
    ack = conn->tcp_conn->rcv_nxt;  // (next expected acknowledgment)

    // Relative values
    // sent_offset shows how much of your data has been acknowledged.
    size_t sent_offset = conn->tcp_conn->snd_una - conn->tcp_conn->iss;
    // recv_offset shows how much of the peer’s data you’ve consumed.
    size_t recv_offset = conn->tcp_conn->rcv_nxt - conn->tcp_conn->irs;

    //printk("tcp_socket_send: seq=%u ack=%u sent_offset=%u recv_offset=%u\n",
       //seq, ack, sent_offset, recv_offset );

// ======================================================

    printk("tcp_socket_send: %d sends to %d\n", 
        local_sk->port, remote_sk->port );

    printk("tcp_socket_send: seq=%u (offset=%u) ack=%u (offset=%u)\n",
        seq,
        seq - conn->tcp_conn->iss,
        ack,
        ack - conn->tcp_conn->irs);


    Flags = (TH_ACK | TH_PUSH);

    rv = 
    network_send_tcp(
        dhcp_info.your_ipv4,       // source IP
        target_ip,                 // destination IP
        NetworkSaved.gateway_mac,  // next-hop MAC (gateway for WAN)
        local_sk->port,
        remote_sk->port,
        seq,
        ack,
        (uint16_t) Flags,
        (char *) buf,
        len
    );

    if (rv < 0)
        return rv;

    // Data consumes sequence space
    conn->tcp_conn->snd_nxt += (tcp_seq) len;
    conn->packets_sent++;

    return (int) len;
}

// #test: Getting the last payload available...
// allowing the ring 3 process to read it.
int tcp_socket_recv(struct socket_d *sk, char *buf, size_t len)
{
    if (!sk || sk->magic != 1234) 
        return -EINVAL;
    if (sk->state != SS_CONNECTED)
        return -ENOTCONN;

    file *fp = sk->private_file;
    if (!fp) return -ENOTCONN;

    // How many bytes are available?
    size_t available = (fp->_w - fp->_r);
    if (available == 0) {
        return 0; // nothing to read yet
    }

    // Limit to user buffer size
    size_t to_copy = (len < available) ? len : available;

    memcpy(buf, fp->_base + fp->_r, to_copy);

    // Advance read offset
    fp->_r += to_copy;

    // If we consumed everything, reset flags
    if (fp->_r >= fp->_w) {
        fp->_r = fp->_w;
        fp->sync.can_read = FALSE;
    }

    return (int) to_copy;
}


// This will be called by sys_connect() when the family is AF_INET.
// see: __connect_inet_remote in socket.c
// IN:
//   sk              = local client socket structure.
//   dst_ip_ipv4_int = destination IPv4, HOST byte order (already ntohl'd by the caller).
//   dst_port        = destination port, HOST byte order (already ntohs'd by the caller).
int 
tcp_client_connect(
    struct socket_d *sk,
    unsigned int dst_ip_ipv4_int,
    unsigned short dst_port)
{
// WAN
// Send SYN

    struct socket_d *sk_local;
    struct socket_d *sk_remote;

    printk("tcp_client_connect: Client sends SYN\n");

    if (!sk)
        return -EINVAL;

    sk_local = sk;
    sk_remote = NULL;

// ------------------------------
// Create connection object
    struct connection_d *conn = create_connection(CONN_TYPE_TCP);
    if (!conn){
        printk("tcp_client_connect: conn\n");
        return -ENOMEM;
    }
    // Register connection
    int id = connection_register(conn);
    if (id < 0 || id >= MAX_CONNECTIONS) 
    {
        printk("tcp_client_connect: id\n");
        return -1;
    }              
    conn->status = CONN_STATUS_NONE;
    conn->tcp_conn->state = TCP_CLOSED;

// ------------------------------
// Create endpoint pair
    struct endpoint_pair_d *pair = create_endpoint_pair_object();
    if (!pair)
        return -ENOMEM;

// ------------------------------
// Local endpoint
// #ps: plugging the socket we already have.
    struct endpoint_d *local_ep = create_endpoint_object();
    if (!local_ep)
        return -ENOMEM;
    local_ep->is_remote = FALSE;
    // Plug socket for local ep
    local_ep->socket = sk_local;
    sk_local->conn = conn;
    sk_local->ep = local_ep;

// ------------------------------
// Remote endpoint
// Creating the socket for the remote ep.
    struct endpoint_d *remote_ep = create_endpoint_object();
    if (!remote_ep)
        return -ENOMEM;
    remote_ep->is_remote = TRUE;
    // Create socket for remote ep
    remote_ep->socket = NULL;
    //remote_ep->socket = create_socket_object();
    sk_remote = (struct socket_d *) create_socket_object();
    if (!sk_remote){
        return -ENOMEM;
    }
    sk_remote->family   = AF_INET;
    sk_remote->type     = SOCK_STREAM;
    sk_remote->protocol = IPPROTO_TCP;
    sk_remote->ip_ipv4  = dst_ip_ipv4_int;  // host order, consistent with rest of socket_d usage
    sk_remote->port     = dst_port;         // host order
    sk_remote->conn = conn;
    sk_remote->ep = remote_ep;

    remote_ep->socket = sk_remote;

    // Plug endpoints into pair
    pair->c_ep = local_ep;
    pair->s_ep = remote_ep;
    conn->ep_pair = pair;

    // Initialize sequence numbers
    tcp_seq our_seq = __generate_ISN();     // 1000 #test
    conn->tcp_conn->iss = our_seq;          // Our initial
    conn->tcp_conn->snd_una = our_seq;      // The las unackowledged byte 
    conn->tcp_conn->snd_nxt = our_seq + 1;  //

    // IRS (Initial Receive Sequence):
    // You don’t know the server’s ISN yet. 
    // Leave irs unset until you receive the SYN+ACK.
    conn->tcp_conn->irs = 0;
    conn->tcp_conn->rcv_nxt = 0;

    // ------------------------------------------------
    // Allocate an ephemeral local port for this client socket.
    // #todo: check for collisions against a real in-use table;
    // for now this is a simple wrap-around counter.
    if (__new_client_port_number < __first_ephemeral_port)
        panic ("__new_client_port_number <");
    if (__new_client_port_number > __last_ephemeral_port)
        panic("__new_client_port_number >");

    // Set
    sk_local->port = __new_client_port_number;
    // sk_local->port = 11888;  // Host bytes order
    // ------------------------------------------------

    // ------------------------------------------------
    // A bare SYN carries no application data, but
    // network_send_tcp() rejects data_buffer == NULL
    // outright, even when data_lenght is 0.
    // So we pass a zero-length dummy buffer instead of NULL,
    // matching the convention used for SYN/ACK and ACK sends
    // elsewhere in this file.
    char syn_payload[2];
    syn_payload[0] = 0;
    syn_payload[1] = 0;
    size_t syn_payload_len = 0;   // Pure SYN: no data.
    // ------------------------------------------------
  
  
    // Convert the destination IP (host order int) into the
    // dotted-octet array network_send_tcp() expects.
    // #important: do NOT cast the int's address to uint8_t* —
    // that reads the machine's native byte layout, not the
    // network octet order. Build the array explicitly instead.
    uint8_t target_ip[4];
    target_ip[0] = (uint8_t) ((dst_ip_ipv4_int >> 24) & 0xFF);
    target_ip[1] = (uint8_t) ((dst_ip_ipv4_int >> 16) & 0xFF);
    target_ip[2] = (uint8_t) ((dst_ip_ipv4_int >> 8)  & 0xFF);
    target_ip[3] = (uint8_t) ( dst_ip_ipv4_int        & 0xFF);


// Send SYN:
// (1st step)

    // #debug
    printk("TCP: [Send SYN] s=%u (sport), d=%u (dport)\n", 
        sk_local->port, dst_port );

    int rv = 
    network_send_tcp(
        dhcp_info.your_ipv4,   // source IP (array)
        target_ip,             // target IP (array, correctly ordered)
        NetworkSaved.gateway_mac,
        sk_local->port,  // 11888 (host order) 
        dst_port,  // host order
        conn->tcp_conn->iss,  // seq
        0,                    // ack
        TH_SYN,
        syn_payload,
        syn_payload_len
    );

    if (rv < 0){
        return rv;
    }

    conn->status = CONN_STATUS_SYN_SENT;
    conn->tcp_conn->state = TCP_SYN_SENT;

// #test
// Remember this connection for the SYN-ACK handler to find.
    cur_conn = conn;

//
// Socket
//

    sk_local->state = SS_CONNECTING;

    printk("Done\n");
    return 0;
}


/*
 * tcp_change_socket_buffer:
 *   Safely enlarge the private_file buffer of a socket
 *   so it can hold multi-segment TCP data (up to 8 KB).
 *
 *   Intended to be called once the connection reaches
 *   ESTABLISHED state (client or server side).
 *
 *   IN:  sk  - socket that will receive remote data
 *   OUT: 0 on success, negative on failure
 */

int tcp_change_socket_buffer(struct socket_d *sk, size_t desired_size)
{
    file *fp;
    char *old_base;
    char *new_base;
    size_t old_size;
    size_t new_size = desired_size;  //8192;          // target size for remote TCP
    size_t copy_len;

    // -------------------------------------------------
    // 1. Validate the socket
    // -------------------------------------------------
    if ((void *) sk == NULL) {
        printk("tcp_change_socket_buffer: sk == NULL\n");
        return -EINVAL;
    }
    if (sk->magic != 1234) {
        printk("tcp_change_socket_buffer: sk validation failed\n");
        return -EINVAL;
    }

// Minimum
    if (new_size < BUFSIZ)
        new_size = BUFSIZ;

    // -------------------------------------------------
    // 2. Validate the private_file
    // -------------------------------------------------
    fp = sk->private_file;
    if ((void *) fp == NULL) {
        printk("tcp_change_socket_buffer: private_file == NULL\n");
        return -ENOENT;
    }
    if (fp->magic != 1234) {
        printk("tcp_change_socket_buffer: private_file validation failed\n");
        return -EINVAL;
    }
    if ((void *) fp->_base == NULL) {
        printk("tcp_change_socket_buffer: _base == NULL\n");
        return -EINVAL;
    }

    // Already large enough → nothing to do
    if (fp->_lbfsize >= (int) new_size) {
        //printk("tcp_enlarge_socket_buffer: already large enough (%d)\n",
        //       fp->_lbfsize);
        return 0;
    }

/*
    // Sanity on current offsets
    if (fp->_w < 0 || fp->_r < 0 ||
        fp->_w > fp->_lbfsize ||
        fp->_r > fp->_lbfsize)
    {
        printk("tcp_change_socket_buffer: corrupt offsets (_r=%d _w=%d _lbfsize=%d)\n",
               fp->_r, fp->_w, fp->_lbfsize);
        return -EINVAL;
    }
*/

    // -------------------------------------------------
    // 3. Allocate the new larger buffer
    // -------------------------------------------------
    new_base = (char *) kmalloc(new_size);
    if ((void *) new_base == NULL) {
        printk("tcp_change_socket_buffer: kmalloc(%u) failed\n",
               (unsigned) new_size);
        return -ENOMEM;
    }
    memset(new_base, 0, new_size);

    // -------------------------------------------------
    // 4. Copy existing data (if any)
    // -------------------------------------------------
    old_base = fp->_base;
    old_size = (size_t) fp->_lbfsize;

/*
    // How many bytes are currently valid?
    copy_len = (size_t) fp->_w;          // everything up to the write pointer
    if (copy_len > old_size)
        copy_len = old_size;
    if (copy_len > new_size)
        copy_len = new_size;
    if (copy_len > 0)
        memcpy(new_base, old_base, copy_len);
*/

    // -------------------------------------------------
    // 5. Install the new buffer
    // -------------------------------------------------
    fp->_base = new_base;
    fp->_p = new_base;
    fp->_lbfsize = (int) new_size;

    // #test
    fp->_r = 0;
    fp->_w = 0;
    fp->_cnt = fp->_lbfsize;

    // Keep the existing read/write pointers (they are still valid
    // relative to the beginning of the buffer).
    // We do NOT touch _r or _w here.

    // Optional but useful: update remaining space counter
    // if your file structure uses it.
    // fp->_cnt = new_size - fp->_w;

    // -------------------------------------------------
    // 6. Free the old buffer (only if it was dynamic)
    // -------------------------------------------------
    // Safety note:
    // - If the original buffer was allocated with kmalloc, free it.
    // - If it came from a static/slab area, do NOT free it.
    // For the moment we free it, because socket private_files
    // created by the normal path are dynamic.
    // If you later introduce static buffers, add a flag to the
    // file structure to decide whether kfree is safe.
    if (old_base != NULL) {
        // kfree(old_base);   // enable when you are sure it is safe
    }

    printk("tcp_change_socket_buffer: socket %p enlarged to %u bytes\n",
           sk, (unsigned) new_size);

    return 0;
}

//
// $
// HANDLER
//

// When receiving tcp frame from NIC device.
// Transport layer.
// The TCP header tracks the state of 
// communication between two TCP endpoints.

// Kernel debugger 11888
static void 
__kd_handle_tcp( 
    const unsigned char *buffer, 
    ssize_t size,
    unsigned int s_ipv4_int,
    unsigned int d_ipv4_int )
{
    struct tcp_d *tcp;  // The buffer
    //register int i=0;
    uint16_t flags=0;
    size_t data_len = 0;

    // No payload for handshake
    char dummy_payload[2];
    dummy_payload[0] = 0x00;
    dummy_payload[1] = 0;

    // #debug
    //printk("__kd_handle_tcp: #todo\n");

// Parameters
    if ((void*) buffer == NULL){
        printk("__kd_handle_tcp: buffer\n");
        return;
    }
    //if (size < 0){
    //}

    // Pointer for the TCP header. Pre-allocated.
    tcp = (struct tcp_d *) buffer;

    uint16_t sport = (uint16_t) FromNetByteOrder16(tcp->th_sport);
    uint16_t dport = (uint16_t) FromNetByteOrder16(tcp->th_dport);

    //printk("TCP Packet: remote=%u (sport), local=%u (dport)\n", tcp->th_sport, tcp->th_dport);
    printk("TCP: [Receiving] s=%u (sport), d=%u (dport)\n", 
        sport, dport );

//
// Super drop
// Only the kernel debugger is allowed
//

    if (dport != 11888){
        printk("__kd_handle_tcp: Invalid port\n");
        return;
    }

// These are extracting the sequence number and 
// acknowledgment number from the TCP header of an incoming packet.

// Sequence Number (th_seq):
// It marks the position of the first byte of data in this segment 
// within the sender’s byte stream.
// Ex: If the packet carries 100 bytes and th_seq = 5000, 
//     then the bytes are numbered 5000–5099.

// Acknowledgment Number (th_ack):
// It tells the sender: 
// “I have successfully received everything up to this byte number minus one.”
// Ex: If _ack_number = 5099, it means the receiver has received 
// all bytes up to 5098 and is expecting 5099 next.

    tcp_seq _seq_number = (tcp_seq) FromNetByteOrder32(tcp->th_seq);
    tcp_ack _ack_number = (tcp_ack) FromNetByteOrder32(tcp->th_ack);

    // Clear the payload local buffer
    memset(__tcp_payload, 0, sizeof(__tcp_payload));


    // #test: Doing this instead of TCP_HEADER_LENGHT.
    //size_t header_len =
        //((FromNetByteOrder16(tcp->do_res_flags) >> 12) & 0xF) * 4;

    //if (header_len != 20)
        //printk("TCP: header_len\n");

// Create a local copy of the TCP payload.
// The size is 1024

    if (size >= TCP_HEADER_LENGHT){
    //if (size >= header_len){

        data_len = (size - TCP_HEADER_LENGHT);
        //data_len = (size - header_len);
        if (data_len >= 1400)
            data_len = 1400 -2;
        strncpy( __tcp_payload, (buffer + TCP_HEADER_LENGHT), data_len );
        //strncpy( __tcp_payload, (buffer + header_len), data_len );
        __tcp_payload[data_len + 1] = 0;
        //__tcp_payload[1400 -1] = 0;
        __tcp_payload[1400 -1] = 0;
    } 
    //if (size < header_len)
    if (size < TCP_HEADER_LENGHT)
    {
        //data_len = 0;
        //__tcp_payload[data_len + 1] = 0;
        // #bugbug: Drop it
        printk("TCP: Invalid buffer size %d\n", size);
        return;
    }

    // Window: The client can only accept this n bytes
    uint16_t peer_window = (uint16_t) FromNetByteOrder16(tcp->window_size);

    // #test:
    //if (peer_window == 0)
        //return;

//
// Flags
//

// FIN  - graceful close,
// SYN  - start handshake,
// RST  - abort,
// PUSH - deliver data now,
// ACK  - update state,
// URG  - handle urgent data.

// Control flags (6 bits)
    uint16_t fFIN=0;
    uint16_t fSYN=0;  // SYN :)
    uint16_t fRST=0;
    uint16_t fPUSH=0;
    uint16_t fACK=0;  // ACK :)
    uint16_t fURG=0;

    flags = (uint16_t) FromNetByteOrder16(tcp->do_res_flags);

    //#debug
    //printk("Flags={%x}\n",flags);

// Receiving a FIN means the peer is done sending; we should acknowledge and
// eventually close our side gracefully.
    if (flags & TH_FIN){
        fFIN = 1;
    }

// Receiving a SYN means the peer wants to start a new connection; respond
// with SYN+ACK if we are listening.
    if (flags & TH_SYN){
        fSYN = 1;
    }

// Receiving a RST means the peer wants to abort/reset the connection; tear
// down state immediately and stop using this socket.
    if (flags & TH_RST){
        fRST = 1;
    }

// Receiving a PUSH means the peer wants the data delivered immediately;
// pass buffered data up to the application without delay.
    if (flags & TH_PUSH){
        fPUSH = 1;
    }

// Receiving an ACK means the peer is acknowledging our sent data or handshake;
// update sequence numbers and possibly advance connection state.
    if (flags & TH_ACK){
        fACK = 1;
    }

// Receiving URG means urgent data is present; handle the urgent pointer and
// notify the application of out‑of‑band data.
    if (flags & TH_URG){
        fURG = 1;
    }

    // ex: 5014H
    // 0101 0000 0001 0100

// Initializing connection
// 1) SYN      >>
// 2) SYN/ACK  <<
// 3) ACK      >>

// Finalizing connection
// 1) FIN >>
// 2) ACK <<
// 3) FIN <<
// 4) ACK >>


//If source IP = 0 → 
//You don’t know who the peer is. 
//You cannot establish a connection. Drop the packet or log an error.
//If destination IP = 0 → 
//You don’t know which local endpoint this packet is for. Drop it.
//If both are 0 → 
//Treat as invalid input. Do not attempt handshake.

//
// ports
//

    //#debug
    //printk("TCP: sport{%d}   #debug\n",sport);
    //printk("TCP: dport{%d}   #debug\n",dport);

    // #todo: Ignore it for now
    if (dport == 443){
        return;  // No verbose
    }
    // #todo: Ignore it for now
    if (dport == 80)
    {
        printk("TCP: dport{%d} (Server not implemented yet)\n", dport);
        printk("SYN={%d} ACK={%d}\n", fSYN, fACK);
        //if ( fSYN == 1 && fACK == 1 ){
        //    printk("TCP: SYS/ACK received in port{%d}\n", dport);
        //}
        return;
    }

// Show

// Special port.
// Just a test.
// >> Connection request: 
// SYN=1, ACK=0
// >> Reply: 
// SYN=1, ACK=1

     printk("TCP: dport=%d SYN={%d} ACK={%d} FIN={%d}\n", 
        dport, fSYN, fACK, fFIN);

    if (dport == 11888)
    {
        printk("TCP_11888: SYN={%d} ACK={%d} FIN={%d}\n", fSYN, fACK, fFIN);

        // (1) SYN
        // Step 1 — SYN received (initialize, not increment)
        // A client is trying to initialize a new connection.
        // The client shares an Initial Sequence Number (ISN) with the server.
        // It means the server here needs to respond.
        if ( fSYN == 1 && fACK == 0 )
        {
            printk("\n");
            printk("TCP_SYN: SEQ={%d} | ACK={%d}\n", _seq_number, _ack_number );

            // Example sequence/ack numbers
            // #ps: It looks safer to test our generator now
            tcp_seq seq = 1000;  //__generate_ISN();  // initial sequence number
            tcp_ack ack = _seq_number + 1;   // acknowledge client’s ISN

            // -- connection structure ----
            struct connection_d *conn = create_connection(CONN_TYPE_TCP);
            if ((void*) conn == NULL){
                printk("Failed to create connection object\n");
                return; // do not respond
            }
            int id = connection_register(conn);
            // #todo: Check id validation
            if (id < 0 || id >= MAX_CONNECTIONS) 
            {
                printk("Failed to register connection\n");
                // free the object if needed
                //kfree(conn->tcp_conn);
                //kfree(conn);
                return; // do not respond
            }
            conn->type = CONN_TYPE_TCP;
            conn->status = CONN_STATUS_SYN_RECEIVED;

            // tcp connection structure
            if ((void*) conn->tcp_conn == NULL){
                printk("Failed to create TCP connection structure\n");
                return; // do not respond
            }
            conn->tcp_conn->state = TCP_SYN_RECEIVED;
            conn->packets_received++;
            // IRS → Initial Receive Sequence number
            conn->tcp_conn->irs     = _seq_number;      // client's ISN
            conn->tcp_conn->rcv_nxt = _seq_number + 1;  // SYN consumes 1 seq number
            // ISS → Initial Send Sequence number
            conn->tcp_conn->iss     = seq;  //1000;  //__generate_ISN();  // our ISN (or randomize later)
            conn->tcp_conn->snd_una = seq;  //conn->tcp_conn->iss;
            conn->tcp_conn->snd_nxt = seq + 1;  //conn->tcp_conn->iss + 1; // our SYN will consume 1

            conn->tcp_conn->snd_wnd = peer_window;  // Client's window size?
            //conn->tcp_conn->rcv_wnd -= 1;         // Maybe
            printk("Connection %d created, state=SYN_RECEIVED\n", id);                 

            // -- ep pair -----------
            struct endpoint_pair_d *pair = create_endpoint_pair_object();
            if (!pair) {
                printk("Failed to create endpoint pair\n");
                return; // do not respond
            }
            pair->case_id = CONN_LSRC; // Local Server → Remote Client

            // -- remote ep (client) ---------------
            // Remote client endpoint
            struct endpoint_d *client_ep = create_endpoint_object();
            client_ep->is_remote = TRUE;  // <<< REMOTE EP
            if (!client_ep) {
                printk("Failed to create remote endpoint\n");
                return; // do not respond
            }
            // Create a socket for the remote client
            struct socket_d *sk_client;
            sk_client = (struct socket_d *) create_socket_object();
            if ((void*) sk_client == NULL){
                panic("TCP: on sk_client\n");
                //return;
            }
            sk_client->family = AF_INET;
            sk_client->type = SOCK_STREAM;
            sk_client->protocol = IPPROTO_TCP;

            sk_client->conn = conn;     // Belongs to this connection
            sk_client->ep = client_ep;  // Belongs to this ep

            // Remote peer identity
            sk_client->pid = -1;   // remote client, not a local process
            sk_client->uid = 0;
            sk_client->gid = 0;

            // IP/Port
            // sk_client->ip_ipv6 = 0;
            sk_client->ip_ipv4 = s_ipv4_int;  //NetworkSaved.caller_ip_int;
            sk_client->port = sport;

            // Connection state
            sk_client->state   = SS_CONNECTING;
            sk_client->flags   = 0;
            sk_client->conn_copy = FALSE;

            // Backlog defaults
            sk_client->backlog_max = 0;
            sk_client->pending_client_count = 0;
            sk_client->pending_server_count = 0;
            // magic string? It indicates pending connection?
            // sk_client->magic_string[0] = 'C';

            // Plug the socket into the ep
            client_ep->socket = sk_client;

            // Plug the ep into the pair
            pair->c_ep = client_ep;

            // -- local ep (server) ---------------
            // Local server endpoint
            struct endpoint_d *server_ep = create_endpoint_object();
            server_ep->is_remote = FALSE;  // NOT REMOTE EP (LOCAL)
            if (!server_ep) {
                printk("__kd_handle_tcp: Failed to create local endpoint\n");
                return; // do not respond
            }
            // #todo:
            // This is the part where we get the socket 
            // from the listening server process. 
            // For now, we set it to NULL.
            // #todo: 
            // It should look up the process that registered itself as 
            // the listener for port 11888 and then grab the socket object 
            // that belongs to that process.
            pair->s_ep = server_ep;

            // --------------------------------------------

            // Plug them together
            conn->ep_pair = pair;

            // --------------------------------------------
            server_ep->socket = NULL;
            struct socket_d *sk_listener; 
            sk_listener = (struct socket_d *) socket_get_tcpserver_socket_by_port(11888);  // dport?
            if ((void*) sk_listener != NULL)
            {
                if (sk_listener->magic == 1234)
                {
                    sk_listener->pending_client_count++;
                    if (sk_listener->pending_client_count > sk_listener->backlog_max)
                    {
                        sk_listener->pending_client_count = 0;
                    }
                    int backlog_tail = sk_listener->pending_client_count;

                    sk_listener->pending_client_endpoints[backlog_tail] = 
                        client_ep->socket;

                    sk_listener->state = SS_CONNECTING;

                    sk_listener->conn = conn;     // Belongs to this connection
                    sk_listener->ep = server_ep;  // Belongs to this ep

                    server_ep->socket = sk_listener;   // Save into the ep
                }
            }

            // #important: Connection status.
            conn->status = CONN_STATUS_SYN_RECEIVED;

            // Our current connection been stablished.
            cur_conn = conn;  // remember this connection for later ACK

            // #todo
            // The client is saying: "I want to connect to the 
            // server process that is listening to the port 11888"

            // #todo
            // >>> Lets work on this response! <<<

            // Flags: SYN + ACK
            uint16_t flags = TH_SYN | TH_ACK;

            //
            printk("__kd_handle_tcp: Sending SYN/ACK >>\n");

            // Send SYN + ACK
            network_send_tcp(
                dhcp_info.your_ipv4,       // server IP
                NetworkSaved.caller_ipv4,  // client IP (Array)
                NetworkSaved.caller_mac,   // client MAC
                11888,           // server port (source=we)
                sport,           // client port (target)
                conn->tcp_conn->iss,      // seq = our ISN
                conn->tcp_conn->rcv_nxt,  // ack = client's ISN + 1
                flags,
                dummy_payload,  // No tcp payload
                0               // No tcp payload lenght char=0x00
            );

            conn->packets_sent++;

            // Waiting for the ACK:
            return;
        }

        // (2) SYN/ACK
        // A server accepted the connection.
        // We received a syn/ack as a response to
        // our syn sent by a process in this machine.
        // #todo: Apply the connection structure that handles this connection.
        // host   → remote : SYN
        // Remote → host   : SYN-ACK
        // host   → remote : ACK
        if (fSYN == 1 && fACK == 1)
        {
            printk("TCP_SYN_ACK: SEQ={%d} | ACK={%d}\n", _seq_number, _ack_number);
            printk("TCP_SYN_ACK: Sending final ACK\n");

            // We're the client here — the remote side acked our SYN and sent
            // its own SYN. Complete the handshake with the final ACK.

            tcp_seq final_seq = _ack_number;       // = our ISN + 1, given by the server's ack
            tcp_ack final_ack = _seq_number + 1;   // acknowledge the server's ISN

            // #test
            struct connection_d *c_conn =
                tcp_find_connection_by_remote_peer(s_ipv4_int, sport);
            //struct connection_d *c_conn = tcp_find_connection_by_client(s_ipv4_int, sport);  
            if (!c_conn){ 
                printk("step2: syn_ack fail\n");
                return; 
            }
            if (c_conn)
            {
                if (c_conn->magic != 1234) {
                    printk("TCP_SYN_ACK: [FAIL] connection validation\n");
                    return;
                }
                cur_conn = c_conn;
            }
            //if ((void*)cur_conn != NULL)
            //{
                // #todo: ...
                //cur_conn->packets_received++;
            //}

            // Send ACK?
            network_send_tcp(
                dhcp_info.your_ipv4,        // our IP
                NetworkSaved.caller_ipv4,   // remote IP (whoever this packet came from)
                NetworkSaved.caller_mac,    // remote MAC
                11888,                      // our local port (source)
                sport,                      // remote port (target) — 80 or 443
                final_seq,
                final_ack,
                TH_ACK,                     // ACK only, no SYN
                dummy_payload,              // No payload
                0                           // no payload — pure ACK doesn't consume a seq number
            );

            //cur_conn = c_conn;

            cur_conn->packets_sent++;
            printk("TCP_SYN_ACK: ACK Sent\n");

            // #test: Update sequence numbers
            cur_conn->tcp_conn->snd_una = final_seq;  // or _ack_number from the peer
            cur_conn->tcp_conn->snd_nxt = final_seq;  // next byte we will send
            cur_conn->tcp_conn->rcv_nxt = final_ack;  // next byte we expect from peer

            // Optional but useful
            // cur_conn->ep_pair->c_ep->socket->state = SS_CONNECTED;
            // cur_conn->ep_pair->s_ep->socket->state = SS_CONNECTED;

            if ( cur_conn->ep_pair && 
                 cur_conn->ep_pair->c_ep && 
                 cur_conn->ep_pair->s_ep ) 
            {
                struct socket_d *c_sock = cur_conn->ep_pair->c_ep->socket;
                struct socket_d *s_sock = cur_conn->ep_pair->s_ep->socket;

                if ( c_sock && 
                     c_sock->magic == 1234 &&
                     s_sock && 
                     s_sock->magic == 1234 )
                {
                    c_sock->state = SS_CONNECTED;
                    s_sock->state = SS_CONNECTED;
                }
            }

            cur_conn->status          = CONN_STATUS_ESTABLISHED;
            cur_conn->tcp_conn->state = TCP_ESTABLISHED;

            return;
        }

        // (3) ACK
        // A client is confirming the connection we accepted.
        // At this point we must locate the correct connection structure
        // based on the endpoint pair (server IP/port + client IP/port).
        // We cannot assume it is the same client as the last SYN,
        // because multiple clients may be handshaking at once.
        // Once the matching connection is found in SYN_RECEIVED state,
        // we transition it to ESTABLISHED.

        // #test
        struct connection_d *c_conn = tcp_find_connection_by_client(s_ipv4_int, sport);  
        if (c_conn){
            cur_conn = c_conn;
        }
        // Normally, the client sends just an ACK (no payload).
        // But if the client adds data ...       
        if ((void*)cur_conn != NULL){
        if (cur_conn->magic == 1234 && cur_conn->status == CONN_STATUS_SYN_RECEIVED){
        if (fSYN == 0 && fACK == 1)
        {
            printk("TCP_ACK: SEQ={%d} | ACK={%d}\n", _seq_number, _ack_number );

            // -----------------------------------------------------
            // #todo
            // We received an ack as a response to
            // our syn/ack sent by a process in this machine.
            // Our connection is now considered stablished.
            // #ps: but we are not using the structure that 
            // handles this connection yet.
            // No response is sent now.

            if ((void*) cur_conn == NULL){
                printk("__kd_handle_tcp: [step 3] cur_conn\n");
                return;
            }
            if (cur_conn->magic != 1234){
                printk("__kd_handle_tcp: [step 3] cur_conn validation\n");
                return;
            }
            if (cur_conn->status == CONN_STATUS_SYN_RECEIVED)
            {
                // by the book, the third ACK in the handshake normally carries no payload. 
                // But in TCP, you must expect that it can carry data, 
                // because the protocol allows it
                cur_conn->tcp_conn->rcv_nxt = _seq_number + data_len;

                // _ack_number → comes from the peer’s TCP header. 
                // It says: “I have received everything up to 
                // this sequence number minus one, and I expect this next byte.”

                // cur_conn->tcp_conn->snd_nxt → your local TCP state. 
                // It tracks the next sequence number you intend to send. 
                // After sending SYN, you set:

                // #ps: If the wrong connection is being checked, 
                // the mismatch is inevitable 
                if (_ack_number != cur_conn->tcp_conn->snd_nxt)
                {
                    printk("TCP: step 3 ack mismatch, expected %d got %d\n",
                        cur_conn->tcp_conn->snd_nxt, _ack_number );
                    return; // don't establish on a bad ack  
                }
                //if (_ack_number != cur_conn->tcp_conn->snd_una + 1) {
                //    printk("TCP: step 3 ack mismatch, expected %u got %u\n",
                //        cur_conn->tcp_conn->snd_una + 1, _ack_number);
                //    return;
                //}
                //if (_ack_number != cur_conn->tcp_conn->iss + 1) {
                //    printk("Handshake ACK mismatch, expected %u got %u\n",
                //        cur_conn->tcp_conn->iss + 1, _ack_number);
                //    return;
                //}

                //cur_conn->tcp_conn->snd_una = cur_conn->tcp_conn->iss + 1;
                cur_conn->tcp_conn->snd_una = _ack_number;
                cur_conn->packets_received++;
                cur_conn->tcp_conn->state = TCP_ESTABLISHED;
                cur_conn->status = CONN_STATUS_ESTABLISHED;
                printk("TCP_ACK: Connection ESTABLISHED for id={%d} :)\n", 
                    cur_conn->id );
            }

            return;
        }  // flags
        }  // valid magic
        }  // vaid pointer

    }  // 11888 only

    //if (dport == 11888)
    //{
        //printk("TCP: MESSAGE: {%s}\n", tcp_payload );
        //memset(tcp_payload,0,sizeof(tcp_payload));
    //}


// When the client sends the HTTP GET (after the 3-way handshake is finished):
// Flag, Value,      Meaning
// SYN,      0,      Connection is already established
// ACK,      1,      Always set on data segments after the handshake
// PSH, 1 (usually), Push the data to the application immediately
// FIN,      0,      Not closing yet
// RST,      0,      Not resetting

    // If we received something right after the connection was stablished
    // #ps: Not using the right structure for connection handling yet.
 
    // Drop packets that are not for port 11888.
    if (dport != 11888) {
        return;
    }


// -------------------------
// #test: 
// Switch to the connection that belongs to the client.
// #bugbug: Maybe this function is not working.
// Maybe the values are wrong or even the value format is wrong.
// See: sockint.c
// If a client refuses to close, you can still 
// find the connection later and reclaim it after timeout.

/*
    struct connection_d *c_conn = 
        tcp_find_connection_by_endpoints(
            NetworkSaved.target_ip_int, dport,    // local
            NetworkSaved.caller_ip_int, sport );  // remote
    printk("ip:%x port:%d | ip:%x port:%d \n", 
        NetworkSaved.target_ip_int, dport,
        NetworkSaved.caller_ip_int, sport
    );
*/

// Getting pointer based on REMOTE ep. 
// NetworkSaved.caller_ip_int
    struct connection_d *c_conn = 
        tcp_find_connection_by_client(s_ipv4_int, sport);  
    printk("REMOTE: ip:%x port:%d \n", d_ipv4_int, sport );

// Switch current connection
    if ((void*)c_conn != NULL) 
    {
        // Update state, window, sequence numbers, deliver payload
        if (c_conn->magic == 1234)
        {
            cur_conn = c_conn;
            // #debug
            // printk("TCP: Reusing conn structure   <<<<<<<< \n");
        }
    }
// -------------------------

    // drop
    if ((void*) cur_conn == NULL)
        return;
    if (cur_conn->magic != 1234)
        return;

// #todo:
// Right now your network_handle_tcp() always drops down into 
// the low‑level worker (network_send_tcp) to push a reply. 
// But in the case of RST flag, we gotta mark the socket 
// as free to reuse and cancel this ending.
// + Mark the socket as free/reusable.
// + Reset its state (SS_CLOSED).
// + Clear connection pointers.
// + Leave the structure in the pool for reuse.

    // #debug: Provisory
    if (fRST == 1)
    {
        printk("TCP: RST found. Not sending response\n");
        return;
    }

/*
    // #todo:
    if (fRST == 1) 
    {
        printk("TCP: RST received, closing socket\n");

        // Get the socket from the connection’s endpoint
        struct socket_d *sk = conn->ep_pair->c_ep->socket;
        if (sk) {
            sk->state = SS_DISCONNECTING;

            // Perform cleanup...
            // Clear buffers, reset flags, detach endpoints
            sk->flags = 0;

            // After cleanup, mark closed
            sk->state = SS_CLOSED;
            sk->free = TRUE;   // mark reusable
        }

        // Mark connection closed
        conn->status = CONN_STATUS_CLOSED;
        conn->tcp_conn->state = TCP_CLOSED;

        // Do not call network_send_tcp() here
        return;
    }
*/

// --------------------------------------------------
// Step 4 — the GET request arrives
// This part is where the kernel is operating as a server,
// and it is responding requests.
    if (cur_conn->status == CONN_STATUS_ESTABLISHED)
    {
        //if (fSYN == 0 && fACK == 1)
        //{
            // #todo:
            // printk("11888: packet after handshake\n");

            // Acknowledge the bytes we just received (the GET request itself)
            // before responding, or the client's TCP will think this data
            // was never ACKed and will retransmit it.
            cur_conn->tcp_conn->rcv_nxt += data_len;
            // Decrease our own window size
            cur_conn->tcp_conn->rcv_wnd -= data_len;
            cur_conn->packets_received++;

            // #test: Checking for HTTP traffic on port 11888.
            // #ps: For now, if a GET is found, the routine
            // will send and Send ACK + PUSH + FIN.
            gramnet_handle_http(
                cur_conn, __tcp_payload, data_len, sport, dport );

            return;
        //}
    }

// -----------------------------------------------------
// We already sent our response + FIN in gramnet_handle_http(),
// so the connection is in FIN_WAIT. This is where the client's
// own FIN (closing their side) is expected to show up.
// -----------------------------------------------------
/*
Most common causes in your situation:
You send data + FIN, but the browser’s FIN is not ACKed cleanly
rcv_nxt is wrong when you send the final ACK
You close the connection too early (set CONN_STATUS_CLOSED before the browser is finished)
The browser sends a large GET and you didn’t advance rcv_nxt by the real length
Retransmissions or duplicate ACKs confuse the state machine
*/

// Step 5 — client's FIN arrives
    if (cur_conn->status == CONN_STATUS_FIN_WAIT1)
    {

        // ACK reveived with the connection stablished.
        // No FIN was received here.
        if (fACK == 1) {
            printk("FIN_WAIT1: received ACK\n");
        }

        // 2. If the peer sent FIN, ACK it and close and return.
        // #ps: Possibly receiving and ACK too.
        if (fFIN == 1)
        {
            printk("FIN_WAIT1: [11888] FIN received in FIN_WAIT1, sending ACK\n");

            // #maybe: rcv_nxt is wrong when you send the final ACK

            // The FIN consumes one sequence number, same as SYN.
            cur_conn->tcp_conn->rcv_nxt += 1;

            cur_conn->packets_received++;

            // Send ACK.
            // Acknoledgind the received data.
            int rv = 
            network_send_tcp(
                dhcp_info.your_ipv4,
                NetworkSaved.caller_ipv4,
                NetworkSaved.caller_mac,
                11888,
                sport,
                cur_conn->tcp_conn->snd_nxt,
                cur_conn->tcp_conn->rcv_nxt,
                TH_ACK,
                dummy_payload, 
                0
            );

            if (rv < 0) {
                printk("FIN_WAIT1: [11888] Failed to ACK client FIN\n");
                return;  // leave state as-is; a retransmitted FIN can retry
            }
            printk("TCP_11888: FIN was acked\n");

            cur_conn->packets_sent++;
            
            // We can close the connection now
            cur_conn->status = CONN_STATUS_CLOSED;   // adjust to your actual enum
            //cur_conn->tcp_conn->state = TCP_CLOSED;
            return;
        }

        // Step 6 — stray data while in FIN_WAIT
        // 1. Always advance rcv_nxt for any data that still arrives
        // While in FIN_WAIT, never call the HTTP handler again. 
        // Just consume the data (advance rcv_nxt) and wait for the FIN.
        if (data_len > 0) 
        {
            printk("FIN_WAIT: received %u extra bytes (ignored)\n", 
                (unsigned) data_len );       
            cur_conn->tcp_conn->rcv_nxt += data_len;

            cur_conn->packets_received++;

            // #test: Checking for HTTP traffic on port 11888.

            // #todo:
            // Maybe here we can serve more data, or simply ACK.
            //gramnet_handle_http(
                //cur_conn, __tcp_payload, data_len, 
                //sport, dport );
            return;
        }

        // whatever arrived while in FIN_WAIT, we're done with it here
        return;
    }

    if (cur_conn->status == CONN_STATUS_CLOSED)
    {
        if (fFIN == 1){
            printk("__kd_handle_tcp: FIN on closed connection\n");
            return;
        }
    }

    //
    // Drop
    //
}

void 
network_handle_tcp ( 
    const unsigned char *buffer, 
    ssize_t size,
    unsigned int s_ipv4_int,
    unsigned int d_ipv4_int )
{
    struct tcp_d *tcp;  // The buffer
    //register int i=0;
    uint16_t flags=0;
    size_t data_len = 0;

    // No payload for handshake
    char dummy_payload[2];
    dummy_payload[0] = 0x00;
    dummy_payload[1] = 0;

    // #debug
    //printk("network_handle_tcp: #todo\n");

// Parameters
    if ((void*) buffer == NULL){
        printk("network_handle_tcp: buffer\n");
        return;
    }
    //if (size < 0){
    //}

    // Pointer for the TCP header. Pre-allocated.
    tcp = (struct tcp_d *) buffer;

    uint16_t sport = (uint16_t) FromNetByteOrder16(tcp->th_sport);
    uint16_t dport = (uint16_t) FromNetByteOrder16(tcp->th_dport);

    //printk("TCP Packet: remote=%u (sport), local=%u (dport)\n", tcp->th_sport, tcp->th_dport);
    //printk("TCP Packet: [receiving] remote=%u (sport), local=%u (dport)\n", 
        //sport, dport);

// Target is kernel debugger
    if (dport == 11888){
        __kd_handle_tcp(buffer, size, s_ipv4_int, d_ipv4_int);
        return;
    }

//
// Super drop
// Only some ports are allowed.
// We are acoiding noise for now.
//

    int AllowThisPort = FALSE;

//
// YOU SHALL NOT PASS!
//

    // Ephemeral ports
    if (dport >= __first_ephemeral_port && 
        dport <= __last_ephemeral_port)
    {
        AllowThisPort = TRUE;
    }

    // Special experimental ports
    if ( dport == 4040 ||
         dport == 4041 || 
         dport == 22888 )
    {
        AllowThisPort = TRUE;
    }

    // Future: enable HTTP/HTTPS when ready
    // if (dport == 80 || dport == 443)
    //     AllowThisPort = TRUE;

//
// Drop
//

    // #ps: This filter is dropping a lot of noise. A LOT.
    if (AllowThisPort != TRUE) {
        //printk("TCP: Invalid port %u <<< X >>>\n", dport);
        return;
    }

//
// Welcome to Gramado Castle
//

   printk("TCP Packet: [Receiving] s=%u (sport), r=%u (dport)\n", 
        sport, dport );

// Not for kernel debugger

    // #todo: We will handle the connection for the cases
    // where the port is not the kernel debugger in a different way.
    // handling the connection structure at the beginning of this routine.

    // #debug: Provisory drop.
    //printk("network_handle_tcp: [TEST TEST TEST] Not 11888\n");
    //return;
    
// -------- normal TCP --------

    tcp_seq _seq_number = (tcp_seq) FromNetByteOrder32(tcp->th_seq);
    tcp_ack _ack_number = (tcp_ack) FromNetByteOrder32(tcp->th_ack);

    // Clear the payload local buffer
    memset(__tcp_payload, 0, sizeof(__tcp_payload));

    if (size >= TCP_HEADER_LENGHT){
    //if (size >= header_len){

        data_len = (size - TCP_HEADER_LENGHT);
        //data_len = (size - header_len);
        if (data_len >= 1400)
            data_len = 1400 -2;
        strncpy( __tcp_payload, (buffer + TCP_HEADER_LENGHT), data_len );
        //strncpy( __tcp_payload, (buffer + header_len), data_len );
        __tcp_payload[data_len + 1] = 0;
        //__tcp_payload[1400 -1] = 0;
        __tcp_payload[1400 -1] = 0;
    } 
    //if (size < header_len)
    if (size < TCP_HEADER_LENGHT)
    {
        //data_len = 0;
        //__tcp_payload[data_len + 1] = 0;
        // #bugbug: Drop it
        printk("TCP: Invalid buffer size %d\n", size);
        return;
    }

    // Window: The client can only accept this n bytes
    uint16_t peer_window = (uint16_t) FromNetByteOrder16(tcp->window_size);

    // #test:
    //if (peer_window == 0)
        //return;

//
// Flags
//

// FIN  - graceful close,
// SYN  - start handshake,
// RST  - abort,
// PUSH - deliver data now,
// ACK  - update state,
// URG  - handle urgent data.

// Control flags (6 bits)
    uint16_t fFIN=0;
    uint16_t fSYN=0;
    uint16_t fRST=0;
    uint16_t fPUSH=0;
    uint16_t fACK=0;
    uint16_t fURG=0;

    flags = (uint16_t) FromNetByteOrder16(tcp->do_res_flags);

    //#debug
    //printk("Flags={%x}\n",flags);

// Receiving a FIN means the peer is done sending; we should acknowledge and
// eventually close our side gracefully.
    if (flags & TH_FIN){
        fFIN = 1;
    }

// Receiving a SYN means the peer wants to start a new connection; respond
// with SYN+ACK if we are listening.
    if (flags & TH_SYN){
        fSYN = 1;
    }

// Receiving a RST means the peer wants to abort/reset the connection; tear
// down state immediately and stop using this socket.
    if (flags & TH_RST){
        fRST = 1;
    }

// Receiving a PUSH means the peer wants the data delivered immediately;
// pass buffered data up to the application without delay.
    if (flags & TH_PUSH){
        fPUSH = 1;
    }

// Receiving an ACK means the peer is acknowledging our sent data or handshake;
// update sequence numbers and possibly advance connection state.
    if (flags & TH_ACK){
        fACK = 1;
    }

// Receiving URG means urgent data is present; handle the urgent pointer and
// notify the application of out‑of‑band data.
    if (flags & TH_URG){
        fURG = 1;
    }

    // ex: 5014H
    // 0101 0000 0001 0100

// Initializing connection
// 1) SYN      >>
// 2) SYN/ACK  <<
// 3) ACK      >>

// Finalizing connection
// 1) FIN >>
// 2) ACK <<
// 3) FIN <<
// 4) ACK >>


//If source IP = 0 → 
//You don’t know who the peer is. 
//You cannot establish a connection. Drop the packet or log an error.
//If destination IP = 0 → 
//You don’t know which local endpoint this packet is for. Drop it.
//If both are 0 → 
//Treat as invalid input. Do not attempt handshake.

    printk("TCP: dport=%d SYN={%d} ACK={%d} FIN={%d}\n", 
        dport, fSYN, fACK, fFIN);

//
// Step 1: No servers for now. (No pure SYN)
//

// Step 1 – Client sends SYN to us (We are the server)

    /*
    if (fSYN && !fACK) 
    {
        // No servers yet (except kd on 11888)
        printk("x: Pure SYN\n");
        return;  // drop pure SYN
    }
    */
    // -----
    // (1) SYN
    // Step 1 — SYN received (initialize, not increment)
    // A client is trying to initialize a new connection.
    // The client shares an Initial Sequence Number (ISN) with the server.
    // It means the server here needs to respond.
    // #important: In this case our goal is connect 
    // to a server running in localhost in ring 3.
    // #ps: This selver already has its own socket. We need to plug it
    // into the connection chain, not create.

    // 1 :: SYN from remote client to a local server
    if (fSYN == 1 && fACK == 0)
    {
        printk("-- Step 1 --------\n");
        //printk("TEST TESTE: STEP 1 STEP 1 STEP 1\n");
        printk("TCP_SYN: SEQ={%d} | ACK={%d}\n", 
            _seq_number, _ack_number );
        //printk("TCP_SYN: seq=%u (offset=%u) ack=%u (offset=%u)\n",
        //    _seq_number,
        //    _seq_number - conn->tcp_conn->iss,
        //    _ack_number,
        //    _ack_number - conn->tcp_conn->irs );

        // Example sequence/ack numbers
        // #ps: It looks safer to test our generator now
        tcp_seq seq = 1000;  //__generate_ISN();  // initial sequence number
        tcp_ack ack = _seq_number + 1;   // acknowledge client’s ISN

        // -- connection structure ----
        // Create a connection structure.
        // #ps: Remote client, local server.
        struct connection_d *conn = create_connection(CONN_TYPE_TCP);
        if ((void*) conn == NULL){
            printk("Failed to create connection object\n");
            return; // do not respond
        }
        // Register connection
        int id = connection_register(conn);
        if (id < 0 || id >= MAX_CONNECTIONS) 
        {
            printk("Failed to register connection\n");
            // free the object if needed
            //kfree(conn->tcp_conn);
            //kfree(conn);
            return; // do not respond
        }
        conn->type = CONN_TYPE_TCP;
        // #ps: Status: Receiving a SYN from a remoter client
        conn->status = CONN_STATUS_SYN_RECEIVED;

        // tcp connection structure
        if ((void*) conn->tcp_conn == NULL){
            printk("Failed to create TCP connection structure\n");
            return; // do not respond
        }
        conn->tcp_conn->state = TCP_SYN_RECEIVED;
        conn->packets_received++;
        // IRS → Initial Receive Sequence number
        conn->tcp_conn->irs     = _seq_number;      // client's ISN
        conn->tcp_conn->rcv_nxt = _seq_number + 1;  // SYN consumes 1 seq number
        // ISS → Initial Send Sequence number
        conn->tcp_conn->iss     = seq;
        conn->tcp_conn->snd_una = seq;  // Oldest unacknowleged byte
        conn->tcp_conn->snd_nxt = seq + 1;

        conn->tcp_conn->snd_wnd = peer_window;  // Client's window size?
        //conn->tcp_conn->rcv_wnd -= 1;         // Maybe
        printk("Connection %d created, state=SYN_RECEIVED\n", id);                 

        // -- ep pair -----------
        struct endpoint_pair_d *pair = create_endpoint_pair_object();
        if (!pair) {
            printk("Failed to create endpoint pair\n");
            return; // do not respond
        }
        pair->case_id = CONN_LSRC; // Local Server → Remote Client

        // -- remote ep (client) ---------------
        // Remote client endpoint
        struct endpoint_d *client_ep = create_endpoint_object();
        client_ep->is_remote = TRUE;  // <<< REMOTE EP
        if (!client_ep) {
            printk("Failed to create remote endpoint\n");
            return; // do not respond
        }
        // Create a socket for the remote client
        client_ep->socket = NULL;
        struct socket_d *sk_client;
        sk_client = (struct socket_d *) create_socket_object();
        if ((void*) sk_client == NULL){
            panic("TCP: on sk_client\n");
            //return;
        }
        sk_client->family = AF_INET;
        sk_client->type = SOCK_STREAM;
        sk_client->protocol = IPPROTO_TCP;

        sk_client->conn = conn;     // Belongs to this connection
        sk_client->ep = client_ep;  // Belonts to this ep

        // Remote peer identity
        sk_client->pid = -1;   // remote client, not a local process
        sk_client->uid = 0;
        sk_client->gid = 0;

        // IP/Port
        // sk_client->ip_ipv6 = 0;
        sk_client->ip_ipv4 = s_ipv4_int;  //NetworkSaved.caller_ip_int;
        sk_client->port = sport;

        // Connection state
        sk_client->state = SS_CONNECTING;
        sk_client->flags = 0;
        sk_client->conn_copy = FALSE;

        // Backlog defaults
        sk_client->backlog_max = 0;
        sk_client->pending_client_count = 0;
        sk_client->pending_server_count = 0;
        // magic string? It indicates pending connection?
        // sk_client->magic_string[0] = 'C';

        // Plug the socket into the client ep
        client_ep->socket = sk_client;

        // Plug the ep into the pair
        pair->c_ep = client_ep;

        // -- local ep (server) ---------------
        // Local server endpoint
        struct endpoint_d *server_ep = create_endpoint_object();
        server_ep->is_remote = FALSE;  // NOT REMOTE EP (LOCAL)
        if (!server_ep) {
            printk("network_handle_tcp: Failed to create local endpoint\n");
            return; // do not respond
        }
        // #todo:
        // This is the part where we get the socket 
        // from the listening server process. 
        // For now, we set it to NULL.
        // #todo: 
        // It should look up the process that registered itself as 
        // the listener for port 11888 and then grab the socket object 
        // that belongs to that process.

        pair->s_ep = server_ep;

        // --------------------------------------------

        // Plug them together
        conn->ep_pair = pair;

        // --------------------------------------------
        // The socket for the local server.
        // #important: We are not creating it,
        // we are getting the pointer based on the port number.
        server_ep->socket = NULL;
        struct socket_d *sk_listener; 
        sk_listener = (struct socket_d *) socket_get_tcpserver_socket_by_port(dport);
        if ((void*) sk_listener != NULL)
        {
            if (sk_listener->magic == 1234)
            {
                sk_listener->pending_client_count++;
                if (sk_listener->pending_client_count > sk_listener->backlog_max)
                {
                    sk_listener->pending_client_count = 0;
                }
                int backlog_tail = sk_listener->pending_client_count;

                sk_listener->pending_client_endpoints[backlog_tail] = 
                    client_ep->socket;

                sk_listener->state = SS_CONNECTING;

                sk_listener->conn = conn;     // Belongs to this connection
                sk_listener->ep = server_ep;  // Belongs to this ep

                server_ep->socket = sk_listener;   // Save into the ep
            }
        }

        // #important: Connection status.
        conn->status = CONN_STATUS_SYN_RECEIVED;

        // Our current connection been stablished.
        cur_conn = conn;  // remember this connection for later ACK

        // #todo
        // The client is saying: "I want to connect to the 
        // server process that is listening to the port 11888"

        // #todo
        // >>> Lets work on this response! <<<

        // Flags: SYN + ACK
        uint16_t flags = TH_SYN | TH_ACK;

        //
        printk("network_handle_tcp: Sending SYN/ACK >>\n");

        // Send SYN + ACK
        network_send_tcp(
            dhcp_info.your_ipv4,       // server IP
            NetworkSaved.caller_ipv4,  // client IP (Array)
            NetworkSaved.caller_mac,   // client MAC
            dport,    // server port (source) (local)
            sport,    // client port (target) (remote)
            conn->tcp_conn->iss,      // seq = our ISN
            conn->tcp_conn->rcv_nxt,  // ack = client's ISN + 1
            flags,
            dummy_payload,  // No tcp payload
            0               // No tcp payload lenght char=0x00
        );

        conn->packets_sent++;

        // Waiting for the ACK:
        return;
    }

//
// Step 2: SYN/ACK  
//

// Step 2 – Server replies with SYN+ACK (We are the client)
// We received a SYN/ACK because we sent a syn to a remote server.

    // (2) SYN/ACK
    // A server accepted the connection.
    // We received a syn/ack as a response to
    // our syn sent by a process in this machine.
    // #todo: Apply the connection structure that handles this connection.
    // host   → remote : SYN
    // Remote → host   : SYN-ACK
    // host   → remote : ACK

    // 2 :: SYN_ACK from remote server to a local client
    if (fSYN == 1 && fACK == 1)
    {
        printk("TCP_SYN_ACK: SEQ={%d} | ACK={%d}\n", 
            _seq_number, _ack_number );
        //printk("TCP_SYN_ACK: seq=%u (offset=%u) ack=%u (offset=%u)\n",
            //_seq_number,
            //_seq_number - conn->tcp_conn->iss,
            //_ack_number,
            //_ack_number - conn->tcp_conn->irs );

        printk("TCP_SYN_ACK: Sending final ACK\n");

        // We're the client here — the remote side acked our SYN and sent
        // its own SYN. Complete the handshake with the final ACK.

        tcp_seq final_seq = _ack_number;       // = our ISN + 1, given by the server's ack
        tcp_ack final_ack = _seq_number + 1;   // acknowledge the server's ISN

        // #test
        struct connection_d *c_conn =
            tcp_find_connection_by_remote_peer(s_ipv4_int, sport);
        //struct connection_d *c_conn = tcp_find_connection_by_client(s_ipv4_int, sport);  
        if (!c_conn){ 
            printk("step2: syn_ack fail\n");
            return; 
        }
        if (c_conn)
        {
            if (c_conn->magic != 1234) {
                printk("TCP_SYN_ACK: [FAIL] connection validation\n");
                return;
            }
            cur_conn = c_conn;
        }

        /*
        // port?
        // unsigned short client_port = cur_conn->ep_pair->c_ep->socket->port;
        unsigned short client_port=0;
        if ( cur_conn->ep_pair && 
             cur_conn->ep_pair->c_ep ) 
        {
            struct socket_d *c_sock = cur_conn->ep_pair->c_ep->socket;
            if ( c_sock && c_sock->magic == 1234)
            {
                client_port = (unsigned short) c_sock->port;
            }
        }
        */

        // #debug
        //if (client_port == dport)
            //printk("client_port and dport are equal\n");

        // Send ACK?
        network_send_tcp(
            dhcp_info.your_ipv4,        // our IP
            NetworkSaved.caller_ipv4,   // remote IP (whoever this packet came from)
            NetworkSaved.caller_mac,    // remote MAC
            dport,                      // our local port (source) 
            sport,                      // remote port (target) — 80 or 443
            final_seq,
            final_ack,
            TH_ACK,                     // ACK only, no SYN
            dummy_payload,              // No payload
            0                           // no payload — pure ACK doesn't consume a seq number
        );

        //cur_conn = c_conn;
        cur_conn->packets_sent++;

        printk("TCP_SYN_ACK: ACK Sent\n");

        // #test: Update sequence numbers
        //cur_conn->tcp_conn->snd_una = final_seq;  // The last unacknowledged byte
        //cur_conn->tcp_conn->snd_nxt = final_seq;  // next byte we will send
        //cur_conn->tcp_conn->irs = final_ack;  // The initial acknoleged byte in remote peer
        //cur_conn->tcp_conn->rcv_nxt = final_ack;  // next byte we expect from peer

        cur_conn->tcp_conn->snd_una = cur_conn->tcp_conn->iss + 1; // SYN acknowledged
        cur_conn->tcp_conn->snd_nxt = cur_conn->tcp_conn->iss + 1; // still next to send
        cur_conn->tcp_conn->irs     = final_seq;                   // server’s ISN
        cur_conn->tcp_conn->rcv_nxt = final_seq + 1;               // expect next byte

        // Optional but useful
        // cur_conn->ep_pair->c_ep->socket->state = SS_CONNECTED;
        // cur_conn->ep_pair->s_ep->socket->state = SS_CONNECTED;

        if ( cur_conn->ep_pair && 
             cur_conn->ep_pair->c_ep && 
             cur_conn->ep_pair->s_ep ) 
        {
            struct socket_d *c_sock = cur_conn->ep_pair->c_ep->socket;
            struct socket_d *s_sock = cur_conn->ep_pair->s_ep->socket;

            if ( c_sock && 
                 c_sock->magic == 1234 &&
                 s_sock && 
                 s_sock->magic == 1234 )
            {
                c_sock->state = SS_CONNECTED;
                s_sock->state = SS_CONNECTED;

                // Let's allow the client to send the first request
                file *fp = c_sock->private_file;
                if ((void*) fp == NULL){
                    panic("TCP step 2: invalid fp\n");  return;
                }
                if (fp->magic != 1234){
                    panic("TCP step 2: fp validation\n");  return;
                }
                fp->sync.action = ACTION_NULL;
                fp->sync.can_write = TRUE;  // Can send a request
                fp->_flags |= __SWR;        // flags: can write

                // Enlarge the socket buffer for the client
                int ok = tcp_change_socket_buffer(c_sock, 5*1024); // 5KB
                if (ok != 0)
                    printk("TCP: [FAIL] couldin't enlarge the socket buffer\n");
            }
        }

        // Connection
        cur_conn->status = CONN_STATUS_ESTABLISHED;
        // TCP connection
        cur_conn->tcp_conn->state = TCP_ESTABLISHED;

        printk("TCP_SYN_ACK: ACK Sent ESTABLISHED    :)\n");

        return;  // Established
    }

//-------------------------------------------

//
// Step 3: For a remote client and local server
//

    // (3) ACK
    // A client is confirming the connection we accepted.
    // At this point we must locate the correct connection structure
    // based on the endpoint pair (server IP/port + client IP/port).
    // We cannot assume it is the same client as the last SYN,
    // because multiple clients may be handshaking at once.
    // Once the matching connection is found in SYN_RECEIVED state,
    // we transition it to ESTABLISHED.

    // Get a connection based on
    // the remote client.
    //struct connection_d *c_conn = tcp_find_connection_by_client(s_ipv4_int, sport);  
    struct connection_d *c_conn = tcp_find_connection_by_remote_peer(s_ipv4_int, sport);  
    if (c_conn){
        cur_conn = c_conn;
    }
    // Normally, the client sends just an ACK (no payload).
    // But if the client adds data ...
    // #ps: The state is CONN_STATUS_SYN_RECEIVED
    // Because the SYN was already received by the 
    // local server.

    // 3 :: ACK from remote client local server
    // right after sending the SYN_ACK.
    if ((void*)cur_conn != NULL){
    if (cur_conn->magic == 1234){
    if (cur_conn->status == CONN_STATUS_SYN_RECEIVED || cur_conn->status == CONN_STATUS_SYN_SENT){
    if (fSYN == 0 && fACK == 1)
    {
        //printk("TCP_ACK: SEQ={%d} | ACK={%d}\n", _seq_number, _ack_number );
        printk("TCP_ACK: seq=%u (offset=%u) ack=%u (offset=%u)\n",
            _seq_number,
            _seq_number - cur_conn->tcp_conn->iss,
            _ack_number,
            _ack_number - cur_conn->tcp_conn->irs );

        //printk("Step3 ACK: received=%u expected_iss+1=%u snd_nxt=%u snd_una=%u\n",
        //    _ack_number,
        //    cur_conn->tcp_conn->iss + 1,
        //    cur_conn->tcp_conn->snd_nxt,
        //    cur_conn->tcp_conn->snd_una );

        // -----------------------------------------------------
        // #todo
        // We received an ack as a response to
        // our syn/ack sent by a process in this machine.
        // Our connection is now considered stablished.
        // #ps: but we are not using the structure that 
        // handles this connection yet.
        // No response is sent now.

        if ((void*) cur_conn == NULL){
            printk("__kd_handle_tcp: [step 3] cur_conn\n");
            return;
        }
        if (cur_conn->magic != 1234){
            printk("__kd_handle_tcp: [step 3] cur_conn validation\n");
            return;
        }
        // --------
        // We already received the SYN
        // We are a server and already received a SYN,
        // and we sent a syn_ack
        if (cur_conn->status == CONN_STATUS_SYN_RECEIVED)
        {
            cur_conn->tcp_conn->snd_una = _ack_number;  // Oldest unacknowleged byte

            // By the book, the third ACK in the handshake normally 
            // carries no payload. But in TCP, you must expect that 
            // it can carry data, because the protocol allows it.
            cur_conn->tcp_conn->rcv_nxt = _seq_number + data_len;

            // _ack_number → comes from the peer’s TCP header. 
            // It says: “I have received everything up to 
            // this sequence number minus one, and I expect this next byte.”

            // cur_conn->tcp_conn->snd_nxt → your local TCP state. 
            // It tracks the next sequence number you intend to send. 
            // After sending SYN, you set:

            // #ps: If the wrong connection is being checked, 
            // the mismatch is inevitable 
            if (_ack_number != cur_conn->tcp_conn->snd_nxt)
            {
                printk("TCP: step 3 ack mismatch, expected %d got %d\n",
                    cur_conn->tcp_conn->snd_nxt, _ack_number );
                return; // don't establish on a bad ack  
            }
            //if (_ack_number != cur_conn->tcp_conn->snd_una + 1) {
            //    printk("TCP: step 3 ack mismatch, expected %u got %u\n",
            //        cur_conn->tcp_conn->snd_una + 1, _ack_number);
            //    return;
            //}
            //if (_ack_number != cur_conn->tcp_conn->iss + 1) {
            //    printk("Handshake ACK mismatch, expected %u got %u\n",
            //        cur_conn->tcp_conn->iss + 1, _ack_number);
            //    return;
            //}

            //cur_conn->tcp_conn->snd_una = cur_conn->tcp_conn->iss + 1;
            cur_conn->packets_received++;
            cur_conn->tcp_conn->state = TCP_ESTABLISHED;
            cur_conn->status = CONN_STATUS_ESTABLISHED;

            printk("TCP_ACK: [ACK match] Connection {%d} ESTABLISHED  :)\n", 
                cur_conn->id );

            return;
        }
        // --------
        // In this case we are client and we send a SYN
        // If after sending a SYN we’re receiving a pure ACK (no SYN flag set), 
        // it usually means the remote server is acknowledging our SYN 
        // but NOT completing the handshake correctly. 
        if (cur_conn->status == CONN_STATUS_SYN_SENT)
        {
            printk("TCP: Pure ACK received after sending SYN\n");
            // ...
            return;
        }

        // Drop
        return;
    }  // flags
    }  // Two valid states
    }  // valid magic for connection pointer
    }  // valid connection pointer

// -----------------------------------------

// the host is the client ... 
// lets get the connection based on the client information.
// Getting pointer based on REMOTE ep. 
// NetworkSaved.caller_ip_int
    //struct connection_d *c_conn = 
        //tcp_find_connection_by_client(s_ipv4_int, dport);
    struct connection_d *cl_conn = 
        tcp_find_connection_by_remote_peer(s_ipv4_int, dport);
   
    printk("TCP Target: ip:%x port:%d \n", d_ipv4_int, dport);

// Switch current connection
// Update state, window, sequence numbers, deliver payload.
    if ((void*)cl_conn != NULL) 
    {
        if (cl_conn->magic == 1234)
        {
            cur_conn = cl_conn;
            // #debug
            // printk("TCP: Reusing conn structure   <<<<<<<< \n");
        }
    }
// drop
    if ((void*) cur_conn == NULL)
        return;
    if (cur_conn->magic != 1234)
        return;

// #todo:
// continue handling operation between the remote server and 
// the local client.
// #ps: 
// The connection was stablished in the step2. Remember, 
// we are the client now ... no other servers, only the 11888 for now.
    if (cur_conn->status == CONN_STATUS_ESTABLISHED)
    {
        printk("TCP: Client received something with the connection already established\n");
        printk(">>> %d bytes\n", data_len);
        // ...

        // fail
        if ((void*) cur_conn->tcp_conn == NULL){
            cur_conn->status = CONN_STATUS_CLOSED;
            return;
        }

        cur_conn->tcp_conn->snd_una = _ack_number;   // Las unacknowledged byte

        cur_conn->packets_received++;

        // -------------------------------------------------
        // 1. Advance rcv_nxt only for in-order data
        // -------------------------------------------------
        if (_seq_number == cur_conn->tcp_conn->rcv_nxt) 
        {
            // In-order data segment
            cur_conn->tcp_conn->rcv_nxt += data_len;
            if (fFIN)
                cur_conn->tcp_conn->rcv_nxt += 1;   // FIN consumes one sequence number

            // #debug: Display payload
            // #todo: Here we are receiving the data,
            // but sometimes the routine bellow can't put
            // the data into the buffer.
            // #todo:
            // Our goal now is put the whole message 
            // with all the segments inside a buffer
            // and allow the ring 3 client to read it.
            if (data_len > 0) 
            {
                printk("TCP Payload (%d bytes):\n%s\n", 
                    (int)data_len, __tcp_payload );
                //int _i;
                //for (_i = 0; _i < 5; _i++)
                //{
                //    printk("%s\n", (__tcp_payload + _i));
                //    _i = _i+200; 
                //}
                //printk("\n");
            }

            // #todo:
            // We can create a worker that do this routine,
            // injecting incoming data into the socket buffer.
            struct socket_d *sk;
            sk = (struct socket_d *) get_client_socket_from_connection(cur_conn);
            if ((void*) sk == NULL){
                panic("TCP: invalid sk\n");
                return;
            }
            if (sk->magic != 1234){
                panic("TCP: sk validation\n");  return;
            }
            file *fp = sk->private_file;
            if ((void*) fp == NULL){
                panic("TCP: invalid fp\n");  return;
            }
            if (fp->magic != 1234){
                panic("TCP: fp validation\n");  return;
            }
            size_t room = (fp->_cnt > 0) ? (size_t) fp->_cnt : 0;
            if (data_len > room){
                // Can't buffer this segment yet — refuse it entirely.
                // rcv_nxt stays put, so the ACK we send below is a dup ACK
                // for the old position, telling the peer to retransmit later.
                printk("TCP: socket buffer full, dropping segment (%u bytes)\n",
                    (unsigned) data_len);
            } else {
                // It is probably ONE message segment
                printk("Saving payload into the file (%d bytes) <<<<\n", data_len);
                // silently truncates
                size_t to_copy = 
                    (data_len < fp->_cnt) 
                    ? data_len 
                    : fp->_cnt;

                // Inject at this position
                memcpy(
                    fp->_base + fp->_w, 
                    buffer + TCP_HEADER_LENGHT, 
                    to_copy );
                fp->_w += (int) to_copy;
                fp->_fsize = fp->_w;
                fp->_cnt = (fp->_lbfsize - fp->_fsize);

                // Permissions
                //fp->_flags &= ~__SRD;         // Cant read for now
                //fp->_flags &= ~__SWR;         // optional: clear write-only
                //fp->sync.can_read = TRUE;      // allow read
                //fp->sync.action = ACTION_REPLY;  // signal to client that data is ready

                fp->_flags |= __SRD;
                fp->sync.can_read = TRUE;
                fp->sync.can_write = FALSE;
                fp->sync.action = ACTION_REPLY;   // wake the client for THIS chunk too
            }

            if (fFIN){
                //fp->_r = 0;  // Read from the beginning when afte FIN
                fp->_flags |= __SRD;             // mark readable
                //fp->_flags &= ~__SWR;          // optional: clear write-only
                fp->sync.can_read = TRUE;        // allow read
                fp->sync.can_write = FALSE;
                fp->sync.action = ACTION_REPLY;  // signal to client that data is ready
            
                //sk->state = SS_UNCONNECTED;
            }
        }
        else if (_seq_number + data_len <= cur_conn->tcp_conn->rcv_nxt) {
            // Pure retransmission / already received → just ACK
            printk("TCP: duplicate segment (seq=%u)\n", _seq_number);
        }
        else {
            // Out-of-order → drop for now
            printk("TCP: out-of-order seq=%u expected=%u\n",
                _seq_number, cur_conn->tcp_conn->rcv_nxt );
        }

        // -------------------------------------------------
        // 3. Always send the current cumulative ACK
        // -------------------------------------------------
        uint16_t Flags = TH_ACK;
        if (fFIN)
            Flags |= TH_FIN;   // only if you want to close your side too

        // Send ACK.
        // Acknoledgind the received data.
        int rv = 
            network_send_tcp(
                dhcp_info.your_ipv4,
                NetworkSaved.caller_ipv4,
                NetworkSaved.caller_mac,
                dport,
                sport,
                cur_conn->tcp_conn->snd_nxt,
                cur_conn->tcp_conn->rcv_nxt,
                Flags,
                dummy_payload, 
                0
            );

        if (rv < 0) {
            printk(": [] Failed to ACK client FIN\n");
            return;  // leave state as-is; a retransmitted FIN can retry
        }
        printk(": acked\n");

        cur_conn->packets_sent++;

        if (fFIN == 1){
            printk("TCP: Disconnecting ...\n");
            cur_conn->status = CONN_STATUS_CLOSED;

            struct socket_d *sk;
            sk = (struct socket_d *) get_client_socket_from_connection(cur_conn);
            if ((void*) sk == NULL){
                panic("TCP: invalid sk\n");
                return;
            }
            if (sk->magic != 1234){
                panic("TCP: sk validation\n");  return;
            }
            sk->state = SS_UNCONNECTED;
            file *fp = sk->private_file;
            if ((void*) fp == NULL){
                panic("TCP: invalid fp\n");  return;
            }
            if (fp->magic != 1234){
                panic("TCP: fp validation\n");  return;
            }
            // #todo: Reset everything
            fp->sync.action = ACTION_REPLY;
            //fp->sync.action = ACTION_DISCONNECTING;  //200000   
            fp->sync.can_read = TRUE;
            //fp->sync.can_write = TRUE;
            //fp->_r = 0;
            //fp->_w = 0;
            //fp->_cnt = fp->_lbfsize; 
            return;
        }
    }

    // Receiving something with the connection closed
    if (cur_conn->status == CONN_STATUS_CLOSED)
    {
        printk("Receiving something with the connection closed\n");

        //if (fFIN){

        struct socket_d *sk;
        sk = (struct socket_d *) get_client_socket_from_connection(cur_conn);
        if ((void*) sk == NULL){
            panic("TCP: invalid sk\n");
            return;
        }
        if (sk->magic != 1234){
            panic("TCP: sk validation\n");  return;
        }
        sk->state = SS_UNCONNECTED;
        file *fp = sk->private_file;
        if ((void*) fp == NULL){
            panic("TCP: invalid fp\n");  return;
        }
        if (fp->magic != 1234){
            panic("TCP: fp validation\n");  return;
        }
        // #todo: Reset everything
        //fp->sync.action = ACTION_NULL;
        fp->sync.action = ACTION_DISCONNECTING;  //200000   
        fp->sync.can_read = TRUE;
        fp->sync.can_write = TRUE;
        fp->_r = 0;
        fp->_w = 0;
        fp->_cnt = fp->_lbfsize; 

        // -------------------------------------------------
        // 3. Always send the current cumulative ACK
        // -------------------------------------------------
        uint16_t Flags = TH_ACK;
        if (fFIN)
            Flags |= TH_FIN;   // only if you want to close your side too

        // Send ACK.
        // Acknoledgind the received data.
        int rv = 
            network_send_tcp(
                dhcp_info.your_ipv4,
                NetworkSaved.caller_ipv4,
                NetworkSaved.caller_mac,
                dport,
                sport,
                cur_conn->tcp_conn->snd_nxt,
                cur_conn->tcp_conn->rcv_nxt,
                Flags,
                dummy_payload, 
                0
            );

        if (rv < 0) {
            printk(": [] Failed to ACK client FIN\n");
            return;  // leave state as-is; a retransmitted FIN can retry
        }
        printk(": acked\n");

        return;
    
        //}
    }

    printk("TCP: drop\n");
    return;
}
