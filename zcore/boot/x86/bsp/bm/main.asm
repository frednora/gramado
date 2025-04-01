; main.asm 
; Main file of Boot manager.
; 2005 - Created by Fred Nora. 

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
;       |   BM.BIN   | 
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



[ORG 0x8000]

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
GRAMADO_DEFAULT_MODE EQU  0x02  ; home        800x600
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
G_VIDEO_MODE EQU  0x0112    ;640x480
;; G_VIDEO_MODE EQU 0x0115    ;800x600
;; G_VIDEO_MODE EQU 0x0118    ;1024x768
;; ...
;; ======================================


__FAT_SEGMENT EQU  0x6000  ;0
__FAT_OFFSET  EQU  0x0200  ;0x1000 

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

ROOTDIRSTART EQU  (bootmanagerOEM_ID)
ROOTDIRSIZE  EQU  (bootmanagerOEM_ID+4)

;; #todo
;; Change some names here!
bootmanagerOEM_ID             db "GRAMA-OS"
bootmanagerBytesPerSector     dw 0x0200
bootmanagerSectorsPerCluster  db 1
bootmanagerReservedSectors    dw 62
bootmanagerTotalFATs          db 0x02
bootmanagerMaxRootEntries     dw 0x0200
bootmanagerTotalSectorsSmall  dw 0x0000
bootmanagerMediaDescriptor    db 0xF8
bootmanagerSectorsPerFAT      dw 246
bootmanagerSectorsPerTrack    dw 0x11
bootmanagerNumHeads           dw 4
; Hidden sectors:  1+1+3 (mbr+vbr+reserved sectors depois do vbr).
bootmanagerHiddenSectors      dd 5
bootmanagerTotalSectorsLarge  dd 0
bootmanagerDriveNumber        db 0x80
bootmanagerFlags              db 0x00
bootmanagerSignature          db 0x29
bootmanagerVolumeID           dd 0x980E63F5
bootmanagerVolumeLabel        db "GRAMA  BMBM"
bootmanagerSystemID           db "FAT16   "

; Número de cilindros do disco.
; Salvando alguns argumentos passados pelo MBR. 
save_cylinder_numbers: dw 0
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
; Code located at 0000:0x8000. 
; Stack located at 0000:0x6000.

    cli
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0x6000 
    sti

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

; #test
; Set the keyboard repeat rate to the max
    ;; mov ax, 0x0305
    ;; xor bx,bx 
    ;; int 0x16

; Heads.
; Numero de heads.
; Logical last index of heads = (number_of - 1). 
; (Because index starts with 0).
; Obs: 
; O valor de Heads foi gravado no BPB mas precisar� ser passado a diante
; para uso posterior.

    xor ax, ax
    mov al, dh
    inc ax                            ;From 0-based to count.
    ;Number of heads.
    mov word [bootmanagerNumHeads], ax
    mov word [META$FILE.HEADS], ax
    mov word [DISKINFO16_heads], ax

; Sectors Per Track e Cylinders.
; Essas informa��es apresentam meio truncadas
; O valor do n�mero de cilindros � aprentado
; de forma parcial, uma parte em cada registrador.

;spt.
;  bits [5:0] logical last index of sectors per track = number_of 
;  (because index starts with 1).
;cyl.
;   bits [7:6] [15:8] logical last index of cylinders = number_of - 1 
;  (because index starts with 0).


; Sectors Per Track - (SPT).
; "Esconde 2 bits que pertencem a quantidade de setores".
; Obs: 
; O valor de SPT foi gravado no BPB mas precisar� 
; ser passado a diante para uso posterior.

    xor eax, eax 
    mov al, cl
    and al, byte 00111111b             ;03Fh
    mov byte [SectorsPerTrack], al    ;BPB (word).
    ;Sectors per track.
    mov ah, 0                                  ; enviamos apenas 'al' 
    mov word [bootmanagerSectorsPerTrack], ax  ; enviamos apenas 'al'
    mov word [META$FILE.SPT], ax
    mov word [DISKINFO16_spt], ax   

