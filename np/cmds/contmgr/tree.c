// tree.c 
// bst - Binary Search Tree
// Evaluation an expression.
// Document created by Fred Nora.


#include "gramcnf.h"

// ??
// #importante
// #expressão em ordem!
// Os tokens serão colocados aqui como uma expressão em ordem.
static int exp_buffer[32];
int exp_offset=0;

//====================================================================
// Buffer pra fazer conta usando 'pos order'.
static int DT_BUFFER[32];
static int POS_BUFFER[32];
int buffer_offset = 0;
//====================================================================

#define DT_INVALID   0
#define DT_OPERATOR  1000
#define DT_DIGIT     2000
// ...

// Node
// #todo: Change to node_d
struct node 
{ 

//  0000 = ignore
//  1000 = operator
//  2000 = digits
//  ...
    int _datatype;  // Type of data
    int _data;   // Data

    struct node *left;
    struct node *right;
}; 

// Stack
// #todo: Change to stack_d
struct stack
{
    int top;
    int items[32];
};

// ==============================================
// Prototypes for internal workers.


static int my_isdigit(char ch);
static void exibirEmOrdem (struct node *node);
static void exibirPreOrdem(struct node *node);
static void exibirPosOrdem (struct node *node);

static void inorder(struct node *root);
static struct node *newNode(int data_type, int data);
static struct node *insert( struct node* node, int data_type, int data );
static void push( struct stack *s, int x );
static int pop (struct stack *s);
static int oper(char c, int opnd1, int opnd2);
static int eval(void);
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
static struct node *newNode(int data_type, int data)
{
    struct node *tmp;

// #malloc
    tmp = (struct node *) malloc( sizeof(struct node) );
    if ((void*) tmp == NULL){
        return NULL;
    }
    tmp->_datatype = (int) data_type;
    tmp->_data = (int) data; 
    tmp->left = NULL;
    tmp->right = NULL;

    return (struct node *) tmp;
}

// A utility function to do inorder traversal of BST.
// IN: root node?
static void inorder(struct node *root)
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
static void exibirEmOrdem (struct node *node)
{
    if ((void*) node == NULL)
        return;

    printf("%d %d\n", node->_datatype, node->_data);

    exibirEmOrdem (node->left);
    exibirEmOrdem (node->right);
}

// Pr�-ordem +ab.
// Imprime o conte�do
// desce at� o �ltimo pela esquerda
// visita a direita e desce at� o �ltimo pela esquerda.
// IN: root node?
static void exibirPreOrdem(struct node *node)
{
    if ((void*) node == NULL)
        return;

    printf("%d %d\n", node->_datatype, node->_data);

    exibirPreOrdem(node->left);
    exibirPreOrdem(node->right);
}

// P�s-ordem ab+.
// #importante
// Exibe em n�veis. de baixo para cima.
// desce at� o ultimo pela esquerda
// visita o da direita e imprime;
static void exibirPosOrdem (struct node *node)
{
// Include into POS_BUFFER[] 
// this buffer is used by eval.

    if ((void*) node == NULL)
        return;

    //?? # what is that?
    if ( buffer_offset < 0 || buffer_offset >= 32 )
    {
        printf("exibirPosOrdem: buffer_offset\n");
        return;
    }

// #importante
// Vai colocando num buffer pra usar no c�lculo 
// isso simula uma digita��o
// eval() vai usar esse buffer

    printf("%d %d\n", node->_datatype, node->_data);

     DT_BUFFER[buffer_offset] = (int) node->_datatype;  // Save data type
    POS_BUFFER[buffer_offset] = (int) node->_data;      // Save data
    buffer_offset++;

// #test
// Nessa ordem temos
// + root element
// + operator
// + digit
// + digit

    exibirPosOrdem(node->left);
    exibirPosOrdem(node->right);
}

// insert:
// An utility function to insert 
// a new node with given key in BST.
// IN: node, value
static struct node *insert( struct node* node, int data_type, int data )
{

// If the tree is empty, createe a new node and return the pointer.
    if ((void*) node == NULL){
        return (struct node *) newNode(data_type, data); 
    }

// Otherwise, recur down the tree.

