
// queue.h
// This routines are suspended.

#ifndef __KE_QUEUE_H
#define __KE_QUEUE_H    1

// Ordenação das listas de filas.
#define QUEUE_NULL     0
#define QUEUE_STANDBY  1
#define QUEUE_RUNNING  2
#define QUEUE_READY    3
#define QUEUE_WAITING  4  //Fila das threads que estão esperando.
#define QUEUE_BLOCKED  5
#define QUEUE_ZOMBIE   6
#define QUEUE_DEAD     7
#define QUEUE_INITIALIZED   8
#define QUEUE_SYSCOOP       9  // Sistema, cooperação.   
#define QUEUE_USERCOOP     10  // Usuário, cooperação.   
#define QUEUE_SYSCONC      11  // Sistema, concorrência. 
#define QUEUE_USERCONC     12  // Usuário, concorrencia.  
#define QUEUE_REALTIME     13 
#define QUEUE_DEFAULT      14
//...

// ??
// Níveis de prioridade.
#define QUEUE_LEVEL_MIN 15
#define QUEUE_LEVEL_MED 16
#define QUEUE_LEVEL_MAX 17

// ---------
// Essa é a organização padrão de uma 
// lista de ponteiros de lista encadeada.
// 0 = DRIVERS
// 1 = SERVERS
// 2 = USER APPS
#define MAX_QUEUES  3
unsigned long QUEUES[3];

/*
 **********************************************************
 * queue_d:
 *     Estrutura para organizar 
 * várias listas de ponteiros de lista encadeada.
 */

// #bugbug
// Isso pode estar errado.
// Os arrays nessa estrutura usam o valor '3'
// a a rotina pode estar colocando valores fora do limite desse
// array, usando o limite de 17, portanto overflow!

struct queue_d
{
	// #importante
	// MAX_QUEUES é o número máximo de filas que pode haver na lista.
	// A lista contém o potneiro head de uma lista encadeada.
	//
   	
	//Movimento 1, initialized --> standby. 
    int standbyHead; 
    int standbyTail;
    int standbyMax;
    unsigned long standbyList[MAX_QUEUES+1];    


	//Movimento 2, standby --> running.
	//Movimento 4, ready   --> running.    
	//*** deve estar em sintonia com a fila do dispatcher.	
    int runningHead; 
    int runningTail;
    int runningMax;
    unsigned long runningList[MAX_QUEUES+1];    
	
	//Movimento 3, running --> ready.
	//Movimento 6, waiting --> ready.
	//Movimento 8, blocked --> ready.	
    int readyHead; 
    int readyTail;
    int readyMax;
    unsigned long readyList[MAX_QUEUES+1];     
	 
	 
	//Movimento 5, ready --> waiting.
    //Esperando para retomar a execução.	
    int waitingHead;
    int waitingTail;
    int waitingMax;
    unsigned long waitingList[MAX_QUEUES+1];
	  
	  
	//Movimento 7, running --> blocked.	
    int blockedHead;
    int blockedTail;
    int blockedMax;
    unsigned long blockedList[MAX_QUEUES+1];
	  
	//Movimento 9, running --> zombie.	
    int zombieHead;
    int zombieTail;
    int zombieMax;
    unsigned long zombieList[MAX_QUEUES+1];
	  
	  
	//Movimento 10, zombie --> dead.
    int deadHead;
    int deadTail;
    int deadMax;
    unsigned long deadList[MAX_QUEUES+1];    

	//Movimento 11, zombie --> initalized.
    //Movimento 0,       x --> initalized. (created)	
    int initializedHead;
    int initializedTail;
    int initializedMax;
    unsigned long initializedList[MAX_QUEUES+1];
	
	
	/*
     * RACE: 
	 *    Filas do loop do sistema. 
	 *    Cooperação e Concorrência.
     */ 
	  
	  
    //Cooperação, sistema.
    int syscoopHead; 
    int syscoopTail;
    int syscoopMax;
    unsigned long syscoopList[MAX_QUEUES+1];   

	//Concorrência, sistema.
    int sysconcHead; 
    int sysconcTail;
    int sysconcMax;
    unsigned long sysconcList[MAX_QUEUES+1];

	/*
     * RACE: Filas do loop do usuário. Cooperação e Concorrência.
     */ 

	
	//Cooperação, usuário.
    int usercoopHead; 
    int usercoopTail;
    int usercoopMax;
    unsigned long usercoopList[MAX_QUEUES+1];  	

	//Concorrência, usuário.
    int userconcHead; 
    int userconcTail;
    int userconcMax;
    unsigned long userconcList[MAX_QUEUES+1];   

	
    /*
     * PITSTOP: Outras filas.
     */ 

   //realtime tasks queue. 
    int realtimeHead; 
    int realtimeTail;
    int realtimeMax;
    unsigned long realtimeList[MAX_QUEUES+1];  //real time rr. 
	
	
    //default queue
    int defaultHead;
    int defaultTail;
    int defaultMax;
    unsigned long defaultList[MAX_QUEUES+1]; 
	
	
	// #test
    // Listas para filas de threads que se encaixam emdeterminadas faixas de prioridade.
	
	
	//Mínimo = 'level <= PRIORITY_LOW'
    int minHead; 
    int minTail;
    int minMax;
    unsigned long minList[MAX_QUEUES+1];
	
	//Médio == 'PRIORITY_LOW > level <= PRIORITY_NORMAL'
    int medHead; 
    int medTail;
    int medMax;
    unsigned long medList[MAX_QUEUES+1];
    
	//Màximo = ' level > PRIORITY_NORMAL '
	int maxHead; 
    int maxTail;
    int maxMax;
    unsigned long maxList[MAX_QUEUES+1];
	
};

struct queue_d *queue;


/*
 * wait_queue_d:
 *     Lista encadeada que pode ser usada para threads que estão esperando 
 * por algum evento.
 */

struct wait_queue_d
{
	int size;
	struct thread_d *head;
	struct thread_d *tail;
};

struct wait_queue_d *wait_queue;




//
// == Prototypes ========
//



//++
// #deprecaed
// Deprecated functions.
// See: queue.c
int init_queue (struct queue_d *q);
int queue_insert_data (struct queue_d *q, unsigned long data, int type);
struct thread_d *queue_get_data ( struct queue_d *q, int type );
int queue_insert_head ( struct queue_d *q, unsigned long data, int type );
void ScanReadyQueue(struct queue_d *q);
void feed_ready_queue (struct queue_d *q, int type);
void show_queue_information(struct queue_d *q);
// --


#endif    




