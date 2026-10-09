
// conio.c
// It is not part of the C standard library or ISO C, 
// nor is it defined by POSIX.

#include <types.h> 
#include <stddef.h>
#include <stdio.h>
#include <conio.h>

// putch - Writes a character directly to the console.
// # do not flush
int putch(int c)
{
    printf ( "%c", (char) c );
    return 1;
}

// cputs - Writes a string directly to the console.
// # do not flush
int cputs (const char *str)
{
    printf ("%s", str);
    return 1;
}

// Interna.
// Suporte ao echo.
/* Read 1 character - echo defines echo mode */
static char getchEcho (int echo)
{
    char ch=0;

    switch (echo)
    {
		//sem echo
		//case 0:
		//@todo:    
		//	break;
			
		//com echo
		//case 1:
		//@todo:
		//    break;

		// negligenciado por enquanto.
        default:
            ch = (char) getchar();
            break;
    };

    return (char) ch;
}

/* Read 1 character without echo */
int getch (void)
{
    return (int) getchEcho (0);
}

/* Read 1 character with echo */
int getche (void)
{
    return (int) getchEcho (1);
}

