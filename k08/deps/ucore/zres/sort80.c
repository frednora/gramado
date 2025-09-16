// simple 'sort' program
// Created by Copilot for Gramado OS.
// Not tested yet.

/*
Let's create a simple Unix-like utility similar to the sort command, 
which reads lines from a file, sorts them, and prints the sorted lines. 
This utility will use fgetc() to read the file and will avoid using malloc and free by using static arrays.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 1024

void sort_lines(char lines[][MAX_LINE_LENGTH], int num_lines) {
    char temp[MAX_LINE_LENGTH];
    for (int i = 0; i < num_lines - 1; i++) {
        for (int j = i + 1; j < num_lines; j++) {
            if (strcmp(lines[i], lines[j]) > 0) {
                strcpy(temp, lines[i]);
                strcpy(lines[i], lines[j]);
                strcpy(lines[j], temp);
            }
        }
    }
}

void sort_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("simple_sort");
        return;
    }

    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int num_lines = 0;
    int c;
    int pos = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '\n' || pos >= MAX_LINE_LENGTH - 1) {
            lines[num_lines][pos] = '\0';
            num_lines++;
            pos = 0;
            if (num_lines >= MAX_LINES) {
                fprintf(stderr, "simple_sort: too many lines\n");
                break;
            }
        } else {
            lines[num_lines][pos++] = (char)c;
        }
    }

    // Check if the last line doesn't end with a newline
    if (pos > 0) {
        lines[num_lines][pos] = '\0';
        num_lines++;
    }

    fclose(file);

    // Sort the lines
    sort_lines(lines, num_lines);

    // Print the sorted lines
    for (int i = 0; i < num_lines; i++) {
        printf("%s\n", lines[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Sort and print the lines of the specified file
    sort_file(argv[1]);

    return 0;
}
