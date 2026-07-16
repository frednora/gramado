;
; Arquivo: fs\format.inc 
;
; Descrição: 
;     Formata um volume.
;
;     Obs:
;     Por enquanto formata o diretório raiz do disco 0.
;
; Versão 1.0, 2015.
;



;-------------------------------
; format_disk:
;     Formatar.
;     IN: AL = Hd number, 0=0x80, 1=0x81.
;
format_disk:

    cmp byte [Selected_Drive_Number], 1
    jne .not_slave	
.is_slave:	
    
	;Select and initialize slave.
	mov al, 1
    call select_hd
    
	jmp .go
.not_slave:
    ;Select and initialize master.
	mov al, 0
    call select_hd
.go:	
	;call init_ide0_master
	
    ;pushad
    push esi
	
	;apaga root
	mov esi, msg_format1
	call os_output
	call apaga_root
	
	;apaga fat
	mov esi, msg_format2
	call os_output
	call apaga_fat
	
	;mensagem
	;mov esi, msg_format3
	;call os_output
	
	;cria os arquivos basicos.
	;call make_boot
	

 	;avisa que o novo bl.bin nao está instalado.
	mov eax, dword 0
	call blconfig_alterar_status_bl32
	mov dword [bootloader_install_status], dword 0	
	
	clc
	
	pop esi
    ;popad
    ret
	
	
;----------------------------------------
; apaga_root:
;     Apaga o Root Dir.
;	
apaga_root:
   
   ;pushad
	push eax
	push ecx
	push edi
	
	call load_root_first_sector 
  
	mov edi, FAT16_ROOTDIR_ADDRESS    ;Início do diretório raiz.
	
    ;prepara o contador de entradas.
    mov ecx, 16     ; 16 entradas do primeiro setor
	
 ;++
.loop_apaga_root:
    
	;apaga uma entrada
	mov al, byte '$'    ;0 ZERO!
    stosb              
	
	add edi, dword 31    ;Próxima entrada.
	
	LOOP .loop_apaga_root
;--	 

    call force_save_root_first_sector
	
	pop edi
	pop ecx
	pop eax
	
	;popad
    ret
	
	
		
;--------------------------------------
; apaga_fat:
;     Apaga a FAT.
;	
apaga_fat:
   ; pushad
	push eax
	push ecx
	push edi	

	call load_fat_first_sector 
  
    ;prepara o contador de entradas.
    mov ecx, 64                   ;64 entradas do primeiro setor.
	mov edi, FAT16_FAT_ADDRESS    ;Início da FAT.
	
  ;++
.loop_apaga_fat:
    ;salva o contador de entradas
	push ecx
	
	;apaga uma entrada
	mov al, byte 0    ;ZERO!
	mov ecx, dword 2         
    rep stosb              
    
    ;recupera o contador de entradas	
    pop ecx
                                                         	
	LOOP .loop_apaga_fat
;--	  
    call save_fat_first_sector
    
	pop edi
	pop ecx
	pop eax
	;popad	
    ret
	

;Inicializa o módulo	
;formatInit:
;    nop
;	 ret 
	
;Mensagens.	
msg_format1 db "Apagando Root..",13,10,0	
msg_format2 db "Apagando FAT..",13,10,0
;msg_format3 db "apagado",13,10,0

;
;fim.
; 
   
   