    // Se for menor, inclui na esquerda.
    if (data_type == DT_INVALID){
        node->left = (struct node *) insert(node->left, data_type, data); 
    }else if (data_type == DT_OPERATOR){
        node->left = (struct node *) insert(node->left, data_type, data); 
    // Se for maior, inclui na direita.
    }else if (data_type == DT_DIGIT){
        node->right = (struct node *) insert(node->right, data_type, data); 
    };

// return the (unchanged) node pointer.
    return (struct node *) node; 
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
static int bst_initialize(void)
{
    struct node *root = NULL; 

// Buffer para dígitos.
    int buffer_digits[32];
    int buffer_digits_offset=0;

// Buffer para operadores.
    int buffer_op[32];
    int buffer_op_offset=0;

    register int i=0;
    int MyInteger = 0;
    char opCH=0;

// Global.
    buffer_offset = 0;

    printf ("bst_initialize:\n");

    memset(buffer_digits,0,sizeof(buffer_digits));
    memset(buffer_op,    0,sizeof(buffer_op));

// #IMPORTANTE:
// ESSE É O BUFFER USADO PARA COLOCAR A EXPRESSÃO EM ORDEM,  
// VAMOS FAZER ELE GLOBAL PARA SER PREENCHIDO PELOS TOKENS.

    //int exp[] = { 4, '+', 3, '-', 2, '*', 5, '?' };

// Colocamos nos buffers em ordem.
// ? representa o fim do buffer
// Se ainda não chegou ao fim, continua.
    printf ("for:: \n");

    int is_operator=FALSE;
    // Number of elements found in the expression
    //for ( i=0; i<32;  i++ )
    for ( i=0; i<exp_offset;  i++ )
    {
        MyInteger = (int) exp_buffer[i];
        opCH = (char) (MyInteger & 0xFF);

        switch (opCH)
        {
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
                is_operator = TRUE;
                buffer_op[buffer_op_offset] = (int) (MyInteger & 0xFF);
                buffer_op_offset++;
                break;
            default:
                is_operator = FALSE;
                buffer_digits[buffer_digits_offset] = (int) MyInteger;
                buffer_digits_offset++; 
                break;
        }

        /*
        // #bugbug
        // Here the numbers are integers of 32bit.
        // buffer1[] para Numbers.
        if (is_operator == TRUE){
            //printf(">");  //#debug
            // dígito
            buffer_digits[buffer_digits_offset] = (int) MyInteger;
            buffer_digits_offset++; 

        // Here are the operators.
        // buffer_op[] para Operators.
        }else if(is_operator == FALSE){
            //printf("$");  //#debug
            // operadores
            buffer_op[buffer_op_offset] = (int) MyInteger;
            buffer_op_offset++;
        }
        */
    };

// Visualizar os buffer,
// pra depois manipular eles.
    buffer_digits[buffer_digits_offset] = (int) 0;  // Digitos
        buffer_op[buffer_op_offset]     = (int) 0;  // Operadores

    printf("total_digits=%d total_op=%d\n", 
       buffer_digits_offset,
       buffer_op_offset );

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
    printf ("insert root\n");
    root = insert(root, DT_INVALID, 'R'); // Invalid data type

// -----------------------------------
// :: Operadores +
// Coloca todos os operadores na árvore.

    printf ("for:: Put all operators into the tree\n");
    for ( 
        i=0; 
        i<buffer_op_offset; 
        i++ )
    {   
        MyInteger = (int) buffer_op[i];
        MyInteger = (int) (MyInteger & 0xFF); 
        // Insert an operator into the tree.    
        insert(root,  DT_OPERATOR, MyInteger);
    };

// -----------------------------------
// :: Digits
// Coloca todos os digitos na árvore.
// Cada operador vai operar sobre dois digitos.
    printf ("for:: Put all digits into the tree\n");

    //for ( i=0; (c = buffer1[i]) != '?'; i++ )
    for ( 
        i=0; 
        i<buffer_digits_offset; 
        i++ )
    {
        MyInteger = (int) buffer_digits[i];    // Redundante
        // Insert a digit into the tree.
        insert ( root, DT_DIGIT, MyInteger );
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
    printf(":: pos ordem: and include into POS_BUFFER[] \n");
    exibirPosOrdem(root);

    return 0; 
} 


//====================================================================

static void push( struct stack *s, int x )
{

// Parameter
    if ((void*) s == NULL){
        printf("push: s\n");
        exit(1);
    }

    if (s->top < 0){
        printf("push: Stack underflow!\n");
        return;
    }
    if (s->top >= 32){
        printf("push: Stack Overflow!\n");
        return;
    }

    s->items[ s->top ] = (int) x;
    s->top++;

}

static int pop (struct stack *s)
{
    int Value=0;

// Parameter
    if ((void*) s == NULL){
        printf("pop: [FAIL] s\n");
        exit(1);
    }

    if (s->top < 0){
        printf("pop: Stack Underflow !\n");
        return 0;  //??
    }
    if (s->top >= 32){
        printf("pop: Stack Overflow !\n");
        return 0;  //??
    }

    Value = (int) s->items[s->top];
    s->top--;

    return (int) Value;
}

static int oper(char c, int opnd1, int opnd2)
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
    case '/':  return (opnd1 / opnd2);  break;  // '/'

    // #todo
    // Yes we have some pow functions in math.c
    case '^': 
        return 0; //return(pow(opnd1,opnd2));
        break;

    //...

    default: 
        printf("oper: Invalid operator! {%c}\n", c);
        return 0;
        break;
    };
}

