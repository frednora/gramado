OUTPUT_FORMAT("pe-i386")
ENTRY(__bootloader__)

SECTIONS
{
    .text 0x00021000:
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
        *(COMMON)
        *(.bss_heap)
        *(.bss_stack)
        . = ALIGN(4096);
    }

    end = .; 
    _end = .; 
    __end = .;
    _bootloader_end = .; 
}