; Cylinders
; Obs: 
; O valor de CylinderNumbers foi gravado em vari�vel mas precisar� ser 
; passado a diante para uso posterior.

    xor eax, eax
    mov al, cl                 ; Two high bits of cylinder number in bits 6&7.
    and al, 11000000b          ; Mask it.
    shl ax, 2                  ; Move them to bits 8&9.
    mov al, ch                 ; Rest of the cylinder bits.(low 8 bits)
    inc eax                    ; Number is 0-based.
    ; Numero de cilindros do disco.
    mov word [save_cylinder_numbers], ax
    mov word [META$FILE.CYLINDERS], ax
    mov word [DISKINFO16_cylinders], ax

; ========================================
; Carregar root.
; Where? es:bx  0:0x1000

bootmanager_LOADROOT:

; Compute size of root directory and store in "cx".

    xor cx, cx
    xor dx, dx
    mov ax, 0x0020                        ; 32 byte directory entry.
    mul WORD [bootmanagerMaxRootEntries]  ; Total size of directory.
    div WORD [bootmanagerBytesPerSector]  ; Sectors used by directory.
    mov word [ROOTDIRSIZE], ax
    mov cx, ax
    ;xchg ax, cx

; Compute location(LBA) of root directory and 
; store in "ax".

    xor ax, ax
    mov al, BYTE [bootmanagerTotalFATs]        ; Number of FATs.
    mul WORD [bootmanagerSectorsPerFAT]        ; Sectors used by FATs.
    add ax, WORD [bootmanagerReservedSectors]  ; Adjust for bootsector.
    add ax, WORD [bootmanagerHiddenSectors]

; Nesse momento ax contem o setor inicial do root dir.

    mov word [ROOTDIRSTART], ax   
    add  ax, cx
    mov WORD [bootmanagerdatasector], ax     ; base of root directory
    ;Read root directory into memory (0:1000) ?
    ;mov WORD [bootmanagerdatasector], 591  ;;SIMULADO In�cio da �rea de dados.

    mov ax, word [ROOTDIRSTART]  ; Inicio do root.
    mov cx, word [ROOTDIRSIZE]   ; Tamanho do root.
    mov bx, 0x1000               ; root_buffer. Copy root dir above bootcode
    call bootmanagerReadSectors

    pusha
    mov si, bootmanagermsgCRLF
    call bootmanagerDisplayMessage
    popa

    ;Debug breakpoint.
    ;jmp $

    ;Browse root directory for binary image.
    mov cx, WORD [bootmanagerMaxRootEntries]  ; Load loop counter.
    mov di, 0x1000                            ; root_buffer, 0x1000, locate first root entry. ?
.bootmanagerLOOP:
    push cx
    mov cx, 0x000B                         ; eleven character name
    mov si, ImageName_GramadoOSBootloader  ; image name to find
    pusha
    call bootmanagerDisplayMessage
    popa
    push di
    rep cmpsb                       ; test for entry match
    pop di
    je bootmanager_LOADFAT
    pop cx
    add di, 0x0020                  ; queue next directory entry
    loop .bootmanagerLOOP
    jmp bootmanagerFAILURE


; -------------------------------------
; Load FAT.
; Where? es:bx  0:0x1000

;; Se o nome for encontrado.
bootmanager_LOADFAT:

    pusha
    mov si, bootmanagermsgFAT
    call bootmanagerDisplayMessage
    popa

    ; Debug breakpoint.
    ;mov ah, 0x00
    ;int 0x16

; Save starting cluster of boot image.
; Offset '0x001A' of the entry that starts in 'di'.
; After saving we can override the root.
    mov dx, WORD [di + 0x001A]
    mov WORD [bootmanagercluster], dx  ; file's first cluster.

; #BUGBUG ?
; NAO ESTAMOS CARREGANDO A FAT INTEIRA.
; CARREGAR A FAT INTEIRA DA PROBLEMA.

; Read FAT into memory (es:bx).?? Onde ??
; ?? 0:0x1000 
; ?? Qual é o segmento e o offset da FAT ??

    ;mov ax, 0
    mov ax, __FAT_SEGMENT
    mov es, ax
    ;mov bx, 0x1000    ; fat_buffer ; copy FAT above bootcode.
    mov bx, __FAT_OFFSET

