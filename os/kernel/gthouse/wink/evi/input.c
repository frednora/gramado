// input.c
// Created by Fred Nora.

#include <kernel.h>

struct input_authority_d  InputAuthority;

unsigned long ksys_mouse_event(int event_id,long long1, long long2)
{
    int rv = 0;

    if (event_id<0)
        return 0;
    rv = (int) wmMouseEvent(event_id, long1, long2);

    return (unsigned long) (rv & 0xFFFFFFFF);
}

unsigned long ksys_keyboard_event(int event_id,long long1, long long2)
{
    int rv = 0;

    if (event_id<0)
        return 0;
    wmKeyboardEvent(event_id, long1, long2);

    return (unsigned long) (rv & 0xFFFFFFFF);
}

unsigned long ksys_timer_event(int signature)
{
    int rv = 0;

    wmTimerEvent(signature);

    return (unsigned long) (rv & 0xFFFFFFFF);
}
