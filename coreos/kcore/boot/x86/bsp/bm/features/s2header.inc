;
; File: s2header.inc 
;
; This is the header file for the Gramado Boot Manager.
; Defines and variables used in the Stage 2.
;
; *Importante:
; O arquivo stage2.inc faz uma sequência de inclusões de módulos
; de 16bit que compõem o stage2. Os promeiros devem ser s2metafile.inc 
; e s2header.inc que servirão de suporte para todos os outros modulos
; do stage2.
;
; 2015 - Created by Fred Nora.
;


;
; SATA Controller Modes: 
; IDE, AHCI, or RAID. Enabling RAID mode also enables 
; the AHCI features.
;
; IDE mode is the simplest mode. In IDE mode, the hard drive is set 
; to run as an IDE or Parallel ATA (PATA) hard drive.
;
; AHCI:
; =====
;     Advanced Host Controller Interface.
;     AHCI is a newer way to interface with the SATA controller. 
;     This allows you to take advantage of SATA features 
;     such as Native Command Queuing. 
;     AHCI mode becomes really relevant with SSD for 
;     queuing and processing commands and for some other features.
;
;

; Definições usadas apenas em 16bit.
; #bugbug: Isso deveria ficar num arquivo mais global.
%DEFINE BOOTMANAGER_VER '1.1'
;;...



; Definições.

;;
;; #BUGBUG: 
;; Talvez o diretório raiz deva ser carregado em outro lugar quando
;; estamos em 16 bit. Em 32bit o boot manager utiliza endereços altos 
;; com facilidade aqui em 16bit é mais difícil, se não impossível.
;; @todo: Encontrar novos endereços para esses elementos
;; que estejam dentro do limite de 1MB.
;; Vamos tentar em 70000h, fica bem depois do boot loader e antes da pilha em 90000.
;; Obs: valores improvisados, potencialmente podem causar bug. #bugbug.
;;
;; es:bx    "Os valores aqui são referentes ao offset apenas."
;;          "Lembrese de configurar a base na rotina" 
;;
;Valores para formatação com imdisk. (volume 31mb)
S2_FAT16_VBR_ADDRESS       equ 0x0000; 0x001A0000  ;0x00020000.
S2_FAT16_VBR_LBA           equ 63 ;134         ;vbr.
S2_FAT16_FAT_ADDRESS       equ 0x1000 ;0x001B0000  ;0x00060000.
S2_FAT16_FAT_LBA           equ 67 ;136         ;fat1. 
S2_FAT16_ROOTDIR_ADDRESS   equ 0x2000 ;0x001C0000  ;0x00080000.
S2_FAT16_ROOTDIR_LBA       equ 559 ;264         ;root.
S2_FAT16_DATAAREA_ADDRESS  equ 0x3000 ;0x00100000
S2_FAT16_DATAAREA_LBA      equ 591 ;296         ;data. 
;;... 


;Modos de inicialização.
BOOTMODE_SHELL  EQU 1    ;Shell do Boot Manager em modo texto.
BOOTMODE_GUI    EQU 2    ;GUI.

;
; Kernel info:
; ============
;
; O KERNEL.BIN será carregado em 0x00100000 pelo BL.BIN.
;


;@todo: bug bug (trocar)
;Esses Valores estão errados, o certo é 0x00100000.
kernel_seg            equ 0900h
kernel_off            equ 0h
kernel_entry_point    equ 0x00008200

;
; FAT info:
; ========
;

FAT16_FILE_SEGMENT      equ 0x2000
FAT16_FILE_OFFSET       equ 0x0000  

; Algumas definições.

;VGA display memory segment.
VGA_VIDEO_SEGMENT       equ  0A000h  

;Offset of BIOS variable.
SCREEN_WIDTH_IN_BYTES   equ  044Ah   

;Bytes in each font char TESTANDO 16 PRA USAR ALGUMA FONT DA ROM.
FONT_CHARACTER_SIZE     equ  8  

