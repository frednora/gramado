

// basic.asm
extern void basic_asm_main();


int basic_main ( int argc, char *argv[] ){
	
	int retval = 0;
	
	printf("basic_main: Initializing 32bit basic interpreter ...");
	
	
    retval = (int) basic_asm_main ();	
	
	switch (retval)
	{
		case 0:
		    printf("basic_main: return 0\n");
			break;
		
		//...
		
		default:
		    printf("basic_main: defaul return\n");
			break;
	};
	
	printf("basic_main: done\n");
	
    return (int) retval;	
};