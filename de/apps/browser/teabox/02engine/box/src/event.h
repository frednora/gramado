// event.h
// Internal definition of event.
// engine.

#ifndef EVENT_H
#define EVENT_H    1

// #todo: 
// no gramado as mensagens do systema
// podem se tornar 'gramado_ev' um tipo de evento gramado.
// e a 'mensagem' determina o tipo de evento.
// alem disso podemos ter mais detalhes
// como se o evento vem do sistema ou do window server ...


struct gws_keyboard_event_Internal_d
{
    int dummy;
};

struct gws_mouse_event_Internal_d
{
    int dummy;
};


struct gws_window_event_Internal_d
{
    int dummy;
};


struct gws_event_Internal_d
{

    //todo: the client
    // ...client

    // TRUE or FALSE
    int has_event;

    //standard msg block
    int wid;    // window id.
    int msg;    // msg. (event type).
    unsigned long long1;
    unsigned long long2;
    
    // extra
    //unsigned long long3;
    //unsigned long long4;
    //unsigned long long5;
    //unsigned long long6;

    
    struct gws_keyboard_event_Internal_d  kEvent;
    struct gws_mouse_event_Internal_d     mEvent;
    struct gws_window_event_Internal_d    wEvent;
    // ...

    struct gws_event_Internal_d *next;
};

//#test
struct gws_event_Internal_d  NextSingleEvent;

//#test
// Main structure for event.
struct gws_event_Internal_d gwsEvent;

//#test
struct gws_event_Internal_d *gwsCurrentEvent;

#endif    


