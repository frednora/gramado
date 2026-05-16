// simple shell
// Created by Copilot to run on Gramado OS.
// Not tested yet.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>

#define MAX_COMMAND_LENGTH 1024

void execute_command(char *command) {
    char *filename;
    
    // Trim leading and trailing whitespace
    while (*command == ' ' || *command == '\t') command++;
    filename = command;
    while (*command && *command != ' ' && *command != '\t' && *command != '\n') command++;
    *command = '\0';

    if (filename[0] == '\0') {
        return; // Empty command
    }

    if (strcmp(filename, "exit") == 0) {
        exit(0);
    }

    // Execute the command using rtl_clone_and_execute
    int ret = rtl_clone_and_execute(filename);
    if (ret != 0) {
        printf("Command not found: %s\n", filename);
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    int c;
    int pos = 0;

    while (1) {
        printf("> ");
        fflush(stdout);

        // Reset command position
        pos = 0;
        memset(command, 0, MAX_COMMAND_LENGTH);

        // Read command input using fgetc
        while ((c = fgetc(stdin)) != '\n' && c != EOF) {
            if (pos < MAX_COMMAND_LENGTH - 1) {
                command[pos++] = (char)c;
            }
        }
        command[pos] = '\0'; // Null-terminate the string

        // Execute the command
        execute_command(command);
    }

    return 0;
}
