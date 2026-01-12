// window.c
// Some other routines for windows.
// Created by Fred Nora.


#include "../ds.h"

unsigned long windows_count=0;
int show_fps_window = FALSE;

static int __config_use_transparency=FALSE;


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

void gws_enable_transparence(void){
    __config_use_transparency=TRUE;
}
void gws_disable_transparence(void){
    __config_use_transparency=FALSE;
}

int window_has_transparence(void){
    return (int) __config_use_transparency;
}

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

// Notify app about an event.
// Targets: taskbar, maybe window manager (the server has an embedded wm).
// Notify app about an event.
// Targets: taskbar, maybe window manager (the server has an embedded wm).
// wid
// ev type (notification)
// sub-event
// client wid
// client pid 
// client tid
int 
window_post_notification( 
    int wid, 
    int event_type, 
    unsigned long long1,
    unsigned long long2,
    unsigned long long3,
    unsigned long long4 )
{
// Low level routine

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

    w->ev_long3[Tail] = (unsigned long) long3; 
    w->ev_long4[Tail] = (unsigned long) long4; 

// Advance the tail index, wrapping around if necessary (circular buffer)
    w->ev_tail++;
    if (w->ev_tail >= 32){
        w->ev_tail=0;
    }

   return 0;

fail:
    return (int) -1;
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
// Low level routine

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
                    

                    // Notify kernel to wake up the target thread if it is necessary
                    wmNotifyKernel(wReceiver,8000,8000);

                    Counter++;
                }
            }
        }
    };

    return (int) Counter;
fail:
    return (int) -1;
}

// #test
// Minimize a window
// #ps: We do not have support for iconic window yet.
void minimize_window(struct gws_window_d *window)
{

// Parameter:
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

// We only minimize application windows.
    if (window->type != WT_OVERLAPPED)
        return;

// The minimized window can't receive input.
// The iconic window that belongs to the minimized window
// will be able to receive input.
// To restore this window, we need to do it via the iconic window.
    window->enabled = FALSE;

// Minimize it.
    change_window_state( window, WINDOW_STATE_MINIMIZED );

// Maybe we're still the active window,
// even minimized.
    //if (window == active_window)
    // ...

// Focus?
// Is the wwf one of our childs?
// Change the falg to 'not receiving input'.
    struct gws_window_d *wwf;
    wwf = (struct gws_window_d *) keyboard_owner;
    if ((void*) wwf != NULL)
    {
        if (wwf->magic == 1234)
        {
            if (wwf->parent == window){
                wwf->enabled = FALSE; // Can't receive input anymore.
            }
        }
    }

// Update the desktop respecting the current zorder.
    wm_update_desktop2();
}

void MinimizeAllWindows(void)
{
    register int i=0;
    struct gws_window_d *tmp;
    int wid = -1;

    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        tmp = (void*) windowList[i];
        if (tmp != NULL)
        {
            if (tmp->used == TRUE)
            {
                if (tmp->magic == 1234)
                {
                    if (tmp->type == WT_OVERLAPPED)
                    {
                        tmp->state = WINDOW_STATE_MINIMIZED;
                        //tmp->enabled = FALSE;
                    }
                }
            }
        }
    };
}

