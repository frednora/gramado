;
; File: pm.inc
;     stage3
;     +Enable Protected Mode with no Paging.
; Trampoline: 
;     Switch to protected mode
;
; This file has a mix of different modes:
; + real mode 16bit
; + protected mode 32bit
; + protected mode 16bit
;
; Segments:
;     DS = stage2_DATA_SEL  
;     ES = stage2_DATA_SEL
; Stack:
;     Segment = stage2_DATA_SEL
;     Offset  = 0x0009FFFC
; Registers:
;     AL  = New initialization mode.
;     EBX = ECX = EDX = 0.
; History:
;     2005+ - Created by Fred Nora.
;     2015  - Adapted from older version.
;     2016  - Revision.


; ===========================================================
; pmInit:
;     Switching to Protected Mode
; #ps: Maybe this routine needs to receive arguments.
; Talvez essa rotina deva receber argumentos. 
;
; routine:
; + Keep the interrupts disabled.
; + Setup registers 
; + GDTR - Carrega o gdtr do stage 2.
; + CR0 - PE ; Muda PE.
; + clear register
; + switch to protected mode.
; + get boot mode.
; + flush pipeline. jmp.
;

pmInit:
; Called by mShell in s2modes.inc

    cli

    mov ax, 0 
    mov ds, ax
    mov es, ax
    ;mov fs, ax  
    ;mov gs, ax  
    xor ax, ax
    mov ss, ax
    mov sp, 0x6000 

    lgdt [stage2_gdtr]

    mov eax, cr0
    or al, 1
    mov cr0, eax

    mov edx, dword 0
    mov ecx, dword 0
    mov ebx, dword 0
    mov eax, dword 0

    call get_boot_mode
    mov bl, al

; Flush pipeline
pmTrampoline:
    jmp stage2_CODE_SEL:pm32_start

[BITS 32]
pm32_start:

    ; Text mode debug
    ; mov byte [0B8000h], byte 'P'
    ; jmp $

; Initializing the boot mode.
; Probably here we are in boot mode 0 because bl is 0.

    mov byte [BOOT.modo], bl

; Set up registers
; Stack = 0x0009FFFC 
; Directory = 9C000 ?

    mov ax, stage2_DATA_SEL 
    mov ds, ax
    mov es, ax
    ;mov fs, stage2_NULL_SEL 
    ;mov gs, stage2_NULL_SEL 

    mov ss, ax
    mov esp, dword 0x0009FFFC

; LDT
; Initialize ldt with a NULL selector.
; Intel VT needs that.

    xor ebx, ebx
    lldt bx

; Put 0 in cr2. It's good in the case of Page Faults.
    xor ebx, ebx
	mov	cr2, ebx

    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx

.do_start:
    
; set IOPL to zero and reset all flags
    ;push 0x00000002
    ;popfd

; Enable PMC in userspace.
    ;mov eax, cr4
    ;or eax, 0x100
    ;mov cr4, eax

; #important:
; We have a working function to return to real mode.
; It will not be used now.

; start:
; Initialize the mode save in [BOOT.modo].
; IN: AL = Boot mode.
; #bugbug: Maybe this variable in corrupted.
; See: start32.inc

;;
;; == Go to Start =======================================
;;

    ; Go to 'start' carring the boot mode in al
    mov al, byte [BOOT.modo]
    ;JMP start
    JMP start32
    JMP $

;;
;; ==================================================================
;;

; ==================================================
; Contexto dos registradores ap�s o JMP para o start.
;
; Segments:
;    DS = stage2_DATA_SEL  
;    ES = stage2_DATA_SEL
; Stack:
;    Segment = stage2_DATA_SEL
;    Offset  = 0x0009FFFC
; Registers:
;    AL  = New initialization mode.
;    EBX = ECX = EDX = 0.


;;
;;    ****    Voltando para o Modo Real.    ****
;;

;;
;; O que segue s�o rotinas de fun��es que ser�o chamadas do modo protegido
;; e ser�o atendidas com recursos do BIOS.
;; @todo: Os ponteiros para essas fun��es devem ser passados para o boot loader.
;; na verdade podemos passar um ponteiro para uma tabela de ponteiros. Cada
;; ponteiro do bloco ser� um ponteiro para uma rotina que atende servi�os
;; usando recursos do bios. Esse bloco fica bem aqui.
;;

;;Essa tabela precisa ser inicializada.
;rm_services_block:
;    .shutdown dd 0     ;<<----- rmShutdown
;    ;...


