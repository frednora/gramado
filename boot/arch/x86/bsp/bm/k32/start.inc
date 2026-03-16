;
; File: start.inc 
;    stage3
; Descricao: 
;     Inicio do modo protegido. 
;     O modo protegido ja foi configurado e 
; começam aqui as rotinas em 32 bit.
; Atribuiçoes: 
;     +Efetivar a escolha do modo de inicializaçao.
;     +Passar o comando para Shell do Boot Manager
;      se essa for a escolha.
;     +Carregar o Boot Loader e passar o comando 
;      pra ele se essa for a escolha.
; Opçoes de inicializaçao:
; ========================
; +1 - Inicia o Shell do Boot Manager no modo texto.
; +2 - Inicia o Boot Loader (BL.BIN) no modo GRAFICO. (GUI).
; Contexto dos registradores:
; ===========================
; Segments:
;    DS  =  stage2_DATA_SEL  
;    ES  =  stage2_DATA_SEL
; Stack:
;    Segment  =  stage2_DATA_SEL
;    Offset   =  0x0009FFFC  ?? @todo: Cuidado.
; Registers:
;    AL  = new initialization mode.
;    EBX = 0
;    ECX = 0
;    EDX = 0
; History:
;     2015 - Created by Fred Nora.


;;=======================================================
;; start:
;;=======================================================
;;     Inicio do modo protegido de 32 bit
;; IN:
;;     AL = New mode.  (o modo a ser inicializado)
;; Pega o modo de video passado pelo stage 2 atraves de AL e 
;; salva no contexto de 32 bits.
;; Called by pm32 in stage3/pm.inc

start:

; #bugbug 
; Esse valor pode estar errado durante a 
; inicializaçao em modo texto.

    mov byte [BOOT.modo], al

    xor edx, edx
    xor ecx, ecx
    xor ebx, ebx
    xor eax, eax

; ajusta_modo: 
; Inicia um modo da configura��o permanente.
; todo: Trocar o nome.
; #bugbug 
; Esse valor pode estar errado durante 
; a inicializa��o em modo texto.
; Pega o modo de inicialisa��o do Boot Manager. 
; Escolhe o modo:
; ==============
; modo 1 - Inicia o BL.BIN no modo TEXTO.(Default).
; modo 2 - Inicia o BL.BIN no modo GR�FICO.
; modo 3 - Inicia a prompt de comandos do bootmanager.
; modo 4 - (reservado)
; Default.

ajusta_modo:
    nop
startSetupMode:
    
    xor eax, eax
    mov al, byte [BOOT.modo]  

    ;; cli
    cmp al, byte 1  
    je startCLI

    ;; gui
    cmp al, byte 2  
    je startGUI

    ;; default
    jmp startCLI 
    jmp $

;;
;; ============================================================
;;


;============================
; startCLI:
;     Inicia o Shell do Boot Manager. 

startCLI:

    call setup_machine

; Segments and stack.
; Keep interrupts disabled.
    
    cli
    mov ax, word 0x10  
    mov ds, ax
    mov es, ax 
    mov eax, stack_ptr
    mov esp, eax 

; Configura volume.
; ?? onde ?? blconfig.inc

    mov al, byte 1
    call set_fs_volume  

; Configura fs type.

    mov al, byte 1
    call set_fs_type

; Limpa a tela em modo texto.

    ; call clear_screen

; Enable all the interrupts.
.LenableInterrupts:

    mov al, 0
    out 0xA1, al 
    out 0x21, al
    sti 

;; startShellLoop:
;; The shell loop.
;; #importante
;; O shell esta em stage3/shell
;; Esse eh loop do shell do boot manager.

; Splash message.
; See: shell32/shmsg.inc
; Go to the loop.
; shell32/shell.inc

startShellLoop:

    push esi
    mov esi, version_msg
    call os_output
    pop esi

    jmp os_command_line
    jmp $


; Strings:

start_msg_saving_metafile: 
    DB 'start.inc: Saving metafile..', 13, 10, 0
start_msg_rebooting: 
    DB 'start.inc: Rebooting..',       13, 10, 0


