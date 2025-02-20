// events.h
// Events support for libgws.
// This header should be included by including "libx.h".
// Created by Fred Nora.

#ifndef __LIBGWS_EVENTS_H
#define __LIBGWS_EVENTS_H  1

struct gws_keyboard_event_d
{
    int dummy;
};

struct gws_mouse_event_d
{
    int dummy;
};


struct gws_window_event_d
{
    int dummy;
};


// ====================================
// The event structure.

struct gws_event_d
{

    int used;
    int magic;

// 'wid' is the window.
// 'msg' is the event type.

    //standard msg block
    //int wid;    // window id.   (The window associated with this event)
    //int msg;    // msg. (event type).
    int window;   //event->window
    int type;     //event->type
    unsigned long long1;
    unsigned long long2;

    // extra
    //unsigned long long3;
    //unsigned long long4;
    //unsigned long long5;
    //unsigned long long6;
    
    struct gws_keyboard_event_d kEvent;
    struct gws_mouse_event_d    mEvent;
    struct gws_window_event_d   wEvent;
    // ...
    
    struct gws_event_d *next;
};

// Event
// Event for libx.
// See:
#define _XEvent  gws_event_d

#define GWS_EVENT_SIZE  (sizeof(struct gws_event_d))

//struct gws_event_d *CurrentEvent;
extern struct gws_event_d *CurrentEvent;


//
// ==================================
//


#endif    