; Compute location of FAT and store in "ax".

    mov ax, WORD [bootmanagerHiddenSectors]    ; adjust for bootsector.
    add ax, WORD [bootmanagerReservedSectors]  ; lba inicial da fat ?.

; #bugbug
; Number of sectors from FAT.
; We need to load 246 sectors.
    ; WORD [bootmanagerSectorsPerFAT]
    mov cx, 128  ; 64KB (1 segment)
    call bootmanagerReadSectors

; Nesse momento ja carregamos a FAT.

    ;Debug breakpoint. 
    ;jmp $

; -----------------------------------------
; Load image.
; Read image file into memory (0x2000:0)(es:bx)
; Destination for the image.
; es:bx = (2000:0).

; Message.
    mov si, bootmanagermsgImg
    call bootmanagerDisplayMessage

; Opçao de mensagem.
; mov si, bootmanagermsgCRLF
; call bootmanagerDisplayMessage

    mov ax, 0x2000 
    mov es, ax
    mov bx, 0x0000

;
; Ajust fat segment.
; 

; gs:bx para a FAT.
; FAT segment.
; Salva o offset da imagem.

    push bx 

    ;mov ax, 0
    mov ax, __FAT_SEGMENT
    mov gs, ax 

;
; Loading the image.
;

bootmanager_LOADIMAGE:

    mov ax, WORD [bootmanagercluster]  ; Cluster to read.
    pop bx                             ; Buffer to read into (offset da imagem).
    call bootmanagerClusterLBA         ; Convert cluster to LBA.

    xor cx, cx
    mov cl, BYTE 1    ;[bootmanagerSectorsPerCluster] ;sectors to read.
    call bootmanagerReadSectors

    push bx
    ;Compute next cluster.
    mov ax, WORD [bootmanagercluster]  ; Identify current cluster.
    add ax, ax                         ; 16 bit(2 byte) FAT entry.
    ;mov bx, 0x1000                     ; fat_buffer, offset.
    mov bx, __FAT_OFFSET
    add bx, ax                         ; Index into FAT.
    ;TESTANDO...
    mov dx, WORD [gs:bx]               ; Read two bytes from FAT.
.bootmanagerDONE:
    mov WORD [bootmanagercluster], dx  ; store new cluster.

; EOF
; 0x0FF0 test for end of file.

    cmp dx, 0xFFFF
    jne bootmanager_LOADIMAGE
    ;jnb bootmanager_LOADIMAGE 

bootmanagerDONE:

    ;mov si, bootmanagermsgCRLF
    ;call bootmanagerDisplayMessage

;========================
; Esse eh primeiro setor do BM.BIN, ele ira carregar o arquivo BLGRAM.BIN 
; e ira passar o comando para o stage 2 do (BM).
;=======================

    mov si, bootmanagermsgDONE
    call bootmanagerDisplayMessage

    ;Debug breakpoint.
    ;jmp $

; ===================================
; Importante:
; >> Nesse momento j� conseguimos carregar o BLGRAM.BIN em 0x2000:0. Agora 
; passamos o comando para o stage2 do BM, onde configuramos o m�quina,
; entramos em modo gr�fico, em modo protegido e por fim entramos no 
; mini-shell do BM.
; >> Observe que o in�cio do BM est� em 16bit. Aproveitamos isso para
; carregarmos o arquivo BLGRAM.BIN com a ajuda dos recursos do BIOS. Isso
; nos oferece um pouco de tranquilidade. Ent�o, j� que o BM, cumpriu 
; seu principal objetivo logo no in�cio do c�digo, podemos usar o resto 
; dele para rotinas mais detalhadas de obten��o de informa��es sobre a 
; arquitetura x86. Assim podemos passar para o BLGRAM.BIN o maior n�mero de 
; inform��es poss�veis, e deix�-lo em um estado confort�vel.
; Sendo assim, o BM.BIN, pode ser um programa com um tamanho um pouco 
; maior, mas talvez isso torne o trabalho o MBR mais dif�cil.
; =====================================


; Go!
; Agora saltamos para a trap que existe no in�cio do META$FILE.
; Trap 1. (isso est� nesse arquivo mesmo)

.goToFisrtTrap:

; #todo
; No momento estamos carregando um bootloader de 32bit
; feito em C e Assembly.
; Mas poderíamos carregar um kernel de 16bit nesse endereço
; imitando o estilo do mikeos.

