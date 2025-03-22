; hw1.asm
; This file handles the traps for the x86_64 processors.
; Only hw interrupts.

;
; Imports
;

;;
;; == Context =================
;;

; See:
; x64cont.c

extern _contextSS        ; User Mode.
extern _contextRSP       ; User Mode.
extern _contextRFLAGS    ; User Mode.
extern _contextCS        ; User Mode.
extern _contextRIP       ; User Mode.

extern _contextDS
extern _contextES
extern _contextFS  ;?
extern _contextGS  ;?

extern _contextRAX
extern _contextRBX
extern _contextRCX
extern _contextRDX

extern _contextRSI
extern _contextRDI

extern _contextRBP

extern _contextR8
extern _contextR9
extern _contextR10
extern _contextR11
extern _contextR12
extern _contextR13
extern _contextR14
extern _contextR15

extern _contextCPL

; Array.
; Defined in x64cont.c
extern _context_fpu_buffer

; ...



align 16
__hw_fpu_buffer:
    times 512 db 0
align 16

;================================
global _DisableSSE
_DisableSSE:
    push rax
    mov rax, cr0
    or ax, 1 << 3    ;Set TS
    mov cr0, rax
    pop rax
    ret

;================================
global _EnableSSE
_EnableSSE:
    push rax
    mov rax, cr0
    and ax, ~(1 << 3)    ; Clear TS
    mov cr0, rax
    pop rax
    ret

;================================
; Advanced Vector Extensions is a SIMD (Single Instruction, Multiple Data) 
; instruction set introduced by Intel in 2011.
; AVX is enabled by setting bit 2 of the XCR0 register. 
; Bit 1 of XCR0 must also be set (indicating SSE support).
; See: 
; https://wiki.osdev.org/SSE
; https://en.wikipedia.org/wiki/AVX-512
global _x64_enable_avx
_x64_enable_avx:

    push rax
    push rcx
    push rdx

    xor rcx, rcx
    xgetbv     ; Load XCR0 register
    or eax, 7  ; Set AVX, SSE, X87 bits
    xsetbv     ; Save back to XCR0

    pop rdx
    pop rcx
    pop rax
    ret


; #todo: Move to unit 2
; ---------------------------------------------------------------------
; reboot -- Reboot the computer
hw_reboot:
	in al, 0x64
	test al, 00000010b		; Wait for an empty Input Buffer
	jne hw_reboot
	mov al, 0xFE
	out 0x64, al			; Send the reboot call to the keyboard controller
	jmp hw_reboot
; ----------------------------------------------------------------------


;;=====================================================
;;  ## TIMER ##
;;=====================================================
;; _irq0: 
;; IRQ 0. 
;; Timer interrupt handler
;; See:
;; 1pump/arch/x86_64/pit.c
;; 0mem/core/ps/disp/ts.c
;;


;================================
extern _irq0_TIMER
; Capture context
align 4  
global _irq0
_irq0:
; Maskable interrupt

    ; #ps:
    ; Actually we dont need this in hw interrupts,
    ; because all the IDT entries are using 8E00,
    ; present, dpl=0, interrupt gate, where the interrupts
    ; are disabled by default.

    cli

; No caso do dispatcher lançar uma nova thread,
; então ele deve acionar enviar um EIO.

    ; mov dword [_irq0PendingEOI], 1

;; == Save context ====================
    
    ; Stack frame. (all double)
    pop qword [_contextRIP]     ; rip
    pop qword [_contextCS]      ; cs  (R3)
    pop qword [_contextRFLAGS]  ; rflags
    pop qword [_contextRSP]     ; rsp
    pop qword [_contextSS]      ; ss

    mov qword [_contextRDX], rdx 
    mov qword [_contextRCX], rcx 
    mov qword [_contextRBX], rbx 
    mov qword [_contextRAX], rax
     
    mov qword [_contextRBP], rbp
 
    mov qword [_contextRDI], rdi 
    mov qword [_contextRSI], rsi 

    mov qword [_contextR8], r8
    mov qword [_contextR9], r9
    mov qword [_contextR10], r10
    mov qword [_contextR11], r11
    mov qword [_contextR12], r12
    mov qword [_contextR13], r13
    mov qword [_contextR14], r14
    mov qword [_contextR15], r15

; Data segments
    xor rax, rax
    mov ax, gs
    mov word [_contextGS], ax
    mov ax, fs
    mov word [_contextFS], ax
    mov ax, es
    mov word [_contextES], ax
    mov ax, ds
    mov word [_contextDS], ax

; FPU
; See:
; https://wiki.osdev.org/SSE
    fxsave [_context_fpu_buffer]

