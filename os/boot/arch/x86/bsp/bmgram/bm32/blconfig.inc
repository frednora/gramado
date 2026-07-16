;
; File: blconfig.inc 
; Descrição:
; Rotinas para configurar os arquivos de inicialização system.cnf e init.cnf.
; @todo: rever os arquivos de inicialização. Pois estamos usando apenas
; um META$FILE.
; Obs:
;     Temos um arquivo de configurações de inicialização, de somente
; leitura chamado INIT.TXT, que será carregado posteriormente pelo Boot Loader.
; Talvez podéssemos ler alguma coisa nele antes do Boot Loader.
; Funções principais:
;     *alterar           - Coloca os valores em init.inc.
;     *salvar alterações - Salva init.inc em init.cnf. 
;                         (tornar permanente as alterações.)
; History:
;     2015 - Created by Fred Nora.



;===========================================
; blconfigSaveModifications: 
;     Salva init.inc em INIT.CNF (torna permanente).
;     #Suspensa.
;     @todo: Mudar o nome. 
;
blconfigSaveModifications:
    nop 
blconfig_salvar_alteracoes: 
    ;Nothing for now.
    ret
 

;---------------------------------------- 
; update_init_cnf:
;     Atualiza init.cnf à partir de init.inc
;     #suspensa.
;
update_init_cnf:
    ;Nothing for now.	
    ret	


;-----------------------------------
; blconfig_open_init_cnf:
;     Carrega INIT.
;     #suspensa.
;
blconfig_open_init_cnf:
    ;Nothing for now.
	ret

	
;----------------------------------------
; blconfig_save_init_cnf:
;     salva INIT.
;     #Suspensa.
;
blconfig_save_init_cnf:	
    ;Nothing for now.
    ret


;-------------------------------------------
; open_sys_cnf:
;     ??
;	
open_sys_cnf:
    ;Nothing for now.	
	ret
	

;---------------------------------------
; save_sys_cnf:
;
;	
save_sys_cnf:
    ;Nothing for now.
    ret


;--------------------------------------------------
; blconfig_transf_load_image:
;     Carrega imagem da lba de tranferencia 
;     para a memoria de tranferencia.
;	
blconfig_transf_load_image:
    ;Nothing for now.
	ret
	

;---------------------------------------------------
; blconfig_transf_save_image:
;     Salva uma imagem da memória de tranferencia 
;     para o início da área de dados.
;	
blconfig_transf_save_image:
    ;Nothing for now.	
	ret
	

;------------------------------------
; blconfig_alterar_modo:
;     @todo: Mudar nome.
;	
blconfig_alterar_modo:
    mov dword [BOOT.modo], eax
    ret


;--------------------------------------
; blconfig_alterar_disco_atual:
;     @todo: Mudar nome.
;	
blconfig_alterar_disco_atual:
    mov dword [BOOT.disco_atual], eax
    ret


;---------------------------------------
; blconfig_alterar_particao_atual:
;     @todo: Mudar nome.
;	
blconfig_alterar_particao_atual:
    mov dword [BOOT.particao_atual], eax
    ret


;---------------------------------------
; blconfig_alterar_bl_cluster:
;     @todo: Mudar nome.
;	
blconfig_alterar_bl_cluster:
    mov dword [BOOT.bl_cluster], eax
    ret

	
;--------------------------------------
; blconfig_alterar_bl_address:
;     @todo: Mudar nome.
;	
blconfig_alterar_bl_address:
    mov dword [BOOT.bl_address], eax
    ret


;--------------------------------------
; blconfig_alterar_bl_entry_point:
;     @todo: Mudar nome.
;
blconfig_alterar_bl_entry_point:
    mov dword [BOOT.bl_entry_point], eax
    ret

	
;--------------------------------------
; blconfig_alterar_status_bm16:
;     @todo: Mudar nome.
;
;	
blconfig_alterar_status_bm16:
    mov dword [BOOT.status_bm16], eax
    ret

	
;==============================================
; blconfig_alterar_status_bl32:
;     @todo: Mudar nome.
;	
blconfig_alterar_status_bl32:
    mov dword [BOOT.status_bl32], eax
    ret

	
