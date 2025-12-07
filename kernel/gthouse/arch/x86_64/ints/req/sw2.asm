; Request Hall (White)
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
; Flag for the state of callback restorer
extern _callback_restorer_done

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
sw2_initialize_syscall_support:

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

; Call the worker to unset the callback_in_progress flag 
; into the thread structure.
extern _callback_is_restored

; Callbacl restorer.
; int 198 (0xC6)
align 4  
global RequestHall_int198
RequestHall_int198:

; #suspended
;    iretq

; Drop the useless stack frame.
; We were in the middle of the timer interrupt,
; so, we're gonna use the saved context to release the next thread.

; Drop the software-int frame (ring3 -> ring0 transition)
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

; One-shot: ensure callback state is cleared
    mov qword [_asmflagDoCallbackAfterCR3], 0

; The context for the function (reset for the next callback)
    mov qword [_ring3_callback_address], 0
    mov qword [_ring3_callback_parm1], 0
    mov qword [_ring3_callback_parm2], 0
    mov qword [_ring3_callback_parm3], 0
    mov qword [_ring3_callback_parm4], 0

    mov qword [_callback_restorer_done], 1

; Call the worker to unset the callback_in_progress flag 
; into the thread structure.
; See: callback.c
    call _callback_is_restored

; #bugbug:
; Here we're using the release routine that belongs to the irq0
; to return from the callback restorer's interrupt. It happens because,
; the last saved context is the context of the thread that is using 
; the callback. But we need to handle this situation in a better way.

; Normal timer exit. (after cr3).
; temos que terminal a rotina do timer e
; retornarmos para ring 3 com o contexto o ultimo contexto salvo.
; que ainda é o window server.

; Resume via the common timer release path
    jmp irq0_release


; ------------------------------------------------------------
; RequestHall_int199 handler – "first interrupt enable"
;
; Context:
; - When the kernel finishes initialization, it jumps to the
;   first user process (init) using iretq.
; - At that moment, interrupts are still disabled (IF=0 in RFLAGS).
; - Without interrupts, the scheduler cannot run and no task
;   switching will happen.
;
; Purpose:
; - This handler is called once by the init process in user mode.
; - It modifies the saved RFLAGS so that IF=1 (interrupts enabled).
; - It also sets IOPL=0, which prevents user programs (ring 3)
;   from executing privileged instructions like CLI/STI or IN/OUT.
; - After pushing the corrected frame and executing iretq,
;   the CPU resumes user mode with interrupts enabled.
;
; Result:
; - From this point on, hardware timer interrupts can fire.
; - The kernel’s scheduler can preempt tasks and switch between them.
; - Other user processes do NOT need to call this handler; only
;   the init process uses it to "unlock" multitasking.
; ------------------------------------------------------------
align 4  
; 0xC7
global RequestHall_int199
RequestHall_int199:
    jmp miC7H
    jmp $
miC7H:
; Maskable interrupt
    pop qword [.frameRIP]
    pop qword [.frameCS]
    pop qword [.frameRFLAGS]
; iopl 0
; This sets bit 9 (IF) = 1, enabling maskable interrupts.
; Bits 12–13 (IOPL) are set to 0 here, meaning 
; only CPL=0 code can execute in/out/cli/sti. 
; That’s good for security — user mode (ring 3) won’t be able 
; to mess with hardware directly.
    mov qword [.frameRFLAGS], 0x0000000000000200

; iopl 3
; Comment out the alternative 0x0000000000003200, 
; which would set IOPL=3, allowing user mode to do I/O 
; not recommended unless you’re deliberately experimenting.
    ;mov qword [.frameRFLAGS], 0x0000000000003200

    push qword [.frameRFLAGS]
    push qword [.frameCS]
    push qword [.frameRIP]
    iretq
.frameRIP:     dq 0
.frameCS:      dq 0
.frameRFLAGS:  dq 0

