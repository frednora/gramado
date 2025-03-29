; shell32/shell.inc
; This is a 32bit shell embedded inside the boot manager.
; Credits:
;     Original - (BareMetal OS, Return Infinity).
; History: 
;     2015 - Adapted by Fred Nora.


; #todo: Long description.
;     Boot Manager Shell.
;     Arquivo principal do Shell do Boot Manager.
; #importante: 
; No Boot Manager, o interpretador de comandos se revela importante, 
; especialmente quando � usado para ver informa��es sobre os dispositivos 
; de armazenamento e sobre o boot.
; Ou seja, o foco do interpretador de comandos do Boot Manager deve ser
; as informa��es sobre o disco de boot e sobre a parti��o ativa. O �nico
; arquivo que o boot manager deve se preocupar � o Boot Loader, no caso
; BL.BIN.
; #todo: 
; Visualizar e manipular informa��es no metafile sobre arquivos e 
; armazenamento de dados.
; #importante: 
; � obriga��o da rotina que fizer alguma altera��o no metafile sinalizar, 
; configurando a flag 'shell_exit_flag' com as op��es '0' ou '1'.
; Assim a rotina exit decidir� se salva o metafile antes de iniciar ou n�o.
; '1' significa salvar o metafile e '0' significa n�o salvar.


align 1
    db 'SHELL'
align 1

;=====================================================
; os_command_line:
;     In�cio do loop para pegar comandos.
;     #### LOOP ####
; IN:
;     Nothing.
; OUT:
;     EAX = 0, Sair sem salvar o metafile, depois reiniciar. 
;     EAX = 1, Sair salvando o metafile, depois reiniciar.
; Called for the first time by startShellLoop in start.inc.
; #todo
; Simplify this thing.
;++

os_command_line:
; We're gonna get a string.
; The last key is a [Enter].

    mov byte [key], 0

; #todo: Deprecated
; Flag diferente de zero se haver� alguma opera��o com arquivo.
    cmp byte [file_flag], 0
    je .do_prompt

.do_change_font_color:
    ;Color.
    ;mov ebx, 0x00FF0000
    ;mov [os_Font_Color], ebx

.do_change_name:
    ; Mostra uma string, que � um prompt para mudar o nome do arquivo.
    mov esi, prompt_change_file
    call os_output
    ; N�o mostra o prompt.
    jmp .no_prompt

; Print new prompt.
; See: shlib.inc
.do_prompt:
    call show_prompt_string
.no_prompt:

    ;Color.
    ;mov ebx, 0x00FFFFFF
    ;mov [os_Font_Color], ebx

; ==========================================================
;         ++++ IMPUT ++++
; ==========================================================

; Get a string.
; See: input32.inc
; Limit the input to 250 characters.
; This buffer has 1024 bytes.
    mov edi, cli_temp_string
    mov ecx, 250
    call os_input

; Now the [ENTER] key was pressed and we returned from the os_input function.
; The user hit enter, so print a new line.
; 'os_input_string' stores the number of charaters received in ECX.
; If there is no chars into the buffer, se it restarts.
; The counter tell us how many chars was stored.

.newLine:
    call os_print_newline
    jecxz os_command_line    ;;Loop

; Remove extra spaces.
; 'os_string_parse' stores the number of words in ECX.
.extraSpaces:
    mov esi, edi
    call os_string_parse
    jecxz os_command_line    ;;Loop 

; Store the number of words found in the string.
.saveTotalWords:
    mov byte [app_argc], cl

; Copy the first word in the string to a new string. 
; This is the command/application to run.
.copyFirstword:
    xor ecx, ecx
    mov esi, cli_temp_string
    mov edi, cli_command_string

;Push the command string (primeira palavra).
    push edi
.nextbyte1:
    add ecx, dword 1
    lodsb

    ;End of the first word.
    cmp al, ' '
    je .endofcommand

    ;End of the string.
    cmp al, 0x00
    je .endofcommand

    ;More than 12 bytes in the first word.
    cmp ecx, 13
    je .endofcommand

    ;Coloca em edi.
    stosb
    jmp .nextbyte1

; End of the first word.
; Let's finalize the work putting a 0x00 at the end of our tmpstring.
.endofcommand:
    nop 
.finalizingWord:
    mov al, 0x00
    stosb

; Let's kill the white spaces.
; Observa��es:
; At this point 'cli_command_string' holds at least 
; "a" and at most "abcdefgh.ijk".
; Break the contents of 'cli_temp_string' 
; into individual strings.
; #todo: Talvez devamos criar um push isso aqui,
; j� que tem um pop logo em baixo.
; Change all instances of a character in a string.
; quando for espa�o, colocaremos zero.
; OBS:  QUEM COLOCOU ISSO NA PILHA??
; os_string_change_char colocou e tirou esi da pilha.
; #BUGBUG
; Trocamos um caractere por outro dentro de uma string.

