// shell.c 

//#include <ctype.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctls.h>
#include <sys/ioctl.h>
#include <types.h>
//#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Shared
#include "../shared/globals.h"

// Backend
#include "core.h"
#include "shell.h"

// Frontend
#include "../ui/ui.h"

#include "../terminal.h"

// Client-side library.
#include <gws.h>

// Testing the 'foreground console' configuration.
// It's working.
void __test_ioctl(void)
{
    //printf ("~ioctl: Tests...\n");

// #test
// TIOCCONS - redirecting console output
// Changing the output console tty.
// A implementaçao que trata do fd=1 eh console_ioctl
// e nao tty_ioctl.
// https://man7.org/linux/man-pages/man2/TIOCCONS.2const.html
    printf("\n");
    printf("Changing the output console\n");
    // IN: fd, request, arg.
    int ioctl_return;
    ioctl_return = (int) ioctl( STDOUT_FILENO, TIOCCONS, 0 );
    printf("ioctl_return: {%d}\n",ioctl_return);
    //printf("Done\n");


// Setup cursor position.
    //ioctl(1, 1001, 10);  // Cursor x
    //ioctl(1, 1002, 10);  // Cursor y
    //ioctl(1, 1003,  2);  //switch to the virtual console 2. 

// Setup cursor position.
    //ioctl( STDOUT_FILENO, 1001, 0 );  // Cursor x 
    //ioctl( STDOUT_FILENO, 1002, 0 );  // Cursor y
    //printf("| Test: Cursor position at 0:0\n");

/*
// Indentation
    ioctl(1, 1010, 8);
    printf ("| Starting at column 8\n");
*/
//-----------

    // 512 = right
    // Get max col
    int maxcol = ioctl( STDOUT_FILENO, 512, 0 );

//-----------
// Goto first line, position 0.
    ioctl( STDOUT_FILENO, 1008, 0 );
    printf("a"); fflush(stdout);
// Goto first line. last position 
// Not the last column. If we hit the last, 
// the console goes to the next line.
    ioctl( STDOUT_FILENO, 1008, maxcol -2 ); //Set
    printf("A"); fflush(stdout);

//-----------
// Goto last line, position 0.
    ioctl( STDOUT_FILENO, 1009, 0 );
    printf("z"); fflush(stdout);
// Goto last line. last position 
// Not the last column. If we hit the last, 
// the console goes to the next line.
    ioctl( STDOUT_FILENO, 1009, maxcol -2 );
    printf("Z"); fflush(stdout);

// Scroll forever.
    //while(1){
    //    printf("%d\n",rtl_jiffies());
    //   ioctl(1,999,0);  //scroll
    //};

// Flush?
// It's not about flushing the ring3 buffer into the file.
    //ioctl ( STDIN_FILENO,  TCIFLUSH, 0 ); // input
    //ioctl ( STDOUT_FILENO, TCIFLUSH, 0 ); // console
    //ioctl ( STDERR_FILENO, TCIFLUSH, 0 ); // regular file
    //ioctl ( 4,             TCIFLUSH, 0 ); // invalid?

// Invalid limits
    //ioctl ( -1, -1, 0 );
    //ioctl ( 33, -1, 0 );

// Changing the color.
// #deprecated.
// The application will not change the colors anymore.
    //ioctl(1, 1000,COLOR_CYAN);

    //printf ("done\n");
}



