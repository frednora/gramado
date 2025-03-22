; head_64.asm
; Kernel entry point.
; Created by Fred Nora.

%include "header/header1.asm"

; segment .head_x86_64
__HEAD

[bits 64]

; See: init.c
extern _saved_bootblock_base
extern _magic
extern _I_kmain

; See:
; kernel.h
__SYSTEM_BOOTING EQU  1
extern _system_state


;---------------------------------------------
; Function table
; see: mod.c
; see: newm0/
extern _xp_die                   ; 0 (no arg)
extern _xp_putchar_test          ; 1 (no arg)
extern _xp_reboot                ; 2 (no arg)
extern _xp_refresh_screen        ; 3 (no arg)
extern _xp_putchar_in_fgconsole  ; 4 (1 arg)
; ...
;---------------------------------------------


;========================================================
; _kernel_begin:
; Entry point.
; This is the entry point of the 64bit kernel image.
; The boot loader jumps here in long mode using a 64bit gdt.
; #todo: 
; We need to check the bit in CS when the boot loader
; makes the jump.
; + The Stack Register (rsp) will be loaded with the 
;   ring 0 base kernel stack pointer.
; IN:
; The boot loader delivers a magic value in edx and
; a boot block in 0x90000.
; unit 0: Kernel begin.
; Jump
; Th CS stuff.
; Do a proper 64-bit jump. Should not be needed as the ...
; jmp EARLY_GDT64.Code:0x30001000 in the boot loader would have sent us ...
; out of compatibility mode and into 64-bit mode.
; The function _go_to_kernel jumps here
; from head.s in BL.BIN.

; ----------------------------------------
; ::(1)
; Kernel entry point
; IN:
; eax = 0
; ebx = 0x00090000 (boot block address)
; ecx = 0
; edx = 1234       (Signature)

global _kernel_begin 
_kernel_begin:
; We don't have any print support yet.

    cli
    cld 
    jmp START

; ----------------------
; header data area:
; Exporting ...
; see: mod.c
; see: newm0/ in the module.
; #todo: maybe we can have this table in another place.

align 4
    nop                ; Nop
    DB '__GRAMADO__'   ; Signature
    DQ _xp_die                   ; symbol 0: (no arg)
    DQ _xp_putchar_test          ; symbol 1: (no arg)
    DQ _xp_reboot                ; symbol 2: (no arg)
    DQ _xp_refresh_screen        ; symbol 3: (no arg)
    DQ _xp_putchar_in_fgconsole  ; symbol 4: (1 arg)
    ; ...
; ----------------------


align 4
    %include "header/header2.asm"
align 4

; START
; #todo
; We can save some values just for debug purpose.
START:

;
; -----------------------------------------------
; Boot block:
; All the information we got from the boot loader for now.
; The bootblock has a lot of data.

; Save the bootblock address.
    mov dword [_saved_bootblock_base], ebx
; Save the signature.
    mov dword [_magic], edx
    ; ...

; Clear some registers.
; Load our own 64-bit gdt.
; Setup data registers and base kernel stack.
; Load a NULL ldt.

    mov rax, qword __SYSTEM_BOOTING
    mov qword [_system_state], rax 

    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx

    mov  r8, rax
    mov  r9, rax
    mov r10, rax
    mov r11, rax
    mov r12, rax
    mov r13, rax
    mov r14, rax
    mov r15, rax

; GDT
; This gdt is here in this document.
    lgdt [EARLY_GDT64.Pointer]

; Segment registers:
; See: boot loader for CS initialization.
    mov ax, EARLY_GDT64.Data
    mov ds, ax
    mov es, ax
    ; Do we need FS and GS in x86_64?
    mov fs, ax
    mov gs, ax
; Early kernel stack. 
; 64 KB.
    mov ss, ax
    mov rsp,  _EarlyKernelStack

; LDT
; Initialize ldt with a NULL selector.
    xor rax, rax
    lldt ax

; Clear registers
; RBP, RSI, RDI.
    xor rax, rax
    mov rbp, rax
    mov rsi, rax
    mov rdi, rax

; IDT
; Uma falta, ou excessão gera uma interrupção não-mascaravel ... certo?
; então mesmo que eu deixe as interrupções do pic mascaradas elas vão acontecer?
; Sendo assim, esses vetores precisam ser 
; tratados mesmo antes de tratarmos os vetores das interrupções mascaraves.

