;
; File: drivers\hdd32.inc
;
; Descrição:
;     Driver do controlador HDD.
;     Legacy IDE.
;     System class: system.device.unblocked.
;
; Atribuições
;     Read and write sectors in PIO 0 mode.
;
; @todo: 
; Talvez algum suporte pra PCI e SATA controler.  
;
; Histórico:
;     Version: 1.0, 2015 - Created.
;     ...
; 
 
 
;Ports:
;=====
;1F0 (Read and Write): Data Register.
;1F1 (Read): Error Register.
;1F1 (Write): Features Register.
;1F2 (Read and Write): Sector Count Register.
;1F3 (Read and Write): LBA Low Register.
;1F4 (Read and Write): LBA Mid Register.
;1F5 (Read and Write): LBA High Register.
;1F6 (Read and Write): Drive/Head Register.
;1F7 (Read): Status Register.
;1F7 (Write): Command Register.
;3F6 (Read): Alternate Status Register.
;3F6 (Write): Device Control Register.	
;
 
; 
; Segmento de código.
;
segment .text
 
 
[BITS 32]  


;
; Selected Drive Number. 0=master 1=slave
;

Selected_Drive_Number:
    db 0

;-------------------------------------------
; _irq14:
;     Handler de interrupção de IDE.
;     Unidade master.
;
;
_irq14:             ;ata_MasterIrqHandler:
	PUSH AX
	MOV AL, 020h
	OUT 0A0h, AL
	OUT 020h, AL
	POP AX
	IRETD

;--------------------------------------------
; _irq15:
;     Handler de interrupção de IDE.
;     Unidade Slave.
;
_irq15:              ;ata_SlaveIrqHandler:
	PUSH AX
	MOV AL, 020h
	OUT 0A0h, AL
	OUT 020h, AL
	POP AX
	IRETD


;--------------------
; Registradores:
; ==============
;
; eax - Endereço.
; ebx - LBA.
; ecx - Null.
; edx - Null.
;--------------------

HD_BUFFER: DD 0    ;EAX.
HD_L:      DB 0    ;EBX.
HD_B:      DB 0    ;ECX.
HD_A:      DB 0    ;EDX.

HD_LBA: DD 0

 
 
;-----------------------------------------------------------------------
; select_hd:
;     Seleciona em qual hd vai ser efetuada a operação de read ou write.
; 
;    IN: AL = Hd number, 0=0x80, 1=0x81.
;
select_hd: 
    ;pushad
	cmp al, 0      ;Master.
	je .master
    cmp al, 1      ;Slave.
	je .slave
	jmp .master    ;Default.
	;
.master:
    mov byte [Selected_Drive_Number], 0
    call init_ide0_master
    jmp .done
.slave:
    mov byte [Selected_Drive_Number], 1	
    call init_ide0_slave
	jmp .done
.done:
    ;popad
	ret 
 
 
;
; Funções.
;

;===============================================
; hd0_read_sector:
;     Lê um setor do disco. (PIO Mode)
; 
; eax - Endereço.
; ebx - LBA.
; ecx - Null.
; edx - Null.
;
hd0_read_sector:
    PUSH EBP
	MOV  EBP, ESP  
	PUSHFD
	PUSHAD
	
	CLI

	;;SALVA. 	
	MOV DWORD [HD_BUFFER], EAX
	
   ;;LBA está em ebx.
	
	;;LOW.
	MOV AL, bl
	MOV BYTE [HD_A], AL
    
	shr ebx, 8
	MOV AL, bl
	MOV BYTE [HD_B], AL
	
	;;HIGH.
	shr ebx, 8
	MOV AL, bl
	MOV BYTE [HD_L], AL

	;
	; Seleciona 'drive number, 
	; lba mode, head number.
    ; ; 0h selects device 0 (master). 
	; ;10h would select device 1 (slave).
	;
	
	;Send 0x40 for the "master" or 0x50 for the "slave" to port 0x1F6: 
	; outb(0x1F6, 0x40 | (slavebit << 4))
	;   uint8_t select = 0xa0;
    ;if(is_master) select |= 1;
	
	mov dx, word 01F6h    ;Drive and head port. ;port to send drive & head numbers.
	mov al, 0E0h          ;lba ;Drive 0, head 0.
