// input.c

#include <kernel.h>

unsigned long ksys_mouse_event(int event_id,long long1, long long2)
{
    if (event_id<0)
        return 0;
    wmMouseEvent(event_id, long1, long2);

// #todo: return value
    return 0;
}

unsigned long ksys_keyboard_event(int event_id,long long1, long long2)
{
    if (event_id<0)
        return 0;
    wmKeyboardEvent(event_id, long1, long2);
// #todo: return value
    return 0;
}

unsigned long ksys_timer_event(int signature)
{
    wmTimerEvent(signature);
// #todo: return value
    return 0;
}
