
// globals.c

#include "gramcnf.h"


// -- Counters --------
// Contador para não estourar a lista. 
int keyword_count=0;
int identifier_count=0; 
int constant_count=0;
int string_count=0;
int separator_count=0; 
int special_count=0;


int maxtoken=0;      // Current length of token buffer.
char *token_buffer;  // Pointer to token buffer.

char real_token_buffer[MAXTOKEN];


