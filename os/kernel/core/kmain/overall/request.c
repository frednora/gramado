// request.c
// Kernel request.
// A kind of defered dialog with the kernel.
// The kernel will respond it right after the context
// been saved, for safety.
// Created by Fred Nora.

#include <kernel.h>    

/*
 * createDeferredKernelRequest:
 *     Cria o request, que será atendido depois.
 *     Isso eh chamado no serviço 70, por exemplo.
 *     #bugbug: Pode haver sobreposicao de requests?
 */

int 
createDeferredKernelRequest ( 
    unsigned long number, 
    int status, 
    int timeout,
    int target_pid,
    int target_tid,
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{

    debug_print ("createDeferredKernelRequest:\n");

    if (number > KERNEL_REQUEST_MAX){
        debug_print ("createDeferredKernelRequest: number not valid\n");
        return 1;
        //return -1;  //#todo: Use this one if it is possible.
    }

    kernel_request = number;

    REQUEST.kernel_request = number;
    REQUEST.status         = status;


    if (timeout < 0 ){
        REQUEST.timeout = 0;
    }else{
        REQUEST.timeout = timeout;
    };


    REQUEST.target_pid = target_pid;
    REQUEST.target_tid = target_tid;

// #obs
// Atenção com isso.
    REQUEST.msg    = (int) msg;
    REQUEST.long1  = (unsigned long) long1;
    REQUEST.long2  = (unsigned long) long2;

// Extra.
// rever isso depois.
    REQUEST.long3 = 0;
    REQUEST.long4 = 0;
    REQUEST.long5 = 0;
    REQUEST.long6 = 0;

    return 0;
}

void clearDeferredKernelRequest(void)
{
    REQUEST.kernel_request = 0;
    REQUEST.status     = 0;
    REQUEST.timeout    = 0;
    REQUEST.target_pid = 0;
    REQUEST.target_tid = 0;

    REQUEST.msg    = 0;
    REQUEST.long1  = 0;
    REQUEST.long2  = 0;
    REQUEST.long3  = 0;
    REQUEST.long4  = 0;
    REQUEST.long5  = 0;
    REQUEST.long6  = 0;

// The number of the current request.
    kernel_request = 0;
}

/*
 * processDeferredKernelRequest:
 *    Trata os requests do Kernel.
 *    Isso eh chamado durante a a fase kernel 
 *    de uma interrupçao de timer.
 *    São serviços que terão seu atendimento atrazado até pouco antes de 
 * retornar da interrupção do timer.
 *    Sendo assim, um programa em user mode pode solicitar um request através
 * de uma system call, sendo a system call transformada em request, adiando
 * o atendimento.
 *   O atendimento do request pendende será feito com o auxílio da estrutura
 * de request.
 *   Um request será atendido somente quando o timeout zerar. (defered)
 */
int processDeferredKernelRequest(void)
{
// Processing the defered kernel request.

// Targets
    struct process_d  *Process;
    struct thread_d   *Thread;
    int PID=-1;
    int TID=-1;
    unsigned long r=0;    //Número do request.
    unsigned long t=0;    //Tipo de thread. (sistema, periódica...).

// targets
    PID = (int) REQUEST.target_pid;
    TID = (int) REQUEST.target_tid;

// Timeout
    if ( REQUEST.timeout > 0 )
    {
        REQUEST.timeout--;
        return 1;
        //return -1; // use this one if it is possible.
    }

// Filter

    if ( PID < 0 || PID > PROCESS_COUNT_MAX ){
        Process = NULL;
    }else{
        Process = (void *) processList[PID];
    };

    if ( TID < 0 || TID > THREAD_COUNT_MAX ){
        Thread = NULL;
    }else{
        Thread = (void *) threadList[TID];
    };


// Number

    r = kernel_request;

    if (r >= KERNEL_REQUEST_MAX){
        // msg ...
        printk ("processDeferredKernelRequest: Invalid request\n");
        return -1;
    }


    switch (r){

        //0 - request sem motivo, (a variável foi negligenciada).
        // Ou ainda estamos checando se tem um resquest,
        // se não tem apenas retornamos.
        case KR_NONE:
            //debug_print ("request: KR_NONE\n");
            //nothing for now.       
            break;


        //1 - Tratar o tempo das threads de acordo com o tipo.  
        //#importante: De acordo com o tipo de thread.
        case KR_TIME:
            //debug_print ("processDeferredKernelRequest: KR_TIME\n");
            panic       ("processDeferredKernelRequest: KR_TIME\n");
            break;


        //2 - faz a current_thread dormir. 
        case KR_SLEEP:
            //debug_print ("processDeferredKernelRequest: block a thread\n");
            //do_thread_blocked ( (int) REQUEST.target_tid );
            break;


        //3 - acorda a current_thread.
        case KR_WAKEUP:
            //debug_print ("processDeferredKernelRequest: wake up a thread\n");
            //wakeup_thread ( (int) REQUEST.target_tid );
            break;


        //4
        case KR_ZOMBIE:
            //debug_print ("processDeferredKernelRequest: do thread zombie\n");
            //do_thread_zombie ( (int) REQUEST.target_tid );
            break;

        //5 - start new task.
        //Uma nova thread passa a ser a current, para rodar pela primeira vez.
        //Não mexer. Pois temos usado isso assim.	
        case KR_NEW:
            //debug_print ("processDeferredKernelRequest: Start a new thread\n");
            //Start a new thread. 
            if (start_new_task_status == 1)
            {
                current_thread = start_new_task_id;
            }
            break;


        //6 - torna atual a próxima thread anunciada pela atual.
        case KR_NEXT:
            //debug_print ("processDeferredKernelRequest: KR_NEXT\n");
            panic       ("processDeferredKernelRequest: KR_NEXT\n");
            break;


        //7 - tick do timer.
        case KR_TIMER_TICK:
            //debug_print ("processDeferredKernelRequest: KR_TIMER_TICK\n");
            panic       ("processDeferredKernelRequest: KR_TIMER_TICK\n");
            break;

        //8 - limite de funcionamento do kernel.
        case KR_TIMER_LIMIT:
            //debug_print ("processDeferredKernelRequest: KR_TIMER_LIMIT\n");
            panic       ("processDeferredKernelRequest: KR_TIMER_LIMIT\n");
            break;


        // 9 - Checa se ha threads para serem inicializadas e 
        // inicializa pelo método spawn.
        // obs: 
        // Se spawn retornar, continua a rotina de request. 
        // Sem problemas.
        case KR_CHECK_INITIALIZED:
            //debug_print ("processDeferredKernelRequest: Check for standby\n");
            //schedi_check_for_standby ();
            break;


        //#todo
        //Chama o procedimento do sistema.
        // ?? args ??	
        // o serviço 124 aciona esse request.
        case KR_DEFERED_SYSTEMPROCEDURE:
            //debug_print ("processDeferredKernelRequest: defered system procedure [TODO]\n");
            //system_procedure ( REQUEST.window, REQUEST.msg, REQUEST.long1, REQUEST.long2 );
            break;


        //exit process
        case 11:
            //debug_print ("processDeferredKernelRequest: Exit process\n");
            //exit_process ( (int) REQUEST.target_pid, (int) REQUEST.long1 );
            break;


        // exit thread.
        // Sairemos da thread, mas se for a thread de controle, 
        // também sairemos do processo.
        case 12:
            //debug_print ("processDeferredKernelRequest: [12] Exit thread\n");
            //printk      ("request: [12] [DEBUG] Exiting thread %d\n", 
            //    REQUEST.target_tid);
            //do_request_12( (int) REQUEST.target_tid );
            break;


        //make target porcess current
        //cuidado.
        case 13:
            //debug_print ("processDeferredKernelRequest: Get current process\n");
            //current_process = REQUEST.target_pid;
            set_current_process(REQUEST.target_pid);
            break;


        //make target thread current
        //cuidado.
        case 14:
            //debug_print ("processDeferredKernelRequest: Get current thread\n");
            current_thread = REQUEST.target_tid;
            break;


        case 15:
            //debug_print ("processDeferredKernelRequest: Wait for a reason \n");
            //wait_for_a_reason ( REQUEST.target_tid, (int) REQUEST.long1  );
            break;

		// todo: 
		// Tratar mais tipos.	
		//...
		
		// clone and execute process.
        case 111:
            //current_process = REQUEST.target_pid;
            set_current_process(REQUEST.target_pid);
            current_thread  = REQUEST.target_tid;
            
            // IN: file name, parent pid, clone flags.
            return (void *) copy_process( 
                                 (const char *) REQUEST.long1, 
                                 (pid_t) get_current_process(),//current_process,
                                 (unsigned long) REQUEST.long2 );
            break;

        default:
            debug_print ("processDeferredKernelRequest: Default\n");  
            printk      ("processDeferredKernelRequest: Default request {%d}\n", r );
            break;
    };


// Done:
// Essas finalizações aplicam para todos os requests.
    clearDeferredKernelRequest();
    kernel_request = (unsigned long) 0;  

// Ok.
    return 0;
}

