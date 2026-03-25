// wproxy.h
// Window proxy. This is a lighteight proxy for the window structure 
// that lives in the display server in the user space.
// It can accelerate some operations that uses the window structure.

#include <kernel.h>

int wproxyddddummmy;

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

