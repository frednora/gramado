//====================================================
// shell.bin - Minimal PTY Shell for Gramado OS
//====================================================

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctls.h>
#include <sys/ioctl.h>
#include <rtl/gramado.h>

#include "shell.h"

//====================================================
// Globals
//====================================================

int ptys_fd = -1;

// Belongs to the library
//char prompt[256];
//int prompt_pos = 0;

//====================================================
// Initialize PTY slave
//====================================================

static void shell_initialize_pty(void)
{
    ptys_fd = open("/DEV/PTYS", 0, "a+");
    if (ptys_fd < 0) {
        printf("shell: failed to open PTYS\n");
        exit(1);
    }
    // #todo: redirect stdin, stdout, stderr
    //dup2(ptys_fd, 0);
    //dup2(ptys_fd, 1);
    //dup2(ptys_fd, 2);
    //dup2(ptys_fd, STDIN_FILENO);
    //dup2(ptys_fd, STDOUT_FILENO);
    //dup2(ptys_fd, STDERR_FILENO);

    //if (ptys_fd > STDERR_FILENO)
        //close(ptys_fd);

}

//====================================================
// Reset prompt buffer
//====================================================

static void reset_prompt(void)
{
    memset(prompt, 0, sizeof(prompt));
    prompt_pos = 0;
}

//====================================================
// Send prompt to terminal
//====================================================

static void show_prompt(void)
{
    write(ptys_fd, "$ ", 2);
}

//====================================================
// Process a completed command
//====================================================

static void process_command(void)
{
    // Compare commands
    if (strncmp(prompt, "about", 5) == 0) {
        write(ptys_fd, "shell: minimal PTY shell\n", 26);
    }
    else if (strncmp(prompt, "help", 4) == 0) {
        write(ptys_fd, "shell: commands: about, help\n", 30);
    }
    else if (prompt_pos == 0) {
        // Empty line → do nothing
    }
    else {
        write(ptys_fd, "shell: unknown command\n", 24);
    }

    reset_prompt();
    show_prompt();
}

//====================================================
// Worker loop: read from PTYS, echo, accumulate
//====================================================

static void shell_worker(void)
{
    char buf[1];
    int C = 0;

    show_prompt();

    while (1)
    {
        if (read(ptys_fd, buf, 1) > 0)
        {
            C = (int) buf[0];

            // Printable ASCII
            if (C >= 0x20 && C <= 0x7E)
            {
                // Store in prompt[]
                if (prompt_pos < sizeof(prompt)-1) {
                    prompt[prompt_pos++] = C;
                    prompt[prompt_pos] = 0;
                }

                // Echo back
                write(ptys_fd, &buf[0], 1);
            }

            // BACKSPACE
            else if (C == 0x7F || C == 0x08)
            {
                if (prompt_pos > 0) {
                    prompt_pos--;
                    prompt[prompt_pos] = 0;
                    write(ptys_fd, "\b \b", 3);
                }
            }

            // ENTER
            //else if (C == VK_RETURN)
            else if (C == '\n')
            {
                write(ptys_fd, "\n", 1);
                process_command();
            }
        }
    }
}

//====================================================
// main()
//====================================================
/*
int main(int argc, char *argv[])
{
    //shell_initialize_pty();
    while(1)
    {
        int c = getc(stdin);
        if (c>0)
        {
           printf("%c",c);
           fflush(stdout);

            //if (c == 'x')
                //printf("sh: x was received\n");

            if (c == '\n')
                printf("sh: [enter] received\n");
        }
        //printf("shell.bin: Hello using stdout\n");
    }
    //reset_prompt();
    //shell_worker();
    return 0;
}
*/

int main(int argc, char *argv[])
{
// The shell must ignore argv[] and 
// switch to reading commands from stdin,
// where the cmdline or script is.

    int c=0;

    //write(1, "[MAIN START]\n", 13);

    //printf ("---- Shell's banner ----\n");

    printf("|argc = %d\n", argc);

    int i=0;
    for (i = 0; i < argc; i++) {
        printf("argv[%d] = '%s'\n", i, argv[i]);
    }

    while (1){
        c = getc(stdin);
        if (c >= 0)
        {
            // Process cmdline
            if (c == '\n'){
                printf("sh: [enter] received\n");
            // ECHO: Send char back
            } else {
                putc(c,stdout);
                fflush(stdout);
            };
        }
    }

    return EXIT_SUCCESS;
}

