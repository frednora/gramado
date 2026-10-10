/* 
 * File: link_r0.ld
 * ld script to make the x86_64 window server image.
 * Base:
 *     0x30A00000
 * Entry point: 
 *     0x30A01000
 *     See: See the entrypoint at crt0.c in rtl/.
 * History:
 *     2021 - Created by Fred Nora.
 */
/* See: https://wiki.osdev.org/Linker_Scripts */
/* See: https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html */
/* 64-bit ELF format for the x86-64 architecture. */

OUTPUT_FORMAT("elf64-x86-64")
ENTRY(main)

/* Essa aplicação está usando o diretório do processo kernel. */

SECTIONS
{
    . = 0x30A01000;
    .text 0x30A01000: 
    {
        _server_begin = .;
        _start_of_server_image = .;
        
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
        *(.bss)  *(COMMON)
        _bss_end = .; 
    }

    _server_end = .; 
    _end_of_server_image = .; 
}


