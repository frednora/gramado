// desktop.h
// This is the header for the desktop application. (explorer-like)
// Created by Fred Nora

#ifndef __DESKTOP_H
#define __DESKTOP_H    1

#include "config.h"

extern struct gws_display_d *Display;

/*
#define TB_ICON_STATE_RUNNING   1
#define TB_ICON_STATE_MINIMIZED 2
#define TB_ICON_STATE_ACTIVE    3
#define TB_ICON_STATE_CLOSED    4
*/

// Think it as a container
struct ui_area_d
{
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    int component_count;

    unsigned long component_list[32];
};

struct icon_info_d
{
// The offset in the taskbar
    int icon_id;
// This is the window id that represents the icon
    int wid;

// Absolute values
    unsigned long absolute_left;
    unsigned long absolute_top;
    unsigned long width; 
    unsigned long height;

// Relative values
    unsigned long left;
    unsigned long top;

// The state of the icon, it also represents
// the state of the client application.
// (running, minimized, etc.).
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
    //int client_pid;
    int client_tid;

// The information about the icon.
    struct icon_info_d icon_info;

    unsigned long created_jiffies;
};
#define CLIENT_COUNT_MAX  32
extern struct tb_client_d clientList[CLIENT_COUNT_MAX];


// Taskbar info
struct taskbar_info_d
{
    int initialized;
    int inUse;
    int style;

    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    struct ui_area_d icon_area;
    struct ui_area_d notification_area;
};
extern struct taskbar_info_d  TaskbarInfo;

//
// == Prototypes =================================
//



#endif   

