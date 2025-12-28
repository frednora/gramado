// cg.h
// cgroups for containers.
// Created by Fred Nora.

// #todo
// See: user.h and user.c for cgroups.

#ifndef __CONT_CG_H
#define __CONT_CG_H    1


// It's used when registering some programs as system services.
// They are registered in a cgroup.
struct cg_system_service_d
{
    pid_t pid;
    //tid_t tid;

    // uid?
    // ...
};

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

//
// MAIN SYSTEM COMPONENTS
//

    struct cg_system_service_d  service_display_server;
    struct cg_system_service_d  service_network_server;
    struct cg_system_service_d  service_osshell;
    struct cg_system_service_d  service_default_browser;
    // ...

// Navigation
    struct cgroup_d *next;
};

// The main cgroup.
// See: /user.c
extern struct cgroup_d  *system_cg;

extern int cg_counter;
// List of cgroups.
#define CGROUP_COUNT_MAX    16
extern unsigned long cgroupList[CGROUP_COUNT_MAX];


#endif   

