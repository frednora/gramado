;
; Arquivo: tss.inc 
;
; Descrição:
;     Para configuração de TSS feita pelo Boot Manager.
;


;
;tss0
; 


dd 0
dd 0	
tss0:
	dd 0   ;/* back link */
	dd stack0_krn_ptr ;/* esp0, ss0 */
	dd 0x10			
	dd 0, 0 ;/* esp1, ss1 */	
	dd 0,0 ;/* esp2, ss2 */			
	dd 0 	;/* cr3 */		
	dd task0 	;/* eip */		
	dd 0x200 ;/* eflags */	
	dd 0, 0, 0, 0 ;/* eax, ecx, edx, ebx */
	dd stack0_ptr ;/* esp, ebp, esi, edi */
	dd 0
	dd 0
	dd 0
	dd DATA_SEL, 0x20, DATA_SEL, DATA_SEL, DATA_SEL, DATA_SEL ;/* es, cs, ss, ds, fs, gs */	
	dd LDT0_SEL ;/* ldt */		
    dd  0x8000000 ;/* trace bitmap */
	
	

	
;
;tss1
; 
;.align 2
dd 0
dd 0
tss1: 			           
	dd 0 ; /* back link */
	
	;.long stack1_krn_ptr, 0x10	    ;     /* esp0, ss0 */
	dd stack1_krn_ptr
	dd 0x10
	
	;.long 0, 0			             ;    /* esp1, ss1 */
	dd 0
	dd 0
	
	;.long 0, 0			              ;   /* esp2, ss2 */
	dd 0
	dd 0
	
	;.long 0				               ;  /* cr3 */
	dd 0
	
	;.long task1			                ; /* eip */
	dd task1
	
	
	;.long 0x200			                ; /* eflags */
	dd 0x200
	
	;.long 0, 0, 0, 0		            ; /* eax, ecx, edx, ebx */
	dd 0
	dd 0
	dd 0
	dd 0
	
	;.long stack1_ptr, 0, 0, 0	        ; /* esp, ebp, esi, edi */
	dd stack1_ptr
	dd 0
	dd 0
	dd 0
	
	;.long 0x17,0x0f,0x17,0x17,0x17,0x17  ;/* es, cs, ss, ds, fs, gs */
	dd DATA_SEL, DATA_SEL, DATA_SEL, DATA_SEL, DATA_SEL, DATA_SEL
	
	;.long LDT1_SEL			             ;/* ldt */
	dd LDT1_SEL
	
	
	;.long 0x8000000			             ;/* trace bitmap */
    dd 0x8000000

	
;
;fim
;