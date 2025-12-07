// obroker.c
// Event broker for output events.

#include <kernel.h>

struct output_broker_info_d OutputBrokerInfo;

int obroker_initialize(void)
{
    OutputBrokerInfo.initialized = TRUE;
    return 0;
}



