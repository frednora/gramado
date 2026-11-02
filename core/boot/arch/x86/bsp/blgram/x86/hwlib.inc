; hwlib.inc
; Support for hw.inc
; 2015 - Created by Fred Nora.

; IDE ports:
; =========
; 1F0 (Read and Write): Data Register
; 1F1 (Read): Error Register
; 1F1 (Write): Features Register
; 1F2 (Read and Write): Sector Count Register
; 1F3 (Read and Write): LBA Low Register
; 1F4 (Read and Write): LBA Mid Register
; 1F5 (Read and Write): LBA High Register
; 1F6 (Read and Write): Drive/Head Register
; 1F7 (Read): Status Register
; 1F7 (Write): Command Register
; 3F6 (Read): Alternate Status Register
; 3F6 (Write): Device Control Register 


[BITS 32]  

;
; HDD support.
; Globais usadas por read e write.
;

global _hd_buffer
_hd_buffer:
    dd 0

global _hd_lba
_hd_lba: 
    dd 0


;--------------------------
; _test_write:
;     Testando escrever no disco. #funcionou.

global _test_write
_test_write:
    mov eax, dword 0x0E00
    mov ebx, dword 1020
    call hd0_write_sector
    ret

;-----------------------------------------
; _os_read_sector:
;    L� um setor do HD IDE.
;    bugbug: limpando a tela.
;    +a fun��o reset nao apresenta o bug. 
;    +testando hd0_read_sector.

global _os_read_sector
_os_read_sector:
    
	push eax
	push ebx
	
	;call _reset_ide0
	;mov eax, dword [_hd_buffer]    ;[900004h]    ;buffer.
	;mov ebx, dword [_hd_lba]       ;[900008h]    ;lba.
	;call hd0_read_sector
	
	
	; Uma nova rotina, bem mais r�pida.
    ; @param EAX Logical Block Address of sector
    ; @param CL  Number of sectors to read
    ; @param RDI The address of buffer to put data 
    ; obtained from disk
	mov eax, dword [_hd_lba]     
	xor ecx, ecx 
	mov cl, 1
	mov edi, dword [_hd_buffer]      
	call ata_lba_read

	pop ebx
	pop eax
	ret

;-----------------------
; _os_write_sector:
;     Escreve um setor no disco.

global _os_write_sector
_os_write_sector:

    push eax
    push ebx
 
    ;mov byte [0xb8000], byte 'r'
    ;mov byte [0xb8001], byte 9
    
	;call _reset_ide0
	
	;mov byte [0xb8002], byte 'w'
    ;mov byte [0xb8003], byte 9
	
	;mov eax, dword [_hd_buffer]    ;[900004h] ;buffer
	;mov ebx, dword [_hd_buffer]    ;[900008h] ;lba
	;call hd0_write_sector
	
	;mov byte [0xb8004], byte '$'
    ;mov byte [0xb8005], byte 9
	
	
    ; Uma nova rotina, bem mais r�pida. 
    ; @param EAX Logical Block Address of sector
    ; @param CL  Number of sectors to write
    ; @param RDI The address of data to write to the disk 
	mov eax, dword [_hd_lba]     
	xor ecx, ecx 
	mov cl, 1
	mov edi, dword [_hd_buffer]      
	call ata_lba_write

	pop ebx
	pop eax
	ret


;==================================================================
; ATA read sectors (LBA mode) 
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
; @return None
;==================================================================

ata_lba_read:

    pushf
    and eax, 0x0FFFFFFF
    push eax
    push ebx
    push ecx
    push edx
    push edi
 
    mov ebx, eax         ; Save LBA in RBX
 
    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al
 
    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
 
    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
 
    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
 
 
    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
 
    mov edx, 0x1F7       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al
 
.still_going:  
    in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov eax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; read CL sectors
    mul bx
    mov ecx, eax         ; RCX is counter for INSW
    mov edx, 0x1F0       ; Data port, in and out
    rep insw             ; in to [RDI]
 
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    popf
    ret

;====================================================================
; ATA write sectors (LBA mode)
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to write
; @param RDI The address of data to write to the disk
; @return None
;====================================================================
 
