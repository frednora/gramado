// serial.h
// Serial port
// Created by Fred Nora.

// see:
// https://wiki.osdev.org/Serial_Ports

#ifndef ____SERIAL_H
#define ____SERIAL_H    1


/*
COM Port  IO Port
COM1	  0x3F8
COM2	  0x2F8
COM3	  0x3E8
COM4	  0x2E8
COM5	  0x5F8
COM6	  0x4F8
COM7	  0x5E8
COM8	  0x4E8
*/

// COM ports
#define COM1_PORT  0x3F8 
#define COM2_PORT  0x2F8
#define COM3_PORT  0x3E8
#define COM4_PORT  0x2E8
// ...

#define XMTRDY  0x20
#define DLAB    0x80
#define TXR     0       // Transmit register (WRITE)
#define RXR     0       // Receive register  (READ)
#define IER     1       // Interrupt Enable
#define IIR     2       // Interrupt ID
#define FCR     2       // FIFO control
#define LCR     3       // Line control
#define MCR     4       // Modem control
#define LSR     5       // Line Status
#define MSR     6       // Modem Status
#define DLL     0       // Divisor Latch Low
#define DLH     1       // Divisor latch High

#define SERIAL_DEV_FREQ  115200

struct serial_port_d
{
    int initialized;

// #todo
//    int is_blocked;

    uint16_t port_number;
    uint16_t divisor;
    char divisorLoByte;
    char divisorHiByte;
    unsigned long baud_rate;  //115200/divisor
    unsigned long dev_freq;  //115200
    //#todo ...

    int is_faulty;

};

struct serial_port_info_d
{
    int initialized;
    struct serial_port_d com1;
    struct serial_port_d com2;
    struct serial_port_d com3;
    struct serial_port_d com4;
};
//see: serial.c
extern struct serial_port_info_d  SerialPortInfo;

//
// == prototypes ==============================================
//

void serial1_handler (void);
void serial2_handler (void);
void serial3_handler (void);
void serial4_handler (void);

char serial_read_char (unsigned int port);
void serial_write_char (unsigned int port, char data);

void serial_print (unsigned int port, char *data );

void debug_print_string ( char *data );

unsigned int serial_in(unsigned int base, int offset);
void serial_out(unsigned int base, int offset, int value);

int serial_ioctl ( int fd, unsigned long request, unsigned long arg );

// Driver initialization
int DDINIT_serial(void);

#endif    

