/* 
 * File: link.ld
 *   Linker script to build the x86_64 kernel image.
 *   Created by Fred Nora.
 *
 * Notes:
 *   - The kernel image is loaded by the bootloader at 0x30000000.
 *   - Execution begins at 0x30001000 (entry point in head_64.asm).
 *   - We define symbols to mark the start/end of the whole image,
 *     and the start/end of each section (.text, .rodata, .data, .bss).
 */

OUTPUT_FORMAT("elf64-x86-64")

/* Entry point symbol: execution begins here */
ENTRY(_code_begin)

SECTIONS 
{
    /* 
     * Define the true load base of the kernel image.
     * This is where the bootloader copies the binary in memory.
     * We only place a symbol here, no section yet.
     * The 768 MB mark.
     */
    . = 0x30000000;
    _kernel_begin = .;

    /* 
     * Define the start of the code section (.text).
     * Execution begins here at 0x30001000.
     */
    . = 0x30001000;
    .text : {
        _code_begin   = .;          /* Start of code section (Entry point) */
        *(.head_x86_64)             /* Early assembly startup code */
        *(.text)                    /* All kernel C functions */
        _code_end     = .;          /* End of code section */
    }

    /* Read-only data (constants, strings) */
    . = ALIGN(4096);
    .rodata : {
        _rodata_begin = .;
        *(.rdata)
        *(.rodata)
        _rodata_end   = .;
    }

    /* Initialized global/static variables */
    . = ALIGN(4096);
    .data : {
        _data_begin = .;
        *(.data)
        _data_end   = .;
    }
    
    /* Uninitialized globals (zeroed at boot) */
    . = ALIGN(4096);
    .bss : {
        _bss_begin = .;
        *(.bss)
        *(COMMON)
        _bss_end   = .;
    }

    /* 
     * End of the kernel image.
     * This marks the final address after .bss.
     * Useful for calculating total image size.
     */
    _kernel_end        = .;
}
