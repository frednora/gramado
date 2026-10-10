// tree.c 
// bst - Binary Search Tree
// Evaluation an expression.
// Document created by Fred Nora.
// Credits: I don't know who is the original author.


#include "gramcnf.h"

// Holds tokens of the expression in order (numbers/operators)
// #expressão em ordem!
// Os tokens serão colocados aqui como uma expressão em ordem.
static long exp_buffer[32];        // was int
static int  exp_type_buffer[32];   // stays int — it only ever holds DT_DIGIT/DT_OPERATOR

// Current position in exp_buffer
int exp_offset=0;

//====================================================================
// Buffer pra fazer conta usando 'pos order'.
static int DT_BUFFER[32];   // Data type buffer (digit/operator/invalid)
static int POS_BUFFER[32];  // Post-order buffer used for evaluation
int buffer_offset = 0;      // Position in POS_BUFFER
//====================================================================


// ==============================================
// Prototypes for internal workers.


static int my_isdigit(char ch);
static void exibirEmOrdem (struct node_d *node);
static void exibirPreOrdem(struct node_d *node);
static void exibirPosOrdemAndInclude (struct node_d *node);

static void inorder(struct node_d *root);
static struct node_d *newNode(int data_type, int data);
static struct node_d *insert( struct node_d* node, int data_type, int data );
static void push( struct stack_d *s, long x );
static long pop (struct stack_d *s);
static long oper(char c, long opnd1, long opnd2);
static int is_high_precedence(char op);
static long __eval_linear(void);
static int bst_initialize(void);
static void treeInitializeGlobals(void);

// ==============================================

static int my_isdigit(char ch)
{
    return ( ch >= '0' && ch <= '9' );
}

// Creating a new node.
// A utility function to create a new BST node.
// IN: The value we're gonna store into the 'key'.
// OUT: The pointer for the structure of a node.
static struct node_d *newNode(int data_type, int data)
{
    struct node_d *tmp;

// #malloc
    tmp = (struct node_d *) malloc( sizeof(struct node_d) );
    if ((void*) tmp == NULL){
        return NULL;
    }
    tmp->_datatype = (int) data_type;
    tmp->_data = (int) data; 
    tmp->left = NULL;
    tmp->right = NULL;

    return (struct node_d *) tmp;
}

// A utility function to do inorder traversal of BST.
// IN: root node?
static void inorder(struct node_d *root)
{
    if ((void*) root == NULL)
        return;

// #todo: Explain it.
    inorder(root->left); 
    printf("%d \n", root->_data); 
// #todo: Explain it.
    inorder(root->right); 
}

// # same as 'inorder()'
// Em ordem  a+b.
// Desce at� o �ltimo pela esquerda, 
// n�o havendo esquerda vai pra direita.
// Visita a esquerda do pr�ximo
// s� retorna no �ltimo ent�o printf funciona 
// mostrando o conte�do do �ltimo 
// ai visita a direita do �ltimo e desce pela esquerda,
// n�o havendo esquerda vai pra direita.
// IN: root node?
static void exibirEmOrdem (struct node_d *node)
{
    if ((void*) node == NULL)
        return;

    //#debug
    printf("%d %d\n", node->_datatype, node->_data);

    exibirEmOrdem (node->left);
    exibirEmOrdem (node->right);
}

// Pr�-ordem +ab.
// Imprime o conte�do
// desce at� o �ltimo pela esquerda
// visita a direita e desce at� o �ltimo pela esquerda.
// IN: root node?
static void exibirPreOrdem(struct node_d *node)
{
    if ((void*) node == NULL)
        return;

    //#debug
    printf("%d %d\n", node->_datatype, node->_data);

    exibirPreOrdem(node->left);
    exibirPreOrdem(node->right);
}

