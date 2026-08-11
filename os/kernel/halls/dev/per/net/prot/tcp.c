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


// For testing only: track the last connection we created.
// Later this will be replaced with endpoint-based lookup.
static struct connection_d *cur_conn = NULL;


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

    // Google Web (HTTP)
    uint8_t google_ip[4] = {142, 250, 190, 46};


    printk("test_sending_tcp: sending SYNs to external targets\n");

    // Example sequence/ack numbers
    tcp_seq seq = __generate_ISN();  //1000; // server initial sequence number
    tcp_ack ack = 0;  //_seq_number + 1;   // acknowledge client’s ISN

    network_send_tcp(
        dhcp_info.your_ipv4,
        google_ip,
        NetworkSaved.gateway_mac,
        SourcePort,   // source port
        80,      // dest port
        seq,  // seq
        ack,  // ack
        TH_SYN,  // flags
        payload,    // no payload
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
        0x2000,
        0,
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
    Ltcp.window_size = ToNetByteOrder16(TCP_WINDOW_SIZE);  // max window

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
 * tcp_client_connect:
 * ------------------------------------------------------------
 * Worker for client-side AF_INET connect.
 *
 * Responsibilities:
 * 1) Create connection_d and mark SYN_SENT.
 * 2) Create endpoint_pair_d with local + remote endpoints.
 * 3) Plug the local socket into the local endpoint.
 * 4) Create a socket_d for the remote endpoint and save ip/port.
 * 5) Send SYN via network_send_tcp() using ip_ipv4_int.
 * 6) Update local socket state to SS_CONNECTING.
 * 7) Block until SYN/ACK → then send final ACK and mark ESTABLISHED.
 */

