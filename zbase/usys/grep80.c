// simple grep
// Created by Copilot for Gramado OS.
// Not tested yet.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

void grep_file(const char *filename, const char *pattern) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("simple_grep");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int c;
    int pos = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '\n' || pos >= MAX_LINE_LENGTH - 1) {
            line[pos] = '\0';
            if (strstr(line, pattern) != NULL) {
                printf("%s\n", line);
            }
            pos = 0;
        } else {
            line[pos++] = (char)c;
        }
    }

    // Check the last line if it doesn't end with a newline
    if (pos > 0) {
        line[pos] = '\0';
        if (strstr(line, pattern) != NULL) {
            printf("%s\n", line);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <pattern> <file>\n", argv[0]);
        return 1;
    }

    // Search the pattern in the specified file
    grep_file(argv[2], argv[1]);

    return 0;
}