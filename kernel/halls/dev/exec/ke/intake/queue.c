
// queue.c
// This routines are suspended.

// #bugbug
// Esse módulo não está bom. 
// precisa ser repensado e refeito.
// Possivelmente temos problemas de overflow.

#include <kernel.h>  

// init_queue:
//     Inicializa uma estrutura de fila.

int init_queue (struct queue_d *q)
{

    panic("init_queue: deprecated");

/*
    unsigned int i=0;

	// #importante
	// MAX_QUEUES é o número máximo de filas que pode haver na lista.
	// A lista contém o potneiro head de uma lista encadeada.
	
    q->standbyHead = 0; 
    q->standbyTail = 0;
    q->standbyMax  = MAX_QUEUES; 

    q->runningHead = 0; 
    q->runningTail = 0;
    q->runningMax  = MAX_QUEUES; 	
		
	//ready.
	q->readyHead = 0; 
    q->readyTail = 0;
    q->readyMax  = MAX_QUEUES; 	
	
	//waiting.
    q->waitingHead  = 0;
    q->waitingTail  = 0;
    q->waitingMax   = MAX_QUEUES; 
		
    q->blockedHead  = 0;
    q->blockedTail  = 0;
    q->blockedMax   = MAX_QUEUES; 
	
	
    q->zombieHead  = 0;
    q->zombieTail  = 0;
    q->zombieMax   = MAX_QUEUES; 

    q->deadHead  = 0;
    q->deadTail  = 0;
    q->deadMax   = MAX_QUEUES; 

    q->initializedHead  = 0;
    q->initializedTail  = 0;
    q->initializedMax   = MAX_QUEUES; 
	
	
  //Cooperação, sistema.
    q->syscoopHead = 0; 
    q->syscoopTail = 0;
    q->syscoopMax = MAX_QUEUES;


	//Concorrência, sistema.
    q->sysconcHead = 0; 
    q->sysconcTail = 0;
    q->sysconcMax = MAX_QUEUES;


	//Concorrência, usuário.
    q->userconcHead = 0; 
    q->userconcTail = 0;
    q->userconcMax = MAX_QUEUES; 
	
	//Cooperação, usuário.
    q->usercoopHead = 0; 
    q->usercoopTail = 0;
    q->usercoopMax = MAX_QUEUES;
	
	//realtime.
	q->realtimeHead = 0; 
    q->realtimeTail = 0;
    q->realtimeMax  = MAX_QUEUES; 

	
	//default.
    q->defaultHead  = 0;
    q->defaultTail  = 0;
    q->defaultMax   = MAX_QUEUES; 


	i=0;
	while(i <= q->standbyMax){
	    q->standbyList[i] = (unsigned long) 0; 
		++i;
	};	
	
	i=0;
	while(i <= q->runningMax){
	    q->runningList[i] = (unsigned long) 0; 
		++i;
	};	

	
    //Ready list.
	i=0;
	while(i <= q->readyMax){
	    q->readyList[i] = (unsigned long) 0; 
		++i;
	};
	
	//Wainting list.
	i=0;
    while(i <= q->waitingMax){
		q->waitingList[i] = (unsigned long) 0;	
		++i;
	};	
	
	i=0;
    while(i <= q->blockedMax){
		q->blockedList[i] = (unsigned long) 0;	
		++i;
	};		
	
	i=0;
    while(i <= q->zombieMax){
		q->zombieList[i] = (unsigned long) 0;	
		++i;
	};		
	
	i=0;
    while(i <= q->deadMax){
		q->deadList[i] = (unsigned long) 0;	
		++i;
	};	
	
	i=0;
    while(i <= q->initializedMax){
		q->initializedList[i] = (unsigned long) 0;	
		++i;
	};		
	
    //syscoop.
	i=0;
	while(i <= q->syscoopMax){
	    q->syscoopList[i] = (unsigned long) 0; 
		++i;
	};

	//usercoop.
	i=0;
	while(i <= q->usercoopMax){
	    q->usercoopList[i] = (unsigned long) 0; 
		++i;
	};

	//sysconc.
	i=0;
	while(i <= q->sysconcMax){
	    q->sysconcList[i] = (unsigned long) 0; 
		++i;
	};
     
	//userconc. 
	i=0;
	while(i <= q->userconcMax){
	    q->userconcList[i] = (unsigned long) 0; 
		++i;
	};

	
    //Realtime list.
	i=0;
	while(i <= q->realtimeMax){
	    q->realtimeList[i] = (unsigned long) 0; 
		++i;
	};
	
	//Default list.
	i=0;
    while(i <= q->defaultMax){
		q->defaultList[i] = (unsigned long) 0;	
		++i;
	};

done:
*/

    return 0;
} 


