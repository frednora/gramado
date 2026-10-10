
OUTPUT_FORMAT("elf64-x86-64")
ENTRY(main)

/* 0x00200000 = image base */
/* 0x00201000 = The entry point */

SECTIONS
{ 
    . = 0x00200000;
    _app_begin = .;

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

    _app_end = .;
}