; Disable IRQs
; Out 0xFF to 0xA1 and 0x21 to disable all IRQs.
; Esse rotina existe mais pra frente,
; mas poderíamos antecipa-la.

    ;mov al, 0xFF  
    ;out 0xA1, al
    ;out 0x21, al

;See: unit0.asm
; This is so dangeours

    call setup_idt      ; Create a common handler, 'unhandled_int'.
    call setup_faults   ; Setup vectors for faults and exceptions.
    call setup_vectors  ; Some new vectors.

; IDT
; #danger:
    lidt [_IDT_register] 

; TR
; tss stuff.
 
    ;mov rax, qword tss0

    ;mov [EARLY_GDT64.tssData + 2], ax
    ;shr rax, 16
    ;mov [EARLY_GDT64.tssData + 4],  al
    ;mov [EARLY_GDT64.tssData + 7],  ah

; Load TR.
; 0x2B = (0x28+3).

    ;mov ax, word 0x2B
    ;ltr ax

; PIC
; Early PIC initialization.
; PIC MODE
; Selecting the 'Processor Interrup Mode'.
; All the APIC components are ignored here, and
; the system will operate in the single-thread mode using LINT0.
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

; PIT
; Early PIT initialization.
; Setup system timers.
; Some frequencies to remember.
; PIT 8253 e 8254 = (1234DD) 1193181.6666 / 100 = 11930. ; 1.19MHz.
; APIC timer      = 3,579,545 / 100 = 35796  3.5 MHz.
; 11931    ; (1193181.6666 / 100 = 11930) timer frequency 100 HZ.
; Send 0x36 to 0x43.
; Send 11931 to 0x40.

    xor rax, rax
    mov al, byte 0x36
    mov dx, word 0x43
    out dx, al
    IODELAY
    mov eax, dword 11931
    mov dx, word 0x40
    out dx, al
    IODELAY
    mov al, ah
    out dx, al
    IODELAY

; #todo: 
; RTC

; Unmask all maskable interrupts.
    mov al, 0
    out 0xA1, al
    IODELAY
    out 0x21, al
    IODELAY

; No interrupts
    cli

; Set up registers.
; See:
; https://wiki.osdev.org/CPU_Registers_x86-64

;--------------------------------------
; Debug registers:
; DR0 ~ DR7
; Debug registers.
; Disable break points.

; DR0~DR3
; Contain linear addresses of up to 4 breakpoints. 
; If paging is enabled, they are translated to physical addresses.

; DR6
; It permits the debugger to determine 
; which debug conditions have occurred.

;--------------------------------------

; Clear register 
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    mov  r8, rax
    mov  r9, rax
    mov r10, rax
    mov r11, rax
    mov r12, rax
    mov r13, rax
    mov r14, rax
    mov r15, rax

;--------------------------------------

; Use the calling convention for this compiler.
; rdi
; No return
; See: kmain.c
; #todo: arch type (2) ??

    xor rax, rax
    mov rdi, rax    ; First argument.
    ; ...

; Call kmain() function in kmain.c
    call _I_kmain
dieLoop:
    cli
    hlt
    jmp dieLoop

; =======================================================
; _x86_64_initialize_machine
;    Called by kmain() to make the early initialization.
align 8
global _x86_64_initialize_machine
_x86_64_initialize_machine:
    ret

;; GDT
;; See:
;; https://wiki.osdev.org/Setting_Up_Long_Mode
;; Entry size ?
;; o dpl são os bits 5 e 6 do access byte.
align 8
EARLY_GDT64:            ; Global Descriptor Table (64-bit).
.Null: equ $ - EARLY_GDT64    ; The null descriptor.
    dw 0xFFFF           ; Limit (low).
    dw 0                ; Base (low).
    db 0                ; Base (middle)
    db 0                ; Access.
    db 1                ; Granularity.
    db 0                ; Base (high).
.Code: equ $ - EARLY_GDT64    ; The code descriptor.
    dw 0                ; Limit (low).
    dw 0                ; Base (low).
    db 0                ; Base (middle)
    db 10011010b        ; Access (exec/read).
    db 10101111b        ; Granularity, 64 bits flag, limit19:16.
    db 0                ; Base (high).
