// host.h
// Created by Fred Nora.

#ifndef __GRAMADO_HOST_H
#define __GRAMADO_HOST_H    1

struct gws_host_d
{
    int id;
    int used;
    int magic;
//
    char name[256];  // host name
    //char os_name[128];  // os name
    // ...
};

#endif    



