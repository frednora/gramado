// ASCII.BIN - Enhanced ASCII Table
// Improved version with better formatting and options

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* control_names[32] = {
    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
    "BS",  "HT",  "LF",  "VT",  "FF",  "CR",  "SO",  "SI",
    "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
    "CAN", "EM",  "SUB", "ESC", "FS",  "GS",  "RS",  "US"
};

void print_help(const char* progname)
{
    printf("Usage: %s [OPTIONS]\n\n", progname);
    printf("Options:\n");
    printf("  -c N     Number of columns (default: 8)\n");
    printf("  -e       Show extended ASCII (0-255)\n");
    printf("  -a       Show all characters including controls\n");
    printf("  -h       Show this help\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    int columns = 8;
    int extended = 0;
    int show_all = 0;
    int max_char = 127;

    int i=0;

    printf("=== ASCII Table ===\n\n");

    // Simple argument parsing
    for (i=1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[i], "-e") == 0) {
            extended = 1;
            max_char = 255;
        }
        else if (strcmp(argv[i], "-a") == 0) {
            show_all = 1;
        }
        else if (strcmp(argv[i], "-c") == 0 && i+1 < argc) {
            columns = atoi(argv[++i]);
            if (columns < 1) columns = 1;
            if (columns > 16) columns = 16;
        }
    }

    printf("Dec  Hex  Char | ");
    for (i=1; i < columns; i++) {
        printf("Dec  Hex  Char | ");
    }
    printf("\n");
    printf("----------------------------------------------------\n");

    for (i=0; i <= max_char; i++)
    {
        if (i > 0 && (i % columns) == 0) {
            printf("\n");
        }

        // Control character
        if (i < 32) {
            if (show_all) {
                printf("%3d  %02X   %s  | ", i, i, control_names[i]);
            } else {
                printf("%3d  %02X   ^%c   | ", i, i, i + 64);
            }
        }
        // Printable character
        else if (i >= 32 && i <= 126) {
            printf("%3d  %02X    %c   | ", i, i, (char)i);
        }
        // DEL and Extended
        else if (i == 127) {
            printf("%3d  %02X   DEL  | ", i, i);
        }
        else if (i > 127) {
            printf("%3d  %02X   .    | ", i, i);
        }
    }

    printf("\n\n");
    printf("Total characters shown: %d\n", max_char + 1);
    if (!show_all)
        printf("Tip: use -a to show control characters\n");

    fflush(stdout);
    return EXIT_SUCCESS;
}

