
// dhcp.c
// dhcp initialization.
// 2023 - Created by Fred Nora.
// Credits: Nelson Cole. (Sirius OS)

// D.O.R.A.
// Discover, Offer, Request, Ack

#include <kernel.h>

// dhcp info.
struct dhcp_info_d  dhcp_info;


// source ip
unsigned char __dhcp_source_ipv4[4] = { 
    0, 0, 0, 0 
};
// source ip
unsigned char __dhcp_source_ipv4_2[4] = { 
    192, 168, 1, 12 
};

// source ip
unsigned char dhcp_saved_server_id[4] = { 
    192, 168, 1, 1 
};


// Target IP. (broadcast)
unsigned char __dhcp_target_ipv4[4]  = { 
    0xFF, 0xFF, 0xFF, 0xFF  
};
// Target MAC.
unsigned char __dhcp_target_mac[6] = { 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
};

//---------------

void network_show_dhcp_info(void)
{
    if (dhcp_info.initialized != TRUE){
        printk("DHCP: Not initialized\n");
        return;
    }
    printk ("DHCP: Your IP is %d.%d.%d.%d\n",
        dhcp_info.your_ipv4[0], 
        dhcp_info.your_ipv4[1], 
        dhcp_info.your_ipv4[2], 
        dhcp_info.your_ipv4[3] );
    printk ("DHCP: Server IP is %d.%d.%d.%d\n",
        dhcp_info.server_ipv4[0], 
        dhcp_info.server_ipv4[1], 
        dhcp_info.server_ipv4[2], 
        dhcp_info.server_ipv4[3] );
    //...
}

// Called by some handler to 
// save the dhcp server ip in an array here in this document.
void network_save_dhcp_server_id( uint8_t ip[4] )
{
    register int i=0;
    for (i=0; i<4; i++){
        dhcp_saved_server_id[i] = (uint8_t) ip[i];
    };
}