// #bugbug overflow
int queue_insert_data (struct queue_d *q, unsigned long data, int type)
{
    panic ("queue_insert_data: deprecated\n"); 

/*
    if ((void*) q == NULL)
    {
        panic ("queue_insert_data:"); 
    }

    switch (type)
    {
        case QUEUE_STANDBY:
		    goto standbyInsertData;
            break;		

        case QUEUE_RUNNING:
		    goto runningInsertData;
            break;		
			
	    //tipo ready
	    case QUEUE_READY:
		    goto readyInsertData;
		    break;
		
		//tipo waiting
		case QUEUE_WAITING:
		    goto waitingInsertData;
		    break;		        
		
        case QUEUE_BLOCKED:
		    goto blockedInsertData;
            break;			
		
        case QUEUE_ZOMBIE:
		    goto zombieInsertData;
            break;	
			
        case QUEUE_DEAD:
		    goto deadInsertData;
            break;
			
        case QUEUE_INITIALIZED:
		    goto initializedInsertData;
            break;				
		
		case QUEUE_SYSCOOP:
		    goto syscoopInsertData;
            break;
			
        case QUEUE_USERCOOP: 
		    goto usercoopInsertData;
            break;
			
        case QUEUE_SYSCONC:
		    goto sysconcInsertData;
            break;
			
        case QUEUE_USERCONC:
		    goto userconcInsertData;
            break;		
		
		//tipo realtime
		case QUEUE_REALTIME:
		    goto realtimeInsertData;
		    break;
		
		//qualquer tipo.
		default:
		    goto defaultInsertData;
		    break;
	};
	
//STANDBY	
standbyInsertData:
    q->standbyList[q->standbyTail++] = (unsigned long) data;
	if(q->standbyTail >= q->standbyMax){
	    q->standbyTail = q->standbyHead;
	}
    goto done;

//running	
runningInsertData:
    q->runningList[q->runningTail++] = (unsigned long) data;
	if(q->runningTail >= q->runningMax){
	    q->runningTail = q->runningHead;
	}
    goto done;
		
	
//Ready Threads. fila do dispatcher	
readyInsertData:
    q->readyList[q->readyTail++] = (unsigned long) data;
	if(q->readyTail >= q->readyMax){
	    q->readyTail = q->readyHead;
	}
    goto done;	
	
//Waiting Threads.	
waitingInsertData:
    q->waitingList[q->waitingTail++] = (unsigned long) data;
	if(q->waitingTail >= q->waitingMax){
	    q->waitingTail = q->waitingHead;
	}
    goto done;	

//blocked	
blockedInsertData:
    q->blockedList[q->blockedTail++] = (unsigned long) data;
	if(q->blockedTail >= q->blockedMax){
	    q->blockedTail = q->blockedHead;
	}
    goto done;
	
	
//zombie	
zombieInsertData:
    q->zombieList[q->zombieTail++] = (unsigned long) data;
	if(q->zombieTail >= q->zombieMax){
	    q->zombieTail = q->zombieHead;
	}
    goto done;
	
//dead	
deadInsertData:
    q->deadList[q->deadTail++] = (unsigned long) data;
	if(q->deadTail >= q->deadMax){
	    q->deadTail = q->deadHead;
	}
    goto done;

//initialized
initializedInsertData:
    q->initializedList[q->initializedTail++] = (unsigned long) data;
	if(q->initializedTail >= q->initializedMax){
	    q->initializedTail = q->initializedHead;
	}
    goto done;

syscoopInsertData: 
    q->syscoopList[q->syscoopTail++] = (unsigned long) data;
	if(q->syscoopTail >= q->syscoopMax){
	    q->syscoopTail = q->syscoopHead;
	}
    goto done;

usercoopInsertData:
    q->usercoopList[q->usercoopTail++] = (unsigned long) data;
	if(q->usercoopTail >= q->usercoopMax){
	    q->usercoopTail = q->usercoopHead;
	}
    goto done;

sysconcInsertData:
    q->sysconcList[q->sysconcTail++] = (unsigned long) data;
	if(q->sysconcTail >= q->sysconcMax){
	    q->sysconcTail = q->sysconcHead;
	}
    goto done;
	
userconcInsertData:
    q->userconcList[q->userconcTail++] = (unsigned long) data;
	if(q->userconcTail >= q->userconcMax){
	    q->userconcTail = q->userconcHead;
	}
    goto done;
	
//Real Time Threads.	
realtimeInsertData:
    q->realtimeList[q->realtimeTail++] = (unsigned long) data;
	if(q->realtimeTail >= q->realtimeMax){
	    q->realtimeTail = q->realtimeHead;
	}
    goto done;

//Default queue.
defaultInsertData:
    q->defaultList[q->defaultTail++] = (unsigned long) data;
	if(q->defaultTail >= q->defaultMax){
	    q->defaultTail = q->defaultHead;
	}
    goto done;
done:
*/
    return 0;
}

