
Here we're dealing with the situation where 
we need to handle a stackframe with 5 or 3 values.

;===========================================================
; IRQ0 SAVE ROUTINE
;
; This routine is entered immediately after an interrupt.
; Depending on the privilege level, the CPU pushes on the
; hardware stack either:
;   • User (ring 3): 5 values: RIP, CS, RFLAGS, RSP, SS 
;   • Kernel (ring 0): 3 values: RIP, CS, RFLAGS
;
; We check the saved CS (at offset +8 from RSP) to obtain 
; the CPL, then pop the correct number of values into our 
; context storage.
;===========================================================
section .text
global _irq0_save
_irq0_save:
    cli                         ; Disable further interrupts

    ;----------------------------------------
    ; Determine the frame layout by checking the saved CS.
    ; Regardless of the frame, the value at [rsp+8] holds
    ; the saved CS. Its lower two bits are the CPL.
    ;----------------------------------------
    mov rax, [rsp+8]            ; Fetch saved CS (offset +8)
    and ax, 3                   ; Isolate the CPL bits
    mov qword [_contextCPL], rax

    cmp rax, 3
    je .save_user_mode         ; If CPL==3, then it’s a user-mode frame

    ;----------------------------------------
    ; Kernel mode path: hardware pushed only 3 values.
    ; Stack (from current [rsp]):
    ;   [rsp + 0]   : Saved RIP
    ;   [rsp + 8]   : Saved CS
    ;   [rsp + 16]  : Saved RFLAGS
    ; (No RSP or SS are pushed)
    ;----------------------------------------
    pop qword [_contextRIP]     ; Pop RIP
    pop qword [_contextCS]      ; Pop CS
    pop qword [_contextRFLAGS]  ; Pop RFLAGS
    ; For kernel-mode interrupts, set these to a known value.
    mov qword [_contextRSP], 0
    mov qword [_contextSS], 0
    jmp .save_regs

.save_user_mode:
    ;----------------------------------------
    ; User mode path: hardware pushed 5 values.
    ; Stack layout (from current [rsp]): 
    ;   [rsp + 0]   : Saved RIP
    ;   [rsp + 8]   : Saved CS
    ;   [rsp + 16]  : Saved RFLAGS
    ;   [rsp + 24]  : Saved RSP   (user-mode stack pointer)
    ;   [rsp + 32]  : Saved SS    (user-mode stack segment)
    ;----------------------------------------
    pop qword [_contextRIP]     ; Pop RIP
    pop qword [_contextCS]      ; Pop CS
    pop qword [_contextRFLAGS]  ; Pop RFLAGS
    pop qword [_contextRSP]     ; Pop RSP
    pop qword [_contextSS]      ; Pop SS

.save_regs:
    ;----------------------------------------
    ; Save additional registers.
    ; (Ensure that registers used for CPL-checking are saved,
    ;  if needed, by first pushing them onto your context area.)
    ;----------------------------------------
    mov qword [_contextRAX], rax   ; Note: rax already holds the CPL value
    mov qword [_contextRBX], rbx
    mov qword [_contextRCX], rcx
    mov qword [_contextRDX], rdx
    mov qword [_contextRSI], rsi
    mov qword [_contextRDI], rdi
    mov qword [_contextRBP], rbp
    mov qword [_contextR8],  r8
    mov qword [_contextR9],  r9
    mov qword [_contextR10], r10
    mov qword [_contextR11], r11
    mov qword [_contextR12], r12
    mov qword [_contextR13], r13
    mov qword [_contextR14], r14
    mov qword [_contextR15], r15

    ; Save segment registers.
    xor rax, rax
    mov ax, ds
    mov word [_contextDS], ax
    mov ax, es
    mov word [_contextES], ax
    mov ax, fs
    mov word [_contextFS], ax
    mov ax, gs
    mov word [_contextGS], ax

    ; Save FPU/SSE state.
    fxsave [_context_fpu_buffer]

    ;-----------------------------------------------------
    ; Continue with additional interrupt processing such as
    ; calling a timer or task switching routine.
    ; For example: call _irq0_TIMER
    ;-----------------------------------------------------
    ; (Your additional code goes here.)

    ; For demonstration, jump directly to the release routine.
    jmp _irq0_release

;===========================================================
; IRQ0 RELEASE (RESTORE) ROUTINE
;
; This routine rebuilds the hardware-saved context on the stack
; before executing iretq. It uses the previously saved value of
; _contextCPL to determine whether to push a 5-value user-mode
; frame or a 3-value kernel-mode frame.
;===========================================================
global _irq0_release
_irq0_release:
    ;----------------------------------------
    ; (Optional code: e.g., ring-3 callback, CR3 refresh,
    ;  sending End-of-Interrupt (EOI) to the PIC, etc.)
    ;----------------------------------------
    mov rax, qword [_asmflagDoCallbackAfterCR3]
    cmp rax, 0x1234
    je __doRing3Callback

    ; Refresh CR3 if necessary.
    mov rax, cr3
    IODELAY 
    mov cr3, rax  

    ; Send PIC EOI.
    mov al, 0x20
    out 0x20, al

    ;----------------------------------------
    ; Restore segment registers.
    ;----------------------------------------
    xor rax, rax
    mov ax, word [_contextDS]
    mov ds, ax
    mov ax, word [_contextES]
    mov es, ax
    mov ax, word [_contextFS]
    mov fs, ax
    mov ax, word [_contextGS]
    mov gs, ax

    ;----------------------------------------
    ; Restore general-purpose registers.
    ;----------------------------------------
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

    ; Restore FPU/SSE state.
    fxrstor [_context_fpu_buffer]

    ;----------------------------------------
    ; Now rebuild the hardware-saved stack frame.
    ; The appropriate frame is chosen based on _contextCPL.
    ;----------------------------------------
    mov rax, qword [_contextCPL]
    cmp rax, 3
    je .restore_user_mode

    ; Kernel mode restore path: push 3 values.
    push qword [_contextRFLAGS]   ; rflags
    push qword [_contextCS]       ; cs
    push qword [_contextRIP]      ; rip
    jmp .restore_done

.restore_user_mode:
    ; User mode restore path: push 5 values.
    push qword [_contextSS]       ; ss
    push qword [_contextRSP]      ; rsp
    push qword [_contextRFLAGS]   ; rflags
    push qword [_contextCS]       ; cs
    push qword [_contextRIP]      ; rip

.restore_done:
    ; Re-enable interrupts (if they aren’t already enabled in the saved flags).
    sti
    iretq
