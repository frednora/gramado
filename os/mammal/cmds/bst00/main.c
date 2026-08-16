// The is an example of bst?
// Pos ordem pra polonesa inversa

#include <stdio.h>
#include <stdlib.h>

struct arvore //cada entrada da equacao sera um no da arvore
{
	char entrada[1];
	struct arvore *esq;//cada no aponta para outros dois
	struct arvore *dir;
};

/* A impressoo Em Ordem eh utilizada para a impressao da notacao infixa*/
void emordem(struct arvore*no)
{	
	if(no!=NULL)
	{
		emordem(no->esq);
		if (no->entrada[0] >= 48 && no->entrada[0] <=57)
		printf("(");		
		printf("%s", no->entrada); 
		if (no->entrada[0] >= 48 && no->entrada[0] <=57)
		printf(")");
		emordem(no->dir);
	}
}

/* A impressao Pos Ordem eh utilizada para a impressao da notacao RNP*/
posordem (struct arvore *lista)
{
	if(lista != NULL)
	{
		posordem(lista->esq);
		posordem(lista->dir);
		printf("%s ", lista->entrada);
	}
}

/*A funcao insertsign insere apenas os operadores da equacao na arvore, 
todos sao inseridos a esquerda*/
void insertsign(struct arvore *no, char *k)
{
	struct arvore *aux = no;
	struct arvore *novo = (struct arvore *)malloc(sizeof(struct arvore));
	novo->esq = NULL;
	novo->dir = NULL;
	strcpy(novo->entrada, k);
	while (aux->esq != NULL)
	{
		aux = aux->esq;
	}
	aux->esq = novo;
}

/*a funcao insertnum insere os numeros da equacao na arvore, sendo o prieiro
deles a esquerda do uktimo sinal, o segundo a direita do ultimo sinal, e os demais
numeros (n > 2) a direita de seus respectivos operadores */
void insertnum(struct arvore *no, char *k, int n, int i) 
{
	int count = 1;
	struct arvore *aux = no;
	struct arvore *a = NULL;
	struct arvore *novo = (struct arvore *)malloc(sizeof(struct arvore));
	novo->esq = NULL;
	novo->dir = NULL;
	strcpy(novo->entrada, k);
	if (n == 1)
	{
		while (aux->esq != NULL)
		{
			aux = aux->esq;
		}
		aux->esq = novo;
	}
	else if (n == 2)
	{
		while (count < i)
		{
			aux = aux->esq;
			count++;
		}
		aux->dir = novo;
	}
	else if (n > 2)
	{
		while (no->dir == NULL)
		{
			if (aux->esq->esq != NULL && aux->esq->dir != NULL && aux->dir == NULL)
			{
				aux->dir = novo;
				break;
			}
			else 
			aux = aux->esq;
		}
	}
}

int main()
{
	struct arvore *raiz = NULL;
	int i = 1, j = 1, k, escolha;
	char a[1], b[1];
	printf("Ola! Este programa ira converter uma entrada em notacao polonesa para a notacao polonesa reversa (RPN) e infixa.\n");
	printf("Digite sua equacao em notacao polonesa\n(separe os simbolos e numeros todos por um 'espaco' e depois tecle 'Enter'):\n");
	raiz = (struct arvore *)malloc(sizeof(struct arvore)); // cria a raiz
	raiz->esq = NULL; //aterra a raiz
	raiz->dir = NULL;
	scanf("%s", a);
	strcpy(raiz->entrada, a);
	scanf("%s", a);
	while(a[0] == 42 || a[0] == 43 || a[0] == 45 || a[0] == 47) //enquanto sao entrados sinais, cahama a funcao insertsign
	{
		insertsign(raiz, a);
		scanf("%s", a);
		i++;	
	} 
	insertnum(raiz, a, j, i); //quando sao inseridos numeros, passa a chamar a funcao insertnum
	for (j = 2; j < i + 2; j++)
	{
		scanf("%s", a);
		insertnum(raiz, a, j, i);
	}
	while (escolha != 3) //repete o menu ate que seja escolhida a opcao "sair"
	{
		printf("\n\nO que voce deseja fazer? Digite o numero correspondente!\n");
		printf("1 - Impressao na notacao polonesa reversa (RPN)\n2 - Impressao na notacao infixa\n3 - Sair\n\n");
		scanf("%d", &escolha);
		if (escolha > 3 || escolha <1) //caso seja escolhida uma opcao fora do menu, uma mensagem de erro eh impressa
		{
			printf("Favor entrar com uma das opcoes do menu.\n");
		}
		else
		{
			switch(escolha)
			{
				case 1:
					printf("\nPolonesa reversa:\n");
					posordem(raiz);
					break;
				case 2:
					printf("\nNotacao infixa:\n");	
					printf("(");
					emordem(raiz);
					printf(")");
			}
		}
	}
}
