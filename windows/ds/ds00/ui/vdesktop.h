
// vdesktop.h
// Virtual desktops.
// ps:
// This is not the same concept of desktops
// found inside the kernel. 
// In the kernel will be used for security and
// distribuition of resources.


#ifndef __VDESKTOP_H
#define __VDESKTOP_H    1


struct gws_vdesktop_d
{
    int used;
    int magic;
    int id;

    int pid;
    int tid;

    unsigned long flags;

// Windows
    struct gws_window_d *desktop_window;   //root?
    struct gws_window_d *sysmenu_window;
    struct gws_window_d *fg_window;
    struct gws_window_d *notification_window;

    struct gws_vdesktop_d *next;
};



#endif    