// P�s-ordem ab+.
// #importante
// Exibe em n�veis. de baixo para cima.
// desce at� o ultimo pela esquerda
// visita o da direita e imprime;
static void exibirPosOrdemAndInclude (struct node_d *node)
{
// Include into POS_BUFFER[] 
// this buffer is used by eval.

    if ((void*) node == NULL)
        return;

    //?? # what is that?
    if ( buffer_offset < 0 || buffer_offset >= 32 )
    {
        printf("exibirPosOrdemAndInclude: buffer_offset\n");
        return;
    }

// #importante
// Vai colocando num buffer pra usar no c�lculo 
// isso simula uma digita��o
// eval() vai usar esse buffer

    //#debug
    //printf("%d %d\n", node->_datatype, node->_data);

     DT_BUFFER[buffer_offset] = (int) node->_datatype;  // Save data type
    POS_BUFFER[buffer_offset] = (int) node->_data;      // Save data
    buffer_offset++;

// #test
// Nessa ordem temos
// + root element
// + operator
// + digit
// + digit

    exibirPosOrdemAndInclude(node->left);
    exibirPosOrdemAndInclude(node->right);
}

// insert:
// An utility function to insert 
// a new node with given key in BST.
// IN: node, value
static struct node_d *insert( struct node_d* node, int data_type, int data )
{

// If the tree is empty, createe a new node and return the pointer.
    if ((void*) node == NULL){
        return (struct node_d *) newNode(data_type, data); 
    }

// Otherwise, recur down the tree.

    // Se for menor, inclui na esquerda.
    if (data_type == DT_INVALID){
        node->left = (struct node_d *) insert(node->left, data_type, data); 
    }else if (data_type == DT_OPERATOR){
        node->left = (struct node_d *) insert(node->left, data_type, data); 
    // Se for maior, inclui na direita.
    }else if (data_type == DT_DIGIT){
        node->right = (struct node_d *) insert(node->right, data_type, data); 
    };

// return the (unchanged) node pointer.
    return (struct node_d *) node; 
} 

/*
 bst_main:
 Called by tree_eval();
 Inicializa �rvore bin�ria.
 Ela pega uma express�o que est� em um buffer e 
 prepara o buffer POS_BUFFER para eval usar.
 Driver Program to test above functions 
 C program to demonstrate insert operation in binary search tree 
 Let us create following BST.

           - 
          /  \ 
         +     * 
        / \   / \ 
       4   3 2   5 
*/
     //4+3 - 2*5 = 12

//==================================================

// bst_initialize:
// Initialize the BST.

// Role: Builds a binary tree from the tokens in exp_buffer.
// Steps:
// + Splits tokens into digits (buffer_digits) and operators (buffer_op).
// + Creates a root node.
// + Inserts all operators into the tree.
// + Inserts all digits into the tree.
// + Traverses the tree in post-order (exibirPosOrdemAndInclude) to fill POS_BUFFER.
// Note: This is a simplified tree builder; it doesn’t yet handle precedence correctly.


