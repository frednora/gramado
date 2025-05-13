; sw2.asm
; Software interrupts support.
; Created by Fred Nora.


;
; Callback support.
; See: callback.c, callback.h
;

; Flag
extern _asmflagDoCallbackAfterCR3
; Address
extern _ring3_callback_address



;; ==============================================
;; unhandled interrupts
;; We use EOI only for IRQs.
;; called by setup_idt in headlib.asm
align 4  
unhandled_int:
; #bugbug: 
; Talvez uma irq esta chamando isso e precisa de eoi.
; #todo: 
; Criar um contador para isso.
    iretq


align 4
global _syscall_handler
_syscall_handler:
    ; Save user-space return address (RIP) and stack pointer (RSP)
    mov qword [.SavedRIP], rcx  ; Save user-space RIP
    mov qword [.SavedRSP], r11  ; Save user-space RSP

    ; Do something (handle syscall)

    ; Restore user-space values before returning
    mov rcx, qword [.SavedRIP]  ; Restore user-space RIP
    mov rsp, qword [.SavedRSP]  ; Restore user-space RSP
    sysret                      ; Return to user mode

; Storage for saved values
.SavedRIP: dq 0
.SavedRSP: dq 0


; Initializing the syscall support for x86_64 machines.
; Writing to MSRs (Model-Specific Registers)
; Before using syscall, configure the entry point for system calls:
; Called by head_64.asm.
sw2_initialize_syscall_support;

; #todo
; This is not working yet. We got fault number 6.
; This is a work in progress.

; Check CPU Mode
    ;mov eax, 0x80000001
    ;cpuid
    ;test edx, (1 << 11)  ; Check syscall support
    ;jz no_syscall_support
; Verify MSR Values
    ;mov ecx, 0xC0000082
    ;rdmsr

    ;mov ecx, 0xC0000082  ; IA32_LSTAR (syscall entry point)
    ;mov rax, _syscall_handler  ; Address of syscall handler
    ;mov rdx, 0
    ;wrmsr

    ;mov ecx, 0xC0000084  ; IA32_FMASK (mask for syscall flags)
    ;mov rax, 0x3F        ; Disable interrupts during syscall
    ;mov rdx, 0
    ;wrmsr

    ret


; -------------------------------------
; callback restorer.
; temos que terminal a rotina do timer e
; retornarmos para ring 3 com o contexto o ultimo contexto salvo.
; #bugbug
; We gotta check what is the process calling this routine.
; ====================================
; int 198
; Restorer.
; Used to return from ring 3 to ring0
; when the kernel calls a procedure in ring 3.
; It can be a signal or a window procedure.
; The purpose here is jumping to a c routine,
; restore the context with the idle thread
; and restart the ifle thread.

; Callbacl restorer.
; int 198 (0xC6)
global _int198
_int198:

; Drop the useless stack frame.
; We were in the middle of the timer interrupt,
; so, we're gonna use the saved context to release the next thread.

    pop rax  ; rip
    pop rax  ; cs
    pop rax  ; rflags
    pop rax  ; rsp
    pop rax  ; ss

; #bugbug
; We gotta check what is the process calling this routine.
     ;call __xxxxCheckCallerPID

; Clear the variables.
; Clear the flag and the procedure address.
; Desse jeito a rotina de saida não tentará
; chamar o callback novamente.
    mov qword [_asmflagDoCallbackAfterCR3], 0
    mov qword [_ring3_callback_address], 0

; #bugbug:
; Here we're using the release routine that belongs to the irq0
; to return from the callback restorer's interrupt. It happens because,
; the last saved context is the context of the thread that is using 
; the callback. But we need to handle this situation in a better way.

; Normal timer exit. (after cr3).
; temos que terminal a rotina do timer e
; retornarmos para ring 3 com o contexto o ultimo contexto salvo.
; que ainda é o window server.
    jmp irq0_release