.select_drive_number:
    cmp byte [Selected_Drive_Number], 1
    jne .not_slave	
.is_slave:	
	or al, 010h            ;Seleciona o Slave.(mudando o bit 4)
.not_slave:
	out dx, al

	;QUANTIDADE DE SETORES. (1 setor).
	mov dx, 1F2h    ;Sector count port.
	mov al, 1       ;Read one sector.
	out dx, al

	;LBA- teste 2304 900h.
	
	;0-7.
	mov dx, 1f3h
	MOV AL, BYTE [HD_A]    ;LOW.
	out dx, al
	
	;8-15.
	mov dx, 1f4h
	MOV AL, BYTE [HD_B] 
	out dx, al

	;16-31.
	mov dx, 1f5h
	MOV AL, BYTE [HD_L] 
	out dx, al
	
	;Comandos.
	xor eax, eax
	mov dx, 1f7h    ;Command port.
	mov al, 20h     ;Read with retry. 30h write.
	out dx, al
	
.read_still_going:
	in al, dx
	
	test al, 8              ;This means the sector buffer requires servicing.
	jz .read_still_going    ;Don't continue until the sector buffer is ready.

	;Contador.
	mov cx, 256    ;(512/2), (One_sector/2).
	
	PUSH EAX
	xor eax, eax
	MOV eax, DWORD [HD_BUFFER]
	mov edi, eax                  ;dword  0x0900000 ;buffer ;offset buffer.
	POP EAX
	
	mov dx, 1f0h    ;Data port, data comes in and out of here.
	rep insw
	
    STI
	
.wait:    
    ;call hd_wait    ;Substituindo por delay manual.
.dalay:	
	;Delay.
	call Small_Delay	
	call Small_Delay	
	call Small_Delay	
	call Small_Delay	
.done:
	POPAD
	POPFD
	POP EBP	 
	RET
	
	
;=================================================
; hd0_write_sector:
;     Grava um setor no disco.
;
; eax - Endereço.
; ebx - LBA.
; ecx - Null.
; edx - Null.
;
hd0_write_sector:

    PUSH EBP
	MOV  EBP, ESP
    
	PUSHFD
	PUSHAD
	
	CLI

	;;SALVA. 	
	MOV DWORD [HD_BUFFER], EAX
 
	;;LOW.
	MOV AL, bl
	MOV BYTE [HD_A], AL
    
	shr ebx, 8
	MOV AL, bl
	MOV BYTE [HD_B], AL
	
	;;HIGH.
	shr ebx, 8
	MOV AL, bl
	MOV BYTE [HD_L], AL

	;
	; Seleciona 'drive number, 
	; lba mode, head number.
	;
	
	mov  dx, word 01f6h    ;Drive and head port.
	mov  al, 0E0h          ;lba ;Drive 0, head 0.
.select_drive_number:
    cmp byte [Selected_Drive_Number], 1
    jne .not_slave	
.is_slave:	
	or al, 010h            ;Seleciona o Slave.(mudando o bit 4)
.not_slave:
	out  dx, al
    
	;QUANTIDADE DE SETORES. (1 setor).
	mov dx, 1f2h    ;Sector count port.
	mov al, 1       ;Read one sector.
	out dx, al

	;LBA.
	
	;0-7.
	mov dx, 1f3h
	MOV AL, BYTE [HD_A]    ;LOW.
	out dx, al
	
	;8-15.
	mov dx, 1f4h
	MOV AL, BYTE [HD_B] 
	out dx, al

	;16-31.
	mov dx, 1f5h
	MOV AL, BYTE [HD_L] 
	out dx, al
	
	;Comandos
	xor eax, eax
	mov dx, 01f7h    ;Command port.
	mov al, 30h      ;with retry. 30h write.
	out dx, al

