// arp.c
// Created by Fred Nora.

#include <kernel.h>

// see: arp.h
struct arp_table_d  ARP_Table;

// --------

unsigned char __arp_broadcast_mac[6] = { 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
};
unsigned char __arp_target_mac[6] = { 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
};
unsigned char __arp_gramado_default_ipv4[4] = { 
    0, 0, 0, 0 
};
unsigned char __arp_target_default_ipv4[4]  = { 
    192, 168, 1, 1 
};
unsigned char __arp_gateway_default_ipv4[4] = { 
    192, 168, 1, 1 
};

// =====================================

void arp_initialize_arp_table(void)
{
    register int i=0;
    for (i=0; i<ARP_TABLE_COUNT_MAX; i++)
    {
        ARP_Table.arpTable[i].id = (int) i;
        memset( ARP_Table.arpTable[i].ipv4_address, 0, 4 );
        memset( ARP_Table.arpTable[i].mac_address, 0, 6 );
        ARP_Table.arpTable[i].used = TRUE;
        ARP_Table.arpTable[i].magic = 1234;
    };
    ARP_Table.initialized = TRUE;
}

void 
arp_table_set_entry(
    int index,
    uint8_t target_ip[4],
    uint8_t target_mac[6] )
{
    if (ARP_Table.initialized != TRUE)
        return;
    if (index<0)
        return;
    if (index >= ARP_TABLE_COUNT_MAX)
        return;

// Set

    ARP_Table.arpTable[index].ipv4_address[0] = target_ip[0];
    ARP_Table.arpTable[index].ipv4_address[1] = target_ip[1];
    ARP_Table.arpTable[index].ipv4_address[2] = target_ip[2];
    ARP_Table.arpTable[index].ipv4_address[3] = target_ip[3];

    ARP_Table.arpTable[index].mac_address[0] = target_mac[0];
    ARP_Table.arpTable[index].mac_address[1] = target_mac[1];
    ARP_Table.arpTable[index].mac_address[2] = target_mac[2];
    ARP_Table.arpTable[index].mac_address[3] = target_mac[3];
    ARP_Table.arpTable[index].mac_address[4] = target_mac[4];
    ARP_Table.arpTable[index].mac_address[5] = target_mac[5];
}

// #debug
void arp_table_show_index(int index)
{
    if (ARP_Table.initialized != TRUE)
        return;
    if (index<0)
        return;
    if (index >= ARP_TABLE_COUNT_MAX)
        return;

    uint8_t a = ARP_Table.arpTable[index].ipv4_address[0];
    uint8_t b = ARP_Table.arpTable[index].ipv4_address[1]; 
    uint8_t c = ARP_Table.arpTable[index].ipv4_address[2];
    uint8_t d = ARP_Table.arpTable[index].ipv4_address[3];
    printk("IP: %c.%c.%c.%c | ",
        a,b,c,d );

    uint8_t a2 = ARP_Table.arpTable[index].mac_address[0];
    uint8_t b2 = ARP_Table.arpTable[index].mac_address[1]; 
    uint8_t c2 = ARP_Table.arpTable[index].mac_address[2];
    uint8_t d2 = ARP_Table.arpTable[index].mac_address[3];
    uint8_t e2 = ARP_Table.arpTable[index].mac_address[4];
    uint8_t f2 = ARP_Table.arpTable[index].mac_address[5];
    printk("MAC: %c.%c.%c.%c.%c.%c ",
        a2,b2,c2,d2,e2,f2 );

    printk("INDEX: %d\n",index);
}

void arp_show_table(void)
{
    register int i=0;

    if (ARP_Table.initialized != TRUE)
        return;

    printk("ARP table:\n");
    printk("-------------------------------------\n");
    for (i=0; i<ARP_TABLE_COUNT_MAX; i++)
    {
        arp_table_show_index(i);
    };
    printk("-------------------------------------\n");
}

