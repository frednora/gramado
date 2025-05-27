; main.asm 
; Main file of Boot manager.
; 2005 - Created by Fred Nora. 

; What is this?
; This is a boot manager, it is able to load files,
; has a popup window with two buttons and a 
; embedded command interpreter.

; Purpose:
; the main goal of this program is loading the 
; bootloader image into the memory and jump to it 
; passing some parameters.

; #todo
; Create the bootloader METAFILE.
; We need to create a configuration method 
; where we have the option to open the initial window or not.
; It is necessary for non-interactive systems like server machine.
; Maybe we're gonna use a form of metafile in the empy sectors 
; of the disk. This metafile will contain the flag that
; say if we're gonna open the window or not.
; The metafile will have some more options.

; Credits: MikeOS (License: BSD).
; Video modes:
; ============
; VirtualBox:
; ===============
; Oracle VirtualBox: 0x0112   640x480x24bpp
; Oracle VirtualBox: 0x0115   800x600x24BPP
; Oracle VirtualBox: 0x0118  1024x768x24BPP
; Nvidia GeForce:  
; =================
; GeForce_8400_GS equ 0x06E4  
; GeForce 8400 GS: 0x0115   800x600x32BPP
; The main resolution is 800x600x24.
; The only way to change the resolution for now is 
; changing a global variable in this document.

;---------------------------------------------------
; #importante
; This image was loaded in 0H:8000H.
; It's easier to handle the gdt stuff 
; when the org is in 8000H and the segment is 0H.
; 32768 - 65535 (hex: 8000h - FFFFh)

; #bugbug
; 32KB is the limit for this program?
; We are almost there.

;       +------------+
;       |     ...    | 
;       |------------| 
;       | BLGRAM.BIN | 
;       |   (32bit)  | Boot loader in C.
;       |------------| 0x2000:0x0000
;       |            | 
;       |     ...    | 
;       |            | 
;       |------------| 0x0000:0xFFFF
;       |            | 
;       |   BM2.BIN  | 
;       |            | The entry point.
;  >>>  |------------| 0x0000:0x8000
;       |            |
;       |------------| 0x0000:0x6000
;       |   INITIAL  | Initial stack address.
;       |    STACK   | It goes down.
;       |------------| 
;       |            |
;       |------------| 
;       |  FAT/ROOT  |
;       |------------| 0x0000:0x1000
;       |            |
;       +------------+

	%DEFINE GBM_VER '1.1'	; version number


[ORG 0x8000]
; We are in 0000H:8000H

; 32768 - 65535 (hex: 8000h - FFFFh)
; 32KiB space for BM2.BIN

;;=====================================
;;    Entry point do Boot Manager    ;;
;;=====================================
;; The gramado mode
;; it will select the video mode.
;; The video mode depends on the gramado mode.
;; See: rmStartGUI on pm.inc
;; This is the default mode.
;; But actually the mode is selected by the user
;; in the command shell.
;; It's like a 'gamemode'.

;;GRAMADO_DEFAULT_MODE EQU 0x00  ; jail        320x200
;;GRAMADO_DEFAULT_MODE EQU 0x01  ; p1          640x480
GRAMADO_DEFAULT_MODE EQU 0x02  ; home        800x600
;;GRAMADO_DEFAULT_MODE EQU 0x03  ; p2          1024x768
;;GRAMADO_DEFAULT_MODE EQU 0x04  ; castle      ??
;;GRAMADO_DEFAULT_MODE EQU 0x05  ; california  ??
;; ...

;; == Selecting the mode =======================
;; This is the default video mode.
;; we will use this if the 'gramado mode' fail.
;; the video mode depends on the 'gramado mode'
;; Global variables to set the video mode 
;; Well tested modes.
;; ok on qemu.
;; It works on nvidia geforce too, but with 32bpp.

;; 24bpp on qemu
;; G_VIDEO_MODE EQU 0x010F    ;320x200
G_VIDEO_MODE EQU 0x0112    ;640x480
;; G_VIDEO_MODE EQU 0x0115    ;800x600
;; G_VIDEO_MODE EQU 0x0118    ;1024x768
;; ...
;; ======================================

;; 16 bit:
;; Estamos no primeiro setor do BM.BIN, ele começa em 16 bit.
[bits 16]