write_still_going:	
	in   al, dx
	
	test al, byte 8    ;This means the sector buffer requires servicing.
	jz write_still_going     ;Don't continue until the sector buffer is ready.
	
	;Contador.
	mov ecx, dword 256    ;(512/2), (One_sector/2).
	
	mov dx, 01f0h    ;Data port, data comes in and out of here.
	MOV esi, DWORD [HD_BUFFER]
	rep outsw
    
    STI
	
    ;call hd_wait    ;Substituido por delay manual.

    ;Delay. 	
	call Small_Delay
	call Small_Delay	
	call Small_Delay	
	call Small_Delay	
	
	POPAD
	POPFD
	
	POP EBP
	RET
	
	
;---------------------------------------
; hd_wait:
;	
;     WAIT. Hard Disk Wait Function 
;
hd_wait: 
    cli 
    mov dx, 1F7h 
waitloop:  
    in al, dx 
    
	cmp al, 50h 
    je waitok 
	cmp al, 0 
    je waitok 
	jmp waitloop 
waitok: 
    sti 
    ret 

	
;-------------------------
; Small_Delay:
;     Delay.                                       
;
Small_Delay:
    push ecx
	mov  ecx, 0x0ffff
BusyDelay3a:                                  
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
    RET
	
	
;========================================
; init_ide0_master:		
;     Reseta o ide0 master.
;
init_ide0_master:
    push edx
	mov dl, byte 0
	call IDE_RESET_DISK
	pop edx
	ret

	
;--------------------------------
; init_ide0_slave:		
;     Reseta o ide0 slave.
;
init_ide0_slave:
    push edx
	mov dl, byte 1
	call IDE_RESET_DISK
	pop edx
	ret
	
	
;==================================================
; IDE_RESET_DISK:
;     reseta o disco.
;
;   dl = Drive (0=Master, 1=Slave).
;
;   Note: 
;   Some disks will respond to the reset command if the other is given it.
;
IDE_RESET_DISK:

	and dl, 01       ;Isolate bit 0. (Use apenas o bit 0)
	mov al, dl       ;al = dl.
	shl al, 4        ;Move drive selector into bit 4.
	or al, 0A0h      ;Make bits 7 and 5 a 1.
	
	mov dx, 01F6h    ;dx = drive/head regsiter port address.
.select_drive_number:
    cmp byte [Selected_Drive_Number], 1
    jne .not_slave	
.is_slave:	
	or al, 010h            ;Seleciona o Slave.(mudando o bit 4)
.not_slave:
	out dx, al       ;Write to drive/head register.
	
	mov al, 10h      ;Command 1xh: Recalibrate.
	inc dx           ;dx is now 01F7h, port address of command register.
	mov byte [lastcommand], 10h    ;Tell our ISR what we are doing.
	out dx, al                     ; Recalibrate.

	mov ax, 0FFFFh  ;Loop var = 0FFFFh.
	xor dl, dl
	mov byte [commanddone], dl    ;Init command done with 0.
again:
	cmp byte [commanddone], 1
	je resetisdone
	cmp ax, 0
	je noreset
	dec ax
	jmp again
resetisdone:
	cli
	mov byte [lastcommand], 01h    ;Set this in case of unexpected command.
	sti
	clc    ;Its all good (Flag).
	RET    ;Outta here.

noreset:
	cli
	mov byte [lastcommand], 01h    ;Set this in case of unexpected command.
	sti
	stc    ;Error (Flag).
	RET    ;Outta here.
	
;### End of 'IDE_RESET_DISK'.



;
; Variáveis.
;

lastcommand  db 0    ;The last command given to the disk.
commanddone  db 0    ;Command done flag.
commandok    db 0    ;Command completed properly flag.
	
;
;fim
;
