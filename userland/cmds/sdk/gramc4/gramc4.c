
// License: GPL-2.0
// File: gramc4.c
// gramc4 is a c-like interpreter for Gramado.
// It's based on c4, Written by Robert Swierczek I guess.
// c4.c - C in four functions
// char, int, and pointer types
// if, while, return, and expression statements
// just enough features to allow self-compilation and a bit more
// Written by Robert Swierczek
// fetch, decode and execute instructions.
// #todo: Usage?

// rtl
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h> 
#include <rtl/gramado.h>


// target: i386.
// #define int long long


char *p;      // current position in source code
char *lp;     // current position in source code
char *data;   // data/bss pointer

int *e;       // current position in emitted code
int *le;      // current position in emitted code
int *id;      // currently parsed identifier
int *sym;     // symbol table (simple list of identifiers)
int tk;       // current token
int ival;     // current token value
int ty;       // current expression type
int loc;      // local variable offset

int src;      // print source and assembly flag
int debug;    // print executed instructions

// Current line number.
int line=0;


// tokens and classes (operators last and in precedence order)
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// opcodes
enum { 
    LEA, IMM, JMP, JSR, BZ, BNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PSH,
    OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV, MOD,
    OPEN, RUN, READ, CLOS, PRTF, MALC, FREE, MSET, MCMP, EXIT 
};

// types
enum { TYPE_CHAR, TYPE_INT, TYPE_PTR };

// identifier offsets (since we can't create an ident struct)
enum { Tk, Hash, Name, Class, Type, Val, HClass, HType, HVal, Idsz };


// -------------------------------------
static void expr(int lev);
static void next(void);
static void stmt(void);
// -------------------------------------


