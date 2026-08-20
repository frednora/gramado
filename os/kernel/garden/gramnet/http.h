// http.h
// Gramnet HTTP implementation

#ifndef __GRAMNET_HTTP_H
#define __GRAMNET_HTTP_H    1



int 
gramnet_handle_http(
    struct connection_d *conn,
    const char *payload,
    size_t len,
    uint16_t sport, uint16_t dport);


#endif  // __GRAMNET_HTTP_H

