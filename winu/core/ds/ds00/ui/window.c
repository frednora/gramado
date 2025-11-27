// window.c
// Some other routines for windows.
// Created by Fred Nora.


#include "../ds.h"

/*
// Keeps client rect logic consistent across all window types.
void update_client_area(struct gws_window_d *w);
void update_client_area(struct gws_window_d *w) 
{
    if ((void*) w == NULL)
        return;
    if (w->magic != 1234)
        return;
    w->rcClient.left   = w->border_size;
    w->rcClient.top    = w->border_size + w->titlebar_height;
    w->rcClient.right  = w->width  - w->border_size;
    w->rcClient.bottom = w->height - w->border_size - w->statusbar_height;
}
*/


struct gws_window_d *get_parent_window(struct gws_window_d *w)
{
    struct gws_window_d *p;

    if ((void*) w == NULL)
        goto fail;
    if (w->magic != 1234){
        goto fail;
    }

    p = (struct gws_window_d *) w->parent;
    if ((void*) p == NULL)
        goto fail;
    if (p->magic != 1234){
        goto fail;
    }

    return (struct gws_window_d *) p;
fail:
    return NULL;
}

// Hit test
int 
is_within ( 
    struct gws_window_d *window, 
    unsigned long x, 
    unsigned long y )
{
// #bugbug
// E se a janela tem janela mae?

// Parameter:
    if ((void*) window == NULL)
        return FALSE;
    if (window->used != TRUE)
        return FALSE;
    if (window->magic != 1234)
        return FALSE;

// Within?
    if ( x >= window->absolute_x  && 
         x <= window->absolute_right  &&
         y >= window->absolute_y  &&
         y <= window->absolute_bottom )
    {
        return TRUE;
    }

    return FALSE;
}


// Hit test
int 
is_within2 ( 
    struct gws_window_d *window, 
    unsigned long x, 
    unsigned long y )
{
    struct gws_window_d *pw;
    struct gws_window_d *w;

// #bugbug
// E se a janela tem janela mae?

// Parameter:
    if ((void*) window == NULL)
        return FALSE;
    if (window->used != TRUE)
        return FALSE;
    if (window->magic != 1234)
        return FALSE;

// ====

// pw
// The parent window.
    pw = window->parent;
    if ((void*) pw == NULL){
        return FALSE;
    }
    if (pw->used != TRUE)
        return FALSE;
    if (pw->magic != 1234)
        return FALSE;

// w
// The window itself
    w = window;
    if ((void*) w == NULL){
        return FALSE;
    }
    if (w->used != TRUE)
        return FALSE;
    if (w->magic != 1234)
        return FALSE;

// Relative to the parent.
    int x1= pw->absolute_x + w->absolute_x; 
    int x2= x1 + w->width;
    int y1= pw->absolute_y  + w->absolute_y;
    int y2= y1 + w->height;

    if( x > x1 && 
        x < x2 &&
        y > y1 && 
        y < y2 )
    {
        return TRUE;
    }

    return FALSE;
}

void disable_window(struct gws_window_d *window)
{
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

    window->enabled = FALSE;
    if (window->type == WT_BUTTON)
        window->status = BS_DISABLED;
}

// Valid states only.
void change_window_state(struct gws_window_d *window, int state)
{
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

// Is it a valid state?
// #todo: We can create a worker for this routine.
    switch (state){
        case WINDOW_STATE_FULL:
        case WINDOW_STATE_MAXIMIZED:
        case WINDOW_STATE_MINIMIZED:
        case WINDOW_STATE_NORMAL:
            window->state = state;
            break;
        default:
            break;
    };
}

void enable_window(struct gws_window_d *window)
{
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

    window->enabled = TRUE;
    if (window->type == WT_BUTTON)
        window->status = BS_DEFAULT;
}

/*
On sending PAINT message to the clients:
How Mature Systems Mitigate This
Expose/paint events are batched to minimize redundant repaints.
Dirty rectangle management is used: only the changed region is repainted.
Shared memory buffers are used for drawing, reducing IPC overhead.
Compositing: The client draws into a buffer, and only the buffer handle 
is passed to the server.
Event coalescing: Multiple paint requests may be merged into one.
*/

// Post message to the window.
// Post an event message to the specified window's event queue.
//
// Parameters:
//   wid        - Window ID (index in windowList)
//   event_type - Event/message type code
//   long1      - First event parameter (usage depends on event_type)
//   long2      - Second event parameter (usage depends on event_type)
//
// Returns:
//   0 on success, -1 on error (invalid parameters or window)
int 
window_post_message( 
    int wid, 
    int event_type, 
    unsigned long long1,
    unsigned long long2 )
{
    struct gws_window_d *w;

// Parameters
    if (wid < 0)
        goto fail;
    if (wid >= WINDOW_COUNT_MAX)
        goto fail;
    if (event_type < 0){
        goto fail;
    }

// Window
    w = (void*) windowList[wid];
    if ((void*) w == NULL)
        goto fail;
    if (w->magic != 1234){
        goto fail;
    }

//
// Event
//

// --- Post event message to window's circular event queue ---

// Get current tail index for the queue
    register int Tail = (int) w->ev_tail;
// Fill in event data at the current tail position
    w->ev_wid[Tail]   = (unsigned long) (wid & 0xFFFFFFFF);        // Window ID
    w->ev_msg[Tail]   = (unsigned long) (event_type & 0xFFFFFFFF); // Event/message code
    w->ev_long1[Tail] = (unsigned long) long1;
    w->ev_long2[Tail] = (unsigned long) long2;

// Advance the tail index, wrapping around if necessary (circular buffer)
    w->ev_tail++;
    if (w->ev_tail >= 32){
        w->ev_tail=0;
    }

   return 0;

fail:
    return (int) -1;
}

// Post message to the window. (broadcast).
// Return the number of sent messages.
int 
window_post_message_broadcast( 
    int wid, 
    int event_type, 
    unsigned long long1,
    unsigned long long2 )
{
    int return_value=-1;
    register int i=0;
    int Counter=0;
    struct gws_window_d *wReceiver;
    int target_wid = -1;

// Invalid message code.
    if (event_type < 0)
        goto fail;

// Probe for valid Overlapped windows and
// send a close message.
    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        wReceiver = (void*) windowList[i];
        if ((void*) wReceiver != NULL)
        {
            if (wReceiver->magic == 1234)
            {
                if (wReceiver->type == WT_OVERLAPPED)
                {
                    target_wid = (int) wReceiver->id;
                    window_post_message( 
                        target_wid, event_type, long1, long2 );
                    Counter++;
                }
            }
        }
    };

    return (int) Counter;
fail:
    return (int) -1;
}

