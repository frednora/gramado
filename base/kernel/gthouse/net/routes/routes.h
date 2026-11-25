
/*
interface → pointer to the NIC abstraction (net_interface_d).
This tells the routing logic which physical/virtual port to send the packet through.

gateway_ipv4 → the next hop IP address.
If the destination is outside the local subnet, packets go here first.
If 0, it means “no gateway, send directly.”

destination_ipv4 → the base address of the destination network.
Example: 192.168.1.0 for the subnet 192.168.1.0/24.

subnet_mask → defines the domain (network size).
Example: 255.255.255.0 means 256 addresses (192.168.1.0–192.168.1.255).

Used to check if a target IP belongs to this domain.
*/

struct net_route_d
{
    // Interface: which NIC to use
    struct net_interface_d *interface;

    // Gateway: next hop IP (0 if direct)
    uint32_t gateway_ipv4;

    // Destination: the target network
    uint32_t destination_ipv4;  // e.g. 192.168.1.0

    // Subnet mask: defines the domain size
    uint32_t subnet_mask;       // e.g. 255.255.255.0
};
