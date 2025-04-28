#line 1 "testfiles/t.y"

	#include <ctype.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	int lex(void);

#line 10 "testfiles/t.y"
typedef union {
	int sum;
	int mul;
	int num;
} yyunion;
#define YYSTYPE yyunion
short yyini = 0;
short yyntoks = 8;
short yyr1[] = {
   2,   1,   3,   3,   3,   2,   3,   0,   2
};
short yyr2[] = {
   0,   1,   1,   1,   1,   1,   1,   2,   2
};
short yyadef[] = {
   7,  -1,   0,   1,   2,   3,   4,   5,  -1,   8,
  -1,  -1,  -1,  -1,  -1,   6
};
short yygdef[] = {
  -1,   9,   1
};
short yyadsp[] = {
  -8,   0,  -8,  -8,  -2,  -2,   1,  -8,   5,  11,
  15,  15,  15,  15,  15,  -8
};
short yygdsp[] = {
 -16,  12, -16
};
short yyact[] = {
   2,   3,  12,  13,  -1,  -1,  14,  10,  11,  12,
  -1,  -1,  15,  10,  11,  12,   3,  -1,  13,  -1,
  -1,  14,   4,   5,   6,   7,   8
};
short yychk[] = {
   0,   1,   4,   3,  -1,   4,   6,   2,   3,   4,
  -1,  -1,   7,   2,   3,   4,   1,  -1,   3,  -1,
  -1,   6,   9,   9,   9,   9,   9
};
#define NUM 128
#define UMIN 129
short yytrns[] = {
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   6,   7,   4,   2,   0,   3,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   1,   5
};

#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval;

int
yyparse()
{
	enum {
		StackSize = 100,
		ActSz = sizeof yyact / sizeof yyact[0],
	};
	struct {
		YYSTYPE val;
		int state;
	} stk[StackSize], *ps;
	int r, h, n, s, tk;
	YYSTYPE yyval;

	ps = stk;
	ps->state = s = yyini;
	tk = -1;
loop:
	n = yyadsp[s];
	if (tk < 0 && n > -yyntoks)
		tk = yytrns[yylex()];
	n += tk;
	if (n < 0 || n >= ActSz || yychk[n] != tk) {
		r = yyadef[s];
		if (r < 0)
			return -1;
		goto reduce;
	}
	n = yyact[n];
	if (n == -1)
		return -1;
	if (n < 0) {
		r = - (n+2);
		goto reduce;
	}
	tk = -1;
	yyval = yylval;
stack:
	ps++;
	if (ps-stk >= StackSize)
		return -2;
	s = n;
	ps->state = s;
	ps->val = yyval;
	goto loop;
reduce:
	ps -= yyr1[r];
	h = yyr2[r];
	s = ps->state;
	n = yygdsp[h] + s;
	if (n < 0 || n >= ActSz || yychk[n] != yyntoks+h)
		n = yygdef[h];
	else
		n = yyact[n];
	switch (r) {
	case 0:
#line 0 "testfiles/t.y"
		yyval = ps[1].val; return 0;
		break;
	case 1:
#line 29 "testfiles/t.y"
{ yyval.sum = ps[1].val.num; }
		break;
	case 2:
#line 30 "testfiles/t.y"
{ yyval.sum = ps[1].val.sum + ps[3].val.sum; }
		break;
	case 3:
#line 31 "testfiles/t.y"
{ yyval.sum = ps[1].val.sum - ps[3].val.sum; }
		break;
	case 4:
#line 32 "testfiles/t.y"
{ yyval.sum = ps[1].val.sum * ps[3].val.sum; }
		break;
	case 5:
#line 33 "testfiles/t.y"
{ yyval.sum = -ps[2].val.sum; }
		break;
	case 6:
#line 34 "testfiles/t.y"
{ yyval.sum = ps[2].val.sum; }
		break;
	case 7:
#line 0 "testfiles/t.y"
		break;
	case 8:
#line 26 "testfiles/t.y"
{ printf("-> %d\n", ps[2].val.sum); }
		break;
	}
	goto stack;
}
#line 37 "testfiles/t.y"


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
