

#ifndef ____DECODE_H
#define ____DECODE_H    1



// Ethernet header.
struct gnssrv_ether_header {

    uint8_t dst[6];
    uint8_t src[6];

    uint16_t type;

} __attribute__((packed));


#define gnssrv_FromNetByteOrder16(v) ((v >> 8) | (v << 8))



void 
print_ethernet_header( 
    const unsigned char *Buffer, 
    int Size );


int gnssrv_decode_buffer ( unsigned long buffer_address );

void
gnssrv_send_packet(void);

void gnssrv_get_and_decode_buffer(void);

#endif    