// -----------------------
// Send dhcp.
void 
network_dhcp_send(
    struct dhcp_d *dhcp,
    int message_type, 
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    unsigned short sport, 
    unsigned short dport )
{
    char *p; // For hostname support.
    int opt_size = 0;

// Parameters
    if ((void*) dhcp == NULL){
        printk("network_dhcp_send: dhcp\n");
        goto fail;
    }
    // DORA
    if (message_type < 0){
        printk("network_dhcp_send: message_type\n");
        goto fail;       
    }

    dhcp->op    = 1;  // 1=REQUEST | 2=REPLY
    dhcp->htype = 1;  // 1 = Ethernet
    dhcp->hlen  = 6;  // MAC lenght
    dhcp->hops  = 0;  // ?

// ??
// The Transaction ID. 
// It's a random number chosen by the client 
// to identify an IP address allocation.
    dhcp->xid   = ToNetByteOrder32(0x3903F326);

// Elapsed time in seconds.
    dhcp->secs  = ToNetByteOrder16(0);

// flags: 
// The leftmost bit is defined as the BROADCAST (B) flag. 
// If this flag is set to 0, the DHCP server sent a reply back by unicast; 
// if this flag is set to 1, the DHCP server sent a reply back by broadcast. 
// The remaining bits of the flags field are reserved for future use.
    dhcp->flags = ToNetByteOrder16(0x0000);
    //dhcp->flags = ToNetByteOrder16(0x8000);

// ciaddr: Client IP address.
    dhcp->ciaddr = (unsigned int) 0;
// yiaddr: 'your' (client) IP address, assigned by the server.
    dhcp->yiaddr = (unsigned int) 0;
// siaddr: 
// Next server IP.
// Server IP address, from which the client obtained 
// configuration parameters.
    dhcp->siaddr = (unsigned int) 0;
// giaddr: 
// Relay agent IP.
// IP address of the first relay agent a request message traveled.
// Relay agent is used when the dhcp is outside the local network.
    dhcp->giaddr = (unsigned int) 0;

// Client MAC:
// Let's fill it. Getting the number in the structure.
    memset( dhcp->chaddr, 0, 16 );
    register int i=0;
    if ((void*) currentNIC == NULL){
        printk("network_dhcp_send: currentNIC\n");
        goto fail;
    }
    if ((void*) currentNIC != NULL)
    {
        for (i=0; i<6; i++){
            dhcp->chaddr[i] = (uint8_t) currentNIC->mac_address[i];
        };
    }

// The server host name, 
// from which the client obtained configuration parameters.
    memset(dhcp->sname, 0, 64);

// Bootfile name and path information, 
// defined by the server to the client.
    memset(dhcp->file, 0, 128);

// DHCP magic cookie: (99, 130, 83, 99)
    dhcp->magic_cookie = (unsigned int) ToNetByteOrder32(0x63825363);

//
// Options
//

//-----------------------------------
// DORA_D
// 53 Message type:
//   + Discover
// 55 Parameter list:
//   + 1   - Subnet mask
//   + 3   - Router
//   + 6   - DNS servers

//-----------------------------------
// DORA_R
// 53 Message type:
//     + Request
// 50 - Requested IP address:
//     + 4 bytes
// 54 - dhcp server id:
//     + 4 bytes
// 55 Parameter list:
//   + 1   - Subnet mask
//   + 3   - Router
//   + 6   - DNS servers


// 53 = DHCP Message type.
    dhcp->options[0] = OPT_DHCP_MESSAGE_TYPE;
    dhcp->options[1] = 0x01;  // lenght
    dhcp->options[2] = (uint8_t) message_type;  // Discover or Request.

// In DORA the client only have two kinds of send: (D|R)
    if ( message_type != DORA_D && message_type != DORA_R )
    {
        printk("Invalid DORA message type\n");
        goto fail;
    }

// Message type: Only (D|R)
    switch (message_type){

    // Discovering and IP.
    case DORA_D:
        printk("DHCP: DORA-D\n");

        //++
        // Parameter Request list
        dhcp->options[3] = OPT_PARAMETER_REQUEST;
        dhcp->options[4] = (uint8_t) 3;  // Lenght
        dhcp->options[5] = OPT_SUBNET_MASK;
        dhcp->options[6] = OPT_ROUTER;
        dhcp->options[7] = OPT_DNS;
        //--

        // Option End
        dhcp->options[8] = OPT_END;
        opt_size = 9;
        break;

    // Requesting an IP.
    case DORA_R:
        printk("DHCP: DORA-R\n");

        //++
        // Requested IP address
        dhcp->options[3] = OPT_REQUESTED_IP_ADDR;
        dhcp->options[4] = (uint8_t) IPV4_IN_BYTES;  // Lenght
        dhcp->options[5] = (uint8_t) source_ip[0];
        dhcp->options[6] = (uint8_t) source_ip[1];
        dhcp->options[7] = (uint8_t) source_ip[2];
        dhcp->options[8] = (uint8_t) source_ip[3];
        //--

        //++
        // Server Identifier
        dhcp->options[9] = OPT_SERVER_ID;
        dhcp->options[10] = (uint8_t)IPV4_IN_BYTES;  // Lenght
        dhcp->options[11] = (uint8_t) target_ip[0];
        dhcp->options[12] = (uint8_t) target_ip[1];
        dhcp->options[13] = (uint8_t) target_ip[2];
        dhcp->options[14] = (uint8_t) target_ip[3];
        //--

        //++
        // Parameter Request list 
        dhcp->options[15] = OPT_PARAMETER_REQUEST;
        dhcp->options[16] = (uint8_t) 3;  // Lenght
        dhcp->options[17] = OPT_SUBNET_MASK;
        dhcp->options[18] = OPT_ROUTER;
        dhcp->options[19] = OPT_DNS;
        //--
        
        //++
        //#test: Host name
        dhcp->options[20] = 12;
        dhcp->options[21] = 7;   // size in bytes

        // This is the host name.
        //dhcp->options[22] = 'G';
        //dhcp->options[23] = 'R';
        //dhcp->options[24] = 'A';
        //dhcp->options[25] = 'M';
        //dhcp->options[26] = 'A';
        //dhcp->options[27] = 'D';
        //dhcp->options[28] = '0';
        //p = UTS_NODENAME;
        p = &kernel_utsname.nodename;
        dhcp->options[22] = p[0];
        dhcp->options[23] = p[1];
        dhcp->options[24] = p[2];
        dhcp->options[25] = p[3];
        dhcp->options[26] = p[4];
        dhcp->options[27] = p[5];
        dhcp->options[28] = p[6];
        //--

        // Option End
        dhcp->options[29]= OPT_END;
        opt_size = 30;
        break;

    default:
        printk("network_dhcp_send: default message_type\n");
        goto fail;
        break;   
    };

// Sending UDP packet.
// UDP payload is the base of the dhcp structure.

    char *__udp_payload = (char *) dhcp;
    size_t __udp_payload_size = (size_t) ( sizeof(struct dhcp_d) );

    printk("network_dhcp_send: Send UDP\n");

// IN:
// src ip, 
// target ip,
// target mac,
// src port, 
// destination port,
// payload buffer, 
// payload size.

    network_send_udp( 
        __dhcp_source_ipv4, 
        __dhcp_target_ipv4,
        __dhcp_target_mac,
        sport, 
        dport,
        __udp_payload, 
        __udp_payload_size ); 

// done:
    printk("network_dhcp_send: Done\n");
    return;

fail:
    //refresh_screen();
    return;
}