void maximize_window(struct gws_window_d *window)
{
    unsigned long l = 50;
    unsigned long t = 50;
    unsigned long w = 50;
    unsigned long h = 50;
    int Style=0;
// Local padding values 
    const unsigned long PAD_FULL_LEFT = 0; 
    const unsigned long PAD_FULL_TOP = 0; 
    const unsigned long PAD_PARTIAL = 24; // used in style 2 
    const unsigned long PAD_BOTTOM = 24; // used in style 3

// Parameter:
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

// Can't maximize root or taskbar
    if (window == __root_window)
        return;
    if (window == taskbar_window)
        return;
// We only maximize application windows
    if (window->type != WT_OVERLAPPED)
        return;

// Enable input for overlapped window
    window->enabled = TRUE;

// Maximize it
    change_window_state( window, WINDOW_STATE_MAXIMIZED );

// Initialization using the working area by default
    if (WindowManager.initialized != TRUE)
        return;
    l = WindowManager.wa.left;
    t = WindowManager.wa.top;
    w = WindowManager.wa.width;
    h = WindowManager.wa.height;

    if (MaximizationStyle.initialized != TRUE)
    {
        //MaximizationStyle.style = 1;  // full
        //MaximizationStyle.style = 2;  // partial 00
        MaximizationStyle.style = 3;  // partial 01
        MaximizationStyle.initialized = TRUE;
    }

    // Based on style
    Style = MaximizationStyle.style;
    switch (Style)
    {
        // full
        case 1:
            l = WindowManager.wa.left + PAD_FULL_LEFT;
            t = WindowManager.wa.top  + PAD_FULL_TOP;
            w = WindowManager.wa.width;
            h = WindowManager.wa.height;
            break;
        // partial 01
        case 2:
            l = (WindowManager.wa.left   + PAD_PARTIAL);
            t = (WindowManager.wa.top    + PAD_PARTIAL);
            w = (WindowManager.wa.width  - (PAD_PARTIAL * 2));
            h = (WindowManager.wa.height - (PAD_PARTIAL * 2));
            break;
        // partial 02
        case 3:
            l = (WindowManager.wa.left);
            t = (WindowManager.wa.top);
            w = (WindowManager.wa.width);
            h = (WindowManager.wa.height - PAD_BOTTOM);
            break;
        // full
        default:
            l = WindowManager.wa.left;
            t = WindowManager.wa.top;
            w = WindowManager.wa.width;
            h = WindowManager.wa.height;
            break;
    };

// --------------

    if (w == 0 || h == 0)
    {
        return;
    }

    gws_resize_window( window, (w), (h));
    gwssrv_change_window_position( window, (l), (t) );

// Local pad variables for client area calculation
    unsigned int pad_left   = METRICS_CLIENTAREA_LEFTPAD;
    unsigned int pad_top    = METRICS_CLIENTAREA_TOPPAD;
    unsigned int pad_right  = METRICS_CLIENTAREA_RIGHTPAD;
    unsigned int pad_bottom = METRICS_CLIENTAREA_BOTTOMPAD;

// The client area
    if (window->type == WT_OVERLAPPED)
    {

        // Left for the client area - (didn't change)
        //window->rcClient.left
        
        // Top for the client area - (didn't change)
        //window->rcClient.top

        // Width for the client area
        window->rcClient.width = 
            (unsigned long) ( 
            window->width - 
            (window->Border.border_size * 2) - 
            (pad_left + pad_right) 
        );

        // Height for the client area
        window->rcClient.height = 
            (unsigned long) ( 
            window->height - 
            (window->Border.border_size * 2) - 
            (pad_top + pad_bottom) - 
            window->titlebar_height 
        );
    }

//
// Redraw and display some windows:
// Root window, taskbar and the maximized window.
//

// Root
    redraw_window(__root_window,TRUE);

// Taskbar
// Send message to the app to repaint all the childs.
    redraw_window(taskbar_window,TRUE);
    window_post_message( taskbar_window->id, GWS_Paint, 0, 0 );

    // Notify kernel to wake up the target thread if it is necessary
    wmNotifyKernel(taskbar_window,8000,8000);

// Our window
// Set focus
// Redraw and show window

    set_focus(window);
    redraw_window(window,TRUE);

// Send message to the app to repaint all the childs.
    int target_wid = window->id;
    window_post_message( target_wid, GWS_Paint, 0, 0 );

    // Notify kernel to wake up the target thread if it is necessary
    wmNotifyKernel(window,8000,8000);
}

void MaximizeAllWindows(void)
{
    register int i=0;
    struct gws_window_d *tmp;
    int wid = -1;

    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        tmp = (void*) windowList[i];
        if (tmp != NULL)
        {
            if (tmp->used == TRUE)
            {
                if (tmp->magic == 1234)
                {
                    if (tmp->type == WT_OVERLAPPED)
                    {
                        tmp->state = WINDOW_STATE_MAXIMIZED;
                        //tmp->enabled = TRUE;
                    }
                }
            }
        }
    };
}

void RestoreAllWindows(void)
{
// Back to normal state

    register int i=0;
    struct gws_window_d *tmp;
    int wid = -1;

    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        tmp = (void*) windowList[i];
        if (tmp != NULL)
        {
            if (tmp->used == TRUE)
            {
                if (tmp->magic == 1234)
                {
                    if (tmp->type == WT_OVERLAPPED)
                    {
                        tmp->state = WINDOW_STATE_NORMAL;
                        //tmp->enabled = TRUE;
                    }
                }
            }
        }
    };
}

// Initialize the window support.
int window_initialize(void)
{
    register int i=0;

    // see: window.h
    windows_count = 0;

    //...
    show_fps_window = FALSE;


// Basic windows.
// At this moment we didn't create any window yet.

// Active window
    active_window = NULL;
// Input
    keyboard_owner = NULL;
    mouse_owner = NULL;
// Stack
    first_window = NULL;
    last_window = NULL;
    top_window = NULL;


// Window list
    for (i=0; i<WINDOW_COUNT_MAX; ++i){
        windowList[i] = 0;
    };

// z-order is gonna be a linked list.
    for (i=0; i<ZORDER_MAX; ++i){
        zList[i] = 0;
    };

    return 0;   //ok
}

