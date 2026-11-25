// TPRINTF.BIN - Enhanced stdio Test Suite for Gramado OS
// Tests printf, file I/O, buffering, scanf, and more.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>     // for getopt if available, or manual parsing

// =============================================
// Test Functions
// =============================================

static void test_printf(void)
{
    printf("\n=== PRINTF TEST ===\n");

    int mi = (1 << (sizeof(int)*8 - 1)) + 1;
    char *np = NULL;
    char *ptr = "Hello world!";

    printf(":: %s\n", ptr);
    printf(":: printf single string\n");
    printf(":: %s is null pointer\n", np);
    printf("%d = 5\n", 5);
    printf("%d = - max int\n", mi);
    printf("char %c = 'a'\n", 'a');
    printf("hex %x = ff\n", 0xff);
    printf("hex %02x = 00\n", 0);
    printf("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
    printf("%d %s(s)\n", 0, "message");
    printf("%d %s(s) with %%\n", 0, "message");

    // Formatting tests
    printf("justif: \"%-10s\"\n", "left");
    printf("justif: \"%10s\"\n", "right");
    printf(" 3: %04d zero padded\n", 3);
    printf(" 3: %-4d left justif.\n", 3);
    printf(" 3: %4d right justif.\n", 3);
    printf("-3: %04d zero padded\n", -3);
    printf("-3: %-4d left justif.\n", -3);
    printf("-3: %4d right justif.\n", -3);

    // 64-bit test (if supported)
    printf("64-bit test: %lld\n", 123456789012345LL);
}

static void test_buffering(void)
{
    printf("\n=== BUFFERING TEST ===\n");

    FILE *f = fopen("buffer_test.txt", "w+");
    if (!f) {
        printf("Failed to create test file\n");
        return;
    }

    setvbuf(f, NULL, _IONBF, 0);   // No buffering
    fprintf(f, "No buffering test\n");

    setvbuf(f, NULL, _IOLBF, 0);   // Line buffering
    fprintf(f, "Line buffering test\n");

    setvbuf(f, NULL, _IOFBF, 4096); // Full buffering
    fprintf(f, "Full buffering test\n");

    fclose(f);
    printf("Buffering modes tested. File: buffer_test.txt\n");
}

static void test_file_io(void)
{
    printf("\n=== FILE I/O TEST ===\n");

    //FILE *f = fopen("stdio_test.txt", "w+");
    FILE *f = fopen("GRAMADO.TXT", "w+");
    if (!f) {
        printf("Failed to open file\n");
        return;
    }

    fprintf(f, "Hello from stdio test!\n");
    fprintf(f, "Line 2: %d + %d = %d\n", 25, 17, 42);
    fflush(f);

    rewind(f);

    char line[128];
    while (fgets(line, sizeof(line), f)) {
        printf("Read: %s", line);
    }

    fclose(f);
    printf("File I/O test completed.\n");
}

static void test_scanf(void)
{
    printf("\n=== SCANF TEST ===\n");
    printf("Enter an integer and a string (ex: 42 hello):\n");

    int num;
    char str[64] = {0};

    if (scanf("%d %63s", &num, str) == 2) {
        printf("You entered: %d and '%s'\n", num, str);
    } else {
        printf("scanf failed or incomplete input.\n");
    }
}

static void test_stress(void)
{
    int i=0;

    printf("\n=== STRESS TEST ===\n");
    for (i=0; i < 1000; i++) {
        printf("Stress line %04d: %x %u %s\n", i, i*123, i*456, "test");
        if (i % 100 == 0) fflush(stdout);
    }
    printf("Stress test completed.\n");
}

// =============================================
// Main
// =============================================

static void print_help(const char *progname)
{
    printf("Usage: %s [OPTIONS]\n\n", progname);
    printf("Options:\n");
    printf("  -p, --printf     Run printf formatting tests\n");
    printf("  -f, --file       Run file I/O tests\n");
    printf("  -b, --buffer     Test different buffering modes\n");
    printf("  -s, --scanf      Run interactive scanf test\n");
    printf("  -a, --all        Run all tests\n");
    printf("  -S, --stress     Run stress test\n");
    printf("  -h, --help       Show this help\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    int do_printf = 0;
    int do_file = 0;
    int do_buffer = 0;
    int do_scanf = 0;
    int do_stress = 0;
    int i = 1;

    printf("=== TPRINTF.BIN - Enhanced stdio Test Suite ===\n\n");

    if (argc < 2) {
        // Default: run basic tests
        do_printf = 1;
        do_file = 1;
        do_buffer = 1;
    } else {
        for (i=1; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                print_help(argv[0]);
                return EXIT_SUCCESS;
            }
            else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--printf") == 0)
                do_printf = 1;
            else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0)
                do_file = 1;
            else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--buffer") == 0)
                do_buffer = 1;
            else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--scanf") == 0)
                do_scanf = 1;
            else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
                do_printf = do_file = do_buffer = do_scanf = 1;
            }
            else if (strcmp(argv[i], "-S") == 0 || strcmp(argv[i], "--stress") == 0)
                do_stress = 1;
        }
    }

    if (do_printf)   test_printf();
    if (do_buffer)   test_buffering();
    if (do_file)     test_file_io();
    if (do_scanf)    test_scanf();
    if (do_stress)   test_stress();

    printf("\n=== All requested tests completed ===\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}

