
; sysvar32.inc
; Memory map and system variables.
; Using physical addresses.
; 2015 - Created by Fred Nora.


align 2
    db 'DEBUG: SYSVAR   '
align 2


; bloco 0
mm_ivt:        equ  0x00000000    ;BIOS IVT.
mm_stackRM:    equ  0x00000E00    ;pilha em modo real. (isso é pra onde foi o mbr.).
mm_stage1:     equ  0x00000E00    ;stage 1 (mbr).

; bloco 1
mm_stage2:       equ  0x00001000    ;stage 2 (boot manager).
mm_meta$file:    equ  0x00001000    ;o inicio do stage 2 é um metafile.
mm_sys_var:      equ  0x00009000    ;variaveis de sistema.

; bloco 2
; bloco 3 - continuação do boot loader.
mm_bl:  equ  0x00020000    ;boot loader.

; bloco 4
; bloco 5 - continuação do root dir
mm_root:    equ  0x00040000    ;root dir.

; bloco 6
; bloco 7 - continuação da fat (talvez a fat use o bloco 8.)
mm_fat:    equ  0x00060000    ;fat.

; bloco 8
mm_mbr_buffer:    equ  0x00080000    ;mbr.
mm_vbr_buffer:    equ  0x00087c00    ;vbr.

; bloco 9 - variaveis diversas e pilha.
mm_variaveis:          equ 0x00090000  ; muitas variaveis daqui pra frente.
ahci_cmdlist:          equ 0x00090000  ; 4096 bytes 0x070000 -> 0x071FFF
ahci_receivedfis:      equ 0x00091000  ; 4096 bytes 0x071000 -> 0x072FFF
ahci_cmdtable:         equ 0x00092000  ; 57344 bytes 0x072000 -> 0x07FFFF
cli_temp_string:       equ 0x00093000  ; 1024 bytes 0x080000 -> 0x0803FF
os_temp_string:        equ 0x00093400  ; 1024 bytes 0x080400 -> 0x0807FF
os_args:               equ 0x00093C00
bmfs_directory:        equ 0x00094000  ; 4096 bytes 0x090000 -> 0x090FFF
os_filehandlers:       equ 0x00095000  ; 64 bytes (1x64)
os_filehandlers_seek:  equ 0x00096000  ; 512 bytes (8x64)
mm_stack:              equ 0x0009fff0  ; inicio da pilha (pra trás)

; bloco a
mm_graphic_video:  equ 0x000A0000

; bloco b
mm_text_video:  equ 0x000B8000

; bloco c - bios stuffs 
; bloco d - bios stuffs 
; bloco e - bios stuffs 
; bloco f - bios stuffs 


;
; -- 1 MB mark --------
;

; #bugbug
; The BM.BIN knows nothing about the kernel.
; It's job is loading the BL.BIN.
; os_KernelStart:       equ  0x00100000  ;Base do kernel. 
; os_KernelEntryPoint:  equ  0x00101000  ;Entry point do kernel. 

;
; -- 2 MB mark --------
;

; Location in memory where 
; programs are loaded (the start of 2MiB).
; #bugbug:
; The BM.BIN knows nothing about programs.
; It's job is loading the BL.BIN.
; programlocation: equ  0x00200000

; 3 MB
; 4 MB
; 5 MB
; 6 MB
; 7 MB
; 8 MB

;
; -- 9 MB mark --------
;

os_stuffs:              equ 0x00900000
os_SystemVariables:     equ 0x00910000  ; 65536 bytes  0x110000 -> 0x11FFFF - Location of System Variables
os_MemoryMap:           equ 0x00920000  ; 131072 bytes 0x120000 -> 0x13FFFF - Location of Memory Map - Room to map 256 GiB with 2 MiB pages
os_EthernetBuffer:      equ 0x00940000  ; 262144 bytes 0x140000 -> 0x17FFFF - Location of Ethernet RX Ring Buffer - Room for 170 packets
os_screen:              equ 0x00980000  ; 4096 bytes   80x25x2 = 4000
os_temp:                equ 0x00990000
os_ethernet_rx_buffer:  equ 0x009C0000
os_eth_rx_buffer:       equ 0x009C8000
os_ethernet_tx_buffer:  equ 0x009D0000
os_eth_tx_buffer:       equ 0x009D8000
cpustatus:              equ 0x009FEF00  ; Location of CPU status data (256 bytes) Bit 0 = Avaiable, Bit 1 = Free/Busy
cpuqueue:               equ 0x009FF000  ; Location of CPU Queue. Each queue item is 16 bytes. (4KiB before the 2MiB mark, Room for 256 entries)


