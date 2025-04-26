; sw1.asm
; This file handles syscall for x86_64 processors.
; Creaed by Fred Nora.

; ints 0x80, 0x81, 0x82
; int 0xC7
; ...

align 16
__sw_local_fpu_buffer:
    times 512 db 0
align 16


;------------------------
; _int128
;     System Call number 0x80
;     >>>> ONLY CALLED FROM USERMODE!
;     + It is never called from kernel mode.
;     + It calls a system service without changing the segment registers.
;     + We are using the caller cr3.
;     It has four parameters:
;     rax - Argument 1
;     rbx - Argument 2
;     rcx - Argument 3
;     rdx - Argument 4
;     #todo: 
;     Maybe we can receive more values using more registers.

extern _sc80h
extern _sci0_cpl
; Capture context
align 4  
global _int128
_int128:

    ; #ps:
    ; We dont need disable interrupts in our syscalls,
    ; because all the IDT entries are using EE00,
    ; present, dpl=3, interrupt gate, where the interrupts
    ; are disabled by default.

    pop qword [.int128_rip]
    pop qword [.int128_cs]

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
    push rsp
    pushfq

; Parameters:
; Let's fill the parameters for the handler.
; RDI, RSI, RDX, RCX, R8, and R9 are used 
; for integer and memory address arguments

    mov rdi, rax  ; arg1: service number
    mov rsi, rbx  ; arg2
    push rdx      ; Saving arg4
    mov rdx, rcx  ; arg3
    pop rcx       ; arg4 

; cpl
; Get the first 2 bits of cs.
; see: x64mi.c sci.c
; #importante: temos que pegar cs na pilha e nao no registrador.
; talvez durante a systemcall o processador mude o cs para o valor 
; do seletor indicado na entrada da idt.
    ;xor rax, rax
    mov rax, qword [.int128_cs]
    and rax, 3
    mov qword [_sci0_cpl], rax

    fxsave [__sw_local_fpu_buffer]

    call _sc80h

    fxrstor [__sw_local_fpu_buffer]

    mov qword [.int128Ret], rax 

    popfq
    pop rsp
    pop gs
    pop fs
    ;pop es
    ;pop ds

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

    mov rax, qword [.int128Ret] 

    push qword [.int128_cs]      ; cs
    push qword [.int128_rip]     ; rip
    iretq
.int128Ret: dq 0
.int128_cs: dq 0
.int128_rip: dq 0
;--  

;;-----
; _int129
;     System Call number 0x81
;     ONLY CALLED FROM USERMODE!
;     + It is never called from kernel mode.
;     + It chamges the segment registers before calling the system service.
;     + We are using the caller cr3.
;     It has four parameters:
;     rax - Argument 1
;     rbx - Argument 2
;     rcx - Argument 3
;     rdx - Argument 4
;     #todo: 
;     Maybe we can receive more values using more registers.
;
;;-----

extern _sc81h
extern _sci1_cpl
; Capture context
align 4  
global _int129
_int129:

    ; #ps:
    ; We dont need disable interrupts in our syscalls,
    ; because all the IDT entries are using EE00,
    ; present, dpl=3, interrupt gate, where the interrupts
    ; are disabled by default.

    pop qword [.int129_rip]
    pop qword [.int129_cs]

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
    push rsp
    pushfq

; Parameters:
; Let's fill the parameters for the handler.
; RDI, RSI, RDX, RCX, R8, and R9 are used 
; for integer and memory address arguments

    mov rdi, rax  ; arg1: service number
    mov rsi, rbx  ; arg2
    push rdx      ; Saving arg4
    mov rdx, rcx  ; arg3
    pop rcx       ; arg4 

; cpl
; Get the first 2 bits of cs.
; see: x64mi.c sci.c
; #importante: temos que pegar cs na pilha e nao no registrador.
; talvez durante a systemcall o processador mude o cs para o valor 
; do seletor indicado na entrada da idt.
    ;xor rax, rax
    mov rax, qword [.int129_cs]
    and rax, 3
    mov qword [_sci1_cpl], rax

    fxsave [__sw_local_fpu_buffer]

    call _sc81h

;-----------------------
    fxrstor [__sw_local_fpu_buffer]
    mov qword [.int129Ret], rax 

    popfq
    pop rsp
    pop gs
    pop fs
    ;pop es
    ;pop ds

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

    mov rax, qword [.int129Ret] 

    push qword [.int129_cs]      ; cs
    push qword [.int129_rip]     ; rip
    iretq
.int129Ret: dq 0
.int129_cs: dq 0
.int129_rip: dq 0
;--  

;;-----
; _int130
;     System Call number 0x82
;     ONLY CALLED FROM USERMODE!
;     + It is never called from kernel mode.
;     + It chamges the segment registers before calling the system service.
;     + We are using the caller cr3.
;     It has four parameters:
;     rax - Argument 1
;     rbx - Argument 2
;     rcx - Argument 3
;     rdx - Argument 4
;     #todo: 
;     Maybe we can receive more values using more registers.
;;-----