; =================================
; startGUI:
; Initializes BL.BIN in graphics mode.
; Inicia o BL.BIN no modo grafico.
; obs: 
; Quando retornamos para o modo real e ativamos o modo gr�fico,
; Saltamos para c� com a opçao de usamos mensagens em modo gr�fico 
; em modo protegido.
; IMPORTANTE:
; O OBJETIVO � CHEGARMOS AQUI COM O ARQUIVO BL.BIN CARREGADO NA MEM�RIA
; E PASSARMOS O COMANDO PARA ELE, SEM TERMOS QUE CARREGA LO 
; USANDO RECURSOS DE 32BIT.
; Importante:
; Fazendo configura��es aqui mesmo ao inv�s de chamarmos 
; 'setup_machine' ou ainda n�o fazendo essas configura��es, 
; pois j� estamos em modo protegido com as interrup��es 
; desabilitadas e em modo gr�fico ..
; Podemos deixar que o boot loader fa�a suas inicializa��es. 
; Obs: Isso est� funcionando bem.
; Mas o timer foi configurado v�rias vezes.
; Called by s2modespm32 in s2modes.inc

startGUI:

;
; Setup machine
;

; Disable, GDT and IDT.

    cli
    lgdt [lgdt_opcode]
    lidt [lidt_opcode]

; PIC
; #todo: 
; Nessa hora alguns vetores podem ser configurados.
; Primeiro as excess�es e depois as ISRs.
; Selecting the 'Processor Interrup Mode'.
; PIC MODE
; Todos os componentes APIC s�o ignorados e o sistema opera
; no modo single-thread usando LINT0.

    mov al, 00010001b  ; Begin PIC 1 initialization.
    out 0x20, al
    mov al, 00010001b  ; Begin PIC 2 initialization.
    out 0xA0, al
    mov al, 0x20       ; IRQ 0-7:  interrupts 20h-27h.
    out 0x21, al
    mov al, 0x28       ; IRQ 8-15: interrupts 28h-2Fh.
    out 0xA1, al
    mov al, 4
    out 0x21, al
    mov al, 2
    out 0xA1, al
    mov al, 1
    out 0x21, al
    out 0xA1, al

; #todo:
; Init RTC.

;step 
;#todo: 
; memory caching control.

; step 
; processor Discovery and initialization
; (cpuid threads and cores, start inter-processor interrupt sipi, 
; ap wakeu pstate,
; wakeup vector alignment, caching consdarations,ap idle state)
; apenas o b�sico para o boot manager.

; Fazendo alguma inicializa��o de dispositivos de I/O suportados.

; step
; i/o devices
; ( embedded controller EC, super io SIO, 
; legacy free systems, miscellaneous io devices)
; Nesse momento os dispositivos de i/o suportados 
; aqui s�o (keyboard, IDE, graphic card).

; #bugbug
; Cancelado.
; N�o precisamos disso, pois vamos configurar o teclado 
; durante a inicializa��o do i8042 no kernel.

;KEYBOARD.
    ;call keyboardInit

;IDE0.   #BUGUBG NO QEMU
    ;call init_ide0_master

;LFB.   #BUGUBG NO QEMU
    ;call configura_lfb   ;;?? onde est� essa rotina ??

; #BUGBUG
; #todo:
; Aqui estamos usando constantes, porem esses valores precisam 
; serem detectados, pois variam � cada formata��o.

;Detectar os par�metros do sistema de arquivos FAT16.
;fat lba, root lba ...
    ;call fat16DetectFSParameters  ;;fat16lib.inc

;
; ========================================================
;

; Set up LBA for first FAT, root dir and data area.

; Fat do volume atual
    mov dword [g_current_fat_lba],  FAT16_FAT_LBA

; Diretorio do volume atual.
    mov dword [g_current_directory_lba],  FAT16_ROOTDIR_LBA

;?? ;lba do inicio da area de dados do volume atual.
    mov dword [g_current_dataarea_lba],  FAT16_DATAAREA_LBA 

; Continua ...

