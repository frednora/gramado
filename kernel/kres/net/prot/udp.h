// udp.h
// UDP protocol support.
// User Datagram Protocol
// Created by Fred Nora.

#ifndef __PROT_UDP_H
#define __PROT_UDP_H    1

#define UDP_HEADER_LENGHT  8

// UDP datagram structure
// 4*2 = 8 bytes.
struct udp_d
{

// Source port number
// Destination port number
    uint16_t uh_sport;
    uint16_t uh_dport;

// Length
// This field specifies the length in bytes of the UDP header and UDP data. 
    uint16_t uh_ulen;

// Checksum
// #remember:
// When UDP runs over IPv4, 
// the checksum is computed using a "pseudo header" 
// that contains some of the same information from the real IPv4 header.
    uint16_t uh_sum;
} __attribute__ ((packed));


int
network_send_udp ( 
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    uint8_t target_mac[6], 
    unsigned short source_port,
    unsigned short target_port,
    char *data_buffer,
    size_t data_lenght );

void 
network_test_udp0(
    uint8_t tmac[6], 
    uint8_t tip[4],
    unsigned short sport,
    unsigned short dport );
void network_test_udp(void);
void network_test_udp2(void);

void udp_save_mac( uint8_t mac[6] );

//
// $
// HANDLER
//

// This is called by the IP support when a UDP payload is found.
void 
network_handle_udp( 
    const unsigned char *buffer, 
    ssize_t size );


#endif   
