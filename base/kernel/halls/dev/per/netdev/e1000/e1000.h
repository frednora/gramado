// e1000.h
// Wrappers.
// High level routines to interact with the 
// e1000 device driver.


#ifndef __NETDEV_E1000_H
#define __NETDEV_E1000_H    1

int 
e1000_send(
    struct intel_nic_info_d *dev, 
    size_t len, 
    const char *data );

// See: e1000.c
int 
e1000_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );



#endif   

