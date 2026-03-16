;
; File: shell32/shcalls.inc
; Descrição:
;     Rotinas para atender algumas chamadas do Boot Manager.
; 2015 - Created.

; =============================================
exit:
    jmp do_boot

;===============================================================
; do_boot:
; Iniciaremos o modo gráfico padrão.
; Chamaremos uma rotina que volta para o modo real, 
; muda o modo de vídeo e inicializa a GUI.
; See: stage3/pm.inc 
; Default to 800x600.
do_boot:
; 320x200
    ;jmp do_gramado_mode_jail
; 640x480
    ;jmp do_gramado_mode_p1
; 800x600
    jmp do_gramado_mode_home
; 1024x768
    ;jmp do_gramado_mode_p2
; ?x?
    ;jmp do_gramado_mode_castle
    jmp do_boot
; =============================================

;===============================================================
; Trampoline:
; Start a gui mode given its number.
; See: stage3/pm.inc 
do_gramado_mode_jail:
    mov byte [META$FILE.GRAMADO_MODE], 0x00
    jmp rmStartGUI
do_gramado_mode_p1:
    mov byte [META$FILE.GRAMADO_MODE], 0x01
    jmp rmStartGUI
do_gramado_mode_home:
    mov byte [META$FILE.GRAMADO_MODE], 0x02
    jmp rmStartGUI
do_gramado_mode_p2:
    mov byte [META$FILE.GRAMADO_MODE], 0x03
    jmp rmStartGUI
do_gramado_mode_castle:
    mov byte [META$FILE.GRAMADO_MODE], 0x04
    jmp rmStartGUI
do_gramado_mode_california:
    mov byte [META$FILE.GRAMADO_MODE], 0x05
    jmp rmStartGUI
;===============================================================

;==============================================================
; do_shutdown:
; Come back to real mode and 
; shutdown the machine via APM, using int 0x15.
do_shutdown:
    jmp rmShutdown

;==============================================================
; do_mount_volume:
;     Montando o volume do sistema. (testing...)
;     Obs: O volume do sistema será um subdiretório 
;          do diretório de volumes.

do_mount_volume:

    pushad

    call os_screen_clear 
    call reset_cursor

    ;Carregando o diretório raiz.
    ;call load_root_first_sector 

	
	;Pega a base do diretório raiz. (diretório de volumes).
	;mov esi, dword FAT16_ROOTDIR_ADDRESS
	;mov edi, system_volume_string
	;call os_string_compare
	;jc .ok	
	;jmp .fail

.ok:
	;Setup current dir.   
    mov dword [g_current_fat_lba], 256        ;Fat do volume atual
    mov dword [g_current_directory_lba], 510  ;Diretório do volume atual.   
    mov dword [g_current_dataarea_lba],  542  ;?? ;lba do inicio da area de dados do volume atual. 

	;load FAT
	;Inicia ide.
	call init_ide0_master
	;Coloca um setor da fat no buffer.
	mov eax, dword FAT16_FAT_ADDRESS
	mov ebx, dword [g_current_fat_lba] ;LBA da fat do diretório atual. 
	call hd0_read_sector
	
	;Load DIR
	;Inicia ide.
	call init_ide0_master	
	;coloca um setor no buffer.
	mov eax, dword FAT16_ROOTDIR_ADDRESS
	mov ebx, dword [g_current_directory_lba] 
	call hd0_read_sector
	
	jmp .done
.fail:
    nop
.done:
    popad
    jmp os_command_line
 


;==============================================================
; debug:
;     Mostra as configurações de inicialização 
;     que estão no arquivo INIT.CNF.
;++

debug:
    pushad
    call debug_main
    popad
    jmp os_command_line
;--


;==============================================================
; do_save:
;     Torna permanente as alterações de 
;     inicialização e partições.
;++
do_save:
    ;pushad
    ;Torna permanente as alterações de inicialização.
    ;call blconfig_salvar_alteracoes   
    ;Torna permanente as alterações nas partições.
    ;call fs32_save_current_partition  
    ;popad
    jmp os_command_line
;--


;==============================================================
; do_formatdisk:
;     Formata um disco.
;++	

do_formatdisk:
    ;pushad
    ;call format_disk
    ;popad
    jmp os_command_line
;--

