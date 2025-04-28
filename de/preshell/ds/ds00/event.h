// event.h
// Event manager.
// Created by Fred Nora.

#ifndef EVENT_H
#define EVENT_H    1

// #todo: 
// no gramado as mensagens do systema
// podem se tornar 'gramado_ev' um tipo de evento gramado.
// e a 'mensagem' determina o tipo de evento.
// alem disso podemos ter mais detalhes
// como se o evento vem do sistema ou do window server ...

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

// Main event strcuture.
struct gws_event_d
{

    // The time we get the event there in the origin.
    // It can be a device driver ...
    //unsigned long origin_jiffies;
    // The time we get the event here in the server.
    //unsigned long jiffies;

    //todo: the client
    // ...client

    // TRUE or FALSE
    int has_event;  //??

    //#test
    // The root window for this event
    //int root_wid;
    // A child affected by this event.
    //int child_wid;

    //standard msg block
    int wid;    // window id. (event window)
    int msg;    // msg. (event type).
    unsigned long long1;  // data (key state, button state)
    unsigned long long2;  // data (key state, button state)
    
    // extra data
    //unsigned long long3;
    //unsigned long long4;
    //unsigned long long5;
    //unsigned long long6;

    //#test
    //int x, y;		/* pointer x, y coordinates in event window */
    //int x_root, y_root;	/* coordinates relative to root */

// #test
// We can have key event, motion event, button event ... etc.
// see: Xlib.h hehehe
    struct gws_keyboard_event_d  kEvent;
    struct gws_mouse_event_d     mEvent;
    struct gws_window_event_d    wEvent;
    // ...

    //?? #test
	//unsigned long serial;	/* # of last request processed by server */

    struct gws_event_d *next;
};

//#test
struct gws_event_d  NextSingleEvent;

//#test
// Main structure for event.
struct gws_event_d gwsEvent;

//#test
struct gws_event_d *gwsCurrentEvent;

/*
 // uma janela tem o foco de entrada e o cursor esta piscando nela.
struct gws_keyboad_focus_d
{
    struct gws_cliend_d *client;
    struct gws_window_d* window;
    //...
};
*/


#endif    


