
// decode.c

#include <gnsint.h>


// ARP packet:
// ETHERNET HEADER + ARP HEADER.
uint8_t test_packet[] = {

// ethernet
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  /* eth dest (broadcast) */
    0x52, 0x54, 0x00, 0x12, 0x34, 0x56,  /* eth source */
    0x08, 0x06,                          /* eth type */

// arp
    0x00, 0x01, /* ARP htype */
    0x08, 0x00, /* ARP ptype */
    0x06, /* ARP hlen */
    0x04, /* ARP plen */
    0x00, 0x01, /* ARP opcode: ARP_REQUEST */
    0x52, 0x54, 0x00, 0x12, 0x34, 0x56, /* ARP hsrc */ //#todo
    192, 168, 1, 112,                   /* ARP psrc */ //#todo
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ARP hdst */ //#todo
    192, 168, 1, 8,                     /* ARP pdst */ //#todo
};

// ========================================================

void 
print_ethernet_header( 
    const unsigned char *Buffer, 
    int Size )
{
    struct gnssrv_ether_header *eth = 
        (struct gnssrv_ether_header *) Buffer;
     
    //printf ("print_ethernet_header:\n");
    printf("\n");
    printf ("Ethernet Header\n");
    printf ("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", 
        eth->dst[0] , 
        eth->dst[1] , 
        eth->dst[2] , 
        eth->dst[3] , 
        eth->dst[4] , 
        eth->dst[5] );
    printf ("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", 
        eth->src[0] , 
        eth->src[1] , 
        eth->src[2] , 
        eth->src[3] , 
        eth->src[4] , 
        eth->src[5] );
    printf ("   |-Protocol            : %u \n",
        (unsigned short)eth->type);
}


int gnssrv_decode_buffer(unsigned long buffer_address)
{
// Only the ethernet header.
    struct gnssrv_ether_header *eh;

    //#debug
    debug_print ("gnssrv_decode_buffer:\n");
    //printf ("network_decode_buffer:\n");

    if (buffer_address == 0){
        printf ("gnssrv_decode_buffer: buffer\n");
        return 1;
    }

//
// ## eth header ##
//

// ethernet header
    //eh = (void *) &buffer[0];
    eh = (void *) buffer_address;
    if ( (void *) eh == NULL ){
        printf ("gnssrv_decode_buffer: eh\n");
        //refresh_screen();
        return 1;
    }else{

        print_ethernet_header(
            (const unsigned char*) buffer_address, 
            1500 );

        //printf("src: ");
        //for( i=0; i<6; i++)
        //    printf("%x ",eh->src[i]);

        //printf("dst: ");
        //for( i=0; i<6; i++)
        //    printf("%x ",eh->dst[i]);

        //printf("type={%x} ",eh->type);
    };

    uint16_t type = gnssrv_FromNetByteOrder16(eh->type);

    switch ((uint16_t) type)
    {
        // ::: IPV4
        // 0x0800	Internet Protocol version 4 (IPv4)
        // Ok como test vamos notificar o processo atual
        // de que recebemos um pacote ipv4
        case 0x0800:
        
           // #debug
            printf ("IPV4 received\n");
            //refresh_screen ();
           
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
            // #debug
            printf ("ARP received\n");
            //refresh_screen ();
                   
            //printf("\nARP ");
            //do_arp ((unsigned long) buffer_address );
            //refresh_screen ();
            return 0;
            break;
            
        //::: SNMP
        // Simple Network Management Protocol.
        case 0x814C:
            printf ("SNMP received\n");
            return 0;
            break;

        //::: IPV6
        //0x86DD	Internet Protocol Version 6 (IPv6)
        case 0x86DD:

            // #debug
            printf ("IPV6 received\n");
            //refresh_screen ();
        
            //printf ("IPv6 ");
            //do_ipv6 ( (unsigned long) buffer_address );
            //refresh_screen ();
            return 0;
            break;

        //:::PPP
        //Point-to-Point Protocol.
        case 0x880B:
            printf ("SNMP received\n");
            return 0;
            break;

        //0x8863	PPPoE, PPP Over Ethernet (Discovery Stage).
        //0x8864	PPPoE, PPP Over Ethernet (PPP Session Stage).

        //::: DEFAULT
        // Error: Default package type.
        default:
            // #debug
            //printf ("UNKNOWN received\n");
            //refresh_screen ();
            // #debug
            //printf("default ethernet type\n");
            //refresh_screen();
            return 0;
            break;
    }

    return 1;
}


// ?
void gnssrv_send_packet(void)
{
    while(1){
        gramado_system_call( 891, 
            (unsigned long)test_packet,  //buf
            (unsigned long)1500,         //len
            0);
    }    
}


void gnssrv_get_and_decode_buffer(void)
{
    char buf[4096];

// Pega o pacote
    gramado_system_call( 
        890, 
        (unsigned long) &buf[0],  //buf
        (unsigned long) 1500,     //len
        0);

    //printf("[begin]%s[end]\n",buf);

    //#test: OK
    //decodifica o pacote.
    //gnssrv_decode_buffer( (unsigned long) &buf[0] );
    gnssrv_decode_buffer(buf);

// #test:
    //gnssrv_send_packet();
}


/*
//talvez possamos usar isso sem o loop.
void network_test_buffer(void);
void network_test_buffer(void)
{
    char buf[4096];
    
    while(1){
        
        //pega o pacote
        gramado_system_call( 890, 
            (unsigned long)&buf[0],   //buf
            (unsigned long)1500,      //len
            0);

        //printf("[begin]%s[end]\n",buf);
        
        //#test: OK
        //decodifica o pacote.
        gdeshell_decode_buffer((unsigned long) &buf[0]);
        
        //#test: OK. o led indica que esta enviando.
        //envia um pacote
        //gdeshell_send_packet();
    }
}
*/

