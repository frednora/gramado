// obroker.c
// Event broker for output events.
// Created by Fred Nora

#include <kernel.h>

struct output_broker_info_d OutputBrokerInfo;

// Called by:
// + VirtualConsole_early_initialization() in console.c
int obroker_initialize(void)
{
    // ...
    OutputBrokerInfo.initialized = TRUE;
    return 0;
}

