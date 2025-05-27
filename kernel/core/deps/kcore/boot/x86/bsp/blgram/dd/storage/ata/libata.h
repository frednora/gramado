// libata.h
// Created by Fred Nora.

#ifndef __DD_HDD_H
#define __DD_HDD_H    1

// Operation codes for internal use.
#define __OPERATION_PIO_READ  1000
#define __OPERATION_PIO_WRITE  2000

// --------------------------


// Wrapper
int 
libata_pio_rw_sector ( 
    unsigned long buffer, 
    unsigned long lba, 
    int port_index,     
    int slave );

void 
ata_read_hd_sector ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx ); 

void 
ata_write_sector ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx ); 

#endif   
