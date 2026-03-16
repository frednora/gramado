ENTRY(_start)

SECTIONS
{
  . = 0x40000000;

  /* Code + read-only data together */
  .text : {
    *(.text*)
    *(.rodata*)
  }

  /* Writable data */
  .data : {
    *(.data*)
  }

  /* Zero-initialized data */
  .bss : {
    __bss_start = .;     /* mark start of bss */
    *(.bss*)
    *(COMMON)
    __bss_end = .;       /* mark end of bss */
  }

  /* Reserve a stack */
  . = ALIGN(16);
  __stack_top = .;
  . = . + 0x4000;        /* 16 KB stack */
  __stack_bottom = .;
}
