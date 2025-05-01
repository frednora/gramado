// tcp.c
// Created by Fred Nora.

#include <kernel.h>

static char tcp_payload[1024];

// ===================================================


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
    register int i=0;
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
    if (dport == 80 || dport == 443){
        printk("TCP: dport{%d}   #debug\n",dport);
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

        printk("SYN={%d} ACK={%d}\n",fSYN,fACK);

        // (1) SYN
        // A client is trying to initialize a new connection.
        if ( fSYN == 1 && fACK == 0 ){
            printk("\n");
            printk("<<<< [TCP] SYN     (1)\n");
            printk("SEQ={%d} | ACK={%d}\n",
                _seq_number, _ack_number);
            // #todo
            // Connect to the process that is listening at 11888.
            return;
        }
        // (2) SYN/ACK
        // A server accepted the connection.
        if ( fSYN == 1 && fACK == 1 ){
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

