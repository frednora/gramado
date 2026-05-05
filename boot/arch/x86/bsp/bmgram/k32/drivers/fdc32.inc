; 
; File: drivers/fdc32.inc 
;
; Descrição: 
;     Driver do FDC (Floppy Disk Controller) para 32bit.
;
; OBS: 
;     Funciona bem. Não mecher!
;     * Sendo tão difícil fazer um driver de floppy que funcione,
;       considere guarda-lo.
;
; History:
;     2013 - Created.
;     2017 - Revision.
;


;
; Segmento de código.
;

segment .text
 
	
;estamos em 32 bits. 
[BITS 32]  


;
; Devinições:
; 


;;
;; Buffer usado pelo DMA
;; Buffer	equ  604*256 ;25c00 - atrapalha a gui.
;; Provisório.
;;
;; #BUGBUG Reconsiderar esse Buffer, provalvelmente o próprio boot manager
;; ocupe esse espaço.
;;
fdc_buffer equ 00006000h 


;
; Tempo para o motor.
;
us equ 1000/4
ms equ 1000*us 



;
; Variáveis:
;


;
; Forth+Icons = 1 sector, plus Source blocks 18 to 252 = 17 sectors    ; # $30
;
nc: dd 16 



;
; Mensagens:
;
;string_fdc_1 db 'Leu 1 setor.',0
   
   
;
; Macros: 
;

;DUP_
%macro DUP_ 0
    lea esi, [esi-4]
	mov [esi], eax
%endmacro

;DROP
%macro DROP 0
	lodsd
%endmacro

;NEXT
%macro NEXT 1
	dec ecx
	jnz %1
%endmacro	
 
	
;
; BPB - Bios Parameter Block.
;
fdc_bpb:
    nop
    db '01230123'
    dw 512     ; bytes/sector
    db 1       ; sector/cluster
    dw 1       ; sector reserved
    db 2       ; FATs
    dw 16*14   ; root directory entries
    dw 80*2*18 ; sectors
    db 0F0h    ; media
    dw 9       ; sectors/FAT
    dw 18      ; sectors/track
    dw 2       ; heads
    dd 0       ; hidden sectors
    dd 80*2*18 + 0xC7000000 ; sectors again  ; to match cfdos4.blk
    db 0       ; drive 
    ;db 0
    ;db 29h     ; signature
    ;dd 44444444h     ; serial
    ;db '           ' ; label
    ;db '        '

		
;
; Tabela de comandos enviados ao FDC.
;
command:
    db 8     ; to match cfdos4.blk
    db 0     ; head, drive 
cylinder:
    db 0     ; trilha?	
    db 0     ; head .. lado
    db 1     ; sector
    db 2     ; 512 bytes/sector
    db 18    ; sectors/track
    db 1bh   ; gap
    db 0ffh  ; # $2D
	db 89h   ; to match cfdos4.blk
	db 0f6h  ; to match cfdos4.blk

;alinhamento
ALIGN 4

;
; FUNÇÕES --------------------------------
;


;
; test_fdc_main(); - le um setor
;
; a = setor.
; b = trilha.
; c = lado.
; d = endereço.
;

salva_a:   dd 0 
salva_b:   dd 0
salva_c:   dd 0
salva_d:   dd 0
salva_esp: dd 0


;alinhamento
ALIGN 4

;================================================
; _irq6: 
;     Floppy disk irq.
;
_irq6:  

	PUSH AX
	MOV AL, 020h
	OUT 0A0h, AL
	OUT 020h, AL
	POP AX
	IRETD



;================================================================
; _fdc_main:
;     Função principal do driver FDC. (Ler setor).
;	
global _fdc_main
_fdc_main:
;fdc32ReadSector:
    
	;salva a pilha para um retorno seguro.
	mov dword [salva_esp], esp

	;salva argumentos a b c d.
    mov dword [salva_a], eax  ;setor
    mov dword [salva_b], ebx  ;trilha
    mov dword [salva_c], ecx  ;lado
    mov dword [salva_d], edx  ;endereço

    ;zera
    xor eax, eax
    mov ebx, eax
    xor ecx, ecx
    mov edx, edx

	;uso interno
    mov esp, dword 0x95000
 
	;
	; OBS: Não vou mexer na pilha. 
	; (esp = 0x9fff0)
	;
	
	;Zera
	xor ecx, ecx
cold:
    ; ?
    ;esp - um pouco.
	mov esi, 0x00090000    ;'esi' está sem propósito?
    ;Ligar motor.
.ligar_motor:		
	mov cl, 0x1c
	;liga.
.__onoff:
	DUP_
	mov al, cl
	mov dx, 0x3f2
	out dx, al
	out 0xe1, al
	DROP

	;Prepara para o loop.
    mov  ECX, 400*ms    ;(c4a)
.spin:	
    loop .spin
.reacalibrar:    
	;mov  [cylinder], cl    ;calibrate ;(c4a).
	mov AL, 7
	mov CL, 2	
    call cmd     
.loop_sense:
    call sense_  
    jz .loop_sense	
.configura_dma:
    ;2 6 16 ms (e 2)  
	;preparando o comando specify. 
	;step rate.
	;specify - funcao 3.
    mov word [command+1], 0x02a1		
	;timing?! Acho que é o specify.
	mov al, 3				
	mov cl, 3
	call cmd
	
	;Limpa a tabela.
    mov  word [command+1], 0      
    
	;O valor indicado pela variável vezes 4.
	;bate com o valor da tranferência depois.
	mov  eax, fdc_buffer*4                  
                                    
	;buffer.
    out 4, al   
    mov al, ah      
    out 4, al
	
    shr eax, 16   
    out 0x81, al
	
	;Quantidade. 
	;está preparado pra enviar até 2 trilhas.
    ;dma 2.
	mov eax, 512*18*2 - 1  
    out 5, al                   
    mov al, ah                  
    out 5, al
	
    mov al, 0x0b                 
    out 0x0f, al                 
	
