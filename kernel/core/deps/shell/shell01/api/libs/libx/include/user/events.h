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

// Security
    int used;
    int magic;

// Header
// Window ID, Event type, data1, data2 
    int window;  // Event target.
    int type;
    unsigned long long1;
    unsigned long long2;

// Extra data
    unsigned long long3;
    unsigned long long4;
    unsigned long long5;
    unsigned long long6;

// #todo:
// Maybe we're gonna use these sub-structures.
// These fields shoud containd information about the devices.
    struct gws_keyboard_event_d kEvent;
    struct gws_mouse_event_d    mEvent;
    struct gws_window_event_d   wEvent;
    // ...

//
// Event propagation support.
//

// Indicates whether the event bubbles up the DOM tree.
    int bubbles;

// Marks the event as processed, preventing further propagation.
// TRUE or FALSE.
    int handled;

// ??
// Determines if the event’s default action can be prevented.
    //int cancelable;



// Navigation.
    struct gws_event_d *next;
};
extern struct gws_event_d *CurrentEvent;

#define GWS_EVENT_SIZE  (sizeof(struct gws_event_d))

// Event
// Event for libx.
// See:
#define _XEvent  gws_event_d


//
// ==================================
//


#endif    




