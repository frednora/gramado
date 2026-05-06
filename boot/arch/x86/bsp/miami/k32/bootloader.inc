;
; File: bootloader.inc 
;
; Descri��o:
;     Inicia o Boot Loader da parti��o ativa.
;     Carrega o BL.BIN para a mem�ria e executa.
;     O Boot Loader pode estar em qualquer parti��o de qualquer disco, 
; mesmo que o disco n�o seja boot�vel.
;
; History:
;     2015 - Created by Fred Nora.
;



;  
; Boot Loader name.
; Obs: Esse � programaque deve ser carregado pelo Boot Manager.
;

boot_loader_name  db  'BL      BIN',0


;------------------------------------------
; BootBlock:
;
;     Table of arguments passaed to BL.BIN.
;     #todo: We need the disk number.
;
;     Argumentos que ser�o passador para o Boot Loader.
;     Passa para o Boot Loader um ponteiro para os servi�os oferecidos pelo 
; Boot Manager.
;     * Informa��es sobre modo de v�deo.
;     * ...
;     *
;
; Obs: Isso � um Meta-Objeto: Um vetor contendo vari�veis e m�todos.
; Obs: .bmServices conter� o endere�o de _bmServices:. que esta em lib32.inc.
;
; Obs: Criaremos um bloco de informa��es seguindo o padr�o da 'Multiboot Specification'.
; O Boot Manager, o Boot Loader e o Kernel base dever�o seguir esse padr�o de boot.
; Para tirarmos proveito da padroniza��o do processo de boot para open source.
;

;; Caution
;; BL.BIN needs to know this order.

BootBlock:

    .lfb:    dd 0              ;; 0
    .x:      dd 0              ;; 4
    .y:      dd 0              ;; 8
    .bpp:    dd 0              ;; 12
    .last_valid_address: dd 0  ;; 16 - ram (size)
    .metafile_address:   dd 0  ;; 20
    .disk_number: dd 0         ;; 24
    .heads:       dd 0         ;; 28
    .spt:         dd 0         ;; 32
    .cylinders:   dd 0         ;; 36 
    .boot_mode:     dd 0       ;; 40 cli or gui,  
    .gramado_mode:  dd 0       ;; 44 (gamemode. jail,p1,home,p2,castle)
    
    ;Continua...

;;
;; ============================================================
;; Gramado mode
;;      This is the same of the BootBlock.gramado_mode.
;;      Nao eh aqui que salvamos a configuraçao
;;      isso eh so usado para ajudar.
;;
tmp_gamemode: dd 0





;;
;;  Bloco de informa��es para o multiboot.
;;

MultiBootBlock:
    dd 0
    dd 0
    dd 0
    dd 0
;;...

	
	
;Status da instala��o do boot.
bootloader_install_status: dd 0



;-----------------------------------------------
; bootloader:
;     Carrega e inicia o Boot Loader da parti��o ativa.
;
;++
bootloader: 
	;suspensa.
	;call load_boot_loader
	;call start_bootloader
	ret	
;--	

	
	
	
;====================================================
; load_boot_loader:
;     Carrega o Boot Loader (BL.BIN) na mem�ria.
;     Essa rotina � chamada em start.inc para carregar o BL.BIN na mem�ria.
;     Essa rotina chama os recursos de \fs da classe fs16XXXX. 
;
load_boot_loader:
    nop
.initHardware:
	call init_ide0_master
.readFile:
	push eax
	push ebx 
    mov eax, dword boot_loader_name
    ;call fat16_read_file
	call fat16ReadFile
	jc .fail
	pop ebx
	pop eax
    clc
	ret
.fail:	
    ;@todo: Some message.
    ;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msm_load_boot_loader_fail 
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen 
	jmp $

msm_load_boot_loader_fail: db "load_boot_loader: fail.",13,10,0	
	
;----------------------------------------------
; start_bootloader:
;     Executa o Boot Loader do System4Nora. No modo texto apenas.
;
start_bootloader:

    call os_screen_clear
	call reset_cursor
	
    ;cabe�alho PE.
    cmp byte [IMAGE_ADDRESS], byte 'L'
	je .ok
	
	;Erro.
    mov esi, bl_bl_nao_presente
	call os_output	

	;Debug
	jmp $
