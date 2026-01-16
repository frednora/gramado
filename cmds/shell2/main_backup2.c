//====================================================
// shell.bin - Minimal STDIN/STDOUT Shell for Gramado OS
//====================================================

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rtl/gramado.h>

//#include "shell.h"

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
        reset_prompt();
        show_prompt();
        return;
    }

    // Built-in commands
    if (strcmp(argv[0], "about") == 0) {
        write(1, "shell: minimal stdin/stdout shell\n", 34);
    }
    else if (strcmp(argv[0], "help") == 0) {
        write(1, "shell: commands: about, help, run\n", 34);
    }
    else if (strcmp(argv[0], "run") == 0 && argc > 1) {
    char __filename_local_buffer[64];
    memset(__filename_local_buffer, 0, sizeof(__filename_local_buffer));

    // Copy argument safely
    strncpy(__filename_local_buffer, argv[1], sizeof(__filename_local_buffer)-1);

    // Trim trailing newline or spaces
    size_t len = strlen(__filename_local_buffer);
    while (len > 0 && 
          (__filename_local_buffer[len-1] == '\n' || 
           __filename_local_buffer[len-1] == '\r' || 
           __filename_local_buffer[len-1] == ' ')) {
        __filename_local_buffer[--len] = '\0';
    }

    // Now call Gramado's launcher
    rtl_clone_and_execute(__filename_local_buffer);
    }
    else {
        write(1, "shell: unknown command\n", 24);
    }

    reset_prompt();
    show_prompt();
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
            }
        }
    }
}

//====================================================
// main()
//====================================================

int main(int argc, char *argv[])
{
    printf ("shell2.bin: Hello world!\n");
    reset_prompt();
    shell_worker();
    return 0;
}
