;;    
;; File: s2modes.inc
;;     stage2.
;;     Ativa um modo de inicializaçao definido para o Boot Manager.
;;     Opçoes:
;;     +1 - Inicia o Shell do boot manager.
;;     +2 - Inicia o BL.BIN no modo GUI.
;; History:
;;     2015 - Created by Fred Nora.
;;


; set_boot_mode:
;     Configura um modo de inicializaçao.
;     in: al

set_boot_mode:
    mov byte [META$FILE.INIT_MODE], al
    ret 


;---------------------------
; get_boot_mode:
;     Pega o modo de inicialisa��o do System4Nora.
;     out: al
;
get_boot_mode:
    mov al, byte [META$FILE.INIT_MODE]
    ret 

;--------------------------------------------
; set_video_mode:
;     Configura um modo de video.
;     in: ax
set_video_mode:
    mov word [META$FILE.VIDEO_MODE], ax
    ret 
 
; get_video_mode:
;     Pega o modo de video.
;     out: ax
get_video_mode:
    mov ax, word [META$FILE.VIDEO_MODE]
    ret 

;==================================================================
; s2modesActivateMode:
;     Ativa o modo escolhido por configuraçao. 
;     O modo esta salvo em META$FILE.INIT_MODE e pegaremos 
;     com a funçao get_boot_mode.
;     Saltaremos para mShell ou mGui.
; 
;+1 - Inicia o Shell do boot manager.  
;+2 - Inicia o BL.BIN no modo GUI. 
;++

;; Called by xxx_Config in main.asm.
;; Called by ____gramado_mode_done in pm.inc.

s2modesActivateMode:

; Creating a fancy screen with a status bar at the bottom.

    call Window.ClearScreen

    pusha
    call Window.StatusBar
    mov si, msg_s2_activating_mode
    call PoeString
    popa

    ; Debug
    ; jmp $

; Pega o modo de inicializaçao no metafile.
; O modo configurado retornara em al.
; al = META$FILE.INIT_MODE

.switch:

    xor eax, eax
    call get_boot_mode

    ;1 - Shell
    cmp al, byte BOOTMODE_SHELL    
    je mShell

    ;2 - GUI.
    cmp al, byte BOOTMODE_GUI      
    je mGui 

    ;; Default
    jmp mShell
    jmp $


;;===========================================
;; mShell:
;;     +1 (boot mode shell) -  
;;     Inicia a prompt de comandos do Boot Manager. #shell.   

mShell:

    pusha
    call Window.StatusBar
    mov si, msg_mode1
    call PoeString
    popa

.videoMode:

; Use video mode 3.
; #todo: Explain it better.

    xor eax, eax
    mov ax, word 3    
    call set_video_mode

;Screen.

    ; call Window.ClearScreen  

; Hide cursor.

    call Cursor.Hide 

; Boot mode.
; Ativar modo de inicializaçao.
; Obs: 
; #todo: 
; Talvez os argumentos devam ser preparados antes.
; Inicializao do modo protegido.
; Switching to Protected Mode.
; See: rm/pm.inc

.bootMode:

    mov al, byte BOOTMODE_SHELL
    call set_boot_mode

    jmp pmInit
    jmp $  


;; ==========================================
;; mGui:
;;     +2 (boot mode gui) - Graphycs mode.
;; VESA:
;;     + Verifica se suporta vesa.
;;     + Inicia configura��o de video usando vesa.
;;     Ativa o modo usando BIOS e valores do metafile.
;;     todo: 
;;     (bugbug) aqui estamos usando vesa para configurar o modo de
;;     de video, sem saber nada sobre a placa de v�deo.
;; IMPORTANTE:
;; O OBJETIVO EH CHEGARMOS AQUI COM O ARQUIVO BL.BIN CARREGADO 
;; NA MEMORIA E PASSARMOS O COMANDO PARA ELE, SEM TERMOS QUE 
;; CARREGA-LO USANDO RECURSOS DE 32BIT.
;++


mGui:
    nop
.videoMode:

; Set video mode.
; Usa um modo previamente configurado.

    xor eax, eax
    mov ax, word [META$FILE.VIDEO_MODE]    
    call set_video_mode

; Salva modo.
    mov al, byte BOOTMODE_GUI

;Verifica suporte a VESA.

.probeVESA:

    pusha
    call s2vesaSearchVESA 
    popa
    jc noVesa 

    pusha
    call Window.StatusBar
    mov si, msg_initializing_VESA
    call PoeString
    popa

; Configura modo de video usando VESA.
.initVESA:

    pusha
    call s2vesaInitVESA
    popa

; #test
; Set start
; Tested on real machine.
; I didn't see any difference in the visible area position.

    ;pusha
    ;mov   ax,4F07h 
    ;xor   bx,bx 
    ;mov   cx, 0 
    ;mov   dx, 0 
    ;int   10h 
    ;popa

