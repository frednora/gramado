; headlib.s 
; Support for head.s
; tss, ldt, stacks.
; 2015 - Created by Fred Nora.

 
;
; Imported functions
;
 
extern _task0 

;
; Internal variables.
;

current:       dd 0 ;current.
scr_loc:       dd 0 ;Coisa da mem�ria de video.
bl_video_mode: db 0 
bl_lfb:        db 0  

;
;tss0
dd 0
tss0:
	dd 0                ;/* back link */
	dd stack0_krn_ptr   ;/* esp0, ss0 */
	dd 0x10
	dd 0, 0             ;/* esp1, ss1 */
	dd 0,0              ;/* esp2, ss2 */
	dd 0                ;/* cr3 */
	dd task0            ;/* eip */
	dd 0x200            ;/* eflags */
	dd 0, 0, 0, 0       ;/* eax, ecx, edx, ebx */
	dd stack0_ptr       ;/* esp, ebp, esi, edi */
	dd 0
	dd 0
	dd 0
	;/* es, cs, ss, ds, fs, gs */
	dd DATA_SEL, 0x20, DATA_SEL, DATA_SEL, DATA_SEL, DATA_SEL
	dd LDT0_SEL         ;/* ldt */
    dd  0x8000000       ;/* trace bitmap */

;
; tss1
dd 0
tss1: 
	dd 0                      ;/* back link */
	dd stack1_krn_ptr         ;/* esp0, ss0 */
	dd 0x10
	dd 0                      ;/* esp1, ss1 */
	dd 0
	dd 0                      ;/* esp2, ss2 */
	dd 0
	dd 0                      ;/* cr3 */
	dd task1                  ;/* eip */
	dd 0x200                  ;/* eflags */
	dd 0                      ;/* eax, ecx, edx, ebx */
	dd 0
	dd 0
	dd 0	        
	dd stack1_ptr             ;/* esp, ebp, esi, edi */ 
	dd 0
	dd 0
	dd 0
    ;/* es, cs, ss, ds, fs, gs */
	dd DATA_SEL, DATA_SEL, DATA_SEL, DATA_SEL, DATA_SEL, DATA_SEL
	dd LDT1_SEL              ;/* ldt */
    dd 0x8000000             ;/* trace bitmap */
;
;fim das tsss.
;

;
;ldt 0
dd 0
ldt0:	
	dq 0x0000000000000000  ;# null
	dq 0x00c0fa01000003ff  ;# 0x0f, base = 0x10000	
	dq 0x00c0f201000003ff  ;# 0x17
;
;ldt 1
dd 0
ldt1:	
	dq 0x0000000000000000  ;# null	 
	dq 0x00c0fa01000003ff  ;# 0x0f, base = 0x10000
    dq 0x00c0f201000003ff  ;# 0x17
	
;
; stacks in kernel mode
;


;
; Pilha principal.
;
    ;times 256 db 0
    ;times 256 db 0
stack_ptr:
    dd stack_ptr
    dw 0x10

;
; Pilha 0.
;
    ;times 256 db 0
    ;times 256 db 0
stack0_krn_ptr:
    dd 0

;
; Pilha 1.
;
    ;times 256 db 0
    ;times 256 db 0
stack1_krn_ptr:
    dd 0

;
; Pilha x.  
;
    ;times 256 db 0
    ;times 256 db 0
stack0_ptr:
    dd 0

;
; Pilha y.  
;
	;times 256 db 0
	;times 256 db 0
stack1_ptr:
    dd 0
	
	
;
; Pilha da task0.
;
	;times 256 db 0
	;times 256 db 0
global _task0_stack
_task0_stack:
    dd 0

;
; Pilha da task1. 
;
	;times 256 db 0
	;times 256 db 0
global _task1_stack
_task1_stack:
    dd 0
	
;
; Pilha da task2. 
;
	;times 256 db 0
	;times 256 db 0
global _task2_stack
_task2_stack:
    dd 0


; Tasks: 
; Obs: Boot Loader n�o usa tasks.
;      Acho que isso � usado apenas para configurar tss.


;#
;# Task 0, 1, 2.
task0:
task1:
;task2:
    nop ;Canceladas.
    jmp task1

;--------------------
; setup_gdt:
;     Carrega gdtr.

setup_gdt:
    lgdt [GDT_register]
    ret

;------------------------------
; setup_idt:
;     Configura a IDT.

setup_idt:
    pushad
    mov edx, unhandled_int 

	mov eax, dword 0x00080000    ;Selector = 0x0008 = cs.
	mov ax, dx                   ;Uma parte do endere�o.
	mov dx, word 0x8E00          ;Interrupt gate - dpl=0, present.
	mov edi, dword idt
    ;Nothing.
    mov ecx, dword 256
rp_sidt:
	mov dword [edi+0], eax
	mov dword [edi+4], edx

	add  edi, dword 8
	dec ecx
	jne rp_sidt
	;lidt [IDT_register]
	popad
	ret

;-------------------------------------
; _setup_idt_vector:
;     Configura um vetor da idt.
; IN:
; eax = ;endere�o.
; ebx = ;numero do vetor.

global _setup_idt_vector
_setup_idt_vector:

    cli
    pushad

    mov dword [.address], eax    ;Endere�o.
    mov dword [.number], ebx     ;N�mero do vetor.

	;calcula o deslocamaneto
	mov eax, dword 8
	mov ebx, dword [.number]
	mul ebx
	;resuldado em eax
	
	;adiciona o deslocamento � base.
	mov edi, idt               ;lea edi, idt
	add edi, eax
	
	mov edx, dword [.address] ;unhandled_int ;ignore_int       ;lea edx, ignore_int
	
	mov eax, dword 0x00080000  ;/* selector = 0x0008 = cs */
	mov ax, dx                 ;uma parte do endere�o
	mov dx, word 0x8E00        ;/* interrupt gate - dpl=0, present */
	
	;coloca o vetor na idt
	mov dword [edi+0], eax
	mov dword [edi+4], edx

	;recarrega a nova idt
	;lidt [IDT_register]
	
	popad
	sti
	ret