// Ethernet + arp
void 
network_send_arp( 
    uint8_t target_mac[6],
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    int op )
{
// Send ARP.

    register int i=0;

    // #debug
    //printk("network_send_arp:\n");
    //refresh_screen();

// The structure for the Intel NIC device.
    if ((void*) currentNIC == NULL){
        printk("network_send_arp: currentNIC fail\n");
        goto fail;
    }

// A given IP number for Gramado.
// Saving it into the NIC structure.
// 192.168.1.12
    currentNIC->ip_address[0] = source_ip[0];  //192;
    currentNIC->ip_address[1] = source_ip[1];  //168;
    currentNIC->ip_address[2] = source_ip[2];  //1;
    currentNIC->ip_address[3] = source_ip[3];  //12;

//==============================================
// # ethernet header #
    struct ether_header  Leh;
    /*
    struct ether_header *eh;
    eh = (void *) kmalloc( sizeof(struct ether_header) );
    if ( (void *) eh == NULL){
        printk ("network_send_arp: eh struct fail\n");
        goto fail;
    }
    */

// MAC
// Save the source and the destination mac into the ethernet header.
// The destination mac came via argument.
    for ( i=0; i<ETH_ALEN; i++){
        Leh.mac_src[i] = (uint8_t) currentNIC->mac_address[i];
        Leh.mac_dst[i] = (uint8_t) target_mac[i];
    };
// TYPE
    Leh.type = (uint16_t) ToNetByteOrder16(ETH_TYPE_ARP);

//==============================================
// # arp header #
    struct ether_arp  Larp;
    /*
    struct ether_arp *h;
    h = (void *) kmalloc ( sizeof(struct  ether_arp) );
    if ((void *) h == NULL){
        printk ("network_send_arp: struct h fail");
        goto fail;
    }
    */

//
// Header
// see: arp.h
//

// Hardware type (HTYPE)   (00 01) = Ethernet.
    Larp.type = (uint16_t) 0x0100;
// Protocol type (PTYPE)   (08 00) = ipv4.
// In the case of Ethernet, a 0x0806 EtherType value 
// is used to identify ARP frames.
    Larp.proto = (uint16_t) 0x0008;
// Hardware address length (MAC)
    Larp.hlen = (uint8_t) ETH_ALEN;
// Protocol address length (IP)
    Larp.plen = (uint8_t) 4;
// Operation (OPER)
// We only have two valid operation codes.
    if (op != ARP_OPC_REQUEST && op != ARP_OPC_REPLY){
        panic("network_send_arp: Invalid operation code\n");
    }
    Larp.op = (uint16_t) ToNetByteOrder16(op);

//
// Addresses
//

// MAC addresses
// Hardware address
    for ( i=0; i<ETH_ALEN; i++ ){
        Larp.arp_sha[i] = (uint8_t) currentNIC->mac_address[i];  //my MAC
        Larp.arp_tha[i] = (uint8_t) target_mac[i]; 
    };
// IP addresses
// Protocol address
    for ( i=0; i<4; i++ ){
        Larp.arp_spa[i] = (uint8_t) source_ip[i]; 
        Larp.arp_tpa[i] = (uint8_t) target_ip[i]; 
    };

//==================================
// #debug
// show arp header.

/*
    printk("\n\n");
    printk("[arp]\n\n");
    printk("type={%x} proto={%x} hlen={%d} plen={%d} op={%x} \n", 
        h->type ,h->proto ,h->hlen ,h->plen ,h->op);
    printk("\n sender: mac ");
    for( i=0; i<6; i++){ printk("%x ",h->arp_sha[i]); }
    printk("\n sender: ip ");
    for( i=0; i<4; i++){ printk("%d ",h->arp_spa[i]); }
    printk("\n target: mac ");
    for( i=0; i<6; i++){ printk("%x ",h->arp_tha[i]); }
    printk("\n target: ip ");
    for( i=0; i<4; i++){ printk("%d ",h->arp_tpa[i]); }
    refresh_screen();
    while(1){}
*/
//==================================

//
// Buffer
//

// Set up the pointers for the copy.

// Offset do endereço do buffer.
// Estamos pegando o offset que nos leva ao endereço do buffer.
// Pegamos esse offset na estrutura do controlador nic intel.
// see: nicintel.h

// pega o tail antigo.
    //unsigned long address = (currentNIC->registers_base_address + 0x3810); //head
    //unsigned long address = (currentNIC->registers_base_address + REG_TDT); // tail
    //unsigned int value32 = (uint16_t) *( (volatile unsigned int *) address );
    //currentNIC->tx_cur = (uint16_t) (value32 & 0xFFFF);
    uint16_t buffer_index = (uint16_t) currentNIC->tx_cur;

    //#debug
    //printk ("buffer_index {%d}\n",buffer_index);

//
// Buffer
//

// Let's call it 'frame'.
// Because we're sending a frame.

// Get the buffer address based on its offset.
    unsigned char *frame = 
        (unsigned char *) currentNIC->tx_buffers_virt[buffer_index];

// Get the addresses for the headers.
    unsigned char *src_ethernet = (unsigned char *) &Leh;   //eh;
    unsigned char *src_arp      = (unsigned char *) &Larp;  //h;

//
// Copy
//

    if ((void*) frame == NULL)
        panic("network_send_arp: frame\n");

// Copy the ethernet header into the buffer.
// 14 bytes.
    for (i=0; i<ETHERNET_HEADER_LENGHT; i++){
        frame[i] = (unsigned char) src_ethernet[i];
    };
// Copy the arp header into the buffer.
// 28 bytes
// It starts right after the ethernet header.
    for (i=0; i<ARP_HEADER_LENGHT; i++){
        frame[ETHERNET_HEADER_LENGHT + i] = (unsigned char) src_arp[i]; 
    };

// ======================
// Sending via e1000 api.
    // 14 + 28 = 42
    size_t ARP_TOTAL_SIZE = 
        ( ETHERNET_HEADER_LENGHT + \
          ARP_HEADER_LENGHT );
// Sending a frame!
    //e1000_send( currentNIC, ARP_TOTAL_SIZE, frame );
    // #test
    // Creating a sender for ethernet packet.
    ethernet_send( currentNIC, ARP_TOTAL_SIZE, frame );

// #test
    //kfree(eh);
    //kfree(h);

    //printk ("Done\n");
    return;
fail:
    return;
}

