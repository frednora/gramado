// mac.h
// MAC address information.
// Created by Fred Nora.

#ifndef __NET_MAC_H
#define __NET_MAC_H    1


struct mac_address_d
{
    struct kobject_d kobj;

    // 6-byte
    unsigned long mac_address_high;   // OUI
    unsigned long mac_address_low;    // NIC
};

#endif   

//
// End
//

