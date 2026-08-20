// netif.h
// Network interface support.

#ifndef __IFCONFIG_NETIF_H
#define __IFCONFIG_NETIF_H    1

// What is the current NIC device in use?
// The network interface structure acts as a wrapper around the NIC driver.
// It describes the current NIC device in use (name, MAC, IP, gateway, etc.)
// and provides a clean abstraction for higher-level protocols to send/receive
// data without dealing directly with hardware details.

#define NET_IF_NAME_SIZE  16

struct net_interface_d 
{
    int used;
    int magic;

    int initialized;

    char name[NET_IF_NAME_SIZE];   // Interface name (e.g., "eth0")
    int link_up;     // Physical link status

// Layer 2 (Ethernet) 
    uint8_t mac[6]; // Hardware MAC address 
    uint16_t mtu; // Maximum Transmission Unit

// Layer 3 (IP) 
    uint32_t ip_addr; // IPv4 address (network byte order) 
    uint32_t netmask; // Subnet mask 
    uint32_t gateway; // Default gateway

// Routing / flags 
    //int is_default; // Is this the default interface? 
    //int flags; // Up/down, broadcast, etc.
};
extern struct net_interface_d *CurrentNetworkInterface;

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
    int link_up );

int netif_initialize(void);

#endif   