extern _sc82h
extern _sci2_cpl
; Capture context
align 4  
global _int130
_int130:

    ; #ps:
    ; We dont need disable interrupts in our syscalls,
    ; because all the IDT entries are using EE00,
    ; present, dpl=3, interrupt gate, where the interrupts
    ; are disabled by default.

    pop qword [.int130_rip]
    pop qword [.int130_cs]

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
    push rsp
    pushfq

; Parameters:
; Let's fill the parameters for the handler.
; RDI, RSI, RDX, RCX, R8, and R9 are used 
; for integer and memory address arguments

    mov rdi, rax  ; arg1: service number
    mov rsi, rbx  ; arg2
    push rdx      ; Saving arg4
    mov rdx, rcx  ; arg3
    pop rcx       ; arg4 

; cpl
; Get the first 2 bits of cs.
; see: x64mi.c sci.c
; #importante: temos que pegar cs na pilha e nao no registrador.
; talvez durante a systemcall o processador mude o cs para o valor 
; do seletor indicado na entrada da idt.
    ;xor rax, rax
    mov rax, qword [.int130_cs]
    and rax, 3
    mov qword [_sci2_cpl], rax

    fxsave [__sw_local_fpu_buffer]

    call _sc82h

; ----------------------
    fxrstor [__sw_local_fpu_buffer]
    mov qword [.int130Ret], rax 

    popfq
    pop rsp
    pop gs
    pop fs
    ;pop es
    ;pop ds

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

    mov rax, qword [.int130Ret] 

    push qword [.int130_cs]      ; cs
    push qword [.int130_rip]     ; rip
    iretq
.int130Ret: dq 0
.int130_cs: dq 0
.int130_rip: dq 0
;--    

;;-----
; _int131
;     System Call number 0x82
;     ONLY CALLED FROM USERMODE!
;     + It is never called from kernel mode.
;     + It chamges the segment registers before calling the system service.
;     + We are using the caller cr3.
;     It has four parameters:
;     rax - Argument 1
;     rbx - Argument 2
;     rcx - Argument 3
;     rdx - Argument 4
;     #todo: 
;     Maybe we can receive more values using more registers.
;;-----

extern _sc83h
extern _sci3_cpl
; Capture context
align 4  
global _int131
_int131:

    ; #ps:
    ; We dont need disable interrupts in our syscalls,
    ; because all the IDT entries are using EE00,
    ; present, dpl=3, interrupt gate, where the interrupts
    ; are disabled by default.

    pop qword [.int131_rip]
    pop qword [.int131_cs]

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
    push rsp
    pushfq

; Parameters:
; Let's fill the parameters for the handler.
; RDI, RSI, RDX, RCX, R8, and R9 are used 
; for integer and memory address arguments

    mov rdi, rax  ; arg1: service number
    mov rsi, rbx  ; arg2
    push rdx      ; Saving arg4
    mov rdx, rcx  ; arg3
    pop rcx       ; arg4

; cpl
; Get the first 2 bits of cs.
; see: x64mi.c sci.c
; #importante: temos que pegar cs na pilha e nao no registrador.
; talvez durante a systemcall o processador mude o cs para o valor 
; do seletor indicado na entrada da idt.
    ;xor rax, rax
    mov rax, qword [.int131_cs]
    and rax, 3
    mov qword [_sci3_cpl], rax

    fxsave [__sw_local_fpu_buffer]

    call _sc83h

; ----------------------
    fxrstor [__sw_local_fpu_buffer]
    mov qword [.int131Ret], rax 

    popfq
    pop rsp
    pop gs
    pop fs
    ;pop es
    ;pop ds

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

    mov rax, qword [.int131Ret] 

    push qword [.int131_cs]      ; cs
    push qword [.int131_rip]     ; rip
    iretq
.int131Ret: dq 0
.int131_cs: dq 0
.int131_rip: dq 0
;--    

; ====================================
; int 198 (0xC6) - Callback restorer.


; ----------------------------------------------
; _int199:
; It will enable the interrupts and the taskswitching
; for the first time.
; It is because the kernel goes to the init process
; with the interrupts disabled. ;)
; Is this called only from the init process in user mode?
; I guess all the processes in ring 3 are calling this routine.
; It means that all processes are starting with some kind
; of privilegies until the crt0 routine call this interrupt.
; see: INIT.BIN.
; Enable the interrupts.
; 9 - IF
; 'Interrupt enable' flag
; Changing the iopl.
; bits '12-13'
; IOPL, I/O privilege level.
; We drop the minimum iopl level to ring 0,
; so, this way only a process with cpl 0
; is able to use instructions like in, out, cli, sti.
align 4  
; 0xC7
global _int199
_int199:
    jmp miC7H
    jmp $
miC7H:
; Maskable interrupt
    pop qword [.frameRIP]
    pop qword [.frameCS]
    pop qword [.frameRFLAGS]
; iopl 0 (Drop)
    mov qword [.frameRFLAGS], 0x0000000000000200
; iopl 3
    ;mov qword [.frameRFLAGS], 0x0000000000003200
    push qword [.frameRFLAGS]
    push qword [.frameCS]
    push qword [.frameRIP]
    iretq
.frameRIP:     dq 0
.frameCS:      dq 0
.frameRFLAGS:  dq 0

