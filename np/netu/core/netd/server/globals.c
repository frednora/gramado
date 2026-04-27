// globals.c
// Created by Fred Nora.

#include <gnsint.h>

// Buffer for the messages.
// This is the buffer for the requests and the responses.
// This is a backbuffer for the socket.
// see: global.h

char __buffer[GNS_BUFFER_SIZE];

// Tmp buffer for next response's metadata.
// This is the buffer used for the next response.
// Marsheling ...
// Esses valores serão enviados como resposta ao serviço atual.
// Eles são configurados pelo dialogo na hora da prestação do serviço.
// No início desse array fica o header.
// see: globals.h

unsigned long next_response[NEXTRESPONSE_BUFFER_SIZE];