ata_lba_write:
    pushf
    and eax, 0x0FFFFFFF
    push eax
    push ebx
    push ecx
    push edx
    push edi
 
    mov ebx, eax         ; Save LBA in RBX
 
    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al
 
    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
 
    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
 
    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
 
 
    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
 
    mov edx, 0x1F7       ; Command port
    mov al, 0x30         ; Write with retry.
    out dx, al
 
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov eax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; write CL sectors
    mul bx
    mov ecx, eax         ; RCX is counter for OUTSW
    mov edx, 0x1F0       ; Data port, in and out
    mov esi, edi
    rep outsw            ; out
 
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    popf
    ret

;------------------------------------------
; hd0_read_sector:
;    L� um setor do HD, IDE.
;    BUG BUG: limpando a tela quando usa 
;             m�quina virtual da oracle.
; eax - endere�o
; ebx - lba
; ecx - null
; edx - null
;
;Vari�veis.
HD_BUFFER: DD 0 ;A
HD_L: DB 0      ;B
HD_B: DB 0      ;C
HD_A: DB 0      ;D
HD_LBA: DD 0
;Nothing. 
hd0_read_sector:

    ;PUSH EBP
    ;MOV  EBP, ESP

	;PUSHFD
	;PUSHA
	
	;CLI
	
    ;Salva em 'HD_BUFFER' o endere�o do buffer passado por 'eax'. 	
	;MOV DWORD [HD_BUFFER], EAX
	
    ;lba est� em ebx
	
	;;LOW
	;MOV AL, bl
	;MOV BYTE [HD_A], AL
    
	;shr ebx, 8
	;MOV AL, bl
	;MOV BYTE [HD_B], AL
	
	;;HIGH
	;shr ebx, 8
	;MOV AL, bl
	;MOV BYTE [HD_L], AL

	;Seleciona 'drive number, lba mode, head number.
    ;mov dx, word 01f6h    ;Drive and head port.
	;mov al, 0e0h          ;lba ;Drive 0, head 0.
	;out dx, al
    
	;QUANTIDADE DE SETORES. (1 setor).
	;mov dx, 1f2h         ;Sector count port.
	;mov al, 1            ;Read one sector.
	;out dx, al

	;LBA.
	
	;0-7
	;mov dx, 1f3h
	;MOV AL, BYTE [HD_A]    ;LOW.
	;out dx, al
	
	;8-15
	;mov dx, 1f4h
	;MOV AL, BYTE [HD_B] 
	;out dx, al

	;16-31
	;mov dx, 1f5h
	;MOV AL, BYTE [HD_L] 
	;out dx, al
	
	;Comandos.
	;xor eax, eax
	;mov dx, 1f7h    ;Command port.
	;mov al, 20h     ;Read with retry. 30h write.
	;out dx, al
	;Nothing.
;read_still_going:
	;in al, dx
	;test al, 8             ;This means the sector buffer requires.
	;jz read_still_going    ;Don't continue until the sector buffer is ready.

	;mov cx, 256            ;(512/2) 256 words por setor.

	;Prepara o destino. (buffer).
	;PUSH EAX
	;xor eax, eax
	;MOV eax, DWORD [HD_BUFFER]
	;mov edi, eax    ;dword.  
	;POP EAX
	
	;mov dx, 1f0h    ;Data port, data comes in and out of here.
	;rep insw
	;Nothing.
    ;STI
	
	;
	;@todo: o fun��o hd_wait funciona. us�-la
	;
	
    ;call hd_wait 	
	
	;call Small_Delay
	;call Small_Delay
	;call Small_Delay
	;call Small_Delay
	;call Small_Delay
	;POPA
	;POPFD
	;POP EBP

    RET

;----------------------------------------
; hd0_write_sector:
;     Escreve no hd.
; eax - endere�o
; ebx - lba
; ecx - null
; edx - null

hd0_write_sector:

    ;PUSH EBP
    ;MOV  EBP, ESP

	;PUSHFD
	;PUSHA
	
	;CLI

	;;SALVA 
	;MOV DWORD [HD_BUFFER], EAX
  	
	;;LOW
	;MOV AL, bl
	;MOV BYTE [HD_A], AL
    
	;shr ebx, 8
	;MOV AL, bl
	;MOV BYTE [HD_B], AL
	
	;;HIGH
	;shr ebx, 8
	;MOV AL, bl
	;MOV BYTE [HD_L], AL

	;Seleciona 'drive number, lba mode, head number
    ;mov dx, word 01f6h    ;Drive and head port
	;mov al, 0e0h          ;lba   ;Drive 0, head 0
	;out dx, al
    
	;QUANTIDADE DE SETORES. (1 setor)
	;mov dx, 1f2h         ;Sector count port
	;mov al, 1            ;Read one sector
	;out dx, al

	;LBA.
	
	;0-7
	;mov dx, 1f3h
	;MOV AL, BYTE [HD_A]    ;LOW.
	;out dx, al
	
	;8-15
	;mov dx, 1f4h
	;MOV AL, BYTE [HD_B] 
	;out dx, al

	;16-31
	;mov dx, 1f5h
	;MOV AL, BYTE [HD_L] 
	;out dx, al
	
	;Comandos.
	;xor eax, eax
	;mov dx, 01f7h    ;Command port.
	;mov al, 30h      ;with retry. 30h write.
	;out dx, al
    ;Nothing.
