// object.h 


#ifndef __OBJECT_H
#define __OBJECT_H    1


// opcode_t — What action to perform
typedef enum {
    // No operation
    OP_NOP,

    // Constants and variables
    OP_LOAD_CONST,   // push constant onto stack
    OP_LOAD_VAR,     // push variable value
    OP_STORE_VAR,    // store value into variable

    // Arithmetic
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,

    // Logic
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_EQ,           // ==
    OP_NEQ,          // !=
    OP_LT,           // <
    OP_GT,           // >
    OP_LE,           // <=
    OP_GE,           // >=

    // Control flow
    OP_JUMP,         // unconditional jump
    OP_JUMP_IF_TRUE, // conditional jump
    OP_JUMP_IF_FALSE,

    // Function calls
    OP_CALL,
    OP_RET,

    // System / special
    OP_LOG,     // debug print
    OP_HALT,    // stop execution

    OP_EOF,           // EOF. The last object
  
    // New high-level ops
    OP_VAR_TYPE,
    OP_BOX_TYPE,
    OP_BOX_BEGIN,
    OP_BOX_END,
    OP_META_TYPE,
    OP_NAME,
    OP_CONTENT,
    OP_BREAK,
    OP_EXIT

} opcode_t;


// operand_t — Extra data for the opcode
typedef enum {
    OPERAND_NONE,       // no operand needed
    OPERAND_CONST_IDX,  // index in constant pool
    OPERAND_VAR_IDX,    // index in symbol table
    OPERAND_ADDR,       // instruction address (for jumps)
    OPERAND_FUNC_IDX,   // function index
    OPERAND_IMMEDIATE,   // immediate literal value
    OPERAND_STRING
} operand_t;


struct object_d 
{
    int used;
    int magic;
    int id;

    // Raw string found by the lexer
    char token_buffer[TOKEN_BUFFER_MAX];
    size_t length;

    unsigned long value;  //#test

    // Source position
    int line;     // from LexerInfo.current_line
    int column;   // optional, for more precise errors

    keywords_t   keyword;
    token_t      token_type;  // TK_KEYWORD, TK_IDENTIFIER, etc.
    types_t      _type;       // symbol type, variable type ...
    modifiers_t  modifier;
    qualifiers_t qualifier;

    // Legacy identifier array embedded here
    long id_info[ID_MAX];      // same layout as before

//
// VM
//
    opcode_t opcode;
    operand_t operand;

    // ...
};

#endif   

