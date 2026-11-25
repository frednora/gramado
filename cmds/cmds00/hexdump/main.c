// hexdump - Dump file contents in hexadecimal.
// Created for Gramado OS.

#include <sys/types.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define FALSE  0
#define TRUE   1

//#define __BufferSize  (4*1024)
#define __BufferSize  (512)

static unsigned char buffer[__BufferSize];

static void doHelp(void);
static int process_file(char *file_name);
static void dump_line(
    unsigned long offset,
    unsigned char *buf,
    int count);

static void __clear_buffer(void);

/*
 * Help
 */

static void doHelp(void)
{
    printf("usage: hexdump file\n");
}

/*
 * Clear buffer
 */

static void __clear_buffer(void)
{
    register int i;

    for (i = 0; i < __BufferSize; i++)
        buffer[i] = 0;
}

/*
 * Dump one 16-byte line.
 */

static void dump_line(
    unsigned long offset,
    unsigned char *buf,
    int count)
{
    register int i;

    printf("%x  ", offset);

    /* Hexadecimal */

    for (i = 0; i < 16; i++)
    {
        if (i < count)
            printf("%x ", buf[i]);
        else
            printf("   ");

        if (i == 7)
            printf(" ");
    }

    printf(" |");

    /* ASCII */

    for (i = 0; i < count; i++)
    {
        if (buf[i] >= 32 && buf[i] <= 126)
            putchar(buf[i]);
        else
            putchar('.');
    }

    for (; i < 16; i++)
        putchar(' ');

    printf("|\n");
}

/*
 * Process file
 */

static int process_file(char *file_name)
{
    int fd;
    int nreads;
    int i;

    unsigned long offset = 0;

    if ((void *) file_name == NULL)
        return -1;

    fd = open(file_name, 0, "a+");

    if (fd < 0)
    {
        printf("hexdump: cannot open %s\n", file_name);
        return -1;
    }

    while (1)
    {
        __clear_buffer();

        nreads = read(fd, buffer, __BufferSize);

        if (nreads <= 0)
            break;

        for (i = 0; i < nreads; i += 16)
        {
            int count;

            count = nreads - i;

            if (count > 16)
                count = 16;

            dump_line(
                offset + i,
                &buffer[i],
                count);
        }

        offset += nreads;
    }

    close(fd);

    return 0;
}

/*
 * Main
 */

int main(int argc, char *argv[])
{

    if (argc != 2){
        doHelp();
        goto fail;
    }

    if (process_file(argv[1]) < 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;

fail:
    return EXIT_FAILURE;
}