;;
;; 'rm' ser� a classe de fun��es que efetuam um servi�o em modo real,
;; sendo chamadas do modo protegido.
;;

;;===========================================================
;; rmShutdown:
;;     Desliga a m�quina fica APM. N�o tem argumentos. 
;;     Obs: Estamos usando a gdt do m�dulo de 32 bit

rmShutdown:
; Called by do_shutdown in shcalls.inc

    mov ax, DATA_SEL    
    mov ds,ax
    mov es,ax
    ;mov fs,ax
    ;mov gs,ax

    mov ss,ax
    mov esp, dword 0x0009FFFC

; Importante:
; Daqui pra frente n�o usaremos mais o seletor para 32bit, 
; somente o seletor para modo protegido de 16bit.

; Pula para o o modo protegido de 16bit, 
; usando o seletor de 16bit.

    jmp RM_Code_Sel:pmode_16bit

[BITS 16]

pmode_16bit:

; Ajusta os seletores de codigo 
; para modo protegido de 16 bits.

    mov ax, RM_Data_Sel
    mov ss, ax
    mov ds, ax
    mov es, ax
	;mov	fs, ax
	;mov	gs, ax

	;Carrega ivt = 0:0

    lidt [ivtr_rm] 

; 5. 
; Clear the PE flag in the CR0 register 
; to switch to real-address mode.

   mov eax, cr0
   and al, 0xFE
   mov cr0, eax
   
;######################################
; 6. 
; Execute a far JMP instruction to jump to a real-address mode program. 
; This operation flushes the instruction queue and loads the appropriate 
; base and access rights values in the CS register.

; Usando um jmp de 16 bits modo real.
    JMP 0:do_rm 
    ;;
do_rm:

; 7. 
; Load the SS, DS, ES, FS, and GS registers as needed by the 
; real-address mode code. If any of the registers are not going 
; to be used in real-address mode, write 0s to them.

    cli

    ;;segmento 0
    mov ax, 0x0000 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Create stack
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFF

; Reprogram PIC: 
; change IRQ 0-7 to INT 08h-0Fh, and  
;        IRQ 8-15 to INT 70h-77h

    mov al, 10001b       ; Begin PIC 1 initialization.
    out 20h, al
    mov al, 10001b       ; Begin PIC 2 initialization.
    out 0A0h, al
    mov al, 08h          ; IRQ 0-7: back to ints 8h-Fh.
    out 21h, al
    mov al, 70h          ; IRQ 8-15: back to ints 70h-77h.
    out 0A1h, al
    mov al, 100b         ; Slave connected to IRQ2.
    out 21h, al
    mov al, 2
    out 0A1h, al
    mov al, 1            ; Intel environment, manual EOI.
    out 21h, al
    out 0A1h, al
    in al, 21h

; Mask off all interrupts for now. 
; PERMITE TODAS AS INTS.
; #importante:
; Enable interrupts

    mov al, 0
    out 0x21, al
    out 0xA1, al

    sti 

.enableTextMode:

; Set text mode for screen
; Mode 3.
; Importante:
; Mensagem em modo real.

    mov ax, 0x0003 
    int 0x10

    mov si, rm_shutdown_msg1
    call DisplayMessage

.stop:
; wait for a key

    xor AX, AX
    int 16h

;Another message.
    ;mov si, rm_shutdown_msg1
    ;call DisplayMessage

.doShutdown:
; See: main.asm

    call stage2Shutdown
    jmp $

;;
;; == Messages ===============================================
;;

rm_shutdown_msg1: 
    DB 'pm.inc: [RM] Press a key to shutdown', 13, 10, 0

;; ...


;;
;; ===================================================
;;


;; Temos que mudar para 32bit [bits 32]
;; para n�o estragar codigos subsequ�ntes.

[bits 32]   


;; ====================================================================
;; rmStartGUI:
;; 
;;     Inicializa o modo gr�fico padr�o.
;;     Essa rotina est� em 32bit e foi chamada pelo shell. 
;;     Essa rotina poder� ser chamada pelo shell em 32 bit modo texto,
;;     que est� dentro do boot manager. 
;;     Obs: 
;;     Estamos usando a gdt do m�dulo de 32 bit      
;;
;; ****  IMPORTANTE  ****
;; BL.BIN j� foi carregado por BM.BIN usando os recursos do BIOS.
;; S� nos resta passar o comando para BL.BIN.
;;

;; Called by do_boot in shcalls.inc

