// wproxy.h
// Window proxy. This is a lighteight proxy for the window structure 
// that lives in the display server in the user space.
// It can accelerate some operations that uses the window structure.

#include <kernel.h>


// List of window proxy objects.
struct wproxy_d *wproxy_head;
// The window proxy that is under the mouse cursor.
struct wproxy_d *wproxy_hover;
// Shell window proxy. It is the taskbar.
struct wproxy_d *wproxy_shell;
// ...


// Add the window proxy to the list of window proxy objects.
static int __wproxy_add_to_list(struct wproxy_d *wproxy);
static int __wproxy_drawframe0(struct wproxy_d *wproxy, int back_or_front);


// ==============================

// Add the window proxy to the list of window proxy objects.
static int __wproxy_add_to_list(struct wproxy_d *wproxy)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE || wproxy->magic != 1234){
        goto fail;
    }

    struct wproxy_d *w;

// Empty list
    w = (struct wproxy_d *) wproxy_head;
    if (w == NULL)
    {
        wproxy_head = wproxy;
        wproxy_head->next = NULL;
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
                w->next = wproxy;
                wproxy->next = NULL;
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
void wproxy_hit_test00(unsigned long x, unsigned long y)
{
    struct wproxy_d *hover = NULL;
    struct wproxy_d *w = NULL;

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
    w = (struct wproxy_d *) wproxy_shell;
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
                wproxy_hover = w;
                w->hit_area = HIT_CLIENT;  // Inside client area
                return;
            }
        }
    }

// ----------------------------------------------
// Walk the list of window proxy objects and check against the mouse cursor.
    w = (struct wproxy_d *) wproxy_head;
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
                    if (w->expanded_nonclient_area == TRUE)
                        w->hit_area = HIT_FRAME;  // Inside frame/chrome 
                }

                // #debug: visual effect
                //__wproxy_drawframe0(hover, 2);
            }
        }
        w = w->next; // walk forward in the list
    };

// New hover
    if (hover != wproxy_hover)
        wproxy_hover = hover;
}


// Create a window proxy object and add it into the list.
struct wproxy_d *wproxyCreateObject(void)
{
    struct wproxy_d *wproxy;
    int status = -1;

    wproxy = (struct wproxy_d *) kmalloc (sizeof(struct wproxy_d));
    if ((void *) wproxy == NULL){
        goto fail;
    }
    wproxy->used = TRUE;
    wproxy->magic = 1234;
    wproxy->has_frame = TRUE;  // By default, assume it has a frame/chrome.

    status = (int) __wproxy_add_to_list(wproxy);
    if (status != 0){
        goto fail;
    }
    return (struct wproxy_d *) wproxy;
fail:
    return NULL;
}

// The current thread has a wproxy window that is 
// the system shell. (The taskbar).
// It has no frame/chrome, only client area.
int wproxy_set_shell(tid_t tid)
{
    struct thread_d *t;
    struct wproxy_d *wproxy;

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

// wproxy
    wproxy = (struct wproxy_d *) t->wproxy;
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE){
        goto fail;
    }
    if (wproxy->magic != 1234){
        goto fail;
    }

// Set the shell window proxy. The taskbar is the shell.
    wproxy_shell = wproxy;
    wproxy_shell->has_frame = FALSE;  // No frame/chrome, only client area.
    return (int) 0;

    // ...

fail:
    return (int) -1;
}

// Expanded non-client area.
// When set, the app does not receive mouse events.
// All mouse events (even inside the client area) are sent to the display server
// for hit-testing. This preserves the old "server authority" style of apps,
// similar to X11, but can be disabled for modern client-side drawing.

int wproxy_set_expanded_nonclient_area(tid_t tid)
{
    struct thread_d *t;
    struct wproxy_d *wproxy;

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

// wproxy
    wproxy = (struct wproxy_d *) t->wproxy;
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE){
        goto fail;
    }
    if (wproxy->magic != 1234){
        goto fail;
    }

// Expand the non-client area.
    wproxy->expanded_nonclient_area = TRUE;
    return (int) 0;

fail:
    return (int) -1;
}


