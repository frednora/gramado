// globals.h
// Created by Fred Nora.

#ifndef __GLOBALS_H
#define __GLOBALS_H    1

// Buffer for the messages.
// See: globals.c
#define GNS_BUFFER_SIZE  512
#define MSG_BUFFER_SIZE  512
extern char __buffer[MSG_BUFFER_SIZE];

// Buffer
// This is the buffer used for the next response.
// Marsheling ...
// Esses valores serão enviados como
// resposta ao serviço atual.
// Eles são configurados pelo dialogo na hora da
// prestação do serviço.
// No início desse array fica o header.
// see: globals.c
#define NEXTRESPONSE_BUFFER_SIZE  32
extern unsigned long next_response[NEXTRESPONSE_BUFFER_SIZE];


#endif    