; #todo
; Media, float pointers, debug.
; #important:
; We are using the kernel segment registers.
; Kernel data segments and stack.
; #bugbug: sempre a mesma pilha?
; Que pilha as interrupçoes de softwar estao usando?

;
; Calls
;

; cpl
; Get the first 2 bits of cs.
; see: x64cont.c
    mov rax, qword [_contextCS]  ;(R3)
    and rax, 3
    mov [_contextCPL], rax

; Timer and taskswitching.
; See: pit.c

    call _irq0_TIMER

; FPU
    fxrstor [_context_fpu_buffer]

; Release a bandit.
; see: hw2.asm

    jmp irq0_release
; --------------------------------------

;========================================
; _irq1:
;     IRQ 1 - Keyboard.
; See:
; keyboard.c
;
extern _xxxxIRQ1_DEBUG_MESSAGE
extern _irq1_KEYBOARD
; Capture context
align 4  
global _irq1  
_irq1:
; Maskable interrupt

    cli

; No caso do dispatcher lançar uma nova thread,
; então ele deve acionar enviar um EIO.

    ; mov dword [_irq0PendingEOI], 1

;; == Save context ====================
    
    ; Stack frame. (all double)
    pop qword [_contextRIP]     ; rip
    pop qword [_contextCS]      ; cs (R3)
    pop qword [_contextRFLAGS]  ; rflags
    pop qword [_contextRSP]     ; rsp
    pop qword [_contextSS]      ; ss

    mov qword [_contextRDX], rdx 
    mov qword [_contextRCX], rcx 
    mov qword [_contextRBX], rbx 
    mov qword [_contextRAX], rax
     
    mov qword [_contextRBP], rbp
 
    mov qword [_contextRDI], rdi 
    mov qword [_contextRSI], rsi 

    mov qword [_contextR8], r8
    mov qword [_contextR9], r9
    mov qword [_contextR10], r10
    mov qword [_contextR11], r11
    mov qword [_contextR12], r12
    mov qword [_contextR13], r13
    mov qword [_contextR14], r14
    mov qword [_contextR15], r15


; Data segments
    xor rax, rax
    mov ax, gs
    mov word [_contextGS], ax
    mov ax, fs
    mov word [_contextFS], ax
    mov ax, es
    mov word [_contextES], ax
    mov ax, ds
    mov word [_contextDS], ax

; FPU
; See:
; https://wiki.osdev.org/SSE
    fxsave [_context_fpu_buffer]

; #todo
; Media, float pointers, debug.
; #important:
; We are using the kernel segment registers.
; Kernel data segments and stack.
; #bugbug: sempre a mesma pilha?
; Que pilha as interrupçoes de softwar estao usando?

;
; Calls
;

; cpl
; Get the first 2 bits of cs.
; see: x64cont.c
    mov rax, qword [_contextCS]  ;(R3)
    and rax, 3
    mov [_contextCPL], rax


; See: 
; keyboard.c
    call _irq1_KEYBOARD


; FPU
    fxrstor [_context_fpu_buffer]

; Release a bandit.

; 64bit
; This is a 64bit pointer to the pml4 table.

; #bugbug
; Não precisamos fazer refresh em todo tick,
; somente quando houver troca de tarefa.

    mov RAX, CR3  
    IODELAY 
    mov CR3, RAX  

; Wait TLB.
    ;IODELAY 
;
; == Restore context ====================
;

    ; Segments
    xor rax, rax
    mov ax, word [_contextDS]
    mov ds, ax
    mov ax, word [_contextES]
    mov es, ax
    mov ax, word [_contextFS]
    mov fs, ax
    mov ax, word [_contextGS]
    mov gs, ax

    mov r15, qword [_contextR15]
    mov r14, qword [_contextR14]
    mov r13, qword [_contextR13]
    mov r12, qword [_contextR12]
    mov r11, qword [_contextR11]
    mov r10, qword [_contextR10]
    mov r9,  qword [_contextR9]
    mov r8,  qword [_contextR8]

    mov rsi, qword [_contextRSI] 
    mov rdi, qword [_contextRDI] 
    
    mov rbp, qword [_contextRBP] 
    
    mov rax, qword [_contextRAX] 
    mov rbx, qword [_contextRBX] 
    mov rcx, qword [_contextRCX] 
    mov rdx, qword [_contextRDX] 

; Stack frame. (all double)
    push qword [_contextSS]      ; ss
    push qword [_contextRSP]     ; rsp
    push qword [_contextRFLAGS]  ; rflags
    push qword [_contextCS]      ; cs
    push qword [_contextRIP]     ; rip

    ; send EOI to XT keyboard
    ;in      al, 061h
    ;mov     ah, al
    ;or      al, 080h
    ;out     061h, al
    ;mov     al, ah
    ;out     061h, al

; EOI - Only the first PIC.
    xor rax, rax 
    MOV AL, 020h
    OUT 020h, AL
    ;IODELAY  

    ;; variável usada pelo dispatcher.
    ;mov dword [_irq0PendingEOI], 0

    ; Acumulator
    mov rax, qword [_contextRAX]

; #bugbug
; We do NOT need the 'sti'. 
; The flags in the 'eflags' will reenable it.

    sti
    iretq
;;===========================

;=======================================================
; IRQ 3 - serial port controller for serial port 2 
; (shared with serial port 4, if present)
; See: serial.c
;extern _serial2_handler
;extern _serial4_handler
; Capture context
align 4  
global _irq3
_irq3:
; Maskable interrupt

    ; #todo
    ; Work on this routine to make it kinda like the irq0.

    cli
    
    ;pushad
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs

    ;call _serial2_handler
    ;call _serial4_handler

; EOI - Only the first PIC.
    mov al, 0x20
    out 0x20, al
    ;IODELAY  
    
    ;popad
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq
    
;; ==========================================


;====================================================
; IRQ 4 - serial port controller for serial port 1 
;(shared with serial port 3, if present)
; See: serial.c
;extern _serial1_handler
;extern _serial3_handler
; Capture context
align 4  
global _irq4
_irq4:
; Maskable interrupt

    ; #todo
    ; Work on this routine to make it kinda like the irq0.

    cli
    ;pushad
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs

    ;call _serial1_handler
    ;call _serial3_handler

; EOI - Only the first PIC.
    mov al, 0x20
    out 0x20, al
    ;IODELAY  
    
    ;popad
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq
;================================

;--------------
; IRQ 5 - parallel port 2 and 3  or  sound card
;_irq5:
;    iretd

;--------------
; IRQ 6  floppy disk controller
;_irq6:
;    iretd

;===================================================
;IRQ 7 parallel port 1. 
; It is used for printers 
; or for any parallel port 
; if a printer is not present. It can also be potentially 
; be shared with a secondary sound card with careful 
; management of the port.
;; The correct way to handle an IRQ 7 is to first check the master PIC 
;; chip's ISR to see if the IRQ is a spurious IRQ or a real IRQ. 
;; If it is a real IRQ then it is treated the same as any other real 
;; IRQ. If it is a spurious IRQ then you ignore it 
;; (and do not send the EOI). 
; Capture context
align 4  
global _irq7
_irq7:
; Maskable interrupt

    ; #todo
    ; Work on this routine to make it kinda like the irq0.

    cli
    ;pushad
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs

; ++
; ================================================
; #test
; Testing the spurious interrupt management.

    xor rax, rax
    mov  al, 03h                ; PIC.OCW3 set function to read ISR (In Service Register)
    out  23h, al                ; write to PIC.OCW3 master
    in   al, 20h                ; read ISR master.
    test al, 80h                ; if the in-service register does not have IR7 bit set
    jz short ExitParallelPort_WithoutEOI   ; this would be a spurious interrupt.

; ================================================
; --

;; Not spurious
;; So this is a norma parallel por interrupt.

    ;call _first_parallel_port_Handler

; EOI. Master. for parallel port
ExitParallelPort_WithEOI:

    ;xor rax, rax
    mov al, 0x20
    out 0x20, al   
    ;IODELAY  

; No eoi for irq 7 spurious
ExitParallelPort_WithoutEOI:
  
    ;popad
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq

;================================================
; _irq8:
;     System CMOS, Realtime clock.
;     IRQ 8 - real-time clock (RTC)
;
; Capture context
align 4  
global _irq8
_irq8:
; Maskable interrupt

    ; #todo
    ; Work on this routine to make it kinda like the irq0.

    cli
    ;pushad
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs

    ;call _irq8_RTC

; EOI
; Order: Second, first.
    mov al, 0x20
    out 0xA0, al  
    IODELAY  
    IODELAY
    out 0x20, al
    ;IODELAY  

    ;popad
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    sti
    iretq
;;============================

;=========================================================
; IRQ 9
;extern _xxxe1000handler
; Capture context
align 4  
global _irq9
_irq9:
; Maskable interrupt

    ;; #test
    jmp unhandled_irq
    jmp $

    cli
    ;pushad
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs



    ;jmp _asm_reboot
    ;call _xxxe1000handler

    ;call _acpiHandler

    ;; EOI.
    ;; Order: Second, first.
    mov al, 0x20
    out 0xA0, al  
    IODELAY  
    out 0x20, al
    IODELAY  
    
    ;popad
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq


;;==========================================
; _irq9_nic_handler:
; e1000 Intel nic handler.
; This is a work in progress!
; interrupção ?. irq ?;
; Temporary: interrupção 41. irq 9
; #bugbug: Remember, each nic can have a different interrupt 
; number, this handler was hardcoded just for irq9 in header3.asm 
; valid just for virtualbox. Maybe qemu uses another number.
; See _asm_nic_create_new_idt_entry() in header3.asm
; see: e1000.c
extern _irq_E1000
align 4  
global _irq9_nic_handler
_irq9_nic_handler:
; Maskable interrupt

    cli

; No caso do dispatcher lançar uma nova thread,
; então ele deve acionar enviar um EIO.

    ; mov dword [_irq0PendingEOI], 1

;; == Save context ====================
    
    ; Stack frame. (all double)
    pop qword [_contextRIP]     ; rip
    pop qword [_contextCS]      ; cs  (R3)
    pop qword [_contextRFLAGS]  ; rflags
    pop qword [_contextRSP]     ; rsp
    pop qword [_contextSS]      ; ss

    mov qword [_contextRDX], rdx 
    mov qword [_contextRCX], rcx 
    mov qword [_contextRBX], rbx 
    mov qword [_contextRAX], rax
     
    mov qword [_contextRBP], rbp
 
    mov qword [_contextRDI], rdi 
    mov qword [_contextRSI], rsi 

    mov qword [_contextR8], r8
    mov qword [_contextR9], r9
    mov qword [_contextR10], r10
    mov qword [_contextR11], r11
    mov qword [_contextR12], r12
    mov qword [_contextR13], r13
    mov qword [_contextR14], r14
    mov qword [_contextR15], r15

; Data segments
; gs,fs,es,ds
    xor rax, rax
    mov ax, gs
    mov word [_contextGS], ax
    mov ax, fs
    mov word [_contextFS], ax
    mov ax, es
    mov word [_contextES], ax
    mov ax, ds
    mov word [_contextDS], ax

; FPU
; See:
; https://wiki.osdev.org/SSE
    fxsave [_context_fpu_buffer]

; #todo
; Media, float pointers, debug.
; #important:
; We are using the kernel segment registers.
; Kernel data segments and stack.
; #bugbug: sempre a mesma pilha?
; Que pilha as interrupçoes de softwar estao usando?

;
; Calls
;

; cpl
; Get the first 2 bits of cs.
; see: x64cont.c
    mov rax, qword [_contextCS]  ;(R3)
    and rax, 3
    mov [_contextCPL], rax

; Timer and taskswitching.
; See: pit.c

    call _irq_E1000

; FPU
    fxrstor [_context_fpu_buffer]

; Release a bandit.

; 64bit
; This is a 64bit pointer to the pml4 table.

; #bugbug
; Não precisamos fazer refresh em todo tick,
; somente quando houver troca de tarefa.

    mov RAX, CR3  
    IODELAY 
    mov CR3, RAX  

; Wait TLB.
    ;IODELAY 
;
; == Restore context ====================
;

    ; Segments
    xor rax, rax
    mov ax, word [_contextDS]
    mov ds, ax
    mov ax, word [_contextES]
    mov es, ax
    mov ax, word [_contextFS]
    mov fs, ax
    mov ax, word [_contextGS]
    mov gs, ax

    mov r15, qword [_contextR15]
    mov r14, qword [_contextR14]
    mov r13, qword [_contextR13]
    mov r12, qword [_contextR12]
    mov r11, qword [_contextR11]
    mov r10, qword [_contextR10]
    mov r9,  qword [_contextR9]
    mov r8,  qword [_contextR8]

    mov rsi, qword [_contextRSI] 
    mov rdi, qword [_contextRDI] 
    
    mov rbp, qword [_contextRBP] 
    
    mov rax, qword [_contextRAX] 
    mov rbx, qword [_contextRBX] 
    mov rcx, qword [_contextRCX] 
    mov rdx, qword [_contextRDX] 

; Stack frame. (all double)
    push qword [_contextSS]      ; ss
    push qword [_contextRSP]     ; rsp
    push qword [_contextRFLAGS]  ; rflags
    push qword [_contextCS]      ; cs
    push qword [_contextRIP]     ; rip

; EOI: Order: Second, first.
    mov al, 0x20
    out 0xA0, al
    IODELAY
    IODELAY
    out 0x20, al
    ;IODELAY

    ;; variável usada pelo dispatcher.
    ;mov dword [_irq0PendingEOI], 0

    ; Acumulator.
    mov rax, qword [_contextRAX]

; #bugbug
; We do NOT need the 'sti'. 
; The flags in the 'eflags' will reenable it.

    sti
    iretq
;===========================

;========================================================
; IRQ 10  The Interrupt is left open for the use 
; of peripherals (open interrupt/available, SCSI or NIC)
; nvidia
; Capture context
align 4  
global _irq10
_irq10:
; Maskable interrupt

    ;; #test
    jmp unhandled_irq
    jmp $

    cli
    ;pushad
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs


    ;call _PciHandler1

    ;; EOI.
    ;; Order: Second, first.
    mov al, 0x20
    out 0xA0, al  
    IODELAY  
    out 0x20, al
    IODELAY  
    
    ;popad
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq

;;==========================================


;=======================================
; IRQ 11 - The Interrupt is left open for 
; the use of peripherals (open interrupt/available, SCSI or NIC)
; audio.
; Capture context
align 4  
global _irq11
_irq11:
; Maskable interrupt

    ;; #test
    jmp unhandled_irq
    jmp $

    cli
    ;pushad
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs


    ;call _PciHandler2

    ;; EOI.
    ;; Order: Second, first.
    mov al, 0x20
    out 0xA0, al  
    IODELAY  
    out 0x20, al
    IODELAY  
    
    ;popad
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq
;===================

;=======================================
; IRQ 12 - mouse on PS/2 connector
; See: /i8042/mouse.c
extern _irq12_MOUSE
; Capture context
align 4  
global _irq12
_irq12:
; Maskable interrupt

    cli

; No caso do dispatcher lançar uma nova thread,
; então ele deve acionar enviar um EIO.

    ; mov dword [_irq0PendingEOI], 1

;; == Save context ====================
    
    ; Stack frame. (all double)
    pop qword [_contextRIP]     ; rip
    pop qword [_contextCS]      ; cs (R3)
    pop qword [_contextRFLAGS]  ; rflags
    pop qword [_contextRSP]     ; rsp
    pop qword [_contextSS]      ; ss

    mov qword [_contextRDX], rdx 
    mov qword [_contextRCX], rcx 
    mov qword [_contextRBX], rbx 
    mov qword [_contextRAX], rax
     
    mov qword [_contextRBP], rbp
 
    mov qword [_contextRDI], rdi 
    mov qword [_contextRSI], rsi 

    mov qword [_contextR8], r8
    mov qword [_contextR9], r9
    mov qword [_contextR10], r10
    mov qword [_contextR11], r11
    mov qword [_contextR12], r12
    mov qword [_contextR13], r13
    mov qword [_contextR14], r14
    mov qword [_contextR15], r15

; Data segments
; gs,fs,es,ds
    xor rax, rax
    mov ax, gs
    mov word [_contextGS], ax
    mov ax, fs
    mov word [_contextFS], ax
    mov ax, es
    mov word [_contextES], ax
    mov ax, ds
    mov word [_contextDS], ax

; FPU
; See:
; https://wiki.osdev.org/SSE
    fxsave [_context_fpu_buffer]

; #todo
; Media, float pointers, debug.
; #important:
; We are using the kernel segment registers.
; Kernel data segments and stack.
; #bugbug: sempre a mesma pilha?
; Que pilha as interrupçoes de softwar estao usando?

;
; Calls
;

; cpl
; Get the first 2 bits of cs.
; see: x64cont.c
    mov rax, qword [_contextCS]  ;(R3)
    and rax, 3
    mov [_contextCPL], rax


; See: 
; keyboard.c
    call _irq12_MOUSE


; FPU
    fxrstor [_context_fpu_buffer]

; Release a bandit.

; 64bit
; This is a 64bit pointer to the pml4 table.

; #bugbug
; Não precisamos fazer refresh em todo tick,
; somente quando houver troca de tarefa.

    mov RAX, CR3  
    IODELAY 
    mov CR3, RAX  

; Wait TLB.
    ;IODELAY 
;
; == Restore context ====================
;

    ; Segments
    xor rax, rax
    mov ax, word [_contextDS]
    mov ds, ax
    mov ax, word [_contextES]
    mov es, ax
    mov ax, word [_contextFS]
    mov fs, ax
    mov ax, word [_contextGS]
    mov gs, ax

    mov r15, qword [_contextR15]
    mov r14, qword [_contextR14]
    mov r13, qword [_contextR13]
    mov r12, qword [_contextR12]
    mov r11, qword [_contextR11]
    mov r10, qword [_contextR10]
    mov r9,  qword [_contextR9]
    mov r8,  qword [_contextR8]

    mov rsi, qword [_contextRSI] 
    mov rdi, qword [_contextRDI] 
    
    mov rbp, qword [_contextRBP] 
    
    mov rax, qword [_contextRAX] 
    mov rbx, qword [_contextRBX] 
    mov rcx, qword [_contextRCX] 
    mov rdx, qword [_contextRDX] 

; Stack frame. (all double)
    push qword [_contextSS]      ; ss
    push qword [_contextRSP]     ; rsp
    push qword [_contextRFLAGS]  ; rflags
    push qword [_contextCS]      ; cs
    push qword [_contextRIP]     ; rip

    ; send EOI to XT keyboard
    ;in      al, 061h
    ;mov     ah, al
    ;or      al, 080h
    ;out     061h, al
    ;mov     al, ah
    ;out     061h, al

;EOI order: Second, first.
    mov al, 0x20
    out 0xA0, al 
    IODELAY 
    IODELAY 
    out 0x20, al
    ;IODELAY 

    ;; variável usada pelo dispatcher.
    ;mov dword [_irq0PendingEOI], 0

    ; Acumulator
    mov rax, qword [_contextRAX]

; #bugbug
; We do NOT need the 'sti'. 
; The flags in the 'eflags' will reenable it.

    sti
    iretq
;;=================


;;============================================================	
; IRQ 13 
; CPU co-processor  or  integrated floating point unit  
; or  inter-processor interrupt (use depends on OS)
; Capture context
align 4  
global _irq13
_irq13:
; Maskable interrupt

    ;; #test
    jmp unhandled_irq
    jmp $

    cli

    ;push ax 
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs

    ;call _coprocessorHandler

    ;; EOI.
    ;; Order: Second, first.
    mov al, 0x20
    out 0xA0, al  
    IODELAY  
    out 0x20, al
    IODELAY  
    
    ;pop ax
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq

;==========================================

;============================================
; _irq14:
;     Tratador de interrupções para unidade master.
;     IRQ 14 - primary ATA channel 
;     ( ATA interface usually serves hard disk drives and CD drives ) 
;     O timer precisa ser desbilitado. ??
extern _irq14_PRIMARY_IDE
; Capture context
align 4  
global _irq14
_irq14:
; Maskable interrupt

    ; #todo
    ; Work on this routine to make it kinda like the irq0.

    cli 
    
    ;push eax
    ;PUSHAD
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs

    fxsave [__hw_fpu_buffer]

    call _irq14_PRIMARY_IDE

    fxrstor [__hw_fpu_buffer]

; EOI
; Order: Second, first.
    MOV AL,020h
    OUT 0A0h,AL
    IODELAY
    IODELAY  
    OUT 020h,AL
    ;IODELAY
    ;IODELAY  
    
    ;POPAD
    ;pop eax
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq 

;=================================================	
; _irq15:
;     Tratador de interrupções para unidade slave.
;     IRQ 15 - secondary ATA channel
;     O timer precisa ser desbilitado. ??
;; The correct way to handle an IRQ 15 is similar, but a little trickier 
;; due to the interaction between the slave PIC and the master PIC. 
;; First check the slave PIC chip's ISR to see if the IRQ is a spurious 
;; IRQ or a real IRQ. If it is a real IRQ then it is treated the same 
;; as any other real IRQ. If it's a spurious IRQ then don't send the 
;; EOI to the slave PIC; however you will still need to send the EOI 
;; to the master PIC because the master PIC itself won't know that it 
;; was a spurious IRQ from the slave. 
extern _irq15_SECONDARY_IDE
; Capture context
align 4  
global _irq15
_irq15:
; Maskable interrupt

    ; #todo
    ; Work on this routine to make it kinda like the irq0.

    cli
    
    ;push eax
    ;PUSHAD
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ;push ds
    ;push es
    push fs
    push gs

    fxsave [__hw_fpu_buffer]

    call _irq15_SECONDARY_IDE

    fxrstor [__hw_fpu_buffer]

; #bugbug
; #todo
; Spurious int for irq15.
; It's different from irq7

; EOI
; Order: Second, first.
    MOV AL, 020h
    OUT 0A0h, AL
    IODELAY
    IODELAY  
    OUT 020h, AL
    ;IODELAY
    ;IODELAY  

    ;POPAD
    ;pop eax
    pop gs
    pop fs
    ;pop es
    ;pop ds
    
    ;#bugbug
    ;#todo: Pop the registers.
    ;popad
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti
    iretq 
; =======================

;========================================
; unhandled_irq:
;     Interrupção de hardware genérica. 
;++ 

; #bugbug
; Não podemos efetuar EOI para todos 
; somente para as irqs.

extern _faults

unhandled_irq:
    cli
    push rax

; #bugbug
; We can't use EOI for every interrupt,
; just for some of them.

    mov al, 0x20
    out 0xA0, al
    IODELAY  
    IODELAY  

    out 0x20, al
    ;IODELAY
    ;IODELAY  

    pop rax
    sti 
    iretq
;--

;----------------------------
; Building trampolines for the faults.

align 8

; int 0
; Se ocorrer em ring 0 o sistema tem que terminar,
; e se for em ring3, fechamos o aplicativo.
global _fault_N0
_fault_N0:
    mov qword [_save_fault_number], qword 0
    jmp all_faults

; int 1 
global _fault_N1
_fault_N1:
    mov qword [_save_fault_number], qword 1
    jmp all_faults

; int 2 
global _fault_N2
_fault_N2:
    mov qword [_save_fault_number], qword 2
    jmp all_faults

; int 3 
global _fault_N3
_fault_N3:
    mov qword [_save_fault_number], qword 3
    jmp all_faults

; int 4 
global _fault_N4
_fault_N4:
    mov qword [_save_fault_number], qword 4
    jmp all_faults

; int 5 
global _fault_N5
_fault_N5:
     mov qword [_save_fault_number], qword 5
    jmp all_faults

; int 6: Invalid opcode
global _fault_INVALID_OPCODE
_fault_INVALID_OPCODE:
    mov qword [_save_fault_number], qword 6
    jmp all_faults

; int 7
global _fault_N7
_fault_N7:
    mov qword [_save_fault_number], qword 7
    jmp all_faults

; int 8 - double fault
global _fault_DOUBLE
_fault_DOUBLE:
    mov qword [_save_fault_number], qword 8
    jmp all_faults

; int 9 
global _fault_N9
_fault_N9:
    mov qword [_save_fault_number], qword 9
    jmp all_faults

; int 10 
global _fault_N10
_fault_N10:
    mov qword [_save_fault_number], qword 10
    jmp all_faults

; int 11
global _fault_N11
_fault_N11:
    mov qword [_save_fault_number], qword 11
    jmp all_faults

; int 12 - Falha de pilha (interrupção 12).
global _fault_STACK
_fault_STACK:
    mov qword [_save_fault_number], qword 12
    jmp all_faults

; int 13 - general protection fault (GPF).
global _fault_GP
_fault_GP:   
    mov qword [_save_fault_number], qword 13
    jmp all_faults

; int 14 - Page Fault (PF).
; #todo:
; The hardware pushes a stack frame with the context and generate
; an error code describing the error.
global _fault_N14
_fault_N14:
    mov qword [_save_fault_number], qword 14
    jmp all_faults

; int 15 
global _fault_N15
_fault_N15:
    mov qword [_save_fault_number], qword 15
    jmp all_faults

; int 16 
global _fault_N16
_fault_N16:
    mov qword [_save_fault_number], qword 16
    jmp all_faults

; int 17 
global _fault_N17
_fault_N17:
    mov qword [_save_fault_number], qword 17
    jmp all_faults

; int 18
global _fault_N18
_fault_N18:
    mov qword [_save_fault_number], qword 18
    jmp all_faults

; int 19 - Intel reserved.
global _fault_N19
_fault_N19:
    mov qword [_save_fault_number], qword 19
    jmp all_faults

; int 20 - Intel reserved. 
global _fault_N20
_fault_N20:
    mov qword [_save_fault_number], qword 20
    jmp all_faults

; int 21 - Intel reserved.
global _fault_N21
_fault_N21:
    mov qword [_save_fault_number], qword 21
    jmp all_faults

; int 22 - Intel reserved.
global _fault_N22
_fault_N22:
    mov qword [_save_fault_number], qword 22
    jmp all_faults

; int 23 - Intel reserved. 
global _fault_N23
_fault_N23:
    mov qword [_save_fault_number], qword 23
    jmp all_faults

; int 24 - Intel reserved. 
global _fault_N24
_fault_N24:
    mov qword [_save_fault_number], qword 24
    jmp all_faults

; int 25 - Intel reserved. 
global _fault_N25
_fault_N25:
    mov qword [_save_fault_number], qword 25
    jmp all_faults

; int 26 - Intel reserved. 
global _fault_N26
_fault_N26:
    mov qword [_save_fault_number], qword 26
    jmp all_faults

; int 27 - Intel reserved. 
global _fault_N27
_fault_N27:
    mov qword [_save_fault_number], qword 27
    jmp all_faults

; int 28 - Intel reserved. 
global _fault_N28
_fault_N28:
    mov qword [_save_fault_number], qword 28
    jmp all_faults

