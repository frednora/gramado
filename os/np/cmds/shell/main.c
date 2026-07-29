//====================================================
// shell.bin - Minimal STDIN/STDOUT Shell for Gramado OS
//====================================================

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rtl/gramado.h>

//#include <unistd.h>
//#include <pwd.h>   // #test for pwd
#include <sys/utsname.h>

//#include "shell.h"

struct utsname  my_un;


static int __no_pipes = TRUE;

static int __fd_input = -1;
static int __fd_stdout = -1;
static int __fd_stderr = -1;


static void process_run_command(const char *cmdline, int use_pipes);
void __test_colors_printf(void);

// ====================================================

//====================================================
// Globals
//====================================================

//static char prompt[256];
//static int  prompt_pos = 0;


// Testing SGR (color) escape sequences via printf,
// meant to be called from the shell (goes through stdout,
// not through tputc()'s internal escape sequence parser).
// #important: Single-parameter SGR codes only (see __test_colors
// for why compound codes like "\033[1;31m" don't work yet).
void __test_colors_printf(void)
{
    printf("\n");
    printf("Testing SGR color sequences (printf):\n");

    // Foreground colors (30-37)
    //printf("\033[30mBlack fg\033[0m\n");
    printf("\033[31mRed fg\033[0m\n");
    printf("\033[32mGreen fg\033[0m\n");
    //printf("\033[33mYellow fg\033[0m\n");
    printf("\033[34mBlue fg\033[0m\n");
    //printf("\033[35mMagenta fg\033[0m\n");
    //printf("\033[36mCyan fg\033[0m\n");
    //printf("\033[37mWhite fg\033[0m\n");

    // Background colors (40-47), fg reset to white first
    //printf("\033[37m\033[40mWhite on black\033[0m\n");
    //printf("\033[30m\033[41mBlack on red\033[0m\n");
    //printf("\033[30m\033[42mBlack on green\033[0m\n");
    printf("\033[30m\033[46mBlack on cyan\033[0m\n");

    // Reset check: after \033[0m color must go back to white/black
    printf("\033[0mBack to default\n");

    // #test: TAB
    printf("\t|TAB\t|TAB\n");

 // Move cursor up one line
    printf("\033[A");
    printf("Moved Up\n");

    // Move cursor down one line
    printf("\033[B");
    printf("Moved Down\n");

    // Move cursor right 5 columns
    printf("\033[5C");
    printf("Moved Right\n");

    // Move cursor left 5 columns
    printf("\033[5D");
    printf("Moved Left\n");

    printf("colors done :)\n");
}

//====================================================
// Built-in: clear screen using escape sequence
//====================================================
static void shell_builtin_clear(void)
{
    // Clear entire screen + move cursor to top-left (0,0)
    write(__fd_stdout, "\033[2J\033[H", 8);
    
    // Optional: Also reset colors to default
    write(__fd_stdout, "\033[0m", 4);
}

