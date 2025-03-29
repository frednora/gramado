// ns.h
// namespaces for containers.
// Created by Fred Nora.

#ifndef __CONT_NS_H
#define __CONT_NS_H    1

//
// Core namespaces
//

//UTS namespace: 
//Each container has its own hostname and domain name.
struct uts_namespace_d
{
    int used;
};

//User namespace: 
//Isolates user and group IDs, enabling different user and group mappings within containers.   
struct user_namespace_d
{
    int used;
};

//PID namespace: 
//Each container sees its own process ID (PID) space, providing process isolation.   
struct pid_namespace_d
{
    int used;
};

//IPC namespace: 
//Isolates system IPC resources like message queues, shared memory, and semaphores.   
struct ipc_namespace_d
{
    int used;
};

//Mount namespace: 
//Each container has its own view of the filesystem, allowing for different file systems and mount points.
struct mount_namespace_d
{
    int used;
};

//Network namespace: 
//Each container has its own network stack, including network interfaces, IP addresses, and routing tables.   
struct network_namespace_d
{
    int used;
};

#endif    

