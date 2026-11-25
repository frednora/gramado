// Estruturas para pacotes.
// Vamos usar as estruturas de headers 
// para construirmos os pacotes.



/* Global variables */
/*
u8 src_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 dst_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 dst_broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u8 src_IP[4] = {0, 0, 0, 0};
u8 src_SN[4] = {0, 0, 0, 0};
u8 src_GW[4] = {0, 0, 0, 0};
u8 dst_IP[4] = {0, 0, 0, 0};
unsigned char buffer[ETH_FRAME_LEN];
unsigned char tosend[ETH_FRAME_LEN];
*/

struct arp_packet
{
    struct ether_header  eh;  //ethernet.h
    struct ether_arp     ah;  //arp.h
};



struct ipv4_packet
{
    struct ether_header     eh;   //ethernet.h
    struct ipv4_header_d  ipv4h;  //ip.h  
};


//#todo
/*
struct icmp_packet
{
    struct ipv4_packet ipv4p; // //packet.h

    u8 type;
    u8 code;
    u16 checksum;
    u16 id;
    u16 sequence;
    u64 timestamp;
    u8 data[2]; // Set to 2 so can be used as pointer
};
*/


struct udp_packet
{
    struct ipv4_packet   ipv4p;  //packet.h
    struct udp_header_d  udph;   //ip.h
};


struct tcp_packet
{
    struct ipv4_packet   ipv4p;  //packet.h

    // See: libc03/include/netinet/tcp.h
    // for another version.
    unsigned short src_port;
    unsigned short dest_port;
    unsigned long seqnum;
    unsigned long acknum;
    unsigned char data_offset;
    unsigned char flags;
    unsigned short window;
    unsigned short checksum;
    unsigned short urg_pointer;
};















