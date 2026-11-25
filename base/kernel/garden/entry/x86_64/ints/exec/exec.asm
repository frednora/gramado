; Execution Hall (Red) — exec.asm
; Created by Fred Nora.


; Faults (fixable, returns to the faulting instruction):
;   Vector 0 (#DE): Divide Error (division by zero)
;	Vector 5 (#BR): BOUND Range Exceeded
;	Vector 6 (#UD): Invalid Opcode (undefined instruction)
;	Vector 13 (#GP): General Protection Fault
;   Vector 14 (#PF): Page Fault
;
; Traps (reported immediately after the execution of the trapping instruction):
;   Vector 1 (#DB): Debug
;   Vector 3 (#BP): Breakpoint
;   Vector 4 (#OF): Overflow
;
; Aborts (severe unrecoverable hardware errors):
;   Vector 8 (#DF): Double Fault
;   Vector 18 (#MC): Machine Check


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


; Save the exception number here
global _save_exception_number
_save_exception_number: 
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

extern _x64excep_all_exceptions

align 4  
x86_64_exception_handler:

; Save context
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
    mov rax, qword [_save_exception_number]
    mov rdi, rax 
    call _x64excep_all_exceptions 

; FPU
    fxrstor [_context_fpu_buffer]

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

;----------------------------
; Building trampolines for the exveptions

align 4

; int 0 - (FAULT)
; 0 (#DE) Divide Error. Occurs when dividing by zero or when a division 
; result exceeds the destination register's capacity.
global ExecutionHall_exception_N0
ExecutionHall_exception_N0:
    mov qword [_save_exception_number], qword 0
    jmp x86_64_exception_handler

; int 1 - (TRAP)
; 1 (#DB) Debug. Triggered by hardware breakpoints, 
; single-stepping instructions, or debug register conditions.
global ExecutionHall_exception_N1
ExecutionHall_exception_N1:
    mov qword [_save_exception_number], qword 1
    jmp x86_64_exception_handler

; int 2 
; 2 (NMI) Non-Maskable Interrupt. Signals critical, unignorable hardware events such as 
; memory parity errors or watchdog timeouts.
global ExecutionHall_exception_N2
ExecutionHall_exception_N2:
    mov qword [_save_exception_number], qword 2
    jmp x86_64_exception_handler

; int 3 - (TRAP)
; 3 (#BP) Breakpoint. Generated intentionally by software executing 
; the INT3 instruction for debugging.
global ExecutionHall_exception_N3
ExecutionHall_exception_N3:
    mov qword [_save_exception_number], qword 3
    jmp x86_64_exception_handler

; int 4 - (TRAP)
; 4 (#OF) Overflow. Triggered when the INTO instruction executes 
; while the CPU's overflow flag (OF) is set.
global ExecutionHall_exception_N4
ExecutionHall_exception_N4:
    mov qword [_save_exception_number], qword 4
    jmp x86_64_exception_handler

; int 5 - (FAULT)
; 5 (#BR) BOUND Range Exceeded. Occurs when the BOUND instruction detects an array index 
; outside its specified lower or upper limits.
global ExecutionHall_exception_N5
ExecutionHall_exception_N5:
     mov qword [_save_exception_number], qword 5
    jmp x86_64_exception_handler

; int 6: Invalid opcode - (FAULT)
; 6 (#UD) Invalid Opcode. Fires when the processor tries to decode an 
; unrecognized, undefined, or unsupported instruction.
global ExecutionHall_exception_INVALID_OPCODE
ExecutionHall_exception_INVALID_OPCODE:
    mov qword [_save_exception_number], qword 6
    jmp x86_64_exception_handler

; int 7
; 7 (#NM) Device Not Available. Happens when executing x87 FPU or SIMD instructions while 
; the math coprocessor is disabled or marked unavailable.
global ExecutionHall_exception_N7
ExecutionHall_exception_N7:
    mov qword [_save_exception_number], qword 7
    jmp x86_64_exception_handler

; int 8 - double fault (ABORT)
; 8 (#DF) Double Fault. An unrecoverable abort triggered when a second exception occurs while 
; trying to invoke the handler for a first exception.
global ExecutionHall_exception_DOUBLE
ExecutionHall_exception_DOUBLE:
    mov qword [_save_exception_number], qword 8
    jmp x86_64_exception_handler

; int 9 
; 9 Coprocessor Segment Overrun. A legacy 386-era fault caused when an 
; external math coprocessor operand crossed a segment boundary.
global ExecutionHall_exception_N9
ExecutionHall_exception_N9:
    mov qword [_save_exception_number], qword 9
    jmp x86_64_exception_handler

; int 10 
; 10 (#TS) Invalid TSS. Occurs during a task switch when the Task State Segment (TSS) 
; contains invalid selectors or limits.
global ExecutionHall_exception_N10
ExecutionHall_exception_N10:
    mov qword [_save_exception_number], qword 10
    jmp x86_64_exception_handler

; int 11
; 11 (#NP) Segment Not Present. Fires when loading a segment register whose 
; descriptor has its "present" bit cleared to zero.
global ExecutionHall_exception_N11
ExecutionHall_exception_N11:
    mov qword [_save_exception_number], qword 11
    jmp x86_64_exception_handler

; int 12 - stack
; 12 (#SS) Stack-Segment Fault. Triggered by stack limit violations or 
; when loading a non-present stack segment selector.
global ExecutionHall_exception_STACK
ExecutionHall_exception_STACK:
    mov qword [_save_exception_number], qword 12
    jmp x86_64_exception_handler

; int 13 - general protection fault (GPF) - (FAULT)
; 13 (#GP) General Protection Fault. Catches privilege violations, 
; null selector dereferences, canonical address errors, and 
; other memory protection breaches.
global ExecutionHall_exception_GP
ExecutionHall_exception_GP:   
    mov qword [_save_exception_number], qword 13
    jmp x86_64_exception_handler

; int 14 - Page Fault (PF) - (FAULT)
; 14 (#PF) Page Fault. Occurs when accessing a virtual memory page that 
; is unmapped, swapped out, or violates read/write/user permissions.
; #todo:
; The hardware pushes a stack frame with the context and generate
; an error code describing the error.
global ExecutionHall_exception_N14
ExecutionHall_exception_N14:
    mov qword [_save_exception_number], qword 14
    jmp x86_64_exception_handler

; int 15 
; 15 Reserved. Permanently reserved by Intel and AMD for internal architecture use.
global ExecutionHall_exception_N15
ExecutionHall_exception_N15:
    mov qword [_save_exception_number], qword 15
    jmp x86_64_exception_handler

; int 16 
; 16 (#MF) x87 Floating-Point Exception. Signals an unmasked x87 math error, 
; such as floating-point overflow, underflow, or invalid operation.
global ExecutionHall_exception_N16
ExecutionHall_exception_N16:
    mov qword [_save_exception_number], qword 16
    jmp x86_64_exception_handler

; int 17 
; 17 (#AC) Alignment Check. Fires when unaligned memory access occurs while 
; alignment checking is enabled in user mode.
global ExecutionHall_exception_N17
ExecutionHall_exception_N17:
    mov qword [_save_exception_number], qword 17
    jmp x86_64_exception_handler

; int 18 - Machine check (ABORT)
; 18 (#MC) Machine Check. An abort indicating severe internal hardware failures 
; like CPU cache, bus, or memory controller errors.
global ExecutionHall_exception_N18
ExecutionHall_exception_N18:
    mov qword [_save_exception_number], qword 18
    jmp x86_64_exception_handler

; int 19 - Intel reserved
; 19 (#XM / #XF) SIMD Floating-Point Exception. Reports unmasked floating-point errors 
; generated by SSE or AVX vector instructions.
global ExecutionHall_exception_N19
ExecutionHall_exception_N19:
    mov qword [_save_exception_number], qword 19
    jmp x86_64_exception_handler

; int 20 - Intel reserved
; 20 (#VE) Virtualization Exception. Notifies a guest OS of 
; Extended Page Table (EPT) violations so it can handle 
; them without a full VM exit.
global ExecutionHall_exception_N20
ExecutionHall_exception_N20:
    mov qword [_save_exception_number], qword 20
    jmp x86_64_exception_handler

; int 21 - Intel reserved
; 21 (#CP) Control Protection Exception. Triggered by 
; Control-flow Enforcement Technology (CET) when a RET or 
; indirect jump violates shadow stack or branch tracking rules.
global ExecutionHall_exception_N21
ExecutionHall_exception_N21:
    mov qword [_save_exception_number], qword 21
    jmp x86_64_exception_handler

; int 22 - Intel reserved
; Unassigned vectors reserved by Intel and AMD for future processor features.
global ExecutionHall_exception_N22
ExecutionHall_exception_N22:
    mov qword [_save_exception_number], qword 22
    jmp x86_64_exception_handler

; int 23 - Intel reserved
; Unassigned vectors reserved by Intel and AMD for future processor features.
global ExecutionHall_exception_N23
ExecutionHall_exception_N23:
    mov qword [_save_exception_number], qword 23
    jmp x86_64_exception_handler

; int 24 - Intel reserved
; Unassigned vectors reserved by Intel and AMD for future processor features.
global ExecutionHall_exception_N24
ExecutionHall_exception_N24:
    mov qword [_save_exception_number], qword 24
    jmp x86_64_exception_handler

; int 25 - Intel reserved
; Unassigned vectors reserved by Intel and AMD for future processor features.
global ExecutionHall_exception_N25
ExecutionHall_exception_N25:
    mov qword [_save_exception_number], qword 25
    jmp x86_64_exception_handler

; int 26 - Intel reserved
; Unassigned vectors reserved by Intel and AMD for future processor features.
global ExecutionHall_exception_N26
ExecutionHall_exception_N26:
    mov qword [_save_exception_number], qword 26
    jmp x86_64_exception_handler

; int 27 - Intel reserved
; Unassigned vectors reserved by Intel and AMD for future processor features.
global ExecutionHall_exception_N27
ExecutionHall_exception_N27:
    mov qword [_save_exception_number], qword 27
    jmp x86_64_exception_handler

; int 28 - Intel reserved
; 28 (#HV) Hypervisor Injection Exception. Used by AMD SEV-SNP hypervisors to 
; safely inject virtual interrupts or events into an encrypted guest VM.
global ExecutionHall_exception_N28
ExecutionHall_exception_N28:
    mov qword [_save_exception_number], qword 28
    jmp x86_64_exception_handler

; int 29 - Intel reserved
; 29 (#VC) VMM Communication Exception. 
; Triggered on AMD SEV-ES/SNP guests when an instruction requires 
; hypervisor assistance via the GHCB protocol.
global ExecutionHall_exception_N29
ExecutionHall_exception_N29:
    mov qword [_save_exception_number], qword 29
    jmp x86_64_exception_handler

; int 30 - Intel reserved
; 30 (#SX) Security Exception. Signals security-sensitive events 
; on AMD processors, such as an external INIT signal attempting to 
; reset a protected system.
global ExecutionHall_exception_N30
ExecutionHall_exception_N30:
    mov qword [_save_exception_number], qword 30
    jmp x86_64_exception_handler

; int 31 - Intel reserved
; 31 Reserved. Unassigned vector reserved for future hardware architecture use
global ExecutionHall_exception_N31
ExecutionHall_exception_N31:
    mov qword [_save_exception_number], qword 31
    jmp x86_64_exception_handler

align 4

