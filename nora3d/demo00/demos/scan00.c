// scan00.c
// Let's scan floating values.
// Let's scan floating values witha custom function created by Copilot.
// v 1.234 -5.678e-2 9.0123
// Created by Fred Nora.


//Lexer stage: 
// DFA-like routines that recognize atomic tokens (integers, floats).
//Parser stage: 
// CFG-like routine that interprets those tokens in context (v → vector, f → face).

// Grammar rules like
// vertex_line → 'v' FLOAT FLOAT FLOAT
// face_line   → 'f' INT INT INT


#include "../gram3d.h"

// ==============================================================


// Local, for now.
// Declaration of our custom float parser.
double __pow0000(double __x, double __y);

static int __lexer_read_int(const char **strPtr);
static float __lexer_read_float(const char **strPtr);


// ==============================================================

// Internal
double __pow0000(double __x, double __y)
{
    double RetValue = 0.0;
    asm volatile (
        "fyl2x \n"
        "fld %%st \n"
        "frndint \n"
        "fsub %%st, %%st(1) \n"
        "fxch \n"
        "fchs \n"
        "f2xm1 \n"
        "fld1 \n"
        "faddp \n"
        "fxch \n"
        "fld1 \n"
        "fscale \n"
        "fstp %%st(1) \n"
        "fmulp \n" : "=t"(RetValue) : "0"(__x),"u"(__y) : "st(1)" );

    return (double) RetValue;
}

// Lexical analysis:
// Act as lexical analyzers. It consumes characters from the input string and 
// produce tokens (integers). In compiler terms, it is “token recognizer”.
// Lexer-like routine: recognizes an integer token from the input stream.
// Equivalent to a DFA over the alphabet {0-9, '+', '-'}.
static int __lexer_read_int(const char **strPtr)
{
    const char *s = *strPtr;

//
// Skip whitespace
//

    while ( *s && 
             isspace((unsigned char)*s))
    {
        s++;
    };

//
// Optional sign
//
    int sign = 1;

    if (*s == '-'){
        sign = -1; 
        s++; 
    } else if (*s == '+'){ 
        s++; 
    }

//
// Copy digits into a small buffer
//

    char buf[32];
    int len = 0;

    while ( *s && 
            isdigit((unsigned char)*s) && 
            len < (int)(sizeof(buf)-1) ) 
    {
        buf[len++] = *s;
        s++;
    };
    buf[len] = '\0';

//
// Convert to int using atoi()
//
    int value = atoi(buf) * sign;

    // Advance pointer
    // What?
    *strPtr = s;

    return (int) value;
}

/**
 * __lexer_read_float - Parses a floating-point number from a string.
 *
 * @strPtr: A pointer to the string pointer that points to the number.
 *          The function will update this pointer to the position immediately
 *          following the parsed number.
 *
 * Returns: The floating-point number parsed from the string.
 *
 * This routine handles optional leading whitespace, an optional '+' or '-' sign,
 * the integral part followed by an optional fractional part (after a decimal point),
 * and an optional exponent prefixed with 'e' or 'E'.
 */

// Lexical analysis:
// Act as lexical analyzers. It consumes characters from the input string and 
// produce tokens (float). In compiler terms, it is “token recognizer”.
// Lexer-like routine: recognizes a floating-point token.
// Grammar fragment: <float> ::= [sign] digits [ '.' digits ] [ ('e'|'E') [sign] digits ]
static float __lexer_read_float(const char **strPtr) 
{
    const char *s = *strPtr;

//
// Skip any leading whitespace
//

    while ( isspace((unsigned char)*s) )
    {
        s++;
    };

//
// Process optional sign
//

    int sign = 1;

    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }

//
// Parse the integer part of the number
//

    float result = 0.0f;
    //double result = 0.0;  // Use double for better accuracy

    while ( *s && 
            isdigit((unsigned char)*s) ) 
    {
        result = result * 10.0f + (*s - '0');
        s++;
    };