;======================================================
; blconfig_set_modo:
;     Configura o modo de inicialização do bootloader.
;
; in: 
;     eax = Novo modo.
;
blconfig_set_modo:

	mov dword [BOOT.modo], eax
    
	;avisa que o modo foi alterado.
	mov eax, dword 1
	call blconfig_alterar_status_bm16	
	ret

	
;-----------------------------------------
; blconfig_get_modo:
;     Pega o modo de inicialização do bootloader.
;
; out: 
;     eax = modo.
;
;
blconfig_get_modo:
    mov eax, dword [BOOT.modo]
	ret
 

;--------------------------------------- 
; set_fs_volume:
;     Configura o volume que pertence o sistema de arquivos atual.
;
;
set_fs_volume:
    mov byte [FILESYSTEM.fs_volume], al
    ret

;--------------------------------------
; set_fs_type:	
;     Configura o tipo de sistema de arquivos.
;
;
set_fs_type:
    mov byte [FILESYSTEM.fs_type], al
    ret
	
	
;-------------------------------------
; set_volume_atual: 
;     Configura o endereço do volume atual.
;     @todo: Mudar nome.
;
; in:
;     eax = ??
;
;
set_volume_atual:
    mov dword [VOLUME.atual], eax
    ret

	
;=====================================================
; set_directory_atual:
;     Configura o endereço do directory atual.
;     @todo: Mudar nome.
;
; in:
;     eax = ??
;
set_directory_atual:
    mov dword [DIRECTORY.atual], eax
    ret

	
;====================================================
; pegar_parametros_da_particao_ativa:
;     Pega os parametros das partições.
;
blconfigGetActivePartitionInfo:
    nop
