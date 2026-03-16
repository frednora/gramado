;
; File: header32.inc 
;
; Descrição:
;     Header principal do Boot Manager.
;     Header para a parte de 32bit.
;
; Importante:
; O arquivo main.asm faz uma sequencia de inclusões de módulos
; que compõem o Boot manager, começando pelos módulos de 16bit
; e depois os módulos de 32bit. O arquivo header32.inc deve ficar no 
; início das inclusões dos módulos de 32bit, para que os módulos
; de 32bit possam utiliza-lo adequadamente.
;
; Sobre os arquivos system.cnf e init.cnf:
;     Os arquivos system.cnf e init.cnf são carregados em 0x00009000.
;     Variáveis dentro do arquivo system.cnf.
;     Address = 0x00009000, logo apos o bootmanager ( 0xe00 ~ 0x8000 ) 31,5 kb.
;
; 2015 - Created by Fred Nora.
;


;
; Defines.
;


;Definições usadas apenas em 32bit.
%DEFINE BOOT_VER          '0.1'
%DEFINE BOOT_MANAGER_VER  '0.1'
%DEFINE SHELL_VER         '0.1'
; ...


;
; Igualdades.
;

;
;Outros.
;

;
;gdt stuffs.
;
sys_code        equ  8 
TSS0_SEL        equ  0x20
LDT0_SEL        equ  0x28
TSS1_SEL        equ  0x30
LDT1_SEL        equ  0x38
SYS64_CODE_SEL  equ  8


;
;idt stuffs.
;

;usanda nas entradas da idt.
sys_interrupt  equ  0x8E  


;
;Address. (memory map for files).
;

;arquivos principais:
SYSTEM_CNF       EQU 0x00008C00    ;@todo: Rever.
INIT_CNF         EQU 0x00008E00    ;@todo: Rever.
BL_BIN           EQU 0x00020000    ;Boot Loader base.
BL_BIN_ENTRY     EQU 0x00021000    ;Boot Loader entry point.
KRN_BASE         EQU 0x00100000    ;Kernel base.
BOOT_DIR         EQU 0x00200000    ;Boot dir.  
SYSTEM_DIR       EQU 0x00300000    ;System dir.
USER_DIR         EQU 0x00400000    ;User dir.    


;; LBA
Boot_Fat  equ  67   ; FAT.
Root_Dir  equ  559  ; Diretório raiz.
;; ...




;
; Variáveis.
;

;reinicia algum modo de 32 bits.
restart32_status: 
    dd 0

;current.
current:  
  dd 0

;deslocamento dentro da memória de video 
scr_loc:    
  dd 0

;Cursor.
g_cursor_x:  dd 0
g_cursor_y:  dd 0



;
; File System support.
;

;Endreço do início da entrada do volume atual.
g_current_volume_entry_address    dd 0    
g_current_fat_address        dd 0    ;Endereço do início da fat atual.
g_current_fat_lba            dd 0    
g_current_directory_address  dd 0    ;Endereço do início do diretório atual.
g_current_directory_lba      dd 0    
g_current_dataarea_address   dd 0    ;Endereço do início da area de dados atual.
g_current_dataarea_lba       dd 0

;SYSTEM  EQU 0x00009000  
g_system_system_name        db 'GRAMADO OS ' ,0 
g_system_BytesPerSector     dw 0    ;.BytesPerSector:     dw 0x0200          
g_system_SectorsPerCluster  db 0   	;.SectorsPerCluster:  db 0x01           
g_system_ReservedSectors    dw 0    ;.ReservedSectors:    dw 0x0001           
g_system_TotalFATs          db 0    ;.TotalFATs:          db 0x02           
g_system_MaxRootEntries     dw 0   	;.MaxRootEntries:     dw 0x0200         
g_system_TotalSectorsSmall  dw 0    ;.TotalSectorsSmall:  dw 0x0000          
g_system_MediaDescriptor    db 0    ;.MediaDescriptor:    db 0xF0           
g_system_SectorsPerFAT      dw 0    ;.SectorsPerFAT:      dw 127                
g_system_SectorsPerTrack    dw 0    ;.SectorsPerTrack:    dw 0x0000         
g_system_NumHeads           dw 0    ;.NumHeads:           dw 0x0000             
g_system_HiddenSectors      dd 0    ;.HiddenSectors:      dd 0x00000000     
g_system_TotalSectorsLarge  dd 0    ;.TotalSectorsLarge:  dd 0x00000000      
;For extended.
g_system_DriveNumber        db 0    ;.DriveNumber:           db 080h           
g_system_Flags              db 0	;.Flags:                 db 0x00           
g_system_Signature          db 0	;.Signature:             db 0              
g_system_VolumeID           dd 0xFFFFFFFF 	    ;.VolumeID:    dd 0xFFFFFFFF     
g_system_VolumeLabel        db "FREDNORA123"    ;.VolumeLabel: db "FREDNORA123"  
g_system_SystemID           db "FAT16   "	 	;.SystemID:    db "FAT16   "	   
;Continua...


;
; Variáveis dentro do arquivo init.cnf.
;

;---------------------------------------
; Observação: (#IMPORTANTE!!!)
;     O Boot Loader apaga essas informações ao carregar    
;     o Kernel nesse endereço físico.
;
;     @todo: 
;         Se essas informações são configurações do
;         sistema, é preciso encontrar um novo endereço pra elas.
;         A não ser que esse metafile seja salvo em disco. Ai
;         ele poderá ser aberto aqualquer hora, com endereço
;         alocado dinamicamente.
;
;

;INIT  EQU 0x00100000    
;principais	
;BOOT:
g_init_modo            dd 0  ;.modo: dd 0  ;+0   modo de inicializção do boot loader.
g_init_disco_atual     dd 0  ;.res2: dd 0  ;+4   numero do disco atual.
g_init_particao_atual  dd 0  ;.res3: dd 0  ;+8   numero da partição atual.
g_init_bl_cluster      dd 0  ;.res4: dd 0  ;+12  bl.bin cluster inicial.
g_init_bl_address      dd 0  ;.res5: dd 0  ;+16  bl.bin address.
g_init_bl_entry_point  dd 0  ;.res6: dd 0  ;+20  bl.bin entrypoint.
g_init_status_bm16     dd 0  ;.res7: dd 0  ;+24 status do bm de 16 bits.
g_init_status_bl32     dd 0  ;.res8: dd 0  ;+28 status do bm de 32 bits.

;Files.
;SYSTEMCNF:
g_init_systemcnf_address  dd 0  ;.address dd 0x00100000   
g_init_systemcnf_cluster  dd 0  ;.lba     dd 0           
;INITCNF:
g_init_initcnf_address    dd 0  ;.address dd 0x00100000  
g_init_initcnf_cluster    dd 0  ;.lba    dd 1            
;BLBIN:
g_init_blbin_address      dd 0  ;.address dd 0x00200000  
g_init_blbin_cluster      dd 0  ;.lba    dd 2            
;BOOTDIR:
g_init_bootdir_address    dd 0  ;.address dd 0x00300000  
g_init_bootdir_cluster    dd 0  ;.lba    dd 3            
;SYSTEMDIR:
g_init_systemdir_address  dd 0  ;.address dd 0x00400000  
g_init_systemdir_cluster  dd 0  ;.lba    dd 4                  
;USERDIR:
g_init_userdir_address    dd 0  ;.address dd 0x00500000  
g_init_userdir_cluster    dd 0  ;.lba    dd 5            
;Continua...


;; =========================
;; video mode support

g_x_resolution dw 0
g_y_resolution dw 0
g_bpp          db 0



;;
;; End.
;;