.Data: equ $ - EARLY_GDT64    ; The data descriptor.
    dw 0                ; Limit (low).
    dw 0                ; Base (low).
    db 0                ; Base (middle)
    db 10010010b        ; Access (read/write).
    db 00000000b        ; Granularity.
    db 0                ; Base (high).
.Ring3Code: equ $ - EARLY_GDT64    ; The code descriptor.
    dw 0                     ; Limit (low).
    dw 0                     ; Base (low).
    db 0                     ; Base (middle)
    db 11111010b             ; Access (exec/read).
    db 10101111b             ; Granularity, 64 bits flag, limit19:16.
    db 0                     ; Base (high).
.Ring3Data: equ $ - EARLY_GDT64    ; The data descriptor.
    dw 0                     ; Limit (low).
    dw 0                     ; Base (low).
    db 0                     ; Base (middle)
    db 11110010b             ; Access (read/write).
    db 00000000b             ; Granularity.
    db 0                     ; Base (high).
;.tssData: equ $ - EARLY_GDT64     ; The data descriptor.
;    dw 0                    ; Limit (low).
;    dw 0                    ; Base (low).
;    db 0                    ; Base (middle)
;    db 0x89                 ; Access (read/write).
;    db 0x10                 ; Granularity.
;    db 0                    ; Base (high).
.Pointer:               ; The GDT-pointer.
    dw $ - EARLY_GDT64 - 1    ; Limit.
    dq EARLY_GDT64            ; Base.

align 8

;; IDT

; Usadas nas entradas da idt.

sys_interrupt equ    0x8E 
sys_code      equ    8     ;Code selector.

;=====================================;
; IDT.                                ;
; Interrupt vectors for intel x86_64  ;
;=====================================;
; IDT in IA-32e Mode (64-bit IDT)
; See:
; https://wiki.osdev.org/Interrupt_Descriptor_Table
; Nesse momento criamos apenas o esqueleto da tabela,
; Uma rotina vai ser chamada para preencher o que falta.

global _idt
_idt:

;0 interrupt 0h, div error.
    dw 0              ; Offset low bits (0..15)
    dw sys_code       ; Selector (Code segment selector)
    db 0              ; Zero
    db sys_interrupt  ; Type and Attributes (same as before)
    dw 0              ; Offset middle bits (16..31)
    dd 0              ; Offset high bits (32..63)
    dd 0              ; Zero

;1 interrupt 1h, debug exception.
    dw 0 
    dw sys_code
    db 0
    db sys_interrupt
    dw 0
    dd 0
    dd 0

;2 interrupt 2h, non maskable interrupt.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;3 interrupt 3h, int3 trap.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;4 interrupt 4h, into trap.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;5 interrupt 5h,  bound trap.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;6 interrupt 6h, invalid instruction.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;7 interrupt 7h, no coprocessor.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;8 interrupt 8h, double fault.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;9 interrupt 9h, coprocessor segment overrun 1.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;10 interrupt Ah, invalid tss.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;11 interrupt Bh, segment not present.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;12 interrupt Ch, stack fault.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;13 interrupt Dh, general protection fault.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;14 interrupt Eh, page fault.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;15 interrupt Fh, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;16 interrupt 10h, coprocessor error.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;17 interrupt 11h, alignment check.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;18 interrupt 12h, machine check. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0
	
	;;
	;; ## Intel reserveds ##
	;;

;19 interrupt 13h, reserved.
	dw 0		   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;20 interrupt 14h, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;21 interrupt 15h, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;22 interrupt 16h, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;23 interrupt 17h, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;24 interrupt 18h, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;25 interrupt 19h, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;26 interrupt 1Ah, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;27 interrupt 1Bh, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;28 interrupt 1Ch, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;29 interrupt 1Dh, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;30 interrupt 1Eh, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;31 interrupt 1Fh, reserved.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

  ;; 
  ;;  ##  IRQs ##
  ;;

