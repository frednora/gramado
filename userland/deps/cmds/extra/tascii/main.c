// Print the ascii table.
// 2018 - Fred Nora.

#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STANDARD_ASCII_MAX  128


int main(int argc, char *argv[])
{
    unsigned char count=0;
    unsigned char number_of_columns = 8;
    unsigned char standard_ascii_max = STANDARD_ASCII_MAX;

// #todo
// Maybe we can get some parameters.

    printf("ASCII.BIN:\n");

    for ( count=0; 
          count < standard_ascii_max; 
          count++ )
    {
        // Printable chars
        if ( count >= 0x20 && count <= 0x7F )
        {
            printf(" [%d:%c]", count, count);
        }
        
        if ((count % number_of_columns) == 0){
            printf("\n");
        }
    };

    return EXIT_SUCCESS;

//fail:
    //return EXIT_FAILURE;
}

