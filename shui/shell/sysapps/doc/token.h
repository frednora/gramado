
// token.h

#ifndef __TOKEN_H
#define __TOKEN_H    1

// Enumeração dos tipos de tokens.
typedef enum {

    TOKENNULL,
    TOKENEOF,
    TOKENKEYWORD,     // int while void. (reserveds)
    TOKENIDENTIFIER,  // var1, total ...
    TOKENCONSTANT,    // 10 20 0x1234 ...
    TOKENSTRING,      // "this is a string"
    TOKENOPERATOR,    // math. + - * / % | & ^ =
    TOKENSEPARATOR,   // separadores (){}[],.;:? ...
    TOKENSPECIAL,     // O resto. $ # & <= >= -= += ! && || ++ -- \" \' ...
    TOKENQUALIFIER,   // static volatile ...
    TOKENMODIFIER,    // signed unsigned ...
    TOKENTYPE,
    ARITHCOMPARE,
    EQCOMPARE,
    ASSIGN,           // =
    PLUSPLUS,
    MINUSMINUS,
    ANDAND,
    OROR,
    LSHIFT,
    RSHIFT,
    POINTSAT

}token_t;

// Enumeração dos tipos de tokens.
typedef enum {

    TNULL,
    TINT,
    TVOID,
    TCHAR,
    TSHORT,
    TLONG
    //...

}types_t;

// Enumeração dos modificadores.
typedef enum {

    MNULL,
    MSIGNED,
    MUNSIGNED
    //...

}modifiers_t;

// Enumeração dos qualificadores.
typedef enum {

    QNULL,
    QVOLATILE,
    QSTATIC
    //...

}qualifiers_t;
 
// Keywords
typedef enum {

    KWNULL,
    KWASM,
    KWGOTO,
    KWRETURN,
    KWCONTINUE,
    KWDEFAULT,
    KWCASE,
    KWSWITCH,
    KWFOR,
    KWDO,
    KWWHILE,
    KWELSE,
    KWIF,
    KWUNION,
    KWSTRUCT,
    KWENUM,
    KWSIZEOF,
    KWVOLATILE,
    KWINLINE,
    KWSTATIC,
    KWDEF,          //não faz parte da linguagem c
    KWVAR           //não faz parte da linguagem c 
    // ...

}keywords_t;


// String s passadas para o parser.
#define TOKENNULL_STRING        "N"
#define TOKENKEYWORD_STRING     "KW"   // int while void.
#define TOKENIDENTIFIER_STRING  "ID"   // var1, total ...
#define TOKENCONSTANT_STRING    "C"    // 10 20 0x1234 ...
#define TOKENSTRING_STRING      "ST"   // "this is a string"
#define TOKENOPERATOR_STRING    "OP"   // math. + - * / % | & ^
#define TOKENSEPARATOR_STRING   "SEP"  // separadores (){}[],.;:? ...
#define TOKENSPECIAL_STRING     "SP"   // O resto. " ' $ # & <= >= -= += ! && || ...


//
// -- Lists --------
// 

#define TLIMIT 80

// Uma pilha para todos os tokens.
char *TOKENLIST[TLIMIT];

//Essas listas são pilhas para diferentes tipos de tokens. 
char *KEYWORDLIST[TLIMIT]; 
char *IDENTIFIERLIST[TLIMIT]; 
char *CONSTANTLIST[TLIMIT]; 
char *STRINGLIST[TLIMIT]; 
char *SEPARATORLIST[TLIMIT]; 
char *SPECIALLIST[TLIMIT]; 
 
 

/* 
 all identifiers that are not reserved words
 and are not declared typedefs in the current block 
 */
//#define TOKEN_IDENTIFIER ?


/* 
 all identifiers that are declared typedefs in the current block.
 In some contexts, they are treated just like IDENTIFIER,
 but they can also serve as typespecs in declarations.  
 */
//#define TOKEN_TYPENAME ?


/* 
 reserved words that specify storage class.
 yylval contains an IDENTIFER_NODE which indicates which one.  
 */
//#define TOKEN_SCSPEC ?


/* 
 reserved words that specify type.
 yylval contains an IDENTIFER_NODE which indicates which one.  
 */
//#define TOKEN_TYPESPEC ?


/* 
 reserved words that modify type: "const" or "volatile".
 yylval contains an IDENTIFER_NODE which indicates which one.  
 */
//#define TOKEN_TYPEMOD ?


/* 
 character or numeric constants.
 yylval is the node for the constant.  
 */
//#define TOKEN_CONSTANT ?


/* 
 String constants in raw form.
 yylval is a STRING_CST node.  
 */
//#define TOKEN_STRING ?


/* 
 "...", used for functions with variable arglists.  
 */
//#define TOKEN_ELLIPSIS ?
 
 
 
 
 
//keywords. (palavras reservadas.)
#define TOKEN_SIZEOF "sizeof"
#define TOKEN_ENUM "enum"
#define TOKEN_SCTRUCT "struct"
#define TOKEN_UNION "union"
#define TOKEN_IF "if"
#define TOKEN_ELSE "else"
#define TOKEN_WHILE "while"
#define TOKEN_DO "do"
#define TOKEN_FOR "for"
#define TOKEN_SWITCH "switch"
#define TOKEN_CASE "case"
#define TOKEN_DEFAULT "default"

