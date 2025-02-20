
//#define	NULL	((void *)0)


#include <types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


#include <rtl/gramado.h>



static char *ohboy_argv[] = { 
    "-flag1", 
    "-flag2", 
    "-flag3", 
    "-flag4", 
    NULL 
};

static char *ohboy_environ[] = { 

    "DISPLAY=kgws-or-gwssrv",  //#todo
    "EDITOR=gramcode",
    "HOME=/HOME",
    "HOSTNAME=gramado",
    "LANG=en-us",
    "LANG2=pt-br",
    "LANGX=en-br",
    "OSTYPE=gramado",   //#todo
    "PATH=/:/PROGRAMS:/GRAMADO",
    "PS1=~$",
    "PS2=:>",
    "PWD=/",             //#bugbug: This is the working directory.
    "SHELL=gdeshell",
    "TERM=noraterm",     //#bugbug
    "TMP=/TMP",
    "UID=no-id",         //#todo
    "USER=anon",  
    NULL 
};


// system call.
void *ohboy_system_call ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
    long __Ret = 0;

    // System interrupt.

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x80), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}


void testThread(void)
{
    while (1){
        //debug_print("THREAD 2\n");
        //printf("2"); 
        //fflush(stdout);
    };
}



#define MSG_KEYDOWN       20
#define MSG_KEYUP         21
#define MSG_SYSKEYDOWN    22
#define MSG_SYSKEYUP      23

#define VK_F1    0x3B  //59    // No DOS é 0x170.  
#define VK_F2    0x3C  //60 
#define VK_F3    0x3D  //61 
#define VK_F4    0x3E  //62 

