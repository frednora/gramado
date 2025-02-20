// host.h
// Created by Fred Nora.

#ifndef ____HOST_H
#define ____HOST_H    1

struct gws_host_d
{
    int used;
    int magic;
    int id;
    char name[256];
    // ...
};
// see: globals.c
extern struct gws_host_d  *CurrentHost;

#endif    