; Nesse momento saltamos para uma rotina que 
; comutará para modo protegido de 32bit e
; executará um shell embutido nesse programa.
; Esse shell consegue voltar para 16.

    push WORD 0
    push WORD AFTER_DATA 
    retf

; Fail. 
; #todo: Colocar uma mensagem de erro.
bootmanagerFAILURE:
    int 0x18
    jmp $
    ;mov  si, bootmanagermsgFailure
    ;call  bootmanagerDisplayMessage
    ;mov  ah, 0x00
    ;int  0x16         ; await keypress
    ;int  0x19         ; warm boot computer

; bootmanagerDisplayMessage:
;     Display ASCIIZ string at "ds:si" via BIOS.

bootmanagerDisplayMessage:
    lodsb                  ; load next character
    or al, al              ; test for NUL character
    jz .bootmanagerDONE
    mov ah, 0x0E           ; BIOS teletype
    mov bh, 0x00           ; display page 0
    mov bl, 0x07           ; text attribute
    int 0x10               ; invoke BIOS
    jmp bootmanagerDisplayMessage
.bootmanagerDONE:
    ret

; bootmanagerReadSectors:
;     Reads "cx" sectors from disk starting at "ax" into memory location 
; "es:bx".
;     Carrega na mem�ria, em es:bx, 'cx' setores do disco, come�ando pela 
; LBA 'ax'.
bootmanagerReadSectors:
    mov WORD [bootmanagerDAPBuffer], bx
    mov WORD [bootmanagerDAPBuffer+2], es
    mov WORD [bootmanagerDAPStart], ax
;Five retries for error. (8??)
.bootmanagerMAIN:
    mov di, 0x0005
.bootmanagerSECTORLOOP:
    push ax
    push bx
    push cx

    push si
    mov ah, 0x42
    mov dl, 0x80
    mov si, bootmanagerDAPSizeOfPacket
    int 0x13
    pop si

    jnc .bootmanagerSUCCESS    ; test for read error
    xor ax, ax                 ; BIOS reset disk
    int 0x13                   ; invoke BIOS
    dec di                     ; decrement error counter
    pop cx
    pop bx
    pop ax
    jnz .bootmanagerSECTORLOOP    ; attempt to read again
; Fail
    int 0x18

.bootmanagerSUCCESS:
    mov si, bootmanagermsgProgress
    call bootmanagerDisplayMessage
    pop cx
    pop bx
    pop ax
    add bx, WORD [bootmanagerBytesPerSector]    ; queue next buffer
    cmp bx, 0x0000  ;;??
    jne .bootmanagerNextSector
    push ax
    mov ax, es
    add ax, 0x1000
    mov es, ax
    pop ax
.bootmanagerNextSector:
    inc ax                               ; queue next sector
    mov WORD [bootmanagerDAPBuffer], bx    
    mov WORD [bootmanagerDAPStart], ax
    loop .bootmanagerMAIN                ; read next sector
    ret


; ====
; bootmanagerClusterLBA:
; convert FAT cluster into LBA addressing scheme
; LBA = (cluster - 2) * sectors per cluster

