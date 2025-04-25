
// globals.h

#ifndef __GLOBALS_H
#define __GLOBALS_H    1

// Contador para não estourar a lista. 
extern int keyword_count; 
extern int identifier_count; 
extern int constant_count; 
extern int string_count; 
extern int separator_count; 
extern int special_count;

extern int maxtoken;       /* Current length of token buffer */
extern char *token_buffer; /* Pointer to token buffer */

#define MAXTOKEN  256 
extern char real_token_buffer[MAXTOKEN];

#endif    


