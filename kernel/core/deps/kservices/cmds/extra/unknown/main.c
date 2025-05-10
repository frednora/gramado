
// unknown.bin
// Remonta o nome do arquivo, caso ele tenha sido
// digitado com espaços ...

// rtl
#include <stdio.h>

char buf[512];
int nread = 1;

int get(int ifile) 
{
    char *ibuf;
    //static ibuf;
    
    //printf ("$");
    //fflush(stdout);

    if (--nread){
        return (*ibuf++);
    }

    if ( nread = read(ifile, buf, 512, 0 ) )
    {
        if(nread < 0)
            goto err;
        
        ibuf = buf;
        return (*ibuf++);
    }

    nread = 1;
    return (0);

err:
    nread = 1;
    printf ("read error\n");
    return(0);
}


int main(int argc, char *argv[])
{
    char b[242];
    char c[60];

    //int nread = 1;

    int l, isw, k, ifile, i, j;

    if (--argc <= 0)
	{
	    ifile = 0;
	    argc = 0;
	    goto newl;
	}

    l = 1;

    while (argc--)
    {
        printf("%s:\n \n",argv[l]);

        // open

        ifile = open( argv[l++], 0, 0 );

	    if(ifile < 0){
			printf("cannot open input file\n");
		    exit (1);
		}
		
		printf ("ifile={%d}\n",ifile);

	    newl:
	    isw = j =  0;
	    i = -1;
	    
        cont:

        while ( (b[++i] = get(ifile)) != 0 )
        {

            //printf ("$");
            //fflush(stdout);

            // Se for letra.
            if ( (b[i] >= 'a' & b[i] <= 'z') || 
                 (b[i] >= 'A' & b[i] <= 'Z') )
            {
                c[j++] = b[i];
                goto cont;
            }

			// se for -
            if (b[i] == '-')
            {
                c[j++] = b[i];

				// Enquanto não acabar a linha.
                if ((b[++i] = get(ifile)) != '\n')
                {
					c[j++] = b[i];
				    goto cont;
                }
                if(j == 1) goto newl;
                isw = 1;
                i = -1;

                // Espaços.
                while ( ((b[++i] = get(ifile)) == ' ') || 
                         (b[i] == '\t') || 
                         (b[i] == '\n') )
                {
                };
                c[j++] = b[i];
                goto cont;
            }

            // Pulando linha
            if (b[i] == '\n')
            {
				if(isw != 1)
				    goto newl;
				i = -1; 
            }

			// Finaliza e exibe.
			if(isw == 1)
			{
				//printf ("j={%d} {%s}\n",j,c);
				            
				k = 0;
				c[j++] = '\n';

				while(k < j)
				{
					printf ("OUTPUT: \n"); 
					putchar(c[k++]);
					putchar('\n');
                    fflush(stdout);
				}
			}

			isw = j = 0;
		}; //while

	}; //while

    printf ("unknown: done\n");
    return 0;
} //main


