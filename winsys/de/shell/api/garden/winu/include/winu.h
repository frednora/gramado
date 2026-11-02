// winu.h
// 2026 - Created by Fred Nora

#ifndef __WINU_WINU_H
#define __WINU_WINU_H    1

#include "version.h"

unsigned long winu_get_event_buffer_base_address(void);

unsigned long winu_get_system_message(unsigned long message_buffer);
unsigned long 
winu_post_system_message(
    int dest_tid, 
    unsigned long message_buffer );

int winu_get_system_event(void);
int winu_initialize(void);

#endif   

