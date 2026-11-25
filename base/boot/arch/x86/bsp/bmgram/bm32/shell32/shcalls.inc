; shell32/shcalls.inc
; It implements the services for some commands.
; 2015 - Created.

; =============================================
exit:
    jmp do_boot

;===============================================================
; do_boot:
; Iniciaremos o modo gr�fico padr�o.
; Chamaremos uma rotina que volta para o modo real, 
; muda o modo de v�deo e inicializa a GUI.
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
;     Seleciona parti��o 0.
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
;     Seleciona parti��o 1.
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
;     Seleciona parti��o 2.
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
;     Seleciona parti��o 3.
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
;     Ativa a parti��o atual.
;++
do_ativar:
    call fs32_ativar_particao_atual
    jmp os_command_line
;--

;----------------------------------------
; do_desativar:
;     Desativa a parti��o atual.
;++
do_desativar:
    call fs32_desativar_particao_atual
    jmp os_command_line
;--

;----------------------------------------
; hd_test:
;     Exibir informa��es sobre um disco
;     Testando opera��es de disco.
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
;     Exibir informa��es sobre uma parti��o.
;     Testa qual partic�o est� ativa.
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


; Nothing for now.
;testzone:
;    jmp os_command_line

;; debug messages
debug_message_fixme2_loading_sector:  
    db "[FIXME]: Loading sector on real machine?", 13, 10, 0

;
; End
;

