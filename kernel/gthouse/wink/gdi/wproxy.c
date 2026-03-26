// wproxy.h
// Window proxy. This is a lighteight proxy for the window structure 
// that lives in the display server in the user space.
// It can accelerate some operations that uses the window structure.

#include <kernel.h>

static int __wproxy_draw0(struct wproxy_d *wproxy, int back_or_front);


// ==============================

// Create a window proxy object.
struct wproxy_d *wproxyCreateObject(void)
{
    struct wproxy_d *wproxy;

    wproxy = (struct wproxy_d *) kmalloc (sizeof(struct wproxy_d));
    if ((void *) wproxy == NULL){
        return NULL;
    }

    wproxy->used = TRUE;
    wproxy->magic = 1234;

    return (struct wproxy_d *) wproxy;
}

// Worker: Draw the window using the wproxy structure.
static int __wproxy_draw0(struct wproxy_d *wproxy, int back_or_front)
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
int wproxy_draw(struct wproxy_d *wproxy, int back_or_front)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    if (wproxy->used != TRUE || wproxy->magic != 1234){
        goto fail;
    }
    return (int) __wproxy_draw0(wproxy, back_or_front);    
fail:
    return (int) -1;
}

// Redraw
int wproxy_redraw(struct wproxy_d *wproxy, int back_or_front)
{
    if ((void *) wproxy == NULL){
        goto fail;
    }
    return (int) wproxy_draw(wproxy, back_or_front);
fail:
    return (int) -1;
}

