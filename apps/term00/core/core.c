// core.c
// Backend part for the terminal application.

#include <types.h>
//#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Shared
#include "../shared/globals.h"

// Backend
#include "core.h"
#include "shell.h"

// Frontend
#include "../ui/ui.h"

#include "../terminal.h"

// Client-side library.
#include <gws.h>


struct terminal_core_d  TerminalCore;

int isWaitingForOutput=FALSE;


static void __send_to_child (void);

// ======================================


// f4
// Send the content of prompt[] to stdin.
// The child process will read this.
// This is why the stdin needs to be a tty.
// (Canonical) 
// If the child is a shell it will read a line.
// (Raw) 
// If the child is a text editor it will read a single char.
// Maybe the shell can change stdin for it's child.
// For now, the shell will need a line.

static void __send_to_child (void)
{
    register int i=0;
    char *shared_flag   = (char *) (0xC0800000 -0x210);   // flag
    char *shared_memory = (char *) (0xC0800000 -0x200);   // input
    //char *shared_memory = (char *) (0xC0800000 -0x100); // output
   
   
    // There is a '\n' terminated line in prompt[].
    // #bugbug: Não podemos mandar uma linha sem '\n'.
    fseek(stderr, 0, SEEK_SET); 
    write ( fileno(stderr), prompt, 80);
    
    //copy to shared memory
    //send a system message.(maybe)
    //flag?

    // Send the command line to the shared memory.
    for(i=0; i<80; i++){ shared_memory[i] = prompt[i]; }
    // Clear prompt.
    for(i=0; i<80; i++){ prompt[i]=0; }
    prompt_pos = 0; 
    
    // Notify the child that it has a message in the shared memory.
    shared_flag[0] = 1; 
}



// Isso chama o aplicativo true.bin
// que se conecta a esse via tty e nos envia uma mensagem.

// Testing tty support.
void test_tty_support(int fd)
{
    gws_debug_print("test_tty_support: [FIXME] undefined reference\n");
    return;
    
    /*
    char buffer[32];
    int nread = 0;



    gws_debug_print("test_tty_support:\n");

    int ____this_tty_id = (int) sc80( 266, getpid(), 0, 0 );


   // lançando um processo filho.  
   sc80 ( 900, 
       (unsigned long) "true.bin", 0, 0 );

    int i=0;
    while(1){

        nread = read_ttyList ( ____this_tty_id, buffer, 32 ); 
        
        if( nread>0){
            
            for(i=0;i<32;i++){
                if( buffer[i] != 0)
                    terminal_write_char(fd, buffer[i]);
            }
            return;
        }

        //i++;
        //if(i>20) i=0;
    }
    */
   
    /*
    int nwrite = -1; // bytes escritos.
    size_t __w_size2=0;
    while(1)
    {
        // Escrevendo na tty desse processo e na tty slave pra leitura.
        nwrite = write_ttyList ( ____this_tty_id, 
                     buffer, 
                     __w_size2 = sprintf (buffer,"THIS IS A MAGIC STRING\n")  );
    
        if (nwrite > 0)
           return 0;//goto __ok;
    }
    */
}

// Launch a child process given a filename.
// Return TID or -1.
int terminal_core_launch_child(const char *filename)
{
    int tid = -1;

    if ((void*) filename == NULL)
        goto fail;
    if (*filename == 0)
        goto fail;

// Kernel service
    tid = (int) rtl_clone_and_execute_return_tid(filename);
    if (tid < 0)
        goto fail;

// Kernel service
// Delegate foreground to the child.
    sc82(10013, tid, tid, tid);

    // Backend state
    isWaitingForOutput = TRUE;

    return (int) tid;
fail:
    return (int) -1;
}

