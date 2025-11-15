


//Enumeração dos tipos de tokens.
typedef enum {
	TOKENNULL,
    TOKENKEYWORD,      // int while void. (reserveds)
	TOKENIDENTIFIER,   // var1, total ...
	TOKENCONSTANT,     // 10 20 0x1234 ...
	TOKENSTRING,       // "this is a string"
	TOKENOPERATOR,     // math. + - * / % | & ^ =
	TOKENSEPARATOR,    // separadores (){}[],.;:? ...
	TOKENSPECIAL,       // O resto. $ # & <= >= -= += ! && || ++ -- \" \' ...
	//...
	TOKENSTRING,
    TOKENTYPE,
    ARITHCOMPARE,
	EQCOMPARE,
	ASSIGN,    // =
	PLUSPLUS,
	MINUSMINUS,
	ANDAND,
	OROR,
	LSHIFT,
	RSHIFT,
	POINTSAT
	
}token_t;



//Enumeração dos tipos de tokens.
typedef enum {
	TNULL,
    TINT,
	TVOID,
	TCHAR,
	TSHORT,
	TLONG
	//...
}types_t;



 

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
	KWDEF,
	KWVAR
	
	//...
}keywords_t;



//String s passadas para o parser.
#define	TOKENNULL_STRING        "N"
#define TOKENKEYWORD_STRING     "KW"   // int while void.
#define	TOKENIDENTIFIER_STRING  "ID"   // var1, total ...
#define	TOKENCONSTANT_STRING    "C"    // 10 20 0x1234 ...
#define	TOKENSTRING_STRING      "ST"   // "this is a string"
#define	TOKENOPERATOR_STRING    "OP"   // math. + - * / % | & ^
#define	TOKENSEPARATOR_STRING   "SEP"  // separadores (){}[],.;:? ...
#define	TOKENSPECIAL_STRING     "SP"   // O resto. " ' $ # & <= >= -= += ! && || ...


#define TLIMIT 80

//
// ## LISTAS ##
// 

//Uma pilha para todos os tokens.
char *TOKENLIST[TLIMIT];
 
//Essas listas são pilhas para diferentes tipos de tokens. 
char *KEYWORDLIST[TLIMIT]; 
char *IDENTIFIERLIST[TLIMIT]; 
char *CONSTANTLIST[TLIMIT]; 
char *STRINGLIST[TLIMIT]; 
char *SEPARATORLIST[TLIMIT]; 
char *SPECIALLIST[TLIMIT]; 
 

//
// ## CONTADORES ##
// 
 

//contador para não estourar a lista. 
int keyword_count; 
int identifier_count; 
int keyword_count; 
int constant_count; 
int string_count; 
int separator_count; 
int special_count;

//
// ## current ##
//

//usado pelo lexar pra saber qual lugar na lista 
//colocar o lexeme.
int current_keyword; 
int current_identifier; 
int current_keyword; 
int current_constant; 
int current_string; 
int current_separator; 
int current_special;
 
 

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
