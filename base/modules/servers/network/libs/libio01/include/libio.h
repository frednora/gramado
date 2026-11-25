/*
 * File: libio.h
 * 
 * 2019 - Created by Fred Nora.
 */


// Retorna o valor.
unsigned char  libio_inport8  (unsigned short port);
unsigned short libio_inport16 (unsigned short port);
unsigned int   libio_inport32 (unsigned short port);

void libio_outport8  ( unsigned short port, unsigned char  value );
void libio_outport16 ( unsigned short port, unsigned short value );
void libio_outport32 ( unsigned short port, unsigned int   value );