// Try to execute the command line in the prompt[].
// This is called by compareStrings()
int terminal_core_launch_from_cmdline(int fd, const char *cmdline)
{
    int ChildTID = -1;
    register int i=0;
    register int ii=0;
    char *p;  // cmdline alias
    char filename_buffer[12];  //8+3+1

// Limits:
// + The buff (prompt[]) limit is BUFSIZ = 1024;
// + The limit for the write() operation is 512 for now.
    size_t WriteLimit = 512;

// Parameters:
    if (fd<0){
        goto fail;
    }
    if ((void*) cmdline == NULL){
        goto fail;
    }
    if (*cmdline == 0){
        goto fail;
    }

// Clone
// #important:
// For now the system will crash if the
// command is not found.
// #bugbug
// We are using the whole 'command line' as an argument.
// We need to work on that routine of passing
// the arguments to the child process.
// See: rtl.c
// Stop using the embedded shell.

// rebubina o arquivo de input.
    //rewind(__terminal_input_fp);
    
// ==================================

//
// Send commandline via stdin.
//

// Write it into stdin.
// It's working
// See: crt0.c

    //rewind(stdin);
    //prompt[511]=0;
    //write(fileno(stdin), prompt, 512);

    //fail
    //fprintf(stdin,"One Two Three ...");
    //fflush(stdin);

/*
// it's working
    char *shared_buffer = (char *) 0x30E00000;  //extra heap 3.
    sprintf(shared_buffer,"One Two Three ...");
    shared_buffer[511] = 0;
*/

// ==================================

//
// Get filename
//

// #bugbug
// The command line accepts only one word
// and the command line has too many words.

//#todo
//Create a method.
//int rtl_get_first_word_in_a_string(char *buffer_pointer, char *string);

// ---------------
// Grab the filename in the first word of the cmdline.
    memset(filename_buffer,0,12);

    p = cmdline;

    while (1)
    {
        // Se tem tamanho o suficiente ou sobra.
        if (ii >= 12){
            filename_buffer[ii] = 0;  //finalize
            break;
        }

        // Se o tamanho esta no limite.
        
        // 0, space or tab.
        // Nao pode haver espace no nome do programa.
        // Depois do nome vem os parametros.
        if ( *p == 0 || 
             *p == ' ' ||
             *p == '\t' )
        {
            // Finalize the buffer that contain the image name.
            filename_buffer[ii] = 0;
            break;
        }

        // Printable.
        // Put the char into the buffer.
        // What are these chars? It includes symbols? Or just letters?
        if ( *p >= 0x20 && *p <= 0x7F )
        {
            filename_buffer[ii] = (char) *p;
        }

        p++;    // next char in the command line.
        ii++;   // next byte into the filename buffer.
    };

//
// Parse the filename inside its local buffer.
//

// Is it a valid extension?
// Pois podemos executar sem extensão.
    int isValidExt = FALSE;
    int dotWasFound = FALSE;

// Look up for the first occorence of '.'.
// 12345678.123 = (8+1+3) = 12
    for (i=0; i<=12; i++)
    {
        // The command name can't have these chars.
        // It means that we reached the end of the command name.
        // Maybe we have parameters after the name, maybe not.
        if ( filename_buffer[i] == 0 || 
             filename_buffer[i] == ' ' ||
             filename_buffer[i] == '\t' )
        {
            break;
        }        

        if (filename_buffer[i] == '.'){
            dotWasFound = TRUE;
            break;
        }
    };

// ----------------
// '.' was NOT found, 
// but the filename is bigger than 8 bytes.
    if (dotWasFound != TRUE)
    {
        if (i > 8){
            printf("terminal: Long command name\n");
            goto fail;
        }
    }

// ----------------
// '.' was found.
// Se temos um ponto e 
// o que segue o ponto não é 'bin' ou 'BIN',
// entao a estencao e' invalida.

    if (dotWasFound == TRUE)
    {
        if ( filename_buffer[i] != '.' )
            goto fail;

        // Ainda nao temos uma extensao valida.
        // Encontramos um ponto,
        // mas ainda não sabemos se a extensão é valida
        // ou não.
        // isValidExt = TRUE;
        
        // Valida a extensao se os proximos chars forem "bin".
        if ( filename_buffer[i+1] == 'b' &&
             filename_buffer[i+2] == 'i' &&
             filename_buffer[i+3] == 'n'  )
        {
            isValidExt = TRUE;
        }
        // Valida a extensao se os proximos chars forem "BIN".
        if ( filename_buffer[i+1] == 'B' &&
             filename_buffer[i+2] == 'I' &&
             filename_buffer[i+3] == 'N'  )
        {
            isValidExt = TRUE;
        }
        // ...
    }

// No extension
// The dot was found, but the extension is invalid.
// Invalid extension.
    if (dotWasFound == TRUE)
    {
        if (isValidExt == FALSE){
            printf("terminal: Invalid extension in command name\n");
            goto fail;
        }
    }

//----------------------------------

//
// Clone and execute.
//

//#todo
// Tem que limpar o buffer do arquivo em ring0, 
// antes de escrever no arquivo.

// cmdline:
// Only if the name is a valid name.
    rewind(stdin);
    //off_t v=-1;
    //v=lseek( fileno(stdin), 0, SEEK_SET );
    //if (v!=0){
    //    printf("testing lseek: %d\n",v);
    //    asm("int $3");
    //}

// Finalize the command line.
// Nao pode ser maior que o buffer.
    //if (WriteLimit > PROMPT_MAX_DEFAULT){
    //    WriteLimit = PROMPT_MAX_DEFAULT;
    //}
    //int __LastChar = (int) (WriteLimit-1);
    //prompt[__LastChar]=0;

    // #debug
    // OK!
    //printf("promt: {%s}\n",prompt);
    //asm ("int $3");

// #bugbug: 
// A cmdline ja estava dentro do arquivo
// antes de escrevermos. Isso porque pegamos mensagens de
// teclado de dentro do sdtin.
// Tambem significa que rewind() não funcionou.
// #test
// Nao pode ser maior que o limite atual para operaçoes de escrita.
    //if (WriteLimit > 512){
    //    WriteLimit = 512;
    //}
    //write(fileno(stdin), prompt, WriteLimit);
    //write(fileno(stdin), prompt, 80);
// #test: The command program's crt0 will read from stderr
// because ti doesn't have the focus and cant read from stdin.
    write(fileno(stdin), cmdline, 80);

// Execute given the filename and the cmdline goes in stdin.

    ChildTID = (int) terminal_core_launch_child(filename_buffer);
    if (ChildTID < 0){
        printf("core.c: ChildTID\n");
        goto fail;
    }

    //Terminal.child_tid = tid;

// #important:
// Child TID handling during terminal spawn.
//
// 1. We receive the TID of the child process/thread.
// 2. Save this TID into the window structure inside the display server.
//    -> Window->delegate_tid = child_tid
// 3. Update the delegation flag:
//    -> Window->client_delegates_foreground = TRUE if terminal wants child
//       to be foreground, FALSE if terminal keeps foreground itself.
// 4. Meaning:
//    - Window->client_tid remains immutable (the terminal itself).
//    - Window->delegate_tid is the nominated child.
//    - Window->client_delegates_foreground tells the server whether to switch
//      foreground focus from the terminal to the delegate.
//
// This ensures the server knows when to route input/output
// to the child instead of the terminal.
// Reminder: 
// Window->client_tid is immutable, 
// Window->delegate_tid is dynamic, flag drives focus.

    //#debug
    //printf("prompt: {%s}\n",cmdline);
    //printf("filename_buffer: {%s}\n",filename_buffer);

    // #todo #test
    // This is a method for the whole routine above.
    // rtl_execute_cmdline(prompt);

// clone and execute via ws.
// four arguments and a string pointer.

/*
    int res = -1;
    res = 
        (int) gws_clone_and_execute2(
                  fd,
                  0,0,0,0,
                  filename_buffer );

   if (res<0){
       //#debug #todo: do not use printf.
       //printf("gws_clone_and_execute2: fail\n");
   }
*/

// #bugbug
// breakpoint
// something is wrong when we return here.
    
    //printf("terminal: breakpoint\n");
    //while(1){}

// #bugbug: 
// Se não estamos usando então
// o terminal vai sair do loop de input e fechar o programa.
    
    //isUsingEmbeddedShell = FALSE;
    //return;

    //printf("Command not found\n");

    rtl_sleep(2000);

done:
    return (int) ChildTID;

fail:
    return (int) -1;
}

