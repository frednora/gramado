// output.c

#include <kernel.h>


ssize_t ksys_console_write_string(int console_number, const char *string)
{
    ssize_t rv=0;

// Parameters:
    if (console_number < 0)
        goto fail;
    if (console_number >= CONSOLETTYS_COUNT_MAX){
        goto fail;
    }

    if ((void*) string == NULL)
        goto fail;
    if (*string == 0)
        goto fail;

    rv = 
        (ssize_t) console_write_string(console_number, string);

    return (ssize_t) rv;
fail:
    return (ssize_t) -1;
}
