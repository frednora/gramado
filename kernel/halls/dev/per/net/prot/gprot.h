// gprot.h
// gprot protocol support.
// Created by Fed Nora.

#ifndef __PROT_GPROT_H
#define __PROT_GPROT_H    1

int
gprot_send_udp ( 
    uint8_t target_ip[4], 
    uint8_t target_mac[6], 
    unsigned short source_port,
    unsigned short target_port,
    char *data_buffer,   // UDP payload
    size_t data_lenght );

int gprot_handle_protocol(char *data, uint16_t s_port, uint16_t d_port);

#endif   

