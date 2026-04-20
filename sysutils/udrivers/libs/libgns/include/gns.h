
// gns.h
// This a client-side library for GNS, Gramado Network Server.

#define LIBGNS_VERSION_STRING  "0.1"

#include "gns_pack.h"
#include "protocol.h"

//
// prototypes
//

void *gns_system_call ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d );

int gns_get_packet( unsigned long buffer, int buffer_lenght );

void
gns_async_command ( 
    int fd, 
    unsigned long request,
    unsigned long sub_request,
    unsigned long data );

void
gns_async_command2 ( 
    int fd, 
    unsigned long request,
    unsigned long sub_request,
    unsigned long data1,
    unsigned long data2,
    unsigned long data3,
    unsigned long data4 );

int gns_hello(int fd);
void gns_yield(void);
void gns_debug_print(char *string);

