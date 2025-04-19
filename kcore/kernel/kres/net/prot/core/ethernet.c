// ethernet.c
// Created by Fred Nora.

#include <kernel.h>


// ==============================================================

// Sending a raw packet.
// #bugbug
// For now we're sending it only for intel e1000 nic device.
int 
ethernet_send(
    struct intel_nic_info_d *dev, 
    size_t len, 
    const char *data )
{

// #todo
// NIC Intel device structure.
    if ((void *) dev == NULL){
        printk("ethernet_send: dev\n");
        goto fail;
    }

// Telling to the network manager that we're gonna send something 
// a a nic device driver.
// see: network.c
    network_on_sending();

// Send frame via NIC.
// IN: nic, frame size, frame pointer.
    e1000_send( dev, len, data );

// done:
    return 0;

fail:
    return (int) -1;
}


// Called when sending some raw packet.
// #ps: We do NOT send, we're called by the sending routines.
int ethernet_on_sending (void)
{
// Called by ethernet_send() in core/ethernet.c.

    //on_sending_counter++;

    // ...

    return 0;
}

//
// $
// HANDLER
//

// IN:
// + frame base address
// + frame total size
// Called by __e1000_on_receive() in e1000.c.
int 
network_handle_ethernet ( 
    const unsigned char *frame, 
    ssize_t frame_size )
{
// + Handle ethernet header.
// + Call the handler for the given protocol.

    struct ether_header *eth = (struct ether_header *) frame;
    uint16_t Type=0;

// Drop it!
// Set this flag using the command "net-on" on terminal.bin.
    if (NetworkInitialization.initialized != TRUE)
    {
        //#debug
        //printk("Packet: Network is OFF\n");
        goto fail;
    }

// If the network is locked
    if (NetworkInitialization.locked == TRUE)
        goto fail;

// Parameters:
// Frame validation
    if ((void*) frame == NULL){
        //printk("network_on_receiving: frame\n");
        goto fail;
    }
    // 1~8192
    if ( frame_size <= 0 || frame_size > E1000_DEFAULT_BUFFER_SIZE ){
        //printk("network_on_receiving: frame_size\n");
        goto fail;
    }

//
// Option
//

// Push packet.
// Maybe we can simply pash the packet into the circular queue.
// Mayme we can change this queue and use a lined list.
// Or create a queue support. We still don't have this.
// Coloca em um dos buffers, 
// de onde os aplicativos podem pegar depois.
// see: network.c

    // #todo
    // We need a configuration flag here.
    // Telling that we need to push the packets into the queue
    // instead of processing them.

    //network_buffer_in ( (void *) buffer, (int) size );
    //return;
    
    // #debug
    //printk("\n");
    //printk("Ethernet Header\n");

// Ethernet header
    if ((void*) eth == NULL){
        goto fail;
    }

// #debug
// Destination MAC
// Source MAC
// Protocol type.

// #todo
// Let's save it for future use.
// We're gonna need to send responses.

/*
    printk ("   |-Destination Address : %x-%x-%x-%x-%x-%x \n", 
        eth->dst[0], eth->dst[1], eth->dst[2], 
        eth->dst[3], eth->dst[4], eth->dst[5] );

    printk ("   |-Source Address      : %x-%x-%x-%x-%x-%x \n", 
        eth->src[0], eth->src[1], eth->src[2], 
        eth->src[3], eth->src[4], eth->src[5] );

    printk ("   |-Ethertype           : %x \n",
        (unsigned short) eth->type);
*/

// Save the MAC of the caller.
    network_fill_mac(__saved_caller_mac, eth->mac_src);
// #todo
// Here we can check if the destination is us,
// if the packet is not for us, we simply drop it.

// Get ethernet type
// Handle the rthernet type
// See: ipv6.c, ipv4.c, arp.c.

    Type = (uint16_t) FromNetByteOrder16(eth->type);
    switch (Type){

    // #todo
    case ETHERTYPE_IPV6:
        // Drop it
        goto fail;
        // #todo
        //network_handle_ipv6( 
            //(frame + ETHERNET_HEADER_LENGHT), 
            //(frame_size - ETHERNET_HEADER_LENGHT) );
        break;

    case ETHERTYPE_IPV4:
        network_handle_ipv4( 
            (frame + ETHERNET_HEADER_LENGHT), 
            (frame_size - ETHERNET_HEADER_LENGHT) );
        break;

    case ETHERTYPE_ARP:
        network_handle_arp( 
            (frame + ETHERNET_HEADER_LENGHT), 
            (frame_size - ETHERNET_HEADER_LENGHT) );
        break;

    // ...

    // Unsupported type.
    default:
        // Drop it
        goto fail;
        break;
    };

// done:
    return 0;
fail:
    return (int) -1;
}




