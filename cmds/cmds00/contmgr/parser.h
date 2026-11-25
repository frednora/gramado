
// parser.h

#ifndef __PARSER_H
#define __PARSER_H    1

//--------------
// Elementos que explicam o identificador.
extern long id[ID_MAX];

//--------------
// Elementos que explicam a constante.
extern long constant[CONSTANT_MAX];


// Salvando a string das constantes,
// onde a constante será armazenada dependendo do tipo.
extern char constant_byte[2];   //0xFF
extern char constant_word[4];   //0xFFFF
extern char constant_dword[8];  //0xFFFFFFFF
extern char constant_qword[16];  //0xFFFFFFFFFFFFFFFF
// O que colocar antes dessa constante.
// Isso varia com a base. Ex: 0x, 0X
extern char constant_before[2];
// O que colocar depois dessa constante.
// Isso varia com a base. Ex: H, B.
extern char constant_after[2];


//--------------------
extern long return_info[8];


extern unsigned long functionList[FUNCTION_COUNT_MAX];

// #test
//#define gramc_skip_blanks(p)     {while (*(p) == ' ') { (p)++; }}
//#define gramc_skip_nonblanks(p)  {while (*(p) && *(p) != ' ') { (p)++; }}


// Espiada, olhadinha.
// #importante
// esquema
// Um símbolo vam seguido de um char.
// a ação depende dessa combinação.
// peekSymbol >>> peekChar
extern int peekSymbol;
extern int peekChar; 
 
extern char function_main_buffer[512];

// Item da expressão matemática.
struct exp_item_d
{
    int id;
    int type;
    int tree;      //left middle right
    char *name;
    struct exp_d *exp;   //a qual expressão o item pertence.	
};

// Uma expressão matemática
struct exp_d
{
    int id;
    int type;
    char *name;
    char **stmt;     // Qual statment ela pertence.
    int item_count;  // Contagem dos items da expressão matemática.
    struct exp_item_d *item_list;  //Lista de itens.
};

// Um statement
struct stmt_d
{
    int id;
    int type;
    int tree;                // Left middle right
    char *name;
    char **stmt;             // Todos os itens do statment
    int exp_count;           // Quantas expressões existem no statment.
    struct exp_d *exp_list;  // Lista de expressões
};

struct body_d
{
    int id;
// Quantos itens tem na lista.
// Cada item é um statement
    int stmt_count;
// Corpo da função (lista de statments)
    struct stmt_d *stmt_list;
};

struct header_d
{
// Header da função ( modificador0, modificador1, tipo, símbolo)
    char **h0;
// Header da função (argumentos)
    char **h1;
};

// modifier0, modifier1, type, identifier
// indexando o header da função com base 
// na lista com todos os tokens do programa.
struct function_h0
{
    int modifier0_token_index;
    int modifier1_token_index;
    int type_token_index;
    int identifier_token_index;
};

// Function
struct function_d
{
    int used;
    int magic;
// Identificador da função para organização da compilação.
    int id; 
// Indiciando elemnentos.
// modifier0, modifier1, type, identifier
    struct function_h0 *h0;
    struct header_d *header;
    struct body_d *body;

    //struct function_d *next;
};
//struct function_d  *function_main;


// Estrutura para gerenciar o programa.
struct program_d
{
    char *name;
// Todos os tokens do programa.
    char **h0;
// Quantas funções existem no programa.
    int function_count;

    struct function_d *function_list;
};

// See: main.c
extern struct program_d  program;



/* frw[i] is index in rw of the first word whose length is i. */
// Credits: GCC 0.9

static char frw[10] = { 
    0, 
    0, 
    0,    // i=2 significa que o índice 0 é a primeira palavra com 2 letras.
    2,    // i=3 significa que o índice 2 é a primeira palavra com 3 letras.
    5,    // i=4 significa que o índice 5 é a primeira palavra com 4 letras.
    13,   // i=5 significa que o índice 13 é a primeira palavra com 5 letras. 
    19,   // ...
    27, 
    29, 
    33 
};

//
// ## reserved words ##
//
  
