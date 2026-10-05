// window.c
// Window. This is a lighteight proxy for the window structure 
// that lives in the display server in the user space.
// It can accelerate some operations that uses the window structure.

#include <kernel.h>

//
// Windows
//

// List of window objects
struct WND_d *window_head;
// The window that is under the mouse cursor
struct WND_d *window_hover;
// Shell window. It is the taskbar.
struct WND_d *window_shell;
// The desktop area window. It is the background window.
struct WND_d *window_desktop;
// ...


// Add the window proxy to the list of window proxy objects.
static int __wproxy_add_to_list(struct WND_d *window);
static int __wproxy_drawframe0(struct WND_d *window, int back_or_front);


// ==============================

// Add the window proxy to the list of window proxy objects.
static int __wproxy_add_to_list(struct WND_d *window)
{
    if ((void *) window == NULL){
        goto fail;
    }
    if (window->used != TRUE || window->magic != 1234){
        goto fail;
    }

    struct WND_d *w;

// Empty list
    w = (struct WND_d *) window_head;
    if (w == NULL)
    {
        window_head = window;
        window_head->next = NULL;
        return (int) 0;
    }

    while (1)
    {
        if (w == NULL)
            break;

        if (w != NULL)
        {
            if (w->next == NULL)
            {
                w->next = window;
                window->next = NULL;
                return (int) 0;
            }
            w = w->next;
        }
    };

fail:
    return (int) -1;
};

// Check against the list of window proxy objects and 
// return the one that is under the mouse cursor.
// It also says if the mouse is over the frame/chrome or 
// the client area of the window.

// Hit test priority:
// 1) Foreground application windows.
// 2) Taskbar/shell app (if the pointer is inside its rectangle).
// 3) Desktop background (if neither of the above applies).

void wproxy_hit_test00(unsigned long x, unsigned long y)
{
    struct WND_d *hover = NULL;
    struct WND_d *w = NULL;

    unsigned long Left = 0;
    unsigned long Top = 0;
    unsigned long Right = 0;
    unsigned long Bottom = 0;

// #todo:
// Taskbar first.
// We gotta register the taskbar in kernel-side too.

// The list
    hover = NULL;

// ----------------------------------------------
// Start with the system shell. The taskbar.
    w = (struct WND_d *) window_shell;
    if (w != NULL)
    {
        if (w->magic == 1234)
        {
            // Taskbar has no frame/chrome, only client area.

            // These are the values for the frame.
            Left   = w->l;
            Top    = w->t;
            Right  = (w->l + w->w);
            Bottom = (w->t + w->h);

            if ( x >= Left && x <= Right &&
                 y >= Top  && y <= Bottom )
            {
                window_hover = w;
                w->hit_area = HIT_CLIENT;  // Inside client area
                return;
            }
        }
    }

// ----------------------------------------------
// Walk the list of window proxy objects and check against the mouse cursor.
    w = (struct WND_d *) window_head;
    while (w != NULL)
    {
        if (w->magic == 1234)
        {
            // Frame/chrome
            Left   = w->l;
            Top    = w->t;
            Right  = (w->l + w->w);
            Bottom = (w->t + w->h);

            // Check against the frame/chrome.
            if ( x >= Left && x <= Right &&
                 y >= Top  && y <= Bottom )
            {
                hover = w;

                w->hit_area = HIT_FRAME;  // Inside frame/chrome

                // Client area
                Left   = w->l + w->ca_l;
                Top    = w->t + w->ca_t;
                Right  = w->l + (w->ca_l + w->ca_w);
                Bottom = w->t + (w->ca_t + w->ca_h);

                if ( x >= Left && x <= Right &&
                     y >= Top  && y <= Bottom )
                {
                    //printk("hit: client area\n");
                    w->hit_area = HIT_CLIENT;  // Inside client area
                }

                // #debug: visual effect
                //__wproxy_drawframe0(hover, 2);
            }
        }
        w = w->next; // walk forward in the list
    };

// New hover
    //if (hover != window_hover)
        //window_hover = hover;


// After walking the list
    if (hover != NULL) {
        window_hover = hover;
    } else {

        if ((void*) window_shell != NULL)
        {
            if (window_shell->magic == 1234)
            {
                // Fallback: route to taskbar as desktop controller
                window_hover = window_shell;
                if (window_hover != NULL)
                    window_hover->hit_area = HIT_DESKTOP;
            }
        }
    };
}

