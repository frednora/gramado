;
; File: shell32/shell.inc
;
; Descrição:
;     Boot Manager Shell.
;     Arquivo principal do Shell do Boot Manager.
; #importante: 
; No Boot Manager, o interpretador de comandos se revela importante, 
; especialmente quando é usado para ver informações sobre os dispositivos 
; de armazenamento e sobre o boot.
; Ou seja, o foco do interpretador de comandos do Boot Manager deve ser
; as informações sobre o disco de boot e sobre a partição ativa. O único
; arquivo que o boot manager deve se preocupar é o Boot Loader, no caso
; BL.BIN.
; #todo: 
; Visualizar e manipular informações no metafile sobre arquivos e 
; armazenamento de dados.
; #importante: 
; É obrigação da rotina que fizer alguma alteração no metafile sinalizar, 
; configurando a flag 'shell_exit_flag' com as opções '0' ou '1'.
; Assim a rotina exit decidirá se salva o metafile antes de iniciar ou não.
; '1' significa salvar o metafile e '0' significa não salvar.
; Credits:
;     Original - (BareMetal, Return Infinity).
; History: 
;     2015 - Adapted by Fred Nora.


align 1
    db 'SHELL'
align 1

;=====================================================
; os_command_line:
;     Início do loop para pegar comandos.
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
; Flag diferente de zero se haverá alguma operação com arquivo.
    cmp byte [file_flag], 0
    je .do_prompt

.do_change_font_color:
    ;Color.
    ;mov ebx, 0x00FF0000
    ;mov [os_Font_Color], ebx

.do_change_name:
    ; Mostra uma string, que é um prompt para mudar o nome do arquivo.
    mov esi, prompt_change_file
    call os_output
    ; Não mostra o prompt.
    jmp .no_prompt

; Print new prompt.
; See: stage3/shell/shlib.inc
.do_prompt:
    call show_prompt_string
.no_prompt:

    ;Color.
    ;mov ebx, 0x00FFFFFF
    ;mov [os_Font_Color], ebx

; Input.
; Limit the input to 250 characters.
; This buffer has 1024 bytes.

; Get a string.
; input32.inc
    mov edi, cli_temp_string
    mov ecx, 250
    call os_input

; O [ENTER] foi pressionado, então saimos da rotina os_input.
; The user hit enter so print a new line.
; 'os_input_string' stores the number of charaters received in ECX.
; Se não há caracteres no buffer, recomeça.
; O contador indica quantos caracteres foram armazenados.

.newLine:
    call os_print_newline
    jecxz os_command_line    ;;Loop

; Remove extra spaces.
; 'os_string_parse' stores the number of words in ECX.
.extraSpaces:
    mov esi, edi
    call os_string_parse
    jecxz os_command_line    ;;Loop 

; Store the number of words in the string.
; Salva o número de palavras digitadas.
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

.endofcommand:
    nop 
; Finaliza a primeira palavra. 
; Colocando um zero no fim da string.
.finalizingWord:
    mov al, 0x00
    stosb

; Observações:
; At this point 'cli_command_string' holds at least 
; "a" and at most "abcdefgh.ijk".
; Break the contents of 'cli_temp_string' 
; into individual strings.
; #todo: Talvez devamos criar um push isso aqui,
; já que tem um pop logo em baixo.
; Change all instances of a character in a string.
; quando for espaço, colocaremos zero.
; OBS:  QUEM COLOCOU ISSO NA PILHA??
; os_string_change_char colocou e tirou esi da pilha.
; #BUGBUG
; Trocamos um caractere por outro dentro de uma string.

.killSpaces:

    mov esi, cli_temp_string
    mov al, 0x20                ;Quando for espaço,
    mov bl, 0x00                ;Coloque zero.
    call os_string_change_char  ;lib32\string32.inc
    pop esi                     ;Pop the command string.


;
; == Compare ========
;

; Compara a primeira palavra - (edi).

; =========
; 'boot'
; Inicia o modo gráfico padrão. GUI 
; See: shcalls.inc
.xBoot:
    mov edi, boot_string
    call os_string_compare
    jc do_boot

; =========
.xBootJail:
    mov edi, bootjail_string
    call os_string_compare
    jc do_gramado_mode_jail

; =========
.xBootP1:
    mov edi, bootp1_string
    call os_string_compare
    jc do_gramado_mode_p1

; =========
.xBootHome:
    mov edi, boothome_string
    call os_string_compare
    jc do_gramado_mode_home

; =========
.xBootP2:
    mov edi, bootp2_string
    call os_string_compare
    jc do_gramado_mode_p2

; =========
.xBootCastle:
    mov edi, bootcastle_string
    call os_string_compare
    jc do_gramado_mode_castle

; =========
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
; Desliga a máquina via APM.
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
; (Mostra o diretório de volumes)
; Depois do BM,(setor 65 +fat1 +fat2)
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
; Está reinicianso a instancia do shell, mas deveríamos sair dele.
.xExit:
    mov edi, exit_string
    call os_string_compare
    jc exit

; =========
; 'disk'
; Mostra informações sobre um disco.
.xDisk:
    mov edi, hd_string
    call os_string_compare
    jc hd_test

; =========
; 'part' 
; Mostra informações sobre uma partição.
.xPart:
    mov edi, particao_string  
    call os_string_compare
    jc shellcallsPartitionTest
    ;jc particao_test

; =========
;'mode1'
;Set mode 1.
.xMode1:
    mov edi, modo1_string
    call os_string_compare
    jc do_set_modo1

