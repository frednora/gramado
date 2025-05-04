// nettest.c

#include <sys/ioctl.h>
#include <sys/ioctls.h>

#include <types.h>
#include <stddef.h>
#include <stdarg.h>   
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <termios.h>
#include <sys/socket.h>

#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

// internal
struct gdeshell_ether_header 
{
    // 14 bytes
    uint8_t dst[6];
    uint8_t src[6];
    
    // Ethertype
    // See: https://en.wikipedia.org/wiki/EtherType
    // 0x0800 	Internet Protocol version 4 (IPv4) 
    // 0x0806 	Address Resolution Protocol (ARP) 
    // ... 
    
    uint16_t type;

} __attribute__((packed));



#define gdeshell_FromNetByteOrder16(v) \
    ((v >> 8) | (v << 8))


//
// ----------------------
//

//interna
void 
print_ethernet_header ( 
    const unsigned char *Buffer, 
    int Size )
{
    struct gdeshell_ether_header *eth = 
        (struct gdeshell_ether_header *) Buffer;

    printf("\n");
    printf ("Ethernet Header\n");
    // Destination
    printf ("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", 
        eth->dst[0] , 
        eth->dst[1] , 
        eth->dst[2] , 
        eth->dst[3] , 
        eth->dst[4] , 
        eth->dst[5] );
    // Source
    printf ("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", 
        eth->src[0] , 
        eth->src[1] , 
        eth->src[2] , 
        eth->src[3] , 
        eth->src[4] , 
        eth->src[5] );
    // Protocol type.
    // ARP, IP ... ?
    printf ("   |-Protocol            : %u \n",
        (unsigned short)eth->type);
}

// ETHERNET + ARP
uint8_t test_packet[] = 
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* eth dest (broadcast) */
    0x52, 0x54, 0x00, 0x12, 0x34, 0x56,  /* eth source */
    0x08, 0x06,                          /* eth type */
    0x00, 0x01,  /* ARP htype */
    0x08, 0x00,  /* ARP ptype */
    0x06,        /* ARP hlen */
    0x04,        /* ARP plen */
    0x00, 0x01,  /* ARP opcode: ARP_REQUEST */
    0x52, 0x54, 0x00, 0x12, 0x34, 0x56,  /* ARP hsrc */
    169, 254, 13, 37,                    /* ARP psrc */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* ARP hdst */
    192, 168, 0, 137,                    /* ARP pdst */
};

/*
//interna
void
gdeshell_send_packet(void)
{
    while(1){
        gramado_system_call( 891, 
            (unsigned long)test_packet,   //buf
            (unsigned long)1500,          //len
            0);
    }    
}
*/

/*
 * nettest_decode_buffer:
 *     Decode a buffer with a network packet.
 */

