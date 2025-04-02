
// taskbar.h

#ifndef __TASKBAR_H
#define __TASKBAR_H    1


struct icon_info_d
{
// The offset in the taskbar.
    int icon_id;
// This is the window id that represents the icon.
    int wid;

// The state of the icon, it also represents
// the state of the client application.
    int state;
};


struct tb_client_d 
{
    int used;
    int magic;

// The index into the client list.
    int client_id;
// The wid for the application window.
    int client_wid;

// The information about the icon.
    struct icon_info_d icon_info;
};
#define CLIENT_COUNT_MAX  32
extern struct tb_client_d clientList[CLIENT_COUNT_MAX];


#endif   





