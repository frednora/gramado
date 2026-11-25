//====================================================
// shell3.bin - Improved minimal STDIN/STDOUT shell
// for Gramado OS (ring-3)
//====================================================
// Based on the original shell3 worker.
// Uses only the facilities available in
// gramde/cmds/.../api/libs/rtl and the current libc.
//====================================================

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rtl/gramado.h>
#include <sys/utsname.h>
#include <sys/wait.h>

// ----------------------------------------------------
// Configuration
// ----------------------------------------------------
#define PROMPT_MAX      256
#define ARGV_MAX        16
#define CMDLINE_MAX     128
#define HISTORY_MAX     8

// ----------------------------------------------------
// Globals
// ----------------------------------------------------

//static char prompt[PROMPT_MAX];
//static int  prompt_pos = 0;

static char history[HISTORY_MAX][PROMPT_MAX];
static int  history_count = 0;
static int  history_index = 0;          // for future up-arrow support

static int  __fd_stdout = STDOUT_FILENO;

struct utsname my_un;

// ----------------------------------------------------
// Forward declarations
// ----------------------------------------------------
static void reset_prompt(void);
static void show_prompt(void);
static void process_command(void);
static void process_run_command(const char *cmdline);
static void shell_worker(void);
static void add_to_history(const char *line);

// ----------------------------------------------------
// Prompt helpers
// ----------------------------------------------------
static void reset_prompt(void)
{
    memset(prompt, 0, sizeof(prompt));
    prompt_pos = 0;
}

static void show_prompt(void)
{
    // Coloured prompt similar to the other shell in the tree.
    // Falls back gracefully if the terminal ignores the escapes.
    char buf[160];
    const char *name = "shell3";

    if (my_un.nodename[0] != '\0')
        name = my_un.nodename;

    // red hostname : blue shell3 $ 
    sprintf(buf, "\033[31m%s:\033[shell3\033[0m$ ", name);
    write(__fd_stdout, buf, strlen(buf));
}

// ----------------------------------------------------
// Simple history (for later expansion with arrow keys)
// ----------------------------------------------------
static void add_to_history(const char *line)
{
    if (line == NULL || line[0] == '\0')
        return;

    // Avoid consecutive duplicates
    if (history_count > 0 &&
        strcmp(history[(history_count - 1) % HISTORY_MAX], line) == 0)
        return;

    strncpy(history[history_count % HISTORY_MAX], line, PROMPT_MAX - 1);
    history[history_count % HISTORY_MAX][PROMPT_MAX - 1] = '\0';
    history_count++;
}

// ----------------------------------------------------
// Launch an external program
// ----------------------------------------------------
static void process_run_command(const char *cmdline)
{
    if (cmdline == NULL || *cmdline == '\0')
        return;

    char local_cmd[CMDLINE_MAX];
    char filename[64];

    memset(local_cmd, 0, sizeof(local_cmd));
    memset(filename, 0, sizeof(filename));

    strncpy(local_cmd, cmdline, sizeof(local_cmd) - 1);

    // First token is the program name
    char *token = strtok(local_cmd, " \t");
    if (token == NULL)
        return;
    strncpy(filename, token, sizeof(filename) - 1);

    // Prefer the extended launcher that also passes the full cmdline
    int tid = rtl_clone_and_execute_return_tid_ex(filename, cmdline);

    if (tid < 0) {
        write(__fd_stdout, "shell3: failed to launch \"", 26);
        write(__fd_stdout, filename, strlen(filename));
        write(__fd_stdout, "\"\n", 2);
        return;
    }

    // Optional debug
    // char msg[64];
    // sprintf(msg, "shell3: launched tid=%d\n", tid);
    // write(__fd_stdout, msg, strlen(msg));

    // Make the child the foreground thread (kernel console / VT)
    sc82(10011, tid, tid, tid);

    // Wait until the child finishes
    rtl_join_thread(tid);
}

// ----------------------------------------------------
// Built-in command handlers
// ----------------------------------------------------
static void builtin_help(void)
{
    const char *help =
        "shell3 - minimal Gramado shell\n"
        "\n"
        "Built-ins:\n"
        "  help, about, version   - information\n"
        "  clear                  - clear screen\n"
        "  echo <text>            - print text\n"
        "  create                 - create test file + dir\n"
        "  reboot                 - reboot the machine\n"
        "  exit, quit             - leave the shell\n"
        "  msg1                   - post a test message to init\n"
        "\n"
        "Anything else is treated as an external program\n"
        "and launched with rtl_clone_and_execute_return_tid_ex.\n"
        "You can also use:  run <program> [args...]\n";
    write(__fd_stdout, help, strlen(help));
}

static void builtin_about(void)
{
    write(__fd_stdout,
          "shell3: improved minimal stdin/stdout shell for Gramado OS\n",
          58);
}

static void builtin_version(void)
{
    char buf[128];
    sprintf(buf, "shell3 (Gramado)  nodename=%s\n",
            my_un.nodename[0] ? my_un.nodename : "(unknown)");
    write(__fd_stdout, buf, strlen(buf));
}

static void builtin_clear(void)
{
    // Clear screen + home cursor (works with the VT / console that
    // already understands basic ANSI in the other shell).
    write(__fd_stdout, "\033[2J\033[H", 8);
    write(__fd_stdout, "\033[0m", 4);   // reset attributes
}

