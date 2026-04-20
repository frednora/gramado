//====================================================
// shell2.bin - Minimal STDIN/STDOUT Shell for Gramado OS
//====================================================

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rtl/gramado.h>

//#include "shell.h"

static int __no_pipes = TRUE;

static int __fd_input = -1;
static int __fd_stdout = -1;
static int __fd_stderr = -1;



static void process_run_command(const char *cmdline, int use_pipes);

// ====================================================

//====================================================
// Globals
//====================================================

//static char prompt[256];
//static int  prompt_pos = 0;

//====================================================
// Reset prompt buffer
//====================================================

static void reset_prompt(void)
{
    memset(prompt, 0, sizeof(prompt));
    prompt_pos = 0;
}

//====================================================
// Send prompt to terminal (stdout)
//====================================================

static void show_prompt(void)
{
    write(1, "$ ", 2);
}


//====================================================
// Local worker: run a command line, feed stdin, launch child
//====================================================

static void process_run_command(const char *cmdline, int use_pipes)
{
    if (cmdline == NULL || *cmdline == '\0')
        return;

    // Make a local copy since strtok modifies the string
    char local_cmd[128];
    memset(local_cmd, 0, sizeof(local_cmd));
    strncpy(local_cmd, cmdline, sizeof(local_cmd)-1);

    // Extract program name
    char filename[64];
    memset(filename, 0, sizeof(filename));
    char *token = strtok(local_cmd, " \t");
    if (token != NULL) {
        strncpy(filename, token, sizeof(filename)-1);
    }

    // Feed stdin with the full command line
    //write(STDIN_FILENO, cmdline, strlen(cmdline));
    //write(STDIN_FILENO, "\n", 1);

/*
    // #backup: Old way. (it's working)
    // Inject cmdline: The kernel passes it to the child.
    sc82(44010, cmdline, cmdline, cmdline);
    // Launch child with just the filename
    int tid = rtl_clone_and_execute_return_tid(filename);
*/

    // #test
    // It passes the cmdline, clone it and return the child's tid.
    int tid = 
        rtl_clone_and_execute_return_tid_ex (
            filename, 
            cmdline 
        );

    // Report result
    if (tid < 0) {
        write(STDOUT_FILENO, "shell2: failed to launch\n", 24);
    } else {

        // #debug
        char msg[64];
        sprintf(msg, "shell2: launched tid=%d\n", tid);
        write(STDOUT_FILENO, msg, strlen(msg));

        rtl_sleep(2000);  //2sec
        // Tell the kernel the child is now foreground 
        sc82 (10011,tid,tid,tid);

        // Exit the shell right after a positive launch 
        exit(0);
    }
}

//====================================================
// Process a completed command
//====================================================

static void process_command(void)
{
    char *argv[16];   // up to 16 args
    int argc = 0;

    // Tokenize input line
    char *token = strtok(prompt, " \t");
    while (token != NULL && argc < 16) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL; // terminate list

    if (argc == 0) {
        // empty line
        //reset_prompt();
        //show_prompt();
        return;
    }

    // Built-in commands
    if (strcmp(argv[0], "about") == 0) {
        write(1, "shell2: minimal stdin/stdout shell\n", 34);
    }
    else if (strcmp(argv[0], "help") == 0) {
        write(1, "shell2: commands: about, help, run\n", 34);

    }
    else if (strcmp(argv[0], "run") == 0 && argc > 1) {
        // Build the command line string from argv[1..]
        char cmdline[128];
        memset(cmdline, 0, sizeof(cmdline));

        // Copy the program name
        strncpy(cmdline, argv[1], sizeof(cmdline)-1);

        // Append extra args if any
        int it=0;
        for (it = 2; it < argc; it++) {
            strncat(cmdline, " ", sizeof(cmdline)-strlen(cmdline)-1);
            strncat(cmdline, argv[it], sizeof(cmdline)-strlen(cmdline)-1);
        }

        // Call the worker
        process_run_command(cmdline, FALSE);
    }
    else if (strcmp(argv[0], "run2") == 0 && argc > 1) {
        char __filename_local_buffer[64];
        memset(__filename_local_buffer, 0, sizeof(__filename_local_buffer));

        // Copy argument safely
        strncpy(__filename_local_buffer, argv[1], sizeof(__filename_local_buffer)-1);

        // Trim trailing newline or spaces
        size_t len = strlen(__filename_local_buffer);
        while (len > 0 && 
              (__filename_local_buffer[len-1] == '\n' || 
               __filename_local_buffer[len-1] == '\r' || 
               __filename_local_buffer[len-1] == ' ')) 
        {
            __filename_local_buffer[--len] = '\0';
        }

        // Now call Gramado's launcher
        rtl_clone_and_execute(__filename_local_buffer);
    }
    else {
        write(1, "shell2: unknown command\n", 24);
    }

    //reset_prompt();
    //show_prompt();
}

//====================================================
// Worker loop: read from stdin, echo, accumulate
//====================================================

static void shell_worker(void)
{
    char buf[1];
    int C = 0;

    show_prompt();

    while (1)
    {
        if (read(0, buf, 1) > 0)
        {
            C = (int) buf[0];

            // Printable ASCII
            if (C >= 0x20 && C <= 0x7E)
            {
                if (prompt_pos < sizeof(prompt)-1) {
                    prompt[prompt_pos++] = C;
                    prompt[prompt_pos] = 0;
                }

                write(1, &buf[0], 1);  // echo
            }

            // BACKSPACE
            else if (C == 0x7F || C == 0x08)
            {
                if (prompt_pos > 0) {
                    prompt_pos--;
                    prompt[prompt_pos] = 0;
                    write(1, "\b \b", 3);
                }
            }

            // ENTER
            else if (C == '\n' || C == '\r')
            {
                write(1, "\n", 1);
                process_command();
                reset_prompt();
                show_prompt();
            }
        }
    }
}

//====================================================
// main()
//====================================================

int main(int argc, char *argv[])
{
    __no_pipes = TRUE;  // No pipes needed for now

    __fd_input  = STDIN_FILENO;
    __fd_stdout = STDOUT_FILENO;
    __fd_stderr = STDERR_FILENO;

    printf ("shell2.bin: Hello world!\n");
    //printf ("table size: %d\n", getdtablesize());
    reset_prompt();
    shell_worker();
    return 0;
}
