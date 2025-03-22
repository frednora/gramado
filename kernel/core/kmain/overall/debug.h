// debug.h
// Created by Fred Nora.

#ifndef __GRAMADO_DEBUG_H 
#define __GRAMADO_DEBUG_H    1

// Serial debug support.
int is_using_serial_debug(void);
void enable_serial_debug(void);
void disable_serial_debug(void);

//
// Verbose via serial.
//

void debug_print (const char *data);
void PROGRESS (const char *string);

int debug(void);
void debug_breakpoint (void);
unsigned long 
debug_compute_checksum ( 
    unsigned char *buffer, 
    unsigned long lenght );

#endif    