;Nothing.
.address: dd 0
.number: dd 0


;----------------------------------------------
; setup_faults:
;     Configura todos vetores da idt para faults.

setup_faults:
    
	push eax
	push ebx
	
	;#0  
	mov eax, dword _fault_N0
	mov ebx, dword 0
	call _setup_idt_vector	
	;#1  
	mov eax, dword _fault_N1
	mov ebx, dword 1
	call _setup_idt_vector	
	;#2  
	mov eax, dword _fault_N2
	mov ebx, dword 2
	call _setup_idt_vector	
	;#3  
	mov eax, dword _fault_N3
	mov ebx, dword 3
	call _setup_idt_vector	
	;#4  
	mov eax, dword _fault_N4
	mov ebx, dword 4
	call _setup_idt_vector	
	;#5  
	mov eax, dword _fault_N5
	mov ebx, dword 5
	call _setup_idt_vector	
	;#6 - Instru��o inv�lida.
	mov eax, dword _fault_INTRUCAO_INVALIDA
	mov ebx, dword 6
	call _setup_idt_vector
	;#7  
	mov eax, dword _fault_N7
	mov ebx, dword 7
	call _setup_idt_vector
	;#8 - double fault
	mov eax, dword _fault_DOUBLE
	mov ebx, dword 8
	call _setup_idt_vector	
	;#9  
	mov eax, dword _fault_N9
	mov ebx, dword 9
	call _setup_idt_vector
	;#10  
	mov eax, dword _fault_N10
	mov ebx, dword 10
	call _setup_idt_vector
	;#11  
	mov eax, dword _fault_N11
	mov ebx, dword 11
	call _setup_idt_vector
	;#12 - stack
	mov eax, dword _fault_STACK
	mov ebx, dword 12
	call _setup_idt_vector
	;#13 - general protection
	mov eax, dword _fault_GP
	mov ebx, dword 13
	call _setup_idt_vector	
	;#14  
	mov eax, dword _fault_N14
	mov ebx, dword 14
	call _setup_idt_vector	
	;#15 
	mov eax, dword _fault_N15
	mov ebx, dword 15
	call _setup_idt_vector	
	;#16 
	mov eax, dword _fault_N16
	mov ebx, dword 16
	call _setup_idt_vector	
	;#17  
	mov eax, dword _fault_N17
	mov ebx, dword 17
	call _setup_idt_vector	
	;#18  
	mov eax, dword _fault_N18
	mov ebx, dword 18
	call _setup_idt_vector	
	;#19  
	mov eax, dword _fault_N19
	mov ebx, dword 19
	call _setup_idt_vector	
	;#20  
	mov eax, dword _fault_N20
	mov ebx, dword 20
	call _setup_idt_vector	
	;#21  
	mov eax, dword _fault_N21
	mov ebx, dword 21
	call _setup_idt_vector	
	;#22  
	mov eax, dword _fault_N22
	mov ebx, dword 22
	call _setup_idt_vector	
	;#23 
	mov eax, dword _fault_N23
	mov ebx, dword 23
	call _setup_idt_vector	
	;#24  
	mov eax, dword _fault_N24
	mov ebx, dword 24
	call _setup_idt_vector	
	;#25  
	mov eax, dword _fault_N25
	mov ebx, dword 25
	call _setup_idt_vector	
	;#26 
	mov eax, dword _fault_N26
	mov ebx, dword 26
	call _setup_idt_vector	
	;#27  
	mov eax, dword _fault_N27
	mov ebx, dword 27
	call _setup_idt_vector	
	;#28 
	mov eax, dword _fault_N28
	mov ebx, dword 28
	call _setup_idt_vector	
	;#29  
	mov eax, dword _fault_N29
	mov ebx, dword 29
	call _setup_idt_vector	
	;#30  
	mov eax, dword _fault_N30
	mov ebx, dword 30
	call _setup_idt_vector	
	;#31 
	mov eax, dword _fault_N31
	mov ebx, dword 31
	call _setup_idt_vector	
	;Nothing.
.done:
	pop ebx
	pop eax
	ret

;---------------------------------
; setup_vectors:
;     Configura alguns vetores da idt.

setup_vectors:

	push eax
	push ebx 

;teclado
	mov eax, dword  _irq1
	mov ebx, dword 33
	call _setup_idt_vector

;int 200 - sistema
	mov eax, dword _int200
	mov ebx, dword 200
	call _setup_idt_vector

;213 - executa nova tarefa
	mov eax,  dword _int213
	mov ebx, dword 213 ;0xd5
	call _setup_idt_vector

;
; timer - 
; iniciamos um timer provis�rio, depois o Kemain() inicia o definitivo.
;
	;mov eax,  dword _irq0         ;ser� inicializado em c. 
	mov eax,  dword _timer_test    ;provis�rio.
	mov ebx, dword 32
	call _setup_idt_vector
	
	pop ebx
	pop eax
	ret

;---------------------
; set_base:
; in: 
;     eax = logic addr. 
;     ebx = base addr. 
;     ecx = table addr. 
;     edi = descriptors offset.

set_base:
	add eax, ebx
	add edi, ecx
	mov [edi+2], word ax
	
	ror eax, 16
	
	mov [edi + 4], al
	mov [edi +7],  ah
	ror eax, 16
	ret

;
; End.
;

 
