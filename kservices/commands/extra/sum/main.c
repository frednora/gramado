/*
 * sum command.
 * Sum bytes in file mod 2^16
 * unix v7 ?
 */

#include <stdio.h>
#include <stdlib.h>

// + Open file
// + Read file
// + Close file. (Saving it into the disk).
int main(int argc, char *argv[])
{
    register FILE *f;
    register unsigned sum=0;
    register int i=0;
    int c=0;
    register long nbytes=0;
    int errflg = 0;

    i=1;

// Loop for multiple files.
    do {
    
        if (i<argc) {
            if ((f = fopen(argv[i], "a+")) == NULL) 
            {
                fprintf(stderr, "sum: Can't open %s\n", argv[i]);
                errflg += 10;
                continue;
            }
        // If no filename was provided.
        } else {
            f = stdin;
        };

        sum = 0;
        nbytes = 0;

        while ((c = getc(f)) != EOF)
        {
            nbytes++;
            
            // #bugbug: '01' ?
            if (sum & 01)
                sum = (sum>>1) + 0x8000;
            else
                sum >>= 1;
            
            sum += c;
            sum &= 0xFFFF;
        };

        if ( ferror(f) )
        {
            errflg++;
            //fprintf(stderr, "sum: read error on %s\n", argc>1?argv[i]:"-");
            // #ugly
            printf("sum: read error on %s\n", 
                argc > 1 
                ? argv[i]
                : "-" );
        }

        //
        // Print results.
        //
        
        //printf("%05u%6ld", sum, (nbytes+BUFSIZ-1)/BUFSIZ);
        printf("sum {%d | %d} \n", 
            sum, 
            (nbytes+BUFSIZ-1)/BUFSIZ );

        if (argc > 2)
            printf(" %s\n", argv[i]);

        printf("\n");
        
        // #bugbug
        // Here we're gonna save the file in the disk.

        fclose(f);

    } while (++i < argc);

    printf("done\n");
    return (int) errflg;

fail:
    return EXIT_FAILURE;
}

