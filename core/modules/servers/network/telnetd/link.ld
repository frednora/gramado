

OUTPUT_FORMAT("elf64-x86-64")
ENTRY(main)

/* 0x00200000 = image base */
/* 0x00201000 = The entry point */

SECTIONS
{ 
    /* Essa aplicação está usando o diretório do processo kernel. */
    /* Quando tiver seu próprio diretório de páginas o endereço será 0x00401000.*/

    .text 0x00201000: 
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