//
// Parse the fractional part if a decimal point is present
//

    if (*s == '.') 
    {
        s++;

        float fraction = 0.0f;
        float divisor = 10.0f;

        while ( *s && 
                isdigit((unsigned char)*s) ) 
        {
            fraction += (*s - '0') / divisor;
            divisor *= 10.0f;

            s++;
        };

        result += fraction;
    }

//
// Parse exponential part if present
//

    if (*s == 'e' || *s == 'E') 
    {
        s++;

        int expSign = 1;
        if (*s == '-') {
            expSign = -1;
            s++;
        } else if (*s == '+') {
            s++;
        }

        int exponent = 0;
        while ( *s && 
                isdigit((unsigned char)*s) ) 
        {
            exponent = exponent * 10 + (*s - '0');
            s++;
        }

        result *= __pow0000(10, expSign * exponent);
        //result *= pow(10, expSign * exponent);
    }


// Update the pointer to reflect the new reading position
    *strPtr = s;

// #ps: Return float value
    return sign * result;
}

// Its real purpose is to parse a line containing a vertex definition (v x y z) and 
// return a 3D vector.
// Modified function: it now returns a pointer into the string after the newline.
// Parser routine: recognizes a vertex production.
// Grammar rule: vertex_line → 'v' float float float
// Automaton transitions: start → 'v' → float → float → float → end_of_line

const char *scan00_read_element_from_line(
    const char *line_ptr, 
    struct obj_element_d *elem )
{

// #bugbug
// The parser cant handle comments. It fails.
// Do not use comments for now!

// But there is still a small problem: 
// this code only skips the comment if the line 
// starts with # right after whitespace.

    // 'ptr' will traverse the string.
    const char *ptr = line_ptr;

    if ((void*) elem == NULL){
        return NULL;
    }

    elem->initialized = FALSE;
    elem->token_count = 0;

//
// Lexer: Skip any whitespace before the identifier
//

    while ( *ptr && 
            isspace((unsigned char)*ptr) )
    {
        ptr++;
    };

//
// Lexer: EOL (End Of Line)
//

    // If we reached the end already, return NULL.
    if (*ptr == '\0')
    {
        elem->token[elem->token_count].type = TOKEN_EOL;
        //strcpy(elem->token[elem->token_count].lexeme, "\\0");
        elem->token_count++;
        return NULL;
    }


// =======================================
// Skip comments and empty lines

/*
    while (*ptr && (isspace((unsigned char)*ptr) || *ptr == '#'))
    {
        if (*ptr == '#')
        {
            // Skip until end of line
            while (*ptr && *ptr != '\n')
                ptr++;
        }
        else
        {
            // Skip whitespace
            ptr++;
        }
    }

    if (*ptr == '\0' || *ptr == '\n')
    {
        // End of string or just newline → move to next line if possible
        if (*ptr == '\n') ptr++;
        return (*ptr != '\0') ? ptr : NULL;
    }
*/


// =======================================
// Handle comment lines ("#")
// When the line starts with comment
/*
    // #wrong
    if (*ptr == '#')
    {
        // Skip until end of line
        while (*ptr && *ptr != '\n')
            ptr++;
        if (*ptr == '\n')
            ptr++;
        if (*ptr == '\0')
            return NULL;

        // Comments are ignored, return pointer to next line
        return ptr;
    }
*/

// =======================================
// Parser: Parse the 'v' statement
// Handle vertex lines ("v")

// parse_v_statement:

    if (*ptr == 'v') 
    {
        // TOKEN_VERTEX
        elem->token[elem->token_count].type = TOKEN_VERTEX;
        strcpy( elem->token[elem->token_count].lexeme, "v");
        elem->token_count++;

        // Lexer: Skip the initial identifier ("v") if it's present.
        ptr++;

        // Lexer: Skip any whitespace after the identifier
        while (*ptr && isspace((unsigned char)*ptr))
        {
            ptr++;
        };

        // Fill the new structure instead of an external pointer
        elem->type = OBJ_ELEMENT_TYPE_VECTOR; 

        // Parse three floats and store them in the structure

        elem->vertex.x = __lexer_read_float(&ptr);
        // TOKEN_FLOAT x
        elem->token[elem->token_count].type = TOKEN_FLOAT;
        elem->token[elem->token_count].float_value = elem->vertex.x;
        // No lexeme for now
        elem->token_count++;

        elem->vertex.y = __lexer_read_float(&ptr);
        // TOKEN_FLOAT y
        elem->token[elem->token_count].type = TOKEN_FLOAT;
        elem->token[elem->token_count].float_value = elem->vertex.y;
        // No lexeme for now
        elem->token_count++;

        elem->vertex.z = __lexer_read_float(&ptr);
        // TOKEN_FLOAT z
        elem->token[elem->token_count].type = TOKEN_FLOAT;
        elem->token[elem->token_count].float_value = elem->vertex.z;
        // No lexeme for now
        elem->token_count++;

        // Debug output: print the parsed coordinates.
        //printf("Parsed Vertex Coordinates:\n");
        //printf("x = %f\n", (double)return_v->x);
        //printf("y = %f\n", (double)return_v->y);
        //printf("z = %f\n", (double)return_v->z);


        elem->vertex.color = COLOR_WHITE; // #test: default color
        elem->initialized = TRUE;

        // Lexer: Advance 'ptr' to the end of the current line.
        while (*ptr && *ptr != '\n')
        {
            ptr++;
        };

        // Lexer: If a newline is found, move one character beyond it.
        if (*ptr == '\n')
            ptr++;

        // Lexer: If we've reached the end of the string, return NULL.
        if (*ptr == '\0'){
            return NULL;
        }

        // Otherwise, return the pointer to the next line's start.
        return ptr;
    };

// =======================================
// Parser: Parse the 'f' statement
// Handle face lines ("f")

// parse_f_statement:

    if (*ptr == 'f') 
    {
        // TOKEN_FACE
        elem->token[elem->token_count].type = TOKEN_FACE;
        strcpy( elem->token[elem->token_count].lexeme, "f");
        elem->token_count++;

        // Skip the initial identifier ("f") if it's present.
        ptr++;

        // Skip any whitespace after the identifier
        while (*ptr && isspace((unsigned char)*ptr))
        {
            ptr++;
        };

        elem->type = OBJ_ELEMENT_TYPE_FACE;

        // Parse three integers

        elem->face.vi[0] = __lexer_read_int(&ptr);
        // TOKEN_INT 0
        elem->token[elem->token_count].type = TOKEN_INT;
        elem->token[elem->token_count].int_value = elem->face.vi[0];
        // No lexeme for now
        elem->token_count++;

        elem->face.vi[1] = __lexer_read_int(&ptr);
        // TOKEN_INT 1
        elem->token[elem->token_count].type = TOKEN_INT;
        elem->token[elem->token_count].int_value = elem->face.vi[1];
        // No lexeme for now
        elem->token_count++;

        elem->face.vi[2] = __lexer_read_int(&ptr);
        // TOKEN_INT 2
        elem->token[elem->token_count].type = TOKEN_INT;
        elem->token[elem->token_count].int_value = elem->face.vi[2];
        // No lexeme for now
        elem->token_count++;


        //printf ("f: %d %d %d \n",
            //elem->face.vi[0], elem->face.vi[1], elem->face.vi[2] );
        //while(1){}

        elem->initialized = TRUE;

        // Lexer: Advance to end of line
        while (*ptr && *ptr != '\n')
        {
            ptr++;
        };
        // Lexer:
        if (*ptr == '\n') 
            ptr++;
        // Lexer:
        if (*ptr == '\0') 
            return NULL;

        return ptr;
    }

// Fail
    return NULL;
}
