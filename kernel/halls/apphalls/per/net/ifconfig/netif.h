// netif.h
// Network interface support.


// Define a structure to represent the network interface state
struct net_interface_d
{
    char name[16];          // Interface name (e.g., "eth0")
    bool link_up;           // Physical link status
    // ...
};

