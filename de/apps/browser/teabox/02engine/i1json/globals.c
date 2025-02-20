
// globals.c

#include "gramcnf.h"

struct metadata_d  metadata[32];

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

// ----------------------------------------------
// ## stack support ##
// #testando 
// Isso será a pilha para os argumentos ou expressões matemáticas.
// #todo: agora o arquivo tree.c lida com pilha.
// Não precismos mais disso.
long stack[1024];
//xxx() if(((( ))))       ... tem que dar zero no final. 
int stack_count=0;
int stack_index=0; //current index.
int stack_flag=0; //aciona para sinalizar que devemos colocar o próximo elemento na pilha.


//#debug
//flag para mostrar o arquivo em assembly.
int asm_flag=0;

FILE *finput;

unsigned long infile_size=0;
char infile[INFILE_MAX_SIZE];

unsigned long outfile_size=0;
char outfile[OUTFILE_MAX_SIZE];

// #importante
// Esses dois buffers serão copiados no final do buffer outfile.
char TEXT[1024];
char DATA[1024];
char BSS[1024];

unsigned long program_header_address=0;
unsigned long program_text_address=0;
unsigned long program_data_address=0;
unsigned long program_bss_address=0;

// Arquivo intermediário
char file_i[512];
// Arquivo em assembly.
char file_s[512];

/* Length of the prefix.  */
int temp_filename_length=0;

//current tmp file.
char *temp_filename;

struct temp_file *temp_file_queue;

//-----------


// Uma pilha para todos os tokens.
char *TOKENLIST[TLIMIT];
//Essas listas são pilhas para diferentes tipos de tokens. 
char *KEYWORDLIST[TLIMIT]; 
char *IDENTIFIERLIST[TLIMIT]; 
char *CONSTANTLIST[TLIMIT]; 
char *STRINGLIST[TLIMIT]; 
char *SEPARATORLIST[TLIMIT]; 
char *SPECIALLIST[TLIMIT]; 