// Lexer.
static void next(void)
{
    char *pp;

// Pega e enquanto for positivo.
// Vamos compará-lo com o sobsequente.
    while (tk = *p){

    // O subsequente.
    ++p;

    // \n
    // Next line
    if (tk == '\n'){

        if (src)
        {
            printf("%d: %.*s", line, p-lp, lp ); 
            lp = p;
            while (le<e)
            {
                printf ( 
                    "%8.4s", &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
                    "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
                    "OPEN,RUN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[*++le * 5] );
                if (*le <= ADJ)
                    printf(" %d\n", *++le);
                else
                    printf("\n");
            };
        }

        ++line;
    
    // #
    // preprocessor
    }
    else if (tk == '#'){

        while (*p != 0 && *p != '\n')
            ++p;
    
    // a-z A-Z _
    // Building identifier
    }
    else if ( (tk >= 'a' && tk <= 'z') || 
              (tk >= 'A' && tk <= 'Z') || 
              tk == '_' ){

        pp = p-1;
        while ( (*p >= 'a' && *p <= 'z') || 
                (*p >= 'A' && *p <= 'Z') || 
                (*p >= '0' && *p <= '9') || 
                *p == '_' )
        {
            tk = tk * 147 + *p++;  //?
        };

        tk = (tk << 6) + (p - pp);
        id = sym;
        while (id[Tk])
        {
            if ( tk == id[Hash] && 
                 !memcmp((char *)id[Name], pp, p-pp) )
            {  
                tk = id[Tk]; 
                return;
            }
            id = id + Idsz;
        };
        id[Name] = (int) pp;
        id[Hash] = tk;
        // Anunciação.
        // O que somos.
        tk = id[Tk] = Id;
        return;

    // 0-9
    // Building a literal
    }
    else if (tk >= '0' && tk <= '9') {

        if (ival = tk - '0') { 
            while (*p >= '0' && *p <= '9') 
                ival = ival * 10 + *p++ - '0';  //? 
        } else if (*p == 'x' || *p == 'X') {
            
            while ( (
                        tk = *++p
                    ) && 
                    (
                        (tk >= '0' && tk <= '9') || 
                        (tk >= 'a' && tk <= 'f') || 
                        (tk >= 'A' && tk <= 'F') 
                    )
                  )
            {
                ival = ival * 16 + (tk & 15) + (tk >= 'A' ? 9 : 0); 
            }

        } else { 
            while (*p >= '0' && *p <= '7') 
                ival = ival * 8 + *p++ - '0'; 
        };
      tk = Num;
      return;
    
    // '/'
    }
    else if (tk == '/') {

        // Se o subsequente for um '/', então temos um comentário.
        if (*p == '/') {
            ++p; 
            // Ignora alguns subsequentes.
            while (*p != 0 && *p != '\n')
                ++p;
        // Divisão
        } else {
            tk = Div; 
            return;
        };

    // (')single  (")double 
    }
    else if (tk == '\'' || tk == '"') {

        pp = data;
        
        while (*p != 0 && *p != tk) 
        {
            if ((ival = *p++) == '\\') 
            {
                if ((ival = *p++) == 'n') 
                    ival = '\n';
            }
            
            if (tk == '"') 
                *data++ = ival;
        };
        
        ++p;
        
        if (tk == '"') 
            ival = (int) pp; 
        else 
            tk = Num;
        
        return;

    // = + - ! < > | & ^% * [ ? ~; { } ( ) ] , : 

    }
    else if (tk == '=') { 
        if (*p == '=') { ++p; tk = Eq;  } else tk = Assign; return; 
    }
    else if (tk == '+') { 
        
        // 2: mas se o subsequente for um '+', então é incremento.
        if (*p == '+') { 
            ++p; 
            tk = Inc; 
        // 1: é uma adição
        } else { 
            tk = Add; 
            return; 
        } 
    }
    else if (tk == '-') { 
        if (*p == '-') { 
            ++p; 
            tk = Dec;    //decrement
        } else { 
            tk = Sub;   // subtração
            return; 
        } 
    }
    else if (tk == '!') { 
        if (*p == '=') { 
            ++p; 
            tk = Ne;  //not equal  
        } 
        return; 
    }
    else if (tk == '<') { 
        if (*p == '=') { 
            ++p; 
            tk = Le;  
        } else if (*p == '<') { 
            ++p; 
            tk = 
            Shl; 
        } else { 
            tk = Lt;  // menor 
            return; 
        } 
    
    }
    else if (tk == '>') { 
        
        if (*p == '=') { 
            ++p; 
            tk = Ge;   // maior igual  
        } else if (*p == '>') { 
            ++p; 
            tk = Shr; // shift right
        } else { 
            tk = Gt;    // maior
            return; 
        } 
    
    }
    else if (tk == '|') { 
        if (*p == '|') { 
            ++p; 
            tk = Lor;   //logic or
        } else { 
            tk = Or;  
            return; 
        }
    }
    else if (tk == '&') { 
        if (*p == '&'){ 
            ++p; 
            tk = Lan;   //logic and
        }else{ 
            tk = And; 
            return;
        }
    }
    else if (tk == '^') { 
        tk = Xor;  
        return; 
    }
    else if (tk == '%') { 
        tk = Mod;
        return; 
    }
    else if (tk == '*') { 
        tk = Mul;  
        return; 
    }
    else if (tk == '[') { 
        tk = Brak; 
        return; 
    }
    else if (tk == '?') {
        tk = Cond; 
        return; 
    }
    else if ( tk == '~' || 
              tk == ';' || 
              tk == '{' || 
              tk == '}' || 
              tk == '(' || 
              tk == ')' || 
              tk == ']' || 
              tk == ',' || 
              tk == ':' ){
        return;
    };

    };  //while
}

// expr
static void expr(int lev)
{
    int t, *d;

  // Nothing| NUM | '"'  
  if (!tk) { printf("%d: unexpected eof in expression\n", line); exit(-1); }
  else if (tk == Num) { *++e = IMM; *++e = ival; next(); ty = TYPE_INT; }
  else if (tk == '"') {

    *++e = IMM; *++e = ival; next();
    while (tk == '"') next();
    data = (char *) ( (int) data + sizeof(int) & -sizeof(int) ); 
    ty = TYPE_PTR;


  // SIZEOF
  }
  else if (tk == Sizeof) {

    next(); if (tk == '(') next(); else { printf("%d: open paren expected in sizeof\n", line); exit(-1); }
    ty = TYPE_INT; if (tk == Int) next(); else if (tk == Char) { next(); ty = TYPE_CHAR; }
    while (tk == Mul) { next(); ty = ty + TYPE_PTR; }
    if (tk == ')') next(); else { printf("%d: close paren expected in sizeof\n", line); exit(-1); }
    *++e = IMM; *++e = (ty == TYPE_CHAR) ? sizeof(char) : sizeof(int);
    ty = TYPE_INT;


  // ID
  }
  else if (tk == Id) {

    d = id; next();
    if (tk == '(') {
      next();
      t = 0;
      while (tk != ')') { expr(Assign); *++e = PSH; ++t; if (tk == ',') next(); }
      next();
      if (d[Class] == Sys) *++e = d[Val];
      else if (d[Class] == Fun) { *++e = JSR; *++e = d[Val]; }
      else { printf("%d: bad function call\n", line); exit(-1); }
      if (t) { *++e = ADJ; *++e = t; }
      ty = d[Type];
    }
    else if (d[Class] == Num) { *++e = IMM; *++e = d[Val]; ty = TYPE_INT; }
    else {
      if (d[Class] == Loc) { *++e = LEA; *++e = loc - d[Val]; }
      else if (d[Class] == Glo) { *++e = IMM; *++e = d[Val]; }
      else { printf("%d: undefined variable\n", line); exit(-1); }
      *++e = ((ty = d[Type]) == TYPE_CHAR) ? LC : LI;
    }


  // '('
  }
  else if (tk == '(') {

    next();
    if (tk == Int || tk == Char) {
      t = (tk == Int) ? TYPE_INT : TYPE_CHAR; next();
      while (tk == Mul) { next(); t = t + TYPE_PTR; }
      if (tk == ')') next(); else { printf("%d: bad cast\n", line); exit(-1); }
      expr(Inc);
      ty = t;
    }
    else {
      expr(Assign);
      if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
    }


  // MUL
  }
  else if (tk == Mul) {

    next(); expr(Inc);
    if (ty > TYPE_INT) ty = ty - TYPE_PTR; else { printf("%d: bad dereference\n", line); exit(-1); }
    *++e = (ty == TYPE_CHAR) ? LC : LI;
  

  // AND
  }
  else if (tk == And) {

    next(); expr(Inc);
    if (*e == LC || *e == LI) --e; else { printf("%d: bad address-of\n", line); exit(-1); }
    ty = ty + TYPE_PTR;


  //  ! | ~ | ADD | SUB 
  }
  else if (tk == '!') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = 0; *++e = EQ;   ty = TYPE_INT; }
  else if (tk == '~') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = -1; *++e = XOR; ty = TYPE_INT; }
  else if (tk == Add) { next(); expr(Inc); ty = TYPE_INT; }
  else if (tk == Sub) {

    next(); *++e = IMM;
    if (tk == Num) { *++e = -ival; next(); } else { *++e = -1; *++e = PSH; expr(Inc); *++e = MUL; }
    ty = TYPE_INT;
  

  // INC | DEC
  }
  else if (tk == Inc || tk == Dec) {

    t = tk; 
    next(); expr(Inc);
    if (*e == LC)      { *e = PSH; *++e = LC; }
    else if (*e == LI) { *e = PSH; *++e = LI; }
    else { printf("%d: bad lvalue in pre-increment\n", line); exit(-1); }
    *++e = PSH;
    *++e = IMM; *++e = (ty > TYPE_PTR) ? sizeof(int) : sizeof(char);
    *++e = (t == Inc) ? ADD : SUB;
    *++e = (ty == TYPE_CHAR) ? SC : SI;
  
  //
  }
  else { 
      printf("%d: bad expression\n", line); exit(-1); 
  }


  // "precedence climbing" or "Top Down Operator Precedence" method
  while (tk >= lev)
  {

    t = ty;
    
    // ASSIGN
    if (tk == Assign) {
      
      next();
      if (*e == LC || *e == LI) *e = PSH; else { printf("%d: bad lvalue in assignment\n", line); exit(-1); }
      expr(Assign); *++e = ((ty = t) == TYPE_CHAR) ? SC : SI;
    
    
    // COND
    }
    else if (tk == Cond) {
      
      next();
      *++e = BZ; d = ++e;
      expr(Assign);
      if (tk == ':') next(); else { printf("%d: conditional missing colon\n", line); exit(-1); }
      *d = (int)(e + 3); *++e = JMP; d = ++e;
      expr(Cond);
      *d = (int)(e + 1);
    
    
    // LOR | LAN | OR | XOR | AND | EQ | NE | LT | GT | LE | GE | SHL | SHR | ADD
    }
    else if (tk == Lor) { next(); *++e = BNZ; d = ++e; expr(Lan); *d = (int)(e + 1); ty = TYPE_INT; }
    else if (tk == Lan) { next(); *++e = BZ;  d = ++e; expr(Or);  *d = (int)(e + 1); ty = TYPE_INT; }
    else if (tk == Or)  { next(); *++e = PSH; expr(Xor); *++e = OR;  ty = TYPE_INT; }
    else if (tk == Xor) { next(); *++e = PSH; expr(And); *++e = XOR; ty = TYPE_INT; }
    else if (tk == And) { next(); *++e = PSH; expr(Eq);  *++e = AND; ty = TYPE_INT; }
    else if (tk == Eq)  { next(); *++e = PSH; expr(Lt);  *++e = EQ;  ty = TYPE_INT; }
    else if (tk == Ne)  { next(); *++e = PSH; expr(Lt);  *++e = NE;  ty = TYPE_INT; }
    else if (tk == Lt)  { next(); *++e = PSH; expr(Shl); *++e = LT;  ty = TYPE_INT; }
    else if (tk == Gt)  { next(); *++e = PSH; expr(Shl); *++e = GT;  ty = TYPE_INT; }
    else if (tk == Le)  { next(); *++e = PSH; expr(Shl); *++e = LE;  ty = TYPE_INT; }
    else if (tk == Ge)  { next(); *++e = PSH; expr(Shl); *++e = GE;  ty = TYPE_INT; }
    else if (tk == Shl) { next(); *++e = PSH; expr(Add); *++e = SHL; ty = TYPE_INT; }
    else if (tk == Shr) { next(); *++e = PSH; expr(Add); *++e = SHR; ty = TYPE_INT; }
    else if (tk == Add) {
      
      next(); *++e = PSH; expr(Mul);
      if ((ty = t) > TYPE_PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL;  }
      *++e = ADD;
    
    // SUB
    }
    else if (tk == Sub) {
      
      next(); *++e = PSH; expr(Mul);
      if (t > TYPE_PTR && t == ty) { *++e = SUB; *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = DIV; ty = TYPE_INT; }
      else if ((ty = t) > TYPE_PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL; *++e = SUB; }
      else *++e = SUB;
    
    
    // MUL | DIV | MOD | INC
    }
    else if (tk == Mul) { next(); *++e = PSH; expr(Inc); *++e = MUL; ty = TYPE_INT; }
    else if (tk == Div) { next(); *++e = PSH; expr(Inc); *++e = DIV; ty = TYPE_INT; }
    else if (tk == Mod) { next(); *++e = PSH; expr(Inc); *++e = MOD; ty = TYPE_INT; }
    else if (tk == Inc || tk == Dec) {

      if (*e == LC) { *e = PSH; *++e = LC; }
      else if (*e == LI) { *e = PSH; *++e = LI; }
      else { printf("%d: bad lvalue in post-increment\n", line); exit(-1); }
      *++e = PSH; *++e = IMM; *++e = (ty > TYPE_PTR) ? sizeof(int) : sizeof(char);
      *++e = (tk == Inc) ? ADD : SUB;
      *++e = (ty == TYPE_CHAR) ? SC : SI;
      *++e = PSH; *++e = IMM; *++e = (ty > TYPE_PTR) ? sizeof(int) : sizeof(char);
      *++e = (tk == Inc) ? SUB : ADD;
      next();
    

    // BRAK
    }
    else if (tk == Brak) {
      
      next(); 
      *++e = PSH; expr(Assign);
      if (tk == ']') next(); else { printf("%d: close bracket expected\n", line); exit(-1); }
      if (t > TYPE_PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(int); *++e = MUL;  }
      else if (t < TYPE_PTR) { printf("%d: pointer type expected\n", line); exit(-1); }
      *++e = ADD;
      *++e = ((ty = t - TYPE_PTR) == TYPE_CHAR) ? LC : LI;
    
    // 
    }
    else { 
        printf("%d: compiler error tk=%d\n", line, tk); exit(-1); 
    }
  }
}


static void stmt(void)
{
    int *a, *b;

    // IF
    if (tk == If){

        next ();

        // '('        
        if (tk == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        expr(Assign);


        // ')'
        if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        *++e = BZ; 
        b = ++e;
        stmt();
        
        // ELSE
        if (tk == Else) 
        {
            *b = (int)(e + 3); 
            *++e = JMP; 
            b = ++e;
            
            next(); stmt();
        }
        
        *b = (int)(e + 1);


    // WHILE
    } else if (tk == While){

        next ();

        a = e + 1;
        
        // '('
        if (tk == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        expr(Assign);

        // ')'
        if (tk == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        *++e = BZ; 
        b = ++e;
        stmt();
        *++e = JMP; 
        *++e = (int)a;
        *b = (int)(e + 1);
  

    // RETURN
    } else if (tk == Return){

        next ();
        if (tk != ';') expr(Assign); *++e = LEV;
        if (tk == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
  

    // '{'
    } else if (tk == '{') {
        next (); while (tk != '}') stmt();
        next ();

    // ';'
    } else if (tk == ';') {
        next ();
    
    // 
    } else {

        expr(Assign);
        if (tk == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
    };
}


int main (int argc, char **argv)
{
    int fd, bt, ty, poolsz, *idmain;
    int i, *t;    // temps

//
// vm registers.
//

    //int *pc, *sp, *bp, a, cycle;    // vm registers
    int *register_pc;
    int *register_sp;
    int *register_bp;
    int register_a;
    int register_cycle;


    printf ("gramc4: Initializing ...\n");

    --argc; 
    ++argv;
    
    // -s
    if (argc > 0 && **argv == '-' && (*argv)[1] == 's') 
    { src = 1;  --argc;  ++argv; }

    // -d
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'd') 
    { debug = 1;  --argc;  ++argv; }


    if (argc < 1) 
    {  printf("usage: c4 [-s] [-d] file ...\n"); return -1;  }


    //
    // Open!
    //

    debug_print("gramc4: Open the file\n");

    if ((fd = open(*argv, 0, 0)) < 0){ 
        printf("could not open(%s)\n", *argv); return -1; 
    }

// alloc size
// arbitrary size
    // poolsz = (256*1024);    
    // poolsz = (2*1024);
    poolsz = 512;

// Buffer.
// sym, le, data, sp

    if (!(sym = malloc(poolsz))) 
    { printf("could not malloc(%d) symbol area\n", poolsz); return -1; }
    

    if (!(le = e = malloc(poolsz))) 
    { printf("could not malloc(%d) text area\n", poolsz); return -1; }
    

    if (!(data = malloc(poolsz))) 
    { printf("could not malloc(%d) data area\n", poolsz); return -1; }
    

    if (!(register_sp = malloc(poolsz))) 
    { printf("could not malloc(%d) stack area\n", poolsz); return -1; }


    memset(sym,  0, poolsz);
    memset(e,    0, poolsz);
    memset(data, 0, poolsz);

    p = "char else enum if int return sizeof while "
        "open run read close printf malloc free memset memcmp exit void main";


// add keywords to symbol table
// Do Char até o While.

    i = Char; 
    while (i <= While) 
    { 
        next(); 
        id[Tk] = i++; 
    }; 

// add library to symbol table
// Do OPEN até o EXIT.
    i = OPEN; 
    while (i <= EXIT) 
    { 
        next(); 
        
        id[Class] = Sys; 
        id[Type]  = TYPE_INT; 
        id[Val]   = i++; 
    };

//
// void main ...
//
  
// handle 'void' type
    next(); 
    id[Tk] = Char; 

// keep track of 'main'
    next(); 
    idmain = id; 

//
// malloc
//

    printf("poolsz = { %d bytes } \n",poolsz);

    if (!(lp = p = malloc(poolsz))) 
    { printf("could not malloc(%d) source area\n", poolsz); return -1; }

//
// Read!
//

    debug_print("gramc4: Read the file\n");

    if ( (i = read(fd, p, poolsz-1)) <= 0 ){ 
        printf ("gramc4: [FAIL] read() returned %d\n", i);
        printf("poolsz = { %d bytes } \n",poolsz);
        return -1; 
    }

    // Finalize.
    p[i] = 0;
    close (fd);

//
// Parse declarations
//

    line = 1;
    
    next ();

    while (tk) 
    {

       // Step 0 = type or enum

        bt = TYPE_INT;    // basetype
        
        // TYPE_INT
        if (tk == Int) { 
            next(); 
        
        // TYPE_CHAR
        }
        else if (tk == Char) { 
            next();  bt = TYPE_CHAR; 
        
        // ENUM
        // Abre e fecha o corpo.
        }
        else if (tk == Enum) {
            
            next();
            if (tk != '{') next();


            // Depois que abre, tem que fazer um while até fechar.
            // Falha se não for um symbol. 
            // (Porque tem symbol no corpo da funções e no corpo do enum)
            // Depos tem que ter uma atribuição ou separador.
                
            // '{'
            if (tk == '{') 
            {   
                next();
                i = 0;
                while (tk != '}') 
                {
                    if (tk != Id){ 
                        printf("%d: bad enum identifier %d\n", line, tk); 
                        return -1; 
                    }
          
                    next();
                
                    // ASSIGN
                    if (tk == Assign) 
                    {
                        next();
                        if (tk != Num) 
                        { printf("%d: bad enum initializer\n", line); return -1; }
                        i = ival;
                        next();
                    }
                    id[Class] = Num; 
                    id[Type]  = TYPE_INT; 
                    id[Val]   = i++;
                
                    // ','
                    if (tk == ',') next();
                }

                next();
            }
        } // terminal o enum.

        // acima foi visto tipo e enum.
        // agora é symbol.
        
        // Step 1:
        // Logo acima encontramos tipos ou enum.
        // Para o enum, abrimos e fechamos o corpo.
        // Para os tipos, vamos tratar agora.
        // Obs: Estamos fazendo declarações globais.

        // While inside the body ?
        
        while (tk != ';' && tk != '}')
        {

            ty = bt;

            // ?? Se a declaração global é um ponteiro ??
            
            // MUL
            while (tk == Mul){ 
                next(); 
                ty = ty + TYPE_PTR; 
            }

            // Se não é um symbol.
            // Deveríamos ter um symbol logo após o tipo.
            if (tk != Id) { 
                printf ("%d: bad global declaration\n", line);  
                return -1; 
            }

            // O if acima nos faz ter certeza que 
            // nesse momento temos um symbol.
            
            if (id[Class]) { 
                printf ("%d: duplicate global definition\n", line); 
                return -1; 
            }

            // Depois do symbol, abre-se a função.
            // Começando com a pilha de parâmetros. '()'.
            // Salvamos a tipagem obtida acima.            
            
            next();
            id[Type] = ty;
            
            // FUNCTION
            if (tk == '(')
            {
                id[Class] = Fun;
                id[Val] = (int)(e + 1);
                
                next(); i=0;
                
                // Vamos contruir a pilha de parâmetros
                // Até encontrarmos o fim da pilha ')'
                while (tk != ')') 
                {
                    ty = TYPE_INT;
                    
                    if (tk == Int)        next();
                    else if (tk == Char){ next (); ty = TYPE_CHAR; }

                    // Is it a pointer ?
                    while (tk == Mul){    next();  ty = ty + TYPE_PTR; }

                    // Tem que ser um symbol.
                    if (tk != Id) 
                    { printf("%d: bad parameter declaration\n", line); return -1; }
                    
                    // Tem que ser local.
                    if (id[Class] == Loc) 
                    { printf("%d: duplicate parameter definition\n", line); return -1; }
                    
                    id[HClass] = id[Class]; 
                    id[Class] = Loc;
                    id[HType] = id[Type];  
                    id[Type] = ty;
                    id[HVal] = id[Val];   
                    id[Val] = i++;
                    
                    next(); 
                    
                    // Concatenação de parâmetros.
                    if (tk == ',') next();
                }

                next();

                // Depois de contruída a pilha de parâmetros
                // devemos abrir o corpo da função.

                if (tk != '{')
                { printf ("%d: bad function definition\n", line); return -1; }
        
                loc = ++i;
                
                next();

                // Declarações de variáveis dentro do corpo da função.
                while (tk == Int || tk == Char)
                {
                    bt = (tk == Int) ? TYPE_INT : TYPE_CHAR;
                    
                    next();

                    while (tk != ';') 
                    {
                        ty = bt;

                        // pointer ??
                        while (tk == Mul) 
                        { next(); ty = ty + TYPE_PTR; }

                        // symbol
                        if (tk != Id) 
                        { printf("%d: bad local declaration\n", line); return -1; }
                        
                        // variavel local.
                        if (id[Class] == Loc) 
                        { printf("%d: duplicate local definition\n", line); return -1; }
                        
                        id[HClass] = id[Class]; id[Class] = Loc;
                        id[HType]  = id[Type];  id[Type]  = ty;
                        id[HVal]   = id[Val];   id[Val]   = ++i;
                        
                        next();
                        
                        // Concatenando declarações de variáveis locais.
                        if (tk == ',') next();
                    }
                    next();
                
                // Fim do while de declarações de variáveis 
                // dentro do corpo da função?
                }; 


                *++e = ENT; 
                *++e = i - loc;

                // Encontramos Statements dentro do corpo da função.
                // Ou seja, todas as declarações foram feitas no início do corpo
                // e se não for }, então é statement.
               
                while (tk != '}') stmt();
                
                
                *++e = LEV;
                id = sym;    // unwind symbol table locals
                
                while (id[Tk]) 
                {
                    if (id[Class] == Loc) 
                    {
                        id[Class] = id[HClass];
                        id[Type]  = id[HType];
                        id[Val]   = id[HVal];
                    }
                    id = id + Idsz;
                };


            // Não era abertura de pilha de parâmetros.
            } else {
                id[Class] = Glo;
                id[Val] = (int) data;
                data = data + sizeof(int);
            };

            // Não era abretura de pilha de parâmetros,
            // provavelmente uma sequência de declarações de variáveis.
            if (tk == ',') next ();
       
        }; //while. Finalização de declaração ou corpo.

        
        // Quando finalizarmos a declaração ou o corpo
        // então pegamos mais um token e tentamos pegar mais
        // uma declaração ou função.
        
        next();
 
    }; // while principal.


    // Step 2
    // Depois que acabaram-se as declarações ou funções
    // vamos checar se 'main' foi definida.

    if ( !( register_pc = (int *) idmain[Val] ) )
    { 
        printf ("main() not defined\n"); return -1; 
    }


    if (src){ 
        debug_print ("gramc4: src\n"); return 0; 
    }



    // Step 3
    // Setup stack

    register_bp = register_sp = (int *)((int) register_sp + poolsz);
 
    // call exit if main returns
    *--register_sp = EXIT; 
    *--register_sp = PSH; 
    t = register_sp;
    *--register_sp = argc;
    *--register_sp = (int) argv;
    *--register_sp = (int) t;



    // Step 4.

    //
    // Run...
    //
    
    int Wait_status;
    
    // ??
    // Vamos rodar o interpretador ?
    // Realizando algumas instruções.

    register_cycle = 0;

    while (1)
    {

        i = *register_pc++;

        ++register_cycle;
    
        if (debug){

            printf ("%d> %.4s", register_cycle,
                &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
                "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
                "OPEN,RUN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[i * 5]);
            
            if (i <= ADJ) printf (" %d\n", *register_pc); else printf("\n");
        }
        
        // O interpretador realiza algumas instruções.

        // load local address
        if (i == LEA) { 
            register_a = (int)(register_bp + *register_pc++); 
        
        // load global address or immediate
        } 
        else if (i == IMM) { 
            register_a = *register_pc++; 
        
        // jump
        } 
        else if (i == JMP) { 
            register_pc = (int *) *register_pc; 
        
        // jump to subroutine
        }  
        else if (i == JSR) { 
            *--register_sp = (int)(register_pc + 1); 
            register_pc = (int *) *register_pc; 
        
        
        // branch if zero
        }        
        else if (i == BZ)  { 
            register_pc = register_a ? register_pc + 1 : (int *) *register_pc; 
        
        
        // branch if not zero
        } 
        else if (i == BNZ) { 
            register_pc = register_a ? (int *) *register_pc : register_pc + 1; 
        
        // enter subroutine
        } 
        else if (i == ENT) { 
            *--register_sp = (int) register_bp; 
            register_bp = register_sp; 
            register_sp = register_sp - *register_pc++; 
        
        // stack adjust
        }     
        else if (i == ADJ) { 
            register_sp = register_sp + *register_pc++; 
        
        // leave subroutine
        }    
        else if (i == LEV) { 
            register_sp = register_bp; 
            register_bp = (int *) *register_sp++; 
            register_pc = (int *) *register_sp++; 
        
        // load int
        } 
        else if (i == LI) { 
            register_a = *(int *) register_a; 
        
        
        // load char
        } 
        else if (i == LC) { 
            register_a = *(char *) register_a; 
        
        
        // store int
        } 
        else if (i == SI) { 
            *(int *)*register_sp++ = register_a; 
        
        // store char
        }   
        else if (i == SC) { 
            register_a = *(char *)*register_sp++ = register_a; 
        
        
        // push
        } 
        else if (i == PSH) { 
            *--register_sp = register_a; 
        
        } 
        else if (i == OR) { 
            register_a = *register_sp++ |  register_a; 
        
        }
        else if (i == XOR) { 
            register_a = *register_sp++ ^  register_a; 
        
        }
        else if (i == AND) { 
            register_a = *register_sp++ &  register_a; 
        
        }
        else if (i == EQ)  { 
            register_a = *register_sp++ == register_a; 
        
        }
        else if (i == NE) { 
            register_a = *register_sp++ != register_a; 
        
        }
        else if (i == LT) { 
            register_a = *register_sp++ <  register_a; 
        
        }
        else if (i == GT) { 
            register_a = *register_sp++ >  register_a; 
        
        }
        else if (i == LE) { 
            register_a = *register_sp++ <= register_a; 
        
        }
        else if (i == GE) { 
            register_a = *register_sp++ >= register_a; 
        
        }
        else if (i == SHL) { 
            register_a = *register_sp++ << register_a; 
        
        }
        else if (i == SHR) { 
            register_a = *register_sp++ >> register_a; 
        
        }
        else if (i == ADD) { 
            register_a = *register_sp++ +  register_a; 
        
        }
        else if (i == SUB) { 
            register_a = *register_sp++ -  register_a; 
        
        }
        else if (i == MUL) { 
            register_a = *register_sp++ *  register_a; 

        }
        else if (i == DIV) { 
            register_a = *register_sp++ /  register_a; 
        
        }
        else if (i == MOD) { 
            register_a = *register_sp++ %  register_a; 

        }
        else if (i == OPEN) { 
            register_a = open ((char *)register_sp[1], *register_sp,0); 

        }
        else if (i == RUN) { 
            //a = open ((char *)sp[1], *sp,0);
            register_a = (int) sc80 (900,(unsigned long)register_sp[1],0,0); 
            waitpid (-1,&Wait_status,0); 
        
        }
        else if (i == READ) { 
            register_a = read (register_sp[2], (char *)register_sp[1], *register_sp); 
        
        }
        else if (i == CLOS) { 
            register_a = close (*register_sp); 
        
        }
        else if (i == PRTF) { 
            t = register_sp + register_pc[1]; 
            register_a = printf ((char *)t[-1], t[-2], t[-3], t[-4], t[-5], t[-6]); 
        
        }
        else if (i == MALC) { 
            register_a = (int) malloc (*register_sp); 
        
        }
        else if (i == FREE) { 
            free ((void *)*register_sp); 
        
        }
        else if (i == MSET) { 
            register_a = (int) memset ((char *)register_sp[2], register_sp[1], *register_sp); 

        }
        else if (i == MCMP) { 
            register_a = memcmp ((char *)register_sp[2], (char *)register_sp[1], *register_sp); 
        
        }
        else if (i == EXIT) { 
            printf ("exit(%d) cycle = %d\n", *register_sp, register_cycle ); 
            return *register_sp; 

        }
        else { 
            printf ("unknown instruction = %d! cycle = %d\n", 
                i, register_cycle ); 
            return -1; 
        };

    }; // while


    printf ("gramc4: done\n");
}


//
// End.
//