static int bst_initialize(void)
{
    struct node_d *root = NULL; 

// Buffer para dígitos
    long buffer_digits[32];
    int buffer_digits_offset=0;

// Buffer para operadores
    int buffer_op[32];
    int buffer_op_offset=0;

    register int i=0;
    long MyLongInteger = 0;
    char opCH=0;

// Global
    buffer_offset = 0;

    //printf ("bst_initialize:\n");

    memset(buffer_digits,0,sizeof(buffer_digits));
    memset(buffer_op,    0,sizeof(buffer_op));

// #IMPORTANTE:
// ESSE É O BUFFER USADO PARA COLOCAR A EXPRESSÃO EM ORDEM,  
// VAMOS FAZER ELE GLOBAL PARA SER PREENCHIDO PELOS TOKENS.

    //int exp[] = { 4, '+', 3, '-', 2, '*', 5, '?' };

// Colocamos nos buffers em ordem.
// ? representa o fim do buffer
// Se ainda não chegou ao fim, continua.
    //printf ("for:: \n");

    int is_operator=FALSE;

    for ( i=0; i<exp_offset; i++ )
    {
        MyLongInteger = (long) exp_buffer[i];

        if (exp_type_buffer[i] == DT_OPERATOR)
        {
            buffer_op[buffer_op_offset] = (int) (MyLongInteger & 0xFF);
            buffer_op_offset++;
        }
        else // DT_DIGIT
        {
            buffer_digits[buffer_digits_offset] = (long) MyLongInteger;
            buffer_digits_offset++;
        }
    }


// Visualizar os buffer,
// pra depois manipular eles.
    buffer_digits[buffer_digits_offset] = (long) 0;  // Digitos
        buffer_op[buffer_op_offset]     = (int) 0;  // Operadores

    //printf("total_digits=%d total_op=%d\n", 
       //buffer_digits_offset, buffer_op_offset );

// ===================================================================
// #todo: 
// NESSA HORA TEM QUE AJUSTAR A 
// PRECEDÊNCIA DOS OPERADORES

//
// Inser into the tree.
//

// -----------------------------------
// :: root
// Inserindo root.
    //printf ("insert root\n");
    root = insert(root, DT_INVALID, 'R'); // Invalid data type

// -----------------------------------
// :: Operadores +
// Coloca todos os operadores na árvore.

    //printf ("for:: Put all operators into the tree\n");
    for ( 
        i=0; 
        i<buffer_op_offset; 
        i++ )
    {   
        int xxxMyOp = (int) buffer_op[i];
        xxxMyOp = (int) (xxxMyOp & 0xFF); 
        // Insert an operator into the tree.    
        insert(root,  DT_OPERATOR, xxxMyOp);
    };

// -----------------------------------
// :: Digits
// Coloca todos os digitos na árvore.
// Cada operador vai operar sobre dois digitos.
    //printf ("for:: Put all digits into the tree\n");

    //for ( i=0; (c = buffer1[i]) != '?'; i++ )
    for ( 
        i=0; 
        i<buffer_digits_offset; 
        i++ )
    {
        MyLongInteger = (long) buffer_digits[i];    // Redundante
        // Insert a digit into the tree.
        insert ( root, DT_DIGIT, MyLongInteger );
    };

// #OK 
// Nos buffers est�o na mesma ordem que na express�o.
// agora vamos inserir na ordem inversa dos buffers.

// ### root ##
//insert 111. 
// � um finalizador, representa o igual
//depois vamos usar o igual =
// x = 4+3 - 2*5

	//root = insert ( root, '?' ); 	
	//os operadores precisar sem inseridos na ordem da express�o.
	//insert(root, '+'); //
	//insert(root, '-'); //
	//insert(root, '*'); //
	//insert(root, 5);   // 
	//insert(root, 2);   //
	//insert(root, 3);   // 
	//insert(root, 4);   //

//
// Exibir a árvore.
//

    // #debug
    //printf(":: em ordem: \n");
    //exibirEmOrdem(root);

    // #debug
    //printf(":: pre ordem: \n");
    //exibirPreOrdem(root);

// #important
// Include into POS_BUFFER[] 
// this buffer is used by eval.
    //printf(":: pos ordem: and include into POS_BUFFER[] \n");
    exibirPosOrdemAndInclude(root);

    return 0; 
} 


//====================================================================

static void push( struct stack_d *s, long x )
{

// Parameter
    if ((void*) s == NULL){
        printf("push: s\n");  exit(1);
    }

    if (s->top < 0){
        printf("push: Stack underflow!\n");
        return;
    }
    if (s->top >= 32){
        printf("push: Stack Overflow!\n");
        return;
    }

    // #debug
    //printf(">>>> PUSH %d into %d\n", x, s->top);

    s->items[ s->top ] = (long) x;

    if (s->top < 32)
        s->top++;
}

static long pop(struct stack_d *s)
{
    long Value=0;

// Parameter
    if ((void*) s == NULL){
        printf("pop: [FAIL] s\n");  exit(1);
    }

    if (s->top < 0){
        printf("pop: Stack Underflow !\n");
        return 0;  //??
    }
    if (s->top >= 32){
        printf("pop: Stack Overflow !\n");
        return 0;  //??
    }
    
    Value = (long) s->items[s->top];

    // #debug
    //printf("<<<< POP %d from %d\n", Value, s->top );

    if (s->top > 0)
        s->top--;

    return (long) Value;
}

