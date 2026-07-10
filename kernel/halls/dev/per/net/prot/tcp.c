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

static char tcp_payload[1024];

// ===================================================

/*
void test_sending_tcp(void)
{
    printk("test_sending_tcp: sending multiple SYNs via gateway\n");

    // Google DNS (TCP 53)
    uint8_t dns_ip[4] = {8, 8, 8, 8};
    tcp_send(dhcp_info.your_ipv4, dns_ip, NetworkSaved.gateway_mac,
             12345, 53, 0x2000, 0, TH_SYN, NULL, 0);

    // Cloudflare DNS (TCP 53)
    uint8_t cf_ip[4] = {1, 1, 1, 1};
    tcp_send(dhcp_info.your_ipv4, cf_ip, NetworkSaved.gateway_mac,
             12346, 53, 0x3000, 0, TH_SYN, NULL, 0);

    // Example.com HTTP (80)
    uint8_t ex_ip[4] = {93, 184, 216, 34};
    tcp_send(dhcp_info.your_ipv4, ex_ip, NetworkSaved.gateway_mac,
             12347, 80, 0x4000, 0, TH_SYN, NULL, 0);

    // Example.com HTTPS (443)
    tcp_send(dhcp_info.your_ipv4, ex_ip, NetworkSaved.gateway_mac,
             12348, 443, 0x5000, 0, TH_SYN, NULL, 0);

    // Google DNS port 22 (SSH, usually closed)
    tcp_send(dhcp_info.your_ipv4, dns_ip, NetworkSaved.gateway_mac,
             12349, 22, 0x6000, 0, TH_SYN, NULL, 0);
}
*/

void test_sending_tcp(void)
{
    printk("test_sending_tcp: sending multiple SYNs via gateway\n");

    // Google Web (HTTP)
    uint8_t google_ip[4] = {142, 250, 190, 46};
    tcp_send(dhcp_info.your_ipv4, google_ip, NetworkSaved.gateway_mac,
             12350, 80, 0x7000, 0, TH_SYN, NULL, 0);

    // Cloudflare Web (HTTPS)
    uint8_t cf_ip[4] = {104, 16, 132, 229};
    tcp_send(dhcp_info.your_ipv4, cf_ip, NetworkSaved.gateway_mac,
             12351, 443, 0x8000, 0, TH_SYN, NULL, 0);

    // Microsoft Azure (HTTP)
    uint8_t ms_ip[4] = {20, 112, 52, 29};
    tcp_send(dhcp_info.your_ipv4, ms_ip, NetworkSaved.gateway_mac,
             12352, 80, 0x9000, 0, TH_SYN, NULL, 0);

    // Wikipedia (HTTPS)
    uint8_t wiki_ip[4] = {208, 80, 154, 224};
    tcp_send(dhcp_info.your_ipv4, wiki_ip, NetworkSaved.gateway_mac,
             12353, 443, 0xA000, 0, TH_SYN, NULL, 0);
}



int 
tcp_send(
    uint8_t source_ip[4],
    uint8_t target_ip[4],
    uint8_t target_mac[6],
    uint16_t source_port,
    uint16_t dest_port,
    tcp_seq seq,
    tcp_ack ack,
    uint16_t flags,
    const char *payload,
    size_t payload_len )
{
    // Buffer for TCP header + payload
    uint8_t segment[TCP_HEADER_LENGHT + payload_len];

    struct tcp_d tcp;

    // Fill TCP header
    tcp.th_sport = ToNetByteOrder16(source_port);
    tcp.th_dport = ToNetByteOrder16(dest_port);
    tcp.th_seq   = ToNetByteOrder32(seq);
    tcp.th_ack   = ToNetByteOrder32(ack);

    // Data offset (header length in 32-bit words = 5 for 20 bytes)
    // Reserved bits = 0, flags = passed in
    tcp.do_res_flags = ToNetByteOrder16(flags | (5 << 12));

    tcp.window_size   = ToNetByteOrder16(65535); // max window
    tcp.checksum      = 0;                       // will calculate later
    tcp.urgent_pointer= 0;

    // Copy header into buffer
    memcpy(segment, &tcp, TCP_HEADER_LENGHT);

    // Copy payload if any
    if (payload_len > 0 && payload != NULL) {
        memcpy(segment + TCP_HEADER_LENGHT, payload, payload_len);
    }

    // TODO: Calculate TCP checksum (pseudo-header + TCP header + payload)

    int rv = -1;

    // Send via IPv4
    rv = 
    (int) ipv4_send(
        PROTOCOL_IP_TCP,
        source_ip,
        target_ip,
        target_mac,
        (const char*) segment,
        TCP_HEADER_LENGHT + payload_len
    );

    return (int) rv;
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
    memset(tcp_payload,0,sizeof(tcp_payload));

// Create a local copy of the TCP payload.
    strncpy( tcp_payload, (buffer + TCP_HEADER_LENGHT), 1020 );
    tcp_payload[1021] = 0;

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

    // #debug
    if (dport == 80 || dport == 443)
    {
        printk("TCP: dport{%d} (Server not implemented yet)\n", dport);
        printk("SYN={%d} ACK={%d}\n", fSYN, fACK);

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
            tcp_send(
                dhcp_info.your_ipv4,       //my_ip,           // server IP
                NetworkSaved.caller_ipv4,  //client_ip,       // client IP
                NetworkSaved.caller_mac,   //client_mac,      // client MAC
                11888,           // server port
                sport,           // client port
                seq,
                ack,
                flags,
                NULL,            // no payload
                0
            );

            // Waiting for the ACK:

            return;
        }

    
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

