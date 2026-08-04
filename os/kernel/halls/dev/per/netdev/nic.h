// nic.h
// Created by Fred Nora

#ifndef __NETDEV_NIC_H
#define __NETDEV_NIC_H    1


// Generic structure for NIC defices
struct nic_device_d 
{
    // #todo: common fields (mac, ip, counters, link state...)
    // #todo Maybe the virtual functions.

// #test:
// The opaque pointer to driver-specific struct.
    void *priv;

// Navigation
    struct nic_device_d *next;
};

#endif   