// Internal worker.
static int eval(void)
{
    register int i=0;
    int opnd1=0;
    int opnd2=0; 
    int val=0;
    struct stack stk;

    printf("eval: Show DT_BUFFER[] and POS_BUFFER[]\n");
    // O elemento extra é o root node.
    for (i=0; i<(exp_offset+1); i++)
    {
        printf("%d %d\n",
             DT_BUFFER[i], 
            POS_BUFFER[i]);
    };
    memset( &stk, 0, sizeof(struct stack) );
    //stk.top = -1;
    stk.top = 0;

// Parameter
    //if ( (void*) buff == NULL)
        //return -1;

    int is_operator=FALSE;
    int MyInteger = 0;
    char opCH=0;
    char opCHSaved=0;
    int MyDT = 0;

    int OperatorFound=FALSE;
    int DigitCounter = 0;

    //for ( i=0; (c = buff[i]) != '?'; i++ )
    for ( 
        i=0; 
        i < (exp_offset + 1);  // Porque incluimos o 'root' na tree. 
        i++ )
    {
        MyDT      = (int)  DT_BUFFER[i];
        MyInteger = (int) POS_BUFFER[i];
        opCH = (char) (MyInteger & 0xFF);

        // #ORDER:
        // The operator come first and then two digits.
        // #todo: We need to redo it each operator found.

        switch (opCH)
        {
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
                is_operator = TRUE;
                if (MyDT != DT_OPERATOR){
                    printf("eval: Expected DT_OPERATOR data type in %d\n",i);
                    exit(1);
                }
                opCHSaved = opCH;
                OperatorFound = TRUE;
                break;

            // The root entrie in the tree
            // ignore
            case 'R':
                is_operator = -1;
                if (MyDT != DT_INVALID){
                    printf("eval: Expected DT_INVALID data type in %d\n",i);
                    exit(1);
                }
                break;

            default:
                is_operator = FALSE;
                if (MyDT != DT_DIGIT){
                    printf("eval: Expected DT_DIGIT data type in %d\n",i);
                    exit(1);
                }
                DigitCounter++;
                printf(">>>>PUSH digit\n");
                push( &stk, (int) MyInteger );
                if (OperatorFound == TRUE)
                {
                    if (DigitCounter == 2)
                    {
                        stk.top--;  // Get last included.
                        opnd2 = (int) pop(&stk);
                        opnd1 = (int) pop(&stk);
                        val = (int) oper( (char) opCHSaved, opnd1, opnd2 );
                        printf("eval: value={%d}\n",val);
                        // Push result.
                        push( &stk, val );
                    }
                }
                break;
        }
    }

// O resultado é o que sobrou na pilha.
    int FinalValue = pop(&stk);
    return (int) FinalValue;
}

static void treeInitializeGlobals(void)
{
    register int i=0;

// Expression buffer
    for (i=0; i<32; i++){
        exp_buffer[i]=0;
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

    while (running == 1){

    // Get from stdin.
    // Esse é o mesmo arquivo que gramcnf esta lendo.
    // Continuamos de one ele parou antes de chamar essa rotina.
    c = yylex();

    // EOF was found
    if (c == TK_EOF){
        printf ("tree_eval: #error EOF in line %d\n", lexer_currentline);
        exit(1);
    }

    // ';' was found. 
    // End of statement.
    if (c == TK_SEPARATOR)
    {
        if ( strncmp ( (char *) real_token_buffer, ";", 1 ) == 0  )
        {
            printf("tree_eval: ';' was found!\n");
            exp_buffer[exp_offset] = (int) 0;
            //exp_offset++;
            goto do_bst;
        }
    }

    switch (State){
    
    // State1: Numbers.
    case 1:
        switch (c){

        // Constants: Números ou separadores.
        case TK_CONSTANT:
            exp_buffer[exp_offset] = (int) atoi(real_token_buffer);
            exp_offset++;
            // Depois de um numero espera-se 
            // um operador ou um separador.
            State=2; 
            break;

        // ';' separador no caso de return void.
        // para quando a express�o � depois do return.
        case TK_SEPARATOR:
            if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
            {
                exp_buffer[exp_offset] = (int) 0;
                //exp_offset++;
                goto do_bst;  // #done
            }
            //if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
            //{}
        // #todo
        // Temos que tratar as aberturas e fechamentos (),{}	
        default:
            printf("tree_eval: State1 default\n");
            exit(1);
            break;
        }
        break;

    // State2: Operators and separators.
    case 2:
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
            exp_buffer[exp_offset] = (int) c;
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
            if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
            {
                exp_buffer[exp_offset] = (int) 0;
                //exp_offset++;
                goto do_bst;  // #done
            }
            // ';'
            if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
            {
                printf("tree_eval: ';' was found\n");
                exp_buffer[exp_offset] = (int) 0;
                //exp_offset++;
                goto do_bst;  // #done
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
    };  // While end.


//
// BST
//

do_bst:

//==================================================
// #debug
// Visualizando o buffer.
    printf("\n");
    printf("do_bst: Show buffer\n");

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
    //#debug 
    //hang
    //printf("do_bst: *debug breakpoint");
    //while(1){}    

//==================================================
// Initialize the BST.
// Pega uma expressão que está em um buffer e 
// prepara o buffer POS_BUFFER para eval() usar.

    bst_initialize(); 

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

// This is the moment where we get the final result.

    unsigned long ret_val=0;
    ret_val = (unsigned long) eval(); 
    printf("result: >>>>> %d\n",ret_val);
    return (unsigned long) ret_val; 
done:
    return (unsigned long) ret_val;
}

//
// End
//

