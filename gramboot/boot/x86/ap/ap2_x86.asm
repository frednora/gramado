; AP trampoline: real mode -> quick A20 -> minimal GDT -> protected mode -> 32-bit entry
; NASM syntax
[bits 16]
[org 0x20000]

apx86_start:
    ; Basic 16-bit setup
    cli                     ; no interrupts during mode switch
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    xor sp, sp

    ; Quick A20 enable via port 0x92 (fast A20)
    in   al, 0x92
    or   al, 0000_0010b     ; set A20
    and  al, 1111_1110b     ; ensure reset bit (bit0) is 0
    out  0x92, al

    ; Tiny settle
    jmp short .a20_ok
.a20_ok:

    ; Point DS to our load segment (org 0x20000 => DS=0x2000)
    mov ax, 0x2000
    mov ds, ax

    ; Load GDT (DS:offset)
    lgdt [gdt_ptr]

    ; Enter protected mode: set PE and far-jump
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp CODE_SEL:ap_pm_entry

; ---------------- Selectors ----------------
CODE_SEL equ 0x08
DATA_SEL equ 0x10

; ---------------- GDT ----------------
align 8
gdt:
    dq 0                    ; null

    ; Code: base=0, limit=0xFFFFF, access=0x9A, flags=0xCF (4KiB gran, 32-bit)
    dw 0xFFFF               ; limit low
    dw 0x0000               ; base low
    db 0x00                 ; base mid
    db 0x9A                 ; access
    db 0xCF                 ; granularity (limit high=0xF + flags)
    db 0x00                 ; base high

    ; Data: base=0, limit=0xFFFFF, access=0x92, flags=0xCF
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

gdt_end:

gdt_ptr:
    dw gdt_end - gdt - 1    ; size
    dd gdt                  ; base (physical address; accessed via DS=0x2000)

; ---------------- 32-bit protected mode entry ----------------
[bits 32]
ap_pm_entry:
    ; Load flat data segments
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set stack to the TOP of the buffer
    mov esp, stack_begin

    ; Optional: mark we reached PM
    mov dword [0x00290000], 0xA0A0A0A0

    ; Minimal idle: enable interrupts so HLT can wake on IPI/timer
.pm_idle:
    sti
    hlt
    jmp .pm_idle

; ---------------- Stack buffer ----------------
stack_size equ (stack_begin - stack_end)
align 16
stack_end:
    times (1024 * 2) db 0       ; 2 KiB stack space
stack_begin:
