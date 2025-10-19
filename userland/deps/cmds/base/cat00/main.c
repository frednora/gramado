// cat - Concatenate files and print on the standard output
// 2022 - Fred Nora.
// See:
// https://man7.org/linux/man-pages/man1/cat.1.html

//#include <rtl/gramado.h>
//#include <types.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


#define FALSE  0
#define TRUE   1

//4KB
#define __BufferSize  (4*1024)
static char buffer[__BufferSize];

static int Max = 8;  // Max number of files

// Flags
static int fNumber=FALSE;
static int fShowTabs=FALSE;
static int fShowEnds=FALSE;
//...

// Strings
const char *VersionString = "Version 1.0";
const char *HelpString = "options: --help, --version, --number";
//...

//-----------------------------
static void doHelp(void);
static void doVersion(void);
// ...
static int process_file(char *file_name);
static void __clear_buffer(void);
//-----------------------------


static void cat00_error( char *why);
static void cat00_notice(char *where);

// ==========================================

static void cat00_error( char *why)
{
    if ((void*) why == NULL)
        return;
    if (*why == 0)
        return;

	fprintf (stderr, "device error: %s\n", why);
	fflush (stderr);
}

// Log a cat.bin error.
static void cat00_notice(char *where)
{
    if ((void*) where == NULL)
        return;
    if (*where == 0)
        return;

	fprintf (stderr, "error: %s at %s\n",
		 (errno > 0 && errno < sys_nerr)  // max 
		 ? sys_errlist[errno]             // List of strings
		 : "?",
		 where );

// Send
	fflush(stderr);
}


static void doHelp(void)
{
    printf("%s\n",HelpString);
}
static void doVersion(void)
{
    printf("%s\n",VersionString);
}

// OUT:
// nwrites or fail.
static int process_file(char *file_name)
{
    int ReturnValue = 0;
    int fdRead = -1;
    register int nreads = 0;
    register int nwrites = 0;

// Parameters
    if ((void*) file_name == NULL){
        printf ("process_file: Missing file_name parameter\n");
        goto fail;
    }

//
// Open
//

    fdRead = (int) open((char *) file_name, 0, "a+");
    if (fdRead < 0){
        printf ("process_file: on open()\n");
        goto fail;
    }

//
// Read from fd.
//

    nreads = (int) read( fdRead, buffer, 511 );
    if (nreads <= 0){
        printf ("cat00: File {%d} failed on read()\n", fdRead);
        goto fail;
    }

//
// Write
//

// Write on stdout. If there's no redirection. 
// Print the whole file into the screen.
// In this case we don't have any modification flag.
    nwrites = (int) write( fileno(stdout), buffer, sizeof(buffer) );
    if (nwrites <= 0){
        printf ("cat00: File {%d} failed on write()\n", 
            fileno(stdout) );
        goto fail;
    }
    ReturnValue = nwrites;

// Clear the buffer
    __clear_buffer();

    return (int) ReturnValue;
fail:
    return (int) -1;
}

static void __clear_buffer(void)
{
    register int i=0;
    for (i=0; i<__BufferSize; i++)
        buffer[i] = 0;
}

int main(int argc, char *argv[])
{
    register int i=0;
    int FileStatus = -1;

// Max number of files.
    Max = 8;

    // #debug
    printf ("cat00: Writing on stderr\n");
    stdout = stderr;

    // #debug
    printf("CAT00.BIN: argc %d | argv[0] %s | argv[1] %s", 
        argc,       // quantos argumentos 
        argv[0],    // CAT.BIN
        argv[1] );  // FILE.TXT
    printf("\n");


    if (argc <= 1){
        printf("cat00: Few parameters. exit(0)\n");
        exit(0);
        //doHelp();
        goto fail;
    }
    if (argc >= Max){
        printf("cat00: Too many files. exit(0)\n");
        exit(0);
        goto fail;
    }

// Clear the tmp buffer.
// #todo: 
// Actually we're gonna malloc the buffer
// based on the file size. I guess.
    __clear_buffer();

// Probe for some flags.
    int isFlag = FALSE;
    for (i=1; i<argc; i++)
    {
        isFlag = FALSE;

        if ( strncmp( argv[i], "--help", 6) == 0 ){
            isFlag=TRUE;
            doHelp();
            goto done;
        }
        if ( strncmp( argv[i], "--version", 9) == 0 ){
            isFlag=TRUE;
            doVersion();
            goto done;
        }
        if ( strncmp( argv[i], "--number", 8) == 0 ){
            isFlag=TRUE;
            fNumber = TRUE;
        }
        if ( strncmp( argv[i], "--show-tabs", 11) == 0 ){
            isFlag = TRUE;
            fShowTabs = TRUE;
        }
        if ( strncmp( argv[i], "--show-ends", 11) == 0 ){
            isFlag = TRUE;
            fShowEnds = TRUE;
        }
        

        // It's NOT a flag.
        // Open the file and print the content into the screen.
        if (isFlag == FALSE)
        {
            if (i >= Max){
                goto fail;
            }

            FileStatus = (int) process_file((char *) argv[i]);
            if (FileStatus < 0){
                goto fail;
            }
        }
    };

done:
    return EXIT_SUCCESS;
fail:
    return EXIT_FAILURE;
}

