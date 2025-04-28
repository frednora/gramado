; sw2.asm
; Software interrupts support.
; Created by Fred Nora.


;
; Callback support.
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

; Normal timer exit. (after cr3).
; temos que terminal a rotina do timer e
; retornarmos para ring 3 com o contexto o ultimo contexto salvo.
; que ainda é o window server.
    jmp irq0_release


