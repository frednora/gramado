// udp.c
// Created by Fred Nora.

#include <kernel.h>

/*
#define __UDP_PACKET_SIZE  ETHERNET_HEADER_LENGHT +\
IP_HEADER_LENGHT +\
UDP_HEADER_LENGHT + 512  
*/

//char udp_packet[__UDP_PACKET_SIZE];
static char udp_payload[1024];

// source ip
unsigned char __udp_gramado_default_ipv4[4] = { 
    192, 168, 1, 12 
};
// destination ip (linux)
unsigned char __udp_target_default_ipv4[4]  = { 
    192, 168, 1, 10  //linux
};
// destination ip (gateway)
unsigned char __udp_target_gateway_ipv4[4]  = { 
    192, 168, 1, 1  //gateway
};

// Target MAC.
unsigned char __udp_target_mac[6] = { 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
};

unsigned char udp_saved_mac[6] = { 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
};


static void __udp_clear_payload_buffer(void);
static void __handle_gprotocol(uint16_t s_port, uint16_t d_port);


//
// =========================================================
//

// Clean the payload local buffer.
static void __udp_clear_payload_buffer(void)
{
    memset( udp_payload, 0, sizeof(udp_payload) );
}

// #test: 
// Respond the UDP message receive on port 11888.
// Somente se o dhcp foi initializado.
// + Nosso IP ficou registrado na estrutura de dhcp.
// + #todo: 
//   O IP do alvo deveria estar salvo em alguma estrutura
//   provavelmente na estrutura de IP.
// + #todo: 
//   O MAC do alvo ficaria registrado na estrutura de ethernet.
static void __handle_gprotocol(uint16_t s_port, uint16_t d_port)
{
    uint16_t sport = s_port;
    uint16_t dport = d_port;
    const uint16_t OurPort = 11888;
    size_t MessageSize = 256;

    int NoReply = TRUE;

    if (dhcp_info.initialized != TRUE)
        return;

    if (dport != OurPort)
        return;

// ----------------
// g:0
// packet type: 0 = request
    if ( udp_payload[0] == 'g' && 
         udp_payload[1] == ':' && 
         udp_payload[2] == '0' )
    {
        //printk("[request]\n"); refresh_screen();
        memset(udp_payload, 0, sizeof(udp_payload));
        ksprintf(udp_payload,"g:1 ");  // Reply code
        ksprintf(
            (udp_payload + 4),
            "This is a response from Gramado OS\n");
        NoReply = FALSE;
        goto done;
    }

// -----------------------
// g:1
// packet type: 1 = reply
    if ( udp_payload[0] == 'g' && 
         udp_payload[1] == ':' && 
         udp_payload[2] == '1' )
    {
        //memset(udp_payload, 0, sizeof(udp_payload));

        // #debug
        printk("[g:1] REPLY\n");
        refresh_screen();

        NoReply = TRUE;
        goto done;
    }

// -------------------------
// g:2
// packet type: 2 = event
    if ( udp_payload[0] == 'g' && 
         udp_payload[1] == ':' && 
         udp_payload[2] == '2' )
    {
        //memset(udp_payload, 0, sizeof(udp_payload));

        // #debug
        printk("[g:2] EVENT\n");
        refresh_screen();

        NoReply = TRUE;
        goto done;
    }

// ---------------------------
// g:3
// packet type: 3 = error
    if ( udp_payload[0] == 'g' && 
         udp_payload[1] == ':' && 
         udp_payload[2] == '3' )
    {
        //memset(udp_payload, 0, sizeof(udp_payload));

        // #debug
        printk("[g:3] ERROR\n");
        refresh_screen();

        // #test
        // Testing keyboard event
        // OK, it's working. We can see a x in the client area of editor.bin.
        // network_keyboard_event(MSG_KEYDOWN, 'x', 'x' );

        NoReply = TRUE;
        goto done;
    }

// --------------------------
// g:4
// packet type: 4 = disconnect
    if ( udp_payload[0] == 'g' && 
         udp_payload[1] == ':' && 
         udp_payload[2] == '4' )
    {
        //printk("[g:4] \n");
        //refresh_screen;

        memset(udp_payload, 0, sizeof(udp_payload));
        ksprintf(udp_payload,"g:0 ");          // Request
        ksprintf( (udp_payload + 4), "exit");  // exit command
        NoReply = FALSE;
        goto done;
    }

    //if (dport == 11888)
        //ksprintf(udp_payload,"This is a response from Gramado OS");

// Fail: 
// The received message is invalid.
    NoReply = TRUE;

// ---------------------
// Response
done:
    if (NoReply == TRUE)
        return;

    //printk ("kernel: Sending response\n");
    //refresh_screen();

    network_send_udp(  
        dhcp_info.your_ipv4,  // scr ip
        NetworkSaved.caller_ipv4,  // dst ip
        NetworkSaved.caller_mac,   // dst mac
        dport,                // source port: "US"
        sport,                // target port  "Who sent"
        udp_payload,          // udp payload
        MessageSize );        // udp payload size (Message size)
}