;--------------------------------------------------------------------------
; System variables:
; DQ - Starting at offset 0, increments by 0x8

os_LocalAPICAddress:    equ os_SystemVariables + 0x00
os_IOAPICAddress:       equ os_SystemVariables + 0x08
os_ClockCounter:        equ os_SystemVariables + 0x10
os_VideoBase:           equ os_SystemVariables + 0x18
screen_cursor_offset:   equ os_SystemVariables + 0x20
os_StackBase:           equ os_SystemVariables + 0x28
os_net_transmit:        equ os_SystemVariables + 0x30
os_net_poll:            equ os_SystemVariables + 0x38
os_net_ack_int:         equ os_SystemVariables + 0x40
os_NetIOBaseMem:        equ os_SystemVariables + 0x48
os_NetMAC:              equ os_SystemVariables + 0x50
os_HPETAddress:         equ os_SystemVariables + 0x58
ahci_base:              equ os_SystemVariables + 0x60
os_NetworkCallback:     equ os_SystemVariables + 0x68
bmfs_TotalBlocks:       equ os_SystemVariables + 0x70

; DD - Starting at offset 128, increments by 4.

cpu_speed:              equ os_SystemVariables + 128  ; in MHz
ip:                     equ os_SystemVariables + 132  ; IPv4 Address
sn:                     equ os_SystemVariables + 136  ; IPv4 Subnet
gw:                     equ os_SystemVariables + 140  ; IPv4 Gateway
os_HPETRate:            equ os_SystemVariables + 144
os_MemAmount:           equ os_SystemVariables + 148  ; in MiB
ahci_port:              equ os_SystemVariables + 152
hd1_size:               equ os_SystemVariables + 156  ; in MiB
os_Screen_Pixels:       equ os_SystemVariables + 160
os_Screen_Bytes:        equ os_SystemVariables + 164
os_Screen_Row_2:        equ os_SystemVariables + 168
os_Font_Color:          equ os_SystemVariables + 172

; DW - Starting at offset 256, increments by 2

os_NumCores:           equ os_SystemVariables + 258
cpuqueuestart:         equ os_SystemVariables + 260
cpuqueuefinish:        equ os_SystemVariables + 262
os_QueueLen:           equ os_SystemVariables + 264
os_QueueLock:          equ os_SystemVariables + 266  ; Bit 0 clear for unlocked, set for locked.
os_NetIOAddress:       equ os_SystemVariables + 268
os_EthernetBusyLock:   equ os_SystemVariables + 270
os_VideoX:             equ os_SystemVariables + 272
os_VideoY:             equ os_SystemVariables + 274
os_Screen_Rows:        equ os_SystemVariables + 276
os_Screen_Cols:        equ os_SystemVariables + 278
os_Screen_Cursor_Row:  equ os_SystemVariables + 280
os_Screen_Cursor_Col:  equ os_SystemVariables + 282

; DB - Starting at offset 384, increments by 1.

cursorx:               equ os_SystemVariables + 384  ; cursor row location
cursory:               equ os_SystemVariables + 385  ; cursor column location
scancode:              equ os_SystemVariables + 386

; Buffer for next key.
; see: keyboard.inc
key:  equ  os_SystemVariables + 387

key_shift:             equ os_SystemVariables + 388
screen_cursor_x:       equ os_SystemVariables + 389
screen_cursor_y:       equ os_SystemVariables + 390
os_PCIEnabled:         equ os_SystemVariables + 391  ; 1 if PCI is detected
os_NetEnabled:         equ os_SystemVariables + 392  ; 1 if a supported network card was enabled
os_NetIRQ:             equ os_SystemVariables + 393  ; Set to Interrupt line that NIC is connected to
os_NetActivity_TX:     equ os_SystemVariables + 394
os_NetActivity_RX:     equ os_SystemVariables + 395
os_EthernetBuffer_C1:  equ os_SystemVariables + 396  ; Counter 1 for the Ethernet RX Ring Buffer
os_EthernetBuffer_C2:  equ os_SystemVariables + 397  ; Counter 2 for the Ethernet RX Ring Buffer
os_DiskEnabled:        equ os_SystemVariables + 398
os_DiskActivity:       equ os_SystemVariables + 399
app_argc:              equ os_SystemVariables + 400
os_VideoDepth:         equ os_SystemVariables + 401
os_VideoEnabled:       equ os_SystemVariables + 402

;--------------------------------------------------------------------------

;
; Variables:
;

cpuqueuemax:  dw  256
screen_cols:  db  80    ;largura. 
screen_rows:  db  25    ;altura. (25 linhas incluindo a linha 0.)
os_show_sysstatus:  db  1