// local
int 
ohboyProcedure ( 
    void *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{

    switch (msg)
    {
        // 20 = MSG_KEYDOWN
        case MSG_KEYDOWN:
            switch (long1)
            {
                default:
                    printf("%c",long1); fflush(stdout);
                    break;
            };
            break;

  // 22 = MSG_SYSKEYDOWN
        case MSG_SYSKEYDOWN:
            switch (long1)
            {
                case VK_F1: 
                    printf ("F1\n");
                    break;

                // Create rectangle.
                // This rectangle willbe used by the compositor.
                //case VK_F2: 
                    //gramado_system_call(897,0,0,0);
                    //break;

            };
            break;
    };

    return 0;
}


void main(void)
{
    
    // #todo
    // This is a place for 

    ohboy_system_call(0,0,0,0);
    //ohboy_system_call(0,0,0,0);
    //ohboy_system_call(0,0,0,0);
    //ohboy_system_call(0,0,0,0);
    
    
    // Get pid
    int pid = (int) ohboy_system_call(85,0,0,0);

    //get Heap address given the pid.
    void *heap;
    //heap = (void*)ohboy_system_call(184,0,0,0);
    heap = (void*)gramado_system_call(184,pid,pid,pid);  //#test: Tentando usar a libc

    // Heap not valid.
    if ( (void*) heap == NULL )
        ohboy_system_call(65,'N',0,0);

    // Valid heap.
    if ( (void*) heap != NULL )
        ohboy_system_call(65,'Y',0,0);

    //ohboy_system_call(49,0,0,0);

    // console put char
    // ohboy_system_call(65,'x',0,0);

    //asm("int $3");
    //while(1){}

    // reboot
    //ohboy_system_call(110,0,0,0);

    // Debug on serial
    //char *s="fred";
    //ohboy_system_call(289,s,s,s);

    // Exit
    //ohboy_system_call(70,0,0,0);
    //while(1){
    //    ohboy_system_call(289,s,s,s);
    //}

    // List files.
    //ohboy_system_call(173,0,0,0);
  
  
  
// =================================================


//
// Initializing the ring3 libc.
//

    // Environment.
    environ = ohboy_environ;

    // See: stdlib/stdlib.c
    ohboy_system_call(65,'1',0,0);
    libcInitRT();

    // See: stdio/stdio.c
    ohboy_system_call(65,'2',0,0);
    stdioInitialize();

    //#fail
    //debug_print("Fred\n");

    //#fail
    //while(1)
        //printf("Fred\n");

    //#fail
    //putchar('z');
    //fflush(stdout);

    //gramado_system_call (82,0,0,0);
    
    ohboy_system_call(65,'z',0,0);
    
    // ok
    //rtl_reboot();

// tests:

    debug_print("-- init: tests ------------------\n");

    if ( (void*) stdin == NULL )
        debug_print("init.bin: stdin fail\n");

    if ( (void*) stdout == NULL )
        debug_print("sm.bin: stdout fail\n");

    if ( (void*) stderr == NULL )
        debug_print("sm.bin: stderr fail\n");



//
// printf()
//

    //long value=1234;
    //printf ("\n");
    //printf ("init.bin: Testing printf()\n");
    
    // #bugbug
    // Wrong value !!
    //printf ("init.bin: Value=%d\n",value);


//
// write
//

    //printf ("\n");
    //printf ("sm.bin: Testing write()\n");
    //char *s = "Fred";
    //putc('v',stdout);
    //write ( fileno(stdin), s, 4 );
    //write ( fileno(stdout), s, 4 );
    //write ( fileno(stderr), s, 4 );
    //fflush(stdout);
    //while(1){}


//
// read
//
    //printf ("\n");
    //printf ("sm.bin: Testing read()\n");
    //char ReadBuffer[32];
    //read( fileno(stdin), ReadBuffer, 4 );
    //read( fileno(stdout), ReadBuffer, 4 );
    //read( fileno(stderr), ReadBuffer, 4 );


// =================================================

    // refresh screen
    ohboy_system_call(11,0,0,0);

    //#debug
    //while(1){}
    
    // ok, a calling conventions funcionou,
    // os parametros estao em ordem.
    //ohboy_system_call(4321,0xa,0xb,0xc);

    //printf ("Testing fflush()");
    //fflush(stdout);
    //while(1){}
    
    //close(0); close(1); close(2);

    // #todo: 
    // Trabalhando na implementação dessa rotina em ring0.
    // See: x64init.c, pages.c, process.c ...


    //rtl_clone_and_execute("reboot.bin");


//
//
//

    asm ("int $199 \n");


// Unlock the taskswitching support.
// Unlock the scheduler embedded into the base kernel.
// Only the init process is able to do this.

    gramado_system_call (641,0,0,0);
    gramado_system_call (643,0,0,0);



    // It is working ... but the crt0 in rtl just have an 'int 3'
    // #todo #bugbug 
    // Actually, int ring 0, is spawning the target thread.
    // That is not what we want ... 
    // We do not have the taskswching enabled yet.
    
    // #bugbug
    // O problema é que precisamos configurar corretamente a
    // thread que vamos rodar.
    // quando deixamos o task switching efetuar o spawn,
    // não estamos indo para ela pela primeira vez corretamente 
    
    // #fail
    //rtl_clone_and_execute("reboot.bin"); 
    //while(1){}


//
// Thread
//

    char thread_stack[1024];
    void *t1;

    //#todo: IN: address, stack, name.
    t1 = (void *) rtl_create_thread(
                      &testThread,
                      &thread_stack[1023], 
                      "Ring3Thread1" );
    //#todo: Ainda não é possível, pois não temos timer.
    rtl_start_thread(t1);






//
// Breakpoint
//

    //printf ("\n");
    printf ("ohboy.c: Hello world !\n");
    //printf ("\n");






//
// Test
//


    // fail!
    //rtl_clone_and_execute("sm2.bin"); 
    //rtl_clone_and_execute("reboot.bin"); 
    //rtl_clone_and_execute("gdeshell.bin"); 
    //rtl_clone_and_execute("shell.bin"); 


//
// Window server
//
    
    // #todo
    // we need to work on the refresh screen support.
    
    rtl_clone_and_execute("gwssrv.bin"); 


    
//exit:
    //return 0;


// interna
// Uma interrupção para habilitar as interrupções mascaráveis.
// So depois disso a interrupção de timer vai funcionar.
// This is Intel processors.
// [32~255]

    // ok, funciona. Podemos usar o teclado.
    //asm ("int $199 \n");


//
// Create process
//

    /*
    char pName[32];
    sprintf(pName,"NewProcessR3");
    pName[31] = 0;
    // Create process
    gramado_system_call( 
        73, 
        (unsigned long) &pName[0],
        3,  //priority
        0 );
    */
    
    // ok.
    //rtl_create_process("NewProcessR3");

// Show info about all processes.

    //gramado_system_call(82,0,0,0);



// Enable prompt.
    //gramado_system_call(898,0,0,0);

// Disable prompt
    //gramado_system_call(899,0,0,0);

// Create the rectangle
    while(1){
        gramado_system_call(897,0,0,0);
    }


//
// Message loop
//

//=================================

    rtl_focus_on_this_thread();

    while (1){

        if ( rtl_get_event() == TRUE )
        {  
            // Podemos chamar mais de um diálogo
            // Retorna TRUE quando o diálogo chamado 
            // consumiu o evento passado à ele.
            // Nesse caso chamados 'continue;'
            // Caso contrário podemos chamar outros diálogos.

            ohboyProcedure ( 
                (void*) RTLEventBuffer[0], 
                RTLEventBuffer[1], 
                RTLEventBuffer[2], 
                RTLEventBuffer[3] );
        }
    };
//=================================





//
// Hang
//

    for(;;){
    };

// #bugbug
// This is printing wrong messages into the serial
// console.

    //char *t1String = "THREAD 1\n";
    while (1){
        //debug_print("THREAD 1 \n");
        //debug_print(t1String);
        //printf("1"); 
        //fflush(stdout);
    }

// Return to marginal.asm
}

















