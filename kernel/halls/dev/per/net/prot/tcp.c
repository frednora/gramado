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

static char __tcp_payload[1024];

static uint16_t 
__tcp_checksum(
    uint8_t src_ip[4],
    uint8_t dst_ip[4],
    const uint8_t *tcp_segment,
    size_t tcp_len );

// ===================================================


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
    Lipv4.ip_off = ToNetByteOrder16(0x4000); 

    // Time to live (8bits)
    Lipv4.ip_ttl = 255;  // 64

    // Protocol (8bit)
    Lipv4.ip_p = 0x06;  // TCP


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
              (const unsigned char *) &Lipv4 + sizeof(struct ip_d));
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
    Ltcp.th_seq   = ToNetByteOrder32(seq);
    Ltcp.th_ack   = ToNetByteOrder32(ack);

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

    // Urgent pointer (16 bits)
    Ltcp.urgent_pointer = 0;

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

// Get the buffer address based on its offset.
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

    if ((void*) frame == NULL)
    {
        // #debug
        // We don't need panic here
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

    int rv = -1;

// Send frame via current NIC
    rv = (int) ethernet_send( FRAME_SIZE, frame );
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

    printk("Done\n");
    return 0;

fail:
    printk ("Fail\n");
    return -1;
}


void test_sending_tcp(void)
{
    unsigned short SourcePort = 11888;
    char payload[4];
    size_t payload_len = 0;  // 1
    payload[0]=0;
    payload[1]=0;
    payload[2]=0;
    payload[3]=0;

    printk("test_sending_tcp: sending SYNs to external targets\n");

    // Google Web (HTTP)
    uint8_t google_ip[4] = {142, 250, 190, 46};
    network_send_tcp(
        dhcp_info.your_ipv4,
        google_ip,
        NetworkSaved.gateway_mac,
        SourcePort,   // source port
        80,      // dest port
        0x1000,  // seq
        0,       // ack
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


//
// $
// HANDLER
//

// When receiving tcp frame from NIC device.
void 
network_handle_tcp( 
    const unsigned char *buffer, 
    ssize_t size )
{
// Transport layer.
// The TCP header tracks the state of 
// communication between two TCP endpoints.

    struct tcp_d *tcp;
    //register int i=0;
    uint16_t flags=0;

    // #debug
    //printk("network_handle_tcp: #todo\n");

// Parameters
    if ((void*) buffer == NULL){
        printk("network_handle_tcp: buffer\n");
        return;
    }
    //if (size < 0){
    //}

// #warning
// It's ok to use pointer here.
// We're not allocating memory, we're using 
// a pre-allocated buffer.
    tcp = (struct tcp_d *) buffer;

    uint16_t sport = (uint16_t) FromNetByteOrder16(tcp->th_sport);
    uint16_t dport = (uint16_t) FromNetByteOrder16(tcp->th_dport);

    tcp_seq _seq_number = (tcp_seq) FromNetByteOrder32(tcp->th_seq);
    tcp_ack _ack_number = (tcp_ack) FromNetByteOrder32(tcp->th_ack);

// Clean the payload local buffer.
    memset(__tcp_payload,0,sizeof(__tcp_payload));

// Create a local copy of the TCP payload.
    strncpy( __tcp_payload, (buffer + TCP_HEADER_LENGHT), 1020 );
    __tcp_payload[1021] = 0;

//
// Control fields
//
    flags = (uint16_t) FromNetByteOrder16(tcp->do_res_flags);

    //#debug
    //printk("Flags={%x}\n",flags);

// Control flags (6 bits)
    uint16_t fFIN=0;
    uint16_t fSYN=0;  // SYN :)
    uint16_t fRST=0;
    uint16_t fPUSH=0;
    uint16_t fACK=0;  // ACK :)
    uint16_t fURG=0;

    if (flags & TH_FIN){
        fFIN = 1;
    }
    if (flags & TH_SYN){
        fSYN = 1;
    }
    if (flags & TH_RST){
        fRST = 1;
    }
    if (flags & TH_PUSH){
        fPUSH = 1;
    }
    if (flags & TH_ACK){
        fACK = 1;
    }
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

//
// ports
//

    //#debug
    //printk("TCP: sport{%d}   #debug\n",sport);
    //printk("TCP: dport{%d}   #debug\n",dport);

    // #todo
    // Ignore it for now.
    if (dport == 443)
    {
        return;  // No verbose
    }

    // #todo
    if (dport == 80)
    {
        printk("TCP: dport{%d} (Server not implemented yet)\n", dport);
        printk("SYN={%d} ACK={%d}\n", fSYN, fACK);

        if ( fSYN == 1 && fACK == 1 )
        {
            printk("TCP: SYS/ACK received in port{%d}\n", dport);
        }

        return;
    }


// Show

// Special port.
// Just a test.
    if (dport == 11888)
    {
        printk ("------------------------\n");
        printk ("---- [11888] << TCP ----\n");

        // >> Connection request: 
        // SYN=1, ACK=0
        // >> Reply: 
        // SYN=1, ACK=1

        printk("SYN={%d} ACK={%d}\n", fSYN, fACK);

        // (1) SYN
        // A client is trying to initialize a new connection.
        // The client shares an Initial Sequence Number (ISN) with the server.
        // It means the server here needs to respond.
        if ( fSYN == 1 && fACK == 0 )
        {
            printk("\n");
            printk("<<<< [TCP] SYN     (1)\n");
            printk("SEQ={%d} | ACK={%d}\n", _seq_number, _ack_number );

            // #todo
            // The client is saying: "I want to connect to the 
            // server process that is listening to the port 11888"

            // #todo
            // >>> Lets work on this response! <<<
    
            // Example sequence/ack numbers
            tcp_seq seq = 1000;              // server initial sequence number
            tcp_ack ack = _seq_number + 1;   // acknowledge client’s ISN

            // Flags: SYN + ACK
            uint16_t flags = TH_SYN | TH_ACK;

            //
            printk(">> Sending SYN/ACK\n");

            // No payload for handshake
            char dummy_payload[4];
            dummy_payload[0] = 0;
            dummy_payload[1] = 0;

            network_send_tcp(
                dhcp_info.your_ipv4,       //my_ip,           // server IP
                NetworkSaved.caller_ipv4,  //client_ip,       // client IP
                NetworkSaved.caller_mac,   //client_mac,      // client MAC
                11888,           // server port (source=we)
                sport,           // client port (target)
                seq,
                ack,
                flags,
                dummy_payload,  // No tcp payload
                1               // No tcp payload lenght
            );

            // Waiting for the ACK:

            return;
        }

        /*
        // (2) SYN/ACK
        // A server accepted the connection.
        if ( fSYN == 1 && fACK == 1 )
        {
            printk("\n");
            printk("<<<< [TCP] SYN/ACK (2)\n");
            printk("SEQ={%d} | ACK={%d}\n",
                _seq_number, _ack_number);
            // #todo
            // We received a syn/ack as a response to
            // our syn sent by a process in this machine.
            return;
        }
        */

        if ( fSYN == 1 && fACK == 1 )
        {
            printk("\n");
            printk("<<<< [TCP] SYN/ACK (2)\n");
            printk("SEQ={%d} | ACK={%d}\n", _seq_number, _ack_number);

            // We're the client here — the remote side acked our SYN and sent
            // its own SYN. Complete the handshake with the final ACK.

            tcp_seq final_seq = _ack_number;       // = our ISN + 1, given by the server's ack
            tcp_ack final_ack = _seq_number + 1;   // acknowledge the server's ISN

            printk(">> Sending final ACK (3)\n");

            char no_payload[1];
            no_payload[0] = 0;

            network_send_tcp(
                dhcp_info.your_ipv4,        // our IP
                NetworkSaved.caller_ipv4,   // remote IP (whoever this packet came from)
                NetworkSaved.caller_mac,    // remote MAC
                11888,                      // our local port (source)
                sport,                      // remote port (target) — 80 or 443
                final_seq,
                final_ack,
                TH_ACK,                     // ACK only, no SYN
                no_payload,
                0                            // no payload — pure ACK doesn't consume a seq number
            );

            return;
        }


        // (3) ACK
        // A client is confirming the connection we accepted.
        if ( fSYN == 0 && fACK == 1 ){
            printk("\n");
            printk("<<<< [TCP] ACK     (3)\n");
            printk("SEQ={%d} | ACK={%d}\n",
                _seq_number, _ack_number);
            // #todo
            // We received an ack as a response to
            // our syn/ack sent by a process in this machine.
            return;
        }
    }

    //if (dport == 11888)
    //{
        //printk("TCP: MESSAGE: {%s}\n", tcp_payload );
        //memset(tcp_payload,0,sizeof(tcp_payload));
    //}

    //
    // Drop!
    //
}