// Create a window proxy object and initialize it with the given parameters.
struct wproxy_d *wproxy_create0(
    tid_t tid,
    unsigned long l, 
    unsigned long t, 
    unsigned long w, 
    unsigned long h, 
    unsigned int color)
{
    struct wproxy_d *wproxy;

    if (tid < 0)
        return NULL;
    if (tid >= THREAD_COUNT_MAX)
        return NULL;

    wproxy = wproxyCreateObject();
    if ((void *) wproxy == NULL){
        goto fail;
    }

// Frame/chrome
    wproxy->l = l;
    wproxy->t = t;
    wproxy->w = w;
    wproxy->h = h;
    wproxy->color = color;

// Client area
    wproxy->ca_l = l;
    wproxy->ca_t = t;
    wproxy->ca_w = w;
    wproxy->ca_h = h;

    wproxy->tid = (tid_t) tid;

    return (struct wproxy_d *) wproxy;
fail:
    return NULL;
}

// #test:
// Create a data buffer for the window proxy.
// This is used for off-screen rendering.
// #todo This is a work in progress.
char *wproxy_create_data_buffer(struct wproxy_d *wproxy, int size)
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
char *wproxy_get_data_buffer(struct wproxy_d *wproxy)
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


// Worker: Draw the window using the wproxy structure.
static int __wproxy_drawframe0(struct wproxy_d *wproxy, int back_or_front)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE || wproxy->magic != 1234){
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
                wproxy->l, wproxy->t, wproxy->w, wproxy->h,
                wproxy->color, rop );

        return (int) rv;
    }
    if (back_or_front == 2)
    {
        rv = 
            (int) frontbuffer_draw_rectangle(
                wproxy->l, wproxy->t, wproxy->w, wproxy->h,
                wproxy->color, rop );

        return (int) rv;
    }

fail:
    return (int) -1;
}

// Draw
int wproxy_drawframe(struct wproxy_d *wproxy, int back_or_front)
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
int wproxy_redrawframe(struct wproxy_d *wproxy, int back_or_front)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    return (int) wproxy_drawframe(wproxy, back_or_front);
fail:
    return (int) -1;
}

// Is it inside the frame?
int wproxy_is_inside_frame(struct wproxy_d *wproxy, unsigned long x, unsigned long y)
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
    struct wproxy_d *wproxy, 
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

void wproxy_test0(unsigned long x, unsigned long y)
{
    struct wproxy_d *wproxy;
    tid_t TargetTID = 0;

    wproxy = 
    (struct wproxy_d *) wproxy_create0(
        TargetTID, x, y, 40, 40, COLOR_PINK );

    if ((void *) wproxy == NULL)
        return;

    // Draw it in the front buffer
    wproxy_drawframe(wproxy, 2);
}

// #test
// Lets use the information in the fg thread structure
// to draw a rectangle.
void wproxy_test2(unsigned long x, unsigned long y)
{
    struct thread_d *t;

    if (foreground_thread < 0)
        return;
    if (foreground_thread >= THREAD_COUNT_MAX)
        return;
    t = (struct thread_d *) threadList[foreground_thread];
    if ((void *) t == NULL)
        return;
    if (t->used != TRUE || t->magic != 1234)
        return;

// Get the wproxy that belongs to the cureground thread
    struct wproxy_d *wproxy;
    wproxy = (struct wproxy_d *) t->wproxy;
    if ((void *) wproxy == NULL)
        return;
    if (wproxy->used != TRUE || wproxy->magic != 1234)
        return;

// Change the color
    // wproxy->color = COLOR_WHITE;

// Change the position
    wproxy->l = x;
    wproxy->t = y;

    // draw it in the front buffer
    wproxy_drawframe(wproxy, 2);
}

// Update the values for wproxy given the owner's tid.
void 
wproxy_set_parameters_given_tid(
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

// Get the wproxy that belongs to the cureground thread
    struct wproxy_d *wproxy;
    wproxy = (struct wproxy_d *) target_thread->wproxy;
    if ((void *) wproxy == NULL)
        return;
    if (wproxy->used != TRUE || wproxy->magic != 1234)
        return;

// Change the color
    // wproxy->color = COLOR_WHITE;

// Change values
    wproxy->l = l;
    wproxy->t = t;
    wproxy->w = w;
    wproxy->h = h;

    wproxy->ca_l = ca_l;
    wproxy->ca_t = ca_t;
    wproxy->ca_w = ca_w;
    wproxy->ca_h = ca_h;

    // #todo: Client area?
}


