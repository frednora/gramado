;
; Arquivo: ldt32.inc
;
; Descrição:
;     LDT support.
;
; Versão 1.0, 2015.
;

;
;ldt 0
dd 0
ldt0:	
	dq 0x0000000000000000
	dq 0x00c0fa01000003ff
	dq 0x00c0f201000003ff
	
;
;ldt 1
dd 0
ldt1:	
	dq 0x0000000000000000
	dq 0x00c0fa01000003ff
    dq 0x00c0f201000003ff
	
;
;fim
;

