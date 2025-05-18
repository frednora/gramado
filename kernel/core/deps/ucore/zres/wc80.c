// simple 'wc' program
// Created by Copilot for Gramado OS.
// Not tested yet.

/*
Let's create a simple Unix-like utility similar to the wc command, 
which counts the number of lines, words, and characters in a file.
 This utility will use fgetc() to read the file.

Here is the code for the simple wc-like utility:
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void wc_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("simple_wc");
        return;
    }

    int c;
    int lines = 0, words = 0, characters = 0;
    int in_word = 0;

    while ((c = fgetc(file)) != EOF) {
        characters++;
        if (c == '\n') {
            lines++;
        }
        if (isspace(c)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            words++;
        }
    }

    fclose(file);

    printf(" %d %d %d %s\n", lines, words, characters, filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Count lines, words, and characters in the specified file
    wc_file(argv[1]);

    return 0;
}