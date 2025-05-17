.global _start
.global uart_putc

.section .text
_start:
    mov x0, #'H'   // ASCII 'H'
    bl uart_putc   // Print character
    mov x0, #'i'
    bl uart_putc   // Print 'i'
    mov x0, #'\n'
    bl uart_putc   // Print newline

loop:
    wfi            // Wait for interrupt (CPU idle)
    b loop         // Stay in infinite loop

uart_putc:
    // x0 = character to print
    mov x1, #0x09000000  // Base address of UART (PL011)

wait:
    ldr w2, [x1, #0x18]  // Read UARTFR (Flag Register)
    tst w2, #0x20        // Check if TX FIFO is full
    b.ne wait            // If full, wait until ready

    str w0, [x1]         // Write character to UARTDR (Data Register)
    ret
