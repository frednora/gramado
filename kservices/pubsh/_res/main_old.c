/*
 * File: main.c
 * 
 * return true;
 */


// rtl
#include <types.h>
#include <stdio.h>
 

//internal
//check the flag and read the lines.
char *__get_commandline (void);
char *__get_commandline (void)
{

    char *shared_flag   = (char *) (0xC0800000 -0x210);   // flag
    char *shared_memory = (char *) (0xC0800000 -0x200);   // input
    //char *shared_memory = (char *) (0xC0800000 -0x100); // output
    
    
    
    // check flag.
    char FlagStatus = -1;
    
    FlagStatus = (char) shared_flag[0];
    
    if (FlagStatus != 0){
        shared_flag[0] = 0; //cleaning the flag.
        //todo: clear the command line.
        return (char *) shared_memory;
    }

fail:
    if (FlagStatus == 0){
        gramado_system_call(265,0,0,0); //yield 
        return (char *) 0;
    }
}


 
int main ( int argc, char *argv[] )
{

    //shared memory
   
    char *buf; 
    char cmd[32];
    int i=0; 


     printf("shell.bin: Hello!\n");
    
    while (1){
 
        buf = __get_commandline();
        
        //Temos uma linha de comando.
        if ((void *)buf != NULL)
        {
            
            printf("CommandLine={%s}\n",buf);
            for(i=0; i<20; i++)
            {   cmd[i] = (char) *buf; buf++; }
             
            printf("Char0={%c} ", cmd[0]);
            printf("Char1={%c} ", cmd[1]);
            printf("Char2={%c} ", cmd[2]);
            printf("Char3={%c} ", cmd[3]);
            fflush(stdout);
             
            //if ( strncmp ( (const char *) cmd, "exit", 4 ) == 0 )
            //{ printf ("OK\n"); exit(0); } 

            if ( memcmp ( (const char *) cmd, "exit", 4 ) == 0 )
            { printf ("OK\n"); exit(0); } 
            
            
            
            // ...
            
            printf ("Unknown command\n");
            printf ("$ "); fflush(stdout);
        }
    
        //try again
        //até que aflag mude.
    }

    return 0;
}




//
// End.
//


