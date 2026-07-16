// dev.h
// Main header file for the dev/ kernel module.
// Global informations about device management.
// Created by Fred Nora.

#ifndef __DEV_DEV_H
#define __DEV_DEV_H  1


// driver_type_t
// Enumerate types of device drivers.
// + Persistent: Belongs to the kernel image and can't be deleted.
// + Modular:    Loadable ring 0 kernel module.
// + Ring 3:     Loadable ring3 device driver.
typedef enum {
    DriverTypeSystem,     // Persistente.
    DriverTypeModular,    // Modular. Loadable ring 0 kernel module.
    DriverTypeRing3       // Loadable ring3 device driver.
    //...
}driver_type_t; 


// Belongs to this type of host device. Mobo type?
typedef enum {
    HostDeviceTypePC,        // PC.
    HostDeviceTypeMobile,    // Mobile.
    HostDeviceTypeIOT,       // Iot.
    //...
}host_device_type_t; 


struct host_device_info_d
{

//Type.
    host_device_type_t type;

// Screen size.
    unsigned long host_device_screen_width;
    unsigned long host_device_screen_height;

    // Continue ...
};
// #todo: Define this thing in some place.
// extern struct host_device_info_d *HostDeviceInfo;


// Display device stuff.
// Screen sizes and bpp.
// see: bldisp.c
extern unsigned long g_device_screen_width;
extern unsigned long g_device_screen_height;
extern unsigned long g_device_screen_bpp; 

#endif   

//
// End.
//

