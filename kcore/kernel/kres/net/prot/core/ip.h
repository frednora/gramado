// ip.h
// Created by Fred Nora.

#ifndef __NET_IP_H
#define __NET_IP_H    1

#define IPVERSION    4

#define MAC_IN_BYTES  6
#define IPV4_IN_BYTES  4
//#define IPV6_IN_BYTES  ?


//
// Protocols
//

#define PROTOCOL_IP_ICMP  1
#define PROTOCOL_IP_TCP   6
#define PROTOCOL_IP_UDP   17
// ...

#define IP_RF  0x8000			/* reserved fragment flag */
#define IP_EF  0x8000			/* evil flag, per RFC 3514 */
#define IP_DF  0x4000			/* dont fragment flag */
#define IP_MF  0x2000			/* more fragments flag */
#define IP_OFFMASK  0x1fff		/* mask for fragmenting bits */

// ??
#define IP_MINFRAGSIZE  69     /* minumum size that can be fraged */
#define IP_MAXPACKET    65535  /* maximum packet size */

#define IP_HEADER_LENGHT  20

// ipv4
// Lenght = 20 bytes
struct ip_d
{

// - Version (4bits)
// - IHL (4bits). Lenght of the header in chuncks of 4 bytes. 
    u_int8_t v_hl;

// Type of service (8bits)
// - Differentiated Services Code Point (6bits)
// - Explicit Congestion Notification (2bits)
    u_int8_t ip_tos;

// Total lenght (16bits)
// IP + (ip payload).
// (IP + (TCP + data)) given in bytes.
// 20~65535
// This 16-bit field defines the entire packet size in bytes, 
// including header and data. 
// The minimum size is 20 bytes (header without data) and 
// the maximum is 65,535 bytes. 
    u_int16_t ip_len;

//-------------------------------

// Identification (16bits)
// When the message is large and we have a lot of packets.
// Actually they are 'fragments' of a packet.
    u_int16_t ip_id;

// Flags (3bits) (Do we have fragments?)
// Fragment offset (13bits) (fragment position)
    u_int16_t ip_off;

//-------------------------------

// Time to live (8bits)
// Counter 
    u_int8_t ip_ttl;

// Protocol (8bits)
    u_int8_t ip_p;

// Checksum (16bits)
// Only the header.
    u_int16_t ip_sum;

//-------------------------------

// IP:
// Source and dest IP address.
// Address in network byte order.
// See: net/in.h 
    struct in_addr  ip_src;  //(32bits)

//-------------------------------

    struct in_addr  ip_dst;  //(32bits) 

// Optional if IHL > 5.
// 0~320 bits.
} __attribute__ ((packed));

// =====================================

int 
ipv4_send (
    unsigned char protocol, 
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    uint8_t target_mac[6],
    char *data_buffer, 
    size_t data_lenght );


//
// $
// HANDLER
//

void 
network_handle_ipv4( 
    const unsigned char *buffer, 
    ssize_t size );


#endif   


