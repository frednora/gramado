// lexer.h
// Created by Fred Nora

#ifndef __COMMON_LEXER_H
#define __COMMON_LEXER_H    1


// 'EOF': token that represents end-of-file
#define ENDFILE  -1  


#define lexer_isalnum(char) \
    ((char >= 'a' && char <= 'z') || \
    (char >= 'A' && char <= 'Z')  || \
    (char >= '0' && char <= '9'))

// #define isdigit(char) (char >= '0' && char <= '9')



#endif   