// Request to gateway
void network_send_arp_request(void)
{
// Send ARP request to 192.168.1.6.
    //if (networkGetStatus() != TRUE)
    //   return;
    // IN: src ipv4, dst ipv4, dst mac.
    network_arp_request_to( 
        __arp_gramado_default_ipv4,
        __arp_target_default_ipv4 );
}

// Request to gateway
void network_send_arp_request2(void)
{
// Send ARP request to 192.168.1.1.
    //if (networkGetStatus() != TRUE)
    //   return;
    // IN: src ipv4, dst ipv4, dst mac.
    network_arp_request_to( 
        __arp_gramado_default_ipv4,
        __arp_gateway_default_ipv4 );
}

// Send ARP request to a given ipv4.
void 
network_arp_request_to(
    uint8_t src_ipv4[4],
    uint8_t dst_ipv4[4] )
{
    if (networkGetStatus() != TRUE)
       return;
    network_send_arp( 
        __arp_broadcast_mac,   // target mac (broadcast)
        src_ipv4,  // src ip 
        dst_ipv4,  // dst ip 
        ARP_OPC_REQUEST );
}

void 
network_send_arp_reply(
    uint8_t target_mac[6],
    uint8_t target_ip[4] )
{
// #warning
// We really need to do the dhcp dialog before.
    if (dhcp_info.initialized != TRUE)
        return;

    network_send_arp( 
        target_mac,           // target mac
        dhcp_info.your_ipv4,  // src ip 
        target_ip,            // dst ip (Linux)
        ARP_OPC_REPLY );
}


//
// $
// HANDLER
//

