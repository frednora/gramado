// ethernet.c
// Created by Fred Nora.

/*
Build/send raw Ethernet frames 
Dispatch received frames to ARP/IP handlers 
NOT choose which NIC to use 
NOT decide next hop 
NOT resolve IP routing
*/

#include <kernel.h>


// ==============================================================

// Sending a raw packet.
// #bugbug
// For now we're sending it only for intel e1000 nic device.
// #test #todo
// device‑agnostic
// Debugging is easier: 
// you can start printing interface state (name, MAC, IP) whenever you send, 
// which helps confirm the stack is using the right interface.

// IN:
// + len = Frame size
// + data = Frame pointer

// Expectation: 
// frame_pointer should point to a contiguous buffer containing 
// Ethernet header + IP header + payload. 

// #todo
// We are wiring the Ethernet layer into the network interface abstraction.

// Buffers: [ethernet, data]
int 
ethernet_send(
    size_t frame_size, 
    const char *frame_pointer )
{
    struct net_interface_d *iface; 

// Get current network interface
    iface = CurrentNetworkInterface;
    if ((void*)iface == NULL){
        printk("ETH: iface\n");
    }
    if (iface->magic != 1234)
        printk("ETH: iface magic\n");
    if (iface->initialized != TRUE)
        printk("ETH: iface not initialized\n");

// Telling to the network manager that we're gonna send something 
// a a nic device driver.
// see: network.c
    network_on_sending();

// ========================================

    int UseIntel = TRUE;
    struct intel_nic_info_d *nic_intel;
    // ...

    if (UseIntel == TRUE)
    {
        nic_intel = currentNIC;
        if ((void *) nic_intel == NULL)
        {
            printk("ethernet_send: nic_intel\n");
            goto fail;
        }
        
        // Send frame via NIC.
        // IN: nic, frame size, frame pointer.
        e1000_send( nic_intel, frame_size, frame_pointer );
    }

    // #todo: Other devices
    //if (...){
    //}

    return 0;  // Done

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
// + Handle ethernet header.
// + Call the handler for the given protocol.

int 
network_handle_ethernet ( 
    const unsigned char *frame, 
    ssize_t frame_size )
{
    struct ethernet_d *eth = (struct ethernet_d *) frame;
    uint16_t Type=0;
    unsigned char *payload_base;
    ssize_t PayloadSize;
    int UseFirewall = FALSE;
    int FirewallStatus = FALSE;

// The network interface needs to be initialized and unlocked.
    if (NetworkInitialization.initialized != TRUE)
    {
        goto fail;
    }
    if (NetworkInitialization.locked == TRUE)
    {
        goto fail;
    }

// Parameters:
// Frame validation
    if ((void*) frame == NULL){
        //printk("network_on_receiving: frame\n");
        goto fail;
    }
    // 1 ~ 8192
    if (frame_size <= 0 || frame_size > E1000_DEFAULT_BUFFER_SIZE)
    {
        //printk("network_on_receiving: frame_size\n");
        goto fail;
    }
    // #test: Smaller than ethernet header
    if ( frame_size < sizeof(struct ethernet_d) )
    {
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

    // Invalid Ethernet header
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

/*
    // #debug: Cathing broadcast
    if (eth->mac_dst[0]==0xFF && eth->mac_dst[1]==0xFF &&
        eth->mac_dst[2]==0xFF && eth->mac_dst[3]==0xFF &&
        eth->mac_dst[4]==0xFF && eth->mac_dst[5]==0xFF) 
    {
        printk("Ethernet: Broadcast MAC received\n");
    }
*/

// Save the MAC of the caller
    network_fill_mac( NetworkSaved.caller_mac, eth->mac_src );

// #todo
// Here we can check if the destination is us,
// if the packet is not for us, we simply drop it.

    payload_base = (frame + ETHERNET_HEADER_LENGHT);
    PayloadSize = (frame_size - ETHERNET_HEADER_LENGHT);

// Get ethernet type
// Handle the rthernet type
// See: ipv6.c, ipv4.c, arp.c.

    Type = (uint16_t) FromNetByteOrder16(eth->type);
    switch (Type){

    case ETH_TYPE_IPV6:
        // network_handle_ipv6(payload_base, PayloadSize); 
        goto fail;
        break;

    case ETH_TYPE_IPV4:
        if (UseFirewall == TRUE)
        {
            FirewallStatus = firewall_apply_ipv4_filters(payload_base, PayloadSize);
            if (FirewallStatus < 0)
            {
                // Do something ... drop, log etc ...
            }
        }
        // Call the handler
        network_handle_ipv4(payload_base, PayloadSize);
        return 0;  // OK
        break;

    case ETH_TYPE_ARP:
        network_handle_arp(payload_base, PayloadSize);
        return 0;  // OK
        break;

    case ETH_TYPE_RARP:
        goto fail;
        break;

    // ...

    // Unsupported type
    default:
        goto fail;  // Drop it
        break;
    };

    return 0;   // done

fail:
    return (int) -1;
}




