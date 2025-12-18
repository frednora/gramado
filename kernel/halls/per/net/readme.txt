
net/
 |-- ifconfig/   # configure interfaces
 |-- hosts/      # individual nodes
 |-- domains/    # administrative domains (groups of hosts)
 |-- routes/     # routing table and forwarding logic
 |-- prot/       # protocols TCP, UDP, ICMP, DHCP, DNS ...
      |-- core/  # Ethernet, IP, ARP

Also see:
 |-- blkdev/
 |-- chardev/
 |-- netdev/   # Network devices (NICs, Wi‑Fi adapters, virtual interfaces).

-----------------------------
Networking Stack (net/)

ifconfig/ → interface setup and configuration (assign IP, mask, gateway).
hosts/ → individual nodes (self, peers, gateway, DNS).
domains/ → administrative domains (logical groups of hosts under one authority).
routes/ → routing table and forwarding logic (decide which interface/gateway to use).
prot/ → protocol implementations.
        Higher‑level protocols (TCP, UDP, ICMP, DHCP, DNS) layered on top.
prot/core/ → foundational protocols (Ethernet, IP, ARP).

This gives you a layered networking stack:
+ Core protocols at the bottom.
+ Routing and host/domain abstractions in the middle.
+ Interfaces at the edge.
+ Higher‑level protocols on top.