// When receving ARP packet from NIC device.
// handle arp package
// IN:
// buffer = The address after the ethernet header.
void 
network_handle_arp( 
    const unsigned char *buffer, 
    ssize_t size )
{
// Called by network_on_receiving() in network.c.
// 0x0806

    struct ether_arp *ar;


// #warning
// It's ok to use pointer here.
// We're not allocating memory, we're using 
// a pre-allocated buffer.
    ar = (struct ether_arp *) buffer;
    if ((void*) ar == NULL){
        printk("network_handle_arp: ar\n");
        goto fail;
    }

// The minimum size.
    //if (size < ARP_HEADER_LENGHT){
    //    printk("network_handle_arp: size\n");
    //    goto fail;
    //}

/*
// Show data.
// Bytes: Net-style.
// Hardware type (HTYPE)   (00 01) = Ethernet.
    printk("Hardware type: {%x}\n",ar->type);
// Protocol type (PTYPE)   (08 00) = ipv4.
// In the case of Ethernet, a 0x0806 EtherType value 
// is used to identify ARP frames.
    printk("Protocol type: {%x}\n",ar->proto);
    printk("Hardware address lenght: {%x}\n",ar->hlen);
    printk("Protocol address lenght: {%x}\n",ar->plen);
    printk("ARP operation {%x}\n",ar->op);
    // continua: macs e ips ... 
*/

// Operation
    uint16_t op = (uint16_t) FromNetByteOrder16(ar->op);

// ------------
// REQUEST
// Sending a reply.
// sha is the [mac] of the machine that sent us a request.
// spa is the [ip]  of the machine that sent us a request.
    if (op == ARP_OPC_REQUEST){
        //printk("ARP: REQUEST\n");
        network_send_arp_reply( 
            ar->arp_sha,     // Target mac
            ar->arp_spa );   // Target ip
        return;

// ------------
// REPLY
    } else if (op == ARP_OPC_REPLY){

        // #todo: This is a work in progress.
        // #test
        // #bugbug: Setting only the first index.
        //arp_table_set_entry( 
        //   0,
        //   ar->arp_spa,
        //   ar->arp_sha );

        // #debug
        printk("ARP: REPLY from %d.%d.%d.%d\n",
            ar->arp_spa[0], 
            ar->arp_spa[1], 
            ar->arp_spa[2], 
            ar->arp_spa[3] );

        // From gateway?
        // #todo:
        // We got to use the ipv4 given by the dhcp dialog.
        if ( ar->arp_spa[0] == 192 && 
             ar->arp_spa[1] == 168 && 
             ar->arp_spa[2] == 1 &&
             ar->arp_spa[3] == 1 )
        {
            // Show MAC
            //printk("ARP:  MAC found %x.%x.%x.%x.%x.%x\n",
                //ar->arp_sha[0], 
                //ar->arp_sha[1], 
                //ar->arp_sha[2], 
                //ar->arp_sha[3],
                //ar->arp_sha[4],
                //ar->arp_sha[5] );

            // Save into the default network info.
            if ((void*) NetworkInfo != NULL)
            {
                if (NetworkInfo->initialized == TRUE)
                {
                    // #debug
                    printk("Saving gateway info\n");
                    network_fill_ipv4( 
                        NetworkInfo->gateway_ipv4,
                        ar->arp_spa );
                    network_fill_mac( 
                        NetworkInfo->gateway_mac,
                        ar->arp_sha );

                    // Show saved MAC
                    //printk("ARP:  Saved MAC %x.%x.%x.%x.%x.%x\n",
                        //NetworkInfo->gateway_mac[0], 
                        //NetworkInfo->gateway_mac[1], 
                        //NetworkInfo->gateway_mac[2], 
                        //NetworkInfo->gateway_mac[3],
                        //NetworkInfo->gateway_mac[4],
                        //NetworkInfo->gateway_mac[5] );
                }
            }
            //die();
        }

        // Show IP
        //printk("ARP: REPLY to %d.%d.%d.%d\n",
        //    ar->arp_tpa[0], ar->arp_tpa[1], ar->arp_tpa[2], ar->arp_tpa[3] );

        /*
        // to me
        if ( ar->arp_tpa[3] == 12 )
        {
            printk("ARP: Reply to ME!\n");
            printk("ARP:  MAC found %x.%x.%x.%x.%x.%x\n",
                ar->arp_sha[0], 
                ar->arp_sha[1], 
                ar->arp_sha[2], 
                ar->arp_sha[3],
                ar->arp_sha[4],
                ar->arp_sha[5] );
            //refresh_screen();
            //while(1){}

            // #todo
            // Save this values to use in the future.
            // zzzz_save_mac(ar->arp_sha);
        }
        */
    
        return;
    } else {
        //Invalid operation
    };

    return;
fail:
    printk("network_handle_arp: Fail\n");
    return;
}