; Entry point.
; This is the entry point for the BM.BIN.
; Jump after the data area.
os_call_vectors:
    jmp bm_main
    ;jmp bm_vector1
    ;jmp bm_vector2
    ;jmp bm_vector3
    ;jmp bm_vector4
    ; ...

; Data
; see: features/disk.inc
ROOTDIRSTART EQU  (bootmanagerOEM_ID)
ROOTDIRSIZE  EQU  (bootmanagerOEM_ID+4)

; ...

; ========
; bm_main:
; The real entry point.
; #importante: 
; O unico argumento passado pelo MBR foi o numero do disco.
; IN: dl = Disk number.
; /dev/sda - 0x80
; /dev/sdb - 0x81
; /dev/sdc - 0x82
; /dev/sdd - 0x83

bm_main:
; Entry point. (16bits)

; Set up registers.
; Adjust segment registers and stack.
; Stack located at 0000:0x6000.
; Code located at 0000:0x8000. 

    cli
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0x6000 
    sti

; The default direction for string operations
; will be 'up' - incrementing address in RAM
    cld

; Data segments in 0x0000.
    mov ax, 0x0000
    mov ds, ax
    mov es, ax

; Save disk number.
    mov byte [bootmanagerDriveNumber], dl
    mov byte [META$FILE.DISK_NUMBER], dl
    mov byte [DISKINFO16_disk_number], dl

; =====================
; Reset keyboard controller.
; see:
; http://www.ctyme.com/intr/int-16.htm

; Int 16/AX=0601h - AAKEYS - EMPTY KEY-EVENT BUFFER
    ;xor ax, ax
    ;mov ax,  0601h
    ;int 0x16

; Int 16/AH=04h - Tandy 2000 - KEYBOARD - FLUSH KEYBOARD BUFFER
    ;xor ax, ax
    ;mov ah,  04h
    ;int 0x16

; Int 16/AH=25h - HUNTER 16 - RESET KEYBOARD
    ;xor ax, ax
    ;mov ah,  25h
    ;int 0x16

; Int 16/AX=6F09h - HP Vectra EX-BIOS - F16_KBD_RESET - RESET KEYBOARD TO DEFAULTS
    ;xor ax, ax
    ;mov ax,  6F09h
    ;int 0x16

; Int 16/AH=FFh - KEYBOARD - KBUF extensions - ADD KEY TO TAIL OF KEYBOARD BUFFER
    ;xor ax, ax
    ;mov ah,  0xFF
    ;int 0x16

; #test
; Set the keyboard repeat rate to the max
    ;; mov ax, 0x0305
    ;; xor bx,bx 
    ;; int 0x16

; get keystroke
; Int 16/AH=00h - KEYBOARD - GET KEYSTROKE
; On extended keyboards, this function discards any extended keystrokes, 
; returning only when a non-extended keystroke is available. 
    ;xor ax, ax
    ;int 0x16

; Get disk info.
; Get drive parameters: 
; =====================
; Return: CF set on error.
; AH = status (07h).
; CF clear if successful.
; AH = 00h.
; AL = 00h on at least some BIOSes.
; BL = drive type (AT/PS2 floppies only). 
; CH = low eight bits of maximum cylinder number.
; CL = maximum sector number (bits 5-0).
;      high two bits of maximum cylinder number (bits 7-6).
; DH = maximum head number.
; DL = number of drives.

    xor ax, ax
    mov ah, byte 08h
    int 0x13 

; Heads.
; Numero de heads.
; Logical last index of heads = (number_of - 1). 
; (Because index starts with 0).
; Obs: 
; O valor de Heads foi gravado no BPB mas precisar� ser passado a diante
; para uso posterior.

    xor ax, ax
    mov al, dh
    inc ax    ;From 0-based to count.
    ;Number of heads.
    mov word [bootmanagerNumHeads], ax
    mov word [META$FILE.HEADS], ax
    mov word [DISKINFO16_heads], ax