;==============================================================
; do_make_boot:
;     Cria diretórios e arquivos para Boot.
;++
do_make_boot:  
    ;pushad
    ;call make_boot
    ;popad
    jmp os_command_line
;--

;==============================================================
; do_install:
;     Transfere conteúdo do disco 
;     de uma lba para outra.
;     (da area reservada(lba 1024) para o sistema de arquivos.)
;++
do_install:
    ;#suspensa!
    ;call install_bootloader ; instala o bootloader.
    ;mov eax, file1_string 
    ;call installer      ;executa o installer
    jmp os_command_line
;--

;==============================================================
; do_set_modo1:
;     Configura modo1.
;++
do_set_modo1:
    push eax
    call blconfig_set_modo1
    mov eax, dword 1
    call blconfig_alterar_status_bm16
    pop eax
    jmp os_command_line
;--

;==============================================================
; do_set_modo2:
;     Configura modo2.
;++
do_set_modo2:
    push eax
    call blconfig_set_modo2
    mov eax, dword 1
    call blconfig_alterar_status_bm16
    pop eax
    jmp os_command_line
;--

;==============================================================
; do_set_modo3:
;     Configura modo3.
;++
do_set_modo3:
    push eax
    call blconfig_set_modo3
    mov eax, dword 1
    call blconfig_alterar_status_bm16
    pop eax
    jmp os_command_line
;--

;==============================================================
; do_set_modo4:
;     Configura modo4.
;++
do_set_modo4:
    push eax
    call blconfig_set_modo4
    mov eax, dword 1
    call blconfig_alterar_status_bm16
    pop eax
    jmp os_command_line
;--

;==============================================================
; do_set_v0:
; Configura volume 0.
do_set_v0:
    push eax
    mov eax, VOLUME.p0_name
    call set_volume_atual
    pop eax
    ret
;--

;==============================================================
; do_set_v1:
; Configura volume 1.
do_set_v1:
    push eax
    mov eax, VOLUME.p1_name
    call set_volume_atual
    pop eax
    ret
;--

;==============================================================
; do_set_v2:
; Configura volume 2.
do_set_v2:
    push eax
    mov eax, VOLUME.p2_name
    call set_volume_atual
    pop eax
    ret
;--

;==============================================================
; do_set_v3:
; Configura volume 3.
do_set_v3:
    push eax
    mov eax, VOLUME.p3_name
    call set_volume_atual
    pop eax
    ret
;--

;==============================================================
; do_openfile:
;     Abre um arquivo.
do_openfile:
    mov byte[file_flag], 1
    jmp os_command_line
;--

;==============================================================
; do_makefile:
;     Cria um arquivo.
;++

do_makefile:
    mov byte[file_flag], 2
    jmp os_command_line
;--

;==============================================================
; do_deletefile: 
;     Deleta um arquivo.
;++
do_deletefile:
    mov byte[file_flag], 3
    jmp os_command_line
;--

;==============================================================
; do_opendir:
;     Abre um diretório.
;++
do_opendir:
    mov byte[file_flag], 4
    jmp os_command_line
;--

;==============================================================
; do_makedir:
;     Cria um diretório.
;++
do_makedir:
    mov byte[file_flag], 5
    jmp os_command_line
;--

;==============================================================
; do_deletedir:
;    Deleta um diretório.
;++
do_deletedir:
    mov byte[file_flag], 6
    jmp os_command_line
;--

;==============================================================
; do_changedir:
;      Muda o diretório atual.
do_changedir:
    jmp os_command_line

;==============================================================
; ls:
; lista os arquivos.
; Apresentação no estilo unix.
ls:
    jmp os_command_line

;==============================================================
; dir:
; Show the entries in the root dir.
; #bugbug: 
; It is not working on my real machine. (fred)
; See: 
; fs32.inc
;++
dir:
    ;call os_screen_clear 
    ;call reset_cursor
    call fs32_show_rootdir
    jmp os_command_line
;--

;----------------------------------
; do_seleciona_d0:
;     Seleciona disco 0.
;++ 
do_seleciona_d0:
    push eax
    mov eax, dword 0
    call fs32_select_current_disk
    pop eax
    jmp os_command_line
;--

