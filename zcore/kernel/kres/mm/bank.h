

// Número máximo de bancos no sistema. 
#define BANKS_COUNT_MAX  8

//contando a quantidade de bancos no sistema. 
//A quantidade de bancos depende do tamanho do sistema.
//A quantidade mínima de bancos é 1.
//Se tivermos apenas um banco, a user session deverá ficar dentro desse banco.
//se tivermos 2 bancos a user session ficará no último banco.
//Se tivermos até oito banco de memória a user session ficará no último banco.
int banksCount;


//*Importante: A user session ficará no último banco.
//caso o sistema tenha apenas um banco, então a user session ficará no fim do banco.
int lastBank;

typedef enum {

    banktypeLDB,    // LDB - Local Data Base.   (private)
    banktypeGDB,    // GDB - Global Data Base.  (shareable)
    banktypeKDB,    // KDB - Kernel Data Base.  (kernel only)
    banktypeFDB,    // FDB - Free Data Base.    (free memory)

}bank_type_t;


typedef struct bank_d bank_t;
struct bank_d
{

	object_type_t  objectType;
	object_class_t objectClass;

	int used;
	int magic;

	bank_type_t type;
	//...

	struct user_info_d *user;    // Que usuário é dono desse banco.

	//
	// * Listas de contas nesse banco.
	// Obs: 
	//     As listas serão colocadas em um banco de acordo com o tipo 
	// de banco e tipo de conta.
	//
	//struct aspace_d *aspaceListHead;
	//struct dspace_d *dspaceListHead;	


	// Lista de processos utilizando esse objeto.
	struct process_d *processListHead;
	//struct process_d *currentProcess;
	//int processCount;

    //...	
	
	//Navegação.
    struct bank_d *Next;  
};

struct bank_d *Bank;
//bank_t *Bank;
//bank_t *KernelBank;
//...