int 
nettest_decode_buffer ( unsigned long buffer_address )
{
    // The ethernet header.
    struct gdeshell_ether_header *eh;
    
    //See: net/  and netinet/
    //struct ether_header *__eth;
    //struct arphdr       *__arp; 
    //struct ip           *__ip;
    //struct tcphdr       *__tcp;
    //struct udphdr       *__udp;
    // ...

    //#debug
    debug_print ("nettest_decode_buffer:\n");
    //printf ("network_decode_buffer:\n");

    if (buffer_address == 0){
        printf ("nettest_decode_buffer: [FAIL] null buffer address\n");
        return -1;
    }

	//
	// == Ethernet header =============================
	//

	//ethernet header
    //eh = (void *) &buffer[0];
    
    eh = (void *) buffer_address;

    if ( (void *) eh == NULL )
    {
        printf ("nettest_decode_buffer: [FAIL] Ethernet header\n");
        return -1;
    }else{

        //
        // Print
        //

        print_ethernet_header ( 
            (const unsigned char*) buffer_address, 1500 );

        //printf("type={%x} ",eh->type);
    };

// #importante
// Provavelmente o buffer seja enviado para um
// servidor de protocolos.
// Um servidor só para todos os protocolos.
// See: 
// https://en.wikipedia.org/wiki/EtherType
// http://www.networksorcery.com/enp/protocol/802/ethertypes.htm
// https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml
// ...

// Ethertype
// See: https://en.wikipedia.org/wiki/EtherType
// 0x0800 	Internet Protocol version 4 (IPv4) 
// 0x0806 	Address Resolution Protocol (ARP) 
// ... 
    
    uint16_t Type = gdeshell_FromNetByteOrder16(eh->type);
    
    switch ((uint16_t) Type)
    {
        // ::: IPV4
        // 0x0800	Internet Protocol version 4 (IPv4)
        // Ok como test vamos notificar o processo atual
        // de que recebemos um pacote ipv4
        
        case 0x0800:
            printf ("[0x0800]: IPV4 received.\n");
           
           //#test
           //notificando ...(ok funcionou.)
           //network_procedure ( NULL, 3000, 0,0 ); 
           
           // #debug
           //printf("todo: Internet Protocol version 4 (IPv4)\n");
           //printf("IPv4 ");
           //do_ipv4 ( (unsigned long) buffer_address );
           //refresh_screen ();
           return 0;
           break;

        //::: ARP
        //0x0806	Address Resolution Protocol (ARP)
        //#todo: devemos chamar uma rotina para tratamento de ARP e n�o
        //fazermos tudo aqui. kkk.
        case 0x0806:
            printf ("[0x0806]: ARP received\n");
            //refresh_screen ();
                   
            //printf("\nARP ");
            //do_arp ((unsigned long) buffer_address );
            //refresh_screen ();
            return 0;
            break;
            
        //::: SNMP
        // Simple Network Management Protocol.
        case 0x814C:
            printf ("[0x814C]: SNMP received\n");
            return 0;
            break;
            
        
            
        //::: IPV6
        //0x86DD	Internet Protocol Version 6 (IPv6)
        case 0x86DD:
            printf ("[0x86DD]: IPV6 received\n");
            //refresh_screen ();
        
            //printf ("IPv6 ");
            //do_ipv6 ( (unsigned long) buffer_address );
            //refresh_screen ();
            return 0;
            break;

        //:::PPP
        //Point-to-Point Protocol.
        case 0x880B:
            printf ("[0x880B]: SNMP received\n");
            return 0;
            break;

        // 0x8863 PPPoE, PPP Over Ethernet (Discovery Stage).
        // 0x8864 PPPoE, PPP Over Ethernet (PPP Session Stage).

        //::: DEFAULT
        // Error: Default package type.
        default:
            printf ("[0x%x]: protocol received\n",Type);
            return 0;
            break;
    }

    return -1;
}

/*
 * network_test_buffer:
 *     Loop to read the buffers.
 *     Called by the builting command "network"
 */

void network_test_buffer(void)
{
    char buf[4096];
    int i=0;

    debug_print("network_test_buffer:\n");
    printf("\n");
    printf("=========================\n");
    printf("network_test_buffer: [LOOP] Reading the buffers in ring3\n");

    for (i=0; i<4096; i++){
        buf[i] = 0;
    };

// Loop:
// Get the packet.
// Decode the buffer.

    while (1)
    {
        // IN: Service, buffer, lenght, nothing.
        gramado_system_call( 890, 
            (unsigned long) &buf[0], (unsigned long) 1500, 0);

        //printf("[begin]%s[end]\n",buf);

        nettest_decode_buffer((unsigned long) &buf[0]);
        
        //#test: OK. o led indica que esta enviando.
        //envia um pacote
        //gdeshell_send_packet();
    };

// Exit 
    debug_print("network_test_buffer: Done\n");
    printf     ("network_test_buffer: Done\n");
}

void network_initialize(void)
{
    debug_print("network_initialize:\n");
    printf     ("network_initialize: Initializing the ring0 network manager...\n");
    gramado_system_call(968,0,0,0);
}

// 
int main(void)
{
    network_initialize();
    network_test_buffer();
    // ...
    return 0;
}