rmStartGUI:

    mov ax, DATA_SEL 
    mov ds, ax
    mov es, ax
    ;mov fs, ax
    ;mov gs, ax

    mov ss,ax
    mov esp, dword 0x0009FFFC

; #importante:
; Pula para o o modo protegido de 16 bit, 
; usando o seletor de 16 bit.
; Daqui pra frente n�o usaremos mais o seletor para 32 bit, 
; somente o seletor para modo protegido de 16 bit.

    jmp RM_Code_Sel:pmode_16bit_startgui

[BITS 16]

pmode_16bit_startgui:

; Ajusta os seletores de c�digo para 
; modo protegido de 16 bit.

    mov ax, RM_Data_Sel
    mov ss, ax
    mov ds, ax
    mov es, ax
    ;mov fs, ax
    ;mov gs, ax

    ;debug
    ;jmp $

    ; Carrega ivt = 0:0

    lidt [ivtr_rm] 

; 5. 
; Clear the PE flag in the CR0 register to switch to real-address mode.


   mov eax, cr0
   and al, 0xFE
   mov cr0, eax


;; ###############################################################
;; 6. 
;; Execute a far JMP instruction to 
;; jump to a real-address mode program. 
;; This operation flushes the instruction queue and 
;; loads the appropriate base and 
;; access rights values in the CS register.
;;

;; #importante:
;; Usando um jmp de 16 bit modo real.

    JMP 0:do_rm_startgui 

    ;; Nothing.

do_rm_startgui:

;; 7. 
;; Load the SS, DS, ES, FS, and GS registers as needed by the 
;; real-address mode code. If any of the registers are not going 
;; to be used in real-address mode, write 0s to them.

;; Segmento 0

    cli
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Create stack
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFF

;; Reprogram PIC: 
;; Voltando ao estado legado.
;; Changes: 
;; IRQ 0-7 to INT 08h-0Fh  
;; IRQ 8-15 to INT 70h-77h

    mov al, 10001b       ; Begin PIC 1 initialization.
    out 20h, al
    mov al, 10001b       ; Begin PIC 2 initialization.
    out 0A0h, al

    mov al, 08h          ; IRQ 0-7: back to ints 8h-Fh.
    out 21h, al

    mov al, 70h          ; IRQ 8-15: back to ints 70h-77h.
    out 0A1h, al

    mov al, 100b         ; Slave connected to IRQ2.
    out 21h, al
    mov al, 2
    out 0A1h, al
    mov al, 1            ; Intel environment, manual EOI.
    out 21h, al
    out 0A1h, al
    in al, 21h

; Mask off all interrupts for now. 
; PERMITE TODAS AS INTS.
; Enable interrupts

    mov al, 0
    out 0x21, al
    out 0xA1, al

; #importante
; Renable interrupts.

    sti 

; #importante:
; Entraremos no modo texto para exibimos mensagens em modo real.
; set text mode for screen

.enableTextMode:

; #bugbug
; N�o precisamos disso, pois estamos entrando no modo
; gr�fico atrav�s do vesa?
; Mas o que acontece se falhar?
; Precisaremos de um verbose em modo texto.

    mov ax, 0x0003
    int 0x10

	;; #Importante:
	;; Mensagem em modo real.
	;; Suspenderemos essa mensagem porque est� degradando o desempenho.
	;; Ela ser� uma op��o de debug.

	;; #debug
	;; mov si, rm_startgui_msg1
	;; call DisplayMessage


;.stop:

    ;; #debug
    ;; wait for a key
    ;; xor AX, AX 
    ;; int 16h

