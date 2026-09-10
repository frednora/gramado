// winu.c
// Created by Fred Nora


#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <rtl/gramado.h>

#include "include/winu.h"

//
// =====================================================
//

unsigned long winu_get_event_buffer_base_address(void)
{
    // #ps: It depends on rtl
    unsigned long rv = (unsigned long) &RTLEventBuffer[0];

    return (unsigned long) rv;
}


unsigned long winu_get_system_message(unsigned long message_buffer)
{
    return (unsigned long) rtl_get_system_message(message_buffer);
}

unsigned long 
winu_post_system_message(
    int dest_tid, 
    unsigned long message_buffer )
{
    return (unsigned long) winu_post_system_message(dest_tid,message_buffer);
}

int winu_get_system_event(void)
{
    int Status = -1;

    Status = (int) rtl_get_event();
    return (int) Status;
}

int winu_initialize(void)
{
    printf("Hello from winu.c\n");
    return 0;
}