bootmanagerClusterLBA:
    sub ax, 0x0002     ; zero base cluster number.
    xor cx, cx
    mov cl, BYTE  1    ;[bootmanagerSectorsPerCluster] ; convert byte to word.
    mul cx
    add ax, WORD 591   ;[bootmanagerdatasector] ; base data sector.(#bugbug Valor determinado.)
    ret

; ===============================
; Dados de supporte.

; DAP.
bootmanagerDAPSizeOfPacket  db  10h
bootmanagerDAPReserved      db  00h
bootmanagerDAPTransfer      dw  0001h
bootmanagerDAPBuffer        dd  00000000h
bootmanagerDAPStart         dq  0000000000000000h

; Sector, Head, Track.
bootmanagerabsoluteSector   db 0x00
bootmanagerabsoluteHead     db 0x00
bootmanagerabsoluteTrack    db 0x00

bootmanagerdatasector  dw 0x0000  ; Data sector.
bootmanagercluster     dw 0x0000  ; Cluster.

; ===============================================
; Messages and strings.

; Gramado OS bootloader.
ImageName_GramadoOSBootloader:
    db "BLGRAM  BIN", 0x0D, 0x0A, 0x00

; Another OS bootloader.
;ImageName_AnotherOSBootloader:
    ;db "12345678BIN", 0x0D, 0x0A, 0x00

; Strings
bootmanagermsgFAT       db  0x0D, 0x0A, "Loading FAT",   0x0D, 0x0A, 0x00
bootmanagermsgImg       db  0x0D, 0x0A, "Loading Image", 0x0D, 0x0A, 0x00
bootmanagermsgFailure   db  0x0D, 0x0A, "ROOT", 0x00
bootmanagermsgFail      db  "Read", 0x00
bootmanagermsgSearch    db  "S", 0x00
bootmanagermsgProgress  db  "*", 0x00
bootmanagermsgDONE      db  0x0D, 0x0A, "DONE", 0x0D, 0x0A, 0x00
bootmanagermsgCRLF      db  0x0D, 0x0A, 0x00
;; ...
;=================
; Obs: 
; Aqui é o fim do Stage1 do Boot Manager.
; ===========================================================


;=================
; Obs: Aqui é começo do Stage2 do Boot Manager.

[bits 16]

; Stage 2.
; Esse é o segundo setor.
; Daqui pra frente temos código de inicialização.
; SEGUNDO SETOR

; ==========================================
; Gramado Boot Manager - This is the stage 2 for the boot manager.
; It's a 16bit/32bit program to make some basic system initialization and 
; to load the Boot Loader program.
; 2015-2017 Fred Nora.
; It starts on second sector.
; Importante:
; O arquivo stage2.inc faz uma sequ�ncia de inclus�es de m�dulos
; de 16bit que comp�em o stage2. Os promeiros devem ser s2metafile.inc 
; e s2header.inc que servir�o de suporte para todos os outros modulos
; do stage2.
; IMPORTANTE:
;      � IMPRESSIND�VEL A POSSIBILIDADE DE CARREGAR O 'BOOT LOADER' USANDO 
; OS RECURSOS DO BIOS DE 16BIT. Esse arquivo deve chamar a rotina de 
; carregamento de arquivo. Obs: J� est� implementada a rotina de carregar 
; um setor usando fat16 em 16bit usando recursos do BIOS.   
; Atribuiçoes:
;     +1 - Configura o sistema. 
;     +2 - Entra no modo de inicializa��o configurado.
; Modo de v�deo do Boot Manager:
; =============================
;     O Boot Manager usa o modo de texto, mas configura o modo de video
; de acordo com as especifica��es do metafile que est� no segundo setor.
; Estado dos registradores:
; ========================
; O 'stage2 e o resto do Boot Manager' s�o carregados em 0000h:1000h pelo 
; stage1 (MBR).
; O stage 2 inicia com os seguintes valores nos registradores:
;
;       ;cs:ip. 
;       CS = 0x0000
;       IP = 0x1000
;
;       ;Segmentos.
;       DS = 0x0000
;       ES = 0x0000
;
;       ;ss:sp. 
;       SS = 0x0000
;       SP = 0x0E00
;
;       SI = Bios Parameter Block
;
;       BX = Magic number (0xF0ED).
;       AX = Number of heads.
;       DL = Drive number.
;       CL = Sectors per track.
;
; Memory map:
; ==========
;     +VBR        = 8000:7c00 (Primeiro setor da parti��o ativa).
;     +FAT        = 6000:0000
;     +ROOT       = 4000:0000
;     +BootLoader = 2000:0000
;
; Informa��es sobre as parti��es:
; ==============================
; Parti��o 0 - (31MB).
;     vbr  - ?    
;     fat1 - ?
;     fat2 - ?
;     root - ?
;     data - ?
; Parti��o 1 - N�o usada.
; Parti��o 2 - N�o usada.
; Parti��o 3 - N�o usada.
;
; OBS: 
; � importante receber bem os par�metros de disco passados pelo stage1. 
; O stage2 salva os par�metros de disco no META$FILE carregado na mem�ria. 
; (volatil).
; Algumas constantes usadas pelo stage 2.
; Obs: Por conveni�ncia, o desenvolvedor pode manipular essas constantes.
;;Tipos de bootloader que poder�o ser carregados pelo boot manager.
;BOOTLOADER_TYPE_NONE      EQU 0  ;Sem tipo definido. Negligenciado.
;BOOTLOADER_TYPE_GRAMADO   EQU 1  ;Boot Loader do sistema operacional Gramado.
;BOOTLOADER_TYPE_MULTIBOOT EQU 1  ;Usando o padr�o multiboot.
;BOOTLOADER_TYPE_UNKNOW    EQU 2  ;Desconhecido.
;;...

; ==========================================
; Importante:
;     Nesse momento determinamos a localização, 
; no disco, dos elementos do sistema de arquivos FAT16.
;     Não é isso o que queremos. Essas informações precisam 
; ser obtidas através de rotinas de sondagem. 

; stage 2
CODE_SEGMENT   equ  0
DATA_SEGMENT   equ  0
STACK_SEGMENT  equ  0
STACK_POINTER  equ  0x6000
; vbr
VBR_SEGMENT  equ  8000H
VBR_OFFSET   equ  7C00H
VBR_LBA      equ  63
; fat
FAT_SEGMENT  equ  6000H
FAT_OFFSET   equ  0
FAT_LBA      equ  67
; root
ROOT_SEGMENT  equ  4000H
ROOT_OFFSET   equ  0
ROOT_LBA      equ  559

; ========================
; Aqui está a localização do bootloader na memória. 
; A LBAn�o importa, pois ele foi carregado do sistema sistema de 
; arquivos e a LBA inicial dele estava armazenada na entrada do 
; diret�rio raiz.

; Boot Loader.
BL_SEGMENT  equ  2000H
BL_OFFSET   equ  0
BL_LBA      equ  0

;--------------------------------------- 
; Algum suporte para cores no modo texto. 
; BLUE    equ     01f00h
; RED     equ     02f00h
; GREEN   equ     04f00h

;==================================
; stage2_main:
; Início do stage 2. 
; O endereço do stage 2 é 0000H:1000H.
; O stage 2 fica no segundo setor do disco.
; Jump
stage2_main:
    PUSH 0 
    PUSH AFTER_DATA 
    RETF

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
    ; ...

; ==============================================================
; AFTER_DATA:
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

; Segments at '0'.

    mov ax, 0 
    mov ds, ax
    mov es, ax 

; Message: Boot Manager Splash.
; See: s2header.inc

    mov si, msg_bm_splash
    call DisplayMessage

    ; Debug.
    ; jmp $

;; Checar se a assinatura PE est� na mem�ria, se estiver, pularemos e
;; a etapa de carregamento do arquivo.
;; #todo
;; Rever essa assinatudo, pois tudo no sistema agora usa ELF.

xxx_checkSig:

    mov ax, 0x2000
    mov gs, ax 

; Testando o 4C
    xor bx, bx
    mov al, byte [gs:bx]    ; 0x2000:0 
    cmp al, byte 0x4C         ;'L' primeiro byte
    jne .sigNotFound

; Testando o 01
    mov bx, 1
    mov al, byte [gs:bx]    ; 0x2000:1 
    cmp al, byte 0x01         ; Segundo byte
    jne .sigNotFound

; Se os dois char n�o est�o ausentes, 
; significa que o arquivo eta no lugar.
    jmp .sigFound

; ========
; Not Found
; A assinatura n�o foi encontrada,
; o arqui n�o est� na mem�ria.
; message: 
; O arquivo n�o esta presente na mem�ria.
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
xxx_setupRegisters:
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
    sti

; Enable a20 line.
; see: features/s2a20.inc
xxx_setupA20:

    pusha
    call A20_fast_enable
    ;; call A20_enable
    mov si, msg_a20
    call DisplayMessage
    popa

; xxx_Config
; Configurando o modo de inicializaçao do Boot Manager:
; ======================================
; Seleciona um modo de inicializa�ao para o Boot Manager.
; A op��o est� salva no metafile do Boot Mananger.
; Op��es:
;     +1 ~ Shell do boot manager.
;     +2 ~ GUI
; Configura o metafile. META$FILE.INIT_MODE = al

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
; Select CLI. 
; The bm-shell in 32bit assembly.

    mov al, 0 

    cmp al, 1
    je .xxxxGUI
    cmp al, 0
    je .xxxxCLI

    jmp .xxxxGUI
;; =====================================
;; --


;;
;; == Text mode ==============================
;;

; text mode.
;   ## SET UP BOOT MODE ##
.xxxxCLI:
    mov al, byte BOOTMODE_SHELL
    call set_boot_mode
    jmp .xxxxGO

;;
;; == Graphics mode ==========================
;;

; gui mode.
;   ## SET UP BOOT MODE ##
.xxxxGUI:
    mov word [META$FILE.VIDEO_MODE], G_VIDEO_MODE
    mov al, byte BOOTMODE_GUI
    call set_boot_mode
    jmp .xxxxGO

;
; Go!
;

;
; == \o/ ========
;

; Activate the chosen mode.
; (s2modes.inc)

.xxxxGO:
    JMP s2modesActivateMode  
    JMP $

;
; == \o/ ========
;

; ==================================
; stage2Shutdown:
;     Shutdown the machine via APM.
;     16bit, real mode, using BIOS.

stage2Shutdown:
; Connect to APM API
    MOV  AX, 5301h
    XOR  BX, BX
    INT  15h
; Try to set APM version (to 1.2)
    MOV  AX, 530Eh
    XOR  BX, BX
    MOV  CX, 0102h
    INT  15h
; Turn off the system
    MOV  AX, 5307h
    MOV  BX, 0001h
    MOV  CX, 0003h
    INT  15h
; Exit 
; (for good measure and in case of failure)
    RET

; =======================================================
; Messages

stage2_msg_pe_sigNotFound: 
    db "bm-xxx_checkSig: Signature not found", 13, 10, 0
stage2_msg_pe_sigFound: 
    db "bm-xxx_checkSig: Signature found", 13, 10, 0
; ===================================================

; Trampoline.
; pm
; Switch to protected mode.
; Comuta para o modo protegido.

    %include  "features/pm.inc"

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
    %include "k32/system.inc"     ; Arquivo de configura��o do sistema.
    %include "k32/init.inc"       ; Arquivo de configura��o da inicializa��o.
    %include "k32/sysvar32.inc"   ; Variáveis do sistema.
    %include "k32/x8632/gdt32.inc"  ; Gdt
    %include "k32/x8632/idt32.inc"  ; Idt
    %include "k32/x8632/ldt32.inc"  ; Ldt
    %include "k32/x8632/tss32.inc"  ; Tss.
    %include "k32/stacks32.inc"   ; Stacks
    %include "k32/x8632/ints32.inc"      ; Handles para as interrupções.
    %include "k32/fs/fat16header.inc"  ; Headers para o sistema de arquivos fat16.
; 12 - Monitor.
    %include "k32/drivers/screen32.inc"  ; Rotinas de screen em 32 bits.
    %include "k32/drivers/input32.inc"   ; Rotinas de input 2m 32 bits.
    %include "k32/string32.inc"  ; Rotinas de strings em 32 bits.
    %include "k32/font32.inc"    ; Fonte.
; 11 - Hardware.
    %include "k32/x8632/cpuinfo.inc"  ; Rotinas de detec��o e configura��o de cpu.
    %include "k32/hardware.inc"     ; Rotinas de detec��o e configura��o de hardware.
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
    %include "k32/fs/fat16.inc"     ;FAT16 (fun��es principais).
    %include "k32/fs/ramfs.inc"     ;RamDisk fs.
    %include "k32/fs/format.inc"    ;Formata.
    %include "k32/fs/fs32.inc"      ;fs, (ger�ncia os sistemas de arquivos).
; 5 - File.
    %include "k32/installer.inc"   ;Instala metafiles em LBAs espec�ficas.
    %include "k32/fs/file.inc"     ;Operaçoes com aquivos.
    %include "k32/bootloader.inc"  ;Carrega o Boot Loader (BLGRAM.BIN).
; 4 - Debug.
; System debug.
    %include "k32/debug.inc"
; 3 - blconfig.
; Gerencia a inicialização.
    %include "k32/blconfig.inc"
; 2 - Boot Manager Mini-Shell.
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

