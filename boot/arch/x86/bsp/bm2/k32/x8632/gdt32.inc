;
; File: gdt.inc
; Description:
;   Global Descriptor Table (GDT) for the Boot Manager.
;   Provides:
;     - Null descriptor
;     - 32-bit flat code and data segments (ring 0)
;     - 16-bit protected-mode segments for the real-mode trampoline
;     - Additional system descriptors (text mode screen, TSS, LDT)
;   2015 - Fred Nora
;

;
; GDT
;

gdt:
; Null descriptor (selector 0 is always invalid)
NULL_SEL equ $-gdt
    dd 0
    dd 0
; 32-bit code segment (ring 0), base=0, limit=4GB, readable
; Access 0x9A: present=1, DPL=0, code=1, readable=1
; Flags  0xCF: granularity=4KB (G=1), 32-bit (D=1), limit high=0xF
CODE_SEL equ $-gdt
	dw 0xFFFF
	dw 0
	db 0 
	db 0x9A  ;present, ring0, code, non-confirming, readble.
	db 0xCF
	db 0
; 32-bit data segment (ring 0), base=0, limit=4GB, writable
; Access 0x92: present=1, DPL=0, data=1, writable=1
; Flags  0xCF: granularity=4KB (G=1), 32-bit (D=1), limit high=0xF
DATA_SEL equ $-gdt
	dw 0xFFFF
	dw 0
	db 0 
	db 0x92 ;present, ring0, data, expanded up, writeble. (BITS)
	db 0xCF
	db 0

; 16-bit protected-mode code segment (for real-mode return trampoline)
; Access 0x9A: present=1, DPL=0, code=1, readable=1
; Flags: D=0 (16-bit), G=0 (byte granularity), limit=0xFFFF
RM_Code_Sel equ $-gdt
   dw   0xFFFF
   dw   0x0000
   dw   0x9A00
   dw   0x0000
; 16-bit protected-mode data segment (for real-mode return trampoline)
; Access 0x92: present=1, DPL=0, data=1, writable=1
; Flags: D=0 (16-bit), G=0 (byte granularity), limit=0xFFFF
RM_Data_Sel   equ $-gdt
   dw 0xFFFF
   dw 0x0000
   dw 0x9200
   dw 0x0000
   
; Additional system descriptors
; - Text mode screen (B8000h) as a data segment (ring 0, read/write)
; - TSS/LDT descriptors (sizes and bases are encoded in the quadwords)
; Note: These are already pre-encoded as 8-byte descriptor values.
    dq 0x00c0920b80000002  ; screen 0x18 - for display in text mode. 
    dq 0x0000e90100000068  ; TSS0 descr 0x20
    dq 0x0000e20100000040  ; LDT0 descr 0x28
    dq 0x0000e90100000068  ; TSS1 descr 0x30
    dq 0x0000e20100000040  ; ?
end_gdt:
    dd 0    ;Separador.
; -----------------------------------------------------------------------------
; GDTR setup (limit and base), for use with LGDT
; -----------------------------------------------------------------------------
lgdt_opcode:
    dw (end_gdt-gdt)-1  ; GDT size in bytes minus one
    dd gdt              ; Linear base address of GDT

;
; End
;
