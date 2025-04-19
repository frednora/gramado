
// tasks.h 
// #todo
// Some deprecated data here.

#ifndef __KE_TASKS_H
#define __KE_TASKS_H    1

//new task variables.
unsigned long start_new_task_status;
int start_new_task_id;
unsigned long start_new_task_address;
 
/*
 * quantum 
 * Quanto tempo fica rodando uma tarefa ate ser interrompida.
 */
unsigned long quantum;
unsigned long current_quantum;
unsigned long next_quantum;

//
// outras
//
unsigned long task_waiting;   //?espera
unsigned long preempted_task;
unsigned long running_tasks;   //numero de tarefas rodando. //esta em estrutura agora.
unsigned long task_count;      //contador de tarefas criadas.


#endif    






