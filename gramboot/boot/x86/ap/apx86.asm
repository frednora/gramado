[bits 16]
[org 0x20000]          ; code physically at 0x20000

apx86_start:
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; switch to 0x2000 segment to reach 0x29000
    mov ax, 0x2000
    mov ds, ax
    mov byte [0x9000], 0xA0   ; writes to physical 0x29000
    mov byte [0x9001], 0xA0

AP_LOOP:
    cli
    hlt
    jmp AP_LOOP
