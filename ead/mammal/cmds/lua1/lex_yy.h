// lex_yy.h
// Lua 1.0
// Created by Fred Nora.

#ifndef __LEX_YY_H
#define __LEX_YY_H    1

extern FILE *yyin;
extern FILE *yyout;

// #test
// Default io files
//extern FILE *__lua_stdin;
//extern FILE *__lua_stdout;

//
// == Prototypes ===================
//

int yylex(void);
int yylook(void);
int yyback(int *p, int m);
int yyinput(void);
void yyoutput(int c);
void yyunput(int c);

#endif  

