// #test
// Send udp using raw socket.
// Created by Copilot.



// #test
#include <inttypes.h>
#include <stdint.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>

// =======================================================================================

//#include <stdio.h>
//#include <stdint.h>
//#include <arpa/inet.h>

#define myINADDR_NONE  0xFFFFFFFF

static in_addr_t my_inet_addr(const char *cp);

static in_addr_t my_inet_addr(const char *cp) 
{
    in_addr_t addr = 0;
    int octet;
    char ch;
    int i;
    for (i = 0; i < 4; i++) {
        octet = 0;
        while (*cp && *cp != '.') {
            ch = *cp++;
            if (ch < '0' || ch > '9') {
                return myINADDR_NONE; // Invalid character
            }
            octet = octet * 10 + (ch - '0');
        }
        if (octet > 255) {
            return myINADDR_NONE; // Invalid octet
        }
        addr = (addr << 8) | octet;
        if (*cp) {
            cp++;
        }
    }
    return htonl(addr);
}

/*
//testing
int test00() {
    const char *ip = "192.168.1.1";
    in_addr_t addr = my_inet_addr(ip);
    if (addr == INADDR_NONE) {
        printf("Invalid IP address\n");
    } else {
        printf("IP address in network byte order: %u\n", addr);
    }
    return 0;
}
*/

//========================================================================================

// Calculate the checksum for the packet
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


int main(int argc, char *argv[]) 
{
    static char packet[4090];
    unsigned long packet_address = (unsigned long) &packet[0];

    // The message string
    const char *msg = "Hello, UDP!";

    int UseFastSend = TRUE;

    if (argc != 4) {
        printf("Usage: %s <source IP> <destination IP> <destination port>\n", argv[0]);
        return 1;
    }

    int sockfd;
    struct sockaddr_in dest_addr;


// ETHERNET
    //#define __ethHEADERLENGHT  14

// IP
    //#define __ipHEADERLENGHT  20
    struct iphdr *ip_hdr = (struct iphdr *) (packet_address + 14);

// UDP
    //#define __udpHEADERLENGHT  8  
    struct udphdr *udp_hdr = 
        (struct udphdr *) ( packet_address + 14 + 20);

// Create raw socket
    //sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    //sockfd = socket(AF_INET, SOCK_RAW, 88111);  //#test
    sockfd = socket(AF_INET, SOCK_RAW, 0);  //#test
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // Set destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = my_inet_addr(argv[2]);

// Cleat
    memset(packet, 0, sizeof(packet));

/*
// -------------------
// Prepare ethernet
    int i;
    char mac00[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    char mac01[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    for (i=0; i<6; i++){
        packet[i]   = (uint8_t) mac00[i];  // source 
    };
    for (i=0; i<6; i++){
        packet[i+6] = (uint8_t) mac01[i];  // dest
    };
    packet[12] = 0x08;
    packet[13] = 0x00;
    //Leh.type = (uint16_t) ToNetByteOrder16(ETHERTYPE_IPV4);
*/


    packet[0] = 0;
    packet[1] = 0;
    packet[2] = 0;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;

    packet[6] = 0;
    packet[7] = 0;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = 0;
    packet[11] = 0;

    packet[12] = 0x08; //ok
    packet[13] = 0x00;

    //#debug
    //printf ("packet={%x} ip_hdr={%x} \n", packet_address,ip_hdr);
    //while(1){}

// -------------------
// Prepare IP header
    ip_hdr->ip_hl = 5;
    ip_hdr->ip_v = 4;
    ip_hdr->ip_tos = 0;
    ip_hdr->ip_len = htons( 20 + 8 + strlen(msg));
    ip_hdr->ip_id = htons(54321);
    ip_hdr->ip_off = 0;
    ip_hdr->ip_ttl = 255;
    ip_hdr->ip_p = IPPROTO_UDP;
    ip_hdr->ip_sum = 0;

    struct in_addr s00;
    s00.s_addr = my_inet_addr(argv[1]);
    struct in_addr d00;
    d00.s_addr = my_inet_addr(argv[2]);

    ip_hdr->ip_src = s00; //my_inet_addr(argv[1]);
    ip_hdr->ip_dst = d00; //my_inet_addr(argv[2]);

    ip_hdr->ip_sum = 
        checksum((unsigned short *)packet, ip_hdr->ip_len);

// -------------------
// Prepare UDP header
    udp_hdr->uh_sport = htons(12345);
    udp_hdr->uh_dport = htons(atoi(argv[3]));
    udp_hdr->uh_ulen = htons(sizeof(struct udphdr) + strlen(msg));
    udp_hdr->uh_sum = 0;

// ===================================

/*
// Payload    
    char *data = 
        packet + 
        __ethHEADERLENGHT + 
        __ipHEADERLENGHT + 
        __udpHEADERLENGHT;
*/

    unsigned long data_address = packet_address + 14 + 20 +  8;

// Copy data
    strcpy(data_address, msg);

// Send
    if (UseFastSend)
    {
        while (1){
            rtl_send_raw_packet( packet, sizeof(packet) );
        }
        close(sockfd);
        return 0;
    }

// Write!
// Send packet
// Write it on our own socket.
    if (
        sendto(
            sockfd, 
            packet, 
            ntohs(ip_hdr->ip_len), 
            0, 
            (struct sockaddr *)&dest_addr, 
            sizeof(dest_addr)
            ) < 0 ) 
    {
        perror("sendto");
        close(sockfd);
        return 1;
    }

    printf("UDP packet sent to %s:%s\n", argv[2], argv[3]);

    close(sockfd);
    return 0;
}

// gcc -o raw_udp raw_udp.c
// sudo ./raw_udp <source IP> <destination IP> <destination port>

