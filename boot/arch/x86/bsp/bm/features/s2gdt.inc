;
; File: s2gdt.inc 
;
;     This is the GDT for the Stage 2 of Gramado Boot Manager.
;
; Version: 1.0, 2015.
;


;
;GDT.
;
stage2_gdt:

;nulo
stage2_NULL_SEL equ $-stage2_gdt
    dw 0
    dw 0
    db 0
    db 0
    db 0
    db 0
	
;segmento de código - (leitura)
stage2_CODE_SEL equ $-stage2_gdt
	dw 0xFFFF
	dw 0
	db 0 
	db 0x9A                  
	db 0xCF
	db 0
	
;segmento de dados - (leitura e escrita)
stage2_DATA_SEL equ $-stage2_gdt
	dw 0xFFFF
	dw 0
	db 0 
	db 0x92                   
	db 0xCF
	db 0

;;
;; e os de 16 bit??
;;

;Outros.	
    dq 0x00c0920b80000002    ;screen 0x18 - for display in text mode. 
	dq 0x0000e90100000068    ;TSS0 descr 0x20
	dq 0x0000e20100000040    ;LDT0 descr 0x28
	dq 0x0000e90100000068    ;TSS1 descr 0x30
    dq 0x0000e20100000040    ;??   
	
stage2_gdt_end:
nop
stage2_gdtr:
    dw stage2_gdt_end - stage2_gdt -1  ;limit.(TAMANHO)
    dd stage2_gdt                      ;endereço físico da GDT.
	
	
;;
;; Algum suporte para voltar para o modo real.
;;
	
;----------------------------------------------------------
; Para voltar para o modo real  esse é o registro da IVT.
dw 0
dw 0	
ivtr_rm:
	dw (256*4-1)		;;# idt contains 256 entries
	dd 0 ; t 0000:0000 inicio
	
	
	
	
;
;fim.
;	
