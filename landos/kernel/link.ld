/* 
 * File: link.ld
 * 
 * Linker script for kernel image.
 *
 * Base:
 *     0x30000000
 *
 * Entry point: 
 *     0x30001000
 *     Kernel entry point in 1pump/arch/x86/boot.asm. 
 */

/* See: https://wiki.osdev.org/Linker_Scripts */

/* 64-bit ELF format for the x86-64 architecture. */

OUTPUT_FORMAT("elf64-x86-64")
ENTRY(_kernel_begin)

SECTIONS 
{
    _kernel_begin = .;

    . = 0x30001000;
    .text 0x30001000:
    {
        _code_begin = .;
        *(.head_x86_64)  *(.text) 
        _code_end = .; 
    }
    
    . = ALIGN(4096);
    .rodata :
    {
        _rodata_begin = .;
        *(.rdata)  *(.rodata) 
        _rodata_end = .;
    }
    
    . = ALIGN(4096);
    .data :  
    { 
        _data_begin = .; 
        *(.data) 
        _data_end = .; 
    }
    
    . = ALIGN(4096);
    .bss : 
    { 
        _bss_begin = .; 
        *(COMMON)  *(.bss)
        _bss_end = .; 
    }


    _kernel_end = .; 
}





