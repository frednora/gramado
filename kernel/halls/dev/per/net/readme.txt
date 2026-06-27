
Basically there are two models:
+ client/server. (connection-driver)
+ message queue. (publishers and subscribers) (queue-driven)

#ps:
We can also mix the idea of 'message queue' and 'database', 
for volatile and persistent data.

List of the main infrastructure models, each with a single‑line explanation:

- Client/Server     → Direct request/response between client and server, best for synchronous interactions.  
- Message Queue     → Producers send messages to a broker, consumers process them asynchronously.  
- Publish/Subscribe → Publishers broadcast to topics, subscribers receive updates like a news feed.  
- Event Sourcing    → Every change is stored as an event log, state is rebuilt by replaying events.  
- Hybrid Models     → Mix of direct client/server calls and asynchronous queues for flexibility.  

-------------------------
Consumer Interaction Models:

Pull model   → The consumer explicitly requests the next message from the broker. 
               This is how Kafka works: consumers poll topics/partitions for new events.

Push model   → The broker delivers messages to consumers as soon as they arrive. 
               This is how RabbitMQ works by default: it pushes messages from queues to subscribers.

Hybrid model → Some systems allow both: consumers can request messages (pull) or 
               receive them automatically (push), depending on configuration.


Analogy
Pull: Like refreshing your news feed manually — you ask, “Any new articles?”
Push: Like getting notifications on your phone — the system tells you immediately when something new arrives.

--------------------------------------------------

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

