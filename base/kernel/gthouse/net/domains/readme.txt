
 domains - A place to keep the information about the domains.

 Domain is a group of nodes.

Represents a collection of hosts under a common boundary or policy.
Used to group hosts logically (e.g., “corpnet”, “lab”, “ISP”).
Can store metadata about the domain itself (administrator, routing rules, trust level).
Doesn’t duplicate host data — instead, it references hosts.

Avoiding Redundancy:
Keep all host definitions in net/hosts/.
Let net/domains/ contain lists of references (IDs or pointers) to hosts that belong to each domain.
This way, hosts/ is the source of truth for node info, and domains/ is just a grouping mechanism.