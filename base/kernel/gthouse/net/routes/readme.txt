
 How RouteEntry Works Conceptually

+ NIC device (iface) → the physical or virtual interface through which packets leave your host.
+ Gateway (gateway) → the next hop device that knows how to forward traffic beyond your local subnet.
+ Destination (dest + netmask) → defines the domain (network segment) that this route applies to.

Example: 
dest=192.168.1.0, 
netmask=255.255.255.0 → this route covers the domain 192.168.1.0/24.

Node → the final target host you want to reach, 
       which must fall inside one of the domains defined by your routing table.


How It Works in Routing
When sending a packet:

Compare the destination IP against each net_route_d entry.
Apply the subnet_mask to see if the IP falls inside the destination_ipv4 domain.
If it matches:
Use the specified interface.
If gateway_ipv4 != 0, forward to the gateway.
Otherwise, send directly (via ARP).
If no match, fall back to a default route (usually 0.0.0.0/0 with a gateway).

       