;; Configurando PCI. (O b�sico suportado pelo Boot Manager.)
;; O boot manager est� carregando o BL utilizando recursos 
;; do BIOS em modo real. Mas se o BM fosse carregar o BL 
;; de um dispositivo SATA em 32bit teria que configurar o PCI.
;; (Se poss�vel deixaremos para o BL o penoso trabalho de lidar
;; com PCI, j� que o BL � feito em C, isso deve ser mais f�cil para
;; o BL do que para o BM.)

; step
; PCI device descovery.

; Cursor.
    call reset_cursor 

; ...

; Mask all interrupts.

    mov al, 255
    out 0xA1, al
    out 0x21, al

; Setup data registers and stack.

    mov ax, word 0x10 
    mov ds, ax
    mov es, ax 
    ;mov fs, ax
    ;mov gs, ax 
    ; #bugbug
    ; Missing ss register. 
    ; (This is a test) (out. 2020)
    mov ss, ax
    mov eax, stack_ptr
    mov esp, eax 

; #bugbug
; Determinando tipo de volume.

; Configura volume.
    mov al, byte 1
    call set_fs_volume

; Configura fs type.
    mov al, byte 1
    call set_fs_type

;
; LFB
;

; internas.
; Configura LFB e Buffer.
; #todo: Change this name.

    call setup_lfb

; #bugbug
; Cancelando isso.
; Pois esses valores est�o acima do limite de 16MB.
; Qualquer coisa para sistemas acima de 16MB ser�o 
; configuradas pelo kernel depois de detectar a 
; quantidade de mem�ria.
; #todo: Change this name.
    
    ;call setup_buffer

; Se configurar o modo 3 n�o tem como 
; reconfigurar atravez desse modo ainda.
; entao salva modo 1

    ;mov eax, dword 1
    ;call blconfig_set_modo	
    ;call do_save

; #todo:
; N�o � certo pintar um backgraund direto na mem�ria de video.
; O Boot Manger n�o tem sincroniza��o vertical ainda.
;background test!
    ;call _background
    ;call _background_teste       
    ;call _fade_screen
    ;jmp $

;
; == Testing strings ========================
;

; #bugbug: 
; As rotinas de 'char' 'string' e 'load' aparentemente est�o travando 
; o sistema, as duas janelas de status est�o funcionando quando n�o 
; chamamos essas fun��es.
; #obs:
; Aqui vamos testar a rotina de mostrar strings no modo gr�fico.
; O caractere deve ser constru�do, p�xel por p�xel, utilizando 
; o mapa de caracteres do BIOS.

.LtestingStrings:

    nop

; #todo: 
; Testar rotina de mostrar uma string.

    ;debug break
    ;jmp $

;Debug:
;Pinta uma janela pra testar modo gr�fico.

    ;mov eax, 0    ;x.
    ;mov ebx, 0    ;y.
    ;mov ecx, 20   ;largura.
    ;mov edx, 20   ;altura.	
    ;mov edi, dword  0x80808000    ;blue 
    ;call CreateWindow
    ;call _asm_refresh_screen
  
; #BUGBUG
; O teste indicou que as rotinas de char e string est�o
; com mal funcionamento, travando o sistema.
; a segunda janale n�o aparece que sinaliza prograsso n�o aparece.

; #todo: 
; Fazer debug da fun��o print char antes de tudo ...
; status: testando ...

;Char.

; a =  cor (rrggbbaa)
; b =  x
; c =  y
; d =  Char.    
  
.LTestingPrintChar:  

	;push eax
	;push ebx
	;push ecx 
	;push edx 
	;mov eax, 0x00ffff00  ;; Color
	;xor edx, edx 
	;mov dl, 'a'          ;; Caractere de teste.	
	;call lib32PrintChar	
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	;call _asm_refresh_screen


	;push eax
	;push ebx
	;push ecx 
	;push edx 	
	;mov eax, 0x00ffff00  ;; Color
	;xor edx, edx 
	;mov dl, 'b'          ;; Caractere de teste.	
	;call lib32PrintChar	
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
    ;call _asm_refresh_screen


	;push eax
	;push ebx
	;push ecx 
	;push edx 
	;mov eax, 0x00ffff00  ;; Color
	;xor edx, edx 
	;mov dl, 'A'          ;; Caractere de teste.	
	;call lib32PrintChar	
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
    ;call _asm_refresh_screen

	;push eax
	;push ebx
	;push ecx 
	;push edx 
	;mov eax, 0x00ffff00  ;; Color
	;xor edx, edx 
	;mov dl, 'B'          ;; Caractere de teste.
	;call lib32PrintChar	
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 	
	;call _asm_refresh_screen
	
	;debug break
	;jmp $