.doStartGUI:

    ; #bugbug: Isso foi para o fim dessa fun�ao.
    ; Set mode.
    ;mov al, byte BOOTMODE_GUI
    ;call set_boot_mode


    ;;
    ;; == Video mode ================================================
    ;;
    
    ;; #todo
    ;; The video mode depends on the 'gramado mode'
    ;; We get the 'gramado mode' in the root dir entry,
    ;; and the default mode comes from the configuration
    ;; in the main.asm file
    ;; video: 0x010F, 0x0112, 0x0115, 0x0118

    ;; default: JAIL !
    xor ax, ax
    
    ;mov al, byte 0x00   ;; JAIL
    ;mov al, byte 0x01   ;; P1
    ;mov al, byte 0x02   ;; HOME
    ;mov al, byte 0x03   ;; P2
    ;mov al, byte 0x04   ;; CASTLE
    ;mov al, byte 0x05   ;; CALIFORNIA
    ;; ...

    ;; Selecionamos primeiramente a 
    ;; configura�ao default feita no main.asm,
    ;; mas em seguida pegamos a que nos foi oferecida 
    ;; pelo comando de boot.
    ;; boot, bootjail, bootp1 ... etc ...
    ;; o comando de boot configura o metafile em [META$FILE.GRAMADO_MODE];
    
    ;; See: main.asm
    mov al, byte GRAMADO_DEFAULT_MODE
    
    ;; #todo
    ;; Some routine needs to get this value for us
    ;; in the root dir, entry 'B'RAMADO. 'B' means boot configuration.
    ;;
    
    ;; Pegamos a que nos foi oferecida 
    ;; pelo comando de boot.
    ;; boot, bootjail, bootp1 ... etc ...
    ;; o comando de boot configura o metafile em [META$FILE.GRAMADO_MODE];
    
    mov al, byte [META$FILE.GRAMADO_MODE]


    ;; #todo:
    ;; Temos que rever essa configura�ao acima,
    ;; por enquanto quem decide eh o comando de boot.
    ;; parece divertido.
    

;; ======================
;; 0x010F - 320x200
.Lgramado_mode_JAIL:
    cmp al, byte 0x00
    jne .Lgramado_mode_P1
    mov word [META$FILE.VIDEO_MODE], 0x010F
    jmp ____gramado_mode_done


;; ======================
;; 0x0112 - 640x480
.Lgramado_mode_P1:
    cmp al, byte 0x01
    jne .Lgramado_mode_HOME
    mov word [META$FILE.VIDEO_MODE], 0x0112
    jmp ____gramado_mode_done


;; ======================
;; 0x0115 - 800x600
.Lgramado_mode_HOME:
    cmp al, byte 0x02
    jne .Lgramado_mode_P2
    mov word [META$FILE.VIDEO_MODE], 0x0115
    jmp ____gramado_mode_done


;; ======================
;; 0x0118 - 1024x768
.Lgramado_mode_P2:
    cmp al, byte 0x03
    jne .Lgramado_mode_CASTLE
    mov word [META$FILE.VIDEO_MODE], 0x0118
    jmp ____gramado_mode_done


;; ======================
;; ??
.Lgramado_mode_CASTLE:
    cmp al, byte 0x04
    jne .Lgramado_mode_CALIFORNIA
    mov word [META$FILE.VIDEO_MODE], 0x0118 ;;todo: more then that.
    jmp ____gramado_mode_done


;; ======================
;; ??
.Lgramado_mode_CALIFORNIA:
    cmp al, byte 0x05
    jne .Lgramado_mode_CASTLE
    mov word [META$FILE.VIDEO_MODE], 0x0118   ;;todo: more then that.
    jmp ____gramado_mode_done


;#todo
;.Lgramado_mode_CALIFORNIA2:
;.Lgramado_mode_CALIFORNIA3:
;.Lgramado_mode_CALIFORNIA4:
;; ...


;;
;; == FAIL ====================================================
;;
    
;; Well, it fails.
;; So we need to use the default video mode,
;; and select the default 'gramado mode' in the arguments.
    
;; #IMPORTANTE
;; Defini��o do arquivo main.asm

    mov word [META$FILE.VIDEO_MODE], G_VIDEO_MODE


;;================================================================
;; ____gramado_mode_done:
;;
;;

; Inicializa o modo selecionado.
; See: stage2/lib16/s2modes.inc
    
____gramado_mode_done:

;; Saving the gramado mode in a tmp variable.
;; We will save in the boot block in the future.
;; See: bootloader.inc. Defined here.
;; See: start.inc. Where we setup the boot block
    
    xor eax, eax
    mov al, byte [META$FILE.GRAMADO_MODE]
    mov dword [tmp_gamemode], eax
    
    
    ; Set mode.
    ; gui or cli.
    mov al, byte BOOTMODE_GUI
    call set_boot_mode

; See: stage2/lib16/s2modes.inc

    JMP s2modesActivateMode 
    JMP $


;; Mensagens para a rotina de shutdown.
rm_startgui_msg1: 
    db  'pm.inc: [RM] Press a key to start GUI', 13, 10, 0
; ...



;;
;; ===========================================================
;;


;; #importante
;; Temos que mudar para 32bit [bits 32]
;; para n�o estragar codigos subsequ�ntes.

[bits 32]

;
; End.
;

