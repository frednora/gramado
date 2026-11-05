// firewall.c
// This is the low level part for the kernel-side 
// portion of the Gramado Firewall.
// Created by Fred Nora.

#ifndef __FIREWALL_FIREWALL_H
#define __FIREWALL_FIREWALL_H    1


// Inbound rules
// These control traffic coming into 
// your computer from the network (or internet).

// Outbound rules
// These control traffic going out 
// from your computer to the network/internet.

// Inbound/Outbound rules:
// “Can this traffic pass?”

// Connection Security rules:
// “Must this traffic be secured (authenticated/encrypted)?”


struct firewall_info_d 
{
    int used;
    int magic;
    int initialized;

    // ...
};
extern struct firewall_info_d  FirewallInfo;


// ------------------------------------------------

int 
firewall_apply_ethernet_filters ( 
    const unsigned char *frame, 
    ssize_t frame_size );

int 
firewall_apply_ipv4_filters( 
    const unsigned char *buffer, 
    ssize_t size );

int 
firewall_apply_tcp_filters( 
    const unsigned char *buffer, 
    ssize_t size,
    unsigned int s_ipv4_int,
    unsigned int d_ipv4_int );


int firewall_initialize(void);

#endif    