.LTestingPrintString:

; Cursor

    mov dword [g_cursor_x], 8
    mov dword [g_cursor_y], 8


;String
    ;mov esi, msg_start_loading
    ;call lib32PrintString
    ;call _asm_refresh_screen

; debug
; Verbose: 'startGUI'
    ;mov esi, startMsgTest1   
    ;call lib32PrintString
    ;call _asm_refresh_screen

; #debug 
; #todo: 
; Testando modos no geforce.
    ; jmp $

; #todo: 
; #importante:
; No boot manager ainda nao ha funçoes de 
; print string em modo grafico.
; Entao para o carregamento do BL.BIN nao ha mensagens.      

;
; Carrega o Boot Loader.
;

; Carrega o arquivo  BL.BIN na memoria e 
; passa o comando para ele.
; Enviando argumento em AL, e EBX.

; ATENÇAO 
; O BL.BIN FOI CARREGADO PELO MBR USANDO RECURSOS DO BIOS.
; NAO DEVEMOS CARREGA-LO NOVAMENTE NESSE MOMENTO 
; USANDO RECURSO DE 32BIT.

; .LoadBootLoader:

; #debug
    ;mov esi, msg_start_ok
    ;call lib32PrintString
    ;call _asm_refresh_screen

;;
;;======================
;;

.StartBootLoader:

; #debug
; Precisamos disso no debug na maquina real.

    mov esi, msg_start_jumping
    call lib32PrintString
    call _asm_refresh_screen

;#breakpoint
    ;jmp $

;
; == Image =============================
;

; #bugbug
; Temos que permitir o formato ELF.
; Porem o BL ainda esta no formato PE.

; Check PE binary in BL.BIN.
; IMAGE_ADDRESS

    mov edx, dword 0x20000   
    mov eax, [edx]

; First byte: 'L'
; Second byte: 0x01

    cmp al, byte 'L'
    jne .peFail

    inc edx 
    mov eax, [edx]

    cmp al, byte 0x01
    jne .peFail


;
;  ## Boot Block ## 
;

; Preparing a table of arguments.
; See: stage3/include/bootloader.inc.
; #todo
; We need the boot disk value!
; Prepara a tabela.
; Estamos preparando a tabela para enviarmos para o Boot Loader.
; #todo
; Precismos passar mais informa��es sobre o disco.

; =================
; boot block structure:
; LFB address.
; Width in pixels.
; Height in pixel.
; BPP.
; metafile address
; ...
; ==================


; lfb
    xor eax, eax
    mov eax, dword [ModeInfo_PhysBasePtr]  
    mov dword [BootBlock.lfb], eax

; screen width
    xor eax, eax
    mov ax, word [ModeInfo_XResolution]    
    mov dword [BootBlock.x], eax 

; screen height
    xor eax, eax
    mov ax, word [ModeInfo_YResolution]    
    mov dword [BootBlock.y], eax 

; bpp
    xor eax, eax
    mov al, byte [ModeInfo_BitsPerPixel]   
    mov dword [BootBlock.bpp], eax 

; #bugbug
; Algo nao permite enderecarmos os elementos do metafile.
; mas conseguimos passar seu endereço.

;; == ?? ==================================================
;; metafile address
    xor eax, eax
    mov eax, dword META$FILE    
    mov dword [BootBlock.metafile_address], eax

;; disk number.
    xor eax, eax
    mov al, byte [DISKINFO16_disk_number]  
    mov dword [BootBlock.disk_number], eax 

;; heads (word)
    xor eax, eax
    mov ax, word [DISKINFO16_heads]
    mov dword [BootBlock.heads], eax 

