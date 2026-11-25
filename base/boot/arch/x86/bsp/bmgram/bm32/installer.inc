;
; Arquivo: installer.inc
;
; Descrição:
;     @todo: Essa é uma tentativa de criar algum tipo de instalação
;            e montagem do sistema de arquivos.
;            Que seja na verdade apenas formatação da partição de boot. 
;            Instalação do META$FILE e arqivos de inicialização. 
;            Obs: Tem biblioteca em 32it tentando fazer montagem de file system.
; 
;
;            (1024) - lba da área de transferência.
;
;
; Versão: 1.0, 2015, 2016.
;



;
; Definições.
;


IMG_ADDRESS  equ 0x00009000  ;@todo: Valor desatualizado.
IMG_SIZE     equ 64          ;@todo: Valor desatualizado. 



 
;
; Variáveis.
;


IMG_LBA_ORIGEM:  dd 0     
IMG_LBA_DESTINO: dd 0   



;---------------------------------------------
; install_bootloader: 
;
;     Instala o Boot Loader que está na area de transferência.
;     Obs: Isso é um test.
;
install_bootloader:

    ;check dir for bl.bin
	call load_root_first_sector 

	cmp [FAT16_ROOTDIR_ADDRESS +64], byte 'b'
	jne .fail

	cmp [FAT16_ROOTDIR_ADDRESS +65], byte 'l'
	jne .fail
	
	;1 - carregar na memoria o que esta na area de transferencia.
	mov dword [IMG_LBA_ORIGEM], dword 1024 ; area de transferencia
	call transf_load_image
    
	
	;2 - criar arquivo
	;mov eax, bl_string
	;call fat16_create_file
    ;retorno eax = cluster inicial.
		
	;calcula destino
	;eax = cluster inicial
	mov eax, dword 2         ;BL.BIN (terceiro arquivo do fs)
	mov ebx, dword IMG_SIZE ;cluster size em ebx.
	mul ebx                 ;multiplica	
	add eax, dword 2336     ;adiciona o inicio da area de dados.	
	
	;3 - salva arquivo.
	mov dword [IMG_LBA_DESTINO], eax	
    call transf_save_image 
    jmp .done	
.fail:
    mov esi, msg_installer_bl_not_found
	call os_output		
    ret 
.done:
	mov dword [bootloader_install_status], dword 1
	
	;avisa que o bl.bin está instalado.
	mov eax, dword 1
	call blconfig_alterar_status_bl32	
	
	;salvar permanentemente as alterações
	call blconfig_salvar_alteracoes
	
	mov esi, installer_done_string
	call os_output	
    ret	



;------------------------------------------
; transf_load_image: 
;     Carrega imagem da lba de tranferencia 
;     para a memória de tranferência.
;	
transf_load_image:
	call init_ide0_master
	
    push eax
    push ebx
    push ecx
    
	;buffer
	mov eax, dword IMG_ADDRESS
	mov dword [.buffer], eax 
    
	;lba
	mov eax, dword [IMG_LBA_ORIGEM]
	mov dword [.lba], eax 
	
	;prepara o contador
	mov ecx, dword IMG_SIZE    ;Tamanho da partição.

	;Coloca um setor no buffer.	
.load_sector:	
	mov eax, dword [.buffer] ;buffer
	mov ebx, dword [.lba] ;lba
	call hd0_read_sector
    
	add dword [.buffer], 0200h
	add dword [.lba], 1
	
	loop .load_sector	
	
    clc
	pop ecx
	pop ebx
	pop eax
	ret
.buffer: dd 0	
.lba: dd 0


;------------------------------------
; transf_save_image:
;     Salva uma imagem da memória de tranferencia 
;     para o início da área de dados.
;	
transf_save_image:
	call init_ide0_master
    push eax
    push ebx
    push ecx

	;buffer
	mov eax, dword IMG_ADDRESS
	mov dword [.buffer], eax 
    
	;lba
	mov eax, dword [IMG_LBA_DESTINO]
	mov dword [.lba], eax 
	
	;prepara o contador
	mov ecx, dword IMG_SIZE ; tamanho do cluster

.write_sector:	
		;coloca um setor no buffer.
	mov eax, dword [.buffer] ;buffer
	mov ebx, dword [.lba] ;lba
	call hd0_write_sector
    
	add dword [.buffer], 0200h
	add dword [.lba], 1
	
	loop .write_sector	
    clc
	pop ecx
	pop ebx
	pop eax
	ret
.buffer: dd 0	
.lba: dd 0



;-----------------------------------
; installer: 
;     Instala um arquivo dado seu nome em eax.
;
; in: eax = nome do arquivo.
;
installer:   
    push eax
	push ebx
    
	;criar arquivo dado o nome em eax.
    ;retorna o numero do primeiro cluster.
	call fat16_create_file
	
	;eax = destino
    ;ebx = origem
    mov eax, dword 1024	
	call transferir
	
	mov esi, installer_done_string
	call os_output
	
	pop ebx
	pop eax
	ret
	
 
;--------------------------------------------
; transferir:
;     Transfere um arquivo da lba de origem para a lba de destino.
;
; eax = destino
; ebx = origem
; address 0x00009000 (buffer)
; size 1024
;
;++
transferir:  
	push eax
	push ebx
	
	;salva destino
	mov dword [IMG_LBA_DESTINO], eax	
	
	;calcula origem.
	mov eax, ebx            ;coloca em eax.
	mov ebx, dword IMG_SIZE ;cluster size em ebx. (1024 setores)
	mul ebx                 ;multiplica	
	add eax, dword 2336     ;adiciona o inicio da area de dados.	
	
	;salva origem
	mov dword [IMG_LBA_ORIGEM], eax

	
	;carrega na memória.
	call transf_load_image
    jc .fail	
	
	mov esi, msg_transf_image_carregado
	call os_output
	
    ;salva no sistema de arquivos.
	call transf_save_image
    jc .fail	
	
	mov esi, msg_transf_image_save
	call os_output
    
	jmp .done
.fail:
	mov esi, msg_transf_fail
	call os_output
.done:
	pop ebx
	pop eax
	ret
;--	


;
; Mensagens.
;

msg_transf_image_carregado  db "[Installer:] Arquivo carregado!", 13,10,0
msg_transf_image_save       db "[Installer:] Arquivo salvo.", 13,10,0
installer_done_string       db "[Installer:] Done!",13,10,0
msg_transf_fail             db "[Installer:] Fail!",13,10,0
msg_installer_bl_not_found  db "[Installer:] bl.bin not found",13,10,0	


;
;fim.
;
