
// text to uppercase
// Created by Copilot to Gramado OS.
// Not tested yet.

/*
Let's create a simple utility that reads a text file and converts 
its content to uppercase. This utility will demonstrate the use of various libc 
functions, including fgetc(), fputc(), and toupper().
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void convert_to_uppercase(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("text_to_uppercase");
        return;
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        perror("text_to_uppercase");
        fclose(input_file);
        return;
    }

    int c;
    while ((c = fgetc(input_file)) != EOF) {
        fputc(toupper(c), output_file);
    }

    fclose(input_file);
    fclose(output_file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Convert the content of the input file to uppercase and write to the output file
    convert_to_uppercase(argv[1], argv[2]);

    return 0;
}