; VGA register equates.
SC_INDEX                equ  3C4h    ;SC index register.
SC_MAP_MASK             equ  2       ;SC map mask register index.
GC_INDEX                equ  3CEh    ;GC index register.
GC_SET_RESET            equ  0       ;GC set/reset register index.
GC_ENABLE_SET_RESET     equ  1       ;GC enable set/reset register index.
GC_ROTATE               equ  3       ;GC data rotate/logical function  register index.
GC_MODE                 equ  5       ;GC Mode register.
GC_BIT_MASK             equ  8       ;GC bit mask register index.
TEST_TEXT_ROW           equ  69      ;row to display test text at.
TEST_TEXT_COL           equ  17      ;column to display test text at.
TEST_TEXT_WIDTH         equ  8       ;width of a character in pixels.


;status do modo gráfico.
g_graphic_mode_status:  db 0
;Font.
FontPointer:            dd 0
;Segmentos em modo real.
DS_REAL_MODE:           DW 0
ES_REAL_MODE:           DW 0
; Variáveis usadas pelo carregador de arquivos.
qual_file               db 0 
destino_seg             dw 0
destino_off             dw 0
; Aqui ficará o armazenado o endereço do nome do arquivo.
ImageNamePos:           dw 0
;Disco.
absoluteSector          db 0x00
absoluteHead            db 0x00
absoluteTrack           db 0x00
datasector              dw 0x0000
cluster                 dw 0x0000
salva_carregar_cs       dw 0
salva_carregar_ip       dw 0
salva2_carregar_cs      dw 0
salva2_carregar_ip      dw 0
;s1 BPB.
salva_s1_bpb            dw 0

;
; BPB pointers.
;
g_Stage1BPB dw 0    ;Para rotinas de 16 bit.
g_Stage2BPB dw 0    ;Para rotinas de 16 bit.
g_Fat16BPB  dd 0    ;Para rotinas de 32 bit.
; ...


;;
;; == Messages =====================================================
;;

;;
;; Splash.
;;

msg_bm_splash:
    db 'Gramado Boot Manager ', 
    db 'version ', BOOTMANAGER_VER, 13, 10, 0 



msg_s2_init: 
    db 13, 10, "s2: Initializing stage 2", 13, 10, 0

msg_s2_activating_mode:       
    db "s2: Activating mode", 13, 10, 0

msg_s2_failure:               
    db "s2: No sig", 13, 10, 0  


;msg
msg_iniciando_bootmanager    db "s2: Initializing",         13, 10, 0
msg_kernel_carregado         db "s2: Kernel carregado",     13, 10, 0
msg_desligando_disquete      db "s2: Motor off",            13, 10, 0
msg_initializing_VESA        db "s2: Initializing VESA",    13, 10, 0
msg_a20                      db "s2: A20",                  13, 10, 0
msg_detect_harware           db "s2: Detecting hw",         13, 10, 0
msg_selecting_videomode      db "s2: Selecting video mode", 13, 10, 0
msg_tudo_carregado           db "s2: All loaded",0
msg_back                     db "s2: RM",0
;msg
msgfile1                     db "s2: L",0 
msgLoading                   db "s2: L",0
msgFailure                   db "s2: NOT FOUND",0
msgFim                       db "s2: Loaded",0
msgOK                        db "s2: Executing app",0
msgCRLF                      db " ",0
msgProgress                  db ".",0
;debug
debug_msg_1                  db "s2: Press any key",0
debug_msg_2                  db "s2: loaded.",0
debug_msg_3                  db "s2: F",0
debug_msg_done               db "s2: Loaded, returning",0
;outros
kernel_msg                   db "s2: Loading KERNEL.BIN", 13, 10, 0 
s_a20                        db "s2: A20", 13, 10,0 
s_test_mode12h               db "s2: Mode 12h",0 

s2header_boot_loader_name  db  'BL      BIN',0


;Mensagens usadas pelo sistema de arquivos.
;msg_s2fat16_Loading db 'L',13,10,0
msg_s2fat16_sf_root_ok   db 'Root Ok',13,10,0
msg_s2fat16_sf_root_fail db 'R',13,10,0  ;;Root Not Found.
msg_s2fat16_diskfail     db 'DF',0
msg_s2fat16_Failure      db 'F',13,10,0 ;File not found.
msg_s2file_ok            db 'File found',13,10,0  
msg_s2fat16_step1        db '~1',13,10,0
msg_s2fat16_step2        db '~2',13,10,0
msg_s2fat16_step3        db '~3',13,10,0
msg_s2fat16_step4        db '~4',13,10,0
;...

;
;fim.
;
