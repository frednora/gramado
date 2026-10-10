%{
	#include <ctype.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	int lex(void);
%}

%union {
	int sum;
	int mul;
	int num;
}

%type <sum> A
%token <num> NUM

%left '+' '-'
%nonassoc '*'
%nonassoc UMIN

%%

S:
 | S A { printf("-> %d\n", $2); }
 ;

A: NUM              { $$ = $1; }
 | A '+' A          { $$ = $1 + $3; }
 | A '-' A          { $$ = $1 - $3; }
 | A '*' A          { $$ = $1 * $3; }
 | '-' A %prec UMIN { $$ = -$2; }
 | '(' A ')'        { $$ = $2; }
 ;

%%

enum {
	MaxLine = 1000,
};

char line[MaxLine], *p;

int
yyerror()
{
	puts("oops");
}

int
yylex()
{
	char c;

	p += strspn(p, "\t ");
	switch ((c=*p++)) {
	case '+':
	case '-':
	case '*':
	case '(':
	case ')':
		return c;
	case 0:
	case '\n':
		p--;
		return 0;
	}
	if (isdigit(c)) {
		yylval.num = strtol(p-1, &p, 0);
		return NUM;
	}
	puts("lex error!");
	return 0;
}

int
main()
{
	while ((p=fgets(line, MaxLine, stdin))) {
		if (yyparse() < 0)
			puts("parse error!");
	}
	return 0;
}
