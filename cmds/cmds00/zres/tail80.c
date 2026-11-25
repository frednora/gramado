// simple 'tail' program
// Created by Copilot for Gramado OS.
// Not tested yet.

/*
Let's create a simple Unix-like utility similar to the tail command, 
which prints the last few lines of a file. 
This utility will use fgetc() to read the file and store the lines 
in a circular buffer to efficiently handle large files.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_NUM_LINES 10
#define MAX_LINE_LENGTH 1024

void tail_file(const char *filename, int num_lines) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("simple_tail");
        return;
    }

    // Circular buffer to store the last num_lines lines
    char **lines = malloc(num_lines * sizeof(char *));
    if (lines == NULL) {
        perror("malloc");
        fclose(file);
        return;
    }
    for (int i = 0; i < num_lines; i++) {
        lines[i] = malloc(MAX_LINE_LENGTH);
        if (lines[i] == NULL) {
            perror("malloc");
            for (int j = 0; j < i; j++) {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            return;
        }
        lines[i][0] = '\0';
    }

    int c;
    int pos = 0;
    int line_index = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '\n' || pos >= MAX_LINE_LENGTH - 1) {
            lines[line_index][pos] = '\0';
            line_index = (line_index + 1) % num_lines;
            pos = 0;
        } else {
            lines[line_index][pos++] = (char)c;
        }
    }

    // Print the last num_lines lines
    for (int i = 0; i < num_lines; i++) {
        int index = (line_index + i) % num_lines;
        if (lines[index][0] != '\0') {
            printf("%s\n", lines[index]);
        }
    }

    // Free allocated memory
    for (int i = 0; i < num_lines; i++) 
    {
        // #bugbug
        // Our free() is not working properly yet.
        free(lines[i]);
    }
    free(lines);
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file> [num_lines]\n", argv[0]);
        return 1;
    }

    int num_lines = (argc >= 3) ? atoi(argv[2]) : DEFAULT_NUM_LINES;

    // Print the last num_lines lines of the specified file
    tail_file(argv[1], num_lines);

    return 0;
}
