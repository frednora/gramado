;
; File: drivers\timer.inc
;
; Descrição:
;     PIT.
;     Rotinas de gerenciamento de tempo para o Boot Manager.
;
;  Obs:
;      O Boot Manager não faz task switch.
;      A rotina aqui deve ser mínima.
;
; Histórico:
;     2015 - Created.
;     2017 - Revision.
;



;-------------------------------------------
; __irq0:
;     Irq 0, Irq handler for Timer interrupt.
;
global __irq0
__irq0:
	pushad
    push ax
	mov al, 0x20
    out 0x20, al
    pop ax
    popad
	IRETD
	
	
;------------------
; _irq0:
;     Irq handler opcional.
;	
global _irq0
_irq0:
	jmp __irq0
    jmp $
  	
;-----------------------
; timer_provisorio:
;     Irq handler opcional.
;

timer_provisorio:
	jmp __irq0
    jmp $


;------------------------------------
; timer_interrupt:
;     Timer interrupt handler.
;     Opcional.
;
timer_interrupt:
	jmp __irq0
    jmp $
	
	
;===================================================
; timerInit:
;     Timer initialization.	
;
timerInit:
    nop    ;Nothing for now.
	ret

	
;
;fim
;