;-----------------------------------
; do_seleciona_d1:
;     Seleciona disco 1.
;++ 
do_seleciona_d1:
    push eax
    mov eax, dword 1
    call fs32_select_current_disk
    pop eax
    jmp os_command_line
;--

;------------------------------------
; do_seleciona_d2:
;     Seleciona disco 2.
;++ 
do_seleciona_d2:
    push eax
    mov eax, dword 2
    call fs32_select_current_disk
    pop eax
    jmp os_command_line
;--

;-------------------------------------
; do_seleciona_d3:
;     Seleciona disco 3.
;++ 

do_seleciona_d3:
    push eax
    mov eax, dword 3
    call fs32_select_current_disk
    pop eax
    jmp os_command_line
;--

;-------------------------------------
; do_seleciona_p0:
;     Seleciona partição 0.
;++
do_seleciona_p0:
    push eax
    mov eax, dword 0
    call fs32_select_current_partition
    pop eax
    jmp os_command_line
;--

;-------------------------------------
; do_seleciona_p1:
;     Seleciona partição 1.
;++
do_seleciona_p1:
    push eax
    mov eax, dword 1
    call fs32_select_current_partition
    pop eax
    jmp os_command_line
;--

;--------------------------------------
; do_seleciona_p2:
;     Seleciona partição 2.
;++ 
do_seleciona_p2:
    push eax
    mov eax, dword 2
    call fs32_select_current_partition
    pop eax
    jmp os_command_line
;--

;--------------------------------------
; do_seleciona_p3:
;     Seleciona partição 3.
;++
do_seleciona_p3:
    push eax
    mov eax, dword 3
    call fs32_select_current_partition
    pop eax
    jmp os_command_line
;--

;----------------------------------------
; do_ativar:
;     Ativa a partição atual.
;++
do_ativar:
    call fs32_ativar_particao_atual
    jmp os_command_line
;--

;----------------------------------------
; do_desativar:
;     Desativa a partição atual.
;++
do_desativar:
    call fs32_desativar_particao_atual
    jmp os_command_line
;--

;----------------------------------------
; hd_test:
;     Exibir informações sobre um disco
;     Testando operações de disco.
;++
hd_test:
    pushad
    ;call os_screen_clear
    ;call reset_cursor
; hardware.inc
; mostrar_parametros_do_disco_atual
    call hardware_show_currentdisk_info 
    popad
    jmp os_command_line
;--

;----------------------------------------
; shellcallsPartitionTest:
;     Exibir informações sobre uma partição.
;     Testa qual particão está ativa.
;++
shellcallsPartitionTest:
    nop
particao_test:
    pushad
    
    ;call os_screen_clear
    ;call reset_cursor

    ;; #debug
    mov esi, debug_message_fixme2_loading_sector
    call os_output

    call blconfigFindActivePartition     ; encontra_particao_ativa
    call blconfigFindInitialLBA          ; encontrar_lba_inicial
    call blconfigGetActivePartitionInfo  ; pegar_parametros_da_particao_ativa
    call fs32ShowActivePartitionInfo     ; fs32_show_activepartition_info

    popad
    jmp os_command_line
;--

;--------------------------------
; print_help:
;     Mostra mensagem de ajuda.
;++
print_help:
    ;pushad
    push esi
    mov esi, help_text
    call os_output
    pop esi
    ;popad
    jmp os_command_line
;--


;----------------------------------
; shell_clear_screen:
;     Limpa a tela e reapresenta o cursor.
;     @todo: Talvez deva reiniciar os registradores.
;++

shell_clear_screen:
    ;call clear_screen
    call os_screen_clear
    call reset_cursor 
    jmp os_command_line
;--

;;========================================
;; print_ver:
;;     Print version string.
;;++

print_ver:
    push esi
    mov esi, version_msg
    call os_output
    pop esi
    jmp os_command_line
;;--

;=========================================================
; fail:
;     We didn't get a valid command or program name.
;++

fail:
    nop
shellcallsFail:
    push esi
    mov esi, not_found_msg
    call os_output
    pop esi
    jmp os_command_line
;--

; #deprecated.
;os_bmfs_file_list:
;    ret 

; #deprecated.
testzone:
    jmp os_command_line
;--

;; debug messages
debug_message_fixme2_loading_sector:  
    db "[FIXME]: Loading sector on real machine?", 13, 10, 0

;
; End.
;

