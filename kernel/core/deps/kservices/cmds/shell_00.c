#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 1024   // Maximum command line size
#define MAX_ARGS 64     // Maximum number of arguments

int main() {
    char line[MAX_LINE];   // Buffer for input line
    char *args[MAX_ARGS];  // Array to hold pointers to arguments

    while (1) {
        // Print the shell prompt
        printf("myshell> ");
        fflush(stdout);

        // Read a line of input. If EOF (Ctrl+D) then exit the loop.
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove the newline character at the end of the line
        line[strcspn(line, "\n")] = '\0';

        // Check if the user wants to exit the shell.
        if (strcmp(line, "exit") == 0) {
            break;
        }

        // Tokenize the input into command and arguments
        int i = 0;
        char *token = strtok(line, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  // Null-terminate the list of arguments

        // Skip if the input was empty (i.e., just pressing enter)
        if (args[0] == NULL) {
            continue;
        }

        // Fork a new process
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {  // Child process
            // Execute the command by searching the PATH for the executable
            if (execvp(args[0], args) == -1) {
                perror("execvp");
            }
            exit(EXIT_FAILURE);  // Exit in case execvp fails
        } else {  // Parent process
            // Wait for the child process to finish
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}