;
; Ler setores.
; Isso pode ser o início de um loop, já que o DMA é configurado apenas 
; uma vez.
;
; O registrador edi deve ser incremetado em 512 para mover a informação 
; do buffer do dma até o buffer do arquivo.
;
; O buffer do DMA é de tamanho fixo.
;

.read_sector:	
	;buffer
	;cylinder 0 on addr 0
	;xor edi, edi	
	
	;suja o buffer 3. (provisório) ... b00000h ?
    mov edi, dword 0900000h     
 
	;seek.
	;Read DMA 2.
	mov AL, 16h 
	call seek
	
;++teste_inicio
;testando especificar (chs) no comando read.
	;salva
	push eax
	;trilha  	
	;b = trilha.
	mov eax, dword [salva_b] 
	mov [command+2], byte al
	;lado
	;c = lado
	mov eax, dword [salva_c] 
	mov [command+3], byte al
	;setor
	;a = setor
	mov eax, dword [salva_a] 
	mov [command+4], byte al
    ;recupera
	pop eax
;--teste_fim	
	
	;transfer.
	;read normal data.
	mov al, 0xE6				
	call transfer
	
	;eoi - end of interrupt.
	mov al, byte 0x20
	out 0x20, al
	out 0xA0, al

	;move para esi
	;buffer 3 por enquanto.
	PUSH ESI
    mov  ESI, fdc_buffer*4
    mov  ECX, 512*18*2/4
    rep movsd
    POP  ESI
	
;
; @todo:
;     Antes de desligar o motor
;     pode-se ler mais setores. 
;
.desliga_motor:
;.turnoffFDCMotor:	
	mov cl, 0xc
;liga
._onoff:
	DUP_
	;lea esi, [esi-4]
	;mov [esi], eax
	
	mov al, cl
	mov dx, 0x3f2
	out dx, al
	out 0xe1, al
	DROP	 
	;lodsd
	
    ;recupera esp	
	mov esp, dword [salva_esp]		
	RET
	
  
;
; Outras funções ----------------------------
;	


;-----------------------
; ready:
;
ready:
    mov edx, 0x3f4
.re:	
    in al, dx
	out 0xe1, al
	shl al, 1
	jnc .re
	
	lea edx, [edx+1]
	ret
cmd:	
    push esi	
	lea esi, [command]
	
	;prepara o comando para o lodsb usa-lo.
	;isso enviou o comando principal para 'command'
	mov [esi], al
Xcmd:   
    call ready
    ;se tiver pronto, manda o comando para a porta 3f5.
	jns  cmd0 
	
	in   AL, DX 
    ;tll ;(c4a)	
	OUT  0E1h, AL 
	jmp short Xcmd	
cmd0:   
    ;pega o comando em 'command' e coloca em al.
    lodsb
	;envia o comando para a porta.
	;deveria ser 3f2 e não 3f5 ?!
    out  DX, AL   
	
	;cria um delay.
	push  ecx          ; (c4a)
    mov   ecx, 001eh   ; (c4a)
cmd1:   
    out 0e1h, al       ; (c4a)  ; acho que isso esta funcionando como porta de commando ....
    loop cmd1          ; (c4a)  ;;fazendo o lugar de 3f2h
    pop ecx            ; (c4a)
	
	;próximo  comando. 
	;? - cx pe o numero de comandos.
    loop Xcmd
    pop  ESI
    RET
	
	
;-------------------
; sense_:
;     SENSE.
;
sense_:
	mov al, 8
	mov cl, 1
	call cmd
.se:	
    call ready
    ;jns .se
	in al, dx
	out 0xe1, al
	cmp al, 0x80
	ret


;----------------------------
; seek:
;     SEEK.
;	
seek:
    ;isso deve ser do DMA. ?!	
    out  0x0b, al 
Xseek:
    ;sense suja a tabela de comandos.	
    call sense_ 
    jnz  Xseek
	
	;envia comando seek. 
	;a trilha nao foi especificada ?!
	
	;teste - especificando trilha.
	;teste - funcionou.
	; b = trilha
	push eax
	mov eax, dword [salva_b] 
	mov [command+2], byte al
    pop eax
	
	;teste - testando ler outra trilha.
	;teste - vendo se o seek funciona.
	;mov [command+2], byte 1
	;teste - acho que o teste do seek funcionou
	
	;seek - primeiro comando.
	mov  al, 0fh  
	;número de comandos.
	mov  cl, 3     
cmdi:	
    call cmd      
Xcmdi:    
    call sense_  
    jz Xcmdi
    RET

 
;------------------------------------
; transfer:
;
transfer:
    ;o registrador al contém o comando (e6) - (ler setor).
	;numero de comandos.
	mov cl, 9 
	call cmd
	
	;porque incrementar a trilha ?
	inc byte [cylinder]
.transfer:	
	call ready
	jns .transfer
	RET


;;===============================================================
; write:
;     WRITE.
;

write:
    ;move 
    mov edi, fdc_buffer*4
    mov ecx, 512*18*2/4
    rep movsd
	
	;seek
	;write DMA 2.
	mov al, 1ah  
    call seek                  
    
	;cmd
	mov al, 0c5h                            
	mov cl, 9
	call cmd
	
	;transfer
	inc byte [cylinder]
.W_transfer:	
	call ready
	jns .W_transfer
	RET

	
;===========================================
; fdc32Init:
;     FDC Initialization.
;
	
fdc32Init:
    nop    ;; Nothing for now.
    ret

 
;
; End.
;