static long oper(char c, long opnd1, long opnd2)
{
    printf("oper: OPERATOR=%c o1=%d o2=%d \n",
        c, opnd1, opnd2 );

    switch (c){

    //case 90:  return (opnd1 * opnd2);  break;  // '*'
    //case 91:  return (opnd1 + opnd2);  break;  // '+'
    //case 93:  return (opnd1 - opnd2);  break;  // '-' 
    //case 95:  return (opnd1 / opnd2);  break;  // '/'

    case '*':  return (opnd1 * opnd2);  break;  // '*'
    case '+':  return (opnd1 + opnd2);  break;  // '+'
    case '-':  return (opnd1 - opnd2);  break;  // '-' 

// '/'
    case '/':
        if (opnd2 == 0) {
            printf("oper: Division by zero!\n");
            return 0; // or handle as needed
        }
        return (opnd1 / opnd2);  
        break;

// '%'
    case '%':
        if (opnd2 == 0) {
            printf("oper: Modulo by zero!\n");
            return 0;
        }
        return (opnd1 % opnd2);  
        break;

    case '&':  return (opnd1 & opnd2);  break;   // bitwise AND
    case '|':  return (opnd1 | opnd2);  break;   // bitwise OR
    case '^':  return (opnd1 ^ opnd2);  break;   // bitwise XOR
    case '<':  return (opnd1 < opnd2);  break;   // less than
    case '>':  return (opnd1 > opnd2);  break;   // greater than
    //case '=':  return (opnd1 == opnd2);  break;  // equality
    //case '!':  return (opnd1 != opnd2);  break;  // not equal

    //...

    default: 
        printf("oper: Invalid operator! {%c}\n", c);
        return 0;
        break;
    };
}

// Returns TRUE if op is a "tight-binding" operator (evaluated first pass)
static int is_high_precedence(char op)
{
    return (op == '*' || op == '/' || op == '%');
}

// __eval_linear:
// Two-pass evaluator over exp_buffer[]/exp_type_buffer[].
// Pass 1: resolve all '*','/','%' pairs left-to-right, collapsing
//         each into a single digit in a working buffer.
// Pass 2: resolve remaining '+','-' (and other ops) left-to-right.
// This gives correct precedence for the common arithmetic case
// without a full parser. Still left-to-right within same precedence
// tier, and doesn't handle parentheses regrouping.
static long __eval_linear(void)
{
    long work_val[32];
    char work_op[32];
    int work_count = 0;   // number of values in work_val
    int i;

    if (exp_offset == 0){
        printf("__eval_linear: empty expression\n");
        return 0;
    }
    if (exp_type_buffer[0] != DT_DIGIT){
        printf("__eval_linear: expected digit at position 0\n");
        exit(1);
    }

    // ---- Pass 1: high precedence (* / %) ----
    work_val[0] = exp_buffer[0];
    work_count = 1;

    i = 1;
    while (i < exp_offset)
    {
        char op;
        int rhs;

        if (exp_type_buffer[i] != DT_OPERATOR){
            printf("__eval_linear: expected operator at position %d\n", i);
            exit(1);
        }
        op = (char) (exp_buffer[i] & 0xFF);
        i++;

        if (i >= exp_offset || exp_type_buffer[i] != DT_DIGIT){
            printf("__eval_linear: expected digit after operator at position %d\n", i);
            exit(1);
        }
        rhs = exp_buffer[i];
        i++;

        if (is_high_precedence(op))
        {
            // Fold immediately into the last collected value
            work_val[work_count-1] = oper(op, work_val[work_count-1], rhs);
        }
        else
        {

            // #todo:
            // Defer: keep operator and rhs for pass 2
            //if (work_count >= 32){
            //    printf("__eval_linear: expression too long (max 32 terms)\n");
            //    exit(1);
            //}

            // Defer: keep operator and rhs for pass 2
            work_op[work_count-1] = op;   // op that precedes work_val[work_count]
            work_val[work_count] = rhs;
            work_count++;
        }
    }

    // ---- Pass 2: remaining low precedence (+ - etc.) left to right ----
    long result = work_val[0];
    for (i = 1; i < work_count; i++)
    {
        result = oper(work_op[i-1], result, work_val[i]);
    }

    return (long) result;
}

static void treeInitializeGlobals(void)
{
    register int i=0;

// Expression buffer
    for (i=0; i<32; i++)
    {
        exp_buffer[i]=0;
        exp_type_buffer[i]=0;
    };
    exp_offset = 0;

// dt buffer
    for (i=0; i<32; i++){
        DT_BUFFER[i]=0;
    };
// POS buffer
    for (i=0; i<32; i++){
        POS_BUFFER[i]=0;
    };
    buffer_offset = 0;

// ...

}

