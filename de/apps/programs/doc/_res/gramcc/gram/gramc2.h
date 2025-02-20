

// gramc.h


// ## ARQUIVOS PARA TESTE ##

#define STRING_CFILE "int main(){\n"\
"    return 0x1234;                      \n"\
"};                                      \n"\
"    // this is a comment ...            \n"\
"    /* This is another comment ...  */  \n"\
"\t \n \n                                \n"


// ## file support ##


FILE *finput; 

#define INFILE_SIZE 512
#define OUTFILE_SIZE 512

char infile[INFILE_SIZE];
char outfile[OUTFILE_SIZE];

unsigned long infile_size;
unsigned long outfile_size;




// #importante
// Esses dois buffers serão copiados no final do buffer outfile.

char TEXT[512];
char DATA[512];
char BSS[512];


// ## stack support ##


//#testando 
//Isso será a pilha para os argumentos ou expressões matemáticas.
//#todo: agora o arquivo tree.c lida com pilha.
//Não precismos mais disso.
char stack[512];

//#bugbug: Não deveria ser assim???
//unsigned long stack[128];

//xxx() if(((( ))))       ... tem que dar zero no final. 
int stack_count;
int stack_index;   //current index.
int stack_flag;    //aciona para sinalizar que devemos colocar o próximo elemento na pilha.
 
 
//
// ## identifier support ##
// 

//#testando 
//informações sobre o identificador atual. Encontrado no momento.
#define ID_TOKEN 0 //qual é o token, deve ser sempre TOKENIDENTIFIER
#define ID_CLASS 1 //qual é a classe do identificador, se ele é uma var, string, struct, function, enum etc ...
#define ID_STACK_OFFSET 2  //armazenará o índice na pilha onde está o ideitividador 
#define ID_VALUE 3   // o valor armazenado na variável. 
#define ID_ADDRESS 4 //o endereço de memória referente ao ideitificador.Com base no endereço inicial da compilação.
#define ID_TYPE 5    // qual é o tipo de dado armazenado na variável, caso seja uma variável. Ou qual é o tipo de retorno da função.
#define ID_INDEX 6   //qual é o indice desse identificador pra gente usar nas expressões matemáticas.

//...
#define ID_CLASS_VAR       100
#define ID_CLASS_STRING    101
#define ID_CLASS_STRUCT    102
#define ID_CLASS_FUNCTION  103
#define ID_CLASS_ENUM      104
#define ID_CLASS_LABEL     105
//...

//8 elementos que explicam o identificador.
int id[8];

//#obs lembrabdo que o identificador fica em outro buffer.

//
// ## constant support ##
// 

#define CONSTANT_TOKEN 0
#define CONSTANT_TYPE 1  //1=byte 2=word 4=dword
#define CONSTANT_BASE 2  //2=binário 8=octal 10=decimal 16=hexadecimal
#define CONSTANT_INDEX 3 //qual é o índice dessa constante pra gente usar nas expressões.
//...

#define CONSTANT_TYPE_BYTE 1
#define CONSTANT_TYPE_WORD 2
#define CONSTANT_TYPE_DWORD 4
//...

#define CONSTANT_BASE_BIN 2
#define CONSTANT_BASE_OCT 8
#define CONSTANT_BASE_DEC 10
#define CONSTANT_BASE_HEX 16

//elementos que explicam a constante.
int constant[8];


//salvando a string das constantes,
//onde a constante será armazenada dependendo do tipo.
char constant_byte[2];   //0xFF
char constant_word[4];   //0xFFFF
char constant_dword[8];  //0xFFFFFFFF

//o que colocar antes dessa constante.
//isso varia com a base
char constant_before[2];

//o que colocar depois dessa constante.
//isso varia com a base
char constant_aftes[2];


//#TODO RETURN SUPPORT;

#define RETURN_TOKEN 0
#define RETURN_TYPE 1

int return_info[8];


//
unsigned long program_header_address;
unsigned long program_text_address;
unsigned long program_data_address;
unsigned long program_bss_address;




//#debug
//flag para mostrar o arquivo em assembly.
int asm_flag;

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


#define SYMBOLTYPE_FUNCTION           1
#define SYMBOLTYPE_FUNCTION_PROTOTYPE 2
#define SYMBOLTYPE_FUNCTION_EXTERN    3
#define SYMBOLTYPE_VARIABLE           4
#define SYMBOLTYPE_VARIABLE_EXTERN    5
#define SYMBOLTYPE_SCTRUCT            6
//...


//current tmp file.
char *temp_filename;

/* Length of the prefix.  */
int temp_filename_length;

/* Define the list of temporary files to delete.  */
struct temp_file
{
    char *name;
    struct temp_file *next;
};

struct temp_file *temp_file_queue;


//variável.
typedef struct variable_d variable_t;
struct variable_d
{
    int used;
    int magic;    
};


//símbolos tipados.
//funções e variáveis.
typedef struct symbol_d sysmbol_t;
struct symbol_d
{
    int used;
    int magic;

	int id;
	
    int type;      // função ou variável ou protótipo ou extern.
    char *string;  // void, int, xxx, y ...
	
	
	//caso o simbolo seja uma função;
	struct function_d *function;
	
	//caso o símbolo seja uma variável.
	struct variable_d *variable;
};


//tipos
typedef struct type_d type_t;
struct type_d
{
    int used;
    int magic;

	int id;
	
    int type;
    char *string; // string que representa o tipo.	
};


#define FUNCTION_COUNT_MAX 32

unsigned long functionList[FUNCTION_COUNT_MAX];

int main ( int argc, char *argv[] );

int gramcInitialize ();
	
	
	
	

