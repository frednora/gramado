; AP trampoline: real mode -> quick A20 -> minimal GDT -> protected mode -> 32-bit entry
; NASM syntax

; This is switching to long mode.
; Its running fine in Virtualbox with ICH9 chipset.

[bits 16]
[org 0x20000]

; -- 128 KB mark -----------
; 0x20000 - Base for the AP
; 0x28FF0 - Start of 16bit stack
; 0x29000 - Signature position
; So we have 64 KB to use (192-128) 
; -- 192 KB mark -----------
; 0x00030000 - FAT (Don't touch this thing)

apx86_start:
    jmp StartCode

; ---------------- Selectors ----------------
CODE_SEL     equ 0x08
DATA_SEL     equ 0x10
CODE64_SEL   equ 0x18
DATA64_SEL   equ 0x20
; ...

; ---------------- GDT ----------------
align 8
gdt:
    dq 0                    ; null

    ; 32-bit code: base=0, limit=0xFFFFF, access=0x9A, flags=0xCF
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00

    ; 32-bit data: base=0, limit=0xFFFFF, access=0x92, flags=0xCF
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

    ; 64-bit code: base=0, limit=0xFFFFF, access=0x9A, flags=0x2F (L=1, D=0, G=1)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0x2F
    db 0x00

    ; 64-bit-compatible data: writable data segment
    ; Long mode ignores base/limit for addressing, but SS must be valid.
    ; access=0x92 (present, writable), flags=0xCF (G=1; D bit ignored in long mode)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

gdt_end:

gdt_ptr:
    dw gdt_end - gdt - 1
    dd gdt

;=========================================================

StartCode:
    cli
    mov ax, 0x2000 
    mov ss, ax
    mov sp, 0x8FF0  ; Right before the signature 

    ; Switch to 0x2000 segment to reach 0x29000
    cld
    mov ds, ax
    mov es, ax
    ;#debug
    ;mov byte [0x9000], 0xA0   ; writes to physical 0x29000
    ;mov byte [0x9001], 0xA0

    ; Quick A20 enable via port 0x92 (fast A20)
    xor ax, ax
    in   al, 0x92
    or   al, 0000_0010b     ; set A20
    and  al, 1111_1110b     ; ensure reset bit (bit0) is 0
    out  0x92, al

    ; Tiny settle
    jmp short .a20_ok
.a20_ok:

    ; Point DS to our load segment (org 0x20000 => DS=0x2000)
    ; We need this to load the GDT.
    mov ax, 0x2000
    mov ds, ax

    ; Load GDT (DS:offset)
    lgdt [gdt_ptr]

    ; Enter protected mode: set PE and far-jump
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

; Fail
;    jmp CODE_SEL:ap_pm32_entry

; Force 32-bit far jump encoding (o32 prefix)
    jmp dword CODE_SEL:ap_pm32_entry

; Far jump with 32-bit offset (o32), 16-bit selector
; Override: 66 forces a 32-bit offset in 16-bit mode.
    ;db 0x66, 0xEA
    ;dd ap_pm32_entry   ; 32-bit offset (little-endian)
    ;dw CODE_SEL        ; 16-bit selector

; ---------------- 32-bit protected mode entry ----------------
[bits 32]
ap_pm32_entry:

    ; Load flat data segments
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set stack to the TOP of the buffer
    ;mov esp, stack_begin

    ; Optional: mark we reached PM
    mov dword [0x29000], 0xA0A0A0A0

;; =============================================================================

;[bits 32]

PML4_PHYS     EQU  0x000000000009C000
IA32_EFER     EQU  0xC0000080
EFER_LME_BIT  EQU  (1 << 8)


;ap_pm32_entry:
switch_to_long_mode:

    ; Load flat data segments (as you already do)
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 1) Load CR3 with shared PML4 physical address
    mov eax, PML4_PHYS
    mov cr3, eax

    ; 2) Enable PAE
    mov eax, cr4
    or  eax, (1 << 5)          ; CR4.PAE
    mov cr4, eax

    ; 3) Set EFER.LME
    mov ecx, IA32_EFER
    rdmsr
    or  eax, EFER_LME_BIT
    wrmsr

    ; 4) Enable paging (CR0.PG)
    mov eax, cr0
    or  eax, (1 << 31)         ; CR0.PG
    mov cr0, eax

    ; 5) Far jump to 64-bit code segment to activate long mode
    ; dword because it is still in 32bit mode.
    jmp dword CODE64_SEL:ap_long_entry

[bits 64]

ap_long_entry:

    xor rax, rax

    mov ax, DATA64_SEL
    mov ds, ax
    mov es, ax
    ;mov fs, ax
    ;mov gs, ax
    mov ss, ax

    ; Optional: mark we reached long mode
    mov dword [0x29000], 0x64646464

    jmp SoftPlaceToFall
    jmp $

SoftPlaceToFall:
    cli
    hlt
    jmp SoftPlaceToFall

; ---------------- Stack buffer ----------------
stack_size equ  (stack64_begin - stack64_end)
align 8
stack64_end:
    times (1024 * 2) db 0       ; 2 KiB stack space
stack64_begin:

