// simple 'head' program
// Created by Copilot for Gramado OS.
// Not tested yet.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_NUM_LINES 10
#define MAX_LINE_LENGTH 1024

void head_file(const char *filename, int num_lines) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("simple_head");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int c;
    int pos = 0;
    int lines_printed = 0;

    while (lines_printed < num_lines && (c = fgetc(file)) != EOF) {
        if (c == '\n' || pos >= MAX_LINE_LENGTH - 1) {
            line[pos] = '\0';
            printf("%s\n", line);
            pos = 0;
            lines_printed++;
        } else {
            line[pos++] = (char)c;
        }
    }

    // Check if the last line doesn't end with a newline
    if (lines_printed < num_lines && pos > 0) {
        line[pos] = '\0';
        printf("%s\n", line);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file> [num_lines]\n", argv[0]);
        return 1;
    }

    int num_lines = (argc >= 3) ? atoi(argv[2]) : DEFAULT_NUM_LINES;

    // Print the first num_lines lines of the specified file
    head_file(argv[1], num_lines);

    return 0;
}
