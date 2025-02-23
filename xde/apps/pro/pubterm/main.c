// main.c
// Main file for PUBTERM.BIN
// 2021 - Created by Fred Nora.

// #test
// For now it has a small embedded command line interpreter.
// The POSIX terminal interface.
// Raw or Canonical?
// See:
// https://www.gnu.org/software/libc/manual/html_node/Canonical-or-Not.html
// https://en.wikipedia.org/wiki/POSIX_terminal_interface
// https://en.wikipedia.org/wiki/Computer_terminal
// ...
// Connecting via AF_INET.
// tutorial example taken from. 
// https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
/*
    To make a process a TCP server, 
    you need to follow the steps given below −
    Create a socket with the socket() system call.
    Bind the socket to an address using the bind() system call. 
    For a server socket on the Internet, an address consists of a 
    port number on the host machine.
    Listen for connections with the listen() system call.
    Accept a connection with the accept() system call. 
    This call typically blocks until a client connects with the server.
    Send and receive data using the read() and write() system calls.
*/ 
// See:
// https://wiki.osdev.org/Message_Passing_Tutorial
// https://wiki.osdev.org/Synchronization_Primitives
// https://devblogs.microsoft.com/commandline/
// /windows-command-line-introducing-the-windows-pseudo-console-conpty/
// ...

#include <types.h>
//#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//#include "include/term0.h"
#include "include/terminal.h"

// see: terminal.c and term0.h.
extern struct terminal_d  Terminal;

int main(int argc, char *argv[])
{ 
    int Status = -1;

    debug_print ("terminal.bin:\n");

//
// #todo: Parse parameters.
//

    if (argc<1){
    }

// flags.
// Comparando os argumentos para acionar as flags.

/*
    for (i=0; i<argc; i++)
    {
        // #todo
        // Create useful flags for this application.

        if ( strncmp( argv[i], "-a", 2) == 0 ){
        }
        if ( strncmp( argv[i], "-b", 2) == 0 ){
        }
        if ( strncmp( argv[i], "-s", 2) == 0 ){
            asm_flag = 1;
        }
        if ( strncmp( argv[i], "--stats", 7) == 0 ){
            fShowStats = TRUE;
        }
        if ( strncmp( argv[i], "--dumpo", 7) == 0 ){
            fDumpOutput = TRUE;
        }
        //...
    };
*/


// Initializing the structure.
    Terminal.initialized = FALSE;
    Terminal.pid = (pid_t) getpid();
    Terminal.uid = (uid_t) getuid();
    Terminal.gid = (gid_t) getgid();

    //setreuid(-1, -1);
    //setpgrp(0, getpid());

    Terminal.esc = 0;
 
    Terminal.client_fd = -1;

    Terminal.main_window_id = -1;
    Terminal.client_window_id = -1;

    Terminal.left = 0;
    Terminal.top = 0;
    Terminal.width = 50;
    Terminal.height = 50;

    Terminal.width_in_chars = 0;
    Terminal.height_in_chars = 0;

    // ...

// --------------------------------
// #test
// Telling to the kernel that we are a terminal.
// This way the kernel will create connectors when we clone ourself.
// Probably for linking with the child.
// This connector is gonna be our new stdin.
// See: __input_from_connector() in terminal.c
// #todo
// Explain and document this connectors.

    // #todo: Create an API for this.
    // IN: syscall number,  ... signature.
    sc82(901,1234,1234,1234);

// --------------------------------
// IN: flags
    Status = (int) terminal_init(0);
    if (Status != EXIT_SUCCESS){
        printf("main: Something is wrong\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