void udp_save_mac(uint8_t mac[6])
{
    register int i=0;
    for (i=0; i<6; i++){
        udp_saved_mac[i] = (uint8_t) mac[i];
    };
}

// -----------------

void network_test_udp(void)
{
// Called by the command "test-udp" in console.c.
// Called by the syscall 22003 in sci.c
// The terminal.bin command is "n2".

    char message[512];
    memset(message,0,sizeof(message));
    ksprintf(message,"Hello from Gramado to Linux\n");

    if (networkGetStatus() != TRUE)
       return;

// IN:
// ...
    network_send_udp( 
        dhcp_info.your_ipv4,        //__udp_gramado_default_ipv4,  // scr ip
        __udp_target_gateway_ipv4,  //__udp_target_default_ipv4,   // dst ip
        __udp_target_mac,           // dst mac
        11888,      // source port
        11999,      // dst port
        message,    // msg
         512 );     // msg lenght
}

void 
network_test_udp0(
    uint8_t tmac[6], 
    uint8_t tip[4],
    unsigned short sport,
    unsigned short dport )
{
    char message[512];
    memset(message,0,sizeof(message));
    ksprintf(message,"Hello from Gramado to Linux\n");

    network_send_udp( 
        __udp_gramado_default_ipv4,   // scr ip
        tip,                          // dst ip
        tmac,                         // dst mac
        sport,      // source port
        dport,      // dst port
        message,    // msg
         512 );     // msg lenght
}

void network_test_udp2(void)
{
    network_test_udp0( 
        udp_saved_mac,                  // linux mac
        __udp_target_default_ipv4,  // linux ip
        34885,
        34884 );
}

// -----------------

/*
 * network_send_udp: 
 *    Criando e enviando um pacote ipv4.  (UDP)
 * IN:
 *     source_ip. (It's in the layer 3 of the OSI model. network layer)
 *     target_ip. (It's in the layer 3 of the OSI model. network layer)
 *     target_mac. (It's in the layer 2 of the OSI model. data link layer)
 *     data. (udp payload)
 */
// #todo
// udp needs to have its own document.
// #todo
// Change the return type to 'int'. 
// #obs:
// UDP/IP
// UDP = 0x11 (ip protocol)

