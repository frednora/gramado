
// word_freq.c
// Created by Copilot

/*
Let's create a simple utility that counts the frequency of 
each word in a text file and prints the results. 
This utility will use various libc functions, including 
fgetc(), fputs(), strtok(), and qsort().
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 100

typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordFreq;

int compare_word_freq(const void *a, const void *b) {
    return ((WordFreq *)b)->count - ((WordFreq *)a)->count;
}

void add_word(WordFreq word_freq[], int *word_count, const char *word) {
    for (int i = 0; i < *word_count; i++) {
        if (strcmp(word_freq[i].word, word) == 0) {
            word_freq[i].count++;
            return;
        }
    }
    if (*word_count < MAX_WORDS) {
        strcpy(word_freq[*word_count].word, word);
        word_freq[*word_count].count = 1;
        (*word_count)++;
    }
}

void count_word_frequency(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("word_freq");
        return;
    }

    WordFreq word_freq[MAX_WORDS] = {0};
    int word_count = 0;
    char buffer[MAX_WORD_LENGTH];
    int c, pos = 0;

    while ((c = fgetc(file)) != EOF) {
        if (isalnum(c)) {
            buffer[pos++] = tolower(c);
            if (pos >= MAX_WORD_LENGTH - 1) {
                buffer[pos] = '\0';
                add_word(word_freq, &word_count, buffer);
                pos = 0;
            }
        } else if (pos > 0) {
            buffer[pos] = '\0';
            add_word(word_freq, &word_count, buffer);
            pos = 0;
        }
    }

    if (pos > 0) {
        buffer[pos] = '\0';
        add_word(word_freq, &word_count, buffer);
    }

    fclose(file);

    qsort(word_freq, word_count, sizeof(WordFreq), compare_word_freq);

    for (int i = 0; i < word_count; i++) {
        printf("%s: %d\n", word_freq[i].word, word_freq[i].count);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Count the word frequency in the specified file
    count_word_frequency(argv[1]);

    return 0;
}