// Obs: Indexadas pelo comprimento.  
// Credits: GCC 0.9 
static char *rw[] = { 
    "if", 
    "do", 
    "int", 
    "for", 
    "asm",
    "case", 
    "char", 
    "auto", 
    "goto", 
    "else", 
    "long", 
    "void", 
    "enum",
    "float", 
    "short", 
    "union", 
    "break", 
    "while", 
    "const",
    "double", 
    "static", 
    "extern", 
    "struct", 
    "return", 
    "sizeof", 
    "switch", 
    "signed",
    "typedef", 
    "default",
    "unsigned", 
    "continue", 
    "register", 
    "volatile" 
};


/* 
 Different name spaces for symbols.  Looking up a symbol specifies
 a namespace and ignores symbol definitions in other name spaces.

   VAR_NAMESPACE is the usual namespace.
   In C, this contains variables, function names, typedef names
   and enum type values.

   STRUCT_NAMESPACE is used in C to hold struct, union and enum type names.
   Thus, if `struct foo' is used in a C program,
   it produces a symbol named `foo' in the STRUCT_NAMESPACE.

   LABEL_NAMESPACE may be used for names of labels (for gotos);
   currently it is not used and labels are not recorded at all.
*/

/*
   For a non-global symbol allocated statically,
   the correct core address cannot be determined by the compiler.
   The compiler puts an index number into the symbol's value field.
   This index number can be matched with the "desc" field of
   an entry in the loader symbol table.
*/


/*
enum namespace
{
    UNDEF_NAMESPACE, 
    VAR_NAMESPACE,        // variable, function, typedef
    STRUCT_NAMESPACE,     // struct, union, enum
    LABEL_NAMESPACE,      // labels
};
*/

// ??
// aqui vamos acessar provavelmente um inteiro representado por essas 
// palavras.
// #importante: Referem-se as strings listadas acima.
/*
Credits: GCC 0.9
static short rtoken[] = { 
    IF, 
    DO, 
    TYPESPEC, 
    FOR, 
    ASM,
    CASE, 
    TYPESPEC, 
    SCSPEC, 
    GOTO, 
    ELSE, 
    TYPESPEC, 
    TYPESPEC, 
    ENUM,
    TYPESPEC, 
    TYPESPEC, 
    UNION, 
    BREAK, 
    WHILE, 
    TYPEMOD,
    TYPESPEC, 
    SCSPEC, 
    SCSPEC, 
    STRUCT, 
    RETURN, 
    SIZEOF, 
    SWITCH, 
    TYPESPEC,
    SCSPEC, 
    DEFAULT,
    TYPESPEC, 
    CONTINUE, 
    SCSPEC, 
    TYPEMOD 
};
*/

/*
 Credits: GCC 0.9
enum rid
{
  RID_UNUSED,
  RID_INT,
  RID_CHAR,
  RID_FLOAT,
  RID_DOUBLE,
  RID_VOID,
  RID_UNUSED1,

  RID_UNSIGNED,
  RID_SHORT,
  RID_LONG,
  RID_AUTO,
  RID_STATIC,
  RID_EXTERN,
  RID_REGISTER,
  RID_TYPEDEF,
  RID_SIGNED,
  RID_CONST,
  RID_VOLATILE,

  RID_MAX,
};
*/
//#define RID_FIRST_MODIFIER RID_UNSIGNED



/* 
 This table corresponds to rw and rtoken.
 Its element is an index in ridpointers  
 */
 
 //#define NORID (enum rid) 0
 
 //??
 // Seria identificação de palavra reservada. ???
 // isso é um array de enums.
/* 
static enum rid rid[] = { 
    NORID, 
    NORID, 
    RID_INT, 
    NORID, 
    NORID,
    NORID, 
    RID_CHAR, 
    RID_AUTO, 
    NORID, 
    NORID, 
    RID_LONG, 
    RID_VOID, 
    NORID,
    RID_FLOAT, 
    RID_SHORT, 
    NORID, 
    NORID, 
    NORID, 
    RID_CONST,
    RID_DOUBLE, 
    RID_STATIC, 
    RID_EXTERN, 
    NORID, 
    NORID, 
    NORID, 
    NORID, 
    RID_SIGNED,
    RID_TYPEDEF, 
    NORID,
    RID_UNSIGNED, 
    NORID, 
    RID_REGISTER, 
    RID_VOLATILE 
};
*/

//
// -- Prototypes --------
//

int parser_initialize(void);
int parse(int dump_output);

//int parserCreateMain( char **list );
//parse uma função.
//void parseFunction( char *s );
//int parserMain();

#endif    