.setupVideoMode:

; Registra o modo escolhido.
    mov al, byte BOOTMODE_GUI
    call set_boot_mode

; Setup registers.

    cli
    mov ax, 0 
    mov ds, ax
    mov es, ax
    ;mov fs, ax 
    ;mov gs, ax 
    xor ax, ax
    mov ss, ax
    mov sp, 0x6000 
    xor dx, dx
    xor cx, cx
    xor bx, bx
    xor ax, ax

; #importante
; Temos que entrar no modo protegido ja estando 
; no modo grafico.

;
; GDTR
;
    lgdt [stage2_gdtr]

;
; CR0 - PE
;

; Muda PE.

    mov eax, cr0
    or al, 1
    mov cr0, eax

; Flush pipeline

s2modesFlushPipeline:
    jmp stage2_CODE_SEL:s2modespm32 

;
; 32 bit 
;

[BITS 32]

s2modespm32:

; Setup registers.

    mov ax, stage2_DATA_SEL  
    mov ds, ax
    mov es, ax
    ;mov fs, stage2_NULL_SEL    
    ;mov gs, stage2_NULL_SEL    
    mov ss, ax
    mov esp, dword 0x0009FFFC
    mov edx, dword 0    
    mov ecx, dword 0
    mov ebx, dword 0
    mov eax, dword 0    

;
; ========================================================
;

; Setup the boot block.
; VESA data.
; Get some value from the vesa blodck and
; and save it in the boot block structure.
; LFB physical address.
; Width in pixels.
; Height in pixel.
; BPP.
; Metafile structure physical address
; ...

    xor eax, eax
    mov eax, dword [ModeInfo_PhysBasePtr]  
    mov dword [BootBlock.lfb], eax

    xor eax, eax
    mov ax, word [ModeInfo_XResolution]  
    mov dword [BootBlock.x], eax 

    xor eax, eax
    mov ax, word [ModeInfo_YResolution]  
    mov dword [BootBlock.y], eax 

    xor eax, eax
    mov al, byte [ModeInfo_BitsPerPixel]   
    mov dword [BootBlock.bpp], eax 

; #bugbug
; provavelmente so podemos salvar o endereço do metafile
; e nao seus elementos como estrutura.

    xor eax, eax
    mov eax, dword META$FILE  
    mov dword [BootBlock.metafile_address], eax
    ;; ...

;;
;; ========================================================
;;

; The boot block
; Putting the boot block address in ebp.
    
    mov ebp, dword BootBlock

; Now let's setup eax, ebx, ecx and edx.

;; > eax
;; 'G' flag in al. It indicates the graphics mode.
;; We wish to use the  'ah' register too, so let's clean eax first.

    xor eax, eax
    mov al, byte 'G'    ; Flag. (useing graphics)

;; > ebx
;; Putting the address of the vesa structure in ebx.
;; #bugbug: We are losing this address. We need to put this
;; address into the boot block.

    mov ebx, dword [ModeInfo_PhysBasePtr]  ; LFB address (PA)

;; > ecx 
;; > edx
;; Saving the boot block in ecx and edx

    mov ecx, ebp 
    mov edx, ebp 

;;
;; ========================================================
;;

;; Aqui estamos em modo protegido e no modo grafico, 
;; vamos testar se as mensagens para esse ambiente 
;; estao funcionando.
;; @todo: Poderiamos limpar a tela entes de 
;; apresentarmos essa mensagem.
;; #debug
;; #bugbug
;; Is it working ??
;; Testando mensagem.

    mov eax, 0x00ffff00    ;color
    mov esi, s2modes_initmessageinGM
    call lib32PrintString

;#debug
;#bugbug isso demora.

    ;call _asm_refresh_screen

    ;#debug
    ;jmp $

; Opçao eh saltar para uma area de inicializaçao em 
; 32bit modo grafico.

;
; == \o/ ============================
;

;
; Go!
;

; See:
; stage3/start.inc

    jmp startGUI
    jmp $

;
; == \o/ ============================
;

; Strings:

s2modes_initmessageinGM: 
    DB "s2modes.inc: Initializing PM in GM", 13, 10, 0

;;
;; ##############################################################
;;

;; nao queremos estragar as rotinas de 16bit.	
[bits 16]	

; Nao suporta VESA, 
; entrar no Shell do Boot Manager.
noVesa:

    mov al, byte BOOTMODE_SHELL
    call set_boot_mode

    mov al, byte BOOTMODE_SHELL
    jmp s2modesActivateMode
    jmp $
;--

;
;--------------------------------------------
; 

; Strings:

;; #todo: 
;; Change to msg_mode1_selected and msg_mode2_selected. 

msg_mode1: 
    DB "s2modes.inc: BM-Shell", 13, 10, 0
msg_mode2: 
    DB "s2modes.inc: BM-GUI",   13, 10, 0


;
; End.
;
