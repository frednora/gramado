// TPRINTF.BIN
// Testing libc functions.

#include <ctype.h> 
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// ----------------------------

static int printf_test(void);

// ----------------------------

/*
 * printf_test:
 *     This function test the printf function.
 *     Obs: We can implement this test in user mode.
 * Obs:
 *     If you compile this file with
 *     gcc -Wall $(YOUR_C_OPTIONS) -DTEST_PRINTF -c printf.c
 * you will get a normal warning:
 *   printf.c:214: warning: spurious trailing `%' in format
 * this line is testing an invalid % at the end of the format string.
 * this should display (on 32bit int machine) :
 * Hello world!
 * printf test
 * (null) is null pointer
 * 5 = 5
 * -2147483647 = - max int
 * char a = 'a'
 * hex ff = ff
 * hex 00 = 00
 * signed -3 = unsigned 4294967293 = hex fffffffd
 * 0 message(s)
 * 0 message(s) with %
 * justif: "left      "
 * justif: "     right"
 *  3: 0003 zero padded
 *  3: 3    left justif.
 *  3:    3 right justif.
 * -3: -003 zero padded
 * -3: -3   left justif.
 * -3:   -3 right justif.
 */
// It's not a standard implementation, but it is working.

static int printf_test(void)
{
    int mi=0;
    int i=5;
    unsigned int bs = (sizeof(int) * 8);
    char *np = 0;
    char *ptr = "Hello world!";
    char buf[128];

    mi = (1 << (bs-1)) + 1;

// Hello
	printf(":: %s\n", ptr);
// Single string
	printf(":: printf single string\n");
// Null pointer
	printf(":: %s is null pointer\n", np);
// 5
	printf("%d = 5\n", i);
// int
	printf("%d = - max int\n", mi);
// Char
	printf("char %c = 'a' \n", 'a');
// Hex char.
	printf("hex %x = ff   \n", 0xff);
	printf("hex %02x = 00 \n", 0);
// Signed, unsigned and hex.
	printf("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
// String
	printf("%d %s(s)", 0, "message");
	printf("\n");
// String
	printf("%d %s(s) with %%", 0, "message");
	printf("\n");

//=================
    // We still do not have support for this.

/*
    sprintf(buf, "justif: \"%-10s\"\n", "left"); 
    printf("%s", buf);

    sprintf(buf, "justif: \"%10s\"\n", "right"); 
    printf("%s", buf);

    sprintf(buf, " 3: %04d zero padded\n", 3); 
    printf("%s", buf);

    sprintf(buf, " 3: %-4d left justif.\n", 3); 
    printf("%s", buf);

    sprintf(buf, " 3: %4d right justif.\n", 3); 
    printf("%s", buf);

    sprintf(buf, "-3: %04d zero padded\n", -3); 
    printf("%s", buf);

    sprintf(buf, "-3: %-4d left justif.\n", -3); 
    printf("%s", buf);

    sprintf(buf, "-3: %4d right justif.\n", -3); 
    printf("%s", buf);
*/

    return 0;
}


int main (int argc, char *argv[])
{
// Testing printf() function.

    if ((void*)stdout == NULL)
        goto fail;

    // #debug
    printf("TPRINTF.BIN: initializing ...");
    fflush(stdout);

// Call the test.
    printf_test();

    // #debug
    printf("TPRINTF.BIN: Done");
    fflush(stdout);

    return EXIT_SUCCESS;

fail:
    return EXIT_FAILURE;
}

