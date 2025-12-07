// ns.h
// namespaces for containers.
// Created by Fred Nora.

#ifndef __CONT_NS_H
#define __CONT_NS_H    1

//
// Core namespaces
//

// UTS namespace: 
// Each container has its own hostname and domain name.
struct uts_namespace_d
{
    int used;
    int magic;
    int initialized;
};

// User namespace: 
// Isolates user and group IDs, 
// enabling different user and group mappings within containers.   
struct user_namespace_d
{
    int used;
    int magic;
    int initialized;
};

// PID namespace: 
// Each container sees its own process ID (PID) space, 
// providing process isolation.   
struct pid_namespace_d
{
    int used;
    int magic;
    int initialized;
};

// IPC namespace: 
// Isolates system IPC resources like message queues, 
// shared memory, and semaphores.   
struct ipc_namespace_d
{
    int used;
    int magic;
    int initialized;
};

// Mount namespace: 
// Each container has its own view of the filesystem, 
// allowing for different file systems and mount points.
struct mount_namespace_d
{
    int used;
    int magic;
    int initialized;
};

// Network namespace: 
// Each container has its own network stack, 
// including network interfaces, IP addresses, and routing tables.   
struct network_namespace_d
{
    int used;
    int magic;
    int initialized;
};

// ====================================================

struct ns_proxy_d
{
    struct uts_namespace_d      *ns_uts;
    struct user_namespace_d     *ns_user;
    struct pid_namespace_d      *ns_pid;
    struct ipc_namespace_d      *ns_ipc;
    struct mount_namespace_d    *ns_mount;
    struct network_namespace_d  *ns_network;
    // ...
};

#endif    