; Sectors Per Track e Cylinders.
; Essas informações apresentam meio truncadas
; O valor do número de cilindros é aprentado
; de forma parcial, uma parte em cada registrador.
; spt:
; bits [5:0] logical last index of sectors per track = number_of 
; (because index starts with 1).
; cyl:
; bits [7:6] [15:8] logical last index of cylinders = number_of - 1 
; (because index starts with 0).
; Sectors Per Track - (SPT).
; "Esconde 2 bits que pertencem a quantidade de setores".
; Obs: 
; O valor de SPT foi gravado no BPB mas precisar 
; ser passado a diante para uso posterior.

    xor eax, eax 
    mov al, cl
    and al, byte 00111111b            ;03Fh
    mov byte [SectorsPerTrack], al    ;BPB (word).
    ;Sectors per track.
    mov ah, 0                                  ; enviamos apenas 'al' 
    mov word [bootmanagerSectorsPerTrack], ax  ; enviamos apenas 'al'
    mov word [META$FILE.SPT], ax
    mov word [DISKINFO16_spt], ax   

; Cylinders
; Obs: 
; O valor de CylinderNumbers foi gravado em variável 
; mas precisar ser passado a diante para uso posterior.

    xor eax, eax
    mov al, cl           ; Two high bits of cylinder number in bits 6&7.
    and al, 11000000b    ; Mask it.
    shl ax, 2            ; Move them to bits 8&9.
    mov al, ch           ; Rest of the cylinder bits.(low 8 bits)
    inc eax              ; Number is 0-based.
    ; Numero de cilindros do disco.
    mov word [save_cylinder_numbers], ax
    mov word [META$FILE.CYLINDERS], ax
    mov word [DISKINFO16_cylinders], ax


; ===========================================
; #test

; Generate a signature.
; Signature: 0xEE, 0xAA, 0xD8, 0x??
; #todo: 
; In the future we're gonna use some random value 
; generated on the fly.
    mov al, 0xEE
    mov byte [CONFIG_BUFFER2 +0], al 
    mov al, 0xAA
    mov byte [CONFIG_BUFFER2 +1], al 
    mov al, 0xD8
    mov byte [CONFIG_BUFFER2 +2], al 
    mov al, [bootmanagerDriveNumber]  ; Disk number from boot manager
    mov byte [CONFIG_BUFFER2 +3], al 
; Get one more byte. Return in AL.
; See: finish.inc
    call get_random_byte
    mov byte [CONFIG_BUFFER2 + 4], al   ; Store the random result in your signature
    call get_random_byte
    mov byte [CONFIG_BUFFER2 + 5], al   ; Store the random result in your signature
    call get_random_byte
    mov byte [CONFIG_BUFFER2 + 6], al   ; Store the random result in your signature
    call get_random_byte
    mov byte [CONFIG_BUFFER2 + 7], al   ; Store the random result in your signature

; Sector 4
; Write disk configuration
    MOV AH, 0x03            ; BIOS write sector function
    MOV AL, 0x01            ; Write 1 sector
    MOV CH, 0x00            ; Cylinder 0
    MOV CL, 0x04            ; Sector 4 (1-based index)
    MOV DH, 0x00            ; Head 0
    MOV DL, [bootmanagerDriveNumber]  ; Disk number from boot manager
    MOV BX, CONFIG_BUFFER2  ; Memory location containing the config data
    INT 0x13                ; Call BIOS disk services

; ===========================================
; #test

; Byte 0: 
; Boot Mode (0x4D = Menu, 0x53 = Skip)
; Bytes 1-3: 
; Signature ("CNF")
; Byte 4:    
; Graphics Mode (0x00 = 320x200, 0x01 = 640x480, 0x02 = 800x600)

; Sector 2
; Read boot configuration 
    MOV AH, 0x02        ; BIOS read sector function
    MOV AL, 0x01        ; Read 1 sector
    MOV CH, 0x00        ; Cylinder 0
    MOV CL, 0x02        ; Sector 2 (1-based index)
    MOV DH, 0x00        ; Head 0
    MOV DL, [bootmanagerDriveNumber]  ; Disk number
    MOV BX, CONFIG_BUFFER      ; Memory location to store data
    INT 0x13            ; Call BIOS

; Compare first byte

    ; M?
    MOV AL, [CONFIG_BUFFER]    ; Load first byte of sector into AL
    CMP AL, 0x4D        ; Compare with 'M' (ASCII)
    JE MATCH_FOUND      ; Jump if match found

    ; S?
    MOV AL, [CONFIG_BUFFER]    ; Load first byte of sector into AL
    CMP AL, 0x53        ; Compare with 'S' (ASCII)
    JE SKIP_MENU         ; Jump if match found

