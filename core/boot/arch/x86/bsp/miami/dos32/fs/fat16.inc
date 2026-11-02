;
; File: fat16.inc 
;
; Descrição:
;
;     FAT16 support in (32bit).
;
; Atribuições:
;     +Carrega arquivo.
;     +Passa o comando para ele.
;
; History:
;     2015 - Created by Fred Nora.
;


[BITS 32]


;-----------------------------------------
; CurrentFile:
;     Parametros do arquivo atual.
;
;
CurrentFile:
.name: dd 0         ;Endreço da string.
.size: dd 0         ;Tamanho em número de setores.
.lba_inicial: dd 0  
;...


;
; Uma lista de clusters.
;

lista_file_clusters:
    times 32 dw 0
	

;===========================================================
; fat16ReadFile:
;     Carrega um arquivo na memória. 
;     Status: (funcionando)
;
; IN:	
;     eax = Endereço do nome do arquivo.
;     ebx = ??
;     ecx = ??
;     edx = ??
;

fat16ReadFile:

    pushad 

	;Salva o endereço do nome do arquivo.
    mov dword [g_image_address], eax 	

    ;Confere o número do disco.
    cmp byte [fat16_bpb.DriveNumber], byte 0x80	
    jne .fail


	; Go!

    xor eax, eax  
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
	
	;Post message.
    ;;mov  esi, msg_fat16_Loading
    ;;call os_output
	;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_Loading  
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen	
	
    ;1 - Calcula o início da área de dados.
	;mov esi, msg_fat16_step1
	;call os_output
	;push eax
	;push ebx
	;push ecx 
	;push edx	
	mov esi, msg_fat16_step1  
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen		
	
	call get_dataarea_init	
	
    ;2 - Carrega o rootdir.
	;mov esi, msg_fat16_step2
	;call os_output
	;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_step2  
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen	
	
.loadRoot:	
	call fat16_LOAD_ROOT
	
	;3 - Procura um nome de arquivo no diretório raiz.
	;mov esi, msg_fat16_step3
	;call os_output
	;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_step3  
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen		
	
	
	mov eax, dword [g_image_address]
	call fat16_search_file_name
	;jc  .fail                       ;@todo: Erro. 
	
    mov word [.cluster], ax          ;Salva cluster inicial.
	
    ;4 - Carrega a FAT. 
	;mov esi, msg_fat16_step4
	;call os_output
	;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_step4  
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen		
	
.loadFAT:	
	call fat16_LOAD_FAT
	
    ;5 - Carrega o arquivo.	
	;mov esi, msg_fat16_step5
	;call os_output
	;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_step5  
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen		
	
	
.loadImage:	
	mov ax, word [.cluster]
	call fat16_LOAD_IMAGE		  	
	
	;FUNCIONOU
.fat16_read_DONE:
	;mov esi, msg_fat16_Done
    ;call os_output
    ;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_Done   
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen	

 
    ;olhar o arquivo carregado!
    ;mov esi, dword FAT16_IMAGE_ADDRESS
    ;call os_output
    ;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, FAT16_IMAGE_ADDRESS   
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen	
	
	popad
    ret	 ;Retorna para quem chamou fat16.inc.
.fail:
    ;mov esi, msg_fat16_diskfail
    ;call os_output
   ; push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_diskfail   
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen

	
	popad
	stc
	ret	
.cluster: dw 0


;============================================
; fat16_write_file:
;     Salvar o arquivo.
; IN:
;     eax = Endereço do nome do arquivo.
;

fat16_write_file:
fat16WriteFile:
 
    pushad 

	;salva o endereço do nome do arquivo.
    mov dword [g_image_address], eax 	
	 
    ;confere o numero do disco
    cmp byte [fat16_bpb.DriveNumber], byte 0x80 
    jne .fail
	
	;post message
    mov  esi, msg_fat16_Loading
    call os_output
	
    xor eax, eax  
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
	
    ;1 - calcula o inicio da area de dados
	mov esi, msg_fat16_step1
	call os_output
    ;jmp $
	call get_dataarea_init	
	
    ;2 - carrega a fat 
	mov esi, msg_fat16_step4
	call os_output
	;jmp $
	call fat16_LOAD_FAT
	
	;procura uma entrada vazia na fat.
	mov eax, 1 ; numero de entradas para encontrar(file size)
	call procura_entrada_vazia_na_fat

    ;2 - carrega o rootdir
	mov esi, msg_fat16_step2
	call os_output
    ;jmp $
	call fat16_LOAD_ROOT
	
	;3 - procura um nome de arquivo no diretorio.
	;mov esi, msg_fat16_step3
	;call os_output
	
	
	;jmp $
	call fat16_search_file_name
    	
    ;4 - carrega a fat 
	mov esi, msg_fat16_step4
	call os_output
	;jmp $
	call fat16_LOAD_FAT

    ;5 - carrega o arquivo	
	mov esi, msg_fat16_step5
	call os_output
	;jmp $
	call fat16_LOAD_IMAGE
	;FUNCIONOU
.fat16_write_DONE:
	;mov esi, msg_fat16_Done
    ;call os_output
	;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_Done   
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen		
	
 
    ;olhar o arquivo carregado!
    ;mov esi, dword FAT16_IMAGE_ADDRESS
    ;call os_output 
   ; push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, FAT16_IMAGE_ADDRESS   
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen

    popad
    ret    ; Retorna para quem chamou fat16.inc.

.fail:

    ;mov esi, msg_fat16_diskfail
    ;call os_output

    mov esi, msg_fat16_diskfail  
    call lib32PrintString
 
    call _asm_refresh_screen

    popad
    ret


;
; End.
;