;32 interrupt 20h, IRQ0, TIMER.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;33 interrupt 21h, IRQ1, TECLADO.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;34 interrupt 22h, IRQ2.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;35 interrupt 23h, IRQ3.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;36 interrupt 24h, IRQ4.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;37 interrupt 25h, IRQ5.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;38 interrupt 26h, IRQ6.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;39 interrupt 27h, IRQ7.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;40 interrupt 28h, IRQ8. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;41 interrupt 29h, IRQ9. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;42 interrupt 2Ah, IRQ10. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;43 interrupt 2Bh, IRQ11. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;44 interrupt 2Ch, IRQ12. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;45 interrupt 2Dh, IRQ13. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;46 interrupt 2Eh, IRQ 14. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;47 interrupt 2Fh, IRQ15. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;48 interrupt 30h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;49 interrupt 31h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;50 interrupt 32h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;51 interrupt 33h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;52 interrupt 34h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;53 interrupt 35h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;54 interrupt 36h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;55 interrupt 37h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;56 interrupt 38h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;57 interrupt 39h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;58 interrupt 3Ah.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;59 interrupt 3Bh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;60 interrupt 3Ch.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;61 interrupt 3Dh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;62 interrupt 3Eh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;63 interrupt 3Fh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;64 interrupt 40h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;65 interrupt 41h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;66 interrupt 42h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;67 interrupt 43h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;68 interrupt 44h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;69 interrupt 45h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;70 interrupt 46h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;71 interrupt 47h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;72 interrupt 48h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;73 interrupt 49h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;74 interrupt 4Ah.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;75 interrupt 4Bh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;76 interrupt 4Ch.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;77 interrupt 4Dh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;78 interrupt 4Eh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;79 interrupt 4Fh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;80 interrupt 50h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;81 interrupt 51h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;82 interrupt 52h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;83 interrupt 53h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;84 interrupt 54h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;85 interrupt 55h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;86 interrupt 56h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;87 interrupt 57h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;88 interrupt 58h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;89 interrupt 59h
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;90 interrupt 5Ah.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;91 interrupt 5Bh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;92 interrupt 5Ch.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;93 interrupt 5Dh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;94 interrupt 5Eh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;95 interrupt 5Fh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;96 interrupt 60h. 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;97 interrupt 61h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;98 interrupt 62h.  
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;99 interrupt 63h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;100 interrupt 64h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;101 interrupt 65h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;102 interrupt 66h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;103 interrupt 67h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;104 interrupt 68h
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;105 interrupt 69h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;106 interrupt 6Ah.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;107 interrupt 6Bh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;108 interrupt 6Ch.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;109 interrupt 6Dh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;110 interrupt 6Eh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;111 interrupt 6Fh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;112 interrupt 70h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;113 interrupt 71h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;114 interrupt 72h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;115 interrupt 73h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;116 interrupt 74h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;117 interrupt 75h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;118 interrupt 76h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;119 interrupt 77h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;120 interrupt 78h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;121 interrupt 79h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;122 interrupt 7Ah.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;123 interrupt 7Bh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;124 interrupt 7Ch.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;125 interrupt 7Dh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;126 interrupt 7Eh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;127 interrupt 7Fh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

	;;
	;;  ## system call ##
	;;
	
	;;#importante
	;;Essa agora vai ser a system call.
	;;Ela � UNIX-like e usada em muitos sistemas.

;128 interrupt 80h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;129 interrupt 81h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;130 interrupt 82h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;131 interrupt 83h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;132 interrupt 84h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;133 interrupt 85h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;134 interrupt 86h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;135 interrupt 87h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;136 interrupt 88h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;137 interrupt 89h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;138 interrupt 8Ah.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;139 interrupt 8Bh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;140 interrupt 8Ch.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;141 interrupt 8Dh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;142 interrupt 8Eh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;143 interrupt 8Fh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;144 interrupt 90h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;145 interrupt 91h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;146 interrupt 92h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;147 interrupt 93h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;148 interrupt 94h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;149 interrupt 95h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;150 interrupt 96h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;151 interrupt 97h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;152 interrupt 98h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;153 interrupt 99h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;154 interrupt 9Ah.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;155 interrupt 9Bh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;156 interrupt 9Ch.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;157 interrupt 9Dh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;158 interrupt 9Eh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;159 interrupt 9Fh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;160 interrupt A0h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;161 interrupt A1h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;162 interrupt A2h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;163 interrupt A3h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;164 interrupt A4h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;165 interrupt A5h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;166 interrupt A6h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;167 interrupt A7h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;168 interrupt A8h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;169 interrupt A9h.
	dw 0    
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;170 interrupt AAh.
	dw 0 	   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;171 interrupt ABh.
	dw 0 	   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;172 interrupt ACh.
	dw 0 	   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;173 interrupt ADh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;174 interrupt AEh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;175 interrupt AFh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;176 interrupt B0h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;177 interrupt B1h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;178 interrupt B2h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;179 interrupt B3h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;180 interrupt B4h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;181 interrupt B5h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;182 interrupt B6h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;183 interrupt B7h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;184 interrupt B8h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;185 interrupt B9h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;186 interrupt BAh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;187 interrupt BBh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;188 interrupt BCh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;189 interrupt BDh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;190 interrupt BEh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;191 interrupt BFh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;192 interrupt C0h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;193 interrupt C1h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;194 interrupt C2h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;195 interrupt C3h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;196 interrupt C4h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;197 interrupt C5h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;198 interrupt C6h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;199 interrupt C7h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

  ;; #obs
  ;; Essa não é mais a interrupção do sistema.
  ;; Agora é a tradicional 128 (0x80)   

