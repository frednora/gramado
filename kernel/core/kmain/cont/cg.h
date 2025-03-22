// cg.h
// cgroups for containers.
// Created by Fred Nora.

// #todo
// See: user.h and user.c for cgroups.

#ifndef __CONT_CG_H
#define __CONT_CG_H    1

// ===================================================
// cgroup
// cgroups is all about resources management.

struct cgroup_d
{
// Register some components of the cgroups.
// + display server.
// + network server.
// ...

    object_type_t  objectType;
    object_class_t  objectClass;
    int used;
    int magic;

    // cgroup ID.
    int id;

// usando o mesmo esquema do usuário.
    char __name[64];
    int name_lenght;

    uid_t uid;

// Main PIDs
    pid_t __display_server_pid;   // display server
    pid_t __network_server_pid;   // network server
    pid_t __browser_pid;   // network server
    // ...

// Navigation
    struct cgroup_d *next;
};
// See: gramk/user/user.c
extern struct cgroup_d  *CurrentCG;
extern int current_cgroup; 
extern int cg_counter;
// List of cgroups.
#define CGROUP_COUNT_MAX    16
extern unsigned long cgroupList[CGROUP_COUNT_MAX];


#endif   

