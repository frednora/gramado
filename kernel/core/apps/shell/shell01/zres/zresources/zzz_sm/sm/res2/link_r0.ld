
OUTPUT_FORMAT("elf64-x86-64")
ENTRY(__user_app__)

/* 0x30E00000 = image base */
/* 0x30E01000 = The entry point */

SECTIONS
{ 

    .text 0x30E01000: 
    {
        code = .;  
        _code = .; 
        __code = .;
        *(.text) 
        . = ALIGN(4096);
    }

    /* .rodata ? */

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