//
// $
// INITIALIZATION
//

// Initialize the dhcp_info structure,
// by calling the dialog with the dhcp server via dhcp/udp/ip.
int network_initialize_dhcp(void)
{
// Called by the command "dhcp-test" in console.c.
// Called by the syscall 22003 in sci.c
// The terminal.bin command is "n3".

// Local dhcp structure.
    struct dhcp_d  Ldhcp;

    dhcp_info.initialized = FALSE;

// Can't initialize the dhcp because 
// the network support was not initialized yet.
    if (networkGetStatus() != TRUE){
       printk("network_initialize_dhcp: Network not initialized\n");
       goto fail;
    }

// #hackhack
// No NIC device driver.

    if ((void*) currentNIC == NULL){
        printk("network_initialize_dhcp: currentNIC\n");
        goto fail;
    }
    if (currentNIC->magic != 1234){
        printk("network_initialize_dhcp: currentNIC\n");
        goto fail;
    }
    if (currentNIC->initialized != TRUE){
        printk("network_initialize_dhcp: currentNIC not initialized\n");
        goto fail;
    }

//
// Send 'D'ORA, Discover.
//

    // #debug
    // printk("network_initialize_dhcp: Sending Discover\n");

    network_dhcp_send( 
        (struct dhcp_d *) &Ldhcp,    // dhcp header 
        DORA_D,                      // Message code.
        __dhcp_source_ipv4,          // 255.255.255.255
        __dhcp_target_ipv4,          // FF.FF.FF.FF.FF.FF
        68,                          // src port
        67 );                        // dst port

    printk("network_initialize_dhcp: Done\n");
    //dhcp_info.initialized =  TRUE;

    //while(1){}
    return 0;

fail:
    printk("network_initialize_dhcp: fail\n");
    refresh_screen();

    dhcp_info.initialized =  FALSE;
    return (int) -1;
}

//
// $
// HANDLER
//

