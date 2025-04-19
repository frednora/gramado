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
static int process_file(char *file_name, int file_index);
static void __clear_buffer(void);
//-----------------------------

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
static int process_file(char *file_name, int file_index)
{
    int ReturnValue = 0;
    int fdRead = -1;
    int fdWrite = 1;  //stdout
    register int nreads = 0;
    register int nwrites = 0;

// Parameters
    if ( (void*) file_name == NULL ){
        printf ("process_file: Missing file_name parameter in file {%d}\n", 
            file_index);
        goto fail;
    }
    // see: Max
    if (file_index < 0){
        printf ("process_file: file_index parameter failed in file {%d}\n", 
            file_index);
        goto fail;
    }

// Open
    fdRead = (int) open((char *) file_name, 0, "a+");
    if (fdRead < 0){
        printf ("process_file: File {%d} failed on open()\n", 
            file_index);
        goto fail;
    }
// Read from fd.
    nreads = read( fdRead, buffer, 511 );
    if (nreads <= 0){
        printf ("cat: File {%d} failed on read()\n", 
            file_index);
        goto fail;
    }
// Write on stdout. If there's no redirection. 
// Print the whole file into the screen.
// In this case we don't have any modification flag.
    nwrites = write( fdWrite, buffer, sizeof(buffer) );
    if (nwrites <= 0){
        printf ("cat: File {%d} failed on write()\n", 
            file_index);
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

    /*
    // #debug
    printf("CAT.BIN: argc %d | argv[0] %s | argv[1] %s\n", 
        argc,       // quantos argumentos 
        argv[0],    // CAT.BIN
        argv[1] );  // FILE.TXT
    printf("\n");
    */

    if (argc <= 1){
        printf("Few parameters\n");
        doHelp();
        goto fail;
    }
    if (argc >= Max){
        printf("Too many files\n");
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
        
        //
        // It's NOT a flag.
        //

        // Open the file and print the content into the screen.
        if (isFlag == FALSE)
        {
            if (i >= Max){
                goto fail;
            }
            FileStatus = (int) process_file((char *) argv[i], i);
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

