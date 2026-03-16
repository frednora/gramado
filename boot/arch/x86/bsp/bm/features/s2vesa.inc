;
; File: s2vesa.inc
;
;     Setup graphic mode via VESA.
;
;---------------------------------------------------------;
;                                                         ;
; Autor: (c) Craig Bamford, All rights reserved.          ;
; Version: Frederico Martins Nora.                        ;
;                                                         ;
;=========================================================;
; Histórico de revisão:                                   ;
;                                                         ;
; Vesa Information Block                       01/05/2011 ;
; Versão                                    13/março/2013 ;
; revisão 1                                  27/maio/2013 ;
; revisão 2                                    7/jan/2014 ;
; revisão 3                                          2015 ;
;                                                         ;
;=========================================================;
frame_buffer  dd  0                                       ;
;=========================================================;
; VESA INFORMATION BLOCK                                  ;
;=========================================================;
VESA_Info:		                                          ;
    VESAInfo_Signature		    db	4                     ;
    VESAInfo_Version		    dw	1                     ;
    VESAInfo_OEMStringPtr		dd	1                     ;
    VESAInfo_Capabilities		db	4                     ;
    VESAInfo_VideoModePtr		dd	1                     ;
    VESAInfo_TotalMemory		dw	1                     ;
    VESAInfo_OEMSoftwareRev 	dw	1                     ;
    VESAInfo_OEMVendorNamePtr	dd	1                     ;
    VESAInfo_OEMProductNamePtr	dd	1                     ;
    VESAInfo_OEMProductRevPtr	dd	1                     ;
    VESAInfo_Reserved    		db	222                   ;
    VESAInfo_OEMData	    	db	0xff ;256             ;
;=========================================================;
; VESA MODE INFORMATION                                   ;
;=========================================================;
Mode_Info:	                                              ;
    ModeInfo_ModeAttributes 	  dw	1                 ;
    ModeInfo_WinAAttributes 	  db	1              	  ;
    ModeInfo_WinBAttributes 	  db	1             	  ;
    ModeInfo_WinGranularity 	  dw	1             	  ;
    ModeInfo_WinSize		      dw	1                 ;
    ModeInfo_WinASegment		  dw	1                 ;
    ModeInfo_WinBSegment		  dw	1                 ;
    ModeInfo_WinFuncPtr		      dd	1                 ;
    ModeInfo_BytesPerScanLine	  dw	1                 ;
    ModeInfo_XResolution		  dw	1  ;*width        ;
    ModeInfo_YResolution		  dw	1  ;*height       ;
    ModeInfo_XCharSize		      db	1                 ;
    ModeInfo_YCharSize		      db	1                 ;
    ModeInfo_NumberOfPlanes 	  db	1                 ;
    ModeInfo_BitsPerPixel		  db	1  ;*bpp          ;
    ModeInfo_NumberOfBanks		  db	1                 ;
    ModeInfo_MemoryModel		  db	1                 ;
    ModeInfo_BankSize		      db	1                 ;
    ModeInfo_NumberOfImagePages	  db	1                 ;
    ModeInfo_Reserved_page		  db	1                 ;
    ModeInfo_RedMaskSize		  db	1                 ;
    ModeInfo_RedMaskPos		      db	1                 ;
    ModeInfo_GreenMaskSize		  db	1                 ;
    ModeInfo_GreenMaskPos		  db	1                 ;
    ModeInfo_BlueMaskSize		  db	1                 ;
    ModeInfo_BlueMaskPos		  db	1                 ;
    ModeInfo_ReservedMaskSize	  db	1                 ;
    ModeInfo_ReservedMaskPos	  db	1                 ;
    ModeInfo_DirectColorModeInfo  db    1                 ;
;=========================================================;
; VBE 2.0 extensions.                                     ;
;=========================================================;
    ModeInfo_PhysBasePtr		dd	1    ;*LFB            ;
    ModeInfo_OffScreenMemOffset	dd	1                     ;
    ModeInfo_OffScreenMemSize	dw	1                     ;
