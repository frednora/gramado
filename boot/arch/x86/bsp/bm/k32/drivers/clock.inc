;
; File: drivers/clock.inc
;
; Descrição:
;     Suporte ao relógio do sistema.
;
; HistORY:
;     2015 - Created.
;     2017 - Revision.
;


;=============================================
; _irq8:
;     Clock irq handler.
;

_irq8: 
	PUSH AX
	MOV AL, 020h
	OUT 0A0h, AL
	OUT 020h, AL
	POP AX
	IRETD



;===============================================	
; clockInit:
;     Clock initialization.
;
clockInit:
    ;nop  ;;Nothing for now.
    ret


;
; End.
;

