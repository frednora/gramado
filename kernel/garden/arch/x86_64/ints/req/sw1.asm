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


; systemcall64:
; Privilege transition: Moves execution from CPL=3 (user mode) to CPL=0 (kernel mode).
; Entry point: Loads RIP from the IA32_LSTAR MSR (Model Specific Register), 
; which must be set by the OS to point to the kernel’s syscall handler.
; Stack handling: The kernel is responsible for setting up a 
; separate stack for handling system calls, often using 
; the IA32_STAR MSR to define the stack segment.
; MSR setup: 
; The OS must configure IA32_STAR, IA32_LSTAR, and IA32_FMASK 
; before enabling SYSCALL/SYSRET.
; Shadow stacks (CET): On modern CPUs, SYSCALL also interacts 
; with shadow stack pointers (IA32_PL3_SSP MSR).
;

; RCX and R11 are handled automatically by the CPU 
; when you execute SYSCALL in long mode. 
; The hardware does the save/restore work as part 
; of the privilege transition.
; RCX: The CPU saves the user‑mode RIP (the return address) into RCX.
; R11: The CPU saves the user‑mode RFLAGS into R11.
;      Then it masks those flags using the 
;      IA32_FMASK MSR (to clear bits like interrupt enable).
;      On SYSRET, R11 is used to restore the original RFLAGS back to user mode.


; Flow summary:
; 1) User executes SYSCALL.
; 2) CPU:
; + Saves RIP → RCX.
; + Saves RFLAGS → R11.
; + Loads kernel RIP from IA32_LSTAR.
; + Loads CS/SS from IA32_STAR.
; + Applies IA32_FMASK to RFLAGS.
; 3) Kernel runs syscall handler.
; 4) Kernel eventually executes SYSRET.
; 5) CPU:
; + Restores RIP from RCX.
; + Restores RFLAGS from R11.
; + Returns to CPL=3.

;
; When you invoke SYSCALL, arguments are passed in registers:
;
; RAX → syscall number
; RDI → arg1
; RSI → arg2
; RDX → arg3


align 4
global _systemcall64
_systemcall64: 
    mov qword [.save_rcx], rcx
    mov qword [.save_r11], r11

    ; ...
    int 3

    mov rcx, qword [.save_rcx]
    mov r11, qword [.save_r11]
    sysret
.save_rcx: dq 0
.save_r11: dq 0

;------------------------
; RequestHall_int128
;     System Call number 0x80
;     >>>> ONLY CALLED FROM USERMODE!
;     + It is never called from kernel mode.
;     + It calls a system service without changing the segment registers.
;     + We are using the caller cr3.
; It has four parameters:
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

; Systemcall from 64-bit mode.
; It's not called from kernel-mode.
; It's executed with the interrupts disabled.

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
; System V ABI calling convention:
; System V AMD64 ABI:
; RDI, RSI, RDX, RCX, R8, and R9 are used 
; for integer and memory address arguments
; RAX → RDI
; RBX → RSI
; RCX → RDX
; RDX → RCX

    mov rdi, rax  ; RDI <-- arg1 service number
    mov rsi, rbx  ; RSI <-- arg2
    mov r8, rcx   ; Save arg3
    mov r9, rdx   ; Save arg4
    mov rdx, r8  ; RDX <-- arg3
    mov rcx, r9  ; RCX <-- arg4


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

    ; Get the address for the service handler
    mov r10, qword _sc80h
    call r10
    ;call _sc80h

; ------------------
; .sc80_exit:

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

; Restore the stack frame and exit to user mode
    push qword [.int128_cs]      ; cs
    push qword [.int128_rip]     ; rip
    iretq
.int128Ret: dq 0
.int128_cs: dq 0
.int128_rip: dq 0
;--  

; We can't build a dispatch table here, 
; Because the service indexes are not in order
; and the list of indexes is too long.
;DISPATCH_INT128:
;    dq ?   ; 0
    ; ...

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

; Systemcall from 64-bit mode.
; It's not called from kernel-mode.
; It's executed with the interrupts disabled.

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
; System V ABI calling convention:
; System V AMD64 ABI:
; RDI, RSI, RDX, RCX, R8, and R9 are used 
; for integer and memory address arguments
; RAX → RDI
; RBX → RSI
; RCX → RDX
; RDX → RCX

    mov rdi, rax  ; RDI <-- arg1 service number
    mov rsi, rbx  ; RSI <-- arg2
    mov r8, rcx   ; Save arg3
    mov r9, rdx   ; Save arg4
    mov rdx, r8  ; RDX <-- arg3
    mov rcx, r9  ; RCX <-- arg4

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

    mov r10, qword _sc81h
    call r10
    ;call _sc81h

;.sc81h_exit:

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

; Restore the stack frame and exit to user mode
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

; Systemcall from 64-bit mode.
; It's not called from kernel-mode.
; It's executed with the interrupts disabled.

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
; System V ABI calling convention:
; System V AMD64 ABI:
; RDI, RSI, RDX, RCX, R8, and R9 are used 
; for integer and memory address arguments
; RAX → RDI
; RBX → RSI
; RCX → RDX
; RDX → RCX

    mov rdi, rax  ; RDI <-- arg1 service number
    mov rsi, rbx  ; RSI <-- arg2
    mov r8, rcx   ; Save arg3
    mov r9, rdx   ; Save arg4
    mov rdx, r8  ; RDX <-- arg3
    mov rcx, r9  ; RCX <-- arg4

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

    mov r10, qword _sc82h
    call r10
    ;call _sc82h

;.sc82h_exit:

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

; Restore the stack frame and exit to user mode
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

; Systemcall from 64-bit mode.
; It's not called from kernel-mode.
; It's executed with the interrupts disabled.

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
; System V ABI calling convention:
; System V AMD64 ABI:
; RDI, RSI, RDX, RCX, R8, and R9 are used 
; for integer and memory address arguments
; RAX → RDI
; RBX → RSI
; RCX → RDX
; RDX → RCX

    mov rdi, rax  ; RDI <-- arg1 service number
    mov rsi, rbx  ; RSI <-- arg2
    mov r8, rcx   ; Save arg3
    mov r9, rdx   ; Save arg4
    mov rdx, r8  ; RDX <-- arg3
    mov rcx, r9  ; RCX <-- arg4

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

    mov r10, qword _sc83h
    call r10
    ; call _sc83h

;.sc83h_exit:

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

; Restore the stack frame and exit to user mode
    push qword [.int131_cs]      ; cs
    push qword [.int131_rip]     ; rip
    iretq
.int131Ret: dq 0
.int131_cs: dq 0
.int131_rip: dq 0
;--    


; This is the system call.
; 0x80 - The system call 
; 0x81 - Auxiliary
; 0x82 - Auxiliary
; 0x83 - Auxiliary
align 8
global _system_call
_system_call:
    jmp RequestHall_int128


