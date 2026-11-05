// netif.c
// Network interface support.

#include <kernel.h>

// See: netif.h
struct net_interface_d *CurrentNetworkInterface;

// ==============================================

// Update fields of a given network interface.
int 
netif_update(
    struct net_interface_d *iface,
    const char *name,
    const uint8_t mac[6],
    uint32_t ip_addr,
    uint32_t netmask,
    uint32_t gateway,
    uint16_t mtu,
    int link_up )
{
    register int i=0;

    if ((void*) iface == NULL)
        return -1;
    if (iface->used != TRUE)
        return -1;
    if (iface->magic != 1234)
        return -1;

    // Name
    if (name) 
    {
        strncpy(iface->name, name, NET_IF_NAME_SIZE-1);
        iface->name[NET_IF_NAME_SIZE-1] = '\0';
    }

    // MAC
    if (mac) 
    {
        for (i=0; i<6; i++){
            iface->mac[i] = mac[i];
        };
    }

    // IP configuration
    iface->ip_addr = ip_addr;
    iface->netmask = netmask;
    iface->gateway = gateway;

    // MTU
    iface->mtu = mtu;

    // Link status
    iface->link_up = link_up;

    // Mark as initialized
    iface->initialized = TRUE;
    return 0;
}

// Called by netInitialize() in net.c
int netif_initialize(void)
{
    struct net_interface_d *ni;
    register int i=0;

// Create object
    ni = (void*) kmalloc( sizeof(struct net_interface_d) );
    if ((void*) ni == NULL)
        goto fail;

// Initialize fields
    ni->used = TRUE;
    ni->magic = 1234;
    ni->initialized = FALSE;
    // name
    for (i=0; i<NET_IF_NAME_SIZE; i++){
        ni->name[i] = 0;    
    }
    // link up
    ni->link_up = FALSE;
    // MAC address
    for (i=0; i<6; i++){
        ni->name[i] = 0;    
    }
    // Maximum Transmission Unit
    ni->mtu = 0;
    ni->ip_addr = 0; // IPv4 address (network byte order) 
    ni->netmask = 0; // Subnet mask 
    ni->gateway = 0; // Default gateway

// Mark as initialized 
    ni->initialized = TRUE; 

// Save as current interface 
    CurrentNetworkInterface = ni;

    return 0;

fail:
    printk("netif_initialize: fail\n");
    return (int) -1;
}