;=========================================================;
; VBE 3.0 extensions.                                     ;
;=========================================================;
    ModeInfo_LinBytesPerScanLine  dw	1                 ;
    ModeInfo_BnkNumberOfPages	  db	1                 ;
    ModeInfo_LinNumberOfPages	  db	1                 ;
    ModeInfo_LinRedMaskSize 	  db	1                 ;
    ModeInfo_LinRedFieldPos 	  db	1                 ;
    ModeInfo_LinGreenMaskSize	  db	1                 ;
    ModeInfo_LinGreenFieldPos	  db	1                 ;
    ModeInfo_LinBlueMaskSize	  db	1                 ;
    ModeInfo_LinBlueFieldPos	  db	1                 ;
    ModeInfo_LinRsvdMaskSize	  db	1                 ;
    ModeInfo_LinRsvdFieldPos	  db	1                 ;
    ModeInfo_MaxPixelClock		  dd	1                 ;
;=========================================================;
; Reserved                                                ;
;=========================================================;
    ModeInfo_Reserved		db	190                       ;
;=========================================================;
; VESA MODE INFORMATION END                               ;
;=========================================================;
;
;Space for info ret by vid BIOS.
;
VESAINFO  db  'VBE2'
    times  508  db  0
	
MODEINFO:
    times  256  db  0

;
;-------------------------------------------------------------------
;

;
;funções VESA.
;
	
;
; 0x4112 is 640x480x24bit	(*inicialização)   (3 bytes por pixel) 
; 0x4115 is 800x600x24bit	(*gui)             (3 bytes por pixel) 
; 0x4118 is 1024x768x24bit   	
; 0x411B is 1280x1024x24bit	
; 0x4129 is 640x480x32bit
; 0x412E is 800x600x32bit
; 0x4138 is 1024x768x32bit 
; 0x413D is 1280x1024x32bit 	


;  1: 182h, 320x200x8
;  2: 10Dh, 320x200x15           29: 185h, 640x400x24
;  3: 10Eh, 320x200x16           30: 186h, 640x400x32
;  4: 10Fh, 320x200x24           31: 101h, 640x480x8
;  5: 120h, 320x200x32           32: 110h, 640x480x15
;  6: 192h, 320x240x8            33: 111h, 640x480x16
;  7: 193h, 320x240x15           34: 112h, 640x480x24
;  8: 194h, 320x240x16           35: 121h, 640x480x32
;  9: 195h, 320x240x24           36: 103h, 800x600x8
; 10: 196h, 320x240x32           37: 113h, 800x600x15
; 11: 1A2h, 400x300x8            38: 114h, 800x600x16
; 12: 1A3h, 400x300x15           39: 115h, 800x600x24 *
; 13: 1A4h, 400x300x16           40: 122h, 800x600x32
; 14: 1A5h, 400x300x24           41: 105h, 1024x768x8
; 15: 1A6h, 400x300x32           42: 116h, 1024x768x15
; 16: 1B2h, 512x384x8            43: 117h, 1024x768x16
; 17: 1B3h, 512x384x15           44: 118h, 1024x768x24
; 18: 1B4h, 512x384x16           45: 123h, 1024x768x32
; 19: 1B5h, 512x384x24           46: 107h, 1280x1024x8
; 20: 1B6h, 512x384x32           47: 119h, 1280x1024x15
; 21: 1C2h, 640x350x8            48: 11Ah, 1280x1024x16
; 22: 1C3h, 640x350x15           49: 11Bh, 1280x1024x24
; 23: 1C4h, 640x350x16           50: 124h, 1280x1024x32
; 24: 1C5h, 640x350x24           51: 140h, 1400x1050x8
; 25: 1C6h, 640x350x32           52: 141h, 1400x1050x15
; 26: 100h, 640x400x8            53: 142h, 1400x1050x16
; 27: 183h, 640x400x15           54: 143h, 1400x1050x24
; 28: 184h, 640x400x16           55: 144h, 1400x1050x32

