;
; File: debug.inc 
;
; goal: Show some initialization info.
;
; 2015.
;
 

;-----------------------------------------------
; debug_main:
;     main function.
;

debug_main:
    pushad
    call debug_show_metafile_info
    popad

    xor eax, eax
    RET


;-------------------------------------------
; debug_show_metafile_info:
;
;     Analiza e mostra informa��es sobre o sistema. 
;     Mostra as informa��es salvas em META$FILE.
;
debug_show_metafile_info:

	;mov esi, new_line
    ;call  os_output 
	
	mov esi, msg_debug_about
    call  os_output 	

	;1
    mov esi, msg_debug_1
    call  os_output
	xor eax, eax
	mov AL, byte [META$FILE.INIT_MODE]
    call print_hex   

	;2
    mov esi, msg_debug_2
    call  os_output
	xor eax, eax
	mov ax, word [META$FILE.VIDEO_MODE]
    call print_hex 

	;3
    mov esi, msg_debug_3
    call  os_output
	xor eax, eax
	mov ax, word [META$FILE.SCREEN_X]
    call print_hex   

	;4
    mov esi, msg_debug_4
    call  os_output
	xor eax, eax
	mov ax, word [META$FILE.SCREEN_Y]
    call print_hex   
	
	;5
    mov esi, msg_debug_5
    call  os_output
	xor eax, eax
	mov eax, dword [META$FILE.LFB]
    call print_hex   
	
	;6
    ;mov esi, msg_debug_6
    ;call  os_output
	;xor eax, eax
	;mov eax, dword [META$FILE.]
    ;call print_hex  
	
	;7
    ;mov esi, msg_debug_7
    ;call  os_output
	;xor eax, eax
	;mov eax, dword [META$FILE.]
    ;call print_hex   
  
	;8
    ;mov esi, msg_debug_8
    ;call  os_output
	;xor eax, eax
	;mov eax, dword [META$FILE.]
    ;call print_hex 
	
    ;mov esi, new_line
    ;call  os_output 

    mov esi, new_line
    call  os_output 

    ret



;
; debug_config_ativar_status:
;     Critico ... testar
;
debug_config_ativar_status:
	;faz mudan�as
	call blconfig_open_init_cnf

	mov byte [g_init_status_bl32], byte 1
	
	call blconfig_save_init_cnf
    ret

	
;
; debug_show_system_cnf:
;
;
debug_show_system_cnf:
   
    mov esi, new_line
    call  os_output 
   
   ;carrega system.cnf em SYSTEM
	mov esi, msg_check_system_cnf
	call os_output
	call open_sys_cnf

	;ver primeira letra do nome do sistema
	mov al, byte [g_system_system_name]
	cmp al, byte 'F'
	je .system_name_ok
		
	;oops. no name.
    mov esi, dm_sys_name_fail
    call os_output

	mov esi, new_line
    call  os_output 
	
	ret
	
.system_name_ok:
    mov esi, dm_sys_name
    call os_output
    
	mov esi, new_line
    call  os_output 
	
	ret

dm_sys_name      db "System name Ok.",   13,10,0
dm_sys_name_fail db "System name fail!", 13,10,0






;---------------------------------------- 
; mostrar_parametros_do_disco_atual:
;    Mostra os par�metros do disco atual
;    salvos no metafile. 
; 
mostrar_parametros_do_disco_atual:  
    call hardware_show_currentdisk_info
    ret





 
;---------------------------------------------
; mostrar_parametros_da_particao_ativa:
;     Mostrar os parametros da parti��o ativa, 
;     salvos em estrutura.
;	
mostrar_parametros_da_particao_ativa:  
    call fs32_show_activepartition_info
    ret

;-------------------------------------
; debug_show_root_dir:
;     Interface para a fun��o que mostra o diret�rio raiz.
;
debug_show_root_dir:
    pushad
    call show_root_dir	
	popad
	ret


;;=========================
;; show_current_dir:
;;     Print a list of the entries of the current directory.
;;

show_current_dir:
    call fs32_show_currentdir
    ret



;-------------------------------------
; show_root_dir:
;     (Mostra o diret�rio de volumes)
;     Show root dir.
; 
show_root_dir:
    call fs32_show_rootdir
    ret


	
;
; fs32_show_current:
;
;
fs32_show_current:

    pushad
	
	;new line
	mov esi, new_line 
	call os_output
	
    ;disk
	mov esi, msg_cur_disk 
	call os_output

	xor eax, eax
	mov al, byte [CurrentDisk.disk_number]             
    call print_hex	

	;new line
	mov esi, new_line 
	call os_output	
		
	;part
	mov esi, msg_cur_part
	call os_output

	xor eax, eax
	mov al,  byte  [CurrentDisk.partition_number]             
    call print_hex	

    ;new line
	mov esi, new_line 
	call os_output

    popad
    ret


;
; Informaçoes mostradas pelo debug:
; ================================
;

msg_debug_about: 
    db "Debug ~ Showing info from META$FILE.", 13,10,0

;META$FILE info	
msg_debug_1 db 13, 10, "Init Mode     - " ,0    ;Modo de inicializ��o do System4Nora.
msg_debug_2 db 13, 10, "Video Mode    - " ,0    ;Saved Video mode.
msg_debug_3 db 13, 10, "Screen X      - " ,0    ;Screen X.
msg_debug_4 db 13, 10, "Screen Y      - " ,0    ;Screen Y.
msg_debug_5 db 13, 10, "LFB Address   - " ,0    ;LFB Address.
msg_debug_6 db 13, 10, "Nothing for now!" ,0    ;Nothing for now!
msg_debug_7 db 13, 10, "Nothing for now!" ,0    ;Nothing for now!
msg_debug_8 db 13, 10, "Nothing for now!" ,0    ;Nothing for now!

msg_cur_disk db "Current disk      - ",0
msg_cur_part db "Current partition - ",0


msg_check_system_cnf  db "[BM:] Check system.cnf",13,10,0	
msg_check_init_cnf    db "[BM:] Check init.cnf"  ,13,10,0	

;
; End.
;
 
 