; Default option. Open the menu.
NoValidOption:
    jmp menu_loop

SKIP_MENU:
    MOV AH, 0x0E        ; BIOS teletype output function
    MOV AL, 'S'         ; Print 'S' for no match
    INT 0x10
    ; Select boot option.
    ; 1 = Starts system GUI.
    ; 0 = Starts the Boot Manager CLI.
    mov byte [finish_saved_boot_option], 1  ; default option.
    jmp after_menu
    ;JMP $

MATCH_FOUND:
    MOV AH, 0x0E
    MOV AL, 'M'         ; Print 'M' for match found
    INT 0x10
    jmp menu_loop
    ;JMP $

; Allocate 512 bytes for sector storage
DebugBuffer01: db "CONFIG_BUFFER"
CONFIG_BUFFER: 
    TIMES 512 DB 0

; Allocate 512 bytes for sector storage
DebugBuffer02: db "CONFIG_BUFFER2"
CONFIG_BUFFER2: 
    TIMES 512 DB 0

; ++
; ===========================================
menu_loop:

; Draw the background.
    mov ax, msg_topbar       ; Set up the welcome screen
    mov bx, msg_bottombar
    mov cx, 10011111b         ; Colour: white text on light blue
    call os_draw_background

; Draw the dialog box.
; See: dialog.inc
    mov ax, dialog_string_1  ; Ask if user wants app selector or command-line
    mov bx, dialog_string_2
    mov cx, dialog_string_3
    mov dx, 1                ; We want a two-option dialog box (OK or Cancel)
    call os_dialog_box

; Select boot option.
; 1 = Starts system GUI.
; 0 = Starts the Boot Manager CLI.
    mov byte [finish_saved_boot_option], 1  ; default option.

; If OK (option 0) chosen, start app selector
; O right foi selecionado?
; Se não foi o right então carregue a imagem.
; usaremos a opçao de boot default.
; Load the BLGRAM.BIN image.
    cmp ax, 1 
; >> GUI::
    jne near after_menu

; Caso contrario usaremos a opção CLI.
; 1 = Starts system GUI.
; 0 = Starts the Boot Manager CLI.
    mov byte [finish_saved_boot_option], 0
; >> CLI::
    jmp after_menu

    jmp menu_loop
    jmp $
; ===========================================
; --

; Clear the screen
after_menu:
    call Window.ClearScreen

;
; Load the BLGRAM.BIN image at 2000H:0000H
;

; Load the BLGRAM.BIN image at 2000H:0000H.
; see: features/disk.inc
; This routine starts the system based on a flag,
; that will tell us if we start the system using the graphics mode
; or we start the 32bit embedded shell here in the bm.
; IN:
; ax = pointer to the image name.

load_bootloader_image:

; Load image1
    mov ax, word ImageName_GramadoOSBootloader
    call diskLoadBL

; Load image2
    ;mov ax, word ImageName_GramadoOSBootloader
    ;call diskLoadBL


; Trampoline:
; see: features/finish.inc
Trampoline:
    push WORD 0
    push WORD AFTER_DATA 
    retf

; ================================================
; DATA
; ================================================
; Data for the above code...

    msg_topbar     db 'Gramado Boot Manager', 0
    msg_bottombar  db 'ENTER=Confirm', 0

    dialog_string_1  db 'Please select an option:', 0
    dialog_string_2  db '+ [OK] to initialize the system       ', 0
    dialog_string_3  db '+ [Cancel] for command line           ', 0

; Gramado OS bootloader.
ImageName_GramadoOSBootloader:
    db "BLGRAM  BIN", 0x0D, 0x0A, 0x00

; Another OS bootloader.
;ImageName_AnotherOSBootloader:
    ;db "12345678BIN", 0x0D, 0x0A, 0x00

;
; == Includes ========
;

; 16bit includes.
    %include "features/s2metafile.inc"
    %include "features/s2header.inc"
    %include "features/s2bpb.inc"
    %include "features/s2gdt.inc"
    %include "features/s2vesa.inc" 
    %include "features/s2config16.inc" 
    %include "features/s2a20.inc"
    %include "features/s2lib.inc"
    %include "features/s2fat12.inc"
    %include "features/s2fat16.inc"
    %include "features/s2menu16.inc"
    %include "features/s2modes.inc"
    %include "features/s2detect.inc"
    %include "features/lib16.inc"
    %include "features/disk.inc"
    %include "features/screen.inc"
    %include "features/dialog.inc"
    %include "features/finish.inc"
    %include "features/int21h.inc"
    %include "features/pm.inc"