pegar_parametros_da_particao_ativa:
    push eax
	push esi
    
	;;
	;; ## bugbug ## rever o loval onde esse MBR vai ser carregado, não queremos problemas.
	;;
	;;suspensa ... (perderemos uma rotina mas não damos chance para esse bug sinistro.
	;call load_mbr    ;load MBR.

    ; Point to the partitiontable (512 = Sector size, 64 bytes for 
	; 4 partition tables, and 2 bytes for a partition table signature.	
	mov esi, dword (FAT16_MBR_ADDRESS + 446)
	
    ; The loop counter, for below
	mov ecx, dword 4			
.FindBootPartition:
    test byte [esi], 80h          ; Check if this partition is active (8th bit)
    jz .FindNext
	
	xor eax, eax
	
	mov al, byte  [esi+0] ;flag
	mov byte  [ParticaoAtiva.flag], al                
		
	mov al, byte  [esi+1] ;h
    mov byte  [ParticaoAtiva.h_inicial], al

	mov al, byte  [esi+2] ;c
    mov byte  [ParticaoAtiva.c_inicial], al

    mov al, byte  [esi+3] ;s	
    mov byte  [ParticaoAtiva.s_inicial], al

   	mov al, byte  [esi+4] ;os type
    mov byte  [ParticaoAtiva.os_type], al

    mov al,	byte  [esi+5] ;h
    mov byte  [ParticaoAtiva.h_final], al
	
	mov al, byte  [esi+6] ;c
    mov byte  [ParticaoAtiva.c_final], al             
    
	mov al, byte  [esi+7] ;s
	mov byte  [ParticaoAtiva.s_final], al             
    
	mov eax, dword [esi+8] ;lba inicial
	mov dword [ParticaoAtiva.lba_inicial], eax

	mov eax, dword [esi+12] ;tamanho da partição
    mov dword [ParticaoAtiva.tamanho_da_particao], eax  

	jmp .done

.FindNext:
        add esi, 16                       ; Point to the next partition entry
        loop .FindBootPartition		; and try again
.NoPart:
	mov esi, msg_NoPart
	call os_output
    pop esi
    pop eax
    stc	
	ret

.done:
    pop esi
	pop eax
	ret
	
	
;======================================================
; bl_config_ativar_status:
;     Configura a inicialização utilizando 
;     os arquivos SYSTEM.CNF e INIT.CNF.
;     @todo: Mudar nome. 
;
bl_config_ativar_status:
	;faz mudanças
	call open_init_cnf
	mov byte [g_init_status_bl32], byte 1
	call save_init_cnf
    ret
 
 
;============================================
; open_init_cnf:	
;     Abre init.
;	
open_init_cnf:
    call blconfig_open_init_cnf
	ret

	
;=============================================	
; save_init_cnf:
;     Save init.
;
save_init_cnf:
    call blconfig_save_init_cnf
	ret


;=========================================
; blconfig_set_modo1:
;     Salva o modo 1.
;
blconfig_set_modo1:
    push eax
	
	;abre o arquivo
	call open_init_cnf
	;salva o valor
	mov eax, dword 1
	call blconfig_set_modo	
	;salva
	call save_init_cnf
   
	pop eax	
    ret

	
;=======================================
; blconfig_set_modo2:	
;     Salva o modo 2.
;
blconfig_set_modo2:
    push eax
	
	;abre o arquivo
	call open_init_cnf
	;salva o valor
	mov eax, dword 2
	call blconfig_set_modo	
	;salva
	call save_init_cnf
    
	pop eax	
    ret

	
;=========================================
; blconfig_set_modo3:	
;     Salva o modo 3.
;
blconfig_set_modo3:
    push eax
	
	;abre o arquivo
	call open_init_cnf
	;salva o valor
	mov eax, dword 3
	call blconfig_set_modo	
	;salva
	call save_init_cnf
    
	pop eax	
    ret

	
;==================================
; blconfig_set_modo4:	
;     Salva o modo 4.
;
blconfig_set_modo4:
    push eax
	
	;abre o arquivo
	call open_init_cnf
	;salva o valor
	mov eax, dword 4
	call blconfig_set_modo	
	;salva
	call save_init_cnf
    
	pop eax	
    ret
	
	
;================================================================
; make_boot:
;     Contrói o diretório de boot na primeira partição.
;     Obs: Isso será útil em algum tipo de intaller.
;
; BOOT.xxxx
;.modo:           dd 0  ;+0  (step 1) modo de inicialização do boot manager.
;.disco_atual:    dd 0  ;+4  (step 2) disco atual 
;.particao_atual: dd 0  ;+8  (step 3) partição atual 
;.bl_clsuter:     dd 0  ;+12 (step 4) lba inicial do bl.bin
;.bl_address:     dd 0  ;+16 (step 5) endereço de bl.bin
;.bl_entry_point: dd 0  ;+20 (step 6) entrypoint de bl.bin 
;.res7:           dd 0  ;+24 (step 7) res.
;.res8:           dd 0  ;+28 (step 8) res. 
;
;
make_boot:
    pushad

	mov dword [BOOT.modo],            dword 2    ;cli.
	mov dword [BOOT.disco_atual],     dword 0
	mov dword [BOOT.particao_atual],  dword 0
	mov dword [BOOT.bl_cluster],      dword 2
	mov dword [BOOT.bl_address],      dword 0x00009000
	mov dword [BOOT.bl_entry_point],  dword 0x00009200
	;mov dword [BOOT.res7]
	;mov dword [BOOT.res8]
	
    
	;
	; Cria arquivos.
	;

	;0, Cria o arquivo 'system.cnf'.	
	;1, Cria arquivo 'init.cnf'.	
	;2, Cria 'bl.bin' (Boot Loader).
	;3, /boot.
	;4, /system.
	;5, /user.
	
	call create_sys_cnf_file 
	call create_init_cnf_file 
	call create_bl_file 
	call create_boot_dir
	call create_system_dir
	call create_user_dir	
    
	; More ??

	popad	
    ret
	
	
;0
create_sys_cnf_file:
  ;  pushad    
     	
	mov eax, init_sys_file_string
	call fat16_force_create_file
	
  ;  popad		
	ret	


;1
create_init_cnf_file:
  ;  pushad    
     	
	mov eax, init_cnf_file_string
	call fat16_force_create_file
	
  ;  popad		
	ret	


;2
create_bl_file:
  ;  pushad    
     	
	mov eax, init_bl_file_string
	call fat16_force_create_file
	
  ;  popad		
	ret	

	
;3	
create_boot_dir:
  ;  pushad    
     	
	mov eax, pasta_boot_string
	call fat16_force_create_dir	
	mov word [BOOTDIR.cluster], ax
	;NA VERDADE SERA (AX) + (INICIO DA AREA DE DADOS) 2336  
	
  ;  popad		
	ret	


;4
create_system_dir:
  ;  pushad    
     	
	mov eax, pasta_system_string
	call fat16_force_create_dir
	mov word [SYSTEMDIR.cluster], ax
	
  ;  popad		
	ret	


;5 
create_user_dir:
  ;  pushad    
     	
	mov eax, pasta_user_string
	call fat16_force_create_dir
	mov word [USERDIR.cluster], ax
	
  ;  popad		
	ret
	
	
;====================================================
; blconfigFindInitialLBA:
;     Encontrar na tabela de partição 
;     a lba inicial da partição ativa.
;     @todo: Mudar nome.
;
blconfigFindInitialLBA:
    nop
encontrar_lba_inicial:
    
	pushad
	
	;carrega o mbr em FAT16_MBR_ADDRESS
	call load_mbr    
	
	mov al, byte [FAT16_MBR_ADDRESS + 446 + 0 ] ;flag	
	cmp al, 0x80
	je .p0	
	mov al, byte [FAT16_MBR_ADDRESS + 446 + 16] ;flag	
	cmp al, 0x80
	je .p1
	mov al, byte [FAT16_MBR_ADDRESS + 446 + 32] ;flag	
	cmp al, 0x80
	je .p2
	mov al, byte [FAT16_MBR_ADDRESS + 446 + 48] ;flag	
	cmp al, 0x80
	je .p3
	
	;nenhuma
	mov esi, dword msg_pX 
	call os_output	
	jmp .exit_p
	
	
.p0:
	mov esi, dword msg_p0_lba 
	call os_output	

	mov eax, dword [FAT16_MBR_ADDRESS + 446 + 0 + 8]
    call print_hex ; imprime flag em hexa
	
    mov esi, dword msg_nextline
	call os_output
	
	mov byte [ParticaoAtiva.lba_inicial], byte 0
	jmp .exit_p
		
.p1:
	mov esi, dword msg_p1_lba 
	call os_output

	mov eax, dword [FAT16_MBR_ADDRESS + 446 + 16 + 8]
    call print_hex ; imprime flag em hexa
	
    mov esi, dword msg_nextline
	call os_output
	
	mov byte [ParticaoAtiva.lba_inicial], byte 1
    jmp .exit_p
	
.p2:
	mov esi, dword msg_p2_lba 
	call os_output

	mov eax, dword [FAT16_MBR_ADDRESS + 446 + 32 + 8]
    call print_hex ; imprime flag em hexa
	
    mov esi, dword msg_nextline
	call os_output
	
	mov byte [ParticaoAtiva.lba_inicial], byte 2   
	jmp .exit_p	
		
.p3:
	mov esi, dword msg_p3_lba 
	call os_output
    
	mov eax, dword [FAT16_MBR_ADDRESS + 446 + 48 + 8]
    call print_hex ; imprime flag em hexa

    mov esi, dword msg_nextline
	call os_output	
	
	mov byte [ParticaoAtiva.lba_inicial], byte 3
    jmp .exit_p	

.exit_p:
	popad
    ret
	
	
;--------------------------------------------
; set_current_disk_number:
;
; IN:
;     al = disk number.
;
set_current_disk_number:
    mov byte [CurrentDisk.disk_number], al
	ret
	
	
;------------------------------------------
; set_current_disk_particao_ativa:
;
; IN:
;     al = particao ativa.
;
set_current_disk_particao_ativa:
    mov byte [CurrentDisk.particao_ativa], al
	ret
	

;-----------------------------------------
; set_current_disk_partition_number:
;
; IN:	
;     al = particao selecionada.
;
;
set_current_disk_partition_number:
    mov byte [CurrentDisk.partition_number], al
	ret


;---------------------------------------
; get_current_disk_number:
;
;
; IN:	
;     al = disk number.
;
get_current_disk_number:
    mov al, byte [CurrentDisk.disk_number]
	ret

;-----------------------------------------
; get_current_disk_particao_ativa:
;
; IN:	
;     al = particao ativa.
;
;
get_current_disk_particao_ativa:
    mov al, byte [CurrentDisk.particao_ativa]
	ret
	

;-----------------------------------------
; get_current_disk_partition_number:
;
;
; IN:	
;     al = particao selecionada.
;
;
get_current_disk_partition_number:
    mov al, byte [CurrentDisk.partition_number]
	ret
	
	
;-------------------------------------------
; fs32_ativar_particao_atual:	
;     Ativa a partição atual.
;
fs32_ativar_particao_atual:
    mov byte [CurrentPartition.flag], byte 0x80    
	ret
	
	
;--------------------------------------------
; fs32_desativar_particao_atual:	
;     Desativa a partição atual.
;	
fs32_desativar_particao_atual:
    mov byte [CurrentPartition.flag], byte 0    
	ret
	

;-----------------------------------------
; fs32_save_current_partition:
;
;
fs32_save_current_partition:
    
	;somente o disco 0 por enquanto
	cmp byte [CurrentDisk.disk_number], byte 0
    jne .disk_fail	
	
	;salva uma das quatro partições
    cmp byte [CurrentDisk.partition_number], byte 0
    je .do_saveP0	
	cmp byte [CurrentDisk.partition_number], byte 1	
    je .do_saveP1
	cmp byte [CurrentDisk.partition_number], byte 2	
    je .do_saveP2
	cmp byte [CurrentDisk.partition_number], byte 3		
	je .do_saveP3
	
	jmp .partition_fail
	
	;.....

.do_saveP0:
    mov al, byte  [CurrentPartition.flag]  ;flag
	mov byte  [FAT16_MBR_ADDRESS + 446 + 0 + 0 ], al
    mov eax, dword [CurrentPartition.lba_inicial]         ;lba inicial 	
	mov dword [FAT16_MBR_ADDRESS + 446 + 0 + 8 ], eax 	
	mov eax, dword [CurrentPartition.tamanho_da_particao] ;tamanho
	mov dword [FAT16_MBR_ADDRESS + 446 + 0 + 12], eax 	    
	;... salvar os outros parametros.
	jmp .do_save
	
.do_saveP1:
    mov al, byte  [CurrentPartition.flag]  ;flag
	mov byte  [FAT16_MBR_ADDRESS + 446 + 16 + 0 ], al
    mov eax, dword [CurrentPartition.lba_inicial]         ;lba inicial 	
	mov dword [FAT16_MBR_ADDRESS + 446 + 16 + 8 ], eax 	
	mov eax, dword [CurrentPartition.tamanho_da_particao] ;tamanho
	mov dword [FAT16_MBR_ADDRESS + 446 + 16 + 12], eax 	    
	;... salvar os outros parametros.
	jmp .do_save

.do_saveP2:
    mov al, byte  [CurrentPartition.flag]  ;flag
	mov byte  [FAT16_MBR_ADDRESS + 446 + 32 + 0 ], al
    mov eax, dword [CurrentPartition.lba_inicial]         ;lba inicial 	
	mov dword [FAT16_MBR_ADDRESS + 446 + 32 + 8 ], eax 	
	mov eax, dword [CurrentPartition.tamanho_da_particao] ;tamanho
	mov dword [FAT16_MBR_ADDRESS + 446 + 32 + 12], eax 	    
	;... salvar os outros parametros.
	jmp .do_save

.do_saveP3:
    mov al, byte  [CurrentPartition.flag]  ;flag
	mov byte  [FAT16_MBR_ADDRESS + 446 + 48 + 0 ], al
    mov eax, dword [CurrentPartition.lba_inicial]         ;lba inicial 	
	mov dword [FAT16_MBR_ADDRESS + 446 + 48 + 8 ], eax 	
	mov eax, dword [CurrentPartition.tamanho_da_particao] ;tamanho
	mov dword [FAT16_MBR_ADDRESS + 446 + 48 + 12], eax 	    
	;... salvar os outros parametros.
	jmp .do_save
	
.disk_fail:
    mov esi, msg_disk_fail
	call os_output
	jmp .exit_save
	
.partition_fail:
    mov esi, msg_partition_fail
	call os_output
	jmp .exit_save
 
.do_save:
	call save_mbr
	
.exit_save:	
    ret
	
	
;================================================
; fs32_select_current_disk:
;     Seleciona um disco atual.
; IN: Al
;
fs32_select_current_disk: 
    call select_hd
    mov dword [CurrentDisk.disk_number], eax 
    ret

	
;
; fs32_select_current_partition:
;     Seleciona a partição atual.
;	
fs32_select_current_partition:
    mov dword [CurrentDisk.partition_number], eax	
    ret


;=====================================================
; save_partition_lba:
;     Salva a lba inicial do disco atual.	
;
; IN: 
;     eax = Número do disco.
;
save_partition_lba:
	mov dword [CurrentPartition.lba_inicial], eax
	ret
	
	
	
;-------------------------------------------
; save_partition_flags:
;     Salvar os parametros da partição.	
;
; IN: 
;     edx = Número do disco.
;           (número da entrada).
;
save_partition_flags:
    
	;flag da partição 0,1,2,3
    push eax	
	xor eax, eax	
	mov al, byte [FAT16_MBR_ADDRESS +446 +0 ]  ;flag.    
    mov byte [CurrentDisk.flag_p0], al
	mov al, byte [FAT16_MBR_ADDRESS +446 +16] ;flag.    
    mov byte [CurrentDisk.flag_p1], al
	mov al, byte [FAT16_MBR_ADDRESS +446 +32] ;flag.    
    mov byte [CurrentDisk.flag_p2], al
	mov al, byte [FAT16_MBR_ADDRESS +446 +48] ;flag.    
    mov byte [CurrentDisk.flag_p3], al
	pop eax
	
	ret
	
	
	
;
; save_disk_number:
;     Salva o número do disco atual.	
;
; IN: 
;     eax = Número do disco.
;
save_disk_number:
	mov dword [CurrentDisk.disk_number], eax
	ret

	
;================================================
; blconfigFindActivePartition:
;     Encontra qual partição está ativa.
;
blconfigFindActivePartition:
    nop	
encontra_particao_ativa:
    
	pushad
	
	call load_mbr    ;Load MBR.    
	
	;Check flag.
	mov al, byte [FAT16_MBR_ADDRESS +446 +0 ]    ;Flag.	
	cmp al, 0x80
	je .p0	
	mov al, byte [FAT16_MBR_ADDRESS +446 +16]    ;Flag.	
	cmp al, 0x80
	je .p1
	mov al, byte [FAT16_MBR_ADDRESS +446 +32]    ;Flag.	
	cmp al, 0x80
	je .p2
	mov al, byte [FAT16_MBR_ADDRESS +446 +48]    ;Flag.	
	cmp al, 0x80
	je .p3
	
	;Mensagem de Nenhuma encontrada.
	mov esi, dword msg_pX 
	call os_output	
	
	jmp .exit_p
	
;;	
.p0:
	mov esi, dword msg_p0 
	call os_output	

	;Imprime flag em hexa.	
    xor eax, eax
	mov al, byte [FAT16_MBR_ADDRESS + 446 + 0]
    call print_hex 
	
    mov esi, dword msg_nextline
	call os_output
	
	mov byte [CurrentDisk.particao_ativa], byte 0
	mov byte [CurrentDisk.flag_p0], byte 0x80
	jmp .exit_p
	
;;
.p1:
	mov esi, dword msg_p1 
	call os_output

	;Imprime flag em hexa.
    xor eax, eax
	mov al, byte [FAT16_MBR_ADDRESS + 446 + 16]
    call print_hex

    mov esi, dword msg_nextline
	call os_output	
	
	mov byte [CurrentDisk.particao_ativa], byte 1
	mov byte [CurrentDisk.flag_p1], byte 0x80
    jmp .exit_p
	
;;
.p2:
	mov esi, dword msg_p2 
	call os_output

	;Imprime flag em hexa.
    xor eax, eax
	mov al, byte [FAT16_MBR_ADDRESS + 446 + 32]
    call print_hex

    mov esi, dword msg_nextline
	call os_output	
	
	mov byte [CurrentDisk.particao_ativa], byte 2  
    mov byte [CurrentDisk.flag_p2], byte 0x80	
	jmp .exit_p	
	
;;
.p3:
	mov esi, dword msg_p3 
	call os_output

	;Imprime flag em hexa.
    xor eax, eax
	mov al, byte [FAT16_MBR_ADDRESS + 446 + 48]
    call print_hex

    mov esi, dword msg_nextline
	call os_output	
	
	mov byte [CurrentDisk.particao_ativa], byte 3
	mov byte [CurrentDisk.flag_p3], byte 0x80
    jmp .exit_p	

.exit_p:
	popad
	RET
	
	
	
;-------------------------------------------
; get_current_path:
;     Pega o endereço do nome do arquivo atual.
;
;
; out: 
;     eax = Endereço do nome do arquivo atual.
;
get_current_path:
    mov eax, dword CURRENT_PATH
	ret

	
;---------------------------------------------
; set_current_path: 
;     Configura o endereço do nome do arquivo atual.
;
; in: 
;     eax = Endereço do nome do arquivo atual.
;
set_current_path:
    
	;Pega origem e destino.
	mov esi, eax
	mov edi, dword CURRENT_PATH
	
	;contador.
	mov ecx, dword 250
	push ecx

.next_ch:	
	mov ecx, dword 1
		
	;pega o primeiro
	mov al, byte [esi]
	
	;algum espaço
	cmp al, byte ' '
	je .fim
	
	;fim da string
	cmp al, byte 0
	je .fim
	
	movsb ;incrementa esi edi. ??
	
	pop ecx
	loop .next_ch ;250 vezes	

.fim:
    ;filaniza string
    mov al, byte 0
	stosb
    ret
	
	
	
;-----------------------------------
; save_current_partition:
;     Salva os parametros da partição atual, 
;     dado o numero do disco atual.
;
save_current_partition:
    ;pushad  
	;Nothing for now.
	;popad    
    ret
	
	
;---------------------------------------
; save_dir_entry:
;     Salva uma entrada especifica no diretório de partições.
;
; in: 
;     eax = Número da partição. 
;     ebx = Disco.
;
save_dir_entry:
    ;pushad
	;Nothing for now.	
	;popad    
    ret
	
	
;
; Variáveis.
;

CNF_IMG_LBA_ORIGEM:  dd 0     
CNF_IMG_LBA_DESTINO: dd 0   
	



;
; CurrentImage.xxxx
;     variaveis - struct
;
CurrentImage:
.filename     dd 0  ;Contém o endereço onde esta o nome do arquivo.
.buffer       dd 0
.lba_inicial  dd 0

	
;
;Mensagens.	
;

;names
systemcnf_string  db "SYSTEM  CNF",0	
initcnf_string    db "INIT    CNF",0
file_name_test    db "KERNEL  BIN",0

;/BOOT
dir_name_test     db "BOOT       ",0

;dirs
init_sys_file_string  db "system  cnf",0
init_cnf_file_string  db "init    cnf",0
init_bl_file_string   db "bl      bin",0
pasta_boot_string     db "boot    dir",0
pasta_system_string   db "system  dir",0
pasta_user_string     db "user    dir",0

;blconfig
msg_pX         db "blconfig: No active partition", 13, 10, 0
msg_p0         db "blconfig: Active partition,(0) ", 13, 10, 0	
msg_p1         db "blconfig: Active partition,(1) ", 13, 10, 0
msg_p2         db "blconfig: Active partition,(2) ", 13, 10, 0
msg_p3         db "blconfig: Active partition,(3) ", 13, 10, 0
msg_p0_lba     db "blconfig: LBA,(p0) ", 13, 10, 0	
msg_p1_lba     db "blconfig: LBA,(p1) ", 13, 10, 0
msg_p2_lba     db "blconfig: LBA,(p2) ", 13, 10, 0
msg_p3_lba     db "blconfig: LBA,(p3) ", 13, 10, 0	
;...

;messages
msg_NoPart         db "No partition",13,10,0
msg_disk_fail      db "No support for this disk",13,10,0
msg_partition_fail db "No support for this partition",13,10,0
;...

;Next line
msg_nextline: db '*', 13, 10, 0

;
; End.
;