// queue_get_data:
//    Pegando o head de uma das listas de ponteiros.
//    Isso consome o elemento da lista.

struct thread_d *queue_get_data ( struct queue_d *q, int type )
{

    panic ("queue_get_data: deprecated"); 

/*
    struct thread_d *Thread;


    if ( (void *) q == NULL )
    {
        panic ("queue_get_data:"); 
    }

    switch (type)
    {
        case QUEUE_STANDBY:
		    goto standbyGetData;
            break;		

        case QUEUE_RUNNING:
		    goto runningGetData; 
            break;		
			
	    //tipo ready
	    case QUEUE_READY:
		    goto readyGetData; 
		    break;
		
		//tipo waiting
		case QUEUE_WAITING:
		    goto waitingGetData; 
		    break;		        
		
        case QUEUE_BLOCKED:
		    goto blockedGetData;  
            break;			
		
        case QUEUE_ZOMBIE:
		    goto zombieGetData; 
            break;	
			
        case QUEUE_DEAD:
		    goto deadGetData;  
            break;
			
        case QUEUE_INITIALIZED:
		    goto initializedGetData;  
            break;	
	
        case QUEUE_SYSCOOP:
		    goto syscoopGetData;
            break;
			
        case QUEUE_USERCOOP: 
		    goto usercoopGetData;
            break;
			
        case QUEUE_SYSCONC:
		    goto sysconcGetData;
            break;
			
        case QUEUE_USERCONC:
		    goto userconcGetData;
            break;		
			
	    //tipo realtime
	    case QUEUE_REALTIME:
		    goto realtimeGetData;
		    break;
		
		//qualquer tipo.
		default:
		    goto defaultGetData;
		    break;
	};

	
	//
	// rotinas.
	//
	
	
standbyGetData:
    Thread = (void *) q->standbyList[q->standbyHead];	
    q->standbyList[q->standbyHead] = 0;
    goto done;
	
//#bugbug: Haha ... esse tipo não faz sentido.	
runningGetData:
    Thread = (void *) q->runningList[q->runningHead];
    q->runningList[q->runningHead] = 0;
    goto done;
	
readyGetData:
    Thread = (void *) q->readyList[q->readyHead];
    q->readyList[q->readyHead] = 0;
    goto done;
	
waitingGetData:
    Thread = (void *) q->waitingList[q->waitingHead];
    q->waitingList[q->waitingHead] = 0;
    goto done;
	
blockedGetData:
    Thread = (void *) q->blockedList[q->blockedHead];
	q->blockedList[q->blockedHead] = 0;
    goto done;
	
zombieGetData:
    Thread = (void *) q->zombieList[q->zombieHead];
    q->zombieList[q->zombieHead] = 0;
    goto done;
	
deadGetData:
    Thread = (void *) q->deadList[q->deadHead];
    q->deadList[q->deadHead] = 0;
    goto done;
	
initializedGetData:
    Thread = (void *) q->initializedList[q->initializedHead];
	q->initializedList[q->initializedHead] = 0;
    goto done;
	
syscoopGetData: 
    Thread = (void *) q->syscoopList[q->syscoopHead];	
    q->syscoopList[q->syscoopHead] = 0;
    goto done;
	
usercoopGetData:
    Thread = (void *) q->usercoopList[q->usercoopHead];
    q->usercoopList[q->usercoopHead] = 0;
    goto done;
	
sysconcGetData:
    Thread = (void *) q->sysconcList[q->sysconcHead];	
    q->sysconcList[q->sysconcHead] = 0;
    goto done;
	
userconcGetData:
    Thread = (void *) q->userconcList[q->userconcHead];
    q->userconcList[q->userconcHead] = 0;
    goto done;
	
realtimeGetData:
    Thread = (void *) q->realtimeList[q->realtimeHead];
    q->realtimeList[q->realtimeHead] = 0;
    goto done;
	
defaultGetData:
    Thread = (void *) q->defaultList[q->defaultHead];
    q->defaultList[q->defaultHead] = 0;
    goto done;

// Done.
done:

    //feed_ready_queue(q,1);    //feed head.
	
    return (struct thread_d *) Thread;
    
*/
    return NULL;
}