;write_still_going:	
	;in   al, dx
	;test al, byte 8    ;This means the sector buffer requires servicing.
	;jz write_still_going    ;Don't continue until the sector buffer is ready.
	;mov ecx, dword 256      ;512/2, ;One sector/2.

	;mov dx, 01f0h         ;Data port - data comes in and out of here.
	
	;MOV esi, DWORD [HD_BUFFER]
	
	;rep outsw	
    
	;STI  ;; #bugbug ... isso pode ser problema ...
	
	
	;push eax
	;push ecx
	;mov ecx, 300
	;call delay_do_fred
	;pop ecx 
	;pop eax
	
    ;call hd_wait     ;@todo: Usar wait. 
	
	;call Small_Delay  
	;call Small_Delay
	;call Small_Delay
	;call Small_Delay
	;call Small_Delay
	
	
	;POPA
	;POPFD
	;POP EBP

    RET

;-----------------
;hd_wait: 
;    Hard Disk Wait Function.

hd_wait: 
    cli 
    mov dx,1F7h 
waitloop:
    ;Delay
    in al, dx 
    in al, dx 
    in al, dx 
    in al, dx 
    cmp al, 50h 
    je waitok 
    cmp al, 0 
    je waitok 
    jmp waitloop
waitok: 
    sti 
    ret 

;-------------------------------------
; _reset_ide0:
;     Reset IDE.
;
global _reset_ide0
_reset_ide0:
    mov dl, byte 0
    call IDE_RESET_DISK
    ret
;Reseta o ide0 master.
init_ide0_master:
    mov dl, byte 0
    call IDE_RESET_DISK
    ret

;----------------------------------------
; IDE_RESET_DISK:
;     Reset IDE.
;   dl = Drive (0=master,1=slave).
; Note: 
; Some disks will respond to the reset command 
; if the other is given it.

IDE_RESET_DISK:
    ;Nothing.
	and dl, 01           ;isolate bit 0
	mov al, dl           ;al = dl
	shl al, 4            ;move drive selector into bit 4
	or al, 0A0h          ;make bits 7 and 5 a 1
	mov dx, 01F6h        ;dx = drive/head regsiter port address
	out dx, al           ;write to drive/head register
	mov al, 10h          ;Command 1xh: Recalibrate
	inc dx               ;dx is now 01F7h, port address of command register
	mov byte [lastcommand], 10h ; Tell our ISR what we are doing
	out dx,al           ; Recalibrate!
	IODELAY

	mov ax,0FFFFh       ; loop var = 0FFFFh
	xor dl,dl
	mov byte [commanddone],dl  ; init commanddone with 0
	;Nothing.
again:
	cmp byte [commanddone],1
	je resetisdone
	cmp ax,0
	je noreset
	dec ax
	jmp again
	;Nothing.
resetisdone:
	cli
	mov byte [lastcommand],01h ; set this in case of unexpected command
	sti
	clc                 ; its all good
	ret                 ; outta here
noreset:
	cli
	mov byte [lastcommand],01h ; set this in case of unexpected command
	sti
	stc                 ; error
	ret                 ; outta here
; end of IDE_RESET_DISK
;Nothing.
lastcommand db 0    ;The last command given to the disk.
commanddone db 0    ;Command done flag.
commandok   db 0    ;Command completed properly flag.	


;-----------------------------
delay_do_fred:
    out 0x80, al
    dec ecx
    jnz delay_do_fred
    ret

;--------------------------------------------
; Small_Delay:
;     Delay usado no driver de hdd.                                     

Small_Delay:
    push ecx
    mov  ecx, 0xfffff
BusyDelay3a:
    IODELAY                                  
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    loop BusyDelay3a
    pop ecx
    ret


; hardwarelibInit:
;     Initializing the lib.

;hardwarelibInit:
;    nop
;    ret


;
; End
;