.killSpaces:

    mov esi, cli_temp_string
    mov al, 0x20                ;Quando for espa�o,
    mov bl, 0x00                ;Coloque zero.
    call os_string_change_char  ;lib32\string32.inc
    pop esi                     ;Pop the command string.

; ==========================================================
;         ++++ COMPARE ++++
; ==========================================================

; Compare reserved word.
; Compara a primeira palavra - (edi).
; See: shmsg.inc

; =========
; 'boot'
; GUI: It initializes the default graphical mode.
; The default is boothome (800x600).
; See: shcalls.inc
.xBoot:
    mov edi, boot_string
    call os_string_compare
    jc do_boot

; =========
; 'bootjail'
.xBootJail:
    mov edi, bootjail_string
    call os_string_compare
    jc do_gramado_mode_jail

; =========
; 'bootp1'
.xBootP1:
    mov edi, bootp1_string
    call os_string_compare
    jc do_gramado_mode_p1

; =========
; 'boothome'
.xBootHome:
    mov edi, boothome_string
    call os_string_compare
    jc do_gramado_mode_home

; =========
; 'bootp2'
.xBootP2:
    mov edi, bootp2_string
    call os_string_compare
    jc do_gramado_mode_p2

; =========
; 'bootcastle'
.xBootCastle:
    mov edi, bootcastle_string
    call os_string_compare
    jc do_gramado_mode_castle

; =========
; 'bootcali'
.xBootCalifornia:
    mov edi, bootcalifornia_string
    call os_string_compare
    jc do_gramado_mode_california

; =========
; 'reboot'
; Reboot the system.
.xReboot:
    mov edi, reboot_string
    call os_string_compare
    jc reboot

; =========
; 'shutdown'
; Poweroff the machine via APM.
.xShutdown:
    mov edi, shutdown_string
    call os_string_compare
    jc do_shutdown

; =========
; 'cls'
; Clear screen.
.xCLS:
    mov edi, cls_string
    call os_string_compare
    jc  shell_clear_screen

; =========
; 'help'
; Show help message.
.xhelp:
    mov edi, help_string
    call os_string_compare
    jc print_help

; =========
; 'dir' 
; (Mostra o diret�rio de volumes)
; Depois do BM, (setor 65 +fat1 +fat2)
.xDir:
    mov edi, dir_string
    call os_string_compare
    jc dir

; =========
; 'ver'
; Show version info.
.xVer:
    mov edi, ver_string
    call os_string_compare
    jc print_ver

; =========
; 'exit'
; #bugbug
; Exit the shell,
; at this case it goes to the bootloader (BL.BIN)
.xExit:
    mov edi, exit_string
    call os_string_compare
    jc exit

; =========
; 'disk'
; Show disk information.
.xDisk:
    mov edi, disk_string
    call os_string_compare
    jc hd_test

; =========
; 'part' 
; Show partition entries.
.xPart:
    mov edi, part_string  
    call os_string_compare
    jc shellcallsPartitionTest
    ;jc particao_test

;--------------------------------------------------------

;
; Not a reserved word.
;

; Obs:
; Se a palavra digitada n�o for uma das palavras reservadas 
; testadas acima, pode ser edi��o de arquivos.
; Checando flags relativas � opera��es em arquivos.

    cmp byte [file_flag], 1 
    je .not_app
    cmp byte [file_flag], 2 
    je .not_app
    cmp byte [file_flag], 3 
    je .not_app
    cmp byte [file_flag], 4 
    je .not_app
    cmp byte [file_flag], 5 
    je .not_app
    cmp byte [file_flag], 6 
    je .not_app

; Foi confirmado que se trata mesmo de um app.
; Se n�o for nenhuma das palavras reservadas, nem edi��o de algum 
; arquivo, ent�o abre um app, '.app'. Para isso muda a flag para 7.
; Que indica executar app.

.is_app:
    mov byte[file_flag], 7
    jmp .checkFS

; ?? E agora ???
; bugbug, deveria ter alguma coisa aqui,
; algum jmp ???

.not_app:
    nop

.checkFS:

; At this point it is not one of the built-in CLI functions. 
; Prepare to check the filesystem.
; Check for a '.' in the string.
; If there was a '.' then a suffix is present.

    mov al, '.'
    call os_string_find_char
    cmp eax, 0
    jne full_name

; No suffix was present, so we add the default application suffix 
; of ".app"
; #todo: Adicionar extens�o.
; Calculando o tamanho.
; If the string is longer than 8 chars we can't add a suffix.

    call os_string_length
    cmp ecx, 8
    jg shellcallsFail    ;; shell\shellcalls.inc.
    ;jg fail             ;; shell\shellcalls.inc.

; Op��es de opera��es com arquivo e diret�rios:

