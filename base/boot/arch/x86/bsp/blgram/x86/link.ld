/* 
 * File: link.ld
 *   Linker script to build the x86 bootloader image.
 *   Created by Fred Nora.
 */

OUTPUT_FORMAT("pe-i386")

/* Entry point symbol: execution begins here */
ENTRY(_code_begin)

SECTIONS
{

    /* Beginning of the bootloader image */
    . = 0x00020000;
    _bootloader_begin = .;

    /* Code section starts at 0x00021000 (entry point) */
    .text 0x00021000:
    {
        _code_begin   = .;          /* Start of code section (Entry point) */
        code = .; 
        _code = .; 
        __code = .; 
        *(.head_x86) 
        *(.text)
        . = ALIGN(4096);
    }

    .data :
    {
        _data_begin = .;
        data = .; 
        _data = .; 
        __data = .;
        *(.data)
        . = ALIGN(4096);
    }

    .bss :
    {
        _bss_begin = .;
        bss = .; 
        _bss = .; 
        __bss = .;
        *(.bss)
        *(COMMON)
        *(.bss_heap)
        *(.bss_stack)
        . = ALIGN(4096);
    }

    /* End of the bootloader image */
    end = .; 
    _end = .; 
    __end = .;
    _bootloader_end = .; 
}