;200 interrupt C8h, 
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;201 interrupt C9h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;202 interrupt CAh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;203 interrupt CBh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;204 interrupt CCh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;205 interrupt CDh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;206 interrupt CEh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;207 interrupt CFh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;208 interrupt D0h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;209 interrupt D1h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;210 interrupt D2h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;211 interrupt D3h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;212 interrupt D4h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;213 interrupt D5h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;214 interrupt D6H.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;215 interrupt D7h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;216 interrupt D8h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;217 interrupt D9h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;218 interrupt DAh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;219 interrupt DBh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;220 interrupt DCh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;221 interrupt DDh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;222 interrupt DEh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;223 interrupt DFh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;224 interrupt E0h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;225 interrupt E1h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;226 interrupt E2h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;227 interrupt E3h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;228 interrupt E4h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;229 interrupt E5h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;230 interrupt E6h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;231 interrupt E7h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;232 interrupt E8h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;233 interrupt E9h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;234 interrupt EAh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;235 interrupt EBh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;236 interrupt ECh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;237 interrupt EDh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;238 interrupt EEh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;239 interrupt EFh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;240 interrupt F0h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;241 interrupt F1h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;242 interrupt F2h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;243 interrupt F3h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;244 interrupt F4h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;245 interrupt F5h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;246 interrupt F6h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;247 interrupt F7h.
	dw 0 	   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;248 interrupt F8h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;249 interrupt F9h.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;250 interrupt FAh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;251 interrupt FBh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;252 interrupt FCh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;253 interrupt FDh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;254 interrupt FEh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

;255 interrupt FFh.
	dw 0
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
	dd 0
	dd 0

idt_end:
    dq 0

;
; IDT_register
;

global _IDT_register
_IDT_register:

    ;dw  (256*16) - (1)
    dw idt_end - _idt - 1  
    dq _idt 


align 8


; Includes:
; ========
; Esses includes são padronizados. Não acrescentar outros.

;---------------------
; Inicialização.
; Funções de apoio à inicialização do Kernel.
    %include "header/header3.asm"
    %include "header/header4.asm"
;---------------------
; Interrupções de hardware (irqs) e faults.
    %include "entry/hw/hw1.asm"
    %include "entry/hw/hw2.asm"
;---------------------
; visitor
; Interrupções de software.
    %include "entry/sw/sw1.asm"
    %include "entry/sw/sw2.asm"


;===================================================
; DATA: 
;     Início do Segmento de dados.
;     Coloca uma assinatura no todo.

segment .data
global _data_start
_data_start:
    db 0x55
    db 0xAA


;=================================================================
; BSS:
;     Início do segmento BSS.
;     Coloca uma assinatura no todo.
;     Mas normalmente limpamos essa área.

segment .bss
global _bss_start
_bss_start:
    ;db 0x55
    ;db 0xAA


; Stack usada pelo kernel
; 64 KB.
global _EarlyKernelStackEnd
_EarlyKernelStackEnd:
    resb (64*1024)
global _EarlyKernelStack
_EarlyKernelStack:


; Used by the tss in unit0lib.asm.
; 64 KB
global _rsp0StackEnd
_rsp0StackEnd:
    resb (64*1024)
global _rsp0Stack
_rsp0Stack:




