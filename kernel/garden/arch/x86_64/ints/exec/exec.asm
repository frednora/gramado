; Execution Hall (Red) — exec.asm
; Created by Fred Nora.

unhandled_irq:
    ;cli ;#bugbug
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
    ;sti   ;#bugbug
    iretq
;--


; Provisório
_AllFaultsHang:
    cli
    hlt
    jmp _AllFaultsHang


; Salva aqui o número da fault.	
global _save_fault_number
_save_fault_number: 
    dq 0

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
    ;cli

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

    ;sti
    iretq

;----------------------------
; Building trampolines for the faults.

align 8

; int 0
; Se ocorrer em ring 0 o sistema tem que terminar,
; e se for em ring3, fechamos o aplicativo.
global ExecutionHall_fault_N0
ExecutionHall_fault_N0:
    mov qword [_save_fault_number], qword 0
    jmp all_faults

; int 1 
global ExecutionHall_fault_N1
ExecutionHall_fault_N1:
    mov qword [_save_fault_number], qword 1
    jmp all_faults

; int 2 
global ExecutionHall_fault_N2
ExecutionHall_fault_N2:
    mov qword [_save_fault_number], qword 2
    jmp all_faults

; int 3 
global ExecutionHall_fault_N3
ExecutionHall_fault_N3:
    mov qword [_save_fault_number], qword 3
    jmp all_faults

; int 4 
global ExecutionHall_fault_N4
ExecutionHall_fault_N4:
    mov qword [_save_fault_number], qword 4
    jmp all_faults

; int 5 
global ExecutionHall_fault_N5
ExecutionHall_fault_N5:
     mov qword [_save_fault_number], qword 5
    jmp all_faults

; int 6: Invalid opcode
global ExecutionHall_fault_INVALID_OPCODE
ExecutionHall_fault_INVALID_OPCODE:
    mov qword [_save_fault_number], qword 6
    jmp all_faults

; int 7
global ExecutionHall_fault_N7
ExecutionHall_fault_N7:
    mov qword [_save_fault_number], qword 7
    jmp all_faults

; int 8 - double fault
global ExecutionHall_fault_DOUBLE
ExecutionHall_fault_DOUBLE:
    mov qword [_save_fault_number], qword 8
    jmp all_faults

; int 9 
global ExecutionHall_fault_N9
ExecutionHall_fault_N9:
    mov qword [_save_fault_number], qword 9
    jmp all_faults

; int 10 
global ExecutionHall_fault_N10
ExecutionHall_fault_N10:
    mov qword [_save_fault_number], qword 10
    jmp all_faults

; int 11
global ExecutionHall_fault_N11
ExecutionHall_fault_N11:
    mov qword [_save_fault_number], qword 11
    jmp all_faults

; int 12 - Falha de pilha (interrupção 12).
global ExecutionHall_fault_STACK
ExecutionHall_fault_STACK:
    mov qword [_save_fault_number], qword 12
    jmp all_faults

; int 13 - general protection fault (GPF).
global ExecutionHall_fault_GP
ExecutionHall_fault_GP:   
    mov qword [_save_fault_number], qword 13
    jmp all_faults

; int 14 - Page Fault (PF).
; #todo:
; The hardware pushes a stack frame with the context and generate
; an error code describing the error.
global ExecutionHall_fault_N14
ExecutionHall_fault_N14:
    mov qword [_save_fault_number], qword 14
    jmp all_faults

; int 15 
global ExecutionHall_fault_N15
ExecutionHall_fault_N15:
    mov qword [_save_fault_number], qword 15
    jmp all_faults

; int 16 
global ExecutionHall_fault_N16
ExecutionHall_fault_N16:
    mov qword [_save_fault_number], qword 16
    jmp all_faults

; int 17 
global ExecutionHall_fault_N17
ExecutionHall_fault_N17:
    mov qword [_save_fault_number], qword 17
    jmp all_faults

; int 18
global ExecutionHall_fault_N18
ExecutionHall_fault_N18:
    mov qword [_save_fault_number], qword 18
    jmp all_faults

; int 19 - Intel reserved.
global ExecutionHall_fault_N19
ExecutionHall_fault_N19:
    mov qword [_save_fault_number], qword 19
    jmp all_faults

; int 20 - Intel reserved. 
global ExecutionHall_fault_N20
ExecutionHall_fault_N20:
    mov qword [_save_fault_number], qword 20
    jmp all_faults

; int 21 - Intel reserved.
global ExecutionHall_fault_N21
ExecutionHall_fault_N21:
    mov qword [_save_fault_number], qword 21
    jmp all_faults

; int 22 - Intel reserved.
global ExecutionHall_fault_N22
ExecutionHall_fault_N22:
    mov qword [_save_fault_number], qword 22
    jmp all_faults

; int 23 - Intel reserved. 
global ExecutionHall_fault_N23
ExecutionHall_fault_N23:
    mov qword [_save_fault_number], qword 23
    jmp all_faults

; int 24 - Intel reserved. 
global ExecutionHall_fault_N24
ExecutionHall_fault_N24:
    mov qword [_save_fault_number], qword 24
    jmp all_faults

; int 25 - Intel reserved. 
global ExecutionHall_fault_N25
ExecutionHall_fault_N25:
    mov qword [_save_fault_number], qword 25
    jmp all_faults

; int 26 - Intel reserved. 
global ExecutionHall_fault_N26
ExecutionHall_fault_N26:
    mov qword [_save_fault_number], qword 26
    jmp all_faults

; int 27 - Intel reserved. 
global ExecutionHall_fault_N27
ExecutionHall_fault_N27:
    mov qword [_save_fault_number], qword 27
    jmp all_faults

; int 28 - Intel reserved. 
global ExecutionHall_fault_N28
ExecutionHall_fault_N28:
    mov qword [_save_fault_number], qword 28
    jmp all_faults

; int 29 - Intel reserved.
global ExecutionHall_fault_N29
ExecutionHall_fault_N29:
    mov qword [_save_fault_number], qword 29
    jmp all_faults

; int 30 - Intel reserved.
global ExecutionHall_fault_N30
ExecutionHall_fault_N30:
    mov qword [_save_fault_number], qword 30
    jmp all_faults

; int 31 - Intel reserved. 
global ExecutionHall_fault_N31
ExecutionHall_fault_N31:
    mov qword [_save_fault_number], qword 31
    jmp all_faults

align 8