int 
queue_insert_head ( 
    struct queue_d *q, 
    unsigned long data, 
    int type )
{

    panic ("queue_insert_head: deprecated"); 

/*
    if ( (void *) q == NULL )
    {
        panic ("queue_insert_head:"); 
    }

    switch (type)
    {
        case QUEUE_STANDBY:
		    goto standbyInsertData; 
            break;		

        case QUEUE_RUNNING:
		    goto runningInsertData; 
            break;		
			
	    //tipo ready
	    case QUEUE_READY:
		    goto readyInsertData; 
		    break;
		
		//tipo waiting
		case QUEUE_WAITING:
		    goto waitingInsertData; 
		    break;		        
		
        case QUEUE_BLOCKED:
		    goto blockedInsertData; 
            break;			
		
        case QUEUE_ZOMBIE:
		    goto zombieInsertData; 
            break;	
			
        case QUEUE_DEAD:
		    goto deadInsertData;  
            break;
			
        case QUEUE_INITIALIZED:
		    goto initializedInsertData; 
            break;	
			
        case QUEUE_SYSCOOP:
		    goto syscoopInsertData;
            break;	
			
        case QUEUE_USERCOOP: 
		    goto usercoopInsertData;
            break;
			
        case QUEUE_SYSCONC:
		    goto sysconcInsertData;
            break;
			
        case QUEUE_USERCONC:
		    goto userconcInsertData;
            break;		
			
	    //tipo ready
	    case QUEUE_REALTIME:
		    goto realtimeInsertData;
		    break;
		
		//qualquer tipo.
		default:
		    goto defaultInsertData;
		    break;
	};
	
standbyInsertData:
    q->standbyList[q->standbyHead] = (unsigned long) data;
    goto done;

runningInsertData:
    q->runningList[q->runningHead] = (unsigned long) data;
    goto done;
	
readyInsertData:
    q->readyList[q->readyHead] = (unsigned long) data;
    goto done;	
	
waitingInsertData:
    q->waitingList[q->waitingHead] = (unsigned long) data;
    goto done;	
	
blockedInsertData:
    q->blockedList[q->blockedHead] = (unsigned long) data;
    goto done;	
	
zombieInsertData:
    q->zombieList[q->zombieHead] = (unsigned long) data;
    goto done;	
	
deadInsertData:
    q->deadList[q->deadHead] = (unsigned long) data;
    goto done;	

initializedInsertData:
    q->initializedList[q->initializedHead] = (unsigned long) data;
    goto done;	
	
syscoopInsertData: 
    q->syscoopList[q->syscoopHead] = (unsigned long) data;
    goto done;

usercoopInsertData:
    q->usercoopList[q->usercoopHead] = (unsigned long) data;
    goto done;

sysconcInsertData:
    q->sysconcList[q->sysconcHead] = (unsigned long) data;
    goto done;

userconcInsertData:
    q->userconcList[q->userconcHead] = (unsigned long) data;
    goto done;

realtimeInsertData:
    q->realtimeList[q->realtimeHead] = (unsigned long) data;
    goto done;
	
defaultInsertData:
    q->defaultList[q->defaultHead] = (unsigned long) data;
    goto done;

done:
*/
    return 0;
}

