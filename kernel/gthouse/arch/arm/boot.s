// -----------------------------------------------------------------------------
// Context notes:
// -----------------------------------------------------------------------------
// AArch64 execution state (64‑bit mode).
// Privilege level: EL1 or EL3 (since we’re touching MMIO directly).
// Boot stage: First-stage bootloader, past SoC boot ROM, not yet in OS kernel.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PL011 UART register definitions
// -----------------------------------------------------------------------------
.equ UART_BASE,   0x09000000   // Base address of PL011 UART
.equ UARTDR,      0x00         // Data Register offset
.equ UARTFR,      0x18         // Flag Register offset
.equ UARTIBRD,    0x24         // Integer Baud Rate Divisor
.equ UARTFBRD,    0x28         // Fractional Baud Rate Divisor
.equ UARTLCR_H,   0x2C         // Line Control Register
.equ UARTCR,      0x30         // Control Register
.equ UARTFR_TXFF, 0x20         // Bit mask: Transmit FIFO Full



.extern bl_main


.section .text

.global _start
.global uart_init
.global uart_putc

_start:

// Initial stack configuration
    adrp x1, __stack_top
    add  x1, x1, :lo12:__stack_top
    mov  sp, x1

// -------------------------------------------------------------------------
// Initialize UART before use
// -------------------------------------------------------------------------
// 64‑bit EL1 environment, UART initialization
    bl uart_init

// -------------------------------------------------------------------------
// Print "Hi" to UART
// -------------------------------------------------------------------------
    mov x0, #'H'
    bl uart_putc
    mov x0, #'i'
    bl uart_putc
    mov x0, #'\n'
    bl uart_putc

// Clear bss to enter in C

    adrp x2, __bss_start
    add  x2, x2, :lo12:__bss_start
    adrp x3, __bss_end
    add  x3, x3, :lo12:__bss_end

clear_bss:
    cmp x2, x3
    b.ge bss_done
    str xzr, [x2], #8    // store zero, advance by 8 bytes
    b clear_bss
bss_done:

// jump into C bootloader main
    bl bl_main

// A soft place to fall
loop:
    wfi     // Wait For Interrupt (CPU idle)
    b loop  // Infinite loop, CPU stays here

// -----------------------------------------------------------------------------
// UART initialization routine
// -----------------------------------------------------------------------------
uart_init:
    mov x1, #UART_BASE

    // Disable UART before configuration
    mov w2, #0x0
    str w2, [x1, #UARTCR]

    // Set baud rate divisors (example values for QEMU virt, ~115200 baud)
    mov w2, #1
    str w2, [x1, #UARTIBRD]
    mov w2, #40
    str w2, [x1, #UARTFBRD]

    // Line control: 8 bits, no parity, 1 stop bit, FIFO enabled
    mov w2, #0x70
    str w2, [x1, #UARTLCR_H]

    // Enable UART, TX, RX
    mov w2, #0x301
    str w2, [x1, #UARTCR]

    ret

// -----------------------------------------------------------------------------
// UART output routine
// -----------------------------------------------------------------------------
uart_putc:
    mov x1, #UART_BASE
__uart_wait:
    ldr w2, [x1, #UARTFR]  // Load UARTFR (Flag Register)
    tst w2, #UARTFR_TXFF   // Test TX FIFO full bit
    b.ne __uart_wait       // If full, spin until ready
__uart_do_putc:
    str w0, [x1, #UARTDR]  // Write character to UARTDR (Data Register)
    ret