// Shutdown machine via display server.
void terminal_poweroff_machine(int fd)
{

// Parameter:
    if (fd<0){
        return;
    }

    //cr();
    //lf();
    //tputstring(fd, "Poweroff machine via ds\n");

    //gws_destroy_window(fd,terminal_window);
    //gws_destroy_window(fd,main_window);
    gws_shutdown(fd);
}

// Send async hello. msgcode=44888.
// #todo: Explain it better.
void __test_post_async_hello(void)
{
    unsigned long message_buffer[32];
// The tid of init.bin is '0', i guess. :)
    int InitProcessControlTID = 0;
// Response support.
    //int __src_tid = -1;
    //int __dst_tid = -1;

// The hello message
    message_buffer[0] = 0; //window
    message_buffer[1] = (unsigned long) 44888;  // message code
    message_buffer[2] = (unsigned long) 1234;   // Signature
    message_buffer[3] = (unsigned long) 5678;   // Signature
    message_buffer[4] = 0;  // Receiver
    message_buffer[5] = 0;  // Sender

// ---------------------------------
// Post
// Add the message into the queue. In tail.
// IN: tid, message buffer address
    rtl_post_system_message( 
        (int) InitProcessControlTID, 
        (unsigned long) message_buffer );

// #todo
// Actually this test needs to stay into a loop 
// waiting for the response.
// #bugbug
// We put the handler into the terminalProcedure,
// anyone can send this message to us for now.
    //while(1){ ... get event }
}