// This will be called by sys_connect() when the family is AF_INET.
int 
tcp_client_connect(
    struct socket_d *sk,
    unsigned int dst_ip_ipv4_int,
    unsigned short dst_port)
{

    // #debug
    // This is a work in progress! Not tested yet!

    printk("tcp_client_connect: #todo Client sends SYN\n");

    if (!sk)
        return -EINVAL;

    // Create connection object
    struct connection_d *conn = create_connection(CONN_TYPE_TCP);
    if (!conn) return -ENOMEM;

// #todo:
// Register the connection
    int id = connection_register(conn);
    // #todo: Check id validation
    if (id < 0 || id >= MAX_CONNECTIONS) 
    {
        printk("Failed to register connection\n");
        // free the object if needed
        //kfree(conn->tcp_conn);
        //kfree(conn);
        return -1; // do not respond
    }              

    conn->status = CONN_STATUS_SYN_SENT;
    conn->tcp_conn->state = TCP_SYN_SENT;

    // Create endpoint pair
    struct endpoint_pair_d *pair = create_endpoint_pair_object();
    if (!pair) return -ENOMEM;

    // Local endpoint
    struct endpoint_d *local_ep = create_endpoint_object();
    local_ep->is_remote = FALSE;
    local_ep->socket = sk;

    // Remote endpoint
    struct endpoint_d *remote_ep = create_endpoint_object();
    remote_ep->is_remote = TRUE;
    remote_ep->socket = create_socket_object();
    if (!remote_ep->socket) return -ENOMEM;
    remote_ep->socket->family   = AF_INET;
    remote_ep->socket->type     = SOCK_STREAM;
    remote_ep->socket->protocol = IPPROTO_TCP;
    remote_ep->socket->ip_ipv4  = dst_ip_ipv4_int;  // already in host order
    remote_ep->socket->port     = dst_port;         // Already in host order

    // Plug endpoints into pair
    pair->c_ep = local_ep;
    pair->s_ep = remote_ep;
    conn->ep_pair = pair;

    // Initialize sequence numbers
    conn->tcp_conn->iss     = 1000;  //__generate_ISN();  //1000;  // or random
    conn->tcp_conn->snd_nxt = conn->tcp_conn->iss + 1;
    conn->tcp_conn->snd_una = conn->tcp_conn->iss;

    // #todo:
    // Allocate ephemeral port for local socket
    sk->port = 0;
    //sk->port = __new_client_port_number++;
    //if (__new_client_port_number > 65000)
    //    __new_client_port_number = BASE_NEW_CLIENT_PORT_NUMBER;

    // Send SYN
    int rv = network_send_tcp(
        dhcp_info.your_ipv4,           // source IP (array)
        (uint8_t*) &dst_ip_ipv4_int,   // target IP (int → array cast)
        NetworkSaved.gateway_mac,
        sk->port, dst_port,
        conn->tcp_conn->iss,
        0,
        TH_SYN,
        NULL, 0
    );
    if (rv < 0) 
        return rv;

    sk->state = SS_CONNECTING;
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

void 
network_handle_tcp( 
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

//
// Super drop
//

    // #debug:
    // Not listening to these ports for now. Too much noise.
    if (dport == 80)
        return;
    if (dport == 443)
        return;

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
            tcp_seq seq = 1000;  //__generate_ISN();  //1000; // server initial sequence number
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
            conn->tcp_conn->irs     = _seq_number;      // client's ISN
            conn->tcp_conn->rcv_nxt = _seq_number + 1;  // SYN consumes 1 seq number
            conn->tcp_conn->iss     = 1000;  //__generate_ISN();  //1000; // our ISN (or randomize later)
            conn->tcp_conn->snd_una = conn->tcp_conn->iss;
            conn->tcp_conn->snd_nxt = conn->tcp_conn->iss + 1; // our SYN will consume 1

            conn->tcp_conn->snd_wnd = peer_window;  // Window

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
            //client_ep->socket = (struct socket_d *) kmalloc(sizeof(struct socket_d));
            client_ep->socket = (struct socket_d *) create_socket_object();
            if ((void*) client_ep->socket == NULL){
                panic("TCP: on client_ep->socket\n");
                //return;
            }
            client_ep->socket->family = AF_INET;
            client_ep->socket->type = SOCK_STREAM;
            client_ep->socket->protocol = IPPROTO_TCP;

            // Remote peer identity
            client_ep->socket->pid = -1;   // remote client, not a local process
            client_ep->socket->uid = 0;
            client_ep->socket->gid = 0;

            // IP/Port
            //client_ep->socket->ip_ipv6 = 0;
            client_ep->socket->ip_ipv4 = s_ipv4_int;  //NetworkSaved.caller_ip_int;
            client_ep->socket->port = sport;

            // Connection state
            client_ep->socket->state   = SS_CONNECTING;
            client_ep->socket->flags   = 0;
            client_ep->socket->conn_copy = FALSE;

            // Backlog defaults
            client_ep->socket->backlog_max = 0;
            client_ep->socket->pending_client_count = 0;
            client_ep->socket->pending_server_count = 0;
            // magic string? It indicates pending connection?
            // client_ep->socket->magic_string[0] = 'C';

            pair->c_ep = client_ep;

            // -- local ep (server) ---------------
            // Local server endpoint
            struct endpoint_d *server_ep = create_endpoint_object();
            server_ep->is_remote = FALSE;  // NOT REMOTE EP (LOCAL)
            if (!server_ep) {
                printk("Failed to create local endpoint\n");
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
            server_ep->socket = NULL;
            pair->s_ep = server_ep;

            // --------------------------------------------

            // Plug them together
            conn->ep_pair = pair;

            // --------------------------------------------

            struct socket_d *sk_listener = 
                socket_get_tcpserver_socket_by_port(11888);  // dport?
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
                    sk_listener->pending_client_count++;
                    sk_listener->state = SS_CONNECTING;

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
    
            // Example sequence/ack numbers
            //tcp_seq seq = 1000;  //__generate_ISN();  //1000;// server initial sequence number
            //tcp_ack ack = _seq_number + 1;   // acknowledge client’s ISN

            // Flags: SYN + ACK
            uint16_t flags = TH_SYN | TH_ACK;

            //
            printk(">> Sending SYN/ACK\n");

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

            // Waiting for the ACK:
            return;
        }

        // (2) SYN/ACK
        // A server accepted the connection.
        // We received a syn/ack as a response to
        // our syn sent by a process in this machine.
        // #todo: Apply the connection structure that handles this connection.

        if (fSYN == 1 && fACK == 1)
        {
            printk("TCP_SYN_ACK: SEQ={%d} | ACK={%d}\n", _seq_number, _ack_number);
            printk("TCP_SYN_ACK: Sending final ACK\n");

            // We're the client here — the remote side acked our SYN and sent
            // its own SYN. Complete the handshake with the final ACK.

            tcp_seq final_seq = _ack_number;       // = our ISN + 1, given by the server's ack
            tcp_ack final_ack = _seq_number + 1;   // acknowledge the server's ISN

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
                printk("network_handle_tcp: [step 3] cur_conn\n");
                return;
            }
            if (cur_conn->magic != 1234){
                printk("network_handle_tcp: [step 3] cur_conn validation\n");
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
            printk("TCP: Reusing conn structure   <<<<<<<< \n");
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
            printk("FIN on closed connection\n");
            return;
        }
    }

    //
    // Drop!
    //
}