// -------------------------------------------------
// tree_eval:
// Calcula a express�o e retorna o valor.
// #todo:
// prepara o buffer contendo a express�o em ordem. 
// pra isso precisamos pegar os tokens e colocar no buffer. 
// #exemplo
// tem que pegar os tokens e colocar assim no buffer.
// +os n�meros s�o n�meros mesmo 
// +os operadores s�o chars ou strings.
// tem que finalizar com '?'
// exp[] = { 4, '+', 3, '-', 2, '*', 5, '?' };
// #todo
// vamos copiar a fun��o no parser que pega os tokens de express�es.
// mas por enquanto s� os operadores b�sicos.
// Global function.
// Pegamos os pr�ximos tokens e colocamos no buffer exp_buffer[].
// Initializa a �rvore bin�ria chamando bst_initialize(),
// os dados s�o transferidos para o buffer POS_BUFFER[].
// Calcula o resultado chamando eval();

// Role: Entry point for evaluating an expression.
// Steps:
// + Initializes buffers (treeInitializeGlobals).
// + Reads tokens from the lexer (yylex).
// + Fills exp_buffer with constants and operators.
// + Detects ; → end of expression.
// + Calls bst_initialize() to build a tree.
// + Calls __eval() to compute the final value.
// Shortcut: If only one constant is found before ;, 
// the stack evaluation just returns that constant.

