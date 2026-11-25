; hw.inc
; Handlers for hw interrupts.
; Handlers for faults and exceptions.
; 2015 - Created b Fred Nora.


;
; Imported functions.
;

extern _keyboardHandler
extern _cpu_faults


;-------------------------------------
; unhandled_irq:
; Generic hardware interrupt handler.
;   
unhandled_irq:
    pushad  
    ;Nothing.
    inc byte [.c]
    mov al, byte [.c]
    mov byte [0x000B8000], al
    ;Nothing.
    mov al, 0x20
    out 0x20, al
    out 0xA0, al 
    ;Nothing. 
    popad
    iretd
;Nothing.
.c: db 0

;----------------------------
; _irq0:
;     Irq0 - Timer. (handler).

extern _timer
global _irq0
_irq0:
    ;Obs: Aqui ser� implementado algum tipo de contagem.
    ;Mas a troca de contexto n�o ser� implementada.
    ;cli
    mov al, 20h
    out 20h, al
    sti
    iretd  

;------------------------------------------------------------------
; _timer_test:
;     Timer sem multitarefas. Usado para teste.
;     Obs: A troca de contexto n�o ser� implementada no Boot Loader.

global _timer_test
_timer_test:
    jmp _irq0
    jmp $

;------------------------------------
; timer_interrupt:
;     Outra interrup��o de timer.

timer_interrupt:
    jmp _irq0
    jmp $

;----------------------------------
; _irq1:
;     Keyboard interrupt.

global _irq1  
_irq1:

    ;Debug:
    ;mov al, byte "k"
    ;mov byte [0xb8002], al 

    call _keyboardHandler
    iretd

;------------------------
; _irq14:
;     Tratador de interrup��es para unidade master.
;@input: -
;@output: -

_irq14:
	PUSH AX
	MOV AL, 020h
	OUT 0A0h, AL
	IODELAY
	OUT 020h, AL
	IODELAY
	POP AX
	IRETD

;------------------------------
; _irq15:
;    Tratador de interrup��es para unidade slave.
;@input: -
;@output: -

_irq15: 
	PUSH AX
	MOV AL, 020h
	OUT 0A0h, AL
	IODELAY
	OUT 020h, AL
	IODELAY
	POP AX
	IRETD

;--------------------------------------
; ## FAULTS ## 
; Tratamento das faltas e excess�es.

;int 0. 
global _fault_N0
_fault_N0:
    push dword 0
    call _cpu_faults
    hlt
    jmp $

;int 1. 
global _fault_N1
_fault_N1:
    push dword 1
    call _cpu_faults
    hlt
    jmp $

;int 2.
global _fault_N2
_fault_N2:
    push dword 2
    call _cpu_faults
    hlt
    jmp $

;int 3.
global _fault_N3
_fault_N3:
    push dword 3
    call _cpu_faults
    hlt
    jmp $

;int 4.
global _fault_N4
_fault_N4:
    push dword 4
    call _cpu_faults
    hlt
    jmp $

;int 5.
global _fault_N5
_fault_N5:
    push dword 5
    call _cpu_faults
    hlt
    jmp $

;int 6 - Instru��o inv�lida.
global _fault_INTRUCAO_INVALIDA
_fault_INTRUCAO_INVALIDA:
    push dword 6
    call _cpu_faults
    hlt
    jmp $

;int 7.
global _fault_N7
_fault_N7:
    push dword 7
    call _cpu_faults
    hlt
    jmp $

;int 8 - double fault.
global _fault_DOUBLE
_fault_DOUBLE:
    push dword 8
    call _cpu_faults
    hlt
    jmp $

;int 9.
global _fault_N9
_fault_N9:
    push dword 9
    call _cpu_faults
    hlt
    jmp $

;int 10.
global _fault_N10
_fault_N10:
    push dword 10
    call _cpu_faults
    hlt
    jmp $

;int 11.
global _fault_N11
_fault_N11:
    push dword 11
    call _cpu_faults
    hlt
    jmp $

;int 12 - Falha de pilha (interrup��o 12).
global _fault_STACK
_fault_STACK:
    push dword 12
    call _cpu_faults
    hlt
    jmp $

;int 13 - general protection fault (GPF).
global _fault_GP
_fault_GP:   
    push dword 13
    call _cpu_faults
    hlt
    jmp $

;int 14.
global _fault_N14
_fault_N14:
    push dword 14
    call _cpu_faults
    hlt
    jmp $

;int 15.
global _fault_N15
_fault_N15:
    push dword 15
    call _cpu_faults
    hlt
    jmp $

;int 16.
global _fault_N16
_fault_N16:
    push dword 16
    call _cpu_faults
    hlt
    jmp $

;int 17.
global _fault_N17
_fault_N17:
    push dword 17
    call _cpu_faults
    hlt
    jmp $

;int 18.
global _fault_N18
_fault_N18:
    push dword 18
    call _cpu_faults
    hlt
    jmp $

;int 19.
global _fault_N19
_fault_N19:
    push dword 19
    call _cpu_faults
    hlt
    jmp $

;int 20.
global _fault_N20
_fault_N20:
    push dword 20
    call _cpu_faults
    hlt
    jmp $

;int 21.
global _fault_N21
_fault_N21:
    push dword 21
    call _cpu_faults
    hlt
    jmp $

;int 22.
global _fault_N22
_fault_N22:
    push dword 22
    call _cpu_faults
    hlt
    jmp $

;int 23.
global _fault_N23
_fault_N23:
    push dword 23
    call _cpu_faults
    hlt
    jmp $

;int 24.
global _fault_N24
_fault_N24:
    push dword 24
    call _cpu_faults
    hlt
    jmp $

;int 25.
global _fault_N25
_fault_N25:
    push dword 25
    call _cpu_faults
    hlt
    jmp $

;int 26.
global _fault_N26
_fault_N26:
    push dword 26
    call _cpu_faults
    hlt
    jmp $

;int 27.
global _fault_N27
_fault_N27:
    push dword 27
    call _cpu_faults
    hlt
    jmp $

;int 28.
global _fault_N28
_fault_N28:
    push dword 28
    call _cpu_faults
    hlt
    jmp $

;int 29.
global _fault_N29
_fault_N29:
    push dword 29
    call _cpu_faults
    hlt
    jmp $

;int 30.
global _fault_N30
_fault_N30:
    push dword 30
    call _cpu_faults
    hlt
    jmp $

;int 31.
global _fault_N31
_fault_N31:
    push dword 31
    call _cpu_faults
    hlt
    jmp $

;
; End.
;