void __test_reset(void)
{
    printf("\n\033[31mThis is RED text\033[0m <- should be normal now\n");
    printf("\033[1;32mBold Green\033[0m <- back to normal\n");
    printf("\033[46mCyan background\033[0m <- reset\n");
    printf("Normal text again.\n");
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
// Send prompt to terminal (stdout)
//====================================================

/*
static void show_prompt(void)
{
    write(__fd_stdout, "shell.bin-$ ", 12);
}
*/

static void show_prompt(void)
{
    // Build prompt string
    char prompt_str[256];
    char buf[128];
    size_t StringSize = 0;

    StringSize = (size_t) sizeof(my_un.nodename);

    if (StringSize < 128) {
        strncpy(buf, my_un.nodename, sizeof(my_un.nodename) - 1);
        buf[sizeof(buf) - 1] = '\0';  // ensure null termination
    } else {
        strcpy(buf, "shell.bin");
    }

    sprintf(prompt_str, "\033[31m%s:\033[34mshell.bin\033[0m$ ", buf);

    write(__fd_stdout, prompt_str, strlen(prompt_str));

    // write(__fd_stdout, "shell.bin-$ ", 12);
}


// Worker: read from a given fd and forward to shell's stdout (terminal).
static void pipe_worker(int fd_read)
{
    char buf[256];
    int n;
    int c;
    int i=0;

    if (fd_read < 0)
        return;

    memset(buf, 0, sizeof(buf));

    while (1)
    {
        n = read(fd_read, buf, sizeof(buf));
        if (n > 0) 
        {
            for (i = 0; i < n; i++) 
            {
                c = (int) buf[i];
                if (c != 0)
                    write(__fd_stdout, &c, 1);  // forward to terminal
            }
            memset(buf, 0, sizeof(buf));
        }
    };
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

// Create a pipe
// fd[0] → read end (for the next child’s stdin).
// fd[1] → write end (for the current child’s stdout).
// Same buffer, but two structure pointer.
// It has a shared substructure for pipe info.

    int fd[2];


    if (use_pipes == TRUE)
    {
        pipe(fd);

        // #bugbug
        // Put the command line into the pipe
        write(fd[1], cmdline, strlen(cmdline));
        write(fd[1], "\n", 1);
    }


// Prepare child’s standard streams
// stdin: the child will consume input from the pipe’s read end.
// stdout: the child’s normal output goes into the pipe’s write end.
// stderr: error messages also go into the same pipe’s write end.

    if (use_pipes == TRUE)
    {
        // Child’s stdin is the pipe’s read end.
        dup2(fd[0], STDIN_FILENO);   // child reads from pipe
        // Child’s stdout/stderr are the pipe’s write end.
        dup2(fd[1], STDOUT_FILENO);  // child writes into pipe
        dup2(fd[1], STDERR_FILENO);  // child writes into pipe
    }

// #bugbug
// Feed stdin with the full command line
// All the standard strems for the shell are
// connected to the PTY slave device. It means that 
// if we feed stdin here, the terminal will read it and 
// the child will not get it.

    //write(STDIN_FILENO, cmdline, strlen(cmdline));
    //write(STDIN_FILENO, "\n", 1);

// Feed stdin with the full command line
    //write(STDOUT_FILENO, cmdline, strlen(cmdline));
    //write(STDOUT_FILENO, "\n", 1);

/*
How it works in a normal shell

Without pipes:
 Child inherits stdin/out/err → all pointing to the PTY slave.
 So the child reads from the terminal and writes back to the terminal.
With pipes:
 Shell calls pipe(fd).

For the left child (cat file.txt):
  dup2(fd[1], STDOUT_FILENO) → its stdout goes into the pipe’s write end.
For the right child (grep foo):
  dup2(fd[0], STDIN_FILENO) → its stdin comes from the pipe’s read end.

Now the two children are connected directly through the pipe.
*/

/*
    // #backup: Old way. (it's working)

    if (use_pipes != TRUE){
        // Inject cmdline: The kernel passes it to the child.
        sc82(44010, cmdline, cmdline, cmdline);
    }
    // Launch child with just the filename
    int tid = rtl_clone_and_execute_return_tid(filename);

*/

    int tid = -1;

    if (use_pipes != TRUE)
    {
        // #test
        // It passes the cmdline, clone it and return the child's tid.
        tid = 
            rtl_clone_and_execute_return_tid_ex (
                filename, cmdline );
    }

    if (use_pipes == TRUE)
    {
        // Launch child with just the filename
        tid = rtl_clone_and_execute_return_tid(filename);
    }
   

    // Report result
    if (tid < 0) {
        write(__fd_stdout, "shell: failed to launch\n", 24);
    } else {

        // #debug
        char msg[64];
        sprintf(msg, "shell: launched tid=%d\n", tid);
        write(__fd_stdout, msg, strlen(msg));

        rtl_sleep(2000);  //2sec

        // Tell the kernel the child is now foreground 
        // sc82 (10011,tid,tid,tid);

        if (use_pipes == TRUE){
            pipe_worker(fd[0]);  // Read from the pipe
        }

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

    if (argc == 0) 
    {
        // empty line
        //reset_prompt();
        //show_prompt();
        return;
    }

    // Built-in commands
    if (strcmp(argv[0], "about") == 0) {
        write(__fd_stdout, "shell: minimal stdin/stdout shell\n", 34);
        write( __fd_stdout, "\n", 1 );  // Go to next line
    }
    else if (strcmp(argv[0], "help") == 0) {
        
        write(__fd_stdout, "shell: commands: about, help, run\n", 34);
        write( __fd_stdout, "\n", 1 );  // Go to next line

        //printf("Start\x1B[8CEnd\n");// move cursor right 8 columns
        //printf("Hello World!\n\033[31mRed text\033[0m Normal\n");

        //__test_colors_printf();

    }
    else if (strcmp(argv[0], "clear") == 0) {
        shell_builtin_clear();
    }
    else if (strcmp(argv[0], "reset") == 0) {
        __test_reset();
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
        write(__fd_stdout, "shell: unknown command\n", 24);
        write( __fd_stdout, "\n", 1 );  // Go to next line
    }

    //reset_prompt();
    //show_prompt();
}

//====================================================
// Worker loop: read from stdin, echo, accumulate
//====================================================

static void shell_worker(void)
{
    // Char support
    char char_buf[1];
    int C = 0;

// parameters:
    if (__fd_input < 0) {
        return;
    }
    if (__fd_stdout < 0) {
        return;
    }

    reset_prompt();
    show_prompt();

    while (1)
    {
        if (read( __fd_input, char_buf, 1 ) > 0)
        {
            C = (int) char_buf[0];

            // Printable ASCII
            if (C >= 0x20 && C <= 0x7E)
            {
                // Echo the char to the terminal or kernel console
                write( __fd_stdout, &char_buf[0], 1 );

                // Inject the chat into the cmdline buffer
                if (prompt_pos < sizeof(prompt)-1) 
                {
                    prompt[prompt_pos++] = C;
                    prompt[prompt_pos] = 0;
                }
            }

            // BACKSPACE
            else if (C == 0x7F || C == 0x08)
            {
                if (prompt_pos > 0) 
                {
                    prompt_pos--;
                    prompt[prompt_pos] = 0;
                    write( __fd_stdout, "\b \b", 3 );
                }
            }

            // ENTER
            else if (C == '\n' || C == '\r')
            {
                write( __fd_stdout, "\n", 1 );  // Go to next line
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

    __fd_input  = dup(STDIN_FILENO );
    __fd_stdout = dup(STDOUT_FILENO);
    __fd_stderr = dup(STDERR_FILENO);

    // Get kernel info
    uname(&my_un);


    reset_prompt();
    shell_worker();
    return EXIT_SUCCESS;
}
