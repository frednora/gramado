
// gdef.h

#ifndef __GDEF_H
#define __GDEF_H    1

#define LSH_TOK_DELIM " \t\r\n\a" 
#define SPACE " "
#define TOKENLIST_MAX_DEFAULT 80

// ## ARQUIVOS PARA TESTE ##
#define  STRING_CFILE  "int main(){\n"        \
"    return 0x1234;                      \n"  \
"};                                      \n"  \
"    // this is a comment ...            \n"  \
"    /* This is another comment ...  */  \n"  \
"\t \n \n                                \n"

//#debug
//flag para mostrar o arquivo em assembly.
int asm_flag;

// ## file support ##

FILE *finput; 

#define INFILE_SIZE 512
#define OUTFILE_SIZE 512

unsigned long infile_size;
unsigned long outfile_size;

char infile[INFILE_SIZE];
char outfile[OUTFILE_SIZE];

// #importante
// Esses dois buffers serão copiados no final do buffer outfile.

char TEXT[1024];
char DATA[1024];
char BSS[1024];


// ## stack support ##

// #testando 
// Isso será a pilha para os argumentos ou expressões matemáticas.
// #todo: agora o arquivo tree.c lida com pilha.
// Não precismos mais disso.
char stack[1024];


// #bugbug: 
// Não deveria ser assim ???
// unsigned long stack[128];

//xxx() if(((( ))))       ... tem que dar zero no final. 
int stack_count;
int stack_index;   //current index.
int stack_flag;    //aciona para sinalizar que devemos colocar o próximo elemento na pilha.

//
unsigned long program_header_address;
unsigned long program_text_address;
unsigned long program_data_address;
unsigned long program_bss_address;

//criar um simbolo
//ele pode ser de uma variável ou uma função...
//ele terá caracterrísticas

//
// # defines usados para simular arquivos #
//

// Arquivo em C.
//#define file_c "void main () { }"
//#define file_c "void main(){ return; }\t"
//#define file_c "void main(){ return; }\t"
//#define file_c "int main(){ return 1234; }\t"
//...

// Arquivo intermediário,
char file_i[512];
// Arquivo em assembly.
char file_s[512];

/* Length of the prefix.  */
int temp_filename_length;

//current tmp file.
char *temp_filename;

/* Define the list of temporary files to delete.  */
struct temp_file
{
    char *name;
    struct temp_file *next;
};

struct temp_file *temp_file_queue;

#endif    