//keywords. (palavras reservadas.)
#define TOKEN_BREAK "break"
#define TOKEN_CONTINUE "continue"
#define TOKEN_RETURN "return"
#define TOKEN_GOTO "goto"
#define TOKEN_ASM "asm"
#define TOKEN_VOLATILE "volatile"
#define TOKEN_INLINE "inline"

//types
#define TOKEN_CHAR "char"
#define TOKEN_SHORT "short"
#define TOKEN_LONG "long"
#define TOKEN_SIGNED "signed"
#define TOKEN_UNSIGNED "unsigned"
#define TOKEN_BOOL "bool"
#define TOKEN_DOUBLE "double"
#define TOKEN_FLOAT "float"
#define TOKEN_INT "int"
#define TOKEN_VOID "void"
#define TOKEN_STATIC "static"
#define TOKEN_CONST "const"
#define TOKEN_DEFINE "define"
#define TOKEN_TYPEDEF "typedef"

#define TOKEN_GOTO "goto"

//...

//identifiers
#define TOKEN_MAIN "main"
//...

//constants.
#define TOKEN_ZERO   "0"
#define TOKEN_ONE    "1"
#define TOKEN_TWO    "2"
#define TOKEN_THREE  "3"
#define TOKEN_FOUR   "4"
#define TOKEN_FIVE   "5"
#define TOKEN_SIX    "6"
#define TOKEN_SEVEN  "7"
#define TOKEN_EIGHT  "8"
#define TOKEN_NINE   "9"
//...


//strings
#define TOKEN_STRING_NULL "null"
#define TOKEN_STRING_NIL  "nil"
//...

//Operators. (math)

//Arithmetic Operators
#define TOKEN_ADD "+"
#define TOKEN_SUB "-"
#define TOKEN_MUL "*"
#define TOKEN_DIV "/"
#define TOKEN_MOD "%"
#define TOKEN_INC "++"
#define TOKEN_DEC "--"
//#define TOKEN_?? "+="
//#define TOKEN_?? "-="


//Relational Operators
// == != > < >= <=
//#define TOKEN_?? "=="
//#define TOKEN_?? "!="
//#define TOKEN_?? ">"
//#define TOKEN_?? "<"
//#define TOKEN_?? ">="
//#define TOKEN_?? "<="
//...


/*
//separators.
//(){}[],.;:? 
#define TOKEN_ADD "(" Parentheses
#define TOKEN_ADD ")"
#define TOKEN_ADD "{" Braces 
#define TOKEN_ADD "}"
#define TOKEN_ADD "[" Square brackets
#define TOKEN_ADD "]"
#define TOKEN_ADD "," Comma 
#define TOKEN_ADD "." Period
#define TOKEN_ADD ";" Semicolon
#define TOKEN_ADD ":"
#define TOKEN_ADD "?"
//...
*/


/*
  keywords list
  
auto	double
int	struct
const	float
short	unsigned
break	else
long	switch
continue	
for
signed	 void
case	enum
register	typedef
default	goto
sizeof	volatile
char	extern
return	union
do	
if
static	 while  
  
*/


 
//
// ## identifier support ##
// 

// #testando 
// Informações sobre o identificador atual. 
// Encontrado no momento.

// Qual é o token, deve ser sempre TOKENIDENTIFIER.
#define ID_TOKEN 0 

// Qual é a classe do identificador, 
// se ele é uma var, string, struct, function, enum etc ...
#define ID_CLASS 1 

// Armazenará o índice na pilha onde está o ideitividador 
#define ID_STACK_OFFSET 2  

// O valor armazenado na variável.
#define ID_VALUE 3    

// O endereço de memória referente ao ideitificador. 
// Com base no endereço inicial da compilação.
#define ID_ADDRESS 4

// Qual é o tipo de dado armazenado na variável, caso seja uma variável. 
// Ou qual é o tipo de retorno da função. 
#define ID_TYPE 5    


// Qual é o indice desse identificador pra gente usar nas 
// expressões matemáticas.
#define ID_INDEX 6   

//...
#define ID_CLASS_VAR       100
#define ID_CLASS_STRING    101
#define ID_CLASS_STRUCT    102
#define ID_CLASS_FUNCTION  103
#define ID_CLASS_ENUM      104
#define ID_CLASS_LABEL     105
//...


//#obs lembrando que o identificador fica em outro buffer.


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


//#TODO RETURN SUPPORT;

#define RETURN_TOKEN 0
#define RETURN_TYPE 1


#define SYMBOLTYPE_FUNCTION           1
#define SYMBOLTYPE_FUNCTION_PROTOTYPE 2
#define SYMBOLTYPE_FUNCTION_EXTERN    3
#define SYMBOLTYPE_VARIABLE           4
#define SYMBOLTYPE_VARIABLE_EXTERN    5
#define SYMBOLTYPE_SCTRUCT            6
//...


// Variables.
struct variable_d
{
    int used;
    int magic;
};
typedef struct variable_d  variable_t;

// Symbols.
// símbolos tipados.
// funções e variáveis.

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
typedef struct symbol_d  sysmbol_t;


// Types.
struct type_d
{
    int used;
    int magic;
    int id;
    int type;
    char *string;    // String que representa o tipo.
};
typedef struct type_d  type_t;

#define FUNCTION_COUNT_MAX  32


#endif    


