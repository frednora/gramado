// shell.bin
// Shell application for Gramado OS.
// This is gonna run on terminal.bin, sending data 
// to it via tty. (stderr for now).

// rtl
#include <types.h>
//#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rtl/gramado.h>

// Internal routines.
#include "shell.h"

// ================
// Testing (VGA embedded ring3 driver).
// libio
//#include <libio.h>
#include "vga_test.h"
//================

int isTimeToQuit=FALSE;
unsigned long device_width=0;
unsigned long device_height=0;

//======================================

static void shellPrompt(void);

static unsigned long shellCompare(void);

static unsigned long shellProcessCmdline(void);
static unsigned long shellProcessPrintableChar(int c);

static void doExit(void);
static void doLF(void);

static void __test_escapesequence(void);
//======================================

static void __test_escapesequence(void)
{

    if ( (void*) stdout == NULL )
        return;

    //printf("\n");
    //printf("Testing escape sequence:\n");

    //printf("One: \033[m");          //uma sequencia.
    //printf("Two: \033[m \033[m");   //duas sequencias.
    printf("~Before \033[m ~Inside \033[m ~ After"); 
    //printf("\033[1Am"); //ok
    //printf("\033[2Am X");  // cursor move up.
    printf("\033[4Bm After cursor down 4 times");  // cursor move down.
    
    //printf("\033[sm");     // save cursor position
    //printf("\033[um");     // restore cursor position

    //#test: Line feed
    //printf("\033A");

    //#test: Carriage Return 
    //printf("\033D");
    
    
    
    // apagar N bytes na mesma linha.
    //printf("Before --|\033[0Km|-- After \n");  
    
    printf("\033D");
    printf("Before --|\033[1Km|-- After 1 space");  
    
    printf("\033D");
    printf("Before --|\033[2Km|-- After 2 spaces");  
    
    printf("\033D");
    printf("Before --|\033[3Km|-- After 3 spaces");  
    
    //printf("\033D");
    //printf("Before --|\033[4Km|-- After 4 spaces");  

    printf("\033D");
    //move cursor right
    printf("\033[8Cm After cursor right 8 timer.");

    printf("\033[2Bm After cursor down 2 times");

    //printf("\033D");
    //move cursor left
    printf("\033[2Bm \033[4Dm After cursor left 4 timer.");

    printf("\033D");
    printf("Done :) ");
    
    //fflush(stdout);
}

static void doExit(void)
{
    printf("doExit(): ");
    fflush(stdout);
    exit(0);
    //isTimeToQuit = TRUE;
}

static void doLF(void)
{
    //printf(".\n");
    printf("\033D");
}

/*
 * shellPrompt:
 * Inicializa o prompt.
 * Na inicialização de stdio, 
 * prompt foi definido como stdin->_base.
 */

static void shellPrompt(void)
{
    register int i=0;

// Clean prompt buffer
    for ( i=0; i<PROMPT_MAX_DEFAULT; i++ ){
        prompt[i] = (char) '\0';
    };

// First char
    prompt[0] = (char) '\0';

// Variables
    prompt_pos = 0;
    prompt_status = 0;
    prompt_max = PROMPT_MAX_DEFAULT;

// Draw prompt.

    //printf("\n");
    //putc('$',stdout);
    //putc(' ',stdout);
    //printf("\n");
    //printf("$\n");


// Line feed (unix)
    printf("\033D $ ");

    //printf("$ ");
    fflush(stdout);
}