; Op��o 1, Open file. 
    cmp byte [file_flag], 1
    je full_name
; Op��o 2, Create file.
    cmp byte [file_flag], 2
    je full_name
; Op��o 3, Delete file.
    cmp byte [file_flag], 3
    je full_name
; Op��o 4, Open dir.
    cmp byte [file_flag], 4
    je add_ext_dir
; Op��o 5, Create dir.
    cmp byte [file_flag], 5
    je add_ext_dir
; Op��o 6, Delete dir.
    cmp byte [file_flag], 6
    je add_ext_dir
; Op��o 7, Open app.
    cmp byte [file_flag], 7
    je add_ext_app
;?
    jmp full_name

; Adicionando exten��o ao nome do arquivo.

; Adiciona exten��o '.dir'.
add_ext_dir:
    mov edi, cli_command_string
    mov esi, dirextension    ; .DIR.
    call os_string_append    ; Append the extension to the command string.
    jmp full_name
; Adiciona exten��o '.app'.
add_ext_app:
    mov edi, cli_command_string
    mov esi, appextension    ; .app.
    call os_string_append    ; Append the extension to the command string.
    jmp full_name

; Ver a opera��o de arquivo escolhida.

full_name:
ver_operacao_escolhida:

; Op��o 1, Open file. 
    cmp byte [file_flag], 1
    je open_file_op
; Op��o 2, Create file.
    cmp byte [file_flag], 2
    je new_file_op
; Op��o 3, Delete file.
    cmp byte [file_flag], 3
    je delete_file_op
; Op��o 4, Open dir.
    cmp byte [file_flag], 4
    je open_dir_op
; Op��o 5, Create dir.
    cmp byte [file_flag], 5
    je new_dir_op
; Op��o 6, Delete dir.
    cmp byte [file_flag], 6
    je default_op
; Op��o 7, Open app.
    cmp byte [file_flag], 7
    je open_app_op

; A opera��o em arquivo foi negligenciada.
    jmp default_op

;Op��o 0. Default
default_op:
    mov esi, msg_file_op0
    call os_output
    ;#todo: Read file.
    ;mov eax, cli_command_string
    ;call fat16_read_file
    mov byte [file_flag], 0    ; Restaura o prompt.
    jmp os_command_line

;Op��o 1, Open file.
open_file_op:
	mov esi, msg_file_op1
	call os_output

	mov esi, appextension		   ;'.app'.
	mov edi, cli_command_string    ;Inclui a extens�o aqui.
	call os_string_append		   ;Append the extension to the command string.
	
	mov eax, cli_command_string
	call start_program
	
	mov byte [file_flag], 0          ;Restaura o prompt.
	jmp os_command_line
	
	
;Op��o 2, Create file.	
new_file_op:
	mov esi, msg_file_op2
	call os_output
	
   	mov eax, cli_command_string 
	call fat16_create_file
	mov byte [file_flag], 0        ;Restaura o prompt.
	jmp os_command_line
	
	
;Op��o 3, Delete file.
delete_file_op:
	mov esi, msg_file_op3
	call os_output

	jmp os_command_line

;Op��o 4, Open dir.
open_dir_op:
	mov esi, msg_file_op4
	call os_output

	jmp os_command_line
	
	
;Op��o 5, Create dir.	
new_dir_op:
	mov esi, msg_file_op5
	call os_output

   	mov eax, cli_command_string 
	call fat16_create_dir
	mov byte [file_flag], 0        ;Restaura o prompt.
	jmp os_command_line
	
	
;Op��o 6, Delete dir.
delete_dir_op:
	mov esi, msg_file_op6
	call os_output

	jmp os_command_line

;Op��o 7, Open app.
open_app_op:
	mov esi, msg_file_op7
	call os_output

    mov eax, cli_command_string
	call fat16_openapp	
	mov byte [file_flag], 0        ;Restaura o prompt.
	jmp os_command_line	

;--

os_command_line_ret:
    ret


;================================================
; Wrapper
init_shell:
    jmp  shellInit

;================================================
; shellInit:
;     Interface para chamar o in�cio do Shell do Boot Manager.
;     Obs: O Essa rotina pode receber algum argumento.
; IN:   Nothing.
; OUT:  Nothing.
; #bugbug
; We need to initialize the i8042 when we enter 
; in the 32bit protected mode environment.
;+
shellInit: 
; #todo:
; Maybe we can create a loop here.

; Let's drain the keyboard output buffer.
; #todo: Use a loop for that.
; #bugbug: Not working

    ;in al, 0x60
    ;in al, 0x60

    jmp os_command_line
    ret
;--

;================================================
; Print shell version.
; Called by startShellLoop in start.inc
; See: shcalls.inc
shellPrintShellVersion:
    call print_ver
    ret

;
; End
;
