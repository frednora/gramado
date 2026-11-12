// simple 'uniq' program
// Created by Copilot for Gramado OS.
/*
Let's create a simple Unix-like utility similar to the uniq command, 
which filters out repeated lines in a file. This utility will use fgetc() 
to read the file and avoid using malloc and free by using static arrays.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

void uniq_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("simple_uniq");
        return;
    }

    char current_line[MAX_LINE_LENGTH];
    char previous_line[MAX_LINE_LENGTH] = "";
    int c;
    int pos = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '\n' || pos >= MAX_LINE_LENGTH - 1) {
            current_line[pos] = '\0';
            if (strcmp(current_line, previous_line) != 0) {
                printf("%s\n", current_line);
                strcpy(previous_line, current_line);
            }
            pos = 0;
        } else {
            current_line[pos++] = (char)c;
        }
    }

    // Check if the last line doesn't end with a newline
    if (pos > 0) {
        current_line[pos] = '\0';
        if (strcmp(current_line, previous_line) != 0) {
            printf("%s\n", current_line);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Filter out repeated lines in the specified file
    uniq_file(argv[1]);

    return 0;
}
