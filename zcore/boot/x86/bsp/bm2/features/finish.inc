; finish.inc
; This is a trampoline where we select
; if we're gonna start the GUI or the CLI.
; Created by Fred Nora.

; 1 = Starts system GUI.
; 0 = Starts the Boot Manager CLI.
finish_saved_boot_option db 0

; =======================================================
; Messages

stage2_msg_pe_sigNotFound: 
    db "finish.inc: Signature not found", 13, 10, 0
stage2_msg_pe_sigFound: 
    db "finish.inc: Signature found", 13, 10, 0


; ==============================================================
; AFTER_DATA:
; Called by load_stuff() in main.asm.
; Inicio real do stage 2.
; A primeira coisa a se fazer eh salvar os parametros de disco 
; passados pelo stage1.
; Argumentos recebidos:
;     bx = Magic number. (autorizaçao)
;     ax = Number of heads.
;     dl = Drive number.
;     cl = Sectors per track.
;     si = BPB.

AFTER_DATA:

; Data segments at 0x0000.
    mov ax, 0 
    mov ds, ax
    mov es, ax 

; Message: Boot Manager Splash.
; See: s2header.inc
    mov si, msg_bm_splash
    call DisplayMessage

    ; #debug
    ; jmp $

;; PE signature for BL.BIN image.
;; Check if we have the PE signature at the beginning of the file.
;; #todo
;; We need to rething this because everything alse in the system 
;; is using the ELF header.

xxx_checkSig:

    mov ax, 0x2000
    mov gs, ax 

; Probe for '4C'. The first byte.
    xor bx, bx
    mov al, byte [gs:bx]    ; 0x2000:0 
    cmp al, byte 0x4C       ;'L' primeiro byte
    jne .sigNotFound

; Probe for '01'. The second byte.
    mov bx, 1
    mov al, byte [gs:bx]  ; 0x2000:1 
    cmp al, byte 0x01     ; Segundo byte
    jne .sigNotFound

; ========
; Found:
; If we found both bytes, so the file was found.
    jmp .sigFound

; ========
; Not Found
; File not found. Final message and hang.
.sigNotFound:
    mov si, stage2_msg_pe_sigNotFound
    call DisplayMessage
.sigHalt:
    cli
    hlt
    jmp .sigHalt

; ========
; Found
; A assinatura foi encontrada ... 
; prosseguimos com o stage2.
; message: 
; O arquivo esta presente na mem�ria.
.sigFound:
    mov si, stage2_msg_pe_sigFound
    call DisplayMessage
    ;debug
    ;jmp $
; Turn off fdc motor.
xxx_turnoffFDCMotor:
    mov dx, 3F2h 
    mov al, 0 
    out dx, al
; Reset PS/2 mouse.
xxx_setupPS2Mouse:
    mov ax, 0c201h
    int 15h

; Set up the registers.
; + Data segments in 0x0000.
; + Stack segment in 0x0000.
; + Stack pointer in 0x6000.
xxx_setupRegisters:
    cli
    ; Segments
    mov ax, 0 
    mov ds, ax
    mov es, ax
    ;mov fs, ax 
    ;mov gs, ax
    ; Stack at 0000:6000 
    xor ax, ax
    mov ss, ax
    mov sp, 0x6000
    ; Clear registers.
    xor dx, dx
    xor cx, cx
    xor bx, bx
    xor ax, ax
    sti

; Enable a20 line.
; see: rm/s2a20.inc
xxx_setupA20:

    pusha
    call A20_fast_enable
    ;; call A20_enable
    mov si, msg_a20
    call DisplayMessage
    popa

; ======================================
; Selecting the initialization mode for the boot manager.
;  +1 = Embedded shell.
;  +2 = GUI.
; Savinf al into META$FILE.INIT_MODE.

xxx_Config:

; Message
; See: s2header.inc
.setupBootMode:
    mov si, msg_selecting_videomode
    call DisplayMessage
    ; #debug
    ; JMP $

;; ++
;; =====================================
.preSelection:

; 1 = Starts system GUI.
; 0 = Starts the Boot Manager CLI.

; Select GUI. 
; No bm-shell in 32bit assembly.
    
    mov al, byte [finish_saved_boot_option]
    ;mov al, 1

    ; Initialize Gramado boot loader.
    cmp al, 1
    je .xxxxGUI
    cmp al, 0
    je .xxxxCLI

    jmp .xxxxGUI
;; =====================================
;; --


;
; Go!
;


;; ===========================================
;; == Text mode ==============================
; text mode.
;   ## SET UP BOOT MODE ##
.xxxxCLI:
    mov al, byte BOOTMODE_SHELL
    call set_boot_mode
    JMP s2modesActivateMode  ; s2modes.inc
    jmp $

;; ===========================================
;; == Graphics mode ==========================
; gui mode.
; Initialize Gramado boot loader.
;   ## SET UP BOOT MODE ##
.xxxxGUI:
    mov word [META$FILE.VIDEO_MODE], G_VIDEO_MODE
    mov byte [META$FILE.GRAMADO_MODE], 0x02     ; GRAMADO MODE: HOME 800x600
    mov al, byte BOOTMODE_GUI
    call set_boot_mode
    JMP do_rm_startgui  ; pm.inc
    jmp $

