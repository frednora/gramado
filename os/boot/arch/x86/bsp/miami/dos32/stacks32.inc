; Arquivo: stacks32.inc
; Descri��o:
;     Pilhas para o Boot Manager.
; 2015, 2016 - Created by Fred Nora


;----------------------------------
; Pilha gen�rica.
;
    times 256 db 0
	times 256 db 0
stack_ptr:
    dd stack_ptr
	dw 0x10

	
;----------------------------------
; Pilha 0.
;
    times 256 db 0
	times 256 db 0
stack0_krn_ptr:
    dd 0
	

;----------------------------------
; Pilha 1.
;	
    times 256 db 0
	times 256 db 0	
stack1_krn_ptr:
    dd 0		
	
	
;-----------------------------------
; Pilha da tarefa 0. 
;    (@todo: N�o � necess�rio no Boot Manager. Deletar isso)
;
    times 256 db 0
    times 256 db 0
stack0_ptr:
	dd 0
	

;--------------------------------------
; Pilha da tarefa 1. 
;     (@todo: N�o � necess�rio no Boot Manager. Deletar isso)
;
	times 256 db 0
	times 256 db 0
stack1_ptr:
    dd 0
	
	
;-------------------------------------------
; Pilha da tarefa 0. 
;     (@todo: N�o � necess�rio no Boot Manager. Deletar isso)
;
	;times 256 db 0
	;times 256 db 0
global _task0_stack
_task0_stack:
    dd 0	
	
	
;--------------------------------------------
; Pilha da tarefa 1. 
;     (@todo: N�o � necess�rio no Boot Manager. Deletar isso)
;
	;times 256 db 0
	;times 256 db 0
global _task1_stack
_task1_stack:
    dd 0
	
	
;-----------------------------------------------
; Pilha da tarefa 2. 
;     (@todo: N�o � necess�rio no Boot Manager. Deletar isso)
;
	;times 256 db 0
	;times 256 db 0
global _task2_stack
_task2_stack:
    dd 0

;
; End
;
