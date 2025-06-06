// libk.c
// Wrapper for the whole libk/ module.

#include <kernel.h>


// Called by kmain.c
int libk_initialize(void)
{
    int Status = FALSE;

// libk/ module.
// + Initialize srtuctures for the stadard streams.
// + It uses the global file table.
// + It also makes the early initialization of the consoles.
    Status = (int) kstdio_initialize();
    if (Status != TRUE){
        x_panic("libk_initialize: on kstdio_initialize()\n");
        return FALSE;
    }

// ...

    return TRUE;
}