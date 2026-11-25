; File: s2gdt.inc 
; This is the GDT for the Stage 2 of Gramado Boot Manager.
; 2015 - Created by Fred Nora.

;
; GDT
;
stage2_gdt:

; Null segment
stage2_NULL_SEL equ  $-stage2_gdt
    dw 0
    dw 0
    db 0
    db 0
    db 0
    db 0

; Code segment - (read)
stage2_CODE_SEL equ  $-stage2_gdt
	dw 0xFFFF
	dw 0
	db 0 
	db 0x9A                  
	db 0xCF
	db 0

; Data segment - (read and write)
stage2_DATA_SEL equ  $-stage2_gdt
	dw 0xFFFF
	dw 0
	db 0 
	db 0x92                   
	db 0xCF
	db 0

; More segments
    dq 0x00c0920b80000002  ; screen 0x18 - for display in text mode. 
	dq 0x0000e90100000068  ; TSS0 descr 0x20
	dq 0x0000e20100000040  ; LDT0 descr 0x28
	dq 0x0000e90100000068  ; TSS1 descr 0x30
    dq 0x0000e20100000040  ; ?   
	
stage2_gdt_end:
nop
stage2_gdtr:
    dw stage2_gdt_end - stage2_gdt -1  ; Size
    dd stage2_gdt                      ; PA for GDT


; -------------------------------------------------------
; IVT pointer
; We're gonna use this where returning to real mode
; It starts at 0000:0000
dw 0
dw 0	
ivtr_rm:
	dw (256*4 -1)  ; Size
	dd 0           ; Address

;
; End
;	
