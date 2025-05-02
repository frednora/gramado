// ipv4.c
// Created by Fred Nora.

#include <kernel.h>


static unsigned int ipv4_counter = 0;


// ======================================================

int 
ipv4_send (
    unsigned char protocol, 
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    uint8_t target_mac[6],
    char *data_buffer,     // IPV4 payload
    size_t data_lenght )
{
    int Status = -1;
    register int i=0;
// Frame base. (The IPV4 payload)
    unsigned long addr = (unsigned long) data_buffer;

    // #todo
    //if ( (void*) currentNIC == NULL )
        //goto fail;

//==============================================
// # ethernet header #

// Ethernet base
    struct ether_header  Leh;
// Destination MAC, Our MAC and Type of protocol.
    for (i=0; i<6; i++){
        Leh.mac_src[i] = (uint8_t) currentNIC->mac_address[i];  // source 
        Leh.mac_dst[i] = (uint8_t) target_mac[i];               // dest
    };
    Leh.type = (uint16_t) ToNetByteOrder16(ETHERTYPE_IPV4);


//==============================================
// # ipv4 header #

// IPV4 base
    struct ip_d  Lipv4;

// The Ipv4 header
    //hdr->ihl = 5;
    //hdr->ver = 4;
    //hdr->tos = 0;

//>>>>
    Lipv4.v_hl = 0x45;    // 8 bit

// Type of service (8bits)
// - Differentiated Services Code Point (6bits)
// - Explicit Congestion Notification (2bits)
    Lipv4.ip_tos = 0x00;  // 8 bit (0=Normal)

// Lenght
// IPV4 Length
// ip + (ip payload)
// 16 bit
// This 16-bit field defines the entire packet size in bytes, 
// including header and data. 
// The minimum size is 20 bytes (header without data) and the maximum is 65,535 bytes. 
// ip header + (udp header + data).
// #todo: Check if it is right?
// No payload do ip temos o (udp+data)
// O lenght do protocolo precisa conter o seu proprio header e o seu proprio payload.

    uint16_t xxxdata = (uint16_t) (data_lenght & 0xFFFF);
    uint16_t __ipheaderlen = IP_HEADER_LENGHT;
    uint16_t __ippayloadlen = (uint16_t) (UDP_HEADER_LENGHT +  xxxdata);
    uint16_t __iplen = (uint16_t) (__ipheaderlen + __ippayloadlen);
// Len
    Lipv4.ip_len = (uint16_t) ToNetByteOrder16(__iplen);

// Identification
// ... identifying the group of fragments of a single IP datagram. 
// 16 bit

    Lipv4.ip_id = ToNetByteOrder16(ipv4_counter);
    ipv4_counter++;

    Lipv4.ip_off = ToNetByteOrder16(0x4000);  //htons(0x4000);
    Lipv4.ip_ttl = 64;
    Lipv4.ip_p = protocol;
    Lipv4.ip_sum = 0;   // Checksum

//
// IPV4
//

    //network_fill_ipv4(hdr->src, source_ip);
    //network_fill_ipv4(hdr->dst, target_ip);

    unsigned char *spa = (unsigned char *) &Lipv4.ip_src.s_addr;
    unsigned char *tpa = (unsigned char *) &Lipv4.ip_dst.s_addr;

    for (i=0; i<4; i++){
        spa[i] = (uint8_t) source_ip[i]; 
        tpa[i] = (uint8_t) target_ip[i]; 
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
    Lipv4.ip_sum =
         (uint16_t) ToNetByteOrder16(Lipv4.ip_sum);

// #debug
    // printk("ip_sum={%x} \n",Lipv4.ip_sum);

    //printk ("size %d\n", sizeof (struct ip_d) );
    //refresh_screen();
    //while(1){}

// Send frame via hardware
    size_t FRAME_SIZE = 
               ( ETHERNET_HEADER_LENGHT +\
                 IP_HEADER_LENGHT +\
                 data_lenght );

// Send
    Status = (int) ethernet_send( currentNIC, FRAME_SIZE, addr );
    if (Status<0){
        goto fail;
    }

    return 0;
fail:
    return (int) -1;
}

//
// $
// HANDLER
//

// When receving IPV4 packet from NIC device.
// handle ipv4 package
// IN:
// buffer = ip header base address.
// size = ip packet size. (ip header + ip payload).
void 
network_handle_ipv4( 
    const unsigned char *buffer, 
    ssize_t size )
{
// Network layer.
// Called by network_on_receiving() in network.c.
// 0x0800

// The buffer.
    struct ip_d *ip;
// The protocol for the payload.
    uint8_t Protocol=0;

    //printk ("IP: received\n");

// #warning
// It's ok to use pointer here.
// We're not allocating memory, we're using 
// a pre-allocated buffer.
    ip = (struct ip_d *) buffer;
    if ((void*) ip == NULL){
        printk("network_handle_ipv4: ip\n");
        goto fail;
    }

// The minimum size.
// Only the ip header.
    //if (size < IP_HEADER_LENGHT){
    //    printk("network_handle_ipv4: size\n");
    //    goto fail;
    //}

// IP
// #todo
// Let's save it for future use.
// We're gonna need to send responses.

    unsigned char *src_ipv4 = (unsigned char *) &ip->ip_src.s_addr;
    unsigned char *dst_ipv4 = (unsigned char *) &ip->ip_dst.s_addr;

// Save the IP of the caller.
// For fast response.
    network_fill_ipv4(
        NetworkSaved.caller_ipv4, 
        src_ipv4 );

// Show data.
// Bytes: Net-style.

    uint8_t v_hl = (uint8_t) ip->v_hl;
    uint8_t Version = (uint8_t) ((v_hl >> 4) & 0x0F);
    uint8_t Lenght  = (uint8_t) (v_hl & 0x0F);  // Header lenght. 5=20bytes.

    //printk("IP Version: {%d}\n", Version);
    //printk("Header lenght: {%d}\n", Lenght);

    if (Version != 4){
        printk("IP: Not version 4\n");
        goto fail;
    }

// Total lenght (16bits)
// ip header + ip payload.
// (IP + (TCP + data)) given in bytes.
// 20~65535
    //printk("Total lenght: {%d}\n",ip->ip_len);

// #bugbug
// What is the style of this information?
// little endian?
    uint16_t ip_lenght = (uint16_t) FromNetByteOrder16(ip->ip_len);
    if ( ip_lenght < 20 || ip_lenght > 65535 )
    {
        //#debug
        printk("IP: size={%d}\n",size);
        printk("IP: ip_lenght={%d}\n",ip_lenght);
        // #todo:
        // Maybe simply drop it for now.
        panic("IP: Bad total lenght #debug\n");
    }


    unsigned char *payload_pointer;
    ssize_t payload_size;

    payload_pointer = (buffer + IP_HEADER_LENGHT);
    payload_size = (ip_lenght - IP_HEADER_LENGHT);


    //printk ("target: %d.%d.%d.%d \n",
    //    dst_ipv4[0],dst_ipv4[1],dst_ipv4[2],dst_ipv4[3]);
    //printk("ip_sum={%x} \n",ip->ip_sum);

//
// Protocol
//

// List of IP protocol numbers
// See:
// https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers

// 0x01 -  1 - ICMP - Internet Control Message Protocol
// 0x06 -  6 - TCP  - Transmission Control Protocol
// 0x11 - 17 - UDP  - User Datagram Protocol

    Protocol = (uint8_t) ip->ip_p;
    //printk("Protocol: {%xH}\n",Protocol);

    switch (Protocol)
    {
        case PROTOCOL_IP_TCP:
            network_handle_tcp( payload_pointer, payload_size );
            return;
            break;
        case PROTOCOL_IP_UDP:
            network_handle_udp( payload_pointer, payload_size );
            return;
            break;

        // Not supported ipv4 protocol.
        case PROTOCOL_IP_ICMP:
        // ...
        default:
            goto drop;
            break;
    };

// Something went wrong!
    goto fail;

drop:
    return;

// debug:
// ---------------
// Not to me.
    /*
    if ( dst_ipv4[3] != 12 ||
         dst_ipv4[2] != 1 ||
         dst_ipv4[1] != 168 ||
         dst_ipv4[0] != 192 )
    {
        printk ("IP: NOT TO ME. Drop it\n");
        return;
    }
    */

// ---------------
// To me.

    //printk ("IP: TO ME!\n");
    //printk("Src IPV4: {%x}\n", ip->ip_src.s_addr);
    //printk("Dst IPV4: {%x}\n", ip->ip_dst.s_addr);
    // destination

    /*
    printk ("Src: %d.%d.%d.%d\n",
        src_ipv4[0], src_ipv4[1], src_ipv4[2], src_ipv4[3]);
    printk ("Dst:  %d.%d.%d.%d\n",
        dst_ipv4[0], dst_ipv4[1], dst_ipv4[2], dst_ipv4[3]);

    // hang
    printk("network_handle_ipv4: #breakpoint :)\n");
    while (1){
    };
    */

fail:
    printk("network_handle_ipv4: Fail\n");
    return;
}



