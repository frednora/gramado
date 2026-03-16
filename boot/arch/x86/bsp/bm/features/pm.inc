;
; File: pm.inc
;     stage3
;     +Enable Protected Mode with no Paging.
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
;     ?    - Created by Fred Nora.
;     2015 - Adapted from older version.
;     2016 - Revision.


; ===========================================================
; pmInit:
;     Switching to Protected Mode.
; obs: 
; Talvez essa rotina deva receber argumentos. 
; Na inicialização essa rotina é chamada em modo texto.
; routine:
; Keep the interrupts disabled.
; Setup registers 
; GDTR - Carrega o gdtr do stage 2.
; CR0 - PE ; Muda PE.
; clear register
; switch to protected mode.
; get boot mode.
; flush pipeline. jmp.

pmInit:
; Called by mShell in s2modes.inc.

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

; Flush pipeline.

pmTrampoline:
    jmp stage2_CODE_SEL:Gramado_pm32
    [BITS 32]
Gramado_pm32:

    ; Text mode debug.
    ; mov byte [0B8000h], byte 'P'
    ; jmp $

;
; ## Inicalizando modo de boot ##
;

; Salva o boot mode em um endereço de 32 bits.
; #bugbug: O bl foi zerado um pouco antes.
; Provavelmente podemos deletar isso.
; É preciso conferir se não usamos essa variável 
; logo a frente.

; Aqui estamos selecionando o modo '0'.
    mov byte [BOOT.modo], bl


;
; Reconfigurando registradores.
;

; #todo: 
; Talvez dê para passar outras coisas através dos registradores b, c, d.
; #obs: 
; A pilha está bem perto do diretório do kernel. ??.
; Stack = 0x0009FFFC 
; Directory = 9C000.  

; Set up registers.

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

; LDT
; Initialize ldt with a NULL selector.
; Intel VT needs that.

    xor ebx, ebx
    lldt bx

.do_start:
    
; set IOPL to zero and reset all flags
    ;push 0x00000002
    ;popfd

; Enable PMC in userspace.

    ;mov eax, cr4
    ;or eax, 0x100
    ;mov cr4, eax

; IMPORTANTE: 
; A ROTINA PARA VOLTAR PARA O MODO REAL E EXECUTAR UM SERVIÇO 
; FUNCIONOU CORRETAMENTE, MAS NÃO SERÁ CHAMADA NESSE MOMENTO.

; start:
; Inicia o modo escolhido que foi salvo em 'BOOT.modo'.
; IN: AL = Boot mode.
; #bugbug: 
; Podemos ter um proble nessa variável BOOT BOOT.modo.
; start.inc
; See: start.inc

;;
;; == Go to Start =======================================
;;

    mov al, byte [BOOT.modo]

    JMP start
    JMP $

;;
;; ==================================================================
;;

; ==================================================
; Contexto dos registradores após o JMP para o start.
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
;; O que segue são rotinas de funções que serão chamadas do modo protegido
;; e serão atendidas com recursos do BIOS.
;; @todo: Os ponteiros para essas funções devem ser passados para o boot loader.
;; na verdade podemos passar um ponteiro para uma tabela de ponteiros. Cada
;; ponteiro do bloco será um ponteiro para uma rotina que atende serviços
;; usando recursos do bios. Esse bloco fica bem aqui.
;;

;;Essa tabela precisa ser inicializada.
;rm_services_block:
;    .shutdown dd 0     ;<<----- rmShutdown
;    ;...


;;
;; 'rm' será a classe de funções que efetuam um serviço em modo real,
;; sendo chamadas do modo protegido.
;;

;;===========================================================
;; rmShutdown:
;;     Desliga a máquina fica APM. Não tem argumentos. 
;;     Obs: Estamos usando a gdt do módulo de 32 bit

;; Called by do_shutdown in shcalls.inc

rmShutdown:

    mov ax, DATA_SEL    
    mov ds,ax
    mov es,ax
    ;mov fs,ax
    ;mov gs,ax

    mov ss,ax
    mov esp, dword 0x0009FFFC

; Importante:
; Daqui pra frente não usaremos mais o seletor para 32bit, 
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
;; para não estragar codigos subsequêntes.

[bits 32]   


;; ====================================================================
;; rmStartGUI:
;; 
;;     Inicializa o modo gráfico padrão.
;;     Essa rotina está em 32bit e foi chamada pelo shell. 
;;     Essa rotina poderá ser chamada pelo shell em 32 bit modo texto,
;;     que está dentro do boot manager. 
;;     Obs: 
;;     Estamos usando a gdt do módulo de 32 bit      
;;
;; ****  IMPORTANTE  ****
;; BL.BIN já foi carregado por BM.BIN usando os recursos do BIOS.
;; Só nos resta passar o comando para BL.BIN.
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
; Daqui pra frente não usaremos mais o seletor para 32 bit, 
; somente o seletor para modo protegido de 16 bit.

    jmp RM_Code_Sel:pmode_16bit_startgui

[BITS 16]

pmode_16bit_startgui:

; Ajusta os seletores de código para 
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
; Não precisamos disso, pois estamos entrando no modo
; gráfico através do vesa?
; Mas o que acontece se falhar?
; Precisaremos de um verbose em modo texto.

    mov ax, 0x0003
    int 0x10

	;; #Importante:
	;; Mensagem em modo real.
	;; Suspenderemos essa mensagem porque está degradando o desempenho.
	;; Ela será uma opção de debug.

	;; #debug
	;; mov si, rm_startgui_msg1
	;; call DisplayMessage


;.stop:

    ;; #debug
    ;; wait for a key
    ;; xor AX, AX 
    ;; int 16h

.doStartGUI:

    ; #bugbug: Isso foi para o fim dessa funçao.
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
    ;; configuraçao default feita no main.asm,
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
    ;; Temos que rever essa configuraçao acima,
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
;; Definição do arquivo main.asm

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
;; para não estragar codigos subsequêntes.

[bits 32]

;
; End.
;

