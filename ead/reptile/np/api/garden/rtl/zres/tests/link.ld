OUTPUT_FORMAT("elf32-i386")
ENTRY(__main)
SECTIONS
{ 
	/* Essa aplicação está usando o diretório do processo kernel. */
	/* Quando tiver seu próprio diretório de páginas o endereço será 0x00401000.*/
	
	.text 0x00401000: 
    {
        code = .; 
		_code = .; 
		__code = .;
           
	  *(.head_x86)    
	  *(.text)	 
	  
       . = ALIGN(4096);
    }

    .data :
    {
        data = .; 
		_data = .; 
		__data = .;
		
        *(.data)
		
        . = ALIGN(4096);
    }

    .bss :
    {
        bss = .; 
		_bss = .; 
		__bss = .;
		
        *(.bss)
		
        . = ALIGN(4096);
    }

    end = .; 
	_end = .; 
	__end = .;
}