unsigned long tree_eval(void)
{
// >> This function gets the expression from stdin?
// The lexer get the tokens and
// we put the, all into a local buffer and then 
// we call the eval function to calculate the value.

    int running = 1;
    int State = 1;
    register int c=0;
    int j=0;
    int v=0;

    printf ("tree_eval:\n");

    treeInitializeGlobals();


// State Machine in tree_eval()
// State 1: Expecting a number.
//   If TK_CONSTANT, store it in exp_buffer.
//   Then switch to State 2.
// State 2: Expecting operator or separator.
//   If operator (+, -, etc.), store it and go back to State 1.
//   If separator ;, jump to evaluation.
// Shortcut: For "1;", State 2 sees ; and ends immediately, 
// leaving only one digit in the buffer.

    while (running == 1){

    // Get from stdin.
    // Esse é o mesmo arquivo que gramcnf esta lendo.
    // Continuamos de one ele parou antes de chamar essa rotina.
    c = yylex();

    // EOF was found
    if (c == TK_EOF){
        printf ("tree_eval: #error EOF in line %d\n", 
            LexerInfo.current_line );
        exit(1);
    }

    // ';' was found. 
    // End of statement.
    // Shortcut:
    // If we only saw one constant before ';',
    // bst_initialize() will build a trivial tree
    // and __eval() will just return that constant.

    if (c == TK_SEPARATOR)
    {
        if ( gramado_strncmp ( (char *) real_token_buffer, ";", 1 ) == 0  )
        {
            //printf("tree_eval: ';' was found in State %d\n",State);
            exp_buffer[exp_offset] = (long) 0;
            //exp_offset++;
            goto do_bst;
        }
    }

    switch (State){
    
    // State1: Numbers.
    case 1:
        //printf("tree_eval: entering State %d, token=%d (%s)\n", 
            //State, c, real_token_buffer);
        switch (c){

        // Constants: Números ou separadores.
        case TK_CONSTANT:
            //exp_buffer[exp_offset] = (int) atoi(real_token_buffer);
            exp_buffer[exp_offset] = (long) strtol(real_token_buffer, NULL, 0);
            exp_type_buffer[exp_offset] = DT_DIGIT;      // <-- tag it
            exp_offset++;
            // Depois de um numero espera-se 
            // um operador ou um separador.
            State=2; 
            break;

        // ';' separador no caso de return void.
        // para quando a express�o � depois do return.
        case TK_SEPARATOR:
            if ( gramado_strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
            {
                exp_buffer[exp_offset] = (long) 0;
                //exp_offset++;
                goto do_bst;  // #done
            }
            //if ( gramado_strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
            //{}
        // #todo
        // Temos que tratar as aberturas e fechamentos (),{}	
        default:
            printf("tree_eval: State1 default\n"); exit(1);
            break;
        }
        break;

    // State2: Operators and separators.
    // In the case of a ';' we return the value found in the state 1.
    case 2:
        //printf("tree_eval: entering State %d, token=%d (%s)\n", 
            //State, c, real_token_buffer);
        switch (c){
        
        // Operators
        // Get the valid operators and upt them all into the 
        // expression buffer.
        case '+':  case '-':  case '*':  case '/':
        case '&':  case '|':
        case '<':  case '>':
        case '%':
        case '^':
        case '!':
        case '=':
            exp_buffer[exp_offset] = (long) (c & 0xFF);
            //exp_buffer[exp_offset] = (int) c;
            exp_type_buffer[exp_offset] = DT_OPERATOR;   // <-- tag it
            exp_offset++;
            // Depois do operador esperamos 
            // um n�mero ou um separador ')' ou 
            // finalizador provis�rio?.
            State=1; 
            break;

        // Separators
        // ')' provis�rio para terminar a express�o,
        // da� incluimos o finalizador provis�rio '?'
        case TK_SEPARATOR:
            // ')'
            if ( gramado_strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
            {
                exp_buffer[exp_offset] = (long) 0;
                //exp_offset++;
                goto do_bst;  // #done
            }
            // ';'
            if ( gramado_strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
            {
                //printf("tree_eval: ';' was found\n");
                exp_buffer[exp_offset] = (long) 0;
                //exp_offset++;
                goto do_bst;  // #done
            }
            break;

        case TK_ARITHCOMPARE:
            //printf("tree_eval: TK_ARITHCOMPARE %c\n", real_token_buffer[0]);
            if (LexerInfo.lexer_expression == LT_EXPR)
            {
                exp_buffer[exp_offset] = (int) '<';
                exp_type_buffer[exp_offset] = DT_OPERATOR;   // <-- add this
                exp_offset++;
                State=1;
            }
            if (LexerInfo.lexer_expression == GT_EXPR)
            {
                exp_buffer[exp_offset] = (int) '>';
                exp_type_buffer[exp_offset] = DT_OPERATOR;   // <-- add this
                exp_offset++;
                State=1;
            }
            break;

        // State2 default
        default:
            break;  
        } 
        break;

    default:
        printf("tree_eval: Default State\n");
        break;
    };

    };  // While end


//
// BST
//

do_bst:

// If no tokens were collected, return 0 as default
    if (exp_offset == 0) 
    {
        printf("tree_eval: [do_bst] empty expression before ';' (line %d)\n",
           LexerInfo.current_line );
        return 0;  // or handle as an error
    }

//==================================================
// #debug
// Visualizando o buffer
    //printf("\n");
    //printf("do_bst: Show buffer\n");

    /*
    //for (j=0; j<32; j++)
    //for (j=0; j<16; j++)
    for (j=0; j<exp_offset; j++)
    {
        v = exp_buffer[j];
        if ( v >= 0 && v <= 9 ){
            printf("exp_buffer: %d\n", exp_buffer[j]);
        }else{
            printf("exp_buffer: %c\n", exp_buffer[j]);
        }
    };
    */

    //#debug 
    //hang
    //printf("do_bst: *debug breakpoint");
    //while(1){}    

//==================================================
// Initialize the BST.
// Pega uma expressão que está em um buffer e 
// prepara o buffer POS_BUFFER para eval() usar.

    // bst_initialize(); 

//#debug
//ok funcionou
    //printf ("\n tree_eval: result={%d} \n", eval ( (int*) &POS_BUFFER[0] ) );   	

//#debug 
//hang
    //printf("*debug breakpoint");
    //while(1){}    

//
// Eval
//

    // Old version #delete
    // This is the moment where we get the final result
    //unsigned long ret_val=0;
    //ret_val = (unsigned long) __eval(); 

    unsigned long ret_val=0;
    ret_val = (unsigned long) __eval_linear();

    //printf("result: >>>>> %d\n",ret_val);
    return (unsigned long) ret_val; 
done:
    return (unsigned long) ret_val;
}

//
// End
//