// When receving DHCP packet from NIC device.
void 
network_handle_dhcp( 
    const unsigned char *buffer, 
    ssize_t size )
{
    struct dhcp_d *dhcp;
    uint8_t your_ipv4[4];    // yiaddr: Your IP address.
    uint8_t server_ipv4[4];  // siaddr: Server IP address.

    //printk ("DHCP: Received\n");

// #warning
// It's ok to use pointer here.
// We're not allocating memory, we're using 
// a pre-allocated buffer.
    dhcp = (struct dhcp_d *) buffer;
    if ((void*) dhcp == NULL){
        return;
    }

// Minimum size
    //if (size < ? )
        //return;

// yiaddr: Your IP address.
    your_ipv4[0] = (uint8_t) (  dhcp->yiaddr        & 0xFF);
    your_ipv4[1] = (uint8_t) ( (dhcp->yiaddr >> 8)  & 0xFF);
    your_ipv4[2] = (uint8_t) ( (dhcp->yiaddr >> 16) & 0xFF);
    your_ipv4[3] = (uint8_t) ( (dhcp->yiaddr >> 24) & 0xFF);
    printk ("network_handle_dhcp: Your IP %d.%d.%d.%d  <<< ---------\n",
        your_ipv4[0], your_ipv4[1], your_ipv4[2], your_ipv4[3] );

// siaddr: Server IP address.
    server_ipv4[0] = (uint8_t) (  dhcp->siaddr        & 0xFF);
    server_ipv4[1] = (uint8_t) ( (dhcp->siaddr >> 8)  & 0xFF);
    server_ipv4[2] = (uint8_t) ( (dhcp->siaddr >> 16) & 0xFF);
    server_ipv4[3] = (uint8_t) ( (dhcp->siaddr >> 24) & 0xFF);
    printk ("network_handle_dhcp: Server IP %d.%d.%d.%d  <<< ---------\n",
        server_ipv4[0], server_ipv4[1], server_ipv4[2], server_ipv4[3] );

// chaddr: Client hardware address.
// Pra certificar que a mensagem foi pra nós.
    printk("network_handle_dhcp: Client MAC %x.%x.%x.%x.%x.%x\n",
        dhcp->chaddr[0],
        dhcp->chaddr[1],
        dhcp->chaddr[2],
        dhcp->chaddr[3],
        dhcp->chaddr[4],
        dhcp->chaddr[5] );

// Operation
// Is it a Reply?

    //if (dhcp->op == 1)
        //return;
    if (dhcp->op == 2){
        printk ("DHCP: Reply received\n");
    }

// 53 = DHCP Message type.

    //if (dhcp->options[0] != OPT_DHCP_MESSAGE_TYPE)
        //return;
    //dhcp->options[1] = 0x01;  // lenght
    //dhcp->options[2] = (uint8_t) message_type;  // Discover or Request.

// As a client, we send Discover and Request,
// and only receive Offer, Acknowledge or Decline.

// ---------------------------
// Offer: d'O'ra 
// + We received an Offer from the server.
// + Let's send a Request.
    if (dhcp->options[2] == DORA_O)
    {
        printk("DHCP: DORA-O\n");

        // #debug
        //printk ("When receiving Offer: Your IP %d.%d.%d.%d\n",
        //    your_ipv4[0], your_ipv4[1], your_ipv4[2], your_ipv4[3] );
        //while (1){}

        // Sending REQUEST - do'R'a.
        // Request: do'R'a
        network_dhcp_send( 
            dhcp,
            DORA_R,
            your_ipv4, 
            server_ipv4,
            68, 
            67 );

        return;
    }

// ---------------------------
// Ack
// We received an ACK from the server.
// Let's do:
// + Save some information.
// + Set the online status for the network.
// + Print some information about dhcp, for debug.
// ----------
// Save your IP.
// Save the server IP.
// Se the initialization flag.
// Set the online status.
    if ( dhcp->options[2] == DORA_A )
    {
        printk("DHCP: DORA-A\n");

        // #debug
        //printk ("When receiving ack: Your IP %d.%d.%d.%d\n",
        //    your_ipv4[0], your_ipv4[1], your_ipv4[2], your_ipv4[3] );
        //while (1){}

        // #debug
        //printk ("When receiving ack: Server IP %d.%d.%d.%d\n",
        //    server_ipv4[0], server_ipv4[1], server_ipv4[2], server_ipv4[3] );
        //while (1){}

        // Save info
        network_fill_ipv4( dhcp_info.your_ipv4, your_ipv4 );
        network_fill_ipv4( dhcp_info.server_ipv4, server_ipv4 );
        dhcp_info.initialized = TRUE;
        // Set online status 
        networkSetOnlineStatus(ONLINE);
        // Show info.
        network_show_dhcp_info();
        // #debug
        //die();
        return;
    }

// ---------------------------
// Decline
// The server declined our request.
    if ( dhcp->options[2] == DHCP_DECLINE ){
        printk ("DHCP: Decline received\n");
        return;
    }

// Unknown option
    //printf("DHCP: Unknown option\n")
}