; =========
;'mode2'
;Set mode 2.
.xMode2:
    mov edi, modo2_string
    call os_string_compare
    jc do_set_modo2

; =========
;'mode3'
;Set mode 3.
.xMode3:
    mov edi, modo3_string
    call os_string_compare
    jc do_set_modo3

; =========
;'mode4'
;Set mode 4.
.xMode4:
    mov edi, modo4_string
    call os_string_compare
    jc do_set_modo4

; Obs:
; Se a palavra digitada não for uma das palavras reservadas 
; testadas acima, pode ser edição de arquivos.
; Checando flags relativas à operações em arquivos.

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
; Se não for nenhuma das palavras reservadas, nem edição de algum 
; arquivo, então abre um app, '.app'. Para isso muda a flag para 7.
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
; #todo: Adicionar extensão.
; Calculando o tamanho.
; If the string is longer than 8 chars we can't add a suffix.

    call os_string_length
    cmp ecx, 8
    jg shellcallsFail    ;; shell\shellcalls.inc.
    ;jg fail             ;; shell\shellcalls.inc.

; Opções de operações com arquivo e diretórios:

; Opção 1, Open file. 
    cmp byte [file_flag], 1
    je full_name
; Opção 2, Create file.
    cmp byte [file_flag], 2
    je full_name
; Opção 3, Delete file.
    cmp byte [file_flag], 3
    je full_name
; Opção 4, Open dir.
    cmp byte [file_flag], 4
    je add_ext_dir
; Opção 5, Create dir.
    cmp byte [file_flag], 5
    je add_ext_dir
; Opção 6, Delete dir.
    cmp byte [file_flag], 6
    je add_ext_dir
; Opção 7, Open app.
    cmp byte [file_flag], 7
    je add_ext_app
;?
    jmp full_name

; Adicionando extenção ao nome do arquivo.

; Adiciona extenção '.dir'.
add_ext_dir:
    mov edi, cli_command_string
    mov esi, dirextension    ; .DIR.
    call os_string_append    ; Append the extension to the command string.
    jmp full_name
; Adiciona extenção '.app'.
add_ext_app:
    mov edi, cli_command_string
    mov esi, appextension    ; .app.
    call os_string_append    ; Append the extension to the command string.
    jmp full_name

; Ver a operação de arquivo escolhida.

full_name:
ver_operacao_escolhida:

; Opção 1, Open file. 
    cmp byte [file_flag], 1
    je open_file_op
; Opção 2, Create file.
    cmp byte [file_flag], 2
    je new_file_op
; Opção 3, Delete file.
    cmp byte [file_flag], 3
    je delete_file_op
; Opção 4, Open dir.
    cmp byte [file_flag], 4
    je open_dir_op
; Opção 5, Create dir.
    cmp byte [file_flag], 5
    je new_dir_op
; Opção 6, Delete dir.
    cmp byte [file_flag], 6
    je default_op
; Opção 7, Open app.
    cmp byte [file_flag], 7
    je open_app_op

; A operação em arquivo foi negligenciada.
    jmp default_op

;Opção 0. Default
default_op:
    mov esi, msg_file_op0
    call os_output
    ;#todo: Read file.
    ;mov eax, cli_command_string
    ;call fat16_read_file
    mov byte [file_flag], 0    ; Restaura o prompt.
    jmp os_command_line

;Opção 1, Open file.
open_file_op:
	mov esi, msg_file_op1
	call os_output

	mov esi, appextension		   ;'.app'.
	mov edi, cli_command_string    ;Inclui a extensão aqui.
	call os_string_append		   ;Append the extension to the command string.
	
	mov eax, cli_command_string
	call start_program
	
	mov byte [file_flag], 0          ;Restaura o prompt.
	jmp os_command_line
	
	
;Opção 2, Create file.	
new_file_op:
	mov esi, msg_file_op2
	call os_output
	
   	mov eax, cli_command_string 
	call fat16_create_file
	mov byte [file_flag], 0        ;Restaura o prompt.
	jmp os_command_line
	
	
;Opção 3, Delete file.
delete_file_op:
	mov esi, msg_file_op3
	call os_output

	jmp os_command_line

;Opção 4, Open dir.
open_dir_op:
	mov esi, msg_file_op4
	call os_output

	jmp os_command_line
	
	
;Opção 5, Create dir.	
new_dir_op:
	mov esi, msg_file_op5
	call os_output

   	mov eax, cli_command_string 
	call fat16_create_dir
	mov byte [file_flag], 0        ;Restaura o prompt.
	jmp os_command_line
	
	
;Opção 6, Delete dir.
delete_dir_op:
	mov esi, msg_file_op6
	call os_output

	jmp os_command_line

;Opção 7, Open app.
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
;     Interface para chamar o início do Shell do Boot Manager.
;     Obs: O Essa rotina pode receber algum argumento.
; IN:   Nothing.
; OUT:  Nothing.
; #bugbug
; We need to initialize the i8042 when we enter 
; in the 32bit protected mode environment.
;+
shellInit: 

; Let's drain the keyboard output buffer.
; #todo: Use a loop for that.
; #bugbug: Not working

    ;in al, 0x60
    ;in al, 0x60

    jmp os_command_line
    ret
;--

;================================================
; shellPrintShellVersion:
;     Print shell version.
; Called by startShellLoop in start.inc
; See: shcalls.inc
    
shellPrintShellVersion:
    call print_ver
    ret

;
; End
;


