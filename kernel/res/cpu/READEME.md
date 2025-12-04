# cpu - CPU as a resource

The cpu portion in core/ke/ is seen as an (Execution Engine) and 
in res/cpu its seen as a resource, affected by the cgroups and namespace policies.

=========================================
Dual Identity of the CPU

In core/ke/ → Execution Engine
 The CPU is the heartbeat of the kernel.
 Here it’s seen as the mechanism: scheduling, dispatching, context switching, HAL integration.
 It’s the final step in the chain of user intention → kernel orchestration → execution.

In res/cpu/ → Resource
 + The CPU is treated as a manageable resource, just like memory or devices.
 + Here you expose routines for:
    CPU topology (cores, threads, NUMA).
    Usage statistics and accounting.
    Frequency scaling and power states.
    Affinity and isolation (pinning tasks to cores).
 + This is the layer where cgroups and namespaces apply policies: quotas, shares, isolation.