; int 29 - Intel reserved.
global _fault_N29
_fault_N29:
    mov qword [_save_fault_number], qword 29
    jmp all_faults

; int 30 - Intel reserved.
global _fault_N30
_fault_N30:
    mov qword [_save_fault_number], qword 30
    jmp all_faults

; int 31 - Intel reserved. 
global _fault_N31
_fault_N31:
    mov qword [_save_fault_number], qword 31
    jmp all_faults


;---------------------------------------
; One trampoline above jumps here.
; rdi for first parameter.
; New calling conventions for x86_64.
; Chama código em C. 
; (x64nmi.c)
; #todo
; Temos que salvar o contexto,
; pois vamos retornar após alguns tipos de fault,
; como é o caso de PF.
extern _x64_all_faults
align 4  
all_faults:

; Save context.
    cli

    pop qword [_contextRIP]     ; rip
    pop qword [_contextCS]      ; cs (R3)
    pop qword [_contextRFLAGS]  ; rflags
    pop qword [_contextRSP]     ; rsp
    pop qword [_contextSS]      ; ss

    mov qword [_contextRDX], rdx 
    mov qword [_contextRCX], rcx 
    mov qword [_contextRBX], rbx 
    mov qword [_contextRAX], rax

    mov qword [_contextRBP], rbp

    mov qword [_contextRDI], rdi 
    mov qword [_contextRSI], rsi 

    mov qword [_contextR8], r8
    mov qword [_contextR9], r9
    mov qword [_contextR10], r10
    mov qword [_contextR11], r11
    mov qword [_contextR12], r12
    mov qword [_contextR13], r13
    mov qword [_contextR14], r14
    mov qword [_contextR15], r15

; Segments
    xor rax, rax
    mov ax, gs
    mov word [_contextGS], ax
    mov ax, fs
    mov word [_contextFS], ax
    mov ax, es
    mov word [_contextES], ax
    mov ax, ds
    mov word [_contextDS], ax

; FPU
; See:
; https://wiki.osdev.org/SSE
    fxsave [_context_fpu_buffer]

; cpl
; see: x64cont.c
    mov rax, qword [_contextCS] ;(R3)
    and rax, 3
    mov [_contextCPL], rax

; Call c routine in x64fault.c.
    mov rax, qword [_save_fault_number]
    mov rdi, rax 
    call _x64_all_faults 

; FPU
    fxrstor [_context_fpu_buffer]

    ;jmp _AllFaultsHang

; retornaremos com o contexto da proxima thread,
; e com o cr3 atualizado pela rotina de restauraçao de contexto.
; Nao precisa de eoi, pois não é uma interrupçao de dispositivo
; é so retornar para a proxima thread. que sera a thread
; de controle do processo init.

    mov RAX, CR3  
    IODELAY 
    mov CR3, RAX  

;
; == Restore context ====================
;

    ; Segments
    xor rax, rax
    mov ax, word [_contextDS]
    mov ds, ax
    mov ax, word [_contextES]
    mov es, ax
    mov ax, word [_contextFS]
    mov fs, ax
    mov ax, word [_contextGS]
    mov gs, ax

    mov r15, qword [_contextR15]
    mov r14, qword [_contextR14]
    mov r13, qword [_contextR13]
    mov r12, qword [_contextR12]
    mov r11, qword [_contextR11]
    mov r10, qword [_contextR10]
    mov r9,  qword [_contextR9]
    mov r8,  qword [_contextR8]


    mov rsi, qword [_contextRSI] 
    mov rdi, qword [_contextRDI] 
    
    mov rbp, qword [_contextRBP] 
    
    mov rax, qword [_contextRAX] 
    mov rbx, qword [_contextRBX] 
    mov rcx, qword [_contextRCX] 
    mov rdx, qword [_contextRDX] 


    ;; Stack frame. (all double)
    push qword [_contextSS]      ; ss
    push qword [_contextRSP]     ; rsp
    push qword [_contextRFLAGS]  ; rflags
    push qword [_contextCS]      ; cs
    push qword [_contextRIP]     ; rip

    ; Acumulator
    mov rax, qword [_contextRAX]

; #bugbug
; We do NOT need the 'sti'. 
; The flags in the 'eflags' will reenable it.

    sti
    iretq

; #todo
; We're gonna restore the execution in some cases,
; just like PF.

; Release a bandit.
    ;jmp unit3Irq0Release
    
; Provisório
_AllFaultsHang:
    cli
    hlt
    jmp _AllFaultsHang


; Salva aqui o número da fault.	
global _save_fault_number
_save_fault_number: 
    dq 0
    
align 8