// (Ethernet + IP + UDP)
int
network_send_udp ( 
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    uint8_t target_mac[6], 
    unsigned short source_port,
    unsigned short target_port,
    char *data_buffer,   // UDP payload
    size_t data_lenght )
{
// Buffers:
// ethernet, ipv4, udp, data.

    register int i=0;
    int j=0;
    char *data = (char *) data_buffer;  // UDP payload.

//==============================================

// #todo
// NIC Intel device structure.

    if ((void *) currentNIC == NULL){
        printk("network_send_udp: currentNIC\n");
        goto fail;
    }

// Configurando a estrutura do dispositivo.
// 192.168.1.12
// Not used?
    currentNIC->ip_address[0] = source_ip[0];  //192;
    currentNIC->ip_address[1] = source_ip[1];  //168;
    currentNIC->ip_address[2] = source_ip[2];  //1;
    currentNIC->ip_address[3] = source_ip[3];  //12;
    //...

//==============================================
    if ((void*) data == NULL){
        printk ("network_send_udp: Invalid data buffer\n");
        goto fail;
    }


//==============================================
// # ethernet header #

    struct ether_header  Leh;
    /*
    struct ether_header  *eh;
    eh = (void *) kmalloc( sizeof(struct ether_header ) );
    if ((void *) eh == NULL){
        printk("network_send_udp: eh fail\n");
        goto fail;
    }
    */

// Coloca na estrutura do ethernet header os seguintes valores: 
// > endereço mac da origem.
// > endereço mac do destion.
// O endereço mac da origem está na estrutura do controlador nic intel. 
// O endereço mac do destino foi passado via argumento.

    for (i=0; i<6; i++){
        Leh.mac_src[i] = (uint8_t) currentNIC->mac_address[i];  // source 
        Leh.mac_dst[i] = (uint8_t) target_mac[i];               // dest
    };
    Leh.type = (uint16_t) ToNetByteOrder16(ETHERTYPE_IPV4);

//==============================================
// # ipv4 header #

    struct ip_d  Lipv4;
    /*
    struct ip_d  *ipv4;
    ipv4 = (void *) kmalloc( sizeof(struct ip_d) );
    if ((void *) ipv4 == NULL){
        printk("network_send_udp: ipv4 fail\n");
        goto fail;
    }
    */

// IPv4 common header
// See: ip.h

//>>>>
    Lipv4.v_hl = 0x45;    // 8 bit

// Type of service (8bits)
// - Differentiated Services Code Point (6bits)
// - Explicit Congestion Notification (2bits)
    Lipv4.ip_tos = 0x00;  // 8 bit (0=Normal)

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
    Lipv4.ip_len = (uint16_t) ToNetByteOrder16(__iplen);

// Identification
// ... identifying the group of fragments of a single IP datagram. 
// 16 bit
    //uint16_t ipv4count = 1; 
    //ipv4->ip_id = 0x0100;
    //ipv4->ip_id = (uint16_t) ToNetByteOrder16(ipv4count);
    Lipv4.ip_id = 0;  

// Flags (3bits) (Do we have fragments?)
// Fragment offset (13bits) (fragment position)
// Don't fragment for now.
    Lipv4.ip_off = ToNetByteOrder16(0x4000); 

    Lipv4.ip_ttl = 255;  //64; //0x40;  //8bit

// Protocol (8bit)
// 0x11 = UDP. (17)
    Lipv4.ip_p = 0x11;

/*
// src ip
// #bugbug: Esta na ordem certa?
    memcpy ( 
        (void *) &ipv4->ip_src.s_addr, 
        (const void *) &source_ip[0], 
        4 );
    //ipv4->ip_src.s_addr = (unsigned int) ToNetByteOrder32(ipv4->ip_src.s_addr);

// dst ip
// #bugbug: Esta na ordem certa?
    memcpy ( 
        (void *) &ipv4->ip_dst.s_addr, 
        (const void *) &target_ip[0], 
        4 );
    //ipv4->ip_dst.s_addr = (unsigned int) ToNetByteOrder32(ipv4->ip_dst.s_addr);
    //printk ("ip %x\n", ipv4->ip_dst.s_addr);
*/

    //unsigned char *spa = (unsigned char *) &ipv4->ip_src.s_addr;
    //unsigned char *tpa = (unsigned char *) &ipv4->ip_dst.s_addr;
    unsigned char *spa = (unsigned char *) &Lipv4.ip_src.s_addr;
    unsigned char *tpa = (unsigned char *) &Lipv4.ip_dst.s_addr;

    register int it=0;
    for (it=0; it<4; it++)
    {
        spa[it] = (uint8_t) source_ip[it]; 
        tpa[it] = (uint8_t) target_ip[it]; 
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

//==============================================
// # udp header #
    struct udp_d  Ludp;
    /*
    struct udp_d  *udp;
    udp = (void *) kmalloc( sizeof(struct udp_d) );
    if ((void *) udp == NULL){
        printk("network_send_udp: udp fail\n");
        goto fail;
    }
    */

// Ports
    Ludp.uh_sport = (uint16_t) ToNetByteOrder16(source_port);
    Ludp.uh_dport = (uint16_t) ToNetByteOrder16(target_port);

// UDP Length
// (UPD header + payload).
// This field specifies the length in bytes of the UDP header and UDP data. 
// The minimum length is 8 bytes, the length of the header. 
    uint16_t __udpheaderlen = UDP_HEADER_LENGHT;
    uint16_t __udppayloadlen = (uint16_t) (data_lenght & 0xFFFF);
    uint16_t __udplen = (uint16_t) (__udpheaderlen + __udppayloadlen); 
    Ludp.uh_ulen = (uint16_t) ToNetByteOrder16(__udplen);

// Checksum
// #remember:
// When UDP runs over IPv4, 
// the checksum is computed using a "pseudo header" 
// that contains some of the same information from the real IPv4 header.
// # UDP lets us set the checksum to 0 to ignore it?
    Ludp.uh_sum = 0;  //#todo

    //printk ("size %d\n", sizeof (struct  udp_d) );
    //refresh_screen();
    //while(1){}

// ----------------------------------------------------

//
// Buffer
//

// Let's call it 'frame'.
// Because we're sending a 'frame'.

// ??
// Quem?
// Estamos pegando o offset que nos levara ao endereço do buffer.
// Usaremos esse offset logo abaixo.
// Pegamos esse offset na estrutura do controlador nic intel.
// Copiando o pacote no buffer.
// Pegando o endereco virtual do buffer na estrutura do controlador 
// nic intel. Para isso usamos o offset obtido logo acima.

    uint16_t buffer_index = (uint16_t) currentNIC->tx_cur;

// Get the buffer address based on its offset.
    unsigned char *frame = 
        (unsigned char *) currentNIC->tx_buffers_virt[buffer_index];

    //#debug
    //printk ("buffer_index {%d}\n",buffer_index);

// #importante:
// Preparando ponteiros para manipularmos as 
// estruturas usadas no pacote.

    unsigned char *src_ethernet = (unsigned char *) &Leh;    //eh; 
    unsigned char *src_ipv4     = (unsigned char *) &Lipv4;  //ipv4; 
    unsigned char *src_udp      = (unsigned char *) &Ludp;   //udp; 

//
// Copy
//

    if ((void*) frame == NULL)
        panic("network_send_udp: frame\n");

// Copiando as estruturas para o buffer.
// >Step1) Copiando o header ethernet.
// >Step2) Copiando o heder ipv4.
// >Step3) Copiando o header udp.
// >Step4) Copiando os dados.

//Step1
//copia o header ethernet
    int eth_offset=0;
    for ( j=0; j<ETHERNET_HEADER_LENGHT; j++ )
    {
        frame[eth_offset +j] = src_ethernet[j];
    };

//Step2
//copia o ipv4
    int ipv4_offset = ETHERNET_HEADER_LENGHT;
    for ( j=0; j<IP_HEADER_LENGHT; j++ )
    {
        frame[ipv4_offset +j] = src_ipv4[j];
    };

//Step3
//copia o udp
    int udp_offset = ETHERNET_HEADER_LENGHT + IP_HEADER_LENGHT;
    for ( j=0; j<UDP_HEADER_LENGHT; j++ ){
        frame[udp_offset +j] = src_udp[j];
    };

//Step4
//copia o payload.
    int data_offset = 
            ( ETHERNET_HEADER_LENGHT +
              IP_HEADER_LENGHT +
              UDP_HEADER_LENGHT );
    for ( j=0; j<data_lenght; j++ )
    {
        frame[data_offset +j] = data[j];
    };

// ---------------------------------------
// send
// lenght:
// Vamos configurar na estrutura do nic intel o tamanho do pacote.
// Lenght de um pacote ipv4.
// ethernet header, ipv4 header, udp header, data.
// 14 + 20 + 6 + 512 = 552.
    size_t FRAME_SIZE = 
               ( ETHERNET_HEADER_LENGHT +\
                 IP_HEADER_LENGHT +\
                 UDP_HEADER_LENGHT +\
                 data_lenght );


//
// Check
//

    // #todo
    //if ((void*) frame == NULL)
    //    goto fail;
    //if (FRAME_SIZE == 0)
    //    goto fail;

//
// Send
//

// #bugbug
// I guess we don't need the routine above.
// It's because ethernet_send() will put the given data into 
// the right place.

// Send frame via NIC.
    ethernet_send( currentNIC, FRAME_SIZE, frame );

// #debug
// Send frame to myself.
    //network_on_receiving(frame,FRAME_SIZE);
    //refresh_screen();
    //while(1){}

// #test
    //kfree(eh);
    //kfree(udp);
    //kfree(udp);

    printk("Done\n");
    return 0;
fail:
    printk ("Fail\n");
    return -1;
}

//
// $
// HANDLER
//

// When receving UDP packet from NIC device.
// IN:
// buffer = udp header base address.
// size     = udp packet size. (header + data)
void 
network_handle_udp( 
    const unsigned char *buffer, 
    ssize_t size )
{
    struct udp_d *udp;
    register int i=0;

    //printk ("UDP: Received\n");


// Parameters:
    if ((void*) buffer == NULL){
        printk("network_handle_udp: buffer\n");
        return;
    }
    if (size < 0){
        //
    }
// The minimum size.
// Only the udp header.
    //if (size < UDP_HEADER_LENGHT){
    //    printk("network_handle_udp: size\n");
    //    return;
    //}

// #warning
// It's ok to use pointer here.
// We're not allocating memory, we're using 
// a pre-allocated buffer.
    udp = (struct udp_d *) buffer;

    uint16_t sport = (uint16_t) FromNetByteOrder16(udp->uh_sport);
    uint16_t dport = (uint16_t) FromNetByteOrder16(udp->uh_dport);

/*
    //#debug
    printk ("sp ={%d}\n",sport);
    printk ("dp ={%d}\n",dport);
    printk ("len={%d}\n",udp->uh_ulen);
    printk ("sum={%d}\n",udp->uh_sum);
*/

// Clean the payload local buffer.
    //memset( udp_payload, 0, sizeof(udp_payload) );
    __udp_clear_payload_buffer();

//
// Create a local copy of the payload.
//

    char *p2; 
    p2 = (buffer + UDP_HEADER_LENGHT);
    size_t p_size = strlen(p2);
    if (p_size <= 512)
    {
        strncpy(
            udp_payload,
            p2,
            p_size );
        udp_payload[p_size] = 0;
        udp_payload[p_size +1] = 0;
    }
    /*
    strncpy(
        udp_payload,
        (buffer + UDP_HEADER_LENGHT),
        1020 );
    */
    udp_payload[1021] = 0;

    //#debug
    // A lot of noise.
    //printk("UDP: dport{%d} #debug\n",dport);

// Don't print every message.
// Is it a valid port?
// Hang if the port is valid.

    char *p;
    p = udp_payload;
    int mFlag=0;

// #test
// DHCP dialog
// Receiving Offer and Ack.
// Handle dhcp protocol.

    //printk("UDP: dport{%d}   #debug\n",dport);
    //die();

    // DHCP ports
    // UDP Port 67 – Used by the DHCP server to listen for incoming requests from clients.
    // UDP Port 68 – Used by the client to receive responses from the server.
    //if (sport == 67 || dport == 68)
    if (dport == 68)
    {
        printk("UDP: on DHCP port %d\n",dport);
        network_handle_dhcp(
            (buffer + UDP_HEADER_LENGHT),
            (udp->uh_ulen - UDP_HEADER_LENGHT) );

        //__udp_clear_payload_buffer();
        return;
    }

//
// Ports
//

// --------------
// #test
// Copy into a list of buffers.
// The application is gonna get this.
// #bugbug: 
// What kind of data we're pushing into these buffers?
// Is it the raw frame only? Or only UDP?
// see: network.c
    //int PushIntoTheQueue = TRUE;
    int PushIntoTheQueue = FALSE;

    int NoReply = TRUE;

    if ( dport == 11888 ||
         dport == 22888 ||
         dport == 34884 )
    {

        // #bugbug
        // This is just a test. Actually we're gonna put
        // into the queue all kind of frames, not just
        // the udp's payloads.
        if (PushIntoTheQueue == TRUE){
            // #test
            network_push_packet( udp_payload, 512 );
        } else {

            // Print the message for these ports.
            printk("UDP: { %s }\n", udp_payload );

            // #test
            // System message
            // Send a command to the init process.
            //ipc_post_message_to_init(77888, 1234, 5678);

            //__handle_gprotocol(sport,dport);
            gprot_handle_protocol(udp_payload,sport,dport);
        };

        // Clear UDP local buffer.
        //memset(udp_payload, 0, sizeof(udp_payload));
        //for (i=0; i<1024; i++){
        //    udp_payload[i]=0;
        //};

        __udp_clear_payload_buffer();
        return;
    }

// Fail
// Return if something goes wrong.
    return;
}