; Function variables
os_debug_dump_reg_stage:  db 0x00

;
; -- Structures --------
;

; File System
; Define the structure of a directory entry.
; see: Pure64.
struc  BMFS_DirEnt
    .filename    resb 32
    .start       resd 1    ; starting block index
    .reserved    resd 1    ; number of blocks reserved
    .size        resd 1    ; number of bytes
    .unused      resd 1
endstruc

; stack frame for 32bit.
struc  stack_frame_t
    .ss     resd 1
    .esp    resd 1
    .eflags resd 1
    .cs     resd 1
    .eip    resd 1
endstruc


; palette.
; These colors are in RGB format. 
; Each color byte is actually 6 bits (0x00 - 0x3F)
palette:
    db 0x00, 0x00, 0x00  ;  0 Black
    db 0x33, 0x00, 0x00  ;  1 Red
    db 0x0F, 0x26, 0x01  ;  2 Green
    db 0x0D, 0x19, 0x29  ;  3 Blue
    db 0x31, 0x28, 0x00  ;  4 Orange
    db 0x1D, 0x14, 0x1E  ;  5 Purple
    db 0x01, 0x26, 0x26  ;  6 Teal
    db 0x2A, 0x2A, 0x2A  ;  7 Light Gray
    db 0x15, 0x15, 0x15  ;  8 Dark Gray
    db 0x3B, 0x0A, 0x0A  ;  9 Bright Red
    db 0x22, 0x38, 0x0D  ; 10 Bright Green
    db 0x1C, 0x27, 0x33  ; 11 Bright Blue
    db 0x3F, 0x3A, 0x13  ; 12 Yellow
    db 0x2B, 0x1F, 0x2A  ; 13 Bright Purple
    db 0x0D, 0x38, 0x38  ; 14 Bright Teal
    db 0x3F, 0x3F, 0x3F  ; 15 White

;
; -- Strings --------
;

os_debug_dump_reg_string00:	db ' EAX:', 0
os_debug_dump_reg_string01:	db ' EBX:', 0
os_debug_dump_reg_string02:	db ' ECX:', 0
os_debug_dump_reg_string03:	db ' EDX:', 0
os_debug_dump_reg_string04:	db ' ESI:', 0
os_debug_dump_reg_string05:	db ' EDI:', 0
os_debug_dump_reg_string06:	db ' EBP:', 0
os_debug_dump_reg_string07:	db ' ESP:', 0

os_debug_dump_reg_string08:	db '  8:', 0
os_debug_dump_reg_string09:	db '  9:', 0
os_debug_dump_reg_string0A:	db ' 10:', 0
os_debug_dump_reg_string0B:	db ' 11:', 0
os_debug_dump_reg_string0C:	db ' 12:', 0
os_debug_dump_reg_string0D:	db ' 13:', 0
os_debug_dump_reg_string0E:	db ' 14:', 0
os_debug_dump_reg_string0F:	db ' 15:', 0
os_debug_dump_reg_string10:	db ' RF:', 0

os_debug_dump_flag_string0:	db ' C:', 0
os_debug_dump_flag_string1:	db ' Z:', 0
os_debug_dump_flag_string2:	db ' S:', 0
os_debug_dump_flag_string3:	db ' D:', 0
os_debug_dump_flag_string4:	db ' O:', 0


;
; -- Strings --------
;



;++
; The taskbar.
cpumsg:           db '[cpu: ', 0
memmsg:           db ']  [mem: ', 0
networkmsg:       db ']  [net: ', 0
diskmsg:          db ']  [hdd: ', 0
mibmsg:           db ' MiB', 0
mhzmsg:           db ' MHz', 0
coresmsg:         db ' x ', 0
namsg:            db 'N/A', 0
closebracketmsg:  db ']', 0
;--

;
; -- Strings --------
;

space:     db ' ', 0
newline:   db 13, 0
new_line:  db 13, 10, 0
tab:       db 9, 0
system_status_header:  db '[BM:] Boot manager v1.0', 0
readymsg:              db '[BM:] Boot is ready', 0
memory_message:        db '[BM:] Not enough system memory for CPU stacks! System halted.', 0
startupapp:            db 'startup.app', 0

; Constants
hextable:  db '0123456789ABCDEF'

; Buffer: String para comandos.
; #bugbug: We simply can't have a bigger buffer.
; Why?
cli_command_string:
    times 14 db 0   ;ok (This is the limit size for one single word).
    ;times 32 db 0   ;ok
    ;times 64 db 0   ;ok
    ;times 128 db 0  ;fail. Why?

align 4
this_is_the_end:  db '*'

;
; End
;