static void builtin_echo(int argc, char **argv)
{
    int i;
    for (i = 1; i < argc; i++) {
        if (i > 1)
            write(__fd_stdout, " ", 1);
        write(__fd_stdout, argv[i], strlen(argv[i]));
    }
    write(__fd_stdout, "\n", 1);
}

static void builtin_create(void)
{
    // The original test that creates a file and a directory.
    // Important: the buffers must be writable (not string literals).
    char file00[64];
    char dir00[64];

    memset(file00, 0, sizeof(file00));
    memset(dir00,  0, sizeof(dir00));

    strcat(file00, "newfile.txt");
    strcat(dir00,  "newdir");

    printf("creating %s ...\n", file00);
    rtl_create_empty_file(file00);

    printf("creating %s ...\n", dir00);
    rtl_create_empty_directory(dir00);
}

// ----------------------------------------------------
// Command dispatcher
// ----------------------------------------------------
static void process_command(void)
{
    char *argv[ARGV_MAX];
    int   argc = 0;
    char  work[PROMPT_MAX];

    // Make a working copy – strtok destroys the string
    memset(work, 0, sizeof(work));
    strncpy(work, prompt, sizeof(work) - 1);

    // Tokenise
    char *token = strtok(work, " \t");
    while (token != NULL && argc < ARGV_MAX - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;

    if (argc == 0)
        return;                     // empty line

    add_to_history(prompt);

    // ---------- Built-ins ----------
    if (strcmp(argv[0], "help") == 0 || strcmp(argv[0], "?") == 0) {
        builtin_help();
        return;
    }
    if (strcmp(argv[0], "about") == 0) {
        builtin_about();
        return;
    }
    if (strcmp(argv[0], "version") == 0 || strcmp(argv[0], "ver") == 0) {
        builtin_version();
        return;
    }
    if (strcmp(argv[0], "clear") == 0 || strcmp(argv[0], "cls") == 0) {
        builtin_clear();
        return;
    }
    if (strcmp(argv[0], "echo") == 0) {
        builtin_echo(argc, argv);
        return;
    }
    if (strcmp(argv[0], "create") == 0) {
        builtin_create();
        return;
    }
    if (strcmp(argv[0], "reboot") == 0) {
        write(__fd_stdout, "Rebooting...\n", 13);
        rtl_reboot();
        return;
    }
    if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0) {
        write(__fd_stdout, "Bye.\n", 5);
        exit(0);
    }
    if (strcmp(argv[0], "msg1") == 0) {
        // Post a test message to the init process (tid 0)
        rtl_post_to_tid(0, 44888, 1234, 5678);
        return;
    }

    // Explicit "run" form (kept for compatibility)
    if (strcmp(argv[0], "run") == 0 && argc > 1) {
        char cmdline[CMDLINE_MAX];
        int  i;

        memset(cmdline, 0, sizeof(cmdline));
        strncpy(cmdline, argv[1], sizeof(cmdline) - 1);

        for (i = 2; i < argc; i++) {
            size_t left = sizeof(cmdline) - strlen(cmdline) - 1;
            if (left <= 1)
                break;
            strncat(cmdline, " ", left);
            strncat(cmdline, argv[i], left - 1);
        }
        process_run_command(cmdline);
        return;
    }

    // ---------- Default: treat as external program ----------
    // Rebuild the original command line from argv so we can
    // pass arguments correctly.
    {
        char cmdline[CMDLINE_MAX];
        int  i;

        memset(cmdline, 0, sizeof(cmdline));
        strncpy(cmdline, argv[0], sizeof(cmdline) - 1);

        for (i = 1; i < argc; i++) {
            size_t left = sizeof(cmdline) - strlen(cmdline) - 1;
            if (left <= 1)
                break;
            strncat(cmdline, " ", left);
            strncat(cmdline, argv[i], left - 1);
        }
        process_run_command(cmdline);
    }
}

// ----------------------------------------------------
// Main input worker (the heart of the original design)
// ----------------------------------------------------
static void shell_worker(void)
{
    char buf[1];
    int  C;

    show_prompt();

    for (;;) {
        if (read(STDIN_FILENO, buf, 1) <= 0)
            continue;

        C = (unsigned char)buf[0];

        // Printable ASCII
        if (C >= 0x20 && C <= 0x7E) {
            write(__fd_stdout, &buf[0], 1);

            if (prompt_pos < (int)sizeof(prompt) - 1) {
                prompt[prompt_pos++] = (char)C;
                prompt[prompt_pos]   = '\0';
            }
            continue;
        }

        // Backspace / Delete
        if (C == 0x08 || C == 0x7F) {
            if (prompt_pos > 0) {
                prompt_pos--;
                prompt[prompt_pos] = '\0';
                write(__fd_stdout, "\b \b", 3);
            }
            continue;
        }

        // Enter
        if (C == '\n' || C == '\r') {
            write(__fd_stdout, "\n", 1);
            process_command();
            reset_prompt();
            show_prompt();
            continue;
        }

        // (Future: Ctrl-C, arrows for history, Tab completion…)
    }
}

// ----------------------------------------------------
// main
// ----------------------------------------------------
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    __fd_stdout = STDOUT_FILENO;

    // Best-effort uname (may be empty on some builds)
    memset(&my_un, 0, sizeof(my_un));
    // uname(&my_un);          // uncomment if your libc has a working uname

    printf("shell3.bin: Hello from Gramado!\n");
    printf("Type 'help' for available commands.\n");

    reset_prompt();
    shell_worker();

    return 0;
}
