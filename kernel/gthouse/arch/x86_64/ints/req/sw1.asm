; Request Hall (White)
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
; RequestHall_int128
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
global RequestHall_int128
RequestHall_int128:

    cli 

; #ps:
; For now We dont need disable interrupts in our syscalls,
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

; Current Privilege Level - (CPL)
; Get the first 2 bits of CS.
; see: x64mi.c sci.c
; We need to get CS from the stack and not from the register.
; Maybe the processor load CS with the Selector value
; present in the IDT.

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
; RequestHall_int129
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
global RequestHall_int129
RequestHall_int129:

    cli 

; #ps:
; For now We dont need disable interrupts in our syscalls,
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

; Current Privilege Level - (CPL)
; Get the first 2 bits of CS.
; see: x64mi.c sci.c
; We need to get CS from the stack and not from the register.
; Maybe the processor load CS with the Selector value
; present in the IDT.

    mov rax, qword [.int129_cs]
    and rax, 3
    mov qword [_sci1_cpl], rax

    fxsave [__sw_local_fpu_buffer]

    call _sc81h

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
; RequestHall_int130
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
global RequestHall_int130
RequestHall_int130:

    cli 

; #ps:
; For now We dont need disable interrupts in our syscalls,
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

; Current Privilege Level - (CPL)
; Get the first 2 bits of CS.
; see: x64mi.c sci.c
; We need to get CS from the stack and not from the register.
; Maybe the processor load CS with the Selector value
; present in the IDT.

    mov rax, qword [.int130_cs]
    and rax, 3
    mov qword [_sci2_cpl], rax

    fxsave [__sw_local_fpu_buffer]

    call _sc82h

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
; RequestHall_int131
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
global RequestHall_int131
RequestHall_int131:

    cli 

; #ps:
; For now We dont need disable interrupts in our syscalls,
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

; Current Privilege Level - (CPL)
; Get the first 2 bits of CS.
; see: x64mi.c sci.c
; We need to get CS from the stack and not from the register.
; Maybe the processor load CS with the Selector value
; present in the IDT.

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