// ScanReadyQueue:
//     Procura na fila de threads ready por threads com timeout vencido.
//     e põe na frente da fila.

void ScanReadyQueue(struct queue_d *q)
{

    panic("ScanReadyQueue: deprecated");

/*
    // loop
    register int i=0;

    struct thread_d *t;

	if ((void*)q == NULL){
	    printk("ScanReadyQueue:\n");
	    return;
	}
		
	//printk("head={%d} tail={%d}\n", q->readyHead, q->readyTail);

    while ( i < q->readyMax)
    {
        t = (void *) q->readyList[i];
        if( (void *) t != NULL && t->used == 1 && t->magic == 1234 )
		{
		    if( t->waitingCount >= t->waiting_limit )
            {
			    t->waitingCount = 0;
				t->priority = PRIORITY_MAX;
				queue_insert_head(queue, (unsigned long) t, 1);
				return;
            }
		}

		++i;
	};
*/
}


void feed_ready_queue (struct queue_d *q, int type)
{

    panic ("feed_ready_queue: deprecated");

/*
    // loop
    register int i=0;

    unsigned long p=0;

    struct thread_d *t;


	if ((void*)q == NULL)
	{
	    panic ("feed_ready_queue:");
	}
	
    switch(type)
    {
	    //Alimenta apenas a 'head' com a thread de maior prioridade.
        case 1:
		    goto feed_head; 
            break;
		
		//Usa os slots com todas as tarefas pra alimentar a fila de ready.
        case 2:
		    goto feed_with_threadList; 
            break;
			
        //alimenta a fila do dispatcher(ready) com elementos de outra lista.
        case 3: 
            break;

		//alimenta a fila do dispatcher(ready) com elementos de outra lista.	
        case 4: 
            break;
		
		default:
		    goto feed_head;
		    break;
	};	

//
// feed_head:	 
//     Alimenta apenas a 'head' com a thread de maior prioridade.
//
feed_head:
    
	i = 1;
    
    do
	{	
	    t = (void *) q->readyList[i];
    
	    if( (void *) t != NULL && t->used == 1 && t->magic == 1234)
	    {
		    if( t->priority == PRIORITY_HIGH || t->priority == PRIORITY_MAX )
            {
			    if(t->priority > p)
				{
				    q->readyList[i] = (unsigned long) 0;
				    p = t->priority;
				    queue_insert_head(queue, (unsigned long) t, QUEUE_READY);
				};
				goto done;
            };			
		}; 
		++i;
	}while( i < q->readyMax );
    
	goto done;	
	
	
//usa os slots com todas as tarefas pra alimentar a fila de ready.	
feed_with_threadList:	
	
	i = 1;
    
    do
    {
	    t = (void *) threadList[i];
        if( (void *) t != NULL && t->used == 1 && t->magic == 1234 )
		{
		    if( t->priority == PRIORITY_HIGH || t->priority == PRIORITY_MAX )
            {
			    if( t->priority >= p)
				{
				    q->readyList[i] = (unsigned long) 0;
				    p = t->priority;
				    queue_insert_data(queue, (unsigned long) t, QUEUE_READY);
				}
				goto done;
            }
		}
 
        ++i;

    }while( i < THREAD_COUNT_MAX );
 
    goto done;	
	
done:
*/
    return;
}

// show_queue_information:
//      Mostra informações sobre a queue.
//      #todo: put this in a info file.
void show_queue_information(struct queue_d *q)
{
    panic("show_queue_information: deprecated");
/*
    register int i=0;

    if ((void*) q == NULL)
    {
        printk("show_queue_information error: Struct\n");
        return;
    }

    printk("Ready queue Information:\n");	
    printk("Head={%d} Tail={%d}\n", q->readyHead, q->readyTail);

    while (i < q->readyMax)
    {
        q = (void *) q->readyList[i];
        if ( (void *) q != NULL ){
            printk("Index={%d} Struct={%x}\n", i, (void*) q );
        } 
        
        ++i;
    };

	//@todo: Informações de outras filas.
*/
}


//
// End.
//














 




