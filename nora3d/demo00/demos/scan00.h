// scan00.h
// Created by Fred Nora.

#ifndef __DEMOS_SCAN00_H
#define __DEMOS_SCAN00_H    1


//
// Lexer
//

// Token types
typedef enum {

    TOKEN_NONE,

    TOKEN_VERTEX,  // 'v'
    TOKEN_FACE,    // 'f'

    TOKEN_INT,
    TOKEN_FLOAT,

    TOKEN_EOL,
    TOKEN_EOF

} token_type_t;

// Token structure without union
struct token_d 
{
    token_type_t type;

    // Dedicated fields for each possible token value
    int   int_value;
    float float_value;

    // Optional: raw lexeme string for debugging
    char  lexeme[64];
};


//
// Parser
//

#define OBJ_ELEMENT_TYPE_VECTOR  1
#define OBJ_ELEMENT_TYPE_FACE    2

#define MAX_TOKENS_PER_ELEMENT  16

// obj_element_d
// Represents a parsed element from an OBJ file.
// It can be either a vertex or a face.
// For now, only these two types are supported.
struct obj_element_d
{
    //int used;
    //int magic;
    int initialized;

    int type;                 // 0=none, 1=vertex, 2=face
    struct n3d_vec_d vertex;  // Parsed "v" line
    struct n3d_face_d face;   // Parsed "f" line

    // Static token buffer
    struct token_d token[MAX_TOKENS_PER_ELEMENT];
    int token_count;
};


// It scans a single line given the pointer
const char *scan00_read_element_from_line(
    const char *line_ptr, 
    struct obj_element_d *elem );

#endif   