;; spt (word)
    xor eax, eax
    mov ax, word [DISKINFO16_spt]
    mov dword [BootBlock.spt], eax 

;; cylinders (word)
    xor eax, eax
    mov ax, word [DISKINFO16_cylinders]
    mov dword [BootBlock.cylinders], eax 

;; Saving the gramado mode in the boot block. 
;; Maybe it's enough to the BL have this number.
    xor eax, eax
    mov eax, dword [tmp_gamemode]
    mov dword [BootBlock.gramado_mode], eax 

; Continua...


;
; Display mode. (Graphics)
;

;Flag. (using graphics)

    ;xor eax, eax
    mov al, byte  'G'

; #todo
; Passar isso para BL.BIN
    ;; #mov ah, byte [META$FILE.GRAMADO_MODE]

; LFB physical address.
    mov ebx, dword [ModeInfo_PhysBasePtr]

; Boot block address.
    mov ebp, dword BootBlock
    mov ecx, ebp                ; Table
    mov edx, ebp                ; Table
    
; Gramado mode. (jail, p1, home ...)
    mov edi, dword [tmp_gamemode]

;;
;; == \o/ =================================
;;

; Go!
; Jump to the entrypoint of the boot loader. (BL.BIN).
; 0x00021000
; See: bl/x86/head.s
; IN:
; al:  Boot mode.
; ebx: LFB physical addres.
; ecx: Boot block address.
; edx: Boot block address.
; ebp: Boot block address.
; edi: Gramado mode. (jail, p1, home ...)

    jmp  8:IMAGE_ADDRESS_ENTRY_POINT
    JMP  $

;;
;; == \o/ =================================
;;

;;
;; Fail.
;;

; O arquivo n�o est� presente na mem�ria.
; Muda a cor da string.	
.peFail:

    mov eax, 0x00ffff00
    mov esi, msg_start_sig
    call lib32PrintString

    mov eax, 0x00ffff00
    mov esi, 0x20000
    call lib32PrintString

    call _asm_refresh_screen

.peFailHalt:
    cli
    hlt
    jmp .peFailHalt
    
; #fim 
; Começam as rotinas internas do arquivo.

;; ================================================================


;
; ==== Libs =======================================
;


;-------------------------------------
; setup_lfb:
;     Atualiza LFB address.
;     Salva LFB.
;     #todo: Change the name.
;++

setup_lfb:
    push edx   
    mov  edx, dword [ModeInfo_PhysBasePtr]  
    mov dword [VIDEO.lfb], edx
    pop edx
    ret
;--



;-------------------------------------
; setup_buffer:
;     Configura buffer para modo gr�fico.
;     
;  #bugbug 
; nem lembro desses endere�os, 
; mas eles est�o acima de 16MB e isso � problema, pois o 
; sistema tem a configura��o m�nima de 16MB.
;
;     @todo: Confirmar os endere�os dos buffer.
;            Configurar p�ginas para eles.  
;
;++

setup_buffer:

;; #bugbug
;; Cancelando isso.
;; Pois esses valores est�o acima do limite de 16MB.
;; Qualquer coisa para sistemas acima de 16MB ser�o 
;; configuradas pelo kernel depois de detectar a 
;; quantidade de mem�ria.

    ;buffer 1,2 e 3.
    ;push edx
    ;mov edx, dword 0x01000000
    ;mov dword [VIDEO.buffer1], edx
    ;mov edx, dword 0x01500000
    ;mov dword [VIDEO.buffer2], edx
    ;mov edx, dword 0x01A00000
    ;mov dword [VIDEO.buffer3], edx 
    ;pop edx

    ret
;--


; Start mensagens.

startMsgTest1:      
    db "BM-start: startGUI:",            13, 10, 0
msg_start_ok:       
    db "BM-startGUI: OK",                13, 10, 0
msg_start_jumping:  
    db "BM-startGUI: Jumping to BL.BIN", 13, 10, 0
msg_start_sig:      
    db "BM-startGUI: PE sig fail",       13, 10, 0


;
; End.
;