.ok:	
    ;Debug
	;Mostra o arquivo. (ou um peda�o dele.)
    ;mov esi, IMAGE_ADDRESS
	;call os_output
	
	mov esi, bl_bl_test
	call os_output
   
	;
	;@todo: Passar argumentos.
    ;
    ; al = modo de video
	; ebx = lfb.
    ;
	
	mov al, byte 'T' 
 	;mov al, byte 'G'                       ;Flag. (useing graphics)	
	;mov ebx, dword [ModeInfo_PhysBasePtr]  ;LFB address
	;mov ecx, dword [ModeInfo_XResolution]  ;Width in pixels.
	;mov edx, dword [ModeInfo_YResolution]  ;height in pixel.	
	
	;
	; Go!.
	;
	jmp 8:IMAGE_ADDRESS_ENTRY_POINT
	jmp $
	
	;jmp System4Nora_BootLoader  ;Passa o comando para o Boot Loader.  
	;call System4Nora_BootLoader 

.done:	
	mov eax, 0	
	RET
	
	;mov esi, bl_bl_fail_text
	;call os_output	
	
	;Op��o.
	JMP $
	

;;==================================================================
;; start_bootloader_on_graphic_mode:
;;     Executa o programa BL.BIN no modo gr�fico.
;;
;;
;;   **** PASSANDO O COMANDO PARA O BL.BIN    ****
;;
;;
;; @todo: Verificar antes se ele est� presenta na mem�ria.
;;
start_bootloader_on_graphic_mode:
;bootloaderStartOnGraphicMode:	
	;
	; Meta-Object: @todo:
	; ============
	;     O Boot Manager passar para o Boot Loader um Meta-Objeto na forma 
	; de vetor, contendo vari�veis e m�todos.
	;     O principal m�todo � se passar � _bmServices contendo o m�todo
	; que o Boot Manager usa para oferecer servi�os. Desse m�do o Boot Manager
	; pode atuar como um m�dulo do kernel em kernel mode. Para isso o
	; Boot Loader ter� que passar esse m�todo para o Kernel Base.
	;
	
	;
	; Prepara argumentos.
	;
	; EBP = BootBlock.
	; al = 'G'
	; ebx = LFB.
	;
	;
	
	;Prepara a tabela.
	mov ebp, dword BootBlock
	
	xor eax, eax
	mov eax, dword [ModeInfo_PhysBasePtr]  ;LFB address.	
	mov dword [BootBlock.lfb], eax	
	
	xor eax, eax
	mov ax, word [ModeInfo_XResolution]    ;Width in pixels.
	mov dword [BootBlock.x], eax 	
	
	xor eax, eax
	mov ax, word [ModeInfo_YResolution]    ;Height in pixel.
	mov dword [BootBlock.y], eax 	
	
	xor eax, eax
	mov al, byte [ModeInfo_BitsPerPixel]   ;bpp.
	mov dword [BootBlock.bpp], eax 


	xor eax, eax
	mov eax, dword META$FILE                 ;metafile address	
	mov dword [BootBlock.metafile_address], eax	
    
	;Continua...
	
	xor eax, eax
	mov al, byte 'G'                       ;Flag. (useing graphics)	
	mov ebx, dword [ModeInfo_PhysBasePtr]  ;LFB address (endere�o f�sico)
	mov ecx, ebp
	mov edx, ebp                           ;tabela
	
	;
    ; Go! ( Passa o comando para o entrypoint do Boot Loader. ) BL.BIN. 
    ;	
	
	;call _background_teste
	;call _background
	;call _fade_screen
	;jmp $
	
	
    ;cabe�alho PE. no BL.BIN.
    cmp byte [IMAGE_ADDRESS], byte 'L'
	jne .peFail	

	mov esi, msg_bootloader_jumping
	call lib32PrintString
	call _asm_refresh_screen		
	
	;debug
	jmp $
	
	
	jmp 8:IMAGE_ADDRESS_ENTRY_POINT
	JMP $

;O arquivo n�o est� presente na mem�ria.	
.peFail:
	mov esi, msg_bootloader_sig
	call lib32PrintString
	call _asm_refresh_screen		
    jmp $

msg_bootloader_sig     db "start_bootloader_on_graphic_mode: A assinatura PE de BL.BIN falhou ... *HANG",0	
msg_bootloader_jumping db "start_bootloader_on_graphic_mode: Passando o comando para o BL.BIN ... *HANG",0	
	
;
; Messages.
;

;Erros.
bl_bl_fail_text      db "start_bootloader error: Text Mode fail!",13,10,0
bl_bl_fail_graphics  db "start_bootloader error: Graphic mode fail!",13,10,0	
bl_bl_presente       db "start_bootloader error: BL.BIN Validation!",13,10,0
bl_bl_nao_presente   db "start_bootloader error: BL.BIN Validation!",13,10,0
bl_bl_test           db "start_bootloader: Passa o comando para o Boot Loader..",13,10,0

;
;fim.
;	
 