; Video modes:
; ============
; VirtualBox:
; Oracle VirtualBox: 0x????   640x480x24bpp
; Oracle VirtualBox: 0x0115   800x600x24BPP*
; Oracle VirtualBox: 0x0118  1024x768x24BPP
;
; Nvidia GeForce:  
; GeForce_8400_GS equ 0x06E4  
; GeForce 8400 GS: 0x0115   800x600x32BPP


;
; Define a placa de video
;
;1 = oracle virtual machine.
;2 = nvidia geforce
;
my_video_card: dd 0
;CurrentVideoCard: dd 0

;-------------------------------------------------- 
; stage2_init_vesa:
;     Inicia o modo VESA via bios.
;     ; Vesa start code.
;
; 0x4115  ;800 x 600   (geforce=32bpp,oraclevm=24bpp) 
; 	;mov bx, 4112h    ;640x480x32bpp.
;
stage2_init_vesa:
    pusha
	
    mov bx, 4000h
	add bx, word [META$FILE.VIDEO_MODE]    ;Adiciona o número do modo.
	
	mov ax, 4f01h
    mov di, Mode_Info   
    mov cx, bx
    int 10h 
	
    mov ax, 4f02h
    int 10h
	
	popa	
	ret
	

;----------------------------------------
; search_vesa:
;     Procura por VESA.
;     Ver se a máquina suporta o padrão VESA.
;
search_vesa: 
	;Display Vesa version.
	mov ax, 0x4F00
    mov di, 0xA000
    int 0x10
    ;Se VESA é suportada.
	cmp ax, word 0x004F
    je .vesaOK
;NO VESA.	
.noVESA:	
    pusha 
	call Window.StatusBar   
	mov si, msg_no_vesa
    call PoeString 
	popa
	stc    ;Flag avisa que nao há vesa.	
	ret
;vesa OK.
.vesaOK:     
	;Prepara a string para apresentar o número da versão.
	mov ax, word [di+4]
    mov dx, ax
    add ax, word 48*256+48   
	mov byte [msg_ver_vesa+19], ah
    mov byte [msg_ver_vesa+21], al
	;Mostra número da versão.
	pusha
    call Window.StatusBar
    mov si, msg_ver_vesa
    call PoeString
    popa
	
	;;#bugbug
	;;Não tá dando pra usar recuperar depois esses valores
	;;quando estivermos em 32bit.
	
	;Salva parâmetros no META$FILE.
	xor ax, ax
	mov ax, word [ModeInfo_XResolution]
    mov word [META$FILE.SCREEN_X], ax
	mov word [g_x_resolution], ax
	
	xor ax, ax
	mov ax, word [ModeInfo_YResolution]
    mov word [META$FILE.SCREEN_Y], ax
	mov word [g_y_resolution], ax
	
	;;#bugbug Isso é 'byte'
	xor ax, ax
	mov ax, word [ModeInfo_BitsPerPixel]
    mov word [META$FILE.BPP], ax
	mov word [g_bpp], ax
	
.done:
	xor ax, ax
	clc    ;Limpa a flag.
	ret
	
	
;Pegar Bits Por Pixel.	
s2vesaGetBPP:
    mov al, byte [ModeInfo_BitsPerPixel]
	ret

;Pegar largura da tela em pixels.		
s2vesaGetWidth:
    mov ax, word [ModeInfo_XResolution] 
    ret
	
;Pegar altura da tela em pixels.	
s2vesaGetHeight:
    mov ax, word [ModeInfo_YResolution]
	ret
	
;Inicializa VESA.	
s2vesaInitVESA:
    call stage2_init_vesa
	ret

;Ver se VESA é suportada.	
s2vesaSearchVESA:
    call search_vesa
	ret


;
; Mensagens.
;

msg_no_machine db "s2vesa error: Machine",13,10,0		
msg_no_vesa    db "s2vesa No VESA",13,10,0
msg_ver_vesa   db "s2vesa Ver",13,10,0	
	
;
;fim.
;
