//tree.h

// Estudando esse trem de ?rvores.
// Nesse endere?o aqui: http://andreiformiga.com/post/repformlog/
// Desse cara aqui: Andrei Formiga ( math is hard, let’s go shopping! )


// tipo do no da arvore
typedef enum tagTipo {
  NEG, AND, OR, IMP, BIMP, P, Q, R
} Tipo;

// macro para calcular o valor da implicacao
#define IMPVAL(b1, b2)       (b1 && !b2 ? FALSE : TRUE)

// macro para calcular o valor de uma bi-implicacao
#define BIMPVAL(b1, b2)      (b1 == b2)

#define VARS   3
#define PIND   0
#define QIND   1
#define RIND   2

// representacao da formula
char nome[VARS];     // nome das variaveis
int I[VARS];         // interpretacao das variaveis

void inicializa_formula()
{
  nome[PIND] = 'P';
  nome[QIND] = 'Q';
  nome[RIND] = 'R';

  for (int c = 0; c < VARS; c++)
    I[c] = 0;//FALSE;
}

struct tree_node 
{
	// Valor do nÃ³
    int value; 

    struct tree_node *left; 
    struct tree_node *right; 

    struct tree_node *parent; 
};

//struct tree_node *root;


// isso n?o seria necess?rio se tiv?ssemos usando um array 
// de strings como no come?o. 'char **v' ou char *v[].
struct tree_element
{
	int index;
    struct tree_node *tree;

	//c?pia s?mbolo, para no meio de uma express?o podermos reacessar o nome da var.
	//uma string com 8 chars para representar os 8 nibles de uma dword.
	
    char sysbol_string[32];  
    char value_string[8];  	
}

// a lista  n?s de ?rvores usada pela 
// express?o matem?tica atual.
unsigned long treeelementList[32];