// local
static unsigned long shellCompare(void)
{
    unsigned long ret_value=0;
    char *p;

// The first char.
// $(NULL)
// [ENTER] finalizou a atring antes de ter digitado alguma coisa.
// Se alguem pressiona [ENTER] com prompt vazio d� page fault ?
// Isso cancela caso o buffer esteja vazio.

// Local pointer,
    p = prompt;

    //if ((void*) p == NULL)
    //{
    //    printf("p \n");
    //    exit(0);
    //}

// Null string.
    if ( *p == '\0' )
    {
        //shellInsertLF();
        goto exit_cmp;
    }

//
// Compare
//

// Compare strings.
// Let's start at the next line.

do_compare:

    //shellInsertLF();
    //printf("\n");

// Let's send 0x00~0x1F to the terminal.
// Or scape sequencies.
    if ( strncmp ( prompt, "esc", 3 ) == 0 )
    {
        doLF();
        //printf("shell.bin: This is the shell command\n");
        //printf("\n");
        //printf("\033D");
        printf("Sending escape sequencies: \n");
        __test_escapesequence();
        goto exit_cmp;
    }

    // cls
    // #todo: Send escape sequence.
    if ( strncmp(prompt,"cls",3) == 0 )
    {
        doLF();
        //printf("\033D");
        printf ("~cls");
        //fflush(stdout);
        goto exit_cmp;
    }
 
    // about
    // #todo: Create do_banner();
    if ( strncmp ( prompt, "about", 5 ) == 0 )
    {
        doLF();
        printf("shell.bin: This is the shell command");
        //fflush(stdout);
        goto exit_cmp;
    }

    // getpid
    int my_pid=0;
    if ( strncmp( prompt, "getpid", 6 ) == 0 )
    {
        my_pid = getpid();
        doLF();
        printf("pid: %d",my_pid);
        goto exit_cmp;
    }

    // getppid
    int my_ppid=0;
    if ( strncmp( prompt, "getppid", 7 ) == 0 )
    {
        my_ppid = getppid();
        doLF();
        printf("ppid: %d",my_ppid);
        goto exit_cmp;
    }

// mm-size (MB)
    unsigned long __mm_size_mb = 0;    
    if ( strncmp( prompt, "mm-size", 7 ) == 0 )
    {
        __mm_size_mb = (unsigned long) sc80 (292,0,0,0);
        doLF();
        printf("Memory size = %d MB ",__mm_size_mb);
        goto exit_cmp;
    }

// current-process
    if ( strncmp ( prompt, "current-process", 15 ) == 0 )
    {
        //printf("\n");
        //sc80 ( SYSTEMCALL_CURRENTPROCESSINFO, 0, 0, 0 );
        sc80( 80, 0, 0, 0 );
        goto exit_cmp; 
    }

// process-info
    if ( strncmp ( prompt, "process-info", 12 ) == 0 )
    {
        //printf("\n");
        sc80( 82, 0, 0, 0 );
        goto exit_cmp; 
    }

// exit
    if ( strncmp( prompt, "exit", 4 ) == 0 )
    {
        doLF();
        doExit();
        goto exit_cmp;
    }

// quit
    if ( strncmp( prompt, "quit", 4 ) == 0 )
    {
        doLF();
        doExit();
        goto exit_cmp;
    }

// malloc
    void *hBuffer;
    if ( strncmp( prompt, "malloc", 6 ) == 0 )
    {
        doLF();
        printf ("Testing heap: 32KB\n");
        hBuffer = (void *) malloc( 1024*32 );        // 32 kb
        //...
        if ( (void *) hBuffer == NULL ){
            printf("Fail\n");
        }else{
            printf("OK\n");
        };
        printf("done");
        goto exit_cmp;
    }

// sync 
// Salva os buffers em ring0 no disco fisico.
// Isso pode ser um programa.
// See: unistd.c
    if ( strncmp( prompt, "sync", 4 ) == 0 )
    {
        //printf ("sync: \n");
        sync();
        goto exit_cmp;
    }

// tty3
    if ( strncmp ( prompt, "tty3", 4 ) == 0 )
    {
        //if ( isatty(fileno(stdin)) == 0 ){
        //    printf ("stdin is not a tty\n");
        //}
        //if ( isatty(fileno(stdout)) == 0 ){
        //    printf ("stdout is not a tty\n");
        //}
        //if ( isatty(fileno(stderr)) == 0 ){
        //    printf ("stderr is not a tty\n");
        //}
        goto exit_cmp;
    }

launch_app:

    // #todo: Para testes podemos aceitar os
    // comandos com extensao .bin
    //rtl_clone_and_execute(prompt);

    doLF();
    //printf ("Command not found\n");
    //printf("\n");
    printf("SHELL.BIN: Command not found! ");
    //fflush(stdout);

// #testando escape sequences.
    //printf("Escape \x1b[4B \n");
    //fflush(stdout);
    // ...

exit_cmp:
    ret_value = 0;
done:
    shellPrompt();
    return (unsigned long) ret_value;
}

// Wrapper
static unsigned long shellProcessCmdline(void)
{
    return (unsigned long) shellCompare();
}

static unsigned long shellProcessPrintableChar(int c)
{
    if (c<0)
        return 0;

    if (c >= 0x20 && c <= 0x7F)
    {
        // Feed the command line in prompt[], I guess.
        input(c);

        // Sending data to the terminal.
        printf("%c",c);
        fflush(stdout);
    }

    return 0;
}

//
// Main
//

int main(int argc, char *argv[])
{
    register int i=0;
    int C=0;
    isTimeToQuit = FALSE;

// -----------------------------------
// (>>> stdout)
// Still using the kernel console.

// -----------------------------------
// (>>> stderr)
// Now we have a new stdout.
// Now we're gonna send the data to the terminal.bin
// that is reading stderr.

    stdout = stderr;


    int connector0_fd = 0;
    int connector1_fd = 0;

    int UseConnectors=TRUE;
    if (UseConnectors == TRUE){
    connector0_fd = (int) sc82(902,0,0,0);
    //connector1_fd = (int) sc82(902,1,0,0);
    //#debug
    //printf("terminal.bin: connector0_fd %d | connector1_fd %d \n",
    //    connector0_fd, connector1_fd);
    //while(1){}
    // The shell is writing on connector 0,
    // the same connector the terminal is reading from.
     stdout->_file = (int) connector0_fd;
    }

    doLF();
    printf("shell.bin: argc={%d} \n",argc);
    if (argc>0){
        for (i=0; i<argc; i++){
            printf("argv[%d]: %s\n", i, argv[i] );
        };
    }

    shellPrompt();

// ------------------------------------------------
// Loop: (Input events).
// Reading from stdin and sending to our new stdout.
// stderr.

    while (1){

        if (isTimeToQuit == TRUE)
            break;

        // #bugbug
        // We got a PF when we type a lot of keys.
        // And sometimes when we type Enter.

        C = fgetc(stdin);
        // Como estamos usando um arquivo regular,
        // entao o kernel concatena ate chegar no fim do arquivo.
        if (C == EOF){
            //rewind(stdin);
            //exit(0);
        }

        // Valid char.
        if (C>0)
        {
            // Enter: So, process the command line
            if (C == VK_RETURN){
                shellProcessCmdline();
            // printable chars: So print a regular key.
            } else if (C >= 0x20 && C <= 0x7F){
                shellProcessPrintableChar(C);
            // #todo:
            // Control keys: (0~0x1F)
            // See the example in init.bin.
            };
        }
    };

    if (isTimeToQuit != TRUE)
    {
        printf("Quit fail ");
        fflush(stdout);
        while (1){
        } ;
    }

done:

    printf("Quit ok ");
    fflush(stdout);

    return EXIT_SUCCESS;

fail:
    return EXIT_FAILURE;
}




