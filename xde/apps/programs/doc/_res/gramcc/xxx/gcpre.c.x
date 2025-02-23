

// Gramado C preprocessor test.

int
main( argc, argv, envp )
      int argc;
      char **argv;
      char **envp;
{
    register int i;
    char *filename;
	
	printf("Initializing Gramado C preprocessor test ...\n");
	
	
	
	for( i=1; i<argc; i++ )
	{
        //Como estamos em um for, o '-' indica o início 
        //de uma nova palavra a ser analizada.		
        if (argv[i][0] == '-')
        {
			
			register char *str = argv[i] +1;
			
			//a
		    if(str[0] == 'a')	
			{
				
			}
			
			//b
		    if(str[0] == 'b')	
			{
				
			}

			//c
		    if(str[0] == 'c')	
			{
				
			}

            if( !strcmp( str, "string1") )
            {
				
			}

			//d
		    if(str[0] == '=')	
			{
                register char *p = &str[1];			
			    
				while(*p)
				{
	                switch( *p++ )
		            {
		                case 'x':
		                    //combine_dump = 1;
		                    break;
		                            
						case 'y':
		                    // flow_dump = 1;
		                    break;
		                            
						case 'z':
		                    // global_reg_dump = 1;
		                    break;
 
		            };
				};                				
				
				
				
			};


            if( !strcmp( str, "string2") )
            {
				
			}			
			
			
            if( !strcmp( str, "string3") )
            {
				
			}	

            if( !strcmp( str, "r") )
            {
				
			}				
			
            if( !strcmp( str, "s") )
            {
				
			}				

            if( !strcmp( str, "t") )
            {
				
			}				
			

        }else{
		
            //Se não há argumentos então o que segue é um nome de arquivo ?? 		
			filename = argv[i];
		};	
	};
	
	
    if(filename == 0)
	{
        printf("no input file specified");
    }	
	
	printf("%s",filename);
	printf("*hang");
    while(1){}	
};