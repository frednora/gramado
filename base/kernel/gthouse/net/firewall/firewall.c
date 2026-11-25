// firewall.c
// This is the low level part for the kernel-side 
// portion of the Gramado Firewall.
// Created by Fred Nora.

#include <kernel.h>


struct firewall_info_d  FirewallInfo;

int 
firewall_apply_ethernet_filters ( 
    const unsigned char *frame, 
    ssize_t frame_size )
{
    return 0;  // OK
}

int 
firewall_apply_ipv4_filters( 
    const unsigned char *buffer, 
    ssize_t size )
{
    return 0;  // OK
}

int 
firewall_apply_tcp_filters( 
    const unsigned char *buffer, 
    ssize_t size,
    unsigned int s_ipv4_int,
    unsigned int d_ipv4_int )
{
    return 0;  // OK
}

int firewall_initialize(void)
{
    FirewallInfo.used = FALSE;
    FirewallInfo.magic = 0;
    FirewallInfo.initialized = FALSE;

    // #todo: Create the initialization routine
    // ...


// Done:
    FirewallInfo.used = TRUE;
    FirewallInfo.magic = 1234;
    FirewallInfo.initialized = TRUE;
    return 0;
}

