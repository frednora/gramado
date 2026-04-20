// tcp.h 
// TCP protocol support.
// Created by Fred Nora.

#ifndef __PROT_TCP_H
#define __PROT_TCP_H    1

// Control flags:
/*
#define TCP_ACK  16
#define TCP_PSH  8
#define TCP_RST  4
#define TCP_SYN  2
#define TCP_FIN  1
*/

//
// Control bits
//

// Invalid.
#define TH_NULL  0x00

// bit 0
/* Final: Set on the last segment */
// It is used to request for connection termination.
// Sender has reached the end of its byte stream.
#define TH_FIN    0x01

// bit 1
/* Synchronization: New conn with dst port */
// It is used in first step of connection establishment phase or 
// 3-way handshake process between the two hosts. 
// Synchronizes the sequence numbers.
#define TH_SYN    0x02

// bit 2
/* Reset: Announce to peer conn terminated */
// Resets the connection.
#define TH_RST    0x04

// bit 3
/* Push: Immediately send, don't buffer seg */
// Tells the receiver to process these packets as 
// they are received instead of buffering them.
// Segment requests a PUSH.
#define TH_PUSH   0x08

// bit 4
/* Acknowledge: Part of connection establish */
// Acknowledgement field is valid.
#define TH_ACK  0x10

// bit 5
/* Urgent: send special marked segment now */
// Urgent pointer field is valid.
#define TH_URG  0x20

// bit 6
/* ECN Echo */
#define TH_ECE  0x40

// bit 7
/* Congestion Window Reduced */
#define TH_CWR  0x80

// ------------------

#define TH_NS 0x100


/*
Control Flag Bits
The control flag bit is 6 bits. 
The control flag field is basically divided into 
the following felids that are as follows:

    If the URG(Urgent pointer)=1, then the urgent pointer is in use otherwise, it is not in use.
    IF ACK = 1 means the acknowledgement number is valid, and if ACK = 0 means the segment does not contain acknowledgement.
    If PSH(Push the data without buffering) = 1 means the request to forward the data to the application layer without buffering it.
    If RST = 1 means it abruptly resets the connection whenever there is a host crash or is sometimes used to reject a segment.
    SYN: Synchronize sequence numbers during connection establishment
    
    >> Connection request: 
       SYN=1, ACK=0
    
    >> Reply: 
       SYN=1, ACK=1
    
    FIN: 
       Terminate the connection
*/

// When sending
// without a payload:  
// with a payload: 

// size:
// By default, the max limit of a TCP packet is 64K or 65535 bytes.
// 1GB if you use window scaling (by left shifting the window size by 14).
// The maximum size of a TCP packet is 64K (65535 bytes). 
// Generally, the packet size gets restricted by the 
// Maximum Transmission Unit (MTU) of network resources.
// The MTU (Maximum Transmission Unit) for Ethernet, 
// for instance, is 1500 bytes. 

typedef uint32_t  tcp_seq;
typedef uint32_t  tcp_ack;

#define TCP_HEADER_LENGHT  20

struct tcp_d
{

// Ports:
// 16,16
    uint16_t th_sport;  // source port
    uint16_t th_dport;  // destination port

// 32,32
// (uint32_t)
    tcp_seq th_seq;  // Sequence number
    tcp_ack th_ack;  // Acknowledgement number

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
    uint16_t do_res_flags;

// 16
    uint16_t window_size;

// 16
    uint16_t checksum;

// 16
    uint16_t urgent_pointer;
// -------------
// Optional data: 0~40 bytes
};

// =====================================

//
// $
// HANDLER
//

// This is called by the IP support when a TCP payload is found.
void 
network_handle_tcp( 
    const unsigned char *buffer, 
    ssize_t size );

#endif    

