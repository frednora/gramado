; pic.asm


;============================================
; PIC
; Early PIC initialization.
; PIC MODE
; Selecting the 'Processor Interrup Mode'.
; All the APIC components are ignored here, and
; the system will operate in the single-thread mode using LINT0.

PIC_early_initialization:

    cli

    xor rax, rax
    mov al, 00010001b    ; begin PIC1 initialization.
    out 0x20, al

    IODELAY
    mov al, 00010001b    ; begin PIC2 initialization.
    out 0xA0, al

    IODELAY
    mov al, 0x20         ; IRQ 0-7: interrupts 20h-27h.
    out 0x21, al

    IODELAY
    mov al, 0x28         ; IRQ 8-15: interrupts 28h-2Fh.
    out 0xA1, al

    IODELAY
    mov al, 4
    out 0x21, al

    IODELAY
    mov al, 2
    out 0xA1, al

    IODELAY
    ;mov al, 00010001b    ; 11 sfnm 80x86 support.
    mov al, 00000001b     ; 01 80x86 support.
    out 0x21, al

    IODELAY
    out 0xA1, al

    IODELAY

; Mask all interrupts.
    cli
    mov  al, 255
    out  0xA1,  al
    IODELAY
    out  0x21,  al
    IODELAY

    ret 