;--------------------------------------------------------
; 32 bits - (Boot Manager 32bit Asm.)
;--------------------------------------------------------

[bits 32]

bootmanager_main:

; Em ordem de prioridade na compilação.

; 14 - Header principal. 
; Definições globais usadas em 32bit.
; Header principal em 32 bits.
    %include "k32/header32.inc"
; 13 - Headers. 
    %include "k32/system.inc"       ; System configuration.
    %include "k32/init.inc"         ; Initialization configuration.
    %include "k32/sysvar32.inc"     ; Variáveis do sistema.
    %include "k32/x8632/gdt32.inc"  ; Gdt
    %include "k32/x8632/idt32.inc"  ; Idt
    %include "k32/x8632/ldt32.inc"  ; Ldt
    %include "k32/x8632/tss32.inc"  ; Tss
    %include "k32/stacks32.inc"     ; Stacks
    %include "k32/x8632/ints32.inc"    ; Handles para as interrupções.
    %include "k32/fs/fat16header.inc"  ; Headers para o sistema de arquivos fat16.
; 12 - Monitor.
    %include "k32/drivers/screen32.inc"  ; Rotinas de screen em 32 bits.
    %include "k32/drivers/input32.inc"   ; Rotinas de input 2m 32 bits.
    %include "k32/string32.inc"  ; Rotinas de strings em 32 bits.
    %include "k32/font32.inc"    ; Fonte.
; 11 - Hardware.
    %include "k32/x8632/cpuinfo.inc"  ; CPU
    %include "k32/hardware.inc"       ; Hardware
    ; ...
; 10 - Irqs.
    %include "k32/drivers/timer.inc"     ; Irq 0, Timer.
    %include "k32/drivers/keyboard.inc"  ; Irq 1, Keyboard.
    %include "k32/drivers/fdc32.inc"     ; Irq 6, Fdc. (@todo: Suspender o suporte.)
    %include "k32/drivers/clock.inc"     ; Irq 8, Clock.
    %include "k32/drivers/hdd32.inc"     ; Irq 14/15, Hdd.
    ; ...
; 9 - Tasks. (#no tasks)
; Rotinas de inicialização do sistema de tarefas.
    %include "k32/tasks32.inc"   
; 8 - lib32.
; Rotinas em 32 bits. 
; 7 - setup  
; Inicializa arquitetura.
    %include "k32/setup.inc"
; 6 - Disk.
    %include "k32/fs/fat12pm.inc"   ;FAT12 em 32 bits.
    %include "k32/fs/fat16lib.inc"  ;FAT16 (rotinas).
    %include "k32/fs/fat16.inc"     ;FAT16 (funçoes principais).
    %include "k32/fs/ramfs.inc"     ;RamDisk fs.
    %include "k32/fs/format.inc"    ;Formata.
    %include "k32/fs/fs32.inc"      ;fs, (gerencia os sistemas de arquivos).
; 5 - File.
    %include "k32/installer.inc"   ;Instala metafiles em LBAs específicas.
    %include "k32/fs/file.inc"     ;Operaçoes com aquivos.
    %include "k32/bootloader.inc"  ;Carrega o Boot Loader (BLGRAM.BIN).
; 4 - Debug.
; System debug.
    %include "k32/debug.inc"
; 3 - blconfig.
; Gerencia a inicialização.
    %include "k32/blconfig.inc"
; 2 - Boot Manager 32bit Mini-Shell.
; Prompt de comandos.
    %include "k32/shell32/shell.inc"
    %include "k32/shell32/shcalls.inc"  ;Chamadas dos comandos.
    %include "k32/shell32/shlib.inc"    ;Lib de funções do Shell.
    %include "k32/shell32/shmsg.inc"    ;Mensagens e variáveis do Shell.
; 1 - Start.
    %include "k32/start.inc"
; 0 - lib32.
;Rotinas em 32 bits.
    %include "k32/lib32.inc" 
; ========================================================

;
; Buffer
;

root_buffer:
fat_buffer:
    ; nop

;
; End
;

