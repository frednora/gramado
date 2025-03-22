// plib.c
// Process library.
// Created by Fred Nora.

#include <kernel.h>

/*
 * show_process_information:
 *     Mostra informa��es sobre os processos. 
 *     #todo: na verdade um aplicativo em user mode deve fazer esse trabalho
 * solicitando informa��es sobre cada processo atrav�s de chamadas.
 */

// Mostrar informa��es sobre os processos da lista.
// obs: as imagens s�o carregadas em endere�os virtuais diferentes
// e o endere�o mostrado � em rela��o ao diret�rio de p�ginas do kernel
// pois o kernel � que controla o posicionamento das imagens.

void show_process_information (void)
{
    struct process_d *p;
    int i=0;

    printk ("show_process_information: \n");

    for ( i=0; i<PROCESS_COUNT_MAX; i++ )
    {
        p = (void *) processList[i];

        if ( (void *) p != NULL && 
             p->used  == TRUE && 
             p->magic == 1234 )
        {
            //printk("\n");
            printk("\n=====================================\n");
            printk(">>[%s]\n", p->__processname);
            printk("PID=%d PPID=%d \n", p->pid,  p->ppid );
            printk("~Personality={%d}\n", p->personality);
            
            printk("image-base =%x image-size =%d \n", 
                p->Image, p->ImageSize );
            printk("heap-base  =%x heap-size  =%d \n", 
                p->HeapStart,  p->HeapSize );
            
            
            // #bugbug: It's very wrong!
            // See: create_process() in process.c
            //
            //printk("stack-base =%x stack-size =%d \n", 
            //    p->StackStart, p->StackSize );

            //printk("dir-pa=%x dir-va=%x \n", 
            //    p->DirectoryPA, p->DirectoryVA );

            printk("iopl=%d prio=%d state=%d \n", 
                p->rflags_iopl, p->priority, p->state );

            printk("syscalls = { %d }\n", p->syscalls_counter );
            
            printk("allocated memory = { %d Bytes }\n", 
                p->allocated_memory );
        }
    // Nothing.
    };

    refresh_screen();
}


// Usado pelo comando "current-process" no shell
void show_currentprocess_info (void)
{
    struct process_d  *Current;
    pid_t current_process = (pid_t) get_current_process();

    if ( current_process < 0 || 
         current_process >= PROCESS_COUNT_MAX )
    {
        //printk("show_process_information: current_process fail\n");
        return;
    }

// Struct
    Current = (void *) processList[current_process];
    if ((void *) Current == NULL){
        printk ("show_currentprocess_info: [FAIL] Current \n");
        return;
    }
    if (Current->magic != 1234)
        return;

    //Index.
    printk ("PID={%d} PPID={%d} UID={%d} GID={%d} \n",
        Current->pid, Current->ppid, Current->uid, Current->gid );

    //Name
    //printk ("Name={%s} \n", Current->name_address );
    printk ("Name={%s} \n", Current->name );
 
    //Image Address.
    printk ("ImageAddress={%x} \n", Current->Image );

    //Directory Address. *IMPORTANTE.
    //printk (">>DirectoryPA={%x} \n", Current->DirectoryPA );
    //printk (">>DirectoryVA={%x} \n", Current->DirectoryVA );

// Heap and stack.

    printk("HeapStart={%x}  HeapSize={%d KB}  \n", 
        Current->HeapStart, Current->HeapSize );
    printk("StackStart={%x} StackSize={%d KB} \n", 
        Current->StackStart, Current->StackSize );

    //...

    refresh_screen();
}

