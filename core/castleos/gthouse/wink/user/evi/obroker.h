// obroker.h
// Created by Fred Nora.

#ifndef __EVI_OBROKER_H
#define __EVI_OBROKER_H    1


struct output_broker_info_d 
{
    int initialized;
    // ...
};
extern struct output_broker_info_d OutputBrokerInfo;

int obroker_initialize(void);

#endif 


