;
; Arquivo: gdt.inc
; Descrição:
;     A GDT do Boot Manager.
;     2015 - Fred Nora

;
; GDT.
;

gdt:
;Nulo.
NULL_SEL equ $-gdt
    dd 0
    dd 0
;Segmento de código, Só pode ler.
CODE_SEL equ $-gdt
	dw 0xFFFF
	dw 0
	db 0 
	db 0x9A  ;present, ring0, code, non-confirming, readble.
	db 0xCF
	db 0
;Segmento de dados.
DATA_SEL equ $-gdt
	dw 0xFFFF
	dw 0
	db 0 
	db 0x92 ;present, ring0, data, expanded up, writeble. (BITS)
	db 0xCF
	db 0

; modoprotegido de 16bits
RM_Code_Sel equ $-gdt
   dw   0xFFFF
   dw   0x0000
   dw   0x9A00
   dw   0x0000
; modoprotegido de 16bits
RM_Data_Sel   equ $-gdt
   dw 0xFFFF
   dw 0x0000
   dw 0x9200
   dw 0x0000
   
;Outros.
    dq 0x00c0920b80000002  ; screen 0x18 - for display in text mode. 
    dq 0x0000e90100000068  ; TSS0 descr 0x20
    dq 0x0000e20100000040  ; LDT0 descr 0x28
    dq 0x0000e90100000068  ; TSS1 descr 0x30
    dq 0x0000e20100000040  ; ?
end_gdt:
    dd 0    ;Separador.
lgdt_opcode:
    dw (end_gdt-gdt)-1
    dd gdt 

;
; End
;

