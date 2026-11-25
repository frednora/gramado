;
; Arquivo: ints.inc
; Descrição:
;     Rotinas de tratamento das interrupções.
;     Boot Manager para o BOOT.VHD project.
;     2015 - Fred Nora 


;-----------------------------
; test_int30h:
;     Handler para a interrupção do sistema.

test_int30h:
	
	push ax
	mov al, byte "i"	
	call write_char	
	
	;mov al, byte "3"	
	;call write_char	
	
	;mov al, byte "0"	
	;call write_char	
	
	;mov al, byte " "	
	;call write_char
    pop ax
    iretd
	
	
;---------------------------------
; system_interrupt:
;     System interrupt handler.
;     Opcional.

system_interrupt:
	push ds
	push edx
	push ecx
	push ebx
	push eax
	
	mov ax, word 0x10
	mov ds, ax
	
	call write_char
	
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop ds
	iret

;-------------------------------------------
; unhandled_int e unhandled_irq:
;     Handler para Irqs ou Interrupções
;     negligenciadas.

unhandled_int:
unhandled_irq:
    pushad

    ;Debug.
    ;mov al, byte "I"	
    ;call write_char

    push ax
    mov al, 0x20
    out 0x20, al
    pop ax
    
    popad
    IRETD

;---------------------------------------------
; ignore_int:  
;     Handler para interrupções
;     negligenciadas.

ignore_int:
	push ds
	push eax
	
	mov ax, word 0x10
	mov ds, ax
	
	mov eax, dword 67    
	call write_char
	
	pop eax
	pop ds
	IRETD


;----------------------
; disable_irqs:
;     Disable all IRQs.   
; #todo: new name: Mask all interrupts.

disable_irqs:
    push ax 
    mov	al, 0xFF
    out 0x21, al
    out 0xA1, al
    pop ax
    ret

;---------------------
; enable_irqs:
;     Enable all IRQs.   
; #todo: new name: Unmask all interrupts.

enable_irqs:
    push ax
    mov al, 0x00
    out 0x21, al
    out 0xA1, al
    pop ax
    ret

;-----------------------------
; enable_irq:
;     Enable an IRQ.  cl = irq.

enable_irq:

    push ax
	push cx

	cmp cl, 8
	jb .master

	sub cl, 8
	mov ah, 1
	shl ah, cl
	xor ah, 0xFF

	in al, 0xA1
	and al, ah
	out 0xA1, al

	pop	cx
	pop ax
	ret
.master:
	mov	ah, 1
	shl	ah, cl
	xor	ah, 0xFF

	in al, 0x21
	and al, ah
	out 0x21, al

	pop	cx
	pop	ax
	RET

;------------------------------
; disable_irq:
;     Disable an IRQ.  cl = irq.

disable_irq:
	push ax
	push cx

	cmp cl, 8
	jb .master

	sub cl, 8
	mov ah, 1
	shl ah, cl

	in al, 0xA1
	or al, ah
	out 0xA1, al

	pop	  cx
	pop	  ax
	ret
.master:
    mov ah, 1
	shl	ah, cl

    in al, 0x21
    or al, ah
    out 0x21, al

	pop cx
	pop ax
	RET
	
	
SendEOI:
	push eax
	mov al, 0x20    ;EOI Signal.
	out 0x20, al    ;Send it!
	pop eax
	ret

SendEOI2:
	push eax
	mov al, 0x20    ;EOI Signal.
	out 0xa0, al	;Send it!
	pop eax
	ret

	

;unmask_all_irqs: 
  ;mov  al,0                                        
  ;out  0xA1,al 
  ;out  0x21,al 
  ;sti 
  ;ret 

;----------------------------------------
; int_113h:
;     int 113h, Funções de disco.
;     (converter lba > chs .. usar bpb32).

int_113h:
	push eax
	push ebx
	mov eax, dword [.buffer]    ;Buffer.
	mov ebx, dword 0            ;LBA.
	call hd0_read_sector
	pop ebx
	pop eax
    IRETD
.buffer: 
    dd 0

;
; End
;