// Create a window proxy object and add it into the list
struct WND_d *windowCreateObject(void)
{
    struct WND_d *w;
    int status = -1;

    w = (struct WND_d *) kmalloc(sizeof(struct WND_d));
    if ((void *) w == NULL){
        goto fail;
    }
    w->used = TRUE;
    w->magic = 1234;
    //w->has_frame = TRUE;  // By default, assume it has a frame/chrome.

    status = (int) __wproxy_add_to_list(w);
    if (status != 0){
        goto fail;
    }
    return (struct WND_d *) w;

fail:
    return NULL;
}

// The current thread has a wproxy window that is 
// the system shell. (The taskbar).
// It has no frame/chrome, only client area.
int wproxy_set_shell(tid_t tid)
{
    struct thread_d *t;
    struct WND_d *w;

// parameter:
    if (tid <0 || tid >= THREAD_COUNT_MAX)
        goto fail;
    t = (struct thread_d *) threadList[tid];
    if ((void *) t == NULL)
        goto fail;
    if (t->used != TRUE){
        goto fail;
    }
    if (t->magic != 1234){
        goto fail;
    }

// window
    w = (struct WND_d *) t->wproxy;
    if ((void *) w == NULL){
        goto fail;
    }
    if (w->used != TRUE){
        goto fail;
    }
    if (w->magic != 1234){
        goto fail;
    }

// Set the shell window proxy. The taskbar is the shell.
    window_shell = w;
    //->has_frame = FALSE;  // No frame/chrome, only client area.
    return (int) 0;

    // ...

fail:
    return (int) -1;
}

// Service 47
// Create a window proxy object and initialize it with the given parameters.
// The wproxy holds the pointer to the tid.
// But the thread do not have a pointer for this wproxy.
struct WND_d *window_create0(
    tid_t tid,
    unsigned long l, 
    unsigned long t, 
    unsigned long w, 
    unsigned long h, 
    unsigned int color)
{
    struct WND_d *wnd;

    if (tid < 0)
        return NULL;
    if (tid >= THREAD_COUNT_MAX)
        return NULL;

    wnd = windowCreateObject();
    if ((void *) wnd == NULL){
        goto fail;
    }

// Frame/chrome
    wnd->l = l;
    wnd->t = t;
    wnd->w = w;
    wnd->h = h;
    wnd->color = color;

// Client area
    wnd->ca_l = l;
    wnd->ca_t = t;
    wnd->ca_w = w;
    wnd->ca_h = h;

    wnd->owner.tid = (tid_t) tid;

    return (struct WND_d *) wnd;

fail:
    return NULL;
}

// #test: Called by AP processor for testing purpose.
void wproxy_ap_test(void)
{
    struct WND_d *w;
    int i=0;
    unsigned int Color = COLOR_RED;

    w = (struct WND_d *) window_shell;
    while (w != NULL){
        if ((void*)w != NULL)
        {
            if (w->magic == 1234)
            {
                for (i=0; i<100; i++)
                {
                    frontbuffer_draw_rectangle( 
                        w->l + i, 
                        w->t + 0, 
                        4, 
                        4, 
                        Color, 
                        0x20 
                    );
                }
            }
        }
        w = w->next;
    };
}

// #test:
// Create a data buffer for the window proxy.
// This is used for off-screen rendering.
// #todo This is a work in progress.
char *wproxy_create_data_buffer(struct WND_d *wproxy, int size)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE || wproxy->magic != 1234){
        goto fail;
    }

    wproxy->data = (char *) kmalloc(size);
    if ((void *) wproxy->data == NULL){
        goto fail;
    }
    wproxy->data_size = size;

    return (char *) wproxy->data;
fail:
    return NULL;
}

// #test:
// #todo This is a work in progress.
char *wproxy_get_data_buffer(struct WND_d *wproxy)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE || wproxy->magic != 1234){
        goto fail;
    }
    return (char *) wproxy->data;
fail:
    return NULL;
}

// Worker: Draw the window using the wproxy structure
static int __wproxy_drawframe0(struct WND_d *window, int back_or_front)
{
    if ((void *) window == NULL){
        goto fail;
    }
    if (window->used != TRUE || window->magic != 1234){
        goto fail;
    }

// #test: Provisory
// Draw it 

    unsigned long rop = 0;
    int rv;

// Return the number of changed pixels.
// 1=backbuffer
// 2=frontbuffer

    if (back_or_front == 1)
    {
        rv = 
            (int) backbuffer_draw_rectangle(
                window->l, window->t, window->w, window->h,
                window->color, rop );

        return (int) rv;
    }
    if (back_or_front == 2)
    {
        rv = 
            (int) frontbuffer_draw_rectangle(
                window->l, window->t, window->w, window->h,
                window->color, rop );

        return (int) rv;
    }

fail:
    return (int) -1;
}

// Draw
int wproxy_drawframe(struct WND_d *wproxy, int back_or_front)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE || wproxy->magic != 1234){
        goto fail;
    }
    return (int) __wproxy_drawframe0(wproxy, back_or_front);    
fail:
    return (int) -1;
}

// Redraw
int wproxy_redrawframe(struct WND_d *wproxy, int back_or_front)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    return (int) wproxy_drawframe(wproxy, back_or_front);
fail:
    return (int) -1;
}

// Is it inside the frame?
int wproxy_is_inside_frame(struct WND_d *wproxy, unsigned long x, unsigned long y)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE || wproxy->magic != 1234){
        goto fail;
    }

    // Is it inside the frame?
    if ( x >= wproxy->l && 
         x <= (wproxy->l + wproxy->w) &&
         y >= wproxy->t &&
         y <= (wproxy->t + wproxy->h) )
    {
        return TRUE;
    }
fail:
    return FALSE;
}

// Is it inside the client area?
int 
wproxy_is_inside_client_area(
    struct WND_d *wproxy, 
    unsigned long x, 
    unsigned long y )
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE || wproxy->magic != 1234){
        goto fail;
    }

    // Is it inside the client area?
    if ( x >= wproxy->ca_l && 
         x <= (wproxy->ca_l + wproxy->ca_w) &&
         y >= wproxy->ca_t &&
         y <= (wproxy->ca_t + wproxy->ca_h) )
    {
        return TRUE;
    }
    return FALSE;
fail:
    return FALSE;
}

// Update the values for wproxy given the owner's tid
void 
window_set_parameters_given_tid(
    tid_t tid, 
    unsigned long l, 
    unsigned long t,
    unsigned long w,
    unsigned long h,
    unsigned long ca_l, 
    unsigned long ca_t,
    unsigned long ca_w,
    unsigned long ca_h )
{
    struct thread_d *target_thread;

    if (tid < 0)
        return;
    if (tid >= THREAD_COUNT_MAX)
        return;
    target_thread = (struct thread_d *) threadList[tid];
    if ((void *) target_thread == NULL)
        return;
    if (target_thread->used != TRUE || target_thread->magic != 1234)
        return;

// Get the window that belongs to the cureground thread
    struct WND_d *wnd;
    wnd = (struct WND_d *) target_thread->wproxy;
    if ((void *) wnd == NULL)
        return;
    if (wnd->used != TRUE || wnd->magic != 1234)
        return;

// Change the color
    // wnd->color = COLOR_WHITE;

// Change values
    wnd->l = l;
    wnd->t = t;
    wnd->w = w;
    wnd->h = h;

    wnd->ca_l = ca_l;
    wnd->ca_t = ca_t;
    wnd->ca_w = ca_w;
    wnd->ca_h = ca_h;

    // #todo: Client